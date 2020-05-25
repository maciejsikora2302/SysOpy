#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h> 
#include <signal.h>
#include <pthread.h>


#define MAX_PLAYERS 4
#define SIZE_TEXT 256
#define UNIX_CONNECTION "unix"
#define NET_CONNECTION "net"


#define PLAYING 1
#define AWAITING 2


struct Player{
    char* name;
    int in_game;
}typedef Player;
