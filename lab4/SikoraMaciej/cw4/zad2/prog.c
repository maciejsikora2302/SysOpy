#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h> 
#include <string.h>
#include <sys/wait.h>

#define SU SIGUSR1

#define IGNORE 1
#define HANDLE 2
#define MASK 3
#define PENDING 4

int option = -1;

void check_pending(char* who) {
    sigset_t set;
    sigpending(&set);
    

    if (sigismember(&set, SU)) {
        printf("%s sees pending signal - SIGUSR1 - %d.\n", who, SU);
    } else {
        printf("%s does not see pending signal.\n", who);
    }
}  

void sig_handler(int sig){
    printf("\nI'm a signal handler, look at me dancing\n|-----------------------------------------------------------------------|\n|    o   \\ o /  _ o         __|    \\ /     |__        o _  \\ o /   o    |\n|   /|\\    |     /\\   ___\\o   \\o    |    o/    o/__   /\\     |    /|\\   |\n|   / \\   / \\   | \\  /)  |    ( \\  /o\\  / )    |  (\\  / |   / \\   / \\   |\n|-----------------------------------------------------------------------|\n\n");
}

int main(int argc, char** argv){
    if(argc != 3){
        printf("Wrong amount of arguments. Should be 3.\n");
        return 0;
    }else{
        char* mode = argv[1];
        int exec = -1;
        if(strcmp(argv[2], "exec") == 0){
            exec = 1;
        }else if (strcmp(argv[2], "exec") == 0){
            exec = 0;
        }
        // ignore, handler, mask lub pending,
        if(strcmp(mode, "ignore") == 0){
            signal(SU, SIG_IGN);
            option = IGNORE;
        }
        if(strcmp(mode, "handler") == 0){
            signal(SU, sig_handler);
            option = HANDLE;
        }
        if(strcmp(mode, "mask") == 0){
            sigset_t mask;

            sigemptyset(&mask);
            sigaddset(&mask, SU);

            sigprocmask(SIG_BLOCK, &mask, NULL);

            option = MASK;
        }
        if(strcmp(mode, "pending") == 0){
            sigset_t mask;

            sigemptyset(&mask);
            sigaddset(&mask, SU);

            sigprocmask(SIG_BLOCK, &mask, NULL);

            option = PENDING;
        }

        printf("Option: %d\n", option);

        raise(SU);


        if (option == IGNORE) {
        // Signal is ignored...
            printf("Signal is ignored in parent process\n");
        } else if (option == HANDLE) {
            // Parent handles signal...
            printf("Above me should be little guy showing off his sweet sexy moves.\n");
        } else if (option == MASK) {
            // Signal is blocked...
            printf("Signal is blocked - parent.\n");
        } else if (option == PENDING) {
            // Check if signal is pending
            check_pending("Parent");
        }



        int pid = fork();

        if (pid == 0) {

            if (exec == 1) {
                char num [10];
                sprintf(num, "%d", option);
                
                execl("./child", "./child", num, NULL);
            }

            if (option == PENDING) {
                check_pending("Child");
                return 0;
            }

            raise(SU);
            
            if (option == IGNORE) {
                printf("Signal is ignored in child fork process\n");
            } else if (option == HANDLE) {
                // Child process doesn't copy parent's handlers so 
                // the process terminates as soon as it receives SIGNAL.
                printf("Child is trying to make parent's friend(Steve) dance.");
            } else if (option == MASK) {
                // Signal is blocked... so the process does not terminate.
                printf("Signal is blocked - child.\n");
            } else if (option == MASK) {
                check_pending("Child fork");
            }

            return 0;
        }
        
        if (exec == 1) {
            wait(NULL);
        }

        return 0;
    }
}