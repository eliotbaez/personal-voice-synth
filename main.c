/* standard headers */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

/* extra libraries */
#include <fftw3.h>
#include <png.h>
/* possibly: */
//#include <zlib.h>

/* local headers */
#include "png_export.h"
#include "graphing.h"
#include "sound_io.h"

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
	WAVFile *wp = loadWAVFile("samples/sine_and_sawtooth.wav");
	for (int i = 0; i < width; ++i) {
		in[i][0] = ((int16_t*)(wp->data))[i] / 65536.0;
		in[i][1] = 0.0;
	}
	destroyWAVFile(wp);
	
	fftw_execute(p);	/* where the fun happens */

	/* create an image */
	ImageBuf image = newImage(height, width);
	fillImageRGBA(image, 0xff, 0xff, 0xff, 0xff);
	Pixel color = {200, 200, 200, 255};
	//drawGrid(image, 100, 100, color);
	
	/* and plot our data */
	color.r = 255; color.g = 0; color.b = 0;
	plotSpectrumAbsolute(image, image.width, out, color);
	image.width /= 2;
	int r = export_png("absolute.png", image);

	destroyImage(image);
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);

	return r;
}