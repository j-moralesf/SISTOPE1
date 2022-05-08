#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#define DEF_LENGTH 50
#define DEF_WORD_SIZE 100

char** initializeBuffer(int n){

    char** newBuffer = (char**) (malloc(sizeof(char*)*n));
    
    for(int i=0; i < n; i++){
        newBuffer[i]=(char*) (malloc(sizeof(char)*DEF_WORD_SIZE));
    }

    return newBuffer;
}

void rellenarBuffer(char** buffer, int n){
    for(int i=0; i < n ; i++){
        buffer[i]= "linea numero ";
    }
}


int main(int argc, char** argv){

    int opt=0;
    FILE* fp;
    char linea[100];
    int c_linea, n_discos, ancho_disco;

    char** buffer = initializeBuffer(DEF_LENGTH);
    rellenarBuffer(buffer, DEF_LENGTH);

    while((opt= getopt(argc, argv, "i:o:n:d:b"))){
        switch(opt){
            
            case 'i':
                fp= fopen(optarg, "r");

                if( fp == NULL){
                    printf("No se encuentra el archivo especificado.\n");
                    opt = -1;
                }
                  else{
                    printf("Abriendo el archivo especificado ...\n");
                }

                while(fscanf(fp, "%s", linea)!=EOF){
                   printf("%s\n", linea);
                }
                fclose(fp);
                break;


            case 'o':

                fp= fopen(optarg, "w");

                if( fp == NULL){
                    printf("Error en la creacion del archivo.\n");
                    opt = -1;
                }
                  else{
                    printf("Creando el archivo especificado ...\n");
                }

                c_linea =0;
                while(c_linea < DEF_LENGTH){
                    fprintf(fp, "%s %d\n", buffer[c_linea], c_linea+1);
                    c_linea++;
                }
                fclose(fp);
                break;

            case 'n':
                n_discos = atoi(optarg);
                printf("Cantidad de discos ingresada : %d\n", n_discos);
                break;
            case 'd':
                ancho_disco = atoi(optarg);
                printf("Ancho de cada disco ingresada : %d\n", ancho_disco);
                break;
            case 'b':
                printf("Flag activada.\n");
                break;
        }
        if(opt == -1){
            printf("Finalizando programa\n");
            break;
        }
    }
    return 0;
}