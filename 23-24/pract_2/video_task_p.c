#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

void fgauss (int *, int *, int, int);

#define ejecuciones = {SEQ, PARALEL}
#define ejec PARALEL

int main(int argc, char *argv[]) {

   double ini,fin;
   ini = omp_get_wtime();
   FILE *in;
   FILE *out;
   int i, j, size, seq = 8;
   
   int **pixels, **filtered;

   if (argc == 2) seq = atoi (argv[1]);

   
//   chdir("/tmp");
   in = fopen("movie.in", "rb");
   if (in == NULL) {
      perror("movie.in");
      exit(EXIT_FAILURE);
   }
   

   out = fopen("movie_paralel.out", "wb");
   if (out == NULL) {
      perror("movie.out");
      exit(EXIT_FAILURE);
   }
   
   
   

   int width, height;

   fread(&width, sizeof(width), 1, in);
   fread(&height, sizeof(height), 1, in);

   fwrite(&width, sizeof(width), 1, out);
   fwrite(&height, sizeof(height), 1, out);

   pixels = (int **) malloc (seq * sizeof (int *));
   filtered = (int **) malloc (seq * sizeof (int *));
   #pragma omp parallel num_threads(seq) shared(pixels, filtered ,in, out, height, width, size) private (i)
   {
      
   #pragma omp for
   for (i=0; i<seq; i++)
   {
      pixels[i] = (int *) malloc((height+2) * (width+2) * sizeof(int));
      filtered[i] = (int *) malloc((height+2) * (width+2) * sizeof(int));
   }

   i = 0;
   
   
      #pragma omp single
      {
         do 
         {
            size = fread(pixels[i], (height+2) * (width+2) * sizeof(int), 1, in);

            if (size)
            {  
               #pragma omp task
                  {
                     fgauss (pixels[i], filtered[i], height, width);
                  }
               i++;
            }    
            if(i >=seq || size == 0){
               #pragma omp taskwait
               for (int j = 0;j<i;j++){
                  fwrite(filtered[j], (height+2) * (width + 2) * sizeof(int), 1, out);
               }
               i = 0;
            }     
         } while (!feof(in));
      }
   
   
   
   #pragma omp for
   for (i=0; i<seq; i++)
   {
      free (pixels[i]);
      free (filtered[i]);
   }

   free(pixels);
   free(filtered);

   fclose(out);
   fclose(in);
   }
   fin = omp_get_wtime();
   printf("tiempo:%f \n",fin-ini);
   return EXIT_SUCCESS;
}

void fgauss (int *pixels, int *filtered, int heigh, int width)
{
	int y, x, dx, dy;
	int filter[5][5] = {1, 4, 6, 4, 1, 4, 16, 26, 16, 4, 6, 26, 41, 26, 6, 4, 16, 26, 16, 4, 1, 4, 6, 4, 1};
	int sum;

	for (x = 0; x < width; x++) {
		for (y = 0; y < heigh; y++)
		{		
			sum = 0;
			for (dx = 0; dx < 5; dx++)
				for (dy = 0; dy < 5; dy++)
					if (((x+dx-2) >= 0) && ((x+dx-2) < width) && ((y+dy-2) >=0) && ((y+dy-2) < heigh))
						sum += pixels[(x+dx-2),(y+dy-2)] * filter[dx][dy];
			filtered[x*heigh+y] = (int) sum/273;
		}
	}
}

