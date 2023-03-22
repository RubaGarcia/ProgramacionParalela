#include <pthread.h>
#include <thread>
#include <stdio.h>
#include <cmath>
#include <mutex>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <string.h>

int sumaTotal = 0;
int Nhilos;//numeroHilos
int indiceR=1;

bool ultimoIndiceAtendido = false;
std::mutex g_m;


float time_diff(struct timeval *start, struct timeval *end) ;
void rap(int* array, int* arrayObj, int R, int N, int i);

/**
 * @param argc es el numero de parametros +1
 * @param argv son los parametros [1] es el tamaño y [2] es el numero de hilos 
*/
int main(int argc, char *argv[]){
    
    //toma de parametros y rellenado del array
    struct timeval start, end;

    gettimeofday(&start, NULL);
    int N = atoi(argv[1]);
    auto Nhilos = atoi(argv[2]);
    
    srand(time(NULL));


    int* array = (int*)malloc(N*sizeof(int));
    int* arrayObj = (int*)malloc(N*sizeof(int));
    //printf("%d\n", N);
    
    for(int i = 0; i<N; i++){
        //array[i] = 1;
        array[i] = rand();
    }

    int pedazos[Nhilos];   
    int pedazo = N/Nhilos;
    int sobrante = N%Nhilos;
    int i = Nhilos;
    while(i>0){
        pedazos[i-1] = pedazo;
        if(i==sobrante){
            pedazos[i-1]++;
            sobrante--;
        }
        i--;
    }
    
    std::thread threads[Nhilos];

    for(auto i = 0; i<Nhilos; i++){
        
        threads[i] = std::thread(rap, array, arrayObj, pedazos[i], Nhilos, i);   
    }
    rap(array, arrayObj, N, Nhilos, Nhilos);

    for(auto j = 0; j<Nhilos;j++){
        threads[j].join();
    }
    
    gettimeofday(&end, NULL);

    printf("%f, %d, %d\n", time_diff(&start, &end), N, Nhilos);
}

float time_diff(struct timeval *start, struct timeval *end) {
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}
/**
 * 
 * @param array array de lectura
 * @param arrayObj array objetivo
 * @param R tamanho del problema
 * @param N numero de hilos
 * @param i hilo actual
*/
void rap(int* array, int* arrayObj, int R, int N, int i){
    //printf("entra\n");
    int pedazo = R;
    int index = 0;
    int valorObtenido;
    int iteradorLectura;
    int iteradorEscr=0;
    

    while(index<pedazo){
        //printf("entra en while\n");
        valorObtenido=0;
        iteradorLectura=0;
        for(int j = 0; j< i+1; j++){
            valorObtenido += array[iteradorLectura];
            iteradorLectura++;
            
        }
        i++;
        //printf("hace un for\n");
        {
            std::lock_guard<std::mutex> guard(g_m);
            arrayObj[iteradorEscr] = valorObtenido; 
        }
        iteradorEscr++;
       index++;
    }
}