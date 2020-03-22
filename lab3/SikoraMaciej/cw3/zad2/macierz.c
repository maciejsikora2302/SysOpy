#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define SHARED 0
#define SCATTERED 1
#define PARENT main_pid == getpid()
#define CHILD main_pid != getpid()

int MAX_SIZE;
/*

./find './lista' 2 10 shared
scattered
#lista ilość_procesów_potomnych max_czas_życia_w_sekundach wariant_zapisu_wspolny_plik_albo_wiele_malych

*/


// Założenie, że wynikowa macierz ma być kwadratowa (tak wynika z polecenia i przykładów)

void prepare_result_file(FILE* result, int n){
    char* to_write = calloc(7, sizeof(char));
    strcpy(to_write, "###### ");
    printf("n == %d\n", n);
    for(int rep = 0; rep < n; rep++){
        for(int j = 0; j < n; j++){
            // fwrite(to_write, 1, sizeof(to_write), result);
            fprintf(result, "%s", to_write);
            // printf("%s\n", to_write);
        }
        
        fprintf(result, "\n");
        // fwrite("\n", 1, sizeof(char), result);
    }
    // fseek(result, 0, 0);
    free(to_write);
}

int get_columns(FILE* second_matrix){
    char* c = calloc(100000, sizeof(char));
    int column_counter = 0;

    c = fgets(c, sizeof(c), second_matrix);
    for(int i=0; c[i] != '\0'; i++){
        if(c[i] == ' '){
            column_counter++;
        }
    }
    column_counter++;
    fseek(second_matrix, 0, 0);
    free(c);
    return column_counter;
}

int get_rows(FILE* second_matrix){
    // char* c = calloc(100000, sizeof(char));
    char c;
    int row_counter = 0;

    fseek(second_matrix, 0, SEEK_SET);
    while((( c = fgetc(second_matrix) )) != EOF){
        if(c == '\n') row_counter++;
    }
    row_counter++;
    fseek(second_matrix, 0, SEEK_SET);
    // free(c);
    return row_counter;
}

// int get_value_from_row(char* row, int* pos){
//     // int pos = 0;
//     char c = row[*pos];
//     char* to_int = calloc(10, sizeof(char));
//     while(c != ' '){
//         to_int[*pos] = c;
//         c = row[++*pos];
//     }
//     printf("to_int: %s\n", to_int);
//     int val_row = atoi(to_int);
//     free(to_int);
//     return val_row;
// }

void calculate_block(int start_column, int end_column, int columns, int rows, int matrixA[][rows], int matrixB[][columns], FILE* result_file){
    if(columns < end_column && start_column >= columns){
        printf("I tried to read columns that are too far\n");
        return;
    }else if (columns < end_column && start_column < columns){
        end_column = columns;
        printf("I had to change end_column to max_column\n");
    }
    printf("I got columns from %d to %d with rows %d\n", start_column, end_column, rows);
    
    int res_tab[end_column-start_column][columns];
    for(int i=0;i<end_column - start_column ; i++){
        for(int j=0; j<columns; j++){
            res_tab[i][j]=0;
        }
    }

    for(int column = 0; column < end_column-start_column; column++){
        for(int row = 0; row < columns; row++){

            for(int iter = 0; iter < rows; iter++){
                int val_a = matrixA[row][iter];
                int val_b = matrixB[iter][start_column + column];
                res_tab[column][row] += val_a * val_b;
                // printf("Val_a: %d, Val_b: %d, row: %d, column: %d, iter: %d, rows: %d, columns: %d\n", val_a, val_b, row, column, iter, rows, columns);
            }
            printf("Value [%d][%d] = %d\n", start_column + column, row, res_tab[column][row]);
            // printf("\n");
        }
        printf("\n");
    }
}

