#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <sys/wait.h>

#define DEF_LENGTH_SIZE 512
#define DEF_WORD_SIZE 20480

typedef struct hijo{

    int wPipe[2]; //Pipe donde el hijo escribe (w) y el padre lee.
    int rPipe[2]; //Pipe donde el hijo lee (r) y el padre escribe.
    int pid;      //Pid del proceso.
    int indice;   //Disco al que pertenece el hijo
    struct hijo* sig;
}hijo;

// Entradas: No tiene.
// Salidas: Hijo creado
// Descripcion: Crea un nuevo hijo
hijo* crearHijo(){

    hijo* newHijo= (hijo*) malloc(sizeof(hijo));
    newHijo->sig= NULL;

    return newHijo;
}

// Entradas: Puntero a un hijo; 4 valores de tipo int
// Salidas: Puntero al hijo
// Descripcion: Modifica los valores de los miembros de un hijo
hijo* saveChild(hijo* child, int rBuff[2], int wBuff[2], int pid, int id){

    child->wPipe[0] = wBuff[0];
    child->wPipe[1] = wBuff[1];
    child->rPipe[0] = rBuff[0];
    child->rPipe[1] = rBuff[1];
    child->indice = id;

    return child;
}

// Entradas: Lista enlazada de hijos; hijo a agregar
// Salidas: Puntero al principio de la lista
// Descripcion: Agrega un hijo al final de una lista enlazada de hijos
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

// Entradas: Dos valores de tipo double
// Salidas: double correspondiente al valor de distancia.
// Descripcion: Calcula la distancia entre dos puntos
double calcularDistancia(double x, double y){
    return sqrt(x*x+y*y);
}

// Entradas: Una lista enlazada de hijos; valor de indice
// Salidas: Puntero a un hijo (hijo*)
// Descripcion: Encuentra un hijo en una lista de hijos, a partir del id
hijo* getChildFromId(hijo* lista, int id){

    hijo* aux = lista;

    while(aux!=NULL){
        if(aux->indice == id){
            return aux;
        }

        if(aux->indice <= id && aux->sig == NULL){
            return aux;
        }

        else{
            aux=aux->sig;
        }
    }
    return NULL;
}

int main(int argc, char** argv){

//    FILE* aux = fopen("hijos.txt", "w");
//    fprintf(aux,"pid=%d\n",getpid());
//    fclose(aux);

    int opt=0, flag =0;
    char* in_fileName= (char*) malloc(sizeof(char)*DEF_LENGTH_SIZE);
    char* out_fileName= (char*) malloc(sizeof(char)*DEF_LENGTH_SIZE);
    char linea[100];
    int c_linea, n_discos, ancho_disco;
    FILE* fp;
    int saved_stdout=dup(1);

    // Se leen los parametros de entrada

    while((opt= getopt(argc, argv, "i:o:n:d:b"))){
        switch(opt){
            // Parametro leer archivo de visibilidades
            case 'i':
                in_fileName = optarg;
                printf("%s\n", in_fileName);
            // Parametro de creacion de archivo de salida
            case 'o':

                out_fileName= optarg;

                break;
            // Lectura de cantidad de discos
            case 'n':
                n_discos = atoi(optarg);
                break;
            // Lectura ancho de cada disci
            case 'd':
                ancho_disco = atoi(optarg);
                break;
            // Flag
            case 'b':
                flag = 1;
                break;
        }
        if(opt == -1){
            printf("No se entregaron parametros para la ejecucion del programa.\n");
            break;
        }
    }

    // Crear hijos y pipes de comunicacion con ellos

    hijo* firstChild = NULL;

    pid_t pid;
    int stat, k=0;
    hijo* child;

    while(k <n_discos){

        printf("k=%d\n", k);

        int pipe1[2];
        int pipe2[2];


        if( pipe(pipe1)== -1){
            printf("Error al crear el Pipe\n");
            exit(1);
        }
        if(pipe(pipe2) == -1){
            printf("Error al crear el Pipe\n");
            exit(1);
        }

        if((pid= fork()) == -1){
            printf("error. Terminando programa\n");
            exit(1);
        }

        // HIJO
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
    
        // PADRE
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

    // Se abre el archivo de visibilidades en modo lectura

    fp = fopen(in_fileName, "r");

    if(fp == NULL){
        printf("Error al abrir el archivo de visibilidades. Por favor, volver a iniciar con la opcion '-i nombre_archivo.csv '\n");
    }

    // PADRE

    if(pid !=0 ){

        int id;
        int c=0;

        double eje_u, eje_v, valor_real, valor_imaginario, ruido;

        // El padre lee las visibilidades

        while (fscanf(fp, "%lf,%lf,%lf,%lf,%lf", &eje_u, &eje_v, &valor_real, &valor_imaginario, &ruido) > 0){
            
            double radio = calcularDistancia(eje_u, eje_v);
            id = (int) calcularDistancia(eje_u, eje_v)/ancho_disco;

            if(id >= n_discos){
                id = n_discos-1;
            }

            // Se envia la informacion al hijo correspndiente segun la distancia calculada

            child= getChildFromId(firstChild, id);

            if(dup2(child->rPipe[0], STDIN_FILENO) == -1){
                printf("Error en dup2\n");
                exit(1);
            }
            if(dup2(child->wPipe[1], STDOUT_FILENO) == -1){
                printf("Error en dup2\n");
                exit(1);
            }

            printf("%f,%f,%f;\n", valor_real, valor_imaginario, ruido);
            
            if(dup2(saved_stdout, STDOUT_FILENO) == -1){
                exit(1);
            }

//            printf("%d Se escribio en el hijo %d: %f %f %f %f\n",c, id, radio,valor_real, valor_imaginario, ruido);
            c++;
        

        }
//        printf("Se termino de leer visibilidades\n");

        // Cuando el padre termina de leer las visibilidades se envia un mensaje de FIN a los hijos

        child= firstChild;
        while(child != NULL){
            if(dup2(child->wPipe[1], STDOUT_FILENO) == -1){
                printf("Error en dup2\n");
            }
            printf("FINALIZAR\n");
            waitpid(child->pid,&stat,0);
            child = child->sig;
        }

        // Se reestablece el stdout

        if(dup2(saved_stdout, STDOUT_FILENO) == -1){
            exit(1);
        }

        // El padre lee cada pipe y se escribe el archivo de salida

        FILE* fsalida = fopen(out_fileName, "w");

        char* buffer = (char*) malloc(sizeof(char)*200);

        child= firstChild;
        while(child != NULL){
            if(dup2(child->rPipe[0], STDIN_FILENO) == -1){
                printf("Error en dup2\n");
            }
            read(STDIN_FILENO,buffer,sizeof(char)*200);
            fprintf(fsalida,"Disco %d:\n%s\n", child->indice + 1, buffer);
            child = child->sig;
        }

        fclose(fsalida);
        free(buffer);

    }
    else{ // Es hijo

        // Hijo realiza EXECVE para ejecutar el progrma vis

        if (execl("vis","vis",NULL) == -1){

//            FILE* aux = fopen("hijos.txt", "a");
//            fprintf(aux, "Error\n");
//            fclose(aux);
        }

    }

    fclose(fp);

    return 0;
}



























