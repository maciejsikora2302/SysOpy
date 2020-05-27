#include "common.h"

#define MODE_NET "net"
#define MODE_LOCAL "local"

int sockfd = 0;
char buffer[MESSAGE_BUFFER_LEN]; 
char other_players_names[MAX_CLIENT_NAME];
char mark;

char* random_path = NULL;

struct sockaddr_un addr;

void cleanup() {
    printf("Client: I'm finishing playing...\n");
    printf("-------------------------------------------------------------\n");

    if(random_path != NULL){
        unlink(random_path);
    }

    close(sockfd);
}

void sighandler() {
    //Theoreticly there shouldnt be printf here but since program finishes job after receiving signal I think it's fine to leave it here with this log.
    printf("Client: SIGINT received.");
    exit(EXIT_SUCCESS);
}

void net_socket(char* ipv4, int port) {
    struct sockaddr_in serv_addr; 

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        printf("Client: Failed to create net socket");
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if (inet_pton(AF_INET, ipv4, &serv_addr.sin_addr) <= 0)  
        printf("Client: Invalid/not supported adress");
   
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) 
        printf("Client: Connection failed");
}

void unix_socet(char* path) {
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);


    random_path = random_path_generator(12);


    struct sockaddr_un client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, random_path, 108);
    bind(sockfd, (struct sockaddr *) &client_addr, sizeof(client_addr));

    int opt = 1;  
    setsockopt(sockfd, SOL_SOCKET, SO_PASSCRED, (char *)&opt, sizeof(opt));


    if (sockfd < 0)
        printf("Client: Failed to create unix socket");
    
    if (connect(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0) 
        printf("Client: Connection failed");
} 

int main(int argc, char* argv []) {
    srand(time(NULL));
    if (argc < 3){
        printf("Client: Not enough arguments, should be at lest 3 but got %d", argc);
        exit(EXIT_FAILURE);
    }
        

    char* name = argv[1];
    char* mode = argv[2];

    // create socet depending on the mode
    if (str_eq(mode, MODE_NET)) {
        if (argc < 5)
            printf("Client: Not enough arguments. NET mode specified, should be 5, got %d", argc);

        char* ipv4 = argv[3];
        int   port = atoi(argv[4]);
        net_socket(ipv4, port);
    } else if (str_eq(mode, MODE_LOCAL)) {
        if (argc < 4)
            printf("Client: Not enough arguments. UNIX mode specified, should be 4, got %d", argc);
        
        char* path = argv[3];
        unix_socet(path);
    } else 
        printf("Client: Unknown option");


    //exists handlers
    atexit(cleanup);
    signal(SIGINT, sighandler);

    // once connected handle game logic

    //send my name first
    name_msg(buffer, name);
    if (send(sockfd, buffer, strlen(buffer), MSG_CONFIRM) <= 0){
        printf("Client: Failed to send name msg");
        exit(EXIT_FAILURE);
    }
    


    //set of file descriptors containing standard input and socket fd
    fd_set s_rd;
    int stdinfd = fileno(stdin);
    char header [64];
    Board* board;
    while (1) {
        FD_ZERO(&s_rd);
        FD_SET(stdinfd, &s_rd);
        FD_SET(sockfd, &s_rd);
        int max = (stdinfd > sockfd) > stdinfd ? stdinfd : sockfd;
        select(max + 1, &s_rd, NULL, NULL, NULL);

        // check for server updates
        if (FD_ISSET(sockfd, &s_rd)) {
            int bytesRead = recv(sockfd, buffer, MESSAGE_BUFFER_LEN, MSG_WAITALL);
            buffer[bytesRead] = '\0';
            get_header(buffer, header);

            // game logic
            if (str_eq(header, MESSAGE_MOVE)) {
                int mv;
                parse_move(buffer, &mv);
                move(board, mv);

                int gameov = game_state(board);
                if (gameov == 1) {
                    sprintf(buffer, "%s won!", other_players_names);
                    show(board, buffer);
                    exit(EXIT_SUCCESS);
                } else if (gameov == -1) {
                    show(board, "Tie!");
                    exit(EXIT_SUCCESS);
                } else {
                    show(board, "Your turn, type digit [0-8]");
                }

            } else if (str_eq(header, MESSAGE_WAIT)) {
                printf("Client: Waiting for another player to join game\n");
            } else if (str_eq(header, MESSAGE_NOT_UNIQUE)) {
                printf("Client: Your name is not unique!\n");
                exit(EXIT_SUCCESS);
            } else if (str_eq(header, MESSAGE_PLAY)) {
                parse_play_msg(buffer, other_players_names, &mark);
                printf("\n============================= Game =============================\n");
                printf("\t %s (%c) \t vs \t %s (%c) \n",
                       name, mark, other_players_names, opposite(mark));
                printf("==========================================================\n");

                board = new_board();
                if (mark == MV_CHR_CIRCLE) {
                    printf("Your turn, type digit [0-8]\n");
                } else {
                    printf("%s's turn\n", other_players_names);
                }
                print_board(board);
            } else if (str_eq(header, MESSAGE_PING)) {
                // ping back
                notif_msg(buffer, MESSAGE_PING_BACK);
                send(sockfd, buffer, strlen(buffer), 0);
            } else if (str_eq(header, MESSAGE_SERVER_SHUTDOWN) || bytesRead == 0) {
                printf("Client: Server shut down or connection timed out.. \n");
                exit(EXIT_SUCCESS);
            } else if (str_eq(header, MESSAGE_OP_QUIT)) {
                printf("Client: Opponent quit game.. \n");
                exit(EXIT_SUCCESS);
            }
        }

        // check for input from stdin
        int mv;
        if (FD_ISSET(stdinfd, &s_rd)) {
            read(stdinfd, buffer, MESSAGE_BUFFER_LEN);
            // validate user input
            if (board == NULL) {
                clearline();
            } else if (mark != board -> next_move) {
                clearline();
                show(board, "Wait for your turn!");
            } else {
                clearline();
                int x = sscanf(buffer, "%d", &mv);
                if (x <= 0) {
                    show(board, "Enter valid int between [0-8], try again..");
                } else {
                    if (valid_move(board, buffer, mv) == 0) {
                        move(board, mv);
                        move_msg(buffer, mv);
                        send(sockfd, buffer, strlen(buffer), 0);

                        sprintf(buffer, "%s's turn", other_players_names);
                        show(board, buffer);

                        int gameov = game_state(board);
                        if (gameov == 1) {
                            show(board, "You won!");
                            exit(EXIT_SUCCESS);
                        } else if (gameov == -1) {
                            show(board, "Tie!");
                            exit(EXIT_SUCCESS);
                        }
                    } else {
                        show(board, "Enter valid int between [0-8], try again..");
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}