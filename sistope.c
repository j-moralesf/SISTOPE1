#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <sys/wait.h>

#define DEF_LENGTH_SIZE 512
#define DEF_WORD_SIZE 1024

void takeValues(char* str, double* a, double* b, double* z){
    
    int c=0;
    int i=0;
    int flag = 0;
    char* num = (char*) malloc(sizeof(char)*15);
    while(str[c] != '\0'){
        if(str[c]== ',' || str[c] == ';'){
            double n = atof(num);
            if(flag == 0){
                *a = n;
            }
            else if(flag == 1){
                *b = n;
            }
            else if( flag == 2){
                *z = n;
                break;
            }
        
            memset(num, 0, 15);
            flag++;
            i=0;
            c++;
        }
        else{
            num[i]= str[c];
            i++;
            c++;
        }
    }
    free(num);
    
}

char* separarPorLinea(char* str){
    
    int c =0;
    
    while(str[c] != '\0' || str[c] != '\n'){

        if( str[c] == '\n'){
            int i=0;
            char* aux = (char*) malloc(sizeof(char)*(c+2));
            while(i < c){
                aux[i] = str[i];
                i++;
            }       
            aux[i]= '\0';
            return aux;
        }
        c++;
    }
    return NULL;
}

char* fixString(char* str){
    int c =0;
    while(str[c] != '\0' || str[c] != '\n'){
        if( str[c] == '\n'){
            int i=0;
            char* aux = (char*) malloc(sizeof(char)*DEF_WORD_SIZE);
            c++;
            while(str[c] != '\0'){
                aux[i] = str[c];
                i++;
                c++;
            }
            aux[i]='\0';
            return aux;
        }
        c++;
    }
    
    return NULL;
}


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

double calcularDistancia(double x, double y){
    return sqrt(x*x+y*y);
}

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

    FILE* aux = fopen("hijos.txt", "w");
    fprintf(aux,"pid=%d\n",getpid());
    fclose(aux);

    int opt=0, flag =0;
    char* in_fileName= (char*) malloc(sizeof(char)*DEF_LENGTH_SIZE);
    char* out_fileName= (char*) malloc(sizeof(char)*DEF_LENGTH_SIZE);
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

    /*
    child = firstChild;
    while(child != NULL){
        printf("rPadre=%d lHijo=%d lPadre=%d rHijo=%d\n", child->rPipe[0],child->rPipe[1],child->wPipe[1], child->wPipe[0]);
        child=child->sig;
    }
    */

    fp = fopen(in_fileName, "r");

    if(fp == NULL){
        printf("Error al abrir el archivo de visibilidades. Por favor, volver a iniciar con la opcion '-i nombre_archivo.csv '\n");
    }

    // PADRE

    if(pid !=0 ){
        int id;
        int c=0;

        double eje_u, eje_v, valor_real, valor_imaginario, ruido;

        while (fscanf(fp, "%lf,%lf,%lf,%lf,%lf", &eje_u, &eje_v, &valor_real, &valor_imaginario, &ruido) > 0){
            
            id=(int) calcularDistancia(eje_u, eje_v)/ancho_disco;

            if(id >= n_discos){
                id = n_discos-1;
            }

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

            printf("%d Se escribio en el hijo %d: %f %f %f\n",c, id,valor_real, valor_imaginario, ruido);
            c++;
        

        }
        printf("Se termino de leer visibilidades\n");
        child= firstChild;
        while(child != NULL){
            if(dup2(child->wPipe[1], STDOUT_FILENO) == -1){
                printf("Error en dup2\n");
            }
            printf("FINALIZAR\n");
            //waitpid(child->pid,&stat,0);
            child = child->sig;
        }


    }
    else{ // Es hijo

        /*

        int c=0;

        //printf("Soy el hijo: %d\n", getpid());
        

        while(1){

            // Se define un buffer y variables con las que se calculan los resultados
            
            char* inbuff = (char*) malloc(sizeof(char)*DEF_WORD_SIZE);

            double valor_real, valor_imaginario, ruido;

            // Lee pipe

            read(STDIN_FILENO, inbuff, sizeof(char)*DEF_WORD_SIZE);
            fflush(STDIN_FILENO);

            FILE* aux = fopen("hijos.txt", "a");
            
            int k = 0;
            int flag = 0;

            //fprintf(aux, "PID: %d Original inbuff:\n%s*\n", getpid(), inbuff);



            while(inbuff[0] != '\0'){

                char* auxS = separarPorLinea(inbuff);

                if(auxS == NULL){
                    //fprintf(aux, "Error NULL\n");
                    exit(1);
                }

                // Si llega la senal de fin del padre
                if(strcmp(auxS,"FINALIZAR")==0){
                    //fprintf(aux, "lectura fin\n");
                    flag = 1;
                    break;
                }

                //fprintf(aux, "auxS: %s*\n", auxS);

                inbuff = fixString(inbuff);
                //fprintf(aux, "PID: %d FixString inbuff:\n%s*\n", getpid(), inbuff);

                k++;
                
                takeValues(auxS, &valor_real, &valor_imaginario, &ruido);
                
                fprintf(aux,"%d Floats: %lf %lf %lf\n", getpid(), valor_real, valor_imaginario, ruido);
                
                free(auxS);
            }

            memset(inbuff, 0, DEF_WORD_SIZE);

            free(inbuff);


            // Si el padre manda la senal de fin
            if(flag==1){

                fprintf(aux, "FIN DEL %d\n", getpid());

                // Se calculan los valores finales del disco

               // double media_real, media_im, pot, ruido;


                break;
            }
            
            fclose(aux);
            c++;
        }
        */

        // EXECVE

    }

    fclose(fp);


    return 0;
}



























