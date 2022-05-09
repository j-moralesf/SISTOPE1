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
    struct hijo* sig;
}hijo;


void addChild(hijo** childArray, hijo* child){

    hijo* aux;

    while( aux->sig != NULL){
        aux= aux->sig;
    }
    aux->sig = child;
}

int comunicarProcesos(hijo** childArray, hijo* newHijo){
    
    if (pipe(newHijo->wPipe) == -1){
        printf("No se pudo crear el pipe\n");
        exit(-1);
    }

    if(newHijo->pid = fork() == -1){
        printf("No se pudo crear el hijo\n");
        exit(-1);
    }

    int aPid = newHijo->pid;
    printf("pid= %d\n", newHijo->pid);
    if (newHijo->pid == 0){
        printf("Soy el hijo\n");

        close(newHijo->wPipe[0]); //Cierra el lado de lectura
        close(newHijo->rPipe[1]); //Cierra el lado de escritura

        if(dup2(newHijo->wPipe[1], STDOUT_FILENO) == -1){
            printf("Error en dup2\n");
            exit(-1);
        }
        if(dup2(newHijo->rPipe[0], STDIN_FILENO) == -1){
            printf("Error en dup2\n");
            exit(-1);
        }
        execlp("ls", "ls", NULL);
    }
    else{
        printf("Soy el padre\n");
        close(newHijo->rPipe[0]); //Cierra el lado de lectura
        close(newHijo->wPipe[1]); //Cierra el lado de escritura

        if(dup2(newHijo->wPipe[0], STDIN_FILENO) == -1){
            printf("Error en dup2\n");
            exit(-1);
        }
        //if(dup2(newHijo->rPipe[1], STDOUT_FILENO) == -1){
        //    printf("Error en dup2\n");
        //    exit(-1);
        //}
        execlp("wc", "wc" , "-cwl", NULL);
    }
    addChild(childArray, newHijo);
}


hijo** initializeArray(){

    hijo** array = (hijo**) malloc(sizeof(hijo*)*DEF_LENGTH);

    return array;
}

hijo* initializeHijo(){

    hijo* newHijo = (hijo*) malloc(sizeof(hijo));
    newHijo->sig=NULL;
    return newHijo;
}


int main(){


    hijo** array= initializeArray();
    int c=0;

    do{
        hijo* child = initializeHijo();
        comunicarProcesos(array, child);
        c++;
        if (child->pid == 0){
            break;
        }
    }while(c < 10);


    return 0;
}


