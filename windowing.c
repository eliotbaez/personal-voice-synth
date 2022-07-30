#include <stdlib.h>
#include <math.h>

#include "windowing.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

void generateWindowFunction(size_t N, double *coefs, int windowFunction) {
	switch (windowFunction) {
	case WF_FLATTOP:
		for (int i = 0; i < N; ++i) {
			const double a[5] = {
				0.21557895,
				0.41663158,
				0.277263158,
				0.083578947,
				0.006947368
			};
			coefs[i] = a[0]
				- a[1] * cos(2.0 * M_PI * i / N)
				+ a[2] * cos(4.0 * M_PI * i / N)
				- a[3] * cos(6.0 * M_PI * i / N)
				+ a[4] * cos(8.0 * M_PI * i / N);
		}
		return;
	case WF_RECTANGULAR:
	default:
		for (int i = 0; i < N; ++i) {
			coefs[i] = 1.0;
		}
		return;
	}
}
 
/* 
 * FIXME:
 * 
 * The coefficients in this function are WRONG. I got them by
 * calculating the average value of the flat top window function on a
 * graphing calculator. I just integrated the function over its intended
 * interval and divided by the interval. Should I have used root mean 
 * square instead? Maybe. Either way, these are wrong. 
 */
void generateNormalizedWindowFunction(size_t N, double *coefs, int windowFunction) {
	switch (windowFunction) {
	case WF_FLATTOP:
		for (int i = 0; i < N; ++i) {
			const double a[5] = {
				4.63867181838 * 0.21557895,
				4.63867181838 * 0.41663158,
				4.63867181838 * 0.277263158,
				4.63867181838 * 0.083578947,
				4.63867181838 * 0.006947368
			};
			coefs[i] = a[0]
				- a[1] * cos(2.0 * M_PI * i / N)
				+ a[2] * cos(4.0 * M_PI * i / N)
				- a[3] * cos(6.0 * M_PI * i / N)
				+ a[4] * cos(8.0 * M_PI * i / N);
		}
		break;
	case WF_RECTANGULAR:
	default:
		for (int i = 0; i < N; ++i) {
			coefs[i] = 1.0;
		}
		return;
	}
}

void applyWindowFunction(fftw_complex *data, const double *wf, size_t N) {
	for (size_t i = 0; i < N; ++i) {
		data[i][0] *= wf[i];
		data[i][1] *= wf[i];
	}
	return;
}