#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <fftw3.h>

#include "sound_io.h"
#include "windowing.h"

/* may be true or false */
#define USE_NORMALIZED_WINDOWFUNCTION 1

#define max(a, b) ((a) > (b) ? (a) : (b))

int main(int argc, char **argv) {
	const int width = 8192;
	
	if (argc != 1) {
		fprintf(stderr, "%s takes no arguments\n", argv[0]);
		return EXIT_FAILURE;
	}
	FILE *tones = fopen("sampletones.txt", "rt");
	if (tones == NULL) {
		perror("Could not open sampletones.txt");
		return EXIT_FAILURE;
	}

	int fileCount;
	char directory[64];
	{
		int matched;
		matched = fscanf(tones, "%d %63s\n", &fileCount, directory);
		if (matched != 2) {
			fprintf(stderr, "Malformed sampletones.txt file");
			fclose(tones);
			return EXIT_FAILURE;
		}
	}

	fftw_complex *in, *out;
	fftw_plan p;

	/* constructors */
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * width);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * width);
	p = fftw_plan_dft_1d(width, in, out, FFTW_FORWARD, FFTW_MEASURE);
	
	double *wf = malloc(sizeof(double) * width);

	#if USE_NORMALIZED_WINDOWFUNCTION
	generateNormalizedWindowFunction(width, wf, WF_FLATTOP);
	fprintf(stderr, "Using normalized Flat Top window function\n\n");
	#else
	generateWindowFunction(width, wf, WF_FLATTOP);
	fprintf(stderr, "Using Flat Top window function\n\n");
	#endif

	double *measuredAmplitudes = malloc(sizeof(double) * fileCount);
	double *knownAmplitudes = malloc(sizeof(double) * fileCount);
	/* number of files where amplitude could not be measured. This is
	   used to correct the mean calculation. */
	int fileErrors = 0;

	printf("Frequency (Hz)\tKnown Amplitude\tMeasured Amplitude\n");
	/* for each sample tone */
	for (int i = 0; i < fileCount; ++i) {
		char filename[128];
		int frequency;
		double amplitude;
		fscanf(tones, "%d %lf\n", &frequency, &amplitude);
		knownAmplitudes[i] = amplitude;

		snprintf(filename, 128, "%ssine%d_amplitude%d.wav", directory,
			frequency, (int)round(amplitude * 100));
		
		WAVFile *wp = loadWAVFile(filename);
		if (wp == NULL) {
			measuredAmplitudes[i] = 0.0;
			++fileErrors;
			continue;
		}

		/* populate the in array */
		for (int i = 0; i < width; ++i) {
			in[i][0] = ((int16_t*)(wp->data))[i] / 32768.0;
			in[i][1] = 0.0;
		}
		applyWindowFunction(in, wf, width);
		
		fftw_execute(p);	/* where the fun happens */
		/* normalize the output */
		for (int i = 0; i < width; ++i) {
			out[i][0] *= 2.0 / width;
			out[i][1] *= 2.0 / width;
		}

		/* hertz per bin */
		double binWidth = (double)wp->header.sampleRate / (double)width;
		int targetBin = (int)round(frequency / binWidth);

		double peakAmplitude = 0.0;
		for (int h = -2; h <= 2; ++h) {
			double magnitude = hypot(out[targetBin + h][0], out[targetBin + h][1]);
			peakAmplitude = max(peakAmplitude, magnitude);
		}

		printf("%d\t%.10lf\t%.10lf\n", frequency, amplitude, peakAmplitude);
		measuredAmplitudes[i] = peakAmplitude;
		destroyWAVFile(wp);
	}

	free(wf);
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);
	fclose(tones);

	/* data time */
	{
		double sum = 0.0;
		/* calculate mean ratio of measured to known amplitude */
		for (int i = 0; i < fileCount; ++i) {
			sum += measuredAmplitudes[i] / knownAmplitudes[i];
		}
		double mean = sum / (fileCount - fileErrors);
		fprintf(stderr, "\n%d file names given\n%d file read errors\n",
			fileCount, fileErrors);
		fprintf(stderr, "Mean ratio measured:known = %.10lf\n", mean);
		fprintf(stderr, "Mean ratio known:measured = %.10lf\n", 1.0 / mean);
		fprintf(stderr, "Multiply your correction factor by the second one.\n");
	}
	
	free(measuredAmplitudes);
	free(knownAmplitudes);

	return EXIT_SUCCESS;
}