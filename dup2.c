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
    int indice;
    struct hijo* sig;
}hijo;


hijo* crearHijo(){

	hijo* newHijo= (hijo*) malloc(sizeof(hijo));
	newHijo->sig= NULL;
	newHijo->indice = 0;

	return newHijo;
}

hijo* saveChild(hijo* child, int rBuff[2], int wBuff[2], int pid){

	child->wPipe[0] = wBuff[0];	// Lee Hijo
	child->wPipe[1] = wBuff[1]; // Escribe Padre
	child->rPipe[0] = rBuff[0]; // Lee Padre
	child->rPipe[1] = rBuff[1]; // Escribe Hijo
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
	int saved_stdout = dup(1);

	//printf("STDOUT = %d %d\n", saved_stdout, STDOUT_FILENO);


	int k=0;

	while(k <20){

		int pipe1[2];
		int pipe2[2];


		if( pipe(pipe1)== -1){
			printf("Error al crear el Pipe 1\n");
			exit(1);
		}
		if( pipe(pipe2)== -1){
			printf("Error al crear el Pipe 2\n");
			exit(1);
		}

		if((pid= fork()) == -1){
			printf("Error. Terminando programa\n");
			exit(1);
		}

		//HIJO
		if(pid == 0){
			close(pipe1[1]);  //[1] -> Escritura
			close(pipe2[0]); // [0] -> Lectura
			
			if( (dup2(pipe1[0], STDIN_FILENO)) == -1){
				printf("Error en dup2 1\n");
				exit(1);
			}
			
			if(dup2(pipe2[1], STDOUT_FILENO) == -1){
				printf("Error en dup2 2\n");
				exit(1);
			}

			//Se printea en el Standard Out. En este caso STDOUT deberia ser igual al contenido de pipe2[1].
			printf("%d HIJO std_out=%d y std_in=%d\tPADRE std_out=%d y std_in=%d",k, pipe2[1], pipe1[0], pipe1[1], pipe2[0]);
			exit(1);

		}
	
		//PADRE
		else if(pid != 0){
			//El padre escribe por pipe1[0] y lee por pipe2[1]
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
		}	
		if(dup2(aux->wPipe[1], STDOUT_FILENO) == -1){
			printf("Error en dup2\n");
			exit(1);
		}	

		//pid_t cpid= waitpid(aux->pid, &stat, 0);

		read(STDIN_FILENO, inbuff	, MSGSIZE);
		fprintf(fp, "%s pid=%d\n",inbuff, aux->pid);
		printf("pid=%d\t%d %d %d %d\t\n", aux->pid, aux->wPipe[0], aux->wPipe[1], aux->rPipe[0], aux->rPipe[1]);
		close(aux->rPipe[1]);
		close(aux->wPipe[0]);
		aux=aux->sig;

	}

	if(dup2(saved_stdout, 1) == -1){
		exit(1);
	}
	
	printf("probando\n");
	fclose(fp);

	return 0;
}
