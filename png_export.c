#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <png.h>
#include <stdint.h>
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

#if 0
int unused_main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "%s takes exactly one argument.\n", argv[0]);
		return EXIT_FAILURE;
	}

	FILE *fp;
	fp = fopen(argv[1], "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file ");
		perror(argv[1]);
		return EXIT_FAILURE;
	}

	/* create PNG data structures */
	png_structp pngPtr = png_create_write_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (pngPtr == NULL) {
		fprintf(stderr, "Error creating PNG structure\n");
		return EXIT_FAILURE;
	}

	png_infop infoPtr = png_create_info_struct(pngPtr);
	if (infoPtr == NULL) {
		png_destroy_write_struct(&pngPtr, (png_infopp)NULL);
		fprintf(stderr, "Something went wrong creating the PNG info struct.\n");
		return EXIT_FAILURE;
	}

	/* do stuff ... */
	/* when indexing pixels in rowPtrs, use [y][x] starting at 0. */
	size_t height = 201, width = 628;
	pixel *pixelData = malloc(height * width * sizeof(pixel));
	pixel **rowPtrs = malloc(height * sizeof(pixel*));
	for (int i = 0; i < height; ++i) {
		rowPtrs[i] = pixelData + width * i;
	}

	/* manipulate the image data */
	memset(pixelData, 0xff, height * width * 4);
	for (int i = 0; i < width; ++i) {
		double yd = 100 * sin(i / 100.0);
		int y = round(yd);
		pixel tmp = {0, 0, 0, 255};

		rowPtrs[100 - y][i] = tmp;

		tmp.b = 255;
		yd = 100 * cos(i / 100.0);
		y = 100 - round(yd);
		if (0 <= y && y < 201) {
			rowPtrs[y][i] = tmp;
		}
	}

	/* then set up io */
	png_init_io(pngPtr, fp);
	png_set_write_status_fn(pngPtr, NULL);
	/* populate the info */
	png_set_IHDR(
		pngPtr, infoPtr,
		width, height, 8,
		PNG_COLOR_TYPE_RGB_ALPHA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);

	png_set_rows(pngPtr, infoPtr, (png_byte**)rowPtrs);
	png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);
	png_write_end(pngPtr, infoPtr);

	/* destroy them and exit */
	png_destroy_write_struct(&pngPtr, &infoPtr);
	free(pixelData);
	free(rowPtrs);

	fclose(fp);
	return EXIT_SUCCESS;
}
#endif

void writeRowCallback(png_structp pngPtr, png_uint_32 row, int pass) {
	fprintf(stderr, "Wrote row %u\n", row);
}