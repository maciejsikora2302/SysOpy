#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define SHARED 0
#define SCATTERED 1


/*

./find './lista' 2 10 shared
scattered
#lista ilość_procesów_potomnych max_czas_życia_w_sekundach wariant_zapisu_wspolny_plik_albo_wiele_malych

*/


int main(int argc, char** argv){
    printf("Start mainpid: %d\n", getpid());
    pid_t pid1 = fork();
    printf("Mid pid: (pid1) %d,\t\t\t (getpid) %d,\t (ppid) %d\n", pid1, getpid(), getppid());
    pid_t pid2 = fork();
    printf("End pid: (pid1) %d,\t (pid2) %d,\t (getpid) %d,\t (ppid) %d\n", pid1, pid2, getpid(), getppid());

}