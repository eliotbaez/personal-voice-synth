#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <fftw3.h>
#include <sndfile.h>

#ifndef SOUND_IO_H
#define SOUND_IO_H

struct WAVHeader {
	char signature[4];		/* 4 byte file signature */
	union {
		uint32_t integer;
		unsigned char bytes[4];
	} filesize;				/* total file size */
	char filetypeHeader[4];	/* should be "WAVE" */
	char fmtChunkMarker[4];	/* should be "fmt\0" */
	union {
		uint32_t integer;
		unsigned char bytes[4];
	} formatDataLength;		/* size of everything before this */
	union {
		uint16_t integer;
		unsigned char bytes[2];
	} formatType;			/* type of format (1 = PCM) */
	union {
		uint16_t integer;
		unsigned char bytes[2];
	} channels;				/* number of channels */
	union {
		uint32_t integer;
		unsigned char bytes[4];
	} sampleRate;			/* samples per second */
	union {
		uint32_t integer;
		unsigned char bytes[4];
	} byterate;				/* total byterate of the file */
	union {
		uint16_t num;
		unsigned char bytes[2];
	} totalBytesPerSample;	/* includes both channels */
	union {
		uint16_t num;
		unsigned char bytes[2];
	} bitsPerSample;		/* bits per sample */
	char dataChunkHeader[4];	/* should be "data" */
	union {
		uint32_t num;
		unsigned char bytes[4];
	} dataChunkSize;		/* size of the sound data */
};

#endif /* SOUND_IO_H */