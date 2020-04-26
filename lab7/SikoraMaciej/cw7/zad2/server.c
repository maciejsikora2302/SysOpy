#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mqueue.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>

#include "setting.h"

#define FAIL_EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }

mqd_t queue_descriptor = -2;
int active = 1;
int clients_data[MAX_CLIENTS][3];
int client_count = 0;

void int_handler(int _) { exit(2); }

int find_queue_id(pid_t sender_pid) {
    for (int i=0; i < client_count; i++) {
        if(clients_data[i][0] == sender_pid)
            return clients_data[i][1];
    }
    return -1;
}

void handle_connect(Message *msg) {
    int queue_id = 0;
    for (int i=0; i < client_count; i++) {
        if (clients_data[i][0] == atoi(msg->message_text)) {
            clients_data[i][2] = 1;
            queue_id = clients_data[i][1];
            msg->m_type = CONNECT; 
        }
        if (clients_data[i][0] == msg->sender_pid){
            if(clients_data[i][2] == 1) msg->m_type = MESSAGE;
            clients_data[i][2] = 1;
        }
    }

    sprintf(msg->message_text, "%d", queue_id);
    int client_queue_id = find_queue_id(msg->sender_pid);
    if(mq_send(client_queue_id,(char*) msg, MSG_SIZE, 1) == -1)
        FAIL_EXIT("SERVER: no such client to connect to\n");
}

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
    if(mq_send(client_queue_id,(char*) msg, MSG_SIZE, 1) == -1)
        FAIL_EXIT("SERVER: LIST response failed");
}

void handle_disconnect(Message *msg) {
    for (int i=0; i < client_count; i++) {
        if (clients_data[i][0] == msg->sender_pid) {
            clients_data[i][2] = 0;
            return;
        }
    }
}

void handle_init(struct Message *msg) {

    int client_pid = msg->sender_pid;
    char client_path[15];
    sprintf(client_path, "/%d", client_pid);

    int client_queue_id = mq_open(client_path, O_WRONLY);
    if (client_queue_id == -1) FAIL_EXIT("SERVER -> reading client_queue_id failed\n");

    msg->m_type = INIT;
    msg->sender_pid = getpid();

    if (client_count > MAX_CLIENTS - 1) {
        printf("SERVER -> maximum number of clients reached\n");
        sprintf(msg->message_text, "%d", -1);
    } else {
        clients_data[client_count][0] = client_pid;
        clients_data[client_count][1] = client_queue_id;
        clients_data[client_count++][2] = 0;
        sprintf(msg->message_text, "%d", client_count - 1);
    }

    if (mq_send(client_queue_id,(char*) msg, MSG_SIZE, 1) == -1)
        FAIL_EXIT("server: LOGIN response failed\n");
}

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

void close_queue() {
    int i; for (i = 0; i < client_count; ++i) {
        if (mq_close(clients_data[i][1]) == -1) {
            printf("server: error closing %d client queue\n", i);
        }
        if (kill(clients_data[i][0], SIGINT) == -1) {
            printf("server: error killing %d client\n", i);
        }
    }
    if (queue_descriptor > -1) {
        if(mq_close(queue_descriptor) == -1) {
            printf("server: error closing public queue\n");
        } else {
            printf("server: queue closed\n");
        }

        if (mq_unlink(server_path) == -1) {
            printf("server: error deleting public queue\n");
        } else {
            printf("server: queue deleted successfully\n");
        }
    } else {
        printf("server: there was no need of deleting queue\n");
    }
}

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
        FAIL_EXIT("server: egistering server's atexit failed\n");

    if (signal(SIGINT, int_handler) == SIG_ERR)
        FAIL_EXIT("server: registering INT failed\n");

    struct mq_attr current_state;
    struct mq_attr posix_attr;
    posix_attr.mq_maxmsg = MAX_MESSAGE_QUEUE_SIZE;
    posix_attr.mq_msgsize = MSG_SIZE;

    queue_descriptor = mq_open(server_path, O_RDONLY | O_CREAT | O_EXCL, 0666, &posix_attr);

     if (queue_descriptor == -1)
        FAIL_EXIT("server: creation of public queue failed\n");

    Message buffer;
    while(1) {
        if (active == 0) {
            if (mq_getattr(queue_descriptor, &current_state) == -1)
                FAIL_EXIT("server: getting current state of public queue failed\n");
            if (current_state.mq_curmsgs == 0) break;
        }

        if (mq_receive(queue_descriptor,(char*) &buffer, MSG_SIZE, NULL) == -1)
            FAIL_EXIT("server: receiving message failed\n");
        handle_queue(&buffer);
    }
    return 0;
}