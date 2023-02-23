#include <stdlib.h>

#include <fftw3.h>

#ifndef WINDOWING_H
#define WINDOWING_H

/* TODO: 
   Find a way to add Gaussian window function
   (Normalization might be an issue for this one) */

enum WindowFunctions {
	WF_RECTANGULAR,
	WF_FLATTOP,
   WF_HANN,
   WF_HAMMING
};

/* generates a window function and stores it in coefs, which is assumed
   to be of size N. */
void generateWindowFunction(size_t N, double *coefs, int windowFunction);

/* generates a window function and stores it in coefs, which is assumed
   to be of size N. The window function is scaled to compensate for
   attenuation of amplitude. */
void generateNormalizedWindowFunction(size_t N, double *coefs, int windowFunction);

/* applies the window function WF to DATA, storing the result in DATA. */
void applyWindowFunction(fftw_complex *data, const double *wf, size_t N);

#endif /* WINDOWING_H */