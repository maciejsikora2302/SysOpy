
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>  

#define SLEEP_TIME 1

int main(int argc, char* argv[]) {
    if (argc < 4){
        printf("Not enough arguments, producer got %d arguments\n", argc);
    }else{
        char* pipe_filename = argv[1];
        char* filename = argv[2];
        int N = atoi(argv[3]);

        FILE* pipe = fopen(pipe_filename, "w");
        FILE* file   = fopen(filename, "r");

        char buffer [N + 1];
        int pid = getpid();
        
        for (
            int read = fread(buffer, sizeof(char), N, file);
            read != 0;
            read = fread(buffer, sizeof(char), N, file)
            ) {

            buffer[read] = '\0';

            fprintf(pipe, "#%d#%s\n", pid, buffer);
            sleep(SLEEP_TIME);
        }

        fclose(file);
        fclose(pipe);
    }

    return 0;
}