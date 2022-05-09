#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>

#define MSGSIZE 64

char* msg1 = "Hola padre! soy tu hijo";
char* msg2 = "No me hables mas";




int main(){

	char inbuff[MSGSIZE];
	int p1[2], p2[2], i;

	pid_t pid;
	int stat;

	int saved_stdout= dup(STDOUT_FILENO);

	if(pipe(p1) < 0){
		exit(1);
	}
	if(pipe(p2) < 0){
		exit(1);
	}


	if((pid= fork()) == -1){
		printf("error. Terminando programa\n");
	}
	if(pid == 0){
		close(p1[1]);
		close(p2[0]);
		
		if( (dup2(p1[0], STDIN_FILENO)) == -1){
			printf("Error en dup2\n");
			exit(1);
		}
		
		if(dup2(p2[1], STDOUT_FILENO) == -1){
			printf("Error en dup2\n");
			exit(1);
		}

		//El hijo escribe por p1[1] y lee por p2[0]
		write(p2[1], msg1, MSGSIZE);
		//printf("Soy el hijo pid =%d\n", pid);
	}
	else{
		//El padre escribe por p1[0] y lee por p2[1]
		close(p1[0]);
		close(p2[1]);
		
		
		if(dup2(p2[0], STDIN_FILENO) == -1){
			printf("Error en dup2\n");
			exit(1);
		};
		if( (dup2(p1[1], STDOUT_FILENO)) == -1){
			printf("Error en dup2\n");
			exit(1);
		}

		pid_t cpid = waitpid(pid, &stat, 0);

		FILE* fp = fopen("new.txt", "w");
		
		read(p2[0], inbuff, MSGSIZE);
		fprintf(fp, "%s\n manito sadsadsadsa pa lacasa\n", inbuff);

		fclose(fp);
	}

	if(pid != 0 && dup2(saved_stdout, p1[1]) != -1){
		printf("Proceso exitoso\n");
	}
	else{
		exit(1);
	}

	printf("Proceso de pid=%d terminado\n", pid);
	return 0;
}









