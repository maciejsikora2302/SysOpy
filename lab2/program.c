#include <stdio.h>
#include <stdlib.h>
 
int main(int argc, char **argv) 
{
	// cout << "Nazwa programu:   " << argv[0]  << endl << "Ilosc argumentow: " << argc << endl;
	// for (int i = 1; i < argc; i++){
    //     // cout << "Argumentem nr " << i << " jest " << argv[i] << endl;
    //     printf("Argumenter nr %d jest %s\n", i, argv[i]);
    // }

    rename("test1.txt","test.txt"); 
    // char napis[200];
    char* napis = (char*) calloc(200, sizeof(char));
    FILE *plik=fopen("test.txt", "r"); 
    if(plik) { 
        fread(napis, 1, 200, plik); 
        printf("%s \n",napis);
        // printf("\n"); 
        // fread(napis, 1, 15, plik); 
        // printf("%s",napis);
        // printf("\n"); 
        // fwrite("Zdanie drugie.", 1, 14, plik); 
        // rename("test.txt","test1.txt"); 
        fclose(plik);
    } 
    free(napis);
    return 0;
}