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

//===============POSIX================
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>

//names for POSIX functions
#define SHM_NAME_ORDERS_ARRAY "/ARRAY"
#define SHM_NAME_COUNTER "/KEY"
#define SEMAPHORE_NAME "/MAIN"


#define MAX_ORDERS 5

#define NO_WORKER_PACKER 2
#define NO_WORKER_RECEIVER 4
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
char* get_time(char* buffor);

// Shared array of orders
int create_sh_array();
int get_sh_array();
void detach_sh_array(Order* orders);
void del_sh_array(); 
Order* get_orders(int fd);

// Shared orders counter
int create_sh_counter();
int get_sh_counter();
void detach_sh_counter(Counter* counter);
void del_sh_counter(); 
Counter* get_counter(int id);

// Semaphore \o/
void create_semaphore(); 
void del_semaphore();
sem_t* get_semaphore();

// Semaphore manipulation functions
void use_resource(sem_t* sem);
void free_resource(sem_t* sem);