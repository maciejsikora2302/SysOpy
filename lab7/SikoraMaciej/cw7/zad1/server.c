#include "common_utils.h"

int children[NO_TOTAL_WORKERS];
// int INIT_DONE = 0;
// int sem_id = -1;
// Orders

void exit_handler(int id){
    for(int i=0; i<NO_TOTAL_WORKERS; i++){
        kill(children[i], SIGINT);
    }
    // if(INIT_DONE){
    //     deleteSemaphore(semaphoreId);
    //     detachSharedArray(orders);
    //     detachSharedCounter(counter);
    //     deleteSharedCounter(shCounterId);
    //     deleteSharedArray(shArrayId); 
    // }
}


int main(){
    printf("so far works\n");
    signal(SIGINT, exit_handler);

    int sh_array = create_sh_array();
    int sh_counter = create_sh_counter();
    int sem = create_semaphore();

    Counter* counter = get_counter(sh_counter);
    counter -> to_pack = 0;
    counter -> to_send = 0;

    Order* orders = get_orders(sh_array);
    //creating "empty orders", since memory is already being used why not say that there are orders that are not requested yet -> create slots for them
    for(int i=0; i<MAX_ORDERS; i++){
        orders[i] = create_order(0);
    }

    int children_iter = 0;
    //It's time to create workers
    for(int i=0; i< NO_WORKER_PACKER; i++){
        printf("Packer created\n");
        int pid = fork();
        if(pid == 0){
            execl("./packer", "./packer", NULL);
        }else{
            children[children_iter++] = pid;
        }
    }

    for(int i=0; i<NO_WORKER_RECEIVER;i++){
        printf("Receiver created\n");
        int pid = fork();
        if(pid == 0){
            execl("./receiver", "./receiver", NULL);
        }else{
            children[children_iter++] = pid;
        }
        
    }

    for(int i=0; i<NO_WORKER_SENDER; i++){
        printf("Sender created\n");
        int pid = fork();
        if(pid == 0){
            execl("./sender", "./sender", NULL);
        }else{
            children[children_iter++] = pid;
        }
    }

    for (int i = 0; i < NO_TOTAL_WORKERS; i++) {
        waitpid(children[i], NULL, 0);
    }

    detach_sh_array(orders);
    detach_sh_counter(counter);
    del_semaphore(sem);
    del_sh_array(sh_array);
    del_sh_counter(sh_counter);
    printf("Server finished working \\o/\n");
}