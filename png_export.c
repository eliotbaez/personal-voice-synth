#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include <png.h>

#include "png_export.h"

ImageBuf newImage(size_t height, size_t width) {
	ImageBuf image;
	image.height = height;
	image.width = width;
	image.pixels = malloc(height * width * sizeof(Pixel));
	image.rowPtrs = malloc(height * sizeof(Pixel*));

	for (size_t row = 0; row < height; ++row) {
		image.rowPtrs[row] = image.pixels + row * width;
	}

	return image;
}

void destroyImage(ImageBuf image) {
	free(image.rowPtrs);
	free(image.pixels);
	return;
}

void fillImage(ImageBuf image, Pixel pixel) {
	for (size_t row = 0; row < image.height; ++row) {
		for (size_t col = 0; col < image.width; ++col) {
			image.rowPtrs[row][col] = pixel;
		}
	}
	return;
}

void fillImageRGBA(ImageBuf image, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	Pixel pixel = {
		.r = r,
		.g = g,
		.b = b,
		.a = a
	};
	fillImage(image, pixel);
	return;
}

int export_png(const char *filename, ImageBuf image) {
	FILE *fp;
	fp = fopen(filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file ");
		perror(filename);
		return EXIT_FAILURE;
	}

	/* create PNG data structures */
	png_structp pngPtr = png_create_write_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (pngPtr == NULL) {
		fprintf(stderr, "Error creating PNG\n");
		return EXIT_FAILURE;
	}

	png_infop infoPtr = png_create_info_struct(pngPtr);
	if (infoPtr == NULL) {
		png_destroy_write_struct(&pngPtr, (png_infopp)NULL);
		fprintf(stderr, "Something went wrong creating the PNG info struct.\n");
		return EXIT_FAILURE;
	}

	/* then set up io */
	png_init_io(pngPtr, fp);
	png_set_write_status_fn(pngPtr, NULL);
	/* populate the info */
	png_set_IHDR(
		pngPtr, infoPtr,
		image.width, image.height, 8,
		PNG_COLOR_TYPE_RGB_ALPHA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);

	png_set_rows(pngPtr, infoPtr, (png_byte**)image.rowPtrs);
	png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);
	png_write_end(pngPtr, infoPtr);

	/* destroy them and exit */
	png_destroy_write_struct(&pngPtr, &infoPtr);
	fclose(fp);
	return 0;
}

void writeRowCallback(png_structp pngPtr, png_uint_32 row, int pass) {
	fprintf(stderr, "Wrote row %u\n", row);
}