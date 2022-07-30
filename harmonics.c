#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include <fftw3.h>

#include "harmonics.h"

Harmonic *getHarmonics(double fundamental, fftw_complex const *ft,
		size_t nSamples, Harmonic *harmonics, int nHarmonics,
		double relativeMargin, uint32_t sampleRate) {
	/* 
	 * It's currently 2 in the morning and for whatever reason, the
	 * harmonics in the debug text only line up properly if I divide by
	 * 2 here first. I don't know why and I'm too tired to find out.
	 * 
	 * I wanna say it's because the number of usable bins is half the
	 * number of samples (the second half of the DFT is negative
	 * frequencies, which are redundant data in this case), and the
	 * width of the bin is the sample rate divided by the number of
	 * bins. That sounds intuitive, so I think I just misread my notes.
	 * I'll review this tomorrow after I've slept.
	 */
	nSamples /= 2;

	/* bandwidth in hertz encompassed by a single bin */
	double binWidth = sampleRate / 2.0 / nSamples;
	/* Half the width of our interval of integration */
	double margin = fundamental * relativeMargin / 2.0;
	/* the maximum harmonic that we can measure */
	int maxHarmonic = (sampleRate / 2.0 - margin) / fundamental;

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

	/* 
	 * Vector calculus tells us that the components of the integral of
	 * vectors are equal to the integrals of the vectors' components.
	 * This means it's fine to integrate the real and imaginary parts of
	 * the Fourier transform separately, without angering the math gods.
	 */
	for (int h = 1; h <= maxHarmonic; ++h) {
		/* we will be integrating onto these */
		double im = 0.0, re = 0.0;

		/* bounds of integration, in Hertz*/
		double lower, upper;
		lower = fundamental * h - margin;
		upper = fundamental * h + margin;
		for (int a = lower / binWidth, b = upper / binWidth; a < b; ++a) {
			re += ft[a][0];
			im += ft[a][1];
		}

		allHarmonics[h].amplitude = hypot(re, im);
		allHarmonics[h].phase = acos(re / allHarmonics[h].amplitude);
		// printf("Harmonic %d: bounded by (%lf, %lf)\n", h, lower, upper);
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
	end:
	return;
}
