# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>
# include <omp.h>

int main ( void );
int explode ( double x, double y, int count_max );
int ppma_write ( char *output_filename, int xsize, int ysize, int *r, int *g, int *b );
int ppma_write_data ( FILE *file_out, int xsize, int ysize, int *r, int *g, int *b );
int ppma_write_header ( FILE *file_out, char *output_filename, int xsize, int ysize, int rgb_max );
void timestamp ( void );


#define TIEMPOS 1
int main ( void )
{
  int *b;
  int c;
  int c_max;
  int *count;
  int count_max = 1000;

   

char *filename = "mandelbrot_privs.ppm";
  
  
  int *g;
  int i;
  int ierror;
  int j;
  int k;
  int n = 8001;
  int *r;
  double x;
  double x_max =   1.25;
  double x_min = - 2.25;
  double y;
  double y_max =   1.75;
  double y_min = - 1.75;

  omp_lock_t mut;
  int CPrivada=0;
  int aux;


  double ini, fin_paralelo,fin_total;
  double tiempos[2];
  #pragma omp parallel num_threads(omp_get_max_threads()) firstprivate(i,j,x,y,c, CPrivada) shared(n,count,count_max,x_max,x_min,y_max,y_min,c_max,r,g,b,mut,aux)
  {
  #pragma omp single
  {
  timestamp ( );
  printf ( "\n" );
  printf ( "MANDELBROT\n" );
  printf ( "  C version\n" );
  printf ( "\n" );
  printf ( "  Create an ASCII PPM image of the Mandelbrot set.\n" );
  printf ( "\n" );
  printf ( "  For each point C = X + i*Y\n" );
  printf ( "  with X range [%f,%f]\n", x_min, x_max );
  printf ( "  and  Y range [%f,%f]\n", y_min, y_max );
  printf ( "  carry out %d iterations of the map\n", count_max );
  printf ( "  Z(n+1) = Z(n)^2 + C.\n" );
  printf ( "  If the iterates stay bounded (norm less than 2)\n" );
  printf ( "  then C is taken to be a member of the set.\n" );
  printf ( "\n" );
  printf ( "  An ASCII PPM image of the set is created using\n" );
  printf ( "    N = %d pixels in the X direction and\n", n );
  printf ( "    N = %d pixels in the Y direction.\n", n );

/*
  Carry out the iteration for each pixel, determining COUNT.
*/
  count = ( int * ) malloc ( n * n * sizeof ( int ) );
  }

  #ifdef TIEMPOS
    ini = omp_get_wtime();
  #endif
    
      #pragma omp for
      for ( i = 0; i < n; i++ )
      {
        for ( j = 0; j < n; j++ )
        {
          x = ( ( double ) (     j     ) * x_max
              + ( double ) ( n - j - 1 ) * x_min )
              / ( double ) ( n     - 1 );

          y = ( ( double ) (     i     ) * y_max
              + ( double ) ( n - i - 1 ) * y_min )
              / ( double ) ( n     - 1 );

          count[i+j*n] = explode ( x, y, count_max );
        }
      }
    
    
  
  
/*
  Determine the coloring of each pixel.
*/
  #pragma omp single
  {
    aux=omp_get_num_threads();
    c_max = 0;
  }
    
  #pragma omp for
    for ( j = 0; j < n; j++ )
    {
    int numeroThread = omp_get_thread_num();
      for ( i = 0; i < n; i++ )
      {    
        if ( CPrivada < count[i+j*n]) {
          CPrivada = count[i+j*n];
        }
	  }
	  
		#pragma omp critical
		{
			if (c_max < CPrivada) {
				c_max = CPrivada;  
			}
		}
	  
      
    }
  /*
    Set the image data.
  */
    #pragma omp single
    {
      
      r = ( int * ) malloc ( n * n * sizeof ( int ) );
      g = ( int * ) malloc ( n * n * sizeof ( int ) );
      b = ( int * ) malloc ( n * n * sizeof ( int ) );
    }
    
    #pragma omp for
    for ( i = 0; i < n; i++ )
    {
      for ( j = 0; j < n; j++ )
      {
        if ( count[i+j*n] % 2 == 1 )
        {
          r[i+j*n] = 255;
          g[i+j*n] = 255;
          b[i+j*n] = 255;
        }
        else
        {
          c = ( int ) ( 255.0 * sqrt ( sqrt ( sqrt (
            ( ( double ) ( count[i+j*n] ) / ( double ) ( c_max ) ) ) ) ) );
          r[i+j*n] = 3 * c / 5;
          g[i+j*n] = 3 * c / 5;
          b[i+j*n] = c;
        }
      }
    }
    #ifdef TIEMPOS
    #pragma omp single
    {
        fin_paralelo = omp_get_wtime();
        //printf("Tiempo: %f\n", fin_paralelo - ini);
        tiempos[0] = fin_paralelo - ini;
        
    }
    
    #endif
  /*
    Write an image file.
  */
    #pragma omp single
    {
      ierror = ppma_write ( filename, n, n, r, g, b );

      printf ( "\n" );
      printf ( "  ASCII PPM image data stored in \"%s\".\n", filename );

      free ( b );
      free ( count );
      free ( g );
      free ( r );

      /*
      Terminate.
      */
      printf ( "\n" );
      printf ( "MANDELBROT\n" );
      printf ( "  Normal end of execution.\n" );
      printf ( "\n" );
      timestamp ( );
    }
 
  }
  fin_total = omp_get_wtime();
    //printf("Tiempo: %f\n", fin_total - ini);
    tiempos[1]=fin_total-ini;
    
    printf("Tiempo paralelo:\t%f \nTiempo total:\t%f \n", tiempos[0], tiempos[1]);
  return 0;
}

