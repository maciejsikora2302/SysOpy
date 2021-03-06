#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include "setting.h"

#define FAIL_EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }

int queue_descriptor = -2;
int active = 1;
int clients_data[MAX_CLIENTS][3];
int client_count = 0;

void int_handler(int _) { exit(2); }

//Searching for correct id in clients array
int find_queue_id(pid_t sender_pid) {
    for (int i=0; i < client_count; i++) {
        if(clients_data[i][0] == sender_pid)
            return clients_data[i][1];
    }
    return -1;
}

//handling CONNECT command
void handle_connect(Message *msg) {
    int queue_id = 0;
    for (int i=0; i < client_count; i++) {
        if (clients_data[i][0] == atoi(msg->message_text)) {
            clients_data[i][2] = 1;
            queue_id = clients_data[i][1];
            msg->m_type = CONNECT; 
        }
        if (clients_data[i][0] == msg->sender_pid) {
            if(clients_data[i][2] == 1) msg->m_type = MESSAGE;
            clients_data[i][2] = 1;
        }
    }

    sprintf(msg->message_text, "%d", queue_id);
    int client_queue_id = find_queue_id(msg->sender_pid);
    if(msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1)
        FAIL_EXIT("No such client to connect to -> f-handle_connection | server\n");
}

//handling LIST command
void handle_list(Message *msg) {

    char list[MAX_SIZE];
    
    char buff[64];
    strcpy(list, "List of clients: \n");
    for (int i = 0; i < client_count; i++){
        sprintf(buff, "%d %d \n", clients_data[i][0], clients_data[i][2]);
        strcat(list, buff);
    }
    
    sprintf(msg->message_text, "%s", list);

    int  client_queue_id = find_queue_id(msg->sender_pid);
    if(msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1)
        FAIL_EXIT("LIST response failed -> f-handle_list | server");
}

//handling DISCONNECT command
void handle_disconnect(Message *msg) {
    for (int i=0; i < client_count; i++) {
        if (clients_data[i][0] == msg->sender_pid) {
            clients_data[i][2] = 0;
            return;
        }
    }
}

//creating msg queue
void handle_init(struct Message *msg) {
    key_t client_queue_key;
    if (sscanf(msg->message_text, "%d", &client_queue_key) < 0)
        FAIL_EXIT("Reading client_queue_key failed -> f-handle_init | server\n");

    int client_queue_id = msgget(client_queue_key, 0);
    if (client_queue_id == -1)
        FAIL_EXIT("Reading client_queue_id failed -> f-handle_init | server\n");

    int client_pid = msg->sender_pid;
    msg->m_type = INIT;
    msg->sender_pid = getpid();

    if (client_count > MAX_CLIENTS - 1) {
        printf("Maximum number of clients reached -> f-handle_init | server\n");
        sprintf(msg->message_text, "%d", -1);
    } else {
        clients_data[client_count][0] = client_pid;
        clients_data[client_count][1] = client_queue_id;
        clients_data[client_count++][2] = 0;
        sprintf(msg->message_text, "%d", client_count - 1);
    }

    if (msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1)
        FAIL_EXIT("LOGIN response failed -> f-handle_init | server\n");
}

//handling STOP command
void handle_stop(Message *msg) {
    int idx = 0;
    for (int i = 0; i < client_count; i++) {
        if (clients_data[i][0] == msg->sender_pid)
            idx = i;
    }

    for( int i = idx; i < client_count && i < MAX_CLIENTS-1; i++) {
        for (int j = 0; j < 3; j++){
            clients_data[i][j] = clients_data[i+1][j];
        }
    }
    client_count--;
}

//closing queue
void close_queue() {
    if (queue_descriptor > -1) {
        int tmp = msgctl(queue_descriptor, IPC_RMID, NULL);
        if (tmp == -1) {
            printf("There was some error deleting server's queue -> f-close_queue | server\n");
        }
        printf("Queue deleted successfully -> f-close_queue | server\n");
    }

}

//main swtich for parsing command
void handle_queue(struct Message *msg) {
    if (msg == NULL) return;
    switch(msg->m_type){
        case STOP:
            handle_stop(msg);
            break;
        case DISCONNECT:
            handle_disconnect(msg);
            break;
        case LIST:
            handle_list(msg);
            break;
        case CONNECT:
            handle_connect(msg);
            break;
        case INIT:
            handle_init(msg);
            break;
        default:
            break;
    }
}

int main() {
    if (atexit(close_queue) == -1)
        FAIL_EXIT("Egistering server's atexit failed -> f-main | server\n");

    if (signal(SIGINT, int_handler) == SIG_ERR)
        FAIL_EXIT("Registering INT failed -> f-main | server\n");

    struct msqid_ds current_state;

    char* path = getenv("PATH");
    if(path == NULL)
        FAIL_EXIT("Getting environmental variable 'HOME' failed -> f-main | server\n");

    key_t publicKey = ftok(path, PROJECT_ID);
    if(publicKey == -1)
        FAIL_EXIT("Generation of publicKey failed -> f-main | server\n");

    queue_descriptor = msgget(publicKey, IPC_CREAT | IPC_EXCL | 0666);
    if(queue_descriptor == -1)
        FAIL_EXIT("Creation of public queue failed -> f-main | server\n");

    Message buffer;
    while(1) {
        if (active == 0) {
            if (msgctl(queue_descriptor, IPC_STAT, &current_state) == -1)
                FAIL_EXIT("Getting current state of public queue failed -> f-main | server\n");
            if (current_state.msg_qnum == 0) break;
        }

        if (msgrcv(queue_descriptor, &buffer, MSG_SIZE, 0, 0) < 0)
            FAIL_EXIT("Receiving message failed -> f-main->while | server\n");
        handle_queue(&buffer);
    }
    return 0;
}