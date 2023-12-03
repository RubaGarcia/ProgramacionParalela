/************************************************************/
/* Nombre:Ruben Garcia                                                  */
/* Práctica: 1                                                */
/* Fecha:                                                   */
/************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <sys/time.h>
#include <float.h>
#include <thread>
#include <omp.h>
#include <mutex>

#define RAND rand() % 100
int NTHREADS = 4; //TODO cambiar en funcion del nivel de paralelismo

std::mutex maxMutex;
float globalMax_in = FLT_MIN;
float globalMax_out = FLT_MIN;
bool ultimoIndiceAtendido = false;

/************************************************************/
void Init_Mat_Sup(int dim, float *M);
void Init_Mat_Inf(int dim, float *M);
void Multiplicar_Matrices(float *A, float *B, float *C, int dim);

void Multiplicar_Matrices_Paralelo_omp (float *A, float *B, float *C, int dim);
void Multiplicar_Matrices_Paralelo_cpp (float *A, float *B, float *C, int dim, int numHilo);

void Multiplicar_Matrices_Inf(float *A, float *B, float *C, int dim);
void Multiplicar_Matrices_Inf_Paralelo(float *A, float *B, float *C, int dim);
void Escribir_Matriz(float *M, int dim);
float Calcula_Maximo(float *M, int dim);


void Calcula_Maximo_par_out(float *M, int dim);
void Calcula_Maximo_par_in(float *M, int dim);

float time_diff(struct timeval *start, struct timeval *end);
/************************************************************/

int main (int argc, char ** argv)
{
	int block_size = 1, dim=5;
	float *A, *B, *C;

/* Lectura de parámetros de entrada */
/* Usage: ./MatMul [dim] [block_size] [num_threads]*/
  if (argc == 3)
  {
    dim = atoi (argv[1]);
    block_size = atoi (argv[2]);
	NTHREADS = atoi (argv[3]);
  }
  else
  {
	printf ("Usage: ./MatMul [dim] [block_size] [num_threads]\n");
	exit (0);
  }

  A = (float *) malloc (dim * dim * sizeof (float));
  B = (float *) malloc (dim * dim * sizeof (float));
  C = (float *) malloc (dim * dim * sizeof (float));
	
/*
  Init_Mat_Sup (dim, A);
  Init_Mat_Inf (dim, B);
  printf ("Matriz A\n");
  Escribir_Matriz (A, dim);
  printf ("Matriz B\n");
  Escribir_Matriz (B, dim);

  Multiplicar_Matrices (A, B, C, dim);
  printf ("Matriz Resultado correcto\n");
  Escribir_Matriz (C, dim);

  printf ("-------------------------------------------------\n");

  Init_Mat_Inf (dim, A);
  Init_Mat_Sup (dim, B);

  printf ("Matriz A\n");
  Escribir_Matriz (A, dim);
  printf ("Matriz B\n");
  Escribir_Matriz (B, dim);

	std::thread threads[NTHREADS];

    for(auto i = 0; i<NTHREADS; i++){
        
        threads[i] = std::thread(Multiplicar_Matrices_Paralelo_cpp, A, B, C, dim, i);   
    }
    Multiplicar_Matrices_Paralelo_cpp(A, B, C, dim, NTHREADS);

    for(auto j = 0; j<NTHREADS;j++){
        threads[j].join();
    }

  Multiplicar_Matrices (A, B, C, dim);
  printf ("Matriz Resultado correcto\n");
  Escribir_Matriz (C, dim);

  Multiplicar_Matrices_Inf (A, B, C, dim);
  printf ("Matriz Resultado Inferior\n");
  Escribir_Matriz (C, dim);*/

	
	

	std::thread threads[NTHREADS];
	struct timeval start_time, end_time;

	gettimeofday(&start_time, NULL);

	Calcula_Maximo_par_in(C, dim);

	gettimeofday(&end_time, NULL);

	float execution_time = time_diff(&start_time, &end_time);
	printf("Tiempo in %f \n", execution_time);

    //float max=Calcula_Maximo(C,dim);
    printf("El máximo es %f\n", globalMax_in);
  
	ultimoIndiceAtendido=false;
	struct timeval start_time_o, end_time_o;

	gettimeofday(&start_time_o, NULL);
    Calcula_Maximo_par_out(C, dim);

	gettimeofday(&end_time_o, NULL);

	execution_time = time_diff(&start_time_o, &end_time_o);
	printf("Tiempo out %f \n", execution_time);
  //float max=Calcula_Maximo(C,dim);
  printf("El máximo es %f\n", globalMax_out);
  exit (0);
}

