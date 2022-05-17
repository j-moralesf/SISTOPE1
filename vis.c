
// cada hijo debe ejecutar execve

// leer pipe

// calcular formulas

// enviar datos por el pipe

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEF_WORD_SIZE 20480

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

double potencia(double u, double v){
	double res = (u*u) + (v*v);
	return sqrt(res);
}

void sumarResultados(int* n, double valor_real, double valor_im, double valor_ruido, double* suma_real, double* suma_im, double* suma_pot, double* suma_ruido){
	*suma_real = *suma_real + valor_real;
	*suma_im = *suma_im + valor_im;
	*suma_ruido = *suma_ruido + valor_ruido;
	*suma_pot = *suma_pot + potencia(valor_real, valor_im);
	*n = *n + 1;
}

void calcularMedias(int n, double suma_real, double suma_im, double* media_real, double* media_im){
	*media_real = suma_real / n;
	*media_im = suma_im / n;
}

int main()
{
	int n = 0;
	double suma_real = 0, suma_im = 0, suma_pot = 0, suma_ruido = 0;
	double media_real, media_im;

    int c=0;

    while(1){

        FILE* aux = fopen("hijos.txt", "a");
        fprintf(aux, "PID %d\n", getpid());

        // Se define un buffer y variables con las que se calculan los resultados
        
        char* inbuff = (char*) malloc(sizeof(char)*DEF_WORD_SIZE);

        double valor_real, valor_imaginario, ruido;

        // Lee pipe

        read(STDIN_FILENO, inbuff, sizeof(char)*DEF_WORD_SIZE);
        fflush(STDIN_FILENO);

        fprintf(aux, "1 *%s*\n",inbuff);
        
        int k = 0;
        int flag = 0;

        while(inbuff[0] != '\0'){

            char* auxS = separarPorLinea(inbuff);

            if(auxS == NULL){
                exit(1);
            }

            // Si llega la senal de fin del padre
            if(strcmp(auxS,"FINALIZAR")==0 || strcmp(auxS,"FINALIZAR;")==0){
                free(auxS);
                fprintf(aux, "senal ");
                flag = 1;
                break;
            }

            inbuff = fixString(inbuff);

            k++;
            
            takeValues(auxS, &valor_real, &valor_imaginario, &ruido);

            sumarResultados(&n, valor_real, valor_imaginario, ruido, &suma_real, &suma_im, &suma_pot, &suma_ruido);
            
            free(auxS);
        }

        fprintf(aux, "2 ");

        memset(inbuff, 0, DEF_WORD_SIZE);

        free(inbuff);


        // Si el padre manda la senal de fin
        if(flag==1){

            // Se calculan los valores finales del disco

            calcularMedias(n, suma_real, suma_im, &media_real, &media_im);

            // Se envian los resultados al pipe
            
            //fprintf(aux, "*n: %d\nMedia real: %lf\nMedia imaginaria: %lf\nPotencia: %lf\nRuido: %lf\n", n,media_real, media_im, suma_pot, suma_ruido);
            printf("*n: %d\nMedia real: %lf\nMedia imaginaria: %lf\nPotencia: %lf\nRuido total: %lf\n", n,media_real, media_im, suma_pot, suma_ruido);

            fprintf(aux,"*n: %d\nMedia real: %lf\nMedia imaginaria: %lf\nPotencia: %lf\nRuido total: %lf\n", n,media_real, media_im, suma_pot, suma_ruido);

            break;
        }

        fprintf(aux, "3 ");

        fclose(aux);
        
        c++;
    }

	return 0;
}






