#include "common.h"

int* client_sockets;
int ping_interval = 3;
Player** players;

int net_socket, unix_socket;
char* path;
struct sockaddr_in address;
struct sockaddr_un addr;

pthread_mutex_t mutex;
char buffer [MESSAGE_BUFFER_LEN];

void disconnect(int i) {
    int sd = client_sockets[i];
    int addrlen = sizeof(address);
    getpeername(sd , (struct sockaddr*) &address ,(socklen_t*) &addrlen);
    printf("Server: Client %s(%d) disconnected\n", players[i] -> name, i);
        
    if (players[i] -> in_game) {
        int opponent = players[i] -> other;
        // check if opponent hasn't already left
        if (client_sockets[opponent] != 0) {
            notif_msg(buffer, MESSAGE_OP_QUIT);
            send(client_sockets[opponent], buffer, strlen(buffer), 0);
        }
    }

    // Close the socket and mark for reuse 
    close(sd); 
    free(players[i] -> name);

    client_sockets[i] = 0; 
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
            if (client_sockets[i] > 0 ) {
                Player* player = players[i];
                // check if client responded to ping
                if (player -> was_pinged && !player -> pinged_back) {
                    printf("Ping Thread: Client %s(%d) timed out\n", players[i] -> name, i);
                    disconnect(i);
                } else {
                    notif_msg(buffer, MESSAGE_PING);
                    send(client_sockets[i], buffer, strlen(buffer), 0);
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
        int fd = client_sockets[i];
        free(players[i]);
        notif_msg(buffer, MESSAGE_SERVER_SHUTDOWN);
        send(fd, buffer, strlen(buffer), 0);
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }

    unlink(path);

    free(players);
    free(client_sockets);

    close(net_socket);
    close(unix_socket);

    printf("\nServer: Shutdown -> closed all connections\n");
}

int find_opponent(int j) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] > 0 && i != j && players[i] -> name && !players[i] -> in_game)
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

    client_sockets = calloc(MAX_CLIENTS, sizeof(int));
    players        = malloc(MAX_CLIENTS * sizeof(Player*));
    for(int i = 0; i < MAX_CLIENTS; i++) {
        players[i] = malloc(sizeof(Player));
        players[i] -> name = NULL;
        players[i] -> in_game = false;
        players[i] -> was_pinged = false;
        players[i] -> pinged_back = false;
    }

    atexit(cleanup);
    signal(SIGINT, sighandler);

    // set of socket descriptors
    fd_set readfds;
    int port = atoi(argv[1]);
    path = argv[2];

    if (port < 1024) 
        printf("Port number must be greater or equal 1024\n");

    // address for unix socket
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    // address for net socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // bind & listen net socket
    net_socket = socket(AF_INET, SOCK_STREAM, 0);
    // This is to handle situation when client does not close his socket after server shut down 
    int opt = 1;  
    setsockopt(net_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt));

    if (net_socket == -1) 
        printf("Server: Failed to create socket\n");

    if ((bind(net_socket, (struct sockaddr*) &address, sizeof(address))) != 0)
        printf("Server: Binding socket failed\n");

    if ((listen(net_socket, MAX_WAITING_CONNECTIONS)) != 0)   
        printf("Server: Listen failed\n");
    
    printf("Server: INET socket started on ipv4: %s (localhost), port: %d\n",
            "127.0.0.1\n", ntohs(address.sin_port));

    // bind & listen unix socket
    unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unix_socket == -1) 
        printf("Server: Failed to create unix socket\n");

    if ((bind(unix_socket, (struct sockaddr*) &addr, sizeof(addr))) != 0)
        printf("Server: Binding unix socket failed\n");

    if ((listen(unix_socket, MAX_WAITING_CONNECTIONS)) != 0)   
        printf("Server: Listen unix socket failed\n");

    printf("Server: UNIX socket, listening on path: %s \n", path);

    int addrlen = sizeof(address);
    puts("Server: Waiting for connections\n");

    // start pinging thread
    pthread_t thr;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&thr, NULL, ping_thr, NULL);

    // Actual server logic down here
    int sd, max_sd, newSocket;
    char header [64];
    while (1) {
        // Initialize set 
        max_sd = (net_socket > unix_socket) ? net_socket : unix_socket;

        FD_ZERO(&readfds);
        FD_SET(net_socket, &readfds);
        FD_SET(unix_socket, &readfds);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];

            if (sd > 0)
                FD_SET(sd, &readfds);
            
            if (sd > max_sd)
                max_sd = sd;
        }

        // Wait for any fd in set to become ready && lock sockets
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL); 
        pthread_mutex_lock(&mutex);
		if (activity < 0 && errno != EINTR) 		 
            printf("Server: Select printf occured\n");

        // Check if there's connection on net socket
        if (FD_ISSET(net_socket, &readfds)) {
            newSocket = accept(net_socket, (struct sockaddr*) &address, (socklen_t*)&addrlen);
            if (newSocket == -1) 
                printf("Server: Accept connection failed");

			printf("Server: New connection, socket fd is %d, ip is : %s, port : %d \n",
                   newSocket , inet_ntoa(address.sin_addr),
                   ntohs(address.sin_port)); 
		
            // add new socket/player
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = newSocket;
                    break;
                }
            }
        }

        // Else check if there's connection on unix socket
        if (FD_ISSET(unix_socket, &readfds)) {
            if ((newSocket = accept(unix_socket, NULL, NULL)) == -1) 
                printf("Server: Accept connection failed");

			printf("Server: New connection on unix socket, socket fd is %d\n", newSocket);
		
            // add new socket/player
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = newSocket;
                    break;
                }
            }
        }

        // else check client sockets
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];

            // handle various client messages
            if (FD_ISSET(sd, &readfds)) {
                // here we should read until we reach ;marker
                int bytesRead = read_msg(buffer, sd);
                get_header(buffer, header);

                // handle disconnect
                if (bytesRead <= 0 || str_eq(header, MESSAGE_QUIT)) {
                    disconnect(i);
                } else if (str_eq(header, MESSAGE_NAME)) {
                    // player sends his name and waits for game to start
                    char* name = malloc(sizeof(MAX_CLIENT_NAME));
                    parse_name_msg(buffer, name);

                    // handle non unique name
                    if (!unique(name)) {
                        notif_msg(buffer, MESSAGE_NOT_UNIQUE);
                        send(sd, buffer, strlen(buffer), 0);
                        disconnect(i);
                        continue;
                    } 

                    players[i] -> name = name;

                    int opponent = find_opponent(i);
                    if (opponent == -1) {
                        // send wait message
                        notif_msg(buffer, MESSAGE_WAIT);
                        send(newSocket, buffer, strlen(buffer), 0);
                        printf("Server: Sent wait message to player %s(%d)\n", name, i);
                    } else {
                        // start game 
                        char mark = cross_circle();
                        char oppositeMark = opposite(mark);

                        int playerA = client_sockets[i];
                        int playerB = client_sockets[opponent];

                        players[i] -> other = opponent;                 
                        players[opponent] -> other = i;

                        players[i] -> in_game = true;                 
                        players[opponent] -> in_game = true;

                        printf("Server: Starting game between %s(%d) and %s(%d)\n",
                            players[i] -> name, i, players[opponent] -> name, opponent);

                        // send players play message 
                        play_msg(buffer, players[opponent] -> name, mark);
                        send(playerA, buffer, strlen(buffer), 0);

                        play_msg(buffer, players[i] -> name, oppositeMark);
                        send(playerB, buffer, strlen(buffer), 0);
                    }

                } else if (str_eq(header, MESSAGE_MOVE)) {
                    // forward message
                    int opponent = players[i] -> other;
                    int opfd = client_sockets[opponent];

                    send(opfd, buffer, strlen(buffer), 0);
                } else if(str_eq(header, MESSAGE_PING_BACK)) {
                    players[i] -> pinged_back = true;
                } else {
                    printf("Server: Unknown message %s \n", buffer);
                }
            }
        }
        
        // unlock mutex so that other thread can send pings
        pthread_mutex_unlock(&mutex);
    }


    return EXIT_SUCCESS;
}