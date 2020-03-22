#include <stdio.h>

int main(){
    int n = 4;
    FILE* fp = fopen("bbb.txt", "w");
    // system("chmod -R 777 ./wynik.txt");
    if(fp == NULL){
        perror("fopen: ");
    }
    printf("File name: %s\n", "bbb.txt");
    // char* to_write = calloc(100, sizeof(char));
    // strcpy(to_write, "###### ");
    // char to_write = '#';
    // fprintf(fp, "###### ");
    printf("n == %d\n", n);
    for(int rep = 0; rep < n; rep++){
        for(int j = 0; j < n; j++){
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