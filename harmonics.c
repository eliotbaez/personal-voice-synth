#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include <fftw3.h>

#include "harmonics.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define absmax(a, b) ((fabs(a) > fabs(b) ? (a) : (b)))

Harmonic *getHarmonics(double fundamental, fftw_complex const *ft,
		size_t nSamples, Harmonic *harmonics, int nHarmonics,
		int searchMargin, uint32_t sampleRate) {
	/* bandwidth in hertz encompassed by a single bin */
	double binWidth = (double)sampleRate / (double)nSamples;
	/* the maximum harmonic that we can measure */
	int maxHarmonic = (sampleRate / 2.0 - searchMargin * binWidth) / fundamental;

	/* 
	 * Here we will store all the harmonics we measure within this
	 * function. If we need to truncate it at the end, we can. If the
	 * function was called with nHarmonics < 0, we will return this
	 * pointer.
	 * Use maxHarmonic + 2 to reserve space for the 0th harmonic and the
	 * negative terminator.
	 */
	Harmonic *allHarmonics = malloc(sizeof(Harmonic) * (maxHarmonic + 2));

	/* 0th harmonic treated specially - it's the DC offset */
	allHarmonics[0].amplitude = hypot(ft[0][0], ft[0][1]);
	allHarmonics[0].phase = 0.0;

	for (int h = 1; h <= maxHarmonic; ++h) {
		/* check to the sides of the bin that should contain the target
		   frequency, and find which one has the max amplitude. Use that
		   to calculate amplitude and phase. */
		double peakAmplitude = 0.0;
		fftw_complex peakComplex = {0.0, 0.0};
		int centralBin = (int)round(fundamental * h / binWidth);
		int peakBin = centralBin;

		for (int x = -searchMargin; x <= searchMargin; ++x) {
			double magnitude = hypot(ft[centralBin + x][0], ft[centralBin + x][1]);
			peakAmplitude = max(peakAmplitude, magnitude);
			peakComplex[0] = absmax(peakComplex[0], ft[centralBin + x][0]);
			peakComplex[1] = absmax(peakComplex[1], ft[centralBin + x][1]);
		}

		allHarmonics[h].amplitude = peakAmplitude;
		allHarmonics[h].phase = atan2(peakComplex[1], peakComplex[0]);
	}

	if (nHarmonics < 0) {
		/* terminate the array and hand over all the harmonics */
		allHarmonics[maxHarmonic + 1].amplitude = -1.0;
		allHarmonics[maxHarmonic + 1].phase = 0.0;
		return allHarmonics;
	} else if (nHarmonics <= maxHarmonic + 1) {
		/* we found more than enough harmonics */
		memcpy(harmonics, allHarmonics, sizeof(Harmonic) * nHarmonics);
		free(allHarmonics);
		return NULL;
	} else {
		/* they asked for too many */
		memcpy(harmonics, allHarmonics, sizeof(Harmonic) * (maxHarmonic + 1));
		/* zero the rest of the array */
		for (int h = maxHarmonic + 1; h < nHarmonics; ++h) {
			harmonics[h].amplitude = 0.0;
			harmonics[h].phase = 0.0;
		}
		free(allHarmonics);
		return NULL;
	}
}

void printHarmonicList(Harmonic *harmonics, int n, bool csvFormat) {
	const char *format;
	if (!csvFormat) {
		format = "Harmonic % 2d:\tAmplitude %1.3lf; Phase %1.3lf rad\n";
	} else {
		format = "%d\t%.6lf\t%.6lf\n";
	}

	if (n >= 0) {
		for (size_t i = 0; i < n; ++i) {
			printf(format, i, harmonics[i].amplitude, harmonics[i].phase);
		}
	} else {
		for (size_t i = 0; ; ++i) {
			if (harmonics[i].amplitude < 0.0) break;
			printf(format, i, harmonics[i].amplitude, harmonics[i].phase);
		}
	}

	return;
}

#undef max