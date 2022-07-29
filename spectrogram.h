#include <stdio.h>
#include <stdlib.h>

#include <fftw3.h>

#include "image_io.h"
#include "sound_io.h"

#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

/* expects a mono WAV file. Prefer powers of 2 for samplesPerFrame */
ImageBuf createSpectrogram(const WAVFile * wp, int samplesPerFrame, Pixel (*colorFunc)(fftw_complex));

Pixel thresholdBlack(fftw_complex z);
Pixel colorFuncDecibelBlackToWhite(fftw_complex z);
Pixel colorFuncWhiteToBlack(fftw_complex z);
Pixel colorFuncBlackToWhite(fftw_complex z);

/* Demuxes samples from MUXED, by adding them in groups of CHANNELS and
   storing the mean in the real part of the corresponding index of arr */
void meanDemuxSamples(const void *muxed, fftw_complex *arr, size_t samples, int channels, int sampleSize);

#endif /* SPECTROGRAM_H */