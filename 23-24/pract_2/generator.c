#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/*
 * SIZE: 1700 1250
 * TIME: +/-20 min
 *
 */

#define image(x,y) pixels[x*width+y]

int main(void) {
	srand(time(NULL));
	int width, height;
	int x, y, i, max;

	FILE *out;

//	chdir(TEMP);
	out = fopen("movie.in", "wb");
	if (out == NULL) {
		perror("movie.in");
		exit(EXIT_FAILURE);
	}

	width = rand() % 512 + 1536;
	height = rand() % 384 + 1152;
	width =  1920;
	height = 1440;

	fwrite(&width, sizeof(width), 1, out);
	fwrite(&height, sizeof(height), 1, out);
	fflush(out);

	int *pixels = (int*) malloc((height+2) * (width+2) * sizeof(int));

	max = 10;
	for (i = 0; i < max; i++) {
		for (y = 0; y <= height+1; y++) {
			for (x = 0; x <= width+1; x++) {
				if ((x == 0) || (x == width+1) || (y == 0) || (y == height+1)) image(y,x) = 0;
                                else image(y,x) = rand() % 256;
			}
		}
   fwrite(pixels, (height+2) * (width+2) * sizeof(int), 1, out);
   }

   free(pixels);
   fflush(out);
   fclose(out);

   return EXIT_SUCCESS;
}
