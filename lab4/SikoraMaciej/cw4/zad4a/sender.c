#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h> 
#include <string.h>
#include <sys/wait.h>

#define S1 SIGUSR1
#define S2 SIGUSR2

int number_of_signals_receved = 0;
int number_of_signals_to_send = 0;

// void su1_handler(int sg_nr){
//     number_of_signals_receved++;
// }

// void su2_handler(int sg_nr){
//     printf("I'm a sender and I have received %d singals back and I have send %d", number_of_signals_receved, number_of_signals_to_send);
//     exit(0);
// }

int main(int argc, char** argv){
    //PID, ilość sygnałów do wysłania, tryb
    printf("beg of sender\n");
    if( argc != 4){
        printf("Wrong amount of arguments %d", argc);
    }else{
        int pid = atoi(argv[1]);
        number_of_signals_to_send = atoi(argv[2]);
        char* mode = argv[3];
        // signal(S1, su1_handler);
        // signal(S2, su2_handler);

        if(strcmp(mode, "KILL") == 0){
            for(int i=0; i<number_of_signals_to_send; i++){
                kill(pid, S1);
            }
            kill(pid, S2);
            for(;;){

            }
        }
    }
}