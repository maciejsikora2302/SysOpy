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

int session_id = -2;
int queue_descriptor = -1;
int private_id = -1;
int connect_to_queue = -1;

int create_queue(char *path, int id) {
    int key = ftok(path, id);
    if(key == -1) FAIL_EXIT("Generation of key failed -> f-create_queue | client");

    int queue_id = msgget(key, 0);
    if (queue_id == -1) FAIL_EXIT("Opening queue failed -> f-create_queue | client");

    return queue_id;

}

void close_queue() {
    if (private_id > -1) {
        if (msgctl(private_id, IPC_RMID, NULL) == -1){
            puts("\nThere was some error deleting clients's queue -> f-close_queue | client\n");
        }
        else {
            puts("\nClient's queue deleted successfully -> f-close_queue | client\n");
        }
    }
}

void list_request(Message *msg) {
    msg->m_type = LIST;

    if(msgsnd(queue_descriptor, msg, MSG_SIZE, 0) == -1)
        FAIL_EXIT("LIST request failed -> f-list_request | client");

    if (msgrcv(private_id, msg, MSG_SIZE, 0, 0) == -1)
        FAIL_EXIT("Catching LOGIN response failed -> f-list_request | client\n");

    puts(msg->message_text);
}

void stop_request(Message *msg) {
    msg->m_type = STOP;

    if(msgsnd(queue_descriptor, msg, MSG_SIZE, 0) == -1)
        FAIL_EXIT("STOP request failed -> f-stop_request | client");

    exit(2);
}


void int_handler(int sig) {
    Message msg;
    msg.sender_pid = getpid();
    stop_request(&msg);
}

void disconnect_request(Message *msg) {

    if(msgsnd(queue_descriptor, msg, MSG_SIZE, 0) == -1)
        FAIL_EXIT("DISCONNECT request failed -> f-disconnect_request | client");

    if (msg->m_type != DISCONNECT) {
        sprintf(msg->message_text, "%s", "DISCONNECTED");
        if(msgsnd(connect_to_queue, msg, MSG_SIZE, 0) == -1)
            FAIL_EXIT("DISCONNECT request failed -> f-disconnect_request | client");
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
    if(msgsnd(connect_to_queue, msg, MSG_SIZE, 0) == -1)
        FAIL_EXIT("CLIENT: CONNECT request failed");

    printf("2ND CLIENT -> ");
    if(msgrcv(private_id, msg, MSG_SIZE, 0, 0) == -1)
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
    if(msgsnd(queue_descriptor, msg, MSG_SIZE, 0) == -1)
        FAIL_EXIT("CLIENT: CONNECT request failed");

    if(msgrcv(private_id, msg, MSG_SIZE, 0, 0) == -1)
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



void register_client(key_t private_key) {
    Message msg;
    msg.m_type = INIT;
    msg.sender_pid = getpid();

    sprintf(msg.message_text, "%d", private_key);

    if (msgsnd(queue_descriptor, &msg, MSG_SIZE, 0) == -1)
        FAIL_EXIT("client: LOGIN request failed\n");
    if (msgrcv(private_id, &msg, MSG_SIZE, 0, 0) == -1)
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

    char* path = getenv("HOME");

    if (path == NULL)
        FAIL_EXIT("Getting $HOME failed");

    queue_descriptor = create_queue(path, PROJECT_ID);

    key_t private_key = ftok(path, getpid());

    if (private_key == -1)
        FAIL_EXIT("Generation of private key failed");

    private_id = msgget(private_key, IPC_CREAT | IPC_EXCL | 0666);

    if (private_id == -1)
        FAIL_EXIT("Creation of private queue failed");

    register_client(private_key);

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