int explode ( double x, double y, int count_max )
{
  int k;
  int value;
  double x1;
  double x2;
  double y1;
  double y2;

  value = 0;

  x1 = x;
  y1 = y;

  for ( k = 1; k <= count_max; k++ )
  {
    x2 = x1 * x1 - y1 * y1 + x;
    y2 = 2.0 * x1 * y1 + y;

    if ( x2 < -2.0 || 2.0 < x2 || y2 < -2.0 || 2.0 < y2 )
    {
      value = k;
      break;
    }
    x1 = x2;
    y1 = y2;
  }
  return value;
}

int ppma_write ( char *output_filename, int xsize, int ysize, int *r, int *g, int *b )
{
  int *b_index;
  int error;
  FILE *file_out;
  int *g_index;
  int i;
  int j;
  int *r_index;
  int rgb_max;
/*
  Open the output file.
*/
  file_out = fopen ( output_filename, "wt" );

  if ( !file_out )
  {
    printf ( "\n" );
    printf ( "PPMA_WRITE - Fatal error!\n" );
    printf ( "  Cannot open the output file \"%s\".\n", output_filename );
    return 1;
  }
/*
  Compute the maximum.
*/
  rgb_max = 0;
  r_index = r;
  g_index = g;
  b_index = b;

  for ( j = 0; j < ysize; j++ )
  {
    for ( i = 0; i < xsize; i++ )
    {
      if ( rgb_max < *r_index )
      {
        rgb_max = *r_index;
      }
      r_index = r_index + 1;

      if ( rgb_max < *g_index )
      {
        rgb_max = *g_index;
      }
      g_index = g_index + 1;

      if ( rgb_max < *b_index )
      {
        rgb_max = *b_index;
      }
      b_index = b_index + 1;
    }
  }
/*
  Write the header.
*/
  error = ppma_write_header ( file_out, output_filename, xsize, ysize, rgb_max );

  if ( error )
  {
    printf ( "\n" );
    printf ( "PPMA_WRITE - Fatal error!\n" );
    printf ( "  PPMA_WRITE_HEADER failed.\n" );
    return 1;
  }
/*
  Write the data.
*/
  error = ppma_write_data ( file_out, xsize, ysize, r, g, b );

  if ( error )
  {
    printf ( "\n" );
    printf ( "PPMA_WRITE - Fatal error!\n" );
    printf ( "  PPMA_WRITE_DATA failed.\n" );
    return 1;
  }
/*
  Close the file.
*/
  fclose ( file_out );

  return 0;
}

int ppma_write_data ( FILE *file_out, int xsize, int ysize, int *r, int *g, int *b )
{
  int *b_index;
  int *g_index;
  int i;
  int j;
  int *r_index;
  int rgb_num;

  r_index = r;
  g_index = g;
  b_index = b;
  rgb_num = 0;

  for ( j = 0; j < ysize; j++ )
  {
    for ( i = 0; i < xsize; i++ )
    {
      fprintf ( file_out, "%d  %d  %d", *r_index, *g_index, *b_index );
      rgb_num = rgb_num + 3;
      r_index = r_index + 1;
      g_index = g_index + 1;
      b_index = b_index + 1;

      if ( rgb_num % 12 == 0 || i == xsize - 1 || rgb_num == 3 * xsize * ysize )
      {
        fprintf ( file_out, "\n" );
      }
      else
      {
        fprintf ( file_out, " " );
      }
    }
  }
  return 0;
}

int ppma_write_header ( FILE *file_out, char *output_filename, int xsize, int ysize, int rgb_max )
{
  fprintf ( file_out, "P3\n" );
  fprintf ( file_out, "# %s created by ppma_write.c.\n", output_filename );
  fprintf ( file_out, "%d  %d\n", xsize, ysize );
  fprintf ( file_out, "%d\n", rgb_max );

  return 0;
}

void timestamp ( void )
{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  size_t len;
  time_t now;

  now = time ( NULL );
  tm = localtime ( &now );

  len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  printf ( "%s\n", time_buffer );

  return;
# undef TIME_SIZE
}
