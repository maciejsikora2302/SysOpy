#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h> 
#include <string.h>

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

int main(int argc, char** argv){
    if(argc != 2){
        printf("Wrong amount of arguments in child. Should be 2.\n");
        return 0;
    }else{
        int option = atoi(argv[1]);
        
        raise(SU);

        
        if (option == IGNORE) {
            // Signal is ignored...
            printf("Signal is ignored in child exec process\n");
        } else if (option == HANDLE) {
            // Parent handles signal...
            // Below printf will not execute since defult behaviour of SIGUSR1 ends program
            printf("Above me should be little guy showing off his sweet sexy moves. But I'm a child invoked using exec so I don't inherit handlers\n");
        } else if (option == MASK) {
            // Signal is blocked...
            printf("Signal is blocked - child.\n");
        } else if (option == PENDING) {
            // Check if signal is pending
            check_pending("Child exec");
        }
    }
    return 0;
}