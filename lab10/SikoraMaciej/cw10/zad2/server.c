#include "common.h"

// ping every second
int ping_interval = 1;
int net_socket_fd, unix_socket_fd;
char* path;
Player** players;

// server address
struct sockaddr_in address;
struct sockaddr_un addr;

// client addresses
struct sockaddr_in net_addr_c  [MAX_CLIENTS];
struct sockaddr_un unix_addr_c [MAX_CLIENTS];
int occupied [MAX_CLIENTS]; // 0 free, 1 net, 2 unix

pthread_mutex_t mutex;
char buffer [MESSAGE_BUFFER_LEN];

// sends message stored in buffer to ith client
void send_buffer_to(int i) {
    socklen_t len;
    if (occupied[i] == 1) {
        len = sizeof(net_addr_c[i]);
        sendto(net_socket_fd, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr*) &net_addr_c[i], len);
    } else {
        len = sizeof(unix_addr_c[i]);
        sendto(unix_socket_fd, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr*) &unix_addr_c[i], len);
    }
}


// check for net addr equality
bool net_addr_eq(struct sockaddr_in a, struct sockaddr_in b) {
    return a.sin_port == b.sin_port;
}

// check for unix addr equality
bool unix_addr_eq(struct sockaddr_un a, struct sockaddr_un b) {
    return str_eq(a.sun_path, b.sun_path); 
}


int net_addr_find(struct sockaddr_in addr) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (occupied[i] && net_addr_eq(addr, net_addr_c[i]))
            return i;
    }

    return -1;
}

int unix_addr_find(struct sockaddr_un addr) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (occupied[i] && unix_addr_eq(addr, unix_addr_c[i]))
            return i;
    }

    return -1;
}

int empty_find() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!occupied[i])
            return i;
    }

    printf("Server: Maximum capacity reached\n");
    return -1;
}




void disconnect(int i) {
    printf("Server: Client %s(%d) disconnected\n", players[i] -> name, i);
        
    if (players[i] -> in_game) {
        int opponent = players[i] -> other;
        // check if opponent hasn't already left
        if (occupied[opponent] > 0) {
            notif_msg(buffer, MESSAGE_OP_QUIT);
            send_buffer_to(opponent);
        }
    }

    // Close the socket and mark for reuse 
    occupied[i] = 0;
    free(players[i] -> name);

    players[i] -> name = NULL;
    players[i] -> in_game = false;
    players[i] -> other = -1;
    players[i] -> was_pinged = false;
    players[i] -> pinged_back = false;
}

void* ping_thr(void* arg) {
    while (1) {
        printf("Ping Thread: Sending pings...\n");
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (occupied[i] > 0 ) {
                Player* player = players[i];
                // check if client responded to ping
                if (player -> was_pinged && !player -> pinged_back) {
                    printf("Ping Thread: Client %s(%d) timed out\n", players[i] -> name, i);
                    disconnect(i);
                } else {
                    notif_msg(buffer, MESSAGE_PING);
                    send_buffer_to(i);
                    player -> was_pinged = true;
                    player -> pinged_back = false;
                }
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep(ping_interval);
    }

    return NULL;
}

void sighandler() {
    printf("Server: Received SIGINT\n");
    exit(EXIT_SUCCESS);
}

void cleanup() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if(occupied[i] > 0){
            notif_msg(buffer, MESSAGE_SERVER_SHUTDOWN);
            send_buffer_to(i);
        }
        free(players[i]);
    }

    unlink(path);

    free(players);
    close(net_socket_fd);
    close(unix_socket_fd);

    printf("\nServer: Shutdown -> closed all connections\n");
}

int find_opponent(int j) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (occupied[i] > 0 && i != j && players[i] -> name && !players[i] -> in_game)
            return i;
    }

    return -1;
}

bool unique(char* name) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (players[i] -> name && str_eq(players[i] -> name, name))
            return false;
    }

    return true;
}


