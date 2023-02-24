#include <stdio.h>
#include <stdlib.h>

#include "sound_io.h"
#include "spectrogram.h"
#include "image_io.h"
#include "windowing.h"

int main (int argc, char **argv) {
	if (!(argc == 4 || argc == 5)) {
		fprintf(stderr, "usage: %s WAVFILE FRAMESIZE WINDOWFUNC [linear|invertedlinear|log]\n", argv[0]);
		return EXIT_FAILURE;
	}
	int frameSize = atoi(argv[2]);
	if (frameSize < 1) {
		fprintf(stderr, "FRAMESIZE must be greater than 0\n");
		return EXIT_FAILURE;
	}
	int wfType = getWindowFunction(argv[3]);
	if (wfType < 0) {
		fprintf(stderr, "Unknown window function \"%s\", defaulting to Rectangular.\n", argv[3]);
		wfType = WF_RECTANGULAR;
	}
	Pixel (*colorFunc)(fftw_complex) = colorFuncDecibelBlackToWhite;
	if (argc == 5) {
		if (!strcmp(argv[4], "linear")) {
			colorFunc = colorFuncBlackToWhite;
		} else if (!strcmp(argv[4], "invertedlinear")) {
			colorFunc = colorFuncWhiteToBlack;
		} else if (!strcmp(argv[4], "log")) {
			colorFunc = colorFuncDecibelBlackToWhite;
		} else {
			fprintf(stderr, "Unknown color function \"%s\", defaulting to logarithmic\n", argv[4]);
		}
	}

	WAVFile *wp = loadWAVFile(argv[1]);
	if (wp == NULL) {
		return EXIT_FAILURE;
	}
	fprintWAVHeader(stderr, wp);
	ImageBuf image = createSpectrogram(wp, frameSize, colorFunc, wfType);
	export_png("spectrogram.png", image);
	destroyImage(image);

	destroyWAVFile(wp);

	return EXIT_SUCCESS;
}