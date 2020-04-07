#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char* argv[]) { 
    if (argc < 2){
        printf("Not enaugh arguments, -> %d\n", argc);
    }
    else{
        char* filename = argv[1];   

        if (fork() == 0){
            char* command = calloc(1000, sizeof(char));
            sprintf(command, "sort %s", filename);
            FILE* fd = popen(command, "r");
            char* lines = calloc(1000, sizeof(char));
            while(fgets(lines, 1000, fd)!=NULL){
                printf(lines);
            }
            pclose(fd);
            free(command);
            return 0;
        }

        printf("Main process starts to wait\n");
        
        wait(NULL);

        printf("Aaaaand done.\n");

        return 0;
    }
}