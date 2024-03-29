#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <fftw3.h>

#ifndef HARMONICS_H
#define HARMONICS_H

typedef struct {
	double amplitude;
	double phase;
/*  double fun. */
} Harmonic;

/*
 * Find harmonics of FUNDAMENTAL Hz in the fourier transform FT, and
 * store the amplitude and phase of each one in an array of NHARMONICS
 * Harmonic objects pointed to by HARMONICS. The 0th harmonic will
 * always represent the DC offset of the signal.
 * There are NSAMPLES data points in FT.
 * SEARCHMARGIN is the number of bins to search in either direction to
 * calculate the peak amplitude.
 * If NHARMONICS < 0, then will return a pointer to an array of all
 * possible harmonics measurable from the sample given. In this case,
 * the pointer must be free()'d afterward. Otherwise, returns NULL.
 * If a pointer is returned, it will be terminated by a Hamonic with a
 * negative amplitude. Amplitude will always be positive otherwise.
 */
Harmonic *getHarmonics(
	double fundamental,
	fftw_complex const *ft,
	size_t nSamples,
	Harmonic *harmonics,
	int nHarmonics,
	int searchMargin,
	uint32_t sampleRate
);

/* loads n harmonics from filename into a Harmonic array */
Harmonic *loadHarmonics(const char *filename, int n);

/*
 * If n < 0, then will print all Harmonics until a Harmonic with a
 * negative amplitude is found. 
 */
void printHarmonicList(Harmonic *harmonics, int n, bool csvFormat);

#endif /* HARMONICS_H */