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
#include "image_io.h"
#include "graphing.h"
#include "sound_io.h"
#include "windowing.h"


int main(int argc, char **argv) {
	int height, width, wfType;
	if (argc == 4) {
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		wfType = -1;
	} else if (argc == 5) {
		width = atoi(argv[2]);
		height = atoi(argv[3]);
		wfType = getWindowFunction(argv[4]);
	} else {
		fprintf(stderr, "usage: %s WAVFILE WIDTH HEIGHT [WINDOWFUNC]\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (width < 1 || height < 1) {
		fprintf(stderr, "HEIGHT and WIDTH must be greater than 0\n");
		return EXIT_FAILURE;
	}
	if (wfType < 0) {
		fprintf(stderr, "Defaulting to Rectangular window.\n");
		wfType = WF_RECTANGULAR;
	} else {
		fprintf(stderr, "Using %s window.\n", WINDOWFUNCTION_NAMES[wfType]);
	}

	WAVFile *wp = loadWAVFile(argv[1]);
	if (wp == NULL) {
		return EXIT_FAILURE;
	}
	{
		size_t totalSamples = wp->header.dataChunkSize / wp->header.totalBytesPerSample;
		if (totalSamples < width) {
			fprintf(stderr, "WAV file not long enough, choose a smaller sample size\n");
			fprintf(stderr, "Hint: This file contains %zd samples\n", totalSamples);
			return EXIT_FAILURE;
		}
	}

	fftw_complex *in, *out;
	fftw_plan p;

	/* constructors */
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * width);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * width);
	p = fftw_plan_dft_1d(width, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	/* populate the in array */
	meanDemuxSamples(wp->data, in, width,
		wp->header.channels, wp->header.bitsPerSample / 8);
	for (int i = 0; i < width; ++i) {
		in[i][1] = 0.0;
	}

	double *wf = malloc(sizeof(double) * width);
	generateNormalizedWindowFunction(width, wf, wfType);
	applyWindowFunction(in, wf, width);
	free(wf);

	fftw_execute(p);	/* where the fun happens */

	/* "normalize" the output */
	for (int i = 0; i < width; ++i) {
		out[i][0] *= 2.0 * height / width;
		out[i][1] *= 2.0 * height / width;
	}

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