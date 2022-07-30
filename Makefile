default: all

histogram: analysis/main.c graphing.c image_io.c sound_io.c
	gcc -o histogram analysis/main.c graphing.c image_io.c sound_io.c -lm -lfftw3 -lpng -I.

generate-spectrogram: analysis/spectrogram_test.c spectrogram.c graphing.c image_io.c sound_io.c
	gcc -o generate-spectrogram analysis/spectrogram_test.c spectrogram.c graphing.c image_io.c sound_io.c -lm -lfftw3 -lpng -I.

harmonic-finder: analysis/harmonic-finder.c sound_io.c harmonics.c
	gcc -o harmonic-finder analysis/harmonic-finder.c sound_io.c harmonics.c -lm -lfftw3 -I.

all: histogram generate-spectrogram harmonic-finder

clean:
	rm -f histogram
	rm -f generate-spectrogram
	rm -f harmonic-finder