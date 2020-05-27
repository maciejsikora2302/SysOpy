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

#define UNIX_PATH_MAX    108

struct sockaddr_un {
    sa_family_t sun_family;               /* AF_UNIX */
    char        sun_path[UNIX_PATH_MAX];  /* pathname */
} typedef sockaddr_un;

#define MAX_WAITING_CONNECTIONS 5
#define MAX_CLIENTS 12
#define MAX_CLIENT_NAME 64
#define MESSAGE_BUFFER_LEN 256

// client -> server && server -> client
#define MESSAGE_MOVE  "move"

// client -> server
#define MESSAGE_NAME  "name"
#define MESSAGE_QUIT  "quit"
#define MESSAGE_PING_BACK "ping-back"

// server -> client
#define MESSAGE_WAIT  "wait"
#define MESSAGE_PLAY  "play"
#define MESSAGE_OP_QUIT "oponnent-quit"
#define MESSAGE_SERVER_SHUTDOWN "shutdown"
#define MESSAGE_PING "ping"
#define MESSAGE_NOT_UNIQUE  "not_unique"

#define MV_CHR_BLANK  '-'
#define MV_CHR_CIRCLE 'O'
#define MV_CHR_CROSS  'X'

struct Player {
    char* name;
    bool in_game;
    int other; // if in game
    bool was_pinged;
    bool pinged_back;
} typedef Player;

struct Board {
    char mv [3][3];
    char next_move;
} typedef Board;

bool str_eq(char* a, char* b);

void show(Board* board, const char* comment);

void clear(); 

void clearline();

// game logic
Board* new_board();

void move(Board* board, int position);

void print_board(Board* board);

char opposite(char mv);

char cross_circle();

// -1 for invalid, 0 for valid
int valid_move(Board* board, char* printfBuff, int mv);

int game_state(Board* board); 


// creating valid messages
void notif_msg(char* buffer, char* notification);

void name_msg(char* buffer, char* name);

void move_msg(char* buffer, int move); 

void play_msg(char* buffer, char* name, char mark);


// parsing messages
int read_msg(char* buffer, int sockfd);

void parse_name_msg(char* buffer, char* nameBuff);

void parse_move(char* msg, int* ptr);

void parse_play_msg(char* msg, char* name, char* mark);

void get_header(char* msg, char* headerBuffer);


