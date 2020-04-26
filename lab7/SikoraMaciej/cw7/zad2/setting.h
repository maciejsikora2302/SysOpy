#ifndef SETTINGS_H
#define SETTINGS_H

#include <assert.h>
#include <signal.h>
#include <pwd.h>

#define MAX_CLIENTS 5
#define PROJECT_ID 0x999
#define MAX_SIZE 2048
#define MAX_MESSAGE_QUEUE_SIZE 9

const char server_path[] = "/server";

typedef enum m_type {
    STOP = 1,
    DISCONNECT = 2,
    LIST = 3,
    CONNECT = 4,
    INIT,
    ERROR,
    MESSAGE
} m_type;

typedef struct Message { 
    long m_type;
    pid_t sender_pid;
    char message_text[MAX_SIZE];
} Message;

const size_t MSG_SIZE = sizeof(Message);

#endif