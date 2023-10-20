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

std::mutex g_m;

float time_diff(struct timeval *start, struct timeval *end) ;
void suma(int* array, int indiceHilo,int N);


int main(int argc, char *argv[]){
    //dividimos el array en cachos a los que accede cada hilo
    //si hay 100 elementos y 10 hilos el hilo 1 accede a los elementos 0-9...
    //si no es una division exacta al primer hilo que acabe le asignaremos lo que quede del vector y lo acabará
    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);
    int N = atoi(argv[1]);
    Nhilos = atoi(argv[2]);
    srand(time(NULL));

    int* array = (int*)malloc(N*sizeof(int));
    
    
    for(int i = 0; i<N; i++){
        array[i] = rand();
    }

    std::thread threads[Nhilos];
    
    for(auto i=0;i<Nhilos;i++){
        threads[i] = std::thread(suma,array,i,N);
        if (i==Nhilos){
            suma(array, i+1,N);
        }
    }
    

    for(auto i = 0; i<Nhilos;i++){
        threads[i].join();
    }

    gettimeofday(&end, NULL);

    printf("%f, %d, %d\n", time_diff(&start, &end), N, Nhilos);
}

void suma(int* array, int indiceHilo,int N){

    int accesosHilo = trunc(N/Nhilos);//numero de elementos a los que accede el hilo
    int i=indiceHilo*accesosHilo;
    int aux = 0;
    while(i<(indiceHilo*accesosHilo+accesosHilo)){
        aux += array[i];
        i++;
    }
    
    if (indiceHilo == Nhilos && N%Nhilos != 0){
        int i=accesosHilo*(indiceHilo + 1);
        while(i<N){
            aux += array[i];
            i++;
        }
        
    }
 
    {
        std::lock_guard<std::mutex>guard(g_m);
        sumaTotal+=aux;
    }

    return;
}

float time_diff(struct timeval *start, struct timeval *end) {
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}
