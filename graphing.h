#ifndef GRAPHING_H
#define GRAPHING_H

#include <stdlib.h>
#include <fftw3.h>
#include "png_export.h"

int plotSpectrumAbsolute(ImageBuf image, size_t n, fftw_complex *arr, Pixel color);
int plotSpectrumComplex(ImageBuf image, size_t n, fftw_complex *arr, Pixel (*colorFunc)(fftw_complex));

int drawLineHoriz(ImageBuf image, size_t y, Pixel color);
int drawLineVert(ImageBuf image, size_t x, Pixel color);

int drawGrid(ImageBuf image, size_t f_x, size_t f_y, Pixel color);

#endif /* GRAPHING_H */