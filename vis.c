
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

// Entradas: Un string de formato "num,num,num;"; 3 punteros de tipo double
// Salidas: No retorna nada. Modifica los datos de los punteros ingresados, almacenando los numeros que se obitenen del string
// Descripcion: Obtiene 3 valores tipo double de un string
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

// Entradas: Un string con saltos de linea
// Salidas: La primera linea del string
// Descripcion: Toma un string y retorna la primera linea de este
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

// Entradas: Un string con saltos de linea
// Salidas: El string sin la primera linea
// Descripcion: Le quita la primera linea a un string
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

// Entradas: (double, double) Parte real e imaginaria de una visibilidad
// Salidas: (double) Valor de potencia para los valores ingresados
// Descripcion: Calcula la propiedad de potencia
double potencia(double u, double v){
	double res = (u*u) + (v*v);
	return sqrt(res);
}

// Entradas: Numero n de visibilidades leidas; valor real, imaginario y ruido leidos; Punteros a los acumuladores de las sumas de valores reales, valores imaginarios, potencia y ruido
// Salidas: No tiene retorno (Se modifican los valores por referencia)
// Descripcion: Esta funcion toma los valores leidos del pipe y los suma al total
void sumarResultados(int* n, double valor_real, double valor_im, double valor_ruido, double* suma_real, double* suma_im, double* suma_pot, double* suma_ruido){
	*suma_real = *suma_real + valor_real;
	*suma_im = *suma_im + valor_im;
	*suma_ruido = *suma_ruido + valor_ruido;
	*suma_pot = *suma_pot + potencia(valor_real, valor_im);
	*n = *n + 1;
}

// Entradas: Numero n de visibilidades; sumas de valores reales e imaginarios; 2 Punteros tipo double.
// Salidas: No retorna nada. Modifica los valores almacenados en los punteros media_real y media_im.
// Descripcion: Calcula las medias aritmeticas de suma_real y suma_im.
void calcularMedias(int n, double suma_real, double suma_im, double* media_real, double* media_im){
    if (n != 0){
        *media_real = suma_real / n;
    	*media_im = suma_im / n;
    }
}

int main()
{
	int n = 0;
	double suma_real = 0, suma_im = 0, suma_pot = 0, suma_ruido = 0;
	double media_real = 0, media_im = 0;

    int c=0;

    while(1){

        // Se define un buffer y variables con las que se calculan los resultados
        
        char* inbuff = (char*) malloc(sizeof(char)*DEF_WORD_SIZE);

        double valor_real, valor_imaginario, ruido;

        // Lee pipe y lo almacena en un buffer

        read(STDIN_FILENO, inbuff, sizeof(char)*DEF_WORD_SIZE);
        fflush(STDIN_FILENO);
        
        int k = 0;
        int flag = 0;

        // Obtiene los datos del buffer

        while(inbuff[0] != '\0'){

            char* auxS = separarPorLinea(inbuff);

            if(auxS == NULL){
                exit(1);
            }

            // Si llega la senal de fin se deja de leer el buffer
            if(strcmp(auxS,"FINALIZAR")==0 || strcmp(auxS,"FINALIZAR;")==0){
                free(auxS);
                flag = 1;
                break;
            }

            inbuff = fixString(inbuff);

            k++;

            // Se suman los valores de visisbilidades al total
            
            takeValues(auxS, &valor_real, &valor_imaginario, &ruido);
            sumarResultados(&n, valor_real, valor_imaginario, ruido, &suma_real, &suma_im, &suma_pot, &suma_ruido);
            
            free(auxS);
        }

        memset(inbuff, 0, DEF_WORD_SIZE);

        free(inbuff);


        // Si llega la senal de fin
        // Se envian los resultados al pipe
        if(flag==1){

            if(n==0){
                // Si no hay visibilidades procesadas
                printf("Procese 0 visibilidades\nMedia real: ---\nMedia imaginaria: ---\nPotencia: ---\nRuido total: ---                          \n*");
            }
            else{
                // Si hay visibilidades procesadas
                
                // Se calculan los valores finales del disco

                calcularMedias(n, suma_real, suma_im, &media_real, &media_im);
                printf("Procese %d visibilidades\nMedia real: %lf\nMedia imaginaria: %lf\nPotencia: %lf\nRuido total: %lf\n*", n,media_real, media_im, suma_pot, suma_ruido);
            }

            // Se finaliza la lectura del pipe
            break;
        }
        
        c++;
    }

	return 0;
}






