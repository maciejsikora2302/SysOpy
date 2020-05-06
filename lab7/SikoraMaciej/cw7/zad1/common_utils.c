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



char* random_string(int length) {
    char* str = calloc(length + 1, sizeof(char));
    
    for (int i = 0; i < length; i++) {
        char randomLetter = 'a' + (rand() % 26);
        str[i] = randomLetter;
    }

    str[length] = '\0';

    return str;
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
int get_sh_array(){
    int id;
    if( (id = shmget(SHM_KEY_ORDERS_ARRAY, 0, 0666)) < 0){
        perror("get_sh_array, shmget");
    }
    return id;
}

int create_sh_array(){
    int id;
    if( (id = shmget(SHM_KEY_ORDERS_ARRAY, sizeof(Order) * MAX_ORDERS, 0666 | IPC_CREAT)) < 0 ){
        perror("get_sh_array, shmget");
    }
    return id;
}

void detach_sh_array(Order* orders){
    shmdt(orders);
}

void del_sh_array(int id){
    shmctl(id, IPC_RMID, NULL);
}

Order* get_orders(int id){
    Order* orders = shmat(id, NULL, 0);//id of shared memory, buffer for stats, flags
    if(orders == (void*) -1){
        printf("getting orders from shared memory has failed :(\n");
        return NULL;
    }
    return orders;
}


// Shared orders counter
int get_sh_counter(){
    return shmget(SHM_KEY_COUNTER, 0, 0666);
}

int create_sh_counter(){
    return shmget(SHM_KEY_COUNTER, sizeof(Counter), 0666|IPC_CREAT);
}

void detach_sh_counter(Counter* counter){
    shmdt(counter);
}

void del_sh_counter(int id){
    shmctl(id, IPC_RMID, NULL);
}

Counter* get_counter(int id){
    return shmat(id, NULL, 0);
}

// Semaphore \o/
int create_semaphore(){
    int id = semget(SEMAPHORE_KEY, 1, 0666 | IPC_CREAT);

    union sem_un arg;
    arg.val = 1;
    semctl(id, 0, SETVAL, arg);

    return id;
}

void del_semaphore(int sem_id){
    semctl(sem_id, 0, IPC_RMID);
}

int get_semaphore(){
    return semget(SEMAPHORE_KEY, 0, 0666);
}

void use_resource(int sem_id){
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    
    struct sembuf a_lot_of_operations[1];
    a_lot_of_operations[0] = buf;

    semop(sem_id, a_lot_of_operations, 1);
}

void free_resource(int sem_id){
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    
    struct sembuf a_lot_of_operations[1];
    a_lot_of_operations[0] = buf;

    semop(sem_id, a_lot_of_operations, 1);
}