void Init_Mat_Sup (int dim, float *M)
{
	int i,j,m,n,k;

	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			if (j <= i)
				M[i*dim+j] = 0.0;
			else
//				M[i*dim+j] = j+1;
				M[i*dim+j] = RAND;
		}
	}
}

void Init_Mat_Inf (int dim, float *M)
{
	int i,j,m,n,k;

	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			if (j >= i)
				M[i*dim+j] = 0.0;
			else
//				M[i*dim+j] = j+1;
				M[i*dim+j] = RAND;
		}
	}
}

void Multiplicar_Matrices (float *A, float *B, float *C, int dim)
{
	int i, j, k;

	for (i=0; i < dim; i++)
		for (j=0; j < dim; j++)
			C[i*dim+j] = 0.0;

	for (i=0; i < dim; i++)
		for (j=0; j < dim; j++)
			for (k=0; k < dim; k++)
				C[i*dim+j] += A[i*dim+k] * B[j+k*dim];
}



void Multiplicar_Matrices_Sup (float *A, float *B, float *C, int dim)
{
	int i, j, k;

	for (i=0; i < dim; i++)
		for (j=0; j < dim; j++)
			C[i*dim+j] = 0.0;

	for (i=0; i < (dim-1); i++)
		for (j=0; j < (dim-1); j++)
			for (k=(i+1); k < dim; k++)
				C[i*dim+j] += A[i*dim+k] * B[j+k*dim];
} 


void Multiplicar_Matrices_Inf (float *A, float *B, float *C, int dim)
{
	int i, j, k;

	for (i=0; i < dim; i++)
		for (j=0; j < dim; j++)
			C[i*dim+j] = 0.0;

	for (i=1; i < dim; i++)
		for (j=1; j < dim; j++)
			for (k=0; k < i; k++)
				C[i*dim+j] += A[i*dim+k] * B[j+k*dim];
}




void Escribir_Matriz (float *M, int dim)
{
	int i, j;

	for (i=0; i < dim; i++)
	{
		for (j=0; j < dim; j++)
			fprintf (stdout, "%.1f ", M[i*dim+j]);
		fprintf (stdout,"\n");
	}

   printf ("\n");
}


/**
 * Calcula el valor máximo en un array de tipo float M utilizando procesamiento paralelo.
 * 
 * @param M         Puntero al array de tipo float.
 * @param dim       Dimensión de la matriz cuadrada.
 * @param numHilo   Número de hilo.
 */
void Calcula_Maximo_par_in(float *M, int dim)
{
	float localMax=FLT_MIN;
	#pragma omp parallel private (localMax)
    {
        localMax=FLT_MIN;
        #pragma omp for
        for (int i=0;i<dim*dim;i++) {
            #pragma omp critical
            if (M[i] > globalMax_in) {
                globalMax_in = M[i];
            }
        }

        
    }
	
}

/**
 * @brief Calcula el maximo de una matriz de forma paralela utilizando una variable local parcial y actualizando la global fuera del bucle
 * 
 * @param M Matriz de entrada
 * @param dim Dimension de la matriz
 * @param numHilo Numero de hilo
 * @return void
 * 
*/
void Calcula_Maximo_par_out(float *M, int dim)
{
	
	float localMax=FLT_MIN;
	#pragma omp parallel private (localMax)
    {
        localMax=FLT_MIN;
        #pragma omp for
        for (int i=0;i<dim*dim;i++) {
            if (M[i] > localMax) {
                localMax = M[i];
            }
        }

        #pragma omp critical
        {
            if (localMax > globalMax_out) {
                globalMax_out = localMax;
            }
        }
    }
	
	
	

}

//create a function wich calculate an execution time
float time_diff(struct timeval *start, struct timeval *end) {
	return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}