#ifndef PNG_EXPORT_H
#define PNG_EXPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include <png.h>

void writeRowCallback(png_structp pngPtr, png_uint_32 row, int pass);

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} Pixel;

typedef struct {
	size_t height;
	size_t width;
	Pixel *pixels;
	Pixel **rowPtrs;
} ImageBuf;

ImageBuf newImage(size_t height, size_t width);
void destroyImage(ImageBuf image);

void fillImagePixel(ImageBuf image, Pixel pixel);
void fillImageRGBA(ImageBuf image, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/* returns nonzero on error */
int export_png(const char *filename, ImageBuf image);

#endif	/* PNG_EXPORT_H */