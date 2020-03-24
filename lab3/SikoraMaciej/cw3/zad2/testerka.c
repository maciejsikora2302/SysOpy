#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int rnd_int(int min, int max){
    return rand()%(max-min + 1) + min;
}

int rnd_value(){
    return rand()%201 - 100;
}

void run_program(char* command){
    system("make compile --no-print-directory");
    system(command);
}

int compare_results(int rows_a, int columns_b, int matrix_c[][columns_b], char* name3, int number_of_workers, int available_time){
    if(number_of_workers == 0){
        printf("Worker number to low to complete task(%d)", number_of_workers);
        return -1;
    }
    if(available_time < 0){
        printf("Too low available time(%d)", available_time);
        return -1;
    }
    FILE* third_matrix = fopen(name3, "r");
    for(int row = 0; row < rows_a; row++){
        for(int column = 0; column < columns_b; column++){
            int value_from_file;
            fscanf(third_matrix, "%d", &value_from_file);
            printf("I have read value from file %d comparing to %d\n", value_from_file, matrix_c[row][column]);
            if(value_from_file != matrix_c[row][column]){
                return -1;
            }
        }
        // printf("\n");
    }
    fclose(third_matrix);
    return 1;
}

int main(int argc, char** argv){
    srand(time(NULL));
    if(argc == 4){
        int number_of_tests = atoi(argv[1]);
        int min = atoi(argv[2]);
        int max = atoi(argv[3]);


        for(int i=0;i<number_of_tests;i++){
            int rows_a = rnd_int(min, max);
            int columns_b = rnd_int(min, max);
            int shared_parameter = rnd_int(min, max);
            
            
            int matrix_a[rows_a][shared_parameter];
            int matrix_b[shared_parameter][columns_b];
            int matrix_c[rows_a][columns_b];

            char* name1 = calloc(20, sizeof(char));
            sprintf(name1, "matrix_a_%dx%d.txt", rows_a, shared_parameter);

            char* name2 = calloc(20, sizeof(char));
            sprintf(name2, "matrix_b_%dx%d.txt", shared_parameter, columns_b);

            char* name3 = calloc(20, sizeof(char));
            sprintf(name3, "matrix_c_%dx%d.txt", rows_a, columns_b);

            printf("Name1: %s\nName2: %s\nName3: %s\n", name1, name2, name3);


            FILE* first_matrix = fopen(name1, "w+");
            FILE* second_matrix = fopen(name2, "w+");

            for(int row = 0; row < rows_a;row++){
                for(int column = 0; column < shared_parameter; column++){
                    int val = rnd_value();
                    matrix_a[row][column] = val;
                    // printf("first_matrix[%d][%d]: %d\t", row, column, val);
                    if(fprintf(first_matrix, "%d ", val) < 0){
                        perror("fprintf");
                    }
                }
                // printf("\n");
                fprintf(first_matrix, "\n");
            }    

            for(int row = 0; row < shared_parameter;row++){
                for(int column = 0; column < columns_b; column++){
                    matrix_b[row][column] = rnd_value();
                    if(fprintf(second_matrix, "%d ", matrix_b[row][column]) <= 0){
                        perror("second matrix, fprintf:");
                    }
                    // printf("matrix_b %d\t", matrix_b[row][column]);
                }
                // printf("\n");
                if(fprintf(second_matrix, "\n")<0){
                    perror("newline:");
                }
            }



            fclose(first_matrix);
            fclose(second_matrix);
            // printf("\nsth2\n");
            for(int row = 0; row<rows_a;row++){
                for(int column = 0; column < columns_b; column++){
                    int sum = 0;
                    for(int iter = 0; iter < shared_parameter; iter++){
                        sum += matrix_a[row][iter] * matrix_b[iter][column];
                    }
                    matrix_c[row][column] = sum;
                }
            }

            
            // printf("\nsth3\n");
            FILE* lista = fopen("test_list.txt", "w+");
            

            // printf("fprintf lista: %d", );
            fprintf(lista, "%s %s %s ", name1, name2, name3);

            fclose(lista);

            char* command = calloc(100, sizeof(char));
            int passed_shared = 0;
            int passed_scattered = 0;
            int failed_shared = 0;
            int failed_scattered = 0;
            // printf("\nsth5\n");
            for(int number_of_workers = 2; number_of_workers <= 10; number_of_workers++){
                for(int avaliable_time = 1; avaliable_time <= 20; avaliable_time += 5){
                    for(int mode = 0; mode < 2; mode++){
                        char* mode_s;
                        if(mode == 0){
                            mode_s = "shared";
                        }else{
                            mode_s = "scattered";
                        }
                        sprintf(command, "./macierz './test_list.txt' %d %d %s", number_of_workers, avaliable_time, mode_s);
                        run_program(command);
                        if(compare_results(rows_a, columns_b, matrix_c, name3, number_of_workers, avaliable_time) == 1){
                            printf("Test passed: number of workers(%d), avaliable time(%d), mode_s(%s)\n", number_of_workers, avaliable_time, mode_s);
                            // msleep(1000);    
                            mode==0 ? passed_shared++ : passed_scattered++;
                        }else{
                            printf("Test failed: number of workers(%d), avaliable time(%d), mode_s(%s)\n", number_of_workers, avaliable_time, mode_s);
                            // return 0;
                            mode==0 ? failed_shared++ : failed_scattered++;
                            // failed++;
                        }
                    }
                    
                }
                
            }
            free(name1);
            free(name2);
            free(name3);
            printf("Tests shared passed:\t%d\nTests scattered passed:\t%d\nTests shared failed:\t%d\nTests scattered failed:\t%d\n", passed_shared, passed_scattered, failed_shared, failed_scattered);

        }
    }else{
        printf("Wrong amount of arguments. Got %d, expected 4\n", argc);
    }
}