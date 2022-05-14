#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>

#define MSGSIZE 128
#define DEF_SIZE 512


typedef struct hijo{

    int wPipe[2]; //Pipe donde el hijo escribe (w) y el padre lee.
    int rPipe[2]; //Pipe donde el hijo lee (r) y el padre escribe.
    int pid;      //Pid del proceso.
    struct hijo* sig;
}hijo;


hijo* crearHijo(){

	hijo* newHijo= (hijo*) malloc(sizeof(hijo));
	newHijo->sig= NULL;

	return newHijo;
}

hijo* saveChild(hijo* child, int rBuff[2], int wBuff[2], int pid){

	child->wPipe[0] = wBuff[0];
	child->wPipe[1] = wBuff[1];
	child->rPipe[0] = rBuff[0];
	child->rPipe[1] = rBuff[1];
	child->pid = pid;

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


int main(){

	char inbuff[MSGSIZE];

	hijo* firstChild = NULL;

	pid_t pid;
	int stat;

	int saved_stdout= dup(STDOUT_FILENO);

	int k=0;


	char* msg1 = "Hola Mundo";

	while(k <20){

		int pipe1[2];
		int pipe2[2];


		if( pipe(pipe1)== -1 || pipe(pipe2) == -1){
			printf("Error al crear el Pipe\n");
			exit(1);
		}

		if((pid= fork()) == -1){
			printf("error. Terminando programa\n");
			exit(1);
		}
		if(pid == 0){
			close(pipe1[1]);
			close(pipe2[0]);
			
			if( (dup2(pipe1[0], fileno(stdin))) == -1){
				printf("Error en dup2\n");
				exit(1);
			}
			
			if(dup2(pipe2[1], fileno(stdout)) == -1){
				printf("Error en dup2\n");
				exit(1);
			}

			//Se printea en el Standard Out. En este caso STDOUT deberia ser igual al contenido de pipe2[1].
			printf("%d HIJO std_out=%d y std_in=%d\n",k, fileno(stdout), fileno(stdin));

			//read(STDIN_FILENO, inbuff, MSGSIZE);
			exit(1);
		}
		else if(pid != 0){
			//El padre escribe por pipe1[0] y lee por pipe2[1]
			close(pipe1[0]);
			close(pipe2[1]);
			
			if(dup2(pipe2[0], STDIN_FILENO) == -1){
				printf("Error en dup2\n");
				exit(1);
			};
			if( (dup2(pipe1[1], STDOUT_FILENO)) == -1){
				printf("Error en dup2\n");
				exit(1);
			}


			//printf();
			//pid_t cpid = waitpid(pid, &stat, 0);

			hijo* child = crearHijo();
			child = saveChild(child, pipe2, pipe1, pid);

			if(firstChild == NULL){
				firstChild = child;
			}
			else{
				firstChild=agregarHijo(firstChild, child);
			}
		}
		k++;
	}

	FILE* fp = fopen("salida.txt", "w");

	hijo* aux=firstChild;

	while(aux!= NULL){

		if(dup2(aux->rPipe[0], STDIN_FILENO) == -1){
				printf("Error en dup2\n");
				exit(1);
		};

		pid_t cpid= waitpid(aux->pid, &stat, 0);

		read(STDIN_FILENO, inbuff	, MSGSIZE);
		fprintf(fp, "%s pid=%d\nrPipe= %d %d wPipe= %d %d\n\n\n", inbuff, aux->pid, aux->rPipe[0], aux->rPipe[1],aux->wPipe[0], aux->wPipe[1]);
		aux=aux->sig;
	}

	fclose(fp);

	return 0;
}