#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <time.h>

#define SHARED 0
#define SCATTERED 1
#define PARENT main_pid == getpid()
#define CHILD main_pid != getpid()
#define FLOCK_LOCK flock(fileno(fd), LOCK_EX | LOCK_NB)
#define FLOCK_UNLOCK flock(fileno(fd), LOCK_UN)

int MAX_SIZE;
int MODE;
/*

./find './lista' 2 10 shared
scattered
#lista ilość_procesów_potomnych max_czas_życia_w_sekundach wariant_zapisu_wspolny_plik_albo_wiele_malych

*/


// Założenie, że wynikowa macierz ma być kwadratowa (tak wynika z polecenia i przykładów)

void prepare_result_file(char* result, int rows_a, int columns_b){
    FILE* fp = fopen(result, "w+");
    
    if(fp == NULL){
        perror("fopen: ");
    }
    // system("ls -l");
    printf("File name: %s\n", result);
    // char* to_write = calloc(100, sizeof(char));
    // strcpy(to_write, "###### ");
    // char to_write = '#';
    printf("rows_a = %d, columns_b = %d\n", rows_a, columns_b);
    for(int rep = 0; rep < rows_a; rep++){
        for(int j = 0; j < columns_b; j++){
            // fwrite(to_write, 1, sizeof(to_write), fp);
            fprintf(fp, "###### ");
            printf("###### ");
            // for(int i=0;i<6;i++)
            //     fprintf(fp, "%c", to_write);
            // fprintf(fp, "%c", ' ');
            // printf("%s\n", to_write);
        }
        
        fprintf(fp, "\n");
        printf("\n");
        // fwrite("\n", 1, sizeof(char), result);
    }
    // fseek(result, 0, 0);
    // free(to_write);
    fclose(fp);
}