int main(int argc, char** argv){
    MAX_SIZE = 100;
    puts("");
    // return 0;
    if(argc == 5){
        char* path_to_list = argv[1];
        int number_of_children = atoi(argv[2]);
        int life_time = atoi(argv[3]);
        int save_mode;
        if(strcmp(argv[4], "shared") == 0){
            save_mode = SHARED;
        }else if (strcmp(argv[4], "scattered") == 0){
            save_mode = SCATTERED;
        }else{
            printf("Wrong mode: %s", argv[4]);
            return 0;
        }


        printf("Path to lista: %s\n", path_to_list);
        printf("Number of children: %d\n", number_of_children);
        printf("Life time in seconds: %d\n", life_time);
        printf("Mode (name, program definition): %s, %d\n", argv[4], save_mode);

        FILE* lista = fopen(path_to_list, "r");

        char c;
        char* first_matrix = (char*) calloc(1000, sizeof(char));
        char* second_matrix = (char*) calloc(1000, sizeof(char));
        char* result_file = (char*) calloc(1000, sizeof(char));
        strcpy(first_matrix, "./");
        strcpy(second_matrix, "./");
        strcpy(result_file, "./");
        int times = 0;
        while( ((c = fgetc(lista))) != EOF){
            int i=2;
            while(c != ' ' || c == '\0'){
                if(times == 0){
                    first_matrix[i++] = c;
                }else if(times == 1){
                    second_matrix[i++] = c;
                }else if(times == 2){
                    result_file[i++] = c;
                }
                c = fgetc(lista);
            }
            times++;
        }

        

        printf("First matrix: %s\n", first_matrix);
        printf("Second matrix: %s\n", second_matrix);
        printf("Result file: %s\n", result_file);
        // printf("\n");
        // return 0;

        int number_of_columnts_per_process = 2;

        

        FILE* first_matrix_file = fopen(first_matrix, "r");
        FILE* second_matrix_file = fopen(second_matrix, "r");
        FILE* result_file_file = fopen(result_file, "a");


        
        
        if(first_matrix_file == NULL || second_matrix_file == NULL || result_file_file == NULL){
            perror("fopen: ");
            return 0;
        }     

        int columns = get_columns(second_matrix_file);
        int rows = get_rows(second_matrix_file);

        printf("Columns: %d Rows: %d\n", columns, rows);

        int first_matrix_values[columns][rows];
        fseek(first_matrix_file, 0, SEEK_SET);
        for(int column = 0; column < columns; column++){
            int value;
            for(int row = 0; row < rows; row++){
                fscanf(first_matrix_file, "%d", &value);
                first_matrix_values[column][row] = value;
                printf("fmv: %d, row: %d, column: %d\n", first_matrix_values[column][row], row, column);
            }
            printf("\n");
        }

        int second_matrix_values[rows][columns];
        fseek(second_matrix_file, 0, SEEK_SET);
        for(int row = 0; row < rows; row++){
            int value;
            for(int column = 0; column < columns; column++){
                fscanf(second_matrix_file, "%d", &value);
                second_matrix_values[row][column] = value;
                printf("smv: %d, row: %d, column: %d\n", second_matrix_values[column][row], row, column);
            }
            printf("\n");
        }

        fclose(first_matrix_file);
        fclose(second_matrix_file);

        prepare_result_file(result_file_file, columns);

        fclose(result_file_file); //somehow forks were executing write commands to this file without invoking prepare_result_file

        FILE* result_file_file_2 = fopen(result_file, "a");

        pid_t main_pid = getpid();
        for(int k=0; k<number_of_children; k++){
            if(PARENT){
                fork();
            }
        }
        wait(NULL);

        // columns = 11;
        
        if(CHILD){
            int my_number = getpid() - main_pid - 1;
            for(int i = 0; i <= columns / (number_of_columnts_per_process*number_of_children); i++)
                calculate_block(number_of_children * number_of_columnts_per_process * i + my_number * number_of_columnts_per_process,
                            (number_of_children * number_of_columnts_per_process * i ) + number_of_columnts_per_process + my_number * number_of_columnts_per_process,
                            columns, rows, first_matrix_values, second_matrix_values, result_file_file_2
                );
        }
        
        if(PARENT){
            printf("I'm a parent with pid(%d) and number(%d)\n", getpid(), getpid()-main_pid);
            free(first_matrix);
            free(second_matrix);
            free(result_file);
            fclose(lista);
            // fclose(first_matrix_file);
            // fclose(second_matrix_file);
            fclose(result_file_file);

        }else{
            printf("I'm a child with pid(%d) and number(%d)\n", getpid(), getpid()-main_pid);
            return 0;
        }
        // printf("");

    }else{
        printf("Wrong amount of arguments: %d\n", argc);
        return 0;
    }
    puts("");
}