#include <stdio.h>
#include <stdlib.h>

#include "sound_io.h"
#include "spectrogram.h"
#include "png_export.h"

int main (int argc, char **argv) {
	if (!(argc == 3 || argc == 4)) {
		fprintf(stderr, "usage: %s WAVFILE FRAMESIZE [linear|invertedlinear|log]\n", argv[0]);
		return EXIT_FAILURE;
	}
	int frameSize = atoi(argv[2]);
	if (frameSize < 1) {
		fprintf(stderr, "FRAMESIZE must be greater than 0\n");
		return EXIT_FAILURE;
	}
	Pixel (*colorFunc)(fftw_complex) = colorFuncBlackToWhite;
	if (argc == 4) {
		if (!strcmp(argv[3], "linear")) {
			colorFunc = colorFuncBlackToWhite;
		} else if (!strcmp(argv[3], "invertedlinear")) {
			colorFunc = colorFuncWhiteToBlack;
		} else if (!strcmp(argv[3], "log")) {
			colorFunc = colorFuncDecibelBlackToWhite;
		} else {
			fprintf(stderr, "Unknown color function \"%s\", defaulting to linear\n", argv[3]);
		}
	}

	WAVFile *wp = loadWAVFile(argv[1]);
	if (wp == NULL) {
		return EXIT_FAILURE;
	}
	//fprintWAVHeader(stderr, wp);
	ImageBuf image = createSpectrogram(wp, frameSize, colorFunc);
	export_png("spectrogram.png", image);
	destroyImage(image);

	destroyWAVFile(wp);

	return EXIT_SUCCESS;
}