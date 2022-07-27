#define _USE_MATH_DEFINES /* for math constants */
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
#include "spectrogram.h"

double decibelsOfNormalized(double a) {
	/* equivalent to 10 * log_10(a) but more computer friendly */
	return log2(a) * 20.0 * M_LN2 / M_LN10;
}

/* this is primarily for debugging */
Pixel thresholdBlack(fftw_complex z) {
	Pixel color;
	uint8_t value;

	double magnitude = hypot(z[0], z[1]);
	if (magnitude > 0) {
		value = 0;
	} else {
		value = 255;
	}

	color.r = color.g = color.b = value;
	color.a = 255;
	return color;
}

Pixel colorFuncDecibelBlackToWhite(fftw_complex z) {
	Pixel color;
	uint8_t value;
	/* roughly the smallest decibel level we could have, assuming a
	   normalized amplitude of 1/32768 (normalized meaning that Full
	   Signal = 1) */
	double const noisefloor = 90.0;

	/* 0 should be white, we can make -noisefloor be black */
	double dBFS = decibelsOfNormalized(hypot(z[0], z[1]));
	if (dBFS > 0.0) {
		value = 255;
	} else if (dBFS < -noisefloor) {
		value = 0;
	} else {
		/* value in range :D */
		value = (uint8_t)round((dBFS + noisefloor) / noisefloor * 255.0);
	}

	color.r = color.g = color.b = value;
	color.a = 255;
	return color;
}

Pixel colorFuncWhiteToBlack(fftw_complex z) {
	Pixel color;
	uint8_t value; /* brightness value */

	/* we assume all these numbers have been "normalized" (not really
	   the correct term) to between 0 and 1 in magnitude. We will still
	   check the bounds, however. */
	double magnitude = hypot(z[0], z[1]);
	if (magnitude <= 1.0) {
		value = 255 - (uint8_t) round(255 * magnitude);
	} else {
		value = 0;
	}

	color.r = color.g = color.b = value;
	color.a = 255;
	return color;
}

Pixel colorFuncBlackToWhite(fftw_complex z) {
	Pixel color;
	uint8_t value; /* brightness value */

	/* we assume all these numbers have been "normalized" (not really
	   the correct term) to between 0 and 1 in magnitude. We will still
	   check the bounds, however. */
	double magnitude = hypot(z[0], z[1]);
	if (magnitude <= 1.0) {
		value = (uint8_t) round(255 * magnitude);
	} else {
		value = 255;
	}

	color.r = color.g = color.b = value;
	color.a = 255;
	return color;
}

ImageBuf createSpectrogram(const WAVFile *wp, int samplesPerFrame, Pixel (*colorFunc)(fftw_complex)) {
	ImageBuf image = {
		.height = 0,
		.width = 0,
		.pixels = NULL,
		.rowPtrs = NULL
	};
	fftw_complex *in, *out;
	fftw_plan p;

	/* constructors */
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * samplesPerFrame);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * samplesPerFrame);
	if (in == NULL || out == NULL) {
		fftw_free(in);
		fftw_free(out);
		return image;
	}
	/* set the imaginary part to 0 since we will be working with purely
	   real data */
	for (int i = 0; i < samplesPerFrame; ++i) {
		in[i][1] = 0.0;
	}
	/* we will need to do the transform multuple times... */
	p = fftw_plan_dft_1d(samplesPerFrame, in, out, FFTW_FORWARD, FFTW_MEASURE);

	/* create an image */
	size_t totalFrames;
	size_t totalSamples;
	totalSamples = wp->header.dataChunkSize / wp->header.totalBytesPerSample;
	size_t maxFreqBin = samplesPerFrame / 2;
	/* yes we intend to use floor division */
	totalFrames = totalSamples / samplesPerFrame;
	fprintf(stderr, "total frames = %zd\n", totalFrames);

	image = newImage(maxFreqBin, totalFrames);
	fillImageRGBA(image, 0xff, 0xff, 0xff, 0xff);
	
	/* do FFT for each frame of the sound block */
	for (int frame = 0; frame < totalFrames; ++frame) {
		/* for each sample in the current frame */
		for (int sample = 0; sample < samplesPerFrame; ++sample) {
			in[sample][0] = ((int16_t*)(wp->data))[frame * samplesPerFrame + sample] / 32768.0;
		}

		fftw_execute(p);
		/* "normalize" the output */
		for (int i = 0; i < samplesPerFrame; ++i) {
			out[i][0] *= 2.0 / samplesPerFrame;
			out[i][1] *= 2.0 / samplesPerFrame;
		}

		/* now we can graph it :D */
		for (int frequencyBin = 0; frequencyBin < maxFreqBin; ++frequencyBin) {
			/* draw from bottom up using colorFunc */
			image.rowPtrs[maxFreqBin - frequencyBin - 1][frame] = colorFunc(out[frequencyBin]);
		}
	}
	
	/* and plot our data */

	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);

	return image;
}