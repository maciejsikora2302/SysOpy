#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h> 
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define S1 SIGUSR1
#define S2 SIGUSR2

int number_of_signals_receved = 0;
int number_of_signals_to_send = 0;

int waiting_for_su2 = 1;
int waiting_for_su1 = 1;

void su1_handler(int sg_nr, siginfo_t *info, void *ucontext){
    number_of_signals_receved++;
    // printf("sender got su1\n");
    waiting_for_su1 = 0;
}

void su2_handler(int sg_nr){
    waiting_for_su2 = 0;
    waiting_for_su1 = 0;
    // exit(0);
}

int main(int argc, char** argv){
    //PID, ilość sygnałów do wysłania, try
    if( argc != 4){
        printf("Wrong amount of arguments %d", argc);
    }else{
        int pid = atoi(argv[1]);
        number_of_signals_to_send = atoi(argv[2]);
        char* mode = argv[3];

        // sigset_t mask;
        // sigfillset(&mask);
        // sigdelset(&mask, S1);
        // sigdelset(&mask, S2);
        // sigprocmask(SIG_SETMASK, &mask, NULL);

        printf("Sender | PID: %d, Sig to send: %d, mode: %s\n", pid, number_of_signals_to_send, mode);
        struct sigaction act;
        act.sa_sigaction = su1_handler;
        act.sa_flags = 0;
        sigemptyset(&act.sa_mask);
        sigaction(S1, &act, NULL);


        signal(S2, su2_handler);

        if(strcmp(mode, "KILL") == 0){
            for(int i=0; i<number_of_signals_to_send; i++){
                kill(pid, S1);
                while(waiting_for_su1 == 1){}
                waiting_for_su1 = 1;
            }
            kill(pid, S2);
            // printf("Sender enering inf loop\n");

            printf("I'm a sender and I have received %d singals back and I have send %d\n", number_of_signals_receved, number_of_signals_to_send);
        }else if(strcmp(mode, "SIGQUEUE") == 0){
            for(int i=0; i<number_of_signals_to_send; i++){
                union sigval val;
                sigqueue(pid, S1, val);
                while(waiting_for_su1 == 1){}
                waiting_for_su1 = 1;
            }
            union sigval val;
            sigqueue(pid, S2, val);
            printf("I'm a sender and I have received %d singals back and I have send %d\n", number_of_signals_receved, number_of_signals_to_send);
        }else if(strcmp(mode, "SIGRT") == 0){
            // SIGINT
            // SIGTSTP

            struct sigaction act;
            act.sa_sigaction = su1_handler;
            act.sa_flags = 0;
            sigemptyset(&act.sa_mask);
            sigaction(SIGINT, &act, NULL);

            signal(SIGTSTP, su2_handler);

            for(int i=0; i<number_of_signals_to_send; i++){
                kill(pid, S1);
                while(waiting_for_su1 == 1){}
                waiting_for_su1 = 1;
            }
            kill(pid, S2);
            printf("I'm a sender and I have received %d singals back and I have send %d\n", number_of_signals_receved, number_of_signals_to_send);
        }
    }
}