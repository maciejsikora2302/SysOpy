#include "common_header.h"

int main(int argc, char** argv){


    if (argc < 3 || argc > 4){
        printf("Client: Wrong amount of arguments. Should be 3 or 4 depending on connection type.\n");
        exit(EXIT_FAILURE);
    }else{
        char* name = argv[1];
        char* connection_type = argv[2];
        if (strcmp(connection_type, NET_CONNECTION) == 0){
            char* port = argv[3];
            
            int client_socket;
            client_socket = socket(AF_INET, SOCK_STREAM, 0);

            struct sockaddr_in server_addres;
            server_addres.sin_family = AF_INET;
            server_addres.sin_port = htons(atoi(port));
            server_addres.sin_addr.s_addr = INADDR_ANY;
            
            if( connect(client_socket, (struct sockaddr *) &server_addres, sizeof(server_addres)) < 0){
                printf("Client: Something went wrong with connecting.\n");
            }else{
                printf("Client: Connection succesful.\n");
            }

            send(client_socket, name, sizeof(name), 0);
            
            char text_response[SIZE_TEXT];

            recv(client_socket, &text_response, sizeof(text_response), 0);

            printf("Client: Text response i have recived (should be my name):\n%s\n", text_response);
            printf("Clinet: Now I'm awaiting for server to pick my opponent.\n");

            

            printf("Client: My opponent is: %s\n", text_response);

            int playing = 1;

            while(playing){
                printf("Client: Awaiting for permission to make a move.\n");
                recv(client_socket, &text_response, sizeof(text_response), 0);
                if (strcmp(text_response, "make a move") == 0){
                    printf("Client: I have received permission to make a move. Waiting for input. (1-9 or quit)\n");
                    char input[SIZE_TEXT];
                    gets(input);
                    if(strcmp(input, "quit") == 0){
                        printf("Client: I am ending this game.\n")
                        send(client_socket, input, sizeof(input), 0);
                        playing = 0;
                    }else{
                        printf("Client: Command I'm sending to server: %s", input);
                        send(client_socket, input, sizeof(input), 0);
                    }
                }
            }
            
            
            close(client_socket);
        }

        if (strcmp(connection_type, UNIX_CONNECTION) == 0){
            char path_to_unix_socket[SIZE_TEXT];
            strcpy(path_to_unix_socket, argv[3]);
        }
        
    }

}