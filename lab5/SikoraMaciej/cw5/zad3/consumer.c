#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>  

#define SLEEP_TIME 1

int main(int argc, char* argv[]) { 
    if (argc < 4) {
        printf("Not enoughr arguments, consumer got %d arguments", argc);
    }else{
        char* pipeFilename = argv[1];
        char* filename = argv[2];
        int N = atoi(argv[3]);

        FILE* file = fopen(filename, "w");
        FILE* pipe = fopen(pipeFilename, "r");

        char buffer [N + 1];

        for (int read = fread(buffer, sizeof(char), N, pipe);
                    read != 0;
                    read = fread(buffer, sizeof(char), N, pipe)) {
            
            buffer[read] = '\0';
            fprintf(file, "%s", buffer);
        }
    }

    return 0;
}