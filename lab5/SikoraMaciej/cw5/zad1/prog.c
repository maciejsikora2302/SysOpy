#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int lines_in_file(char* filename){
    FILE* fp = fopen(filename, "r");
    char* line;
    size_t n = 0;
    int lines = 0;
    while( getline(&line, &n, fp) != -1 ){
        lines++;
    }
    return lines;
}

char** line_by_line(char* filename, int number_of_lines){
    char** lines = calloc(number_of_lines, sizeof(char*));
    FILE* fp = fopen(filename, "r");
    size_t n = 1000000;
    for(int i=0; i< number_of_lines;i++){
        getline(&lines[i], &n, fp);
    }
    return lines;
}


char** split(char* string, int* counter){
    char** words = calloc(10000, sizeof(char*));
    char * token = strtok(string, " ");
    // loop through the string to extract all other tokens
    int i=0;
    while( token != NULL ) {
    // printf( " %s\n", token ); //printing each token
        words[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    *counter = i;
    return words;
}


int get_number_of_pipes(char** words, int word_number){
    char* word = words[0];    
    int counter = 0;
    for(int i=0; i< word_number; i++){
        if(strcmp(word, "|") == 0){
            counter++;
        }
        word = words[i];
    }
    return counter;
}

char** subarr(char** arr, int start, int stop){
    void** sub = malloc(sizeof(void*) * (stop - start + 1));

    for (int i = start; i < stop; i++) 
        sub[i - stop] = arr[i];
    
    sub[stop - start] = NULL;

    return (char**)sub;
}


void run(char* line){
    //preprocess
    int word_counter = 0;
    char** words = split(line, &word_counter);
    int number_of_pipes = get_number_of_pipes(words, word_counter);

    //pipe_init
    int** pipes = malloc(sizeof(int*) * (number_of_pipes + 2));
    for (int i = 0; i < number_of_pipes + 2; i++) {
        pipes[i] = malloc(sizeof(int) * 2);
    }

    int first = 1;
    int prev_pipe[2];
    int next_pipe[2]; 


    int start = 0;
    for (int i = 0; i < word_counter; i++) {
        char* word = words[i];
        if (strcmp("|", word) == 0) { 
            pipe(next_pipe);
            if (fork() == 0) {
                //child executing program with arguments and managing his pipes
                char** args = subarr(words, start, i);
                char*  prog_name = args[0];
                
                if (first) {
                    dup2(next_pipe[1], 1);
                    close(next_pipe[0]);
                    first = 0;
                } else {
                    dup2(prev_pipe[0], 0);
                    dup2(next_pipe[1], 1);

                    close(prev_pipe[1]);
                    close(next_pipe[0]);
                }

                execvp(prog_name, args);
                exit(1);
            } else {
                //parent moves to next program
                if (first) {
                    first = 0;
                } else {
                    close(prev_pipe[0]);
                    close(prev_pipe[1]);
                }

                prev_pipe[0] = next_pipe[0];
                prev_pipe[1] = next_pipe[1];
                start = i + 1;
            }
        }
    }   



    dup2(prev_pipe[0], 0);
    close(prev_pipe[1]);
    char** args = (char**) subarr(words, start, word_counter);
    char*  prog_name = args[0];
    execvp(prog_name, args);

    exit(1);
}


int main(int argc, char* argv[]) { 
    if (argc < 2){
        printf("Not enaugh arguments, -> %d\n", argc);
    }
    else{
        char* filename = argv[1];   
        int line_count = lines_in_file(filename);
        printf("lines %d\n", line_count);

        char** lines = line_by_line(filename, line_count);
        for (int i=0; i<line_count;i++){
            printf("Line number %d: -> %s\n", i, lines[i]);
        }

        for(int i=0; i < line_count; i++){
            if (fork() == 0){
                run(lines[i]);
                return 0;
            }
        }


        printf("Main process starts to wait\n");
        //waiting for all children
        for (int i = 0; i < line_count; i++) 
            wait(NULL);

        printf("Aaaaand done.\n");

        return 0;
    }
}