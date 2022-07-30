#include <stdlib.h>

#include <fftw3.h>

#ifndef WINDOWING_H
#define WINDOWING_H

enum WindowFunctions {
	WF_RECTANGULAR,
	WF_FLATTOP
};

/* generates a window function and stores it in coefs, which is assumed
   to be of size N. */
void generateWindowFunction(size_t N, double *coefs, int windowFunction);

/* generates a window function and stores it in coefs, which is assumed
   to be of size N. The window function is scaled to compensate for
   attenuation. */
void generateNormalizedWindowFunction(size_t N, double *coefs, int windowFunction);

/* applies the window function WF to DATA, storing the result in DATA. */
void applyWindowFunction(fftw_complex *data, const double *wf, size_t N);

#endif /* WINDOWING_H */