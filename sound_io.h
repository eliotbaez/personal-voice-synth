#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <fftw3.h>

#ifndef SOUND_IO_H
#define SOUND_IO_H

struct WAVHeader {
	char signature[4];			/* 4 byte file signature */
	uint32_t filesize;			/* total file size */
	char filetypeHeader[4];		/* should be "WAVE" */
	char fmtChunkMarker[4];		/* should be "fmt\0" */
	uint32_t formatDataLength;	/* size of everything before this */
	uint16_t formatType;		/* type of format (1 = PCM) */
	uint16_t channels;			/* number of channels */
	uint32_t sampleRate;		/* samples per second */
	uint32_t byteRate;			/* total byterate of the file */
	uint16_t totalBytesPerSample;	/* includes all channels */
	uint16_t bitsPerSample;		/* bits per sample */
	char dataChunkHeader[4];	/* should be "data" */
	uint32_t dataChunkSize;		/* size of the sound data */
};

typedef struct {
	struct WAVHeader header;
	void *data;
} WAVFile;

WAVFile *loadWAVFile(const char *filename);
void destroyWAVFile(WAVFile *wp);

void fprintWAVHeader(FILE *fp, WAVFile *wp);

/* Demuxes samples from MUXED, by adding them in groups of CHANNELS and
   storing the mean in the real part of the corresponding index of arr */
void meanDemuxSamples(const void *muxed, fftw_complex *arr, size_t samples, int channels, int sampleSize);

#endif /* SOUND_IO_H */