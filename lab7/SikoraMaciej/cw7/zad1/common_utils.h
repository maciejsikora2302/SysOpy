#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

//getting key_t using ftok function and using "HOME" path
#define SHM_KEY_ORDERS_ARRAY (ftok(getenv("HOME"), 12))
#define SHM_KEY_COUNTER (ftok(getenv("HOME"), 124))
#define SEMAPHORE_KEY (ftok(getenv("HOME"), 64))

#define MAX_ORDERS 5

#define NO_WORKER_RECEIVER 2
#define NO_WORKER_PACKER 4
#define NO_WORKER_SENDER 3
#define NO_TOTAL_WORKERS (NO_WORKER_SENDER + NO_WORKER_PACKER + NO_WORKER_RECEIVER)

#define PACKED 1
#define UNPACKED 0

#define TIME_BUFFER_LENGTH 84


struct Order {
    int  n;
    int state;
} typedef Order;

struct Counter {
    int  to_pack;
    int  to_send;
} typedef Counter;

union sem_un {
    int val;
    struct semid_ds *buf;
    unsigned short* array;
} arg;

Order create_order(int n);

// Orders manipulation
int next_empty(int start, Order* orders);
int next_unpacked(int start, Order* orders);
int next_to_send(int start, Order* orders);

// Small utils
char* random_string(int length);
char* get_time(char* buffor);

// Shared array of orders
int get_sh_array();
int create_sh_array();
void detach_sh_array(Order* orders);
void del_sh_array(int id); 
Order* get_orders(int id);

// Shared orders counter
int get_sh_counter();
int create_sh_counter();
void detach_sh_counter(Counter* counter);
void del_sh_counter(int id); 
Counter* get_counter(int id);

// Semaphore \o/
int create_semaphore(); 
void del_semaphore(int sem_id);
int get_semaphore();

// Semaphore manipulation functions
void use_resource(int sem_id);
void free_resource(int sem_id);
