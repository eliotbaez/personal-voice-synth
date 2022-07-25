#include <stdlib.h>

#include <fftw3.h>

#include "graphing.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

int plotSpectrumAbsolute(ImageBuf image, size_t n, fftw_complex *arr, Pixel color) {
	for (int i = 0; i < image.width; ++i) {
		double y = sqrt(arr[i][0] * arr[i][0] + arr[i][1] * arr[i][1]);
		int py = (int)round(y);

		/* fill all pixels below data point with color */
		for (int row = image.height - 1 - min(py, image.height - 1);
				row < image.height - 1; ++row) {
			image.rowPtrs[row][i] = color;
		}
	}
	return 0;
}

int drawLineHoriz(ImageBuf image, size_t y, Pixel color) {
	for (size_t col = 0; col < image.width; ++col) {
		image.rowPtrs[y][col] = color;
	}
	return 0;
}

int drawLineVert(ImageBuf image, size_t x, Pixel color) {
	for (size_t row = 0; row < image.height; ++row) {
		image.rowPtrs[row][x] = color;
	}
	return 0;
}

int drawGrid(ImageBuf image, size_t f_x, size_t f_y, Pixel color) {
	if (f_x != 0) {
		for (size_t x = 0; x < image.width; x += f_x) {
			drawLineVert(image, x, color);
		}
	}

	if (f_y != 0) {
		for (size_t y = 1; y <= image.height; y += f_y) {
			drawLineHoriz(image, image.height - y, color);
		}
	}
	return 0;
}

#undef max
#undef min
