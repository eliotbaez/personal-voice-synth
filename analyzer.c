#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <fftw3.h>
#include <string.h>
#include <stdlib.h>
#include <png.h>
#include "png_export.h"

/* possibly: */
//#include <zlib.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define PI 3.1415926535897932384626433832795
#define TAU 6.283185307179586476925286766559
/* 44100 samples = 1 second */

int main(int argc, char **argv) {

	int height, width;
	if (argc == 3) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	} else {
		fprintf(stderr, "usage: %s WIDTH HEIGHT\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (width < 1 || height < 1) {
		fprintf(stderr, "HEIGHT and WIDTH must be greater than 0\n");
		return EXIT_FAILURE;
	}

	fftw_complex *in, *out;
	fftw_plan p;

	/* constructors */
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * width);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * width);
	p = fftw_plan_dft_1d(width, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	/* populate the in array */
	for (int i = 0; i < width; ++i) {
		/* 1024 hz sample rate, 10hz signal */
		in[i][0] = sin(TAU /44100 * i * 1000);
		in[i][1] = 0.0;
	}
	
	fftw_execute(p);	/* where the fun happens */

	/* create an image */
	ImageBuf image = newImage(height, width);
	fillImageRGBA(image, 0xff, 0xff, 0xff, 0xff);
	
	/* and plot our data */
	for (int i = 0; i < image.width; ++i) {
		Pixel tmp = {0, 0, 0, 0xff};
		double y = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
		int py = (int)round(y);

		image.rowPtrs[height - 1][i] = tmp; /* x axis */
		/* fill all pixels below with red */
		tmp.r = 0xff;
		for (int row = height - 1 - min(py, height - 1); row < height - 1; ++row) {
			image.rowPtrs[row][i] = tmp;
		}
	}

	int r = export_png("example.png", image);

	destroyImage(image);
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);

	return r;
}