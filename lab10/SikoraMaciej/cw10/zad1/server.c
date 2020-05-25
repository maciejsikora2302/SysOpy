#include "common_header.h"

Player** players;

int unique_name(char* name){
    for(int i=0; i<MAX_PLAYERS; i++){
        if(players[i] -> name == NULL) continue;

        if(strcmp(players[i] -> name, name) == 0){
            return -1;
        }
    }
    return 0;
}


void init_players(){
    players = malloc(MAX_PLAYERS * sizeof(Player*));
    for(int i=0;i<MAX_PLAYERS; i++){
        players[i] = malloc(sizeof(Player));
        players[i] -> name = NULL;
        players[i] -> in_game = AWAITING;
    }
}


void clean_stuff(){
    for(int i=0;i < MAX_PLAYERS; i++){
        free(players[i]);
    }
}


int main(int argc, char** argv){

    if(argc != 3){
        printf("Wrong amount of arguments. Should be 3.\n");
        exit(EXIT_FAILURE);
    }

    atexit(clean_stuff);

    init_players();

    
    char* port = argv[1];
    char* path_to_unix_socket = argv[2];
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addres;
    server_addres.sin_family = AF_INET;
    server_addres.sin_port = htons(atoi(port));
    server_addres.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *) &server_addres, sizeof(server_addres));
    
    while(1){
        listen(server_socket, 5);//5 będzie parametrem programu
        int client_socket;
        client_socket = accept(server_socket, NULL, NULL);
        char name[123];
        recv(client_socket, &name, sizeof(name), 0);
        printf("Server: I have recived name equal to: %s\n", name);

        char callback_message[SIZE_TEXT];
        sprintf(callback_message, "Your name is %s", name);
        send(client_socket, callback_message, sizeof(callback_message), 0);
    }
    

    close(server_socket);



}