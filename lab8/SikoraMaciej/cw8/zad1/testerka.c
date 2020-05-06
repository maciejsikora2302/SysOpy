#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define REPORT_FILENAME "Times.txt"
#define TEST_FILE_PBM "gator.ascii.pgm"
#define TEST_OUTPUT  "output.txt"

int main() {
    char* options [3] = {"sign", "block", "interleaved"};
    int  threads [4] = {1, 2, 4, 8};
    printf("Testerka is testing hard.\n");

    int fd = open(REPORT_FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0666);
    write(fd, "Results of tests\n", strlen("Results of tests\n"));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            char* option = options[i];
            int m = threads[j];
            char buff [3];
            sprintf(buff, "%d", m);

            int pipefd[2];
            pipe(pipefd);
            if(fork() == 0) {
                close(pipefd[0]);    // close reading end in the child
                dup2(pipefd[1], 1);  // send stdout to the pipe
                dup2(pipefd[1], 2);  // send stderr to the pipe
                close(pipefd[1]);    // this descriptor is no longer needed

                char* args [6] = {"./main", buff, option, TEST_FILE_PBM, TEST_OUTPUT, NULL};
                execvp("./main", args); 
            } else {
                char buffer [1024];
                close(pipefd[1]);

                int readBytes;
                while ((readBytes = read(pipefd[0], buffer, sizeof(buffer))) != 0) {
                    buffer[readBytes + 1] = '\0';
                    write(fd, buffer, readBytes);
                }
                close(pipefd[0]);
            }
        }
    }
    printf("Testerka finished testing hard.\n");

    return 0;
}