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

mqd_t queue_descriptor = -1;
mqd_t private_id = -1;
mqd_t connect_to_queue = -1;
int session_id = -2;

char* path;

void close_queue() {
    if (private_id > -1) {
        if (mq_close(queue_descriptor) == -1) {
            printf("client: there was some error closing servers's queue!\n");
        } else {
            printf("client: servers's queue closed successfully!\n");
        }

        if (mq_close(private_id) == -1) {
            printf("client: there was some error closing client's queue!\n");
        } else {
            printf("client: queue closed successfully!\n");
        }

        if (mq_unlink(path) == -1) {
            printf("client: there was some error deleting client's queue!\n");
        } else {
            printf("client: queue deleted successfully!\n");
        }
    } else {
        printf("client: there was no need of deleting queue!\n");
    }
}

void list_request(Message *msg) {
    msg->m_type = LIST;

    if(mq_send(queue_descriptor,(char*) msg, MSG_SIZE, 1) == -1)
        FAIL_EXIT("CLIENT -> LIST request failed");

    if (mq_receive(private_id,(char*) msg, MSG_SIZE, NULL) == -1)
        FAIL_EXIT("client: catching LOGIN response failed\n");

    puts(msg->message_text);
}

void stop_request(Message *msg) {
    msg->m_type = STOP;

    if(mq_send(queue_descriptor,(char*) msg, MSG_SIZE, 1) == -1)
        FAIL_EXIT("CLIENT -> STOP request failed");

    exit(2);
}


void int_handler(int sig) {
    Message msg;
    msg.sender_pid = getpid();
    stop_request(&msg);
}

void disconnect_request(Message *msg) {

    if(mq_send(queue_descriptor,(char*) msg, MSG_SIZE, 1) == -1)
        FAIL_EXIT("CLIENT -> DISCONNECT request failed");

    if (msg->m_type != DISCONNECT) {
        sprintf(msg->message_text, "%s", "DISCONNECTED");
        if(mq_send(connect_to_queue,(char*) msg, MSG_SIZE, 1) == -1)
            FAIL_EXIT("CLIENT -> DISCONNECT request failed");
    }
        
    connect_to_queue = -1;
}

void message_request(Message *msg){

    printf("-------------------------\n");
    if (fgets(msg->message_text, MAX_SIZE, stdin) == 0) {
        printf("CLIENT: too many characters\n");
        return;
    }
    msg->m_type = MESSAGE;
    printf("-------------------------\n");
    if(mq_send(connect_to_queue,(char*) msg, MSG_SIZE, 1) == -1)
        FAIL_EXIT("CLIENT: CONNECT request failed");

    printf("2ND CLIENT -> ");
    if(mq_receive(private_id,(char*) msg, MSG_SIZE, NULL) == -1)
        FAIL_EXIT("CLIENT: catching CONNECT response failed");

    if (strcmp(msg->message_text, "DISCONNECTED") == 0) msg->m_type = DISCONNECT;
        
    printf("%s\n", msg->message_text);
}

void connect_request(Message *msg) {
    msg->m_type = CONNECT;

    printf("CONNECT TO? -> ");
    if (fgets(msg->message_text, MAX_SIZE, stdin) == 0) {
        printf("CLIENT: too many characters\n");
        return;
    }
    if(mq_send(queue_descriptor,(char*) msg, MSG_SIZE, 1) == -1)
        FAIL_EXIT("CLIENT: CONNECT request failed");

    if(mq_receive(private_id,(char*) msg, MSG_SIZE, NULL) == -1)
        FAIL_EXIT("CLIENT: catching CONNECT response failed");

    connect_to_queue = atoi(msg->message_text);

    if(connect_to_queue == 0) {
        puts("SERVER -> no such client to connect");
        return;
    }

    
    printf("SERVER -> connected to another client\n");

    char cmd[20];
    while(1) {


        signal(SIGINT, int_handler);
        printf("MESSAGE/DISCONNECT? -> ");
        if (fgets(cmd, 20, stdin) == NULL){
            printf("CLIENT: error reading you request\n");
            continue;
        }
        int n = strlen(cmd);
        if (cmd[n-1] == '\n') cmd[n-1] = 0;

        if (strcmp(cmd, "DISCONNECT") == 0) {
            disconnect_request(msg);
            return;
        } else if (strcmp(cmd, "MESSAGE") == 0) {
            message_request(msg);
        } else {
            printf("client: incorrect command\n");
        }

    }
}



void register_client() {
    Message msg;
    msg.m_type = INIT;
    msg.sender_pid = getpid();

    if (mq_send(queue_descriptor,(char*) &msg, MSG_SIZE, 1) == -1)
        FAIL_EXIT("client: LOGIN request failed\n");
    if (mq_receive(private_id,(char*) &msg, MSG_SIZE, NULL) == -1)
        FAIL_EXIT("client: catching LOGIN response failed\n");
    if (sscanf(msg.message_text, "%d", &session_id) < 1)
        FAIL_EXIT("client: scanning LOGIN response failed\n");
    if (session_id < 0)
        FAIL_EXIT("client: server cannot have more clients\n");

    printf("client: client registered. Session no: %d\n", session_id);
}


int main() {
    if(atexit(close_queue) == -1)
        FAIL_EXIT("Registering client's atexit failed");
    if(signal(SIGINT, int_handler) == SIG_ERR)
        FAIL_EXIT("Registering INT failed");

    path = getenv("HOME");

    if (path == NULL)
        FAIL_EXIT("Getting $HOME failed");

    queue_descriptor = mq_open(server_path, O_WRONLY);
    if (queue_descriptor == -1) FAIL_EXIT("Opening public queue failed\n");


    struct mq_attr posixAttr;
    posixAttr.mq_maxmsg = MAX_MESSAGE_QUEUE_SIZE;
    posixAttr.mq_msgsize = MSG_SIZE;

    private_id = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0666, &posixAttr);
    if (private_id == -1) FAIL_EXIT("client: creation of private queue failed\n");

    register_client();

    char cmd[20];
    Message msg;
    while(1) {

        signal(SIGINT, int_handler);
        msg.sender_pid = getpid();
        printf("CLIENT -> enter request: ");
        if (fgets(cmd, 20, stdin) == NULL){
            printf("CLIENT: error reading you request\n");
            continue;
        }
        int n = strlen(cmd);
        if (cmd[n-1] == '\n') cmd[n-1] = 0;

        if (strcmp(cmd, "LIST") == 0) {
            list_request(&msg);
        } else if (strcmp(cmd, "CONNECT") == 0) {
            connect_request(&msg);
        } else if (strcmp(cmd, "DISCONNECT") == 0) {
            disconnect_request(&msg);
        } else if (strcmp(cmd, "STOP") == 0) {
            stop_request(&msg);
        } else {
            printf("client: incorrect command\n");
        }
    }
}