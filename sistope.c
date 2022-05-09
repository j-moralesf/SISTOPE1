#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#define DEF_LENGTH 50
#define DEF_WORD_SIZE 100


typedef struct hijo{

    int wPipe[2]; //Pipe donde el hijo escribe (w) y el padre lee.
    int rPipe[2]; //Pipe donde el hijo lee (r) y el padre escribe.
    int pid;      //Pid del proceso.
    hijo* sig;
}hijo;


void addChild(hijo** childArray, hijo* child){

    hijo* childArray;
    hijo* aux;

    while( aux->sig != NULL){
        aux= aux->sig;
    }
    aux->sig = child;
}

void comunicarProcesos(hijo** childArray){
    
    hijo newHijo;

    if (pipe(newHijo.wPipe) == -1){
        printf("No se pudo crear el pipe\n");
        exit(-1);
    }

    if(newHijo.pid = fork() == -1){
        printf("No se pudo crear el hijo\n");
        exit(-1);
    }
    if (newHijo.pid == 0){

        close(newHijo.wPipe[0]); //Cierra el lado de lectura
        close(newHijo.rPipe[1]); //Cierra el lado de escritura

        if(dup2(newHijo.wPipe[1], STDOUT_FILENO) == -1){
            printf("Error en dup2\n");
            exit(-1);
        }
        if(dup2(newHijo.rPipe[0], STDIN_FILENO) == -1){
            printf("Error en dup2\n");
            exit(-1);
        }
        execlp("ls", "ls", NULL);
    }
    else{
        close(newHijo.rPipe[0]); //Cierra el lado de lectura
        close(newHijo.wPipe[1]); //Cierra el lado de escritura

        if(dup2(newHijo.wPipe[0], STDIN_FILENO) == -1){
            printf("Error en dup2\n");
            exit(-1);
        }
        if(dup2(newHijo.rPipe[1], STDOUT_FILENO) == -1){
            printf("Error en dup2\n");
            exit(-1);
        }
        execlp("wc", "wc" , "-cwl", NULL);
    }
    addChild(childArray, *newHijo);
}


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

    //Se leen los parametros de entrada

    while((opt= getopt(argc, argv, "i:o:n:d:b"))){
        switch(opt){
            
            //Parametro leer archivo de visibilidades
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

            //Parametro de creacion de archivo de salida
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
            //Lectura de cantidad de discos
            case 'n':
                n_discos = atoi(optarg);
                printf("Cantidad de discos ingresada : %d\n", n_discos);
                break;
            //Lectura ancho de cada disci
            case 'd':
                ancho_disco = atoi(optarg);
                printf("Ancho de cada disco ingresada : %d\n", ancho_disco);
                break;
            //Flag
            case 'b':
                printf("Flag activada.\n");
                break;
        }
        if(opt == -1){
            printf("No se entregaron parametros para la ejecucion del programa.\n");
            break;
        }
    }
    
    //Crear hijos y pipes de comunicacion con ellos

    return 0;
}



























