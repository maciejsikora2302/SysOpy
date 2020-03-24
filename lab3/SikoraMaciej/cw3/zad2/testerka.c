#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int rnd_int(int min, int max){
    return rand()%(max-min + 1) + min;
}

int rnd_value(){
    return rand()%201 - 100;
}

void run_program(char* command){
    system("make compile");
    system(command);
}

int compare_results(int rows_a, int columns_b, int matrix_c[][columns_b], char* name3, int number_of_workers, int available_time){
    FILE* third_matrix = fopen(name3, "r");
    int matrix_r[rows_a][columns_b];
    for(int row = 0; row < rows_a; row++){
        for(int column = 0; column < columns_b; column++){

        }
    }

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


            FILE* first_matrix = fopen(name1, "w");
            FILE* second_matrix = fopen(name2, "w");

            for(int row = 0; row<rows_a;row++){
                for(int column = 0; column < shared_parameter; column++){
                    matrix_a[row][column] = rnd_value();
                    fprintf(first_matrix, "%d ", matrix_a[row][column]);
                }
                fprintf(first_matrix, "\n");
            }

            for(int row = 0; row < shared_parameter;row++){
                for(int column = 0; column < columns_b; column++){
                    matrix_b[row][column] = rnd_value();
                    fprintf(second_matrix, "%d ", matrix_a[row][column]);
                }
                fprintf(first_matrix, "\n");
            }

            for(int row = 0; row<rows_a;row++){
                for(int column = 0; column < columns_b; column++){
                    int sum = 0;
                    for(int iter = 0; iter < shared_parameter; iter++){
                        sum += matrix_a[row][iter] * matrix_b[iter][column];
                    }
                    matrix_c[row][column] = sum;
                }
            }

            FILE* lista = fopen("test_list.txt", "w");
            
            fprintf(lista, "%s %s %s ", name1, name2, name3);

            char* command = calloc(100, sizeof(char));
            for(int number_of_workers = 1; number_of_workers < 30; number_of_workers++){
                for(int avaliable_time = 0; avaliable_time <= 10; avaliable_time += 5){
                    for(int mode = 0; mode < 2; mode++){
                        if(mode == 0){
                            sprintf(command, "./macierz './test_list.txt' %d %d %s", number_of_workers, avaliable_time, "shared");
                            run_program(command);
                        }else{
                            sprintf(command, "./macierz './test_list.txt' %d %d %s", number_of_workers, avaliable_time, "scattered");
                            run_program(command);
                        }
                        comapre_results(rows_a, columns_b, matrix_c, name3, number_of_workers, avaliable_time);
                    }
                    
                }
                
            }
            
            
        }
    }else{
        printf("Wrong amount of arguments. Got %d, expected 4\n", argc);
    }
}