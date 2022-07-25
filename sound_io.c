#include <stdio.h>
#include <stdint.h>
#include "sound_io.h"

static void littleEndianify2b(uint16_t *numptr) {
	uint16_t tmp = *numptr;
	*((uint8_t*)numptr + 0) =  tmp & 0x00ff;
	*((uint8_t*)numptr + 1) = (tmp & 0xff00) >> 8;
}

static void littleEndianify4b(uint32_t *numptr) {
	uint32_t tmp = *numptr;
	*((uint8_t*)numptr + 0) =  tmp & 0x000000ff;
	*((uint8_t*)numptr + 1) = (tmp & 0x0000ff00) >> 8;
	*((uint8_t*)numptr + 2) = (tmp & 0x00ff0000) >> 16;
	*((uint8_t*)numptr + 3) = (tmp & 0xff000000) >> 24;
}

static void localEndianify2b(uint16_t *numptr) {
	uint16_t tmp = 0;
	tmp += *((uint8_t*)numptr + 0);
	tmp += *((uint8_t*)numptr + 1) << 8;
	*numptr = tmp;
}

static void localEndianify4b(uint32_t *numptr) {
	uint32_t tmp = 0;
	tmp += *((uint8_t*)numptr + 0);
	tmp += *((uint8_t*)numptr + 1) << 8;
	tmp += *((uint8_t*)numptr + 2) << 16;
	tmp += *((uint8_t*)numptr + 3) << 24;
	*numptr = tmp;
}

static void littleEndianifyWAVHeader(struct WAVHeader *whp) {
	localEndianify4b(&whp->filesize);
	localEndianify4b(&whp->formatDataLength);
	localEndianify2b(&whp->formatType);
	localEndianify2b(&whp->channels);
	localEndianify4b(&whp->sampleRate);
	localEndianify4b(&whp->byteRate);
	localEndianify2b(&whp->totalBytesPerSample);
	localEndianify2b(&whp->bitsPerSample);
	localEndianify4b(&whp->dataChunkSize);
}

static void localEndianifyWAVHeader(struct WAVHeader *whp) {
	littleEndianify4b(&whp->filesize);
	littleEndianify4b(&whp->formatDataLength);
	littleEndianify2b(&whp->formatType);
	littleEndianify2b(&whp->channels);
	littleEndianify4b(&whp->sampleRate);
	littleEndianify4b(&whp->byteRate);
	littleEndianify2b(&whp->totalBytesPerSample);
	littleEndianify2b(&whp->bitsPerSample);
	littleEndianify4b(&whp->dataChunkSize);
}

WAVFile *loadWAVFile(const char *filename) {
	WAVFile *wp = calloc(1, sizeof(WAVFile));
	if (wp == NULL) {
		return NULL;
	}

	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file ");
		perror(filename);
		return NULL;
	}

	/* now clear to read the file */
	fread(&(wp->header), sizeof(wp->header), 1, fp);
	localEndianifyWAVHeader(&wp->header);

	wp->data = malloc(wp->header.dataChunkSize);
	if (wp->data == NULL) {
		perror("Unable to load WAV file");
		free(wp);
		fclose(fp);
		return NULL;
	}

	fread(wp->data, wp->header.dataChunkSize, 1, fp);
	fclose(fp);
	return wp;
}

void destroyWAVFile(WAVFile *wp) {
	free(wp->data);
	free(wp);
}