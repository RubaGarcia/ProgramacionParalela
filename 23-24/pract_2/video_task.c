#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

void fgauss(int *, int *, int, int);

int main(int argc, char *argv[]) {

   FILE *in;
   FILE *out;
   int i, j, size, seq = 8;
   int **pixels, **filtered;

   if (argc == 2)
      seq = atoi(argv[1]);

   //   chdir("/tmp");
   in = fopen("movie.in", "rb");
   if (in == NULL) {
      perror("movie.in");
      exit(EXIT_FAILURE);
   }

   out = fopen("movie.out", "wb");
   if (out == NULL) {
      perror("movie.out");
      exit(EXIT_FAILURE);
   }

   int width, height;

   fread(&width, sizeof(width), 1, in);
   fread(&height, sizeof(height), 1, in);

   fwrite(&width, sizeof(width), 1, out);
   fwrite(&height, sizeof(height), 1, out);

   pixels = (int **)malloc(seq * sizeof(int *));
   filtered = (int **)malloc(seq * sizeof(int *));

   for (i = 0; i < seq; i++) {
      pixels[i] = (int *)malloc((height + 2) * (width + 2) * sizeof(int));
      filtered[i] = (int *)malloc((height + 2) * (width + 2) * sizeof(int));
   }
  
   

      //for loop to get the size data
      

      for (int i = 0; i < seq; i++) {
         size = fread(pixels[i], (height + 2) * (width + 2) * sizeof(int), 1, in);
         if (!size) {
            perror("fread");
            exit(EXIT_FAILURE);
         }
      }

      i = 0;
      #pragma omp parallel num_threads(seq) shared(pixels, filtered, in, out, height, width, size) private(i)
      {
         #pragma omp single
         {
            do {
               #pragma omp task depend(inout: pixels[i]) depend(out: filtered[i])
               {
                  fgauss(pixels[i], filtered[i], height, width);
               }
               i++;
            } while (!feof(in) && i < seq);
         }
      }

      for(int i = 0; i < seq; i++){
         fwrite(filtered[i], (height + 2) * (width + 2) * sizeof(int), 1, out);
      }

   for (i = 0; i < seq; i++) {
      free(pixels[i]);
      free(filtered[i]);
   }
   free(pixels);
   free(filtered);

   fclose(out);
   fclose(in);

   return EXIT_SUCCESS;
}

void fgauss(int *pixels, int *filtered, int heigh, int width) {
   int y, x, dx, dy;
   int filter[5][5] = {1, 4, 6, 4, 1, 4, 16, 26, 16, 4, 6, 26, 41, 26, 6, 4, 16, 26, 16, 4, 1, 4, 6, 4, 1};
   int sum;

   for (x = 0; x < width; x++) {
      for (y = 0; y < heigh; y++) {
         sum = 0;
         for (dx = 0; dx < 5; dx++)
            for (dy = 0; dy < 5; dy++)
               if (((x + dx - 2) >= 0) && ((x + dx - 2) < width) && ((y + dy - 2) >= 0) && ((y + dy - 2) < heigh))
                  sum += pixels[(x + dx - 2) * heigh + (y + dy - 2)] * filter[dx][dy];
         filtered[x * heigh + y] = (int)sum / 273;
      }
   }
}
