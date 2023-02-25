#include <stdio.h>
#include <stdint.h>
#include <string.h>
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

/* FIXME:
   Are these mislabeled???? */
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

/* please review this code; it's not guaranteed to have any sort of
   sanity checking or error checking yet */
int exportWAVFile(const char *filename, const WAVFile *wp) {
	FILE *fp = fopen(filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file ");
		perror(filename);
		return -1;
	}

	WAVFile wf;
	memcpy(&wf, wp, sizeof(wf));
	/* now clear to write the file */
	littleEndianifyWAVHeader(&wf.header);
	/* consider checking the following return values */

	fwrite(&(wf.header), sizeof(wf.header), 1, fp);
	fwrite(wf.data, wf.header.dataChunkSize, 1, fp);

	fclose(fp);

	return 0;
}

void destroyWAVFile(WAVFile *wp) {
	free(wp->data);
	free(wp);
}

void fprintWAVHeader(FILE *fp, WAVFile *wp) {
	char buf[5];
	buf[4] = '\0';

	memcpy(buf, wp->header.signature, 4);
	fprintf(fp, "Signature: %s\nFilesize: %u\n", buf, wp->header.filesize);
	memcpy(buf, wp->header.filetypeHeader, 4);
	fprintf(fp, "Filetype Header: %s\n", buf);
	memcpy(buf, wp->header.fmtChunkMarker, 4);
	fprintf(fp, "Format chunk marker: %s\n", buf);
	fprintf(fp,
		"Format data length: %u\n"
		"Format type: %hu\n"
		"Channels: %hu\n"
		"Sample rate: %u\n"
		"Byte rate: %u\n"
		"Total bytes per sample: %hu\n"
		"Bits per sample: %hu\n",
		wp->header.formatDataLength,
		wp->header.formatType,
		wp->header.channels,
		wp->header.sampleRate,
		wp->header.byteRate,
		wp->header.totalBytesPerSample,
		wp->header.bitsPerSample
	);
}

void meanDemuxSamples(const void *muxed, fftw_complex *arr, size_t samples, int channels, int sampleSize) {
	/* The maximum value of a signed integer of size sampleSize,
	   multplied by the number of channels to calculate the mean.
	   Multiplying by channels here saves us one division later.
	   Division is generally more expensive than multiplication. */
	double divisor = ((unsigned long)1 << (8 * sampleSize - 1)) * channels;

	switch (sampleSize) {
	case 1:
		for (size_t sample = 0; sample < samples; ++sample) {
			arr[sample][0] = 0.0;
			for (int c = 0; c < channels; ++c) {
				arr[sample][0] += ((int8_t*)muxed)[sample * channels + c] / divisor;
			}
		}
		break;
	case 2:
		for (size_t sample = 0; sample < samples; ++sample) {
			arr[sample][0] = 0.0;
			for (int c = 0; c < channels; ++c) {
				arr[sample][0] += ((int16_t*)muxed)[sample * channels + c] / divisor;
			}
		}
		break;
	case 4:
		for (size_t sample = 0; sample < samples; ++sample) {
			arr[sample][0] = 0.0;
			for (int c = 0; c < channels; ++c) {
				arr[sample][0] += ((int32_t*)muxed)[sample * channels + c] / divisor;
			}
		}
	}

	return;

	#if 0 /* this is under construction */
	/* generalized version for any number of channels */
	for (size_t sample = 0; sample < samples; ++sample) { /* each sample */
		for (int c = 0; c < channels; ++c) { /* each channel within that sample */
			for (int byte = 0; byte < sampleSize; ++byte) { /* each byte within that channel */

			}
			muxed[sampleSize * (sample * channels + c)];
		}
	}
	#endif
}
