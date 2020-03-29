#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h> 
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define S1 SIGUSR1
#define S2 SIGUSR2

int number_of_signals_receved = 0;
int number_of_signals_to_send = 0;

int pid_of_sender = -1;

int not_received_su2 = 1;

void su1_handler(int sg_nr, siginfo_t *info, void *ucontext){
    number_of_signals_receved++;
    if (pid_of_sender == -1){
        pid_of_sender = info->si_pid;
        printf("Catcher got su1 from pid: %d\n", pid_of_sender);
    }
        
    
}

void su2_handler(int sg_nr){
    printf("I'm a catcher and I received SIGUSR2 and I've got %d SIGUSR1\n", number_of_signals_receved);
    not_received_su2 = 0;
    // exit(0);
}

int main(int argc, char** argv){
    //PID, ilość sygnałów do wysłania, tryb
    if( argc != 3){
        printf("Wrong amount of arguments %d\n", argc);
    }else{
        number_of_signals_to_send = atoi(argv[1]);
        char* mode = argv[2];

        printf("My pid: %d, Number of singales: %d, mode: %s\n", getpid(), number_of_signals_to_send, mode);

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

        if(strcmp(mode, "KILL") == 0){
            //waiting untill we get sigusr2 back
            printf("catcher waiting for signals...\n");
            while(not_received_su2 == 1){}
            printf("catcher received SIGUSR2\n");

            for(int i=0;i<number_of_signals_receved;i++){
                kill(pid_of_sender, S1);
            }
            kill(pid_of_sender, S2);
            printf("Catchers ending his job after sending signals back to sender.\n");
            return 0;
        }else if(strcmp(mode, "SIGQUEUE") == 0){
            // not_received_su2 = 1;
            printf("catcher waiting for signals...\n");
            while(not_received_su2 == 1){}
            printf("catcher received SIGUSR2\n");

            for(int i=0;i<number_of_signals_receved;i++){
                union sigval val;
                sigqueue(pid_of_sender, S1, val);
            }
            union sigval val;
            sigqueue(pid_of_sender, S2, val);
            printf("Catchers ending his job after sending signals back to sender.\n");
            return 0;
        }else if(strcmp(mode, "SIGRT") == 0){
            // unblocking
            sigset_t mask2;
            sigemptyset(&mask2);
            // sigdelset(&mask2, S1);
            // sigdelset(&mask2, S2);
            sigprocmask(SIG_SETMASK, &mask2, NULL);
            //handling sigusr1 so that we can get pid of a sender
            struct sigaction act;
            act.sa_sigaction = su1_handler;
            act.sa_flags = SA_SIGINFO;
            sigemptyset(&act.sa_mask);
            sigaction(SIGINT, &act, NULL);

            //handling sigusr2
            signal(SIGTSTP, su2_handler);

            //waiting untill we get sigusr2 back
            printf("catcher waiting for signals...\n");
            while(not_received_su2 == 1){}
            printf("catcher received SIGUSR2\n");

            for(int i=0;i<number_of_signals_receved;i++){
                kill(pid_of_sender, SIGINT);
            }
            kill(pid_of_sender, SIGTSTP);
            printf("Catchers ending his job after sending signals back to sender.\n");
            return 0;
        }
    }
    return 0;
}