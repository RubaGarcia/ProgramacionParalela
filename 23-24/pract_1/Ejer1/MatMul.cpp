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
#include <float.h>
#include <thread>
#include <omp.h>

#define RAND rand() % 100
#define NTHREADS 4 //TODO cambiar en funcion del nivel de paralelismo


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
/************************************************************/

int main (int argc, char ** argv)
{
	int block_size = 1, dim=5;
	float *A, *B, *C;

/* Lectura de parámetros de entrada */
/* Usage: ./MatMul [dim] [block_size]*/
  if (argc == 3)
  {
    dim = atoi (argv[1]);
    block_size = atoi (argv[2]);
  }

  A = (float *) malloc (dim * dim * sizeof (float));
  B = (float *) malloc (dim * dim * sizeof (float));
  C = (float *) malloc (dim * dim * sizeof (float));
	
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
  Escribir_Matriz (C, dim);

  float max=Calcula_Maximo(C,dim);
  printf("El máximo es %f\n", max);

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

/**
 * Metodo que va a multiplicar las matrices de forma paralela, usando threads
 * nativos de cpp. 
 * Cada thread se encargará de usar una parte de la matriz equitativa
 * cada hilo se quedará con un fragmento de las matrices A y B y calculará
 * @param A matriz A
 * @param B matriz B
 * @param C matriz resultado
 * @param dim dimension de las matrices
*/

void Multiplicar_Matrices_Paralelo_cpp (float *A, float *B, float *C, int dim, int numHilo)
{


	int i, j, k;
	int elementos = dim/NTHREADS;
	int inicio = numHilo * elementos;
	
	//si la division de dim/NTHREADS no es entera hay que hacer que un hilo salve las iteraciones que faltan
	if((numHilo==NTHREADS-1)&& dim%NTHREADS != 0){
		elementos += dim%NTHREADS;
	}
	int fin = (numHilo + 1) * elementos;

	for (i=inicio; i < fin; i++)
		for (j=0; j < dim; j++)
			C[i*dim+j] = 0.0;

	for (i=inicio; i < fin; i++)
		for (j=0; j < dim; j++)
			for (k=0; k < dim; k++)
				C[i*dim+j] += A[i*dim+k] * B[j+k*dim];

	
}

/**
 * Metodo que va a multiplicar las matrices de forma paralela, usando threads de 
 * openMP cada thread se encargará de usar una parte de la matriz equitativa
 * @param A matriz A
 * @param B matriz B
 * @param C matriz resultado
 * @param dim dimension de las matrices
*/
void Multiplicar_Matrices_Paralelo_omp(float *A, float *B, float *C, int dim)
{

	int i, j, k;

	#pragma omp parallel num_threads(omp_get_max_threads()) shared(C, A, B) private (i, j, k)
	{
		#pragma omp for
		for (i=0; i < dim; i++)
				for (j=0; j < dim; j++)
					C[i*dim+j] = 0.0;

		#pragma omp for
		for (i=0; i < dim; i++)
			for (j=0; j < dim; j++)
				for (k=0; k < dim; k++)
					C[i*dim+j] += A[i*dim+k] * B[j+k*dim];

	}

	

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

float Calcula_Maximo(float *M, int dim)
{
  float max=FLT_MIN;
  for(int i=0;i<dim*dim;i++)
  {
    if(M[i]>max)
    {
      max=M[i];
    }
  }
  return max;
}
