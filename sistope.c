#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>

#define DEF_LENGTH 50
#define DEF_WORD_SIZE 100


typedef struct hijo{

    int wPipe[2]; //Pipe donde el hijo escribe (w) y el padre lee.
    int rPipe[2]; //Pipe donde el hijo lee (r) y el padre escribe.
    int pid;      //Pid del proceso.
    int indice;
    struct hijo* sig;
}hijo;


hijo* crearHijo(){

    hijo* newHijo= (hijo*) malloc(sizeof(hijo));
    newHijo->sig= NULL;

    return newHijo;
}

hijo* saveChild(hijo* child, int rBuff[2], int wBuff[2], int pid, int id){

    child->wPipe[0] = wBuff[0];
    child->wPipe[1] = wBuff[1];
    child->rPipe[0] = rBuff[0];
    child->rPipe[1] = rBuff[1];
    child->indice = id;

    return child;
}


hijo* agregarHijo(hijo* firstChild, hijo* child){

    hijo* aux = firstChild;

    if(firstChild == NULL){
        return child;
    }

    while( aux->sig != NULL){
        aux = aux ->sig;
    }

    aux->sig = child;

    return firstChild;
}

float calcularDistancia(float x, float y){
    return sqrt(x*x+y*y);
}

hijo* getChildFromId(hijo* lista, int id){

    hijo* aux = lista;

    while(aux!=NULL){
        if(aux->indice == id){
            return aux;
        }
        else{
            aux=aux->sig;
        }
    }
    return NULL;
}


int main(int argc, char** argv){

    int opt=0, flag =0;
    char* in_fileName= (char*) malloc(sizeof(char)*DEF_WORD_SIZE);
    char* out_fileName= (char*) malloc(sizeof(char)*DEF_WORD_SIZE);
    char linea[100];
    int c_linea, n_discos, ancho_disco;
    FILE* fp;
    int saved_stdout=dup(1);

    //Se leen los parametros de entrada

    while((opt= getopt(argc, argv, "i:o:n:d:b"))){
        switch(opt){
            //Parametro leer archivo de visibilidades
            case 'i':
                in_fileName = optarg;
                printf("%s\n", in_fileName);
            //Parametro de creacion de archivo de salida
            case 'o':

                out_fileName= optarg;

                break;
            //Lectura de cantidad de discos
            case 'n':
                n_discos = atoi(optarg);
                break;
            //Lectura ancho de cada disci
            case 'd':
                ancho_disco = atoi(optarg);
                break;
            //Flag
            case 'b':
                flag = 1;
                break;
        }
        if(opt == -1){
            printf("No se entregaron parametros para la ejecucion del programa.\n");
            break;
        }
    }
    //Crear hijos y pipes de comunicacion con ellos

    hijo* firstChild = NULL;

    pid_t pid;
    int stat, k=0;
    hijo* child;

    while(k <n_discos){

        printf("k=%d\n", k);

        int pipe1[2];
        int pipe2[2];


        if( pipe(pipe1)== -1 && pipe(pipe2) == -1){
            printf("Error al crear el Pipe\n");
            exit(1);
        }

        if((pid= fork()) == -1){
            printf("error. Terminando programa\n");
            exit(1);
        }

        //HIJO
        if(pid == 0){
            close(pipe1[1]);  //[1] -> Escritura
            close(pipe2[0]); // [0] -> Lectura
            
            if( (dup2(pipe1[0], STDIN_FILENO)) == -1){
                printf("Error en dup2\n");
                exit(1);
            }
            
            if(dup2(pipe2[1], STDOUT_FILENO) == -1){
                printf("Error en dup2\n");
                exit(1);
            }

            break;
        }
    
        //PADRE
        else if(pid != 0){
            //El padre escribe por pipe1[0] y lee por pipe2[1]

            child = crearHijo();
            child = saveChild(child, pipe2, pipe1, pid, k);

            if(firstChild == NULL){
                firstChild = child;
            }
            else{
                firstChild=agregarHijo(firstChild, child);
            }
        }
        k++;
    }

    fp = fopen(in_fileName, "r");

    if(fp == NULL){
        printf("Error al abrir el archivo de visibilidades. Por favor, volver a iniciar con la opcion '-i nombre_archivo.csv '\n");
    }

    if(pid !=0 ){

        int id;

        double eje_u, eje_v, valor_real, valor_imaginario, ruido;
        while (fscanf(fp, "%lf,%lf,%lf,%lf,%lf", &eje_u, &eje_v, &valor_real, &valor_imaginario, &ruido) != -1){
            id=(int) calcularDistancia(eje_u, eje_v)/ancho_disco;
            child= getChildFromId(firstChild, id);

            if(dup2(child->rPipe[0], STDIN_FILENO) == -1){
                printf("Error en dup2\n");
                exit(1);
            };
            if( (dup2(child->wPipe[1], STDOUT_FILENO)) == -1){
                printf("Error en dup2\n");
                exit(1);
            }

            printf("%f %f %f\n", valor_real, valor_imaginario, ruido);
            
            /*
            if(dup2(saved_stdout, STDOUT_FILENO) == -1){
                exit(1);
            }

            printf("Se escribio en el hijo%f %f %f\n", valor_real, valor_imaginario, ruido);
            */    
        }

    }
    else{ // Es hijo

        char* inbuff = (char*) malloc(sizeof(char)*DEF_WORD_SIZE);

        float valor_real, valor_imaginario, ruido;
        read(STDIN_FILENO, inbuff, sizeof(char)*DEF_WORD_SIZE);
        //fscanf(STDIN_FILENO, "%f %f %f", &valor_real, &valor_imaginario, &ruido);

        if(dup2(saved_stdout, STDOUT_FILENO) == -1){
            exit(1);
        }

        printf("Valores recibidos: %s\n", inbuff);


        FILE* aux = fopen("hijos.txt", "a");

        fprintf(aux, "%f %f %f\n", valor_real, valor_imaginario, ruido);
        
        fclose(aux);
        //exit(1);
    }

    fclose(fp);


    return 0;
}



























