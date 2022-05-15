#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>

#define MSGSIZE 128
#define DEF_SIZE 512



int main(){

    int fd[2];
    int fd2[2];
    
    if(pipe(fd) == -1){
        printf("Error\n");
    }
    if(pipe(fd2) == -1){
        printf("Error\n");
    }
    
    printf("%d %d\n", fd[0], fd[1]);
    printf("%d %d\n", fd2[0], fd2[1]);

    int*** array = (int***) malloc(sizeof(int**)*50);

    array[0][0] = fd;
    array[1][1] = fd2;

    printf("%d %d\n", array[0][0][0], array[1][1][0]);



}