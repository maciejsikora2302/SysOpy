#include "common_utils.h"

Order create_order(int n) {
  Order* order = malloc(sizeof(Order));

  order->n = n;
  order->state = UNPACKED;

  return *order;
}


int next_empty(int start, Order* orders){
    int i = start;
    for(int j=0; j<MAX_ORDERS; j++){
        if(orders[i].n == 0){
            return i;
        }
        i = (i+1) % MAX_ORDERS;
    }
    return -1;
}

int next_unpacked(int start, Order* orders){
    int i = start;
    for(int j=0; j<MAX_ORDERS; j++){
        if(orders[i].n != 0 && orders[i].state == UNPACKED){
            return i;
        }
        i = (i+1) % MAX_ORDERS;
    }
    return -1;
}

int next_to_send(int start, Order* orders){
    int i = start;
    for(int j=0; j<MAX_ORDERS; j++){
        if(orders[i].state == PACKED){
            return i;
        }
        i = (i+1) % MAX_ORDERS;
    }
    return -1;
}

char* get_time(char* buffor) {
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;
    char buff [82];
  
    strftime(buff, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
    sprintf(buffor, "%s:%03d", buff, milli);

    return buffor;
}


// Shared array of orders
int create_sh_array(){
    int fd;
    if( (fd = shm_open(SHM_NAME_ORDERS_ARRAY, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0){
        perror("create_sh_array, shm_open");
    }
    if(ftruncate(fd, MAX_ORDERS * sizeof(Order)) < 0){
        perror("create_sh_array, ftruncate");
    }
    return fd;
}

int get_sh_array(){
    int fd;
    if( (fd = shm_open(SHM_NAME_ORDERS_ARRAY, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) < 0){
        perror("get_sh_array, shm_open");
    }
    return fd;
}

void detach_sh_array(Order* orders){
    if( munmap(orders, MAX_ORDERS * sizeof(Order)) < 0 ){
        perror("detach_sh_array");
    }
}

void del_sh_array(){
    if( shm_unlink(SHM_NAME_ORDERS_ARRAY) < 0 ){
        perror("detach_sh_array");
    }
}

Order* get_orders(int fd){
    Order* orders = mmap(NULL, MAX_ORDERS * sizeof(orders), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(orders == (void*) -1){
        printf("getting orders from shared memory has failed :(\n");
        return NULL;
    }
    return orders;
}


// Shared orders counter
int create_sh_counter(){
    int fd;
    if( (fd = shm_open(SHM_NAME_COUNTER, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0){
        perror("create_sh_counter, shm_open");
    }
    if(ftruncate(fd, sizeof(Counter)) < 0){
        perror("create_sh_counter, ftruncate");
    }
    return fd;
}

int get_sh_counter(){
    int fd;
    if( (fd = shm_open(SHM_NAME_COUNTER, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) < 0){
        perror("get_sh_counter, shm_open");
    }
    return fd;
}

void detach_sh_counter(Counter* counter){
    if( munmap(counter, sizeof(Counter)) < 0 ){
        perror("detach_sh_counter");
    }
}

void del_sh_counter(){
    if( shm_unlink(SHM_NAME_COUNTER) < 0 ){
        perror("detach_sh_counter");
    }
}

Counter* get_counter(int fd){
    Counter* counter = mmap(NULL, sizeof(Counter), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(counter == (void*) -1){
        printf("getting counter from shared memory has failed :(\n");
        return NULL;
    }
    return counter;
}

// Semaphore \o/
void create_semaphore(){
    sem_t *sem = sem_open(SEMAPHORE_NAME, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1); //name, flags, permissions, start_val
    if(sem == SEM_FAILED){
        perror("create_semaphore");
    }
}

void del_semaphore(){
    sem_t *sem = sem_open(SEMAPHORE_NAME, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO); //name, flags, permissions, start_val
    if(sem_close(sem) < 0){
        perror("del_semaphore");
    }
}

sem_t* get_semaphore(){
    sem_t* ret;
    if( (ret = sem_open(SEMAPHORE_NAME, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) == SEM_FAILED ){
        perror("get_semaphore");
        return NULL;
    }
    return ret;
}

void use_resource(sem_t* sem){
    sem_wait(sem);
}

void free_resource(sem_t* sem){
    sem_post(sem);
}