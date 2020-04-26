#include "common_utils.h"

Order* orders;
Counter* counter;
int sh_array;
int sh_coutner;
sem_t* sem;


void detach_stuff(){
    detach_sh_counter(counter);
    detach_sh_array(orders);
}


int main(){
    printf("Pid: %d, I'm a receiver!\n", getpid());
    atexit(detach_stuff);
    sh_array = get_sh_array();
    sh_coutner = get_sh_counter();
    sem = get_semaphore();
    orders = get_orders(sh_array);
    counter = get_counter(sh_coutner);

    srand(time(NULL));


    int i=0, k;
    while (1)
    {
        use_resource(sem);

        if( (k = next_empty(i, orders)) >= 0){
            i = k;
            orders[i].n = rand()%1000 + 1;
            orders[i].state = UNPACKED;
            counter->to_pack++;
            char* time_buff = calloc(100, sizeof(char));

            printf("Pid(%d) Timestamp(%s): I've got %d. Number of orders to pack: %d. Number of orders to send: %d.\n",
                    getpid(), get_time(time_buff), orders[i].n, counter->to_pack, counter->to_send
            );

            free(time_buff);


            i = (i+1) % MAX_ORDERS;
        }

        free_resource(sem);
    }
        

    
    
}