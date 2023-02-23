#include <stdlib.h>
#include <math.h>

#include "windowing.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

void generateWindowFunction(size_t N, double *coefs, int windowFunction) {
	switch (windowFunction) {
	case WF_FLATTOP:
		const double a[5] = {
			0.21557895,
			0.41663158,
			0.277263158,
			0.083578947,
			0.006947368
		};
		for (int i = 0; i < N; ++i) {
			coefs[i] = a[0]
				- a[1] * cos(2.0 * M_PI * i / N)
				+ a[2] * cos(4.0 * M_PI * i / N)
				- a[3] * cos(6.0 * M_PI * i / N)
				+ a[4] * cos(8.0 * M_PI * i / N);
		}
		return;
	case WF_HAMMING: 
		const double a0 = 25.0/46.0;
		for (int i = 0; i < N; ++i) {
			coefs[i] = a0 - (1 - a0) * cos(2 * M_PI * i / N);
		}
		return;
	case WF_HANN:
		for (int i = 0; i < N; ++i) {
			coefs[i] = 0.5 - 0.5 * cos(2 * M_PI * i / N);
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
 * NOTE:
 *
 * If for whatever reason you doubt the accuracy of this correction 
 * factor, you can check using one of two methods. The first is by using
 * a calculus tool to approximate the mean value of the window function 
 * from 0 to N. The second is by changing it and checking either
 * graphically with the generate-histogram utility or numerically with
 * calibration/correction-factor utility.
 */
#define FLATTOP_NORMALIZE (4.6395674206)
#define HANN_NORMALIZE (2)
#define HAMMING_NORMALIZE (46.0/25.0)

void generateNormalizedWindowFunction(size_t N, double *coefs, int windowFunction) {
	switch (windowFunction) {
	case WF_FLATTOP:
		for (int i = 0; i < N; ++i) {
			const double a[5] = {
				FLATTOP_NORMALIZE * 0.21557895,
				FLATTOP_NORMALIZE * 0.41663158,
				FLATTOP_NORMALIZE * 0.277263158,
				FLATTOP_NORMALIZE * 0.083578947,
				FLATTOP_NORMALIZE * 0.006947368
			};
			coefs[i] = a[0]
				- a[1] * cos(2.0 * M_PI * i / N)
				+ a[2] * cos(4.0 * M_PI * i / N)
				- a[3] * cos(6.0 * M_PI * i / N)
				+ a[4] * cos(8.0 * M_PI * i / N);
		}
		return;
	case WF_HAMMING:
		const double a0 = 25.0/46.0;
		for (int i = 0; i < N; ++i) {
			coefs[i] = 1.0 - (HAMMING_NORMALIZE - 1.0) * cos(2 * M_PI * i / N);
		}
		return;
	case WF_HANN:
		for (int i = 0; i < N; ++i) {
			coefs[i] = HANN_NORMALIZE * 0.5 - HANN_NORMALIZE * 0.5 * cos(2 * M_PI * i / N);
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
#undef FLATTOP_NORMALIZE

void applyWindowFunction(fftw_complex *data, const double *wf, size_t N) {
	for (size_t i = 0; i < N; ++i) {
		data[i][0] *= wf[i];
		data[i][1] *= wf[i];
	}
	return;
}