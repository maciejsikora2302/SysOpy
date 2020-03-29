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
#define EVER (;;) {}

int number_of_signals_receved = 0;
int number_of_signals_to_send = 0;

int pid_of_sender = -1;

void su1_handler(int sg_nr, siginfo_t *info, void *ucontext){
    number_of_signals_receved++;
    pid_of_sender = info->si_pid;
}

void su2_handler(int sg_nr){
    printf("I'm a catcher and I received SIGUSR2 and I've got %d SIGUSR1\n", number_of_signals_receved);
    exit(0);
}

int main(int argc, char** argv){
    //PID, ilość sygnałów do wysłania, tryb
    if( argc != 3){
        printf("Wrong amount of arguments %d\n", argc);
    }else{
        number_of_signals_to_send = atoi(argv[1]);
        char* mode = argv[2];

        //blocking all signals execpt for SIGUSR1 and SIGUSR2z  
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, S1);
        sigdelset(&mask, S2);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        
        //handling sigusr1 so that we can get pid of a sender
        struct sigaction act;
        act.sa_sigaction = su1_handler;
        act.sa_flags = SA_SIGINFO;
        sigemptyset(&act.sa_mask);
        sigaction(S1, &act, NULL);

        //handling sigusr2
        signal(S2, su2_handler);

        printf("std");

        if(strcmp(mode, "KILL") == 0){
            char* command = calloc(100, sizeof(char));
            system("chmod 777 ./sender");
            system("chmod 777 ./sender.c");
            sprintf(command, "./sender.c %d %d %s", getpid(), number_of_signals_to_send, mode);
            printf("Using command: '%s' ",command);
            system(command);
            free(command);
            //waiting untill we get sigusr2 back
            sigset_t su2_mask;
            sigemptyset(&su2_mask);
            sigaddset(&su2_mask, S2);
            int rec;
            sigwait(&su2_mask, &rec);

            for(int i=0;i<number_of_signals_receved;i++){
                kill(pid_of_sender, S1);
            }
            kill(pid_of_sender, S2);
            printf("Catchers ending his job.\n");
            return 0;
        }
    }
    return 0;
}