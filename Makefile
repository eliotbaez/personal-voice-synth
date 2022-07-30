default: all

generate-histogram: analysis/generate-histogram.c graphing.c image_io.c sound_io.c windowing.c
	gcc -o generate-histogram analysis/generate-histogram.c graphing.c image_io.c sound_io.c windowing.c -lm -lfftw3 -lpng -I.

generate-spectrogram: analysis/generate-spectrogram.c spectrogram.c graphing.c image_io.c sound_io.c
	gcc -o generate-spectrogram analysis/generate-spectrogram.c spectrogram.c graphing.c image_io.c sound_io.c -lm -lfftw3 -lpng -I.

harmonic-finder: analysis/harmonic-finder.c sound_io.c harmonics.c
	gcc -o harmonic-finder analysis/harmonic-finder.c sound_io.c harmonics.c -lm -lfftw3 -I.

all: generate-histogram generate-spectrogram harmonic-finder

clean:
	rm -f generate-histogram
	rm -f generate-spectrogram
	rm -f harmonic-finder