#include <stdio.h>
#include <stdlib.h>

#include "sound_io.h"
#include "harmonics.h"
#include "windowing.h"
// #include "spectrogram.h" /* meanDemuxSamples */


int main(int argc, char **argv) {
	int samples;
	double fundamental;
	if (argc == 4) {
		samples = atoi(argv[2]);
		fundamental = atof(argv[3]);
	} else {
		fprintf(stderr, "usage: %s WAVFILE SAMPLES FUNDAMENTAL\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (samples < 1 || fundamental <= 0.0) {
		fprintf(stderr, "SAMPLES and FUNDAMENTAL must be greater than 0\n");
		return EXIT_FAILURE;
	}

	WAVFile *wp = loadWAVFile(argv[1]);
	if (wp == NULL) {
		return EXIT_FAILURE;
	}
	{
		size_t totalSamples = wp->header.dataChunkSize / wp->header.totalBytesPerSample;
		if (totalSamples < samples) {
			fprintf(stderr, "WAV file not long enough, choose a smaller sample size\n");
			fprintf(stderr, "Hint: This file contains %zd samples\n", totalSamples);
			return EXIT_FAILURE;
		}
	}

	fftw_complex *in, *out;
	fftw_plan p;

	/* constructors */
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * samples);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * samples);
	p = fftw_plan_dft_1d(samples, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	/* populate the in array */
	meanDemuxSamples(wp->data, in, samples,
		wp->header.channels, wp->header.bitsPerSample / 8);
	for (int i = 0; i < samples; ++i) {
		in[i][1] = 0.0;
	}

	double *wf = malloc(sizeof(double) * samples);
	generateNormalizedWindowFunction(samples, wf, WF_FLATTOP);
	applyWindowFunction(in, wf, samples);
	free(wf);
	
	fftw_execute(p);	/* where the fun happens */

	/* "normalize" the output */
	for (int i = 0; i < samples; ++i) {
		out[i][0] *= 2.0 / samples;
		out[i][1] *= 2.0 / samples;
	}

	/* do the harmonic finding */
	Harmonic *hp = getHarmonics(fundamental, out, samples, NULL, -1, 2, wp->header.sampleRate);

	/* output useful info */
	printHarmonicList(hp, -1, true);

	free(hp);
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);
	destroyWAVFile(wp);

	return EXIT_SUCCESS;
}