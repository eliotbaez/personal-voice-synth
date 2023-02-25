#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971693993
#endif

#include "harmonics.h"
#include "sound_io.h"

/* a quick and dirty program to synthesize a wave based on 
   harmonic data from a CSV file */
int main(int argc, char **argv) {
    
	if (argc != 6) {
		fprintf(stderr, "Usage: %s CSVFILE WAVFILE FREQUENCY SAMPLERATE LENGTH_SEC\n", argv[0]);
	return EXIT_FAILURE;
	}

	double duration = atof(argv[5]);
	if (duration < 0.0) {
		fprintf(stderr, "Duration must be non-negative.\n");
		return EXIT_FAILURE;
	}

	double F = atof(argv[3]);
    
	WAVFile wf = {
		.header = {
			.signature = "RIFF",
			.filesize = 0,
			.filetypeHeader = "WAVE",
			.fmtChunkMarker = "fmt ",
			.formatDataLength = 4 * 4,
			.formatType = 1,
			.channels = 1,
			.sampleRate = atol(argv[4]),
			.byteRate = 0,
			.totalBytesPerSample = 2,
			.bitsPerSample = 16,
			.dataChunkHeader = "data",
			.dataChunkSize = 0
		},
		.data = NULL
	};

	if (wf.header.sampleRate < 1) {
		fprintf(stderr, "Sample rate must be greater than 0.\n");
		return EXIT_FAILURE;
	}

	Harmonic *hl = loadHarmonics(argv[1], 200);
	if (hl == NULL) {
		return EXIT_FAILURE;
	}

	size_t nSamples = duration * wf.header.sampleRate;
	wf.header.byteRate = wf.header.totalBytesPerSample * wf.header.sampleRate;
	wf.header.dataChunkSize = nSamples * wf.header.totalBytesPerSample;
	wf.header.filesize = sizeof(wf.header) + wf.header.dataChunkSize;

	/* now we can generate the sound */
	int16_t *data = malloc(nSamples * 2);
	wf.data = (void *)data;

	/* count the harmonics */
	int n = 0;
	while (hl[n].amplitude >= 0.0)
		++n;

	/* each sample */
	for (int i = 0; i < nSamples; ++i) {
		/* each harmonic */
		double s = 0.0;
		for (int h = 0; h < n; ++h) {
			s += hl[h].amplitude * cos(2*M_PI*F*h / wf.header.sampleRate * i);
		}
		data[i] = 32767 * s;
	}

	exportWAVFile(argv[2], &wf);
	free(hl);
	free(data);

	return EXIT_SUCCESS;
}