int main(int argc, char* argv[]) {
    if (argc < 3) 
        printf("Server: Not enough arguments! Should be at least 3.\n");

    players = malloc(MAX_CLIENTS * sizeof(Player*));
    for(int i = 0; i < MAX_CLIENTS; i++) {
        players[i] = malloc(sizeof(Player));
        players[i] -> name = NULL;
        players[i] -> in_game = false;
        players[i] -> was_pinged = false;
        players[i] -> pinged_back = false;
        occupied[i] = 0;
        memset(&net_addr_c, 0, sizeof(net_addr_c));
        memset(&unix_socket_fd, 0, sizeof(unix_addr_c));
    }

    atexit(cleanup);
    signal(SIGINT, sighandler);

    // set of socket descriptors
    fd_set readfds;
    int port = atoi(argv[1]);
    path = argv[2];

    if (port < 1024) {
        printf("Port number must be greater or equal 1024\n");
        exit(EXIT_FAILURE);
    }
        

    // address for unix socket
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    // address for net socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // bind net socket
    net_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    // This is to handle situation when client does not close his socket after server shut down 
    int opt = 1;  
    setsockopt(net_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt));

    if (net_socket_fd == -1){
        printf("Server: Failed to create socket\n");
        exit(EXIT_FAILURE);
    }
        

    if ((bind(net_socket_fd, (struct sockaddr*) &address, sizeof(address))) != 0){
        printf("Server: Binding socket failed\n");
        exit(EXIT_FAILURE);
    }
        
    
    printf("Server: INET socket started on ipv4: %s (localhost), port: %d\n",
            "127.0.0.1\n", ntohs(address.sin_port));

    // bind unix socket
    unix_socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (unix_socket_fd == -1) {
        printf("Server: Failed to create unix socket\n");
        exit(EXIT_FAILURE);
    }
        

    if ((bind(unix_socket_fd, (struct sockaddr*) &addr, sizeof(addr))) != 0){
        printf("Server: Binding unix socket failed\n");
        exit(EXIT_FAILURE);
    }

        

    printf("Server: UNIX socket, receiving on path: %s \n", path);
    printf("Server: Waiting for connections\n");    

    // start pinging thread
    pthread_t thr;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&thr, NULL, ping_thr, NULL);

    // Actual server logic
    int max_sd;
    char header [64];


    struct sockaddr_in cli_in;
    struct sockaddr_un cli_un;
    socklen_t addr_len;


    while (1) {
        // Initialize set 
        max_sd = (net_socket_fd > unix_socket_fd) ? net_socket_fd : unix_socket_fd;

        FD_ZERO(&readfds);
        FD_SET(net_socket_fd, &readfds);
        FD_SET(unix_socket_fd, &readfds);

        // Wait for any fd in set to become ready && lock sockets
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL); 
        pthread_mutex_lock(&mutex);
		if (activity < 0 && errno != EINTR) 		 
            printf("Server: Select printf occured\n");

        // Check if there's connection on net socket
        if (FD_ISSET(net_socket_fd, &readfds)) {
            recvfrom(net_socket_fd, buffer, sizeof(buffer), MSG_WAITALL, 
                         (struct sockaddr*) &cli_in, &addr_len);

            int i = net_addr_find(cli_in);
            get_header(buffer, header);
            // add new socket/player, we know this is name message
            if (i == -1 && str_eq(header, MESSAGE_NAME)) {
                i = empty_find();
                net_addr_c[i].sin_addr.s_addr = cli_in.sin_addr.s_addr;
                net_addr_c[i].sin_family = cli_in.sin_family;
                net_addr_c[i].sin_port = cli_in.sin_port;
                strcpy((char*) net_addr_c[i].sin_zero, (char*) cli_in.sin_zero);

                char* name = malloc(sizeof(MAX_CLIENT_NAME));
                parse_name_msg(buffer, name);

                players[i] -> in_game = false;
                players[i] -> pinged_back = false;
                players[i] -> was_pinged = false;
                players[i] -> other = -1; 
                occupied[i] = 1; 
                // handle non unique name
                if (!unique(name)) {
                    printf("Server: Not unique name! %s\n", name);
                    notif_msg(buffer, MESSAGE_NOT_UNIQUE);
                    send_buffer_to(i);
                    disconnect(i);
                } else {
                    players[i] -> name = name;

                    int opponent = find_opponent(i);
                    if (opponent == -1) {
                        // send wait message
                        notif_msg(buffer, MESSAGE_WAIT);
                        send_buffer_to(i);
                        printf("Server: Sent wait message to player %s(%d)\n", name, i);
                    } else {
                        // start game 
                        char mark = cross_circle();
                        char opposite_mark = opposite(mark);

                        players[i] -> other = opponent;                 
                        players[opponent] -> other = i;

                        players[i] -> in_game = true;                 
                        players[opponent] -> in_game = true;

                        printf("Server: Starting game between %s(%d) and %s(%d)\n",
                            players[i] -> name, i, players[opponent] -> name, opponent);

                        // send players play message 
                        play_msg(buffer, players[opponent] -> name, mark);
                        send_buffer_to(i);

                        play_msg(buffer, players[i] -> name, opposite_mark);
                        send_buffer_to(opponent);
                    }
                } 
            // handle other messages
            } else if (str_eq(header, MESSAGE_MOVE)) {
                // forward message
                int opponent = players[i] -> other;
                send_buffer_to(opponent);
            } else if(str_eq(header, MESSAGE_PING_BACK)) {
                players[i] -> pinged_back = true;
            } else {
                printf("Server: Unknown message %s \n", buffer);
            }
        }

        // Else check if there's connection on unix socket
        if (FD_ISSET(unix_socket_fd, &readfds)) {
            recvfrom(unix_socket_fd, buffer, sizeof(buffer), 0, 
                         (struct sockaddr*) &cli_un, &addr_len);

            int i = unix_addr_find(cli_un);
            get_header(buffer, header);
            // add new socket/player, we know this is name message
            if (i == -1 && str_eq(header, MESSAGE_NAME)) {
                i = empty_find();
                unix_addr_c[i].sun_family = cli_un.sun_family;
                strcpy(unix_addr_c[i].sun_path, cli_un.sun_path);

                char* name = malloc(sizeof(MAX_CLIENT_NAME));
                parse_name_msg(buffer, name);

                players[i] -> in_game = false;
                players[i] -> pinged_back = false;
                players[i] -> was_pinged = false;
                players[i] -> other = -1; 
                occupied[i] = 2; 
                // handle non unique name
                if (!unique(name)) {
                    notif_msg(buffer, MESSAGE_NOT_UNIQUE);
                    send_buffer_to(i);
                    disconnect(i);
                } else {
                    players[i] -> name = name;

                    int opponent = find_opponent(i);
                    if (opponent == -1) {
                        // send wait message
                        notif_msg(buffer, MESSAGE_WAIT);
                        send_buffer_to(i);
                        printf("Server: Sent wait message to player %s(%d)\n", name, i);
                    } else {
                        // start game 
                        char mark = cross_circle();
                        char opposite_mark = opposite(mark);

                        players[i] -> other = opponent;                 
                        players[opponent] -> other = i;

                        players[i] -> in_game = true;                 
                        players[opponent] -> in_game = true;

                        printf("Server: Starting game between %s(%d) and %s(%d)\n",
                            players[i] -> name, i, players[opponent] -> name, opponent);

                        // send players play message 
                        play_msg(buffer, players[opponent] -> name, mark);
                        send_buffer_to(i);

                        play_msg(buffer, players[i] -> name, opposite_mark);
                        send_buffer_to(opponent);
                    }
                } 
            // handle other messages
            } else if (str_eq(header, MESSAGE_MOVE)) {
                // forward message
                int opponent = players[i] -> other;
                send_buffer_to(opponent);
            } else if(str_eq(header, MESSAGE_PING_BACK)) {
                players[i] -> pinged_back = true;
            } else if (str_eq(header, MESSAGE_QUIT)) {
                disconnect(i);
            } else {
                printf("Server: Unknown message %s \n", buffer);
            }
        }
        
        // unlock mutex so that other thread can send pings
        pthread_mutex_unlock(&mutex);
    }


    return EXIT_SUCCESS;
}