int get_columns(FILE* second_matrix){
    // char* c = calloc(100000, sizeof(char));
    int column_counter = 0;
    // fgets(c, sizeof(c), second_matrix);
    char c;
    while( (c=fgetc(second_matrix)) != '\n'){
        if(c == ' ') column_counter++;
    }
    // for(int i=0; c[i] != '\0'; i++){
    //     if(c[i] == ' '){
    //         column_counter++;
    //     }
    // }
    column_counter++;
    fseek(second_matrix, 0, 0);
    // free(c);
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

void write_result_to_file(int start_column, int end_column, int rows_a, int columns_b, FILE* rf, int res_tab[][rows_a]){
    // printf("Resault table: \n");
    // for(int i=0;i<end_column-start_column;i++){
    //     for(int j=0;j<columns_b;j++){
    //         printf("t[%d][%d]: %d\t", j, i, res_tab[j][i]);
    //     }
    //     printf("\n");
    // }
    fseek(rf, 0, SEEK_SET);
    fseek(rf, start_column*7, SEEK_CUR);
    for(int row = 0; row < rows_a; row++){
        for(int column = 0; column < end_column - start_column; column++){
            fprintf(rf, "%d", res_tab[column][row]);

            char c;
            while(((c = fgetc(rf))) != ' '){
                // printf("%d", c);
                if(c==EOF) break;
            }
        }
        fseek(rf, (columns_b - (end_column - start_column))*7 + 1, SEEK_CUR);
    }
    
}


void write_using_multiple_files(int start_column, int end_column, int rows_a, int columns_b, int res_tab[][rows_a], char* my_file){
    FILE* rf = fopen(my_file, "w+");
    for(int row = 0; row < rows_a; row++){
        for(int column = 0; column < end_column - start_column; column++){
            fprintf(rf, "%d ", res_tab[column][row]);
            // char c;
            // while(((c = fgetc(rf))) != ' '){
            //     // printf("%d", c);
            //     if(c==EOF) break;
            // }
        }
        // fseek(rf, (columns_b - (end_column - start_column))*7 + 1, SEEK_CUR);
        fprintf(rf, "\n");
    }
    fclose(rf);
}


//rows_b must be equals to columns_a
//result is rows_a x columns_b
int calculate_block(int start_column, int end_column, int rows_a, int columns_b, int rows_b, int matrixA[][rows_b], int matrixB[][columns_b], char* result_file, char* my_file){
    if(columns_b < end_column && start_column >= columns_b){
        printf("I tried to read columns that are too far <-> start: %d end %d\n", start_column, end_column);
        return 0;
    }else if (columns_b < end_column && start_column < columns_b){
        end_column = columns_b;
        printf("I had to change end_column to max_column\n");
    }
    printf("I (%d) got columns_b from %d to %d with rows_b %d\n", getpid(), start_column, end_column, rows_b);
    // printf("matrixA:\n");
    // for(int i=0; i<rows_a;i++){
    //     for(int j=0;j<rows_b;j++){
    //         printf("row: %d, column: %d, val: %d \n", i, j, matrixA[i][j]);
    //     }
    //     printf("\n");
    // }
    
    int res_tab[end_column-start_column][rows_a];
    for(int i=0;i<end_column - start_column ; i++){
        for(int j=0; j<rows_a; j++){
            res_tab[i][j]=0;
        }
    }

    // for(int column = 0; column < end_column-start_column; column++){
    //     for(int row = 0; row < rows_a; row++){
    //         for(int iter = 0; iter < rows_b; iter++){
    //             int val_a = matrixA[row][iter];
    //             int val_b = matrixB[iter][start_column + column];
    //             res_tab[column][row] += val_a * val_b;
    //             printf("Val_a: %d, Val_b: %d, row: %d, column: %d, iter: %d, rows_b: %d, columns_b: %d\n", val_a, val_b, row, column, iter, rows_b, columns_b);
    //         }
    //         printf("Res_table[%d][%d] = %d\n", start_column + column, row, res_tab[column][row]);
    //         // printf("\n");
    //     }
    //     // printf("\n");
    // }
    for(int row = 0; row < rows_a; row++){
        for(int column = 0; column < end_column - start_column; column++){
            for(int iter = 0; iter < rows_b; iter++){
                int val_a = matrixA[row][iter];
                int val_b = matrixB[iter][start_column + column];
                res_tab[column][row] += val_a * val_b;
                // printf("Val_a: %d, Val_b: %d, row: %d, column: %d, iter: %d, rows_b: %d, columns_b: %d\n", val_a, val_b, row, column, iter, rows_b, columns_b);
            }
            // printf("Res_table[%d][%d] = %d\n", start_column + column, row, res_tab[column][row]);
        }
    }
    if(MODE == SHARED){
    FILE* fd = fopen("tmp.txt", "r+");
    int already_done = 0;
    // printf("pid: %d is trying to open file (%s)\n", getpid(), result_file);
    if(FLOCK_LOCK == 0){
        write_result_to_file(start_column, end_column, rows_a, columns_b, fd, res_tab);
        FLOCK_UNLOCK;
        already_done = 1;
    }
    while(already_done == 0){
        if(FLOCK_LOCK == 0){
            write_result_to_file(start_column, end_column, rows_a, columns_b, fd, res_tab);
            FLOCK_UNLOCK;
            already_done = 1;
        }
        while(FLOCK_LOCK == -1 && already_done == 0){
        // perror("flock");
        if(FLOCK_LOCK == 0){
            write_result_to_file(start_column, end_column, rows_a, columns_b, fd, res_tab);
            FLOCK_UNLOCK;
            already_done = 1;
            }
        }
    }
    fclose(fd);
    }else if(MODE == SCATTERED){
        write_using_multiple_files(start_column, end_column, rows_a, columns_b, res_tab, my_file);
    }
    
    return 1;
}

void clean_up_result_file(char* result_file){
    FILE* tmp = fopen("tmp.txt", "r");
    FILE* fd = fopen(result_file, "w+");
    char c;
    while((c = fgetc(tmp)) != EOF){
        if(c != '#'){
            // printf("%c", c);
            // fseek(fd, -1, SEEK_CUR);
            fprintf(fd, "%c", c);
        }
    }
    fclose(tmp);
    fclose(fd);
}

int main(int argc, char** argv){
    MAX_SIZE = 100;
    puts("");
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

        MODE = save_mode;

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
        fclose(lista);

        printf("\nFirst matrix: %s\n", first_matrix);
        printf("Second matrix: %s\n", second_matrix);
        printf("Result file: %s\n", result_file);
        // printf("\n");
        // return 0;
        // system("touch wyniki.txt");
        // system("chmod 777 wynik.txt");

        int number_of_columns_per_process = 3;

        FILE* first_matrix_file = fopen(first_matrix, "r");
        FILE* second_matrix_file = fopen(second_matrix, "r");

        if(first_matrix_file == NULL || second_matrix_file == NULL){
            perror("fopen: ");
            return 0;
        }     

        int rows_a = get_rows(first_matrix_file);
        int columns = get_columns(second_matrix_file);
        int rows_b = get_rows(second_matrix_file);
        

        printf("Columns: %d Rows_a: %d, Rows_b: %d\n", columns, rows_a, rows_b);

        int first_matrix_values[rows_a][rows_b];
        fseek(first_matrix_file, 0, SEEK_SET);
        for(int row = 0; row < rows_a; row++){
            int value;
            for(int column = 0; column < rows_b; column++){
                fscanf(first_matrix_file, "%d", &value);
                first_matrix_values[row][column] = value;
                // printf("fmv: %d, row: %d, column: %d\t", first_matrix_values[row][column], row, column);
            }
            // printf("\n");
        }

        printf("\n");

        int second_matrix_values[rows_b][columns];
        fseek(second_matrix_file, 0, SEEK_SET);
        for(int row = 0; row < rows_b; row++){
            int value;
            for(int column = 0; column < columns; column++){
                fscanf(second_matrix_file, "%d", &value);
                second_matrix_values[row][column] = value;
                // printf("smv: %d, row: %d, column: %d\t", second_matrix_values[row][column], row, column);
            }
            // printf("\n");
        }

        fclose(first_matrix_file);
        fclose(second_matrix_file);

        prepare_result_file("tmp.txt", rows_a, columns);

        // fclose(resu); //somehow forks were executing write commands to this file without invoking prepare_result_file

        // int number_of_multiplications[number_of_children];
        // for(int i=0;i<number_of_children;i++){
        //     number_of_multiplications[i] = 0;
        // }
        time_t start = time(NULL);
        pid_t main_pid = getpid();
        for(int k=0; k<number_of_children; k++){
            if(PARENT){
                fork();
            }
        }
        

        // columns = 11;
        int nom=0;
        if(CHILD){
            int my_number = getpid() - main_pid - 1;
            char* my_file = calloc(100, sizeof(char));
            for(int i = 0; i <= columns / (number_of_columns_per_process*number_of_children); i++){
                time_t now = time(NULL);
                if(now - start > life_time){
                    printf("I lived longer that I could. My age: %ld, Number of multiplications: %d\n", now-start, nom);
                    return 0;
                    free(my_file);
                }
                if(MODE == SCATTERED){
                    sprintf(my_file, "scattered%d.txt", number_of_children * number_of_columns_per_process * i + my_number * number_of_columns_per_process);
                    printf("Pid: %d, my_number:%d, my_file: %s\n", getpid(), my_number, my_file);
                }
                nom += calculate_block(number_of_children * number_of_columns_per_process * i + my_number * number_of_columns_per_process,
                            (number_of_children * number_of_columns_per_process * i ) + number_of_columns_per_process + my_number * number_of_columns_per_process,
                            rows_a, columns, rows_b, first_matrix_values, second_matrix_values, result_file, my_file
                );
                

                // printf("asd");
                // number_of_multiplications[my_number-1]+=1;
                // printf("%d ", number_of_multiplications[my_number-1]);
            }
            free(my_file);
            // execlp("echo", "echo", "I'm done");
        }
        
        if(PARENT){
            wait(NULL);
            printf("I'm a parent with pid(%d) and number(%d)\n", getpid(), getpid()-main_pid);
            free(first_matrix);
            free(second_matrix);
            if(MODE == SHARED){
                clean_up_result_file(result_file);
                system("rm tmp.txt");
            }
            if(MODE == SCATTERED){
                char* command = calloc(100, sizeof(char));
                sprintf(command, "paste -d '' scattered* > %s", result_file);
                system(command);
                free(command);
                system("rm -f scattered*");
            }
            free(result_file);
        }else{
            printf("I'm a child with pid(%d) and number(%d) and I did %d multiplications\n", getpid(), getpid()-main_pid, nom);
            return 0;
        }
        // printf("");

    }else{
        printf("Wrong amount of arguments: %d\n", argc);
        return 0;
    }
    puts("");
}