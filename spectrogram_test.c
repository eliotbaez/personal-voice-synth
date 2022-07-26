#include <stdio.h>
#include <stdlib.h>

#include "sound_io.h"
#include "spectrogram.h"
#include "png_export.h"

int main (int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "%s takes exactly 2 arguments\n", argv[0]);
		return EXIT_FAILURE;
	}
	int frameSize = atoi(argv[2]);
	if (frameSize < 1) {
		fprintf(stderr, "FRAMESIZE must be greater than 0\n");
		return EXIT_FAILURE;
	}

	WAVFile *wp = loadWAVFile(argv[1]);
	if (wp == NULL) {
		return EXIT_FAILURE;
	}
	//fprintWAVHeader(stderr, wp);
	ImageBuf image = createSpectrogram(wp, frameSize, colorFuncBlackToWhite);
	export_png("spectrogram.png", image);

	destroyImage(image);
	destroyWAVFile(wp);

	return EXIT_SUCCESS;
}