#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h> 

// void info(char* msg){
//     printf("%s", msg);
// }

int wait_for_stp = 0;

void sigint_handler(int sig){
    system("echo \"\nKończę działanie\n\"");
    exit(0);
}

void sigtstp_handler(int sig){
    wait_for_stp = 1 - wait_for_stp;
    if(wait_for_stp){
        system("echo \"\nOczekuję na \nCTRL+Z - kontynuacja albo \nCTRL+C - zakończenie programu\n\"");
    }
    
}

int main(int argc, char** argv){
    signal(SIGINT, sigint_handler);

    struct sigaction act;

    act.sa_handler = sigtstp_handler;
    act.sa_flags   = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTSTP, &act, NULL);

    while(1){
        if(wait_for_stp == 0){
            system("ls");
            
        }else{
            system("echo \"Waiting...\"");
        }
        sleep(1);
    }
}