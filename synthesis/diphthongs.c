/* funny program to try to synthesize diphthongs using known
   harmonic data */

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971693993
#endif

#include "harmonics.h"
#include "sound_io.h"

/* parameters for diphthong synthesis */
#define T_INITSUSTAIN   0.100
#define T_FINALSUSTAIN	0.150
#define T_INTER		0.300
#define T_SILENCE	0.250
#define T_TOTAL		(25 * (T_INITSUSTAIN T_FINALSUSTAIN + T_INTER + T_SILENCE))

double linearInterpolate(double start, double end, double amtInterval) {
	return start + amtInterval * (end - start);	
}

/* a quick and dirty program to synthesize a wave based on 
   harmonic data from a CSV file */
int main(int argc, char **argv) {
    
	if (argc != 4) {
		fprintf(stderr, "Usage: %s WAVFILE FREQUENCY SAMPLERATE\n", argv[0]);
		return EXIT_FAILURE;
	}

	double F = atof(argv[2]);
    
	WAVFile wf = {
		.header = {
			.signature = "RIFF",
			.filesize = 0,
			.filetypeHeader = "WAVE",
			.fmtChunkMarker = "fmt ",
			.formatDataLength = 4 * 4,
			.formatType = 1,
			.channels = 1,
			.sampleRate = atol(argv[3]),
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
	
	const char vowels[5] = "aeiou";
	Harmonic *vowelHarmonics[5];
	{
		char *filename = malloc(strlen(argv[2]) + 6);
		sprintf(filename, "x%d.csv", (int)round(F));
		for (int i = 0; i < 5; ++i) {
			filename[0] = vowels[i];
			vowelHarmonics[i] = loadHarmonics(filename, 100);
		}
		free(filename);
	}

	for (int i = 0; i < 5; ++i) {
		/* we need all 5 vowel sounds in order to proceed */
		if (vowelHarmonics[i] == NULL) {
			for (int j = 0; i < 5; ++j)
				free(vowelHarmonics[j]);
			return EXIT_FAILURE;
		}
	}

	int initSustainSamples, finalSustainSamples, interpolationSamples, silenceSamples;
	initSustainSamples = wf.header.sampleRate * T_INITSUSTAIN;
	finalSustainSamples = wf.header.sampleRate * T_FINALSUSTAIN;
	interpolationSamples = wf.header.sampleRate * T_INTER;
	silenceSamples = wf.header.sampleRate * T_SILENCE;
	int diphthongSamples = initSustainSamples + finalSustainSamples + interpolationSamples + silenceSamples;

	size_t nSamples = 25 * diphthongSamples;
	wf.header.byteRate = wf.header.totalBytesPerSample * wf.header.sampleRate;
	wf.header.dataChunkSize = nSamples * wf.header.totalBytesPerSample;
	wf.header.filesize = sizeof(wf.header) + wf.header.dataChunkSize;

	/* now we can generate the sound */
	int16_t *data = malloc(nSamples * 4);
	wf.data = (void *)data;

	/* count the harmonics */
	int n = 0;
	while (vowelHarmonics[0][n].amplitude >= 0.0)
		++n;

	/* consider optimizing this to take advantage of sequential memory
	   access ? */
	/* fasten your seatbelts, becauese we are getting nested */
	size_t s = 0;
	/* each start vowel */
	for (int vStart = 0; vStart < 5; ++vStart) {
		/* each end vowel */
		for (int vEnd = 0; vEnd < 5; ++vEnd) {
			/* DIPHTHONGS!!! */

			/* generate first monophthong */
			for (int t = 0; t < initSustainSamples; ++t) {
				double value = 0.0;
				
				for (int h = 0; h < n; ++h) {
					value += cos(2*M_PI*F*h / wf.header.sampleRate * t)
						* vowelHarmonics[vStart][h].amplitude;
				}
				data[s++] = 32767 * value;
			}

			/* then interpolate */
			/* each harmonic */
			for (int t = 0; t < interpolationSamples; ++t) {
				double value = 0.0;
				for (int h = 0; h < n; ++h) {
					value += cos(2*M_PI*F*h / wf.header.sampleRate * s)
						* linearInterpolate(
							vowelHarmonics[vStart][h].amplitude,
							vowelHarmonics[vEnd][h].amplitude,
							(double)t / interpolationSamples
						);
				}
				data[s++] = 32767 * value;
			}

			/* generate second monophthong */
			for (int t = 0; t < finalSustainSamples; ++t) {
				double value = 0.0;
				
				for (int h = 0; h < n; ++h) {
					value += cos(2*M_PI*F*h / wf.header.sampleRate * t)
						* vowelHarmonics[vEnd][h].amplitude;
				}
				data[s++] = 32767 * value;
			}

			/* generate separating silence */
			for (int t = 0; t < silenceSamples; ++t) {
				data[s++] = 0;
			}
		}
	}


	exportWAVFile(argv[1], &wf);
	for (int i = 0; i < 5; ++i)
		free(vowelHarmonics[i]);
	free(data);

	return EXIT_SUCCESS;
}