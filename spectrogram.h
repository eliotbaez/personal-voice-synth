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

#endif /* SPECTROGRAM_H */