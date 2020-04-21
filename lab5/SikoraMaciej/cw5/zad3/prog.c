#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    char* test_files [5] = { 
                            "test1.txt",
                            "test2.txt",
                            "test3.txt",
                            "test4.txt",
                            "test5.txt"
                            };
                        
    char* out_filename = "out.txt";
    char* pipe_filename = "pipe";

    mkfifo(pipe_filename, 0666);

    // Create 5 producers
    for (int i = 0; i < 5; i++) {
        if (fork() == 0) {
            int n = rand() % 10 + 3;
            char buff [3];
            sprintf(buff, "%d", n);
            char* args [5] = {"./producer", pipe_filename, test_files[i], buff, NULL};
            execvp("./producer", args);
        }
    }  

    // Run consumer
    if (fork() == 0) {
        int n = rand() % 10 + 3;
        char buff [3];
        sprintf(buff, "%d", n);

        char* args [5] = {"./consumer", pipe_filename, out_filename, buff, NULL};
        execvp("./consumer", args); 
    }

    for (int i = 0; i < 6; i++) {
        wait(NULL);
    }

    return 0;
}