default: analyzer

histogram: analysis/main.c graphing.c image_io.c sound_io.c
	gcc -o histogram analysis/main.c graphing.c image_io.c sound_io.c -lm -lfftw3 -lpng -I.

generate-spectrogram: analysis/spectrogram_test.c spectrogram.c graphing.c image_io.c sound_io.c
	gcc -o generate-spectrogram analysis/spectrogram_test.c spectrogram.c graphing.c image_io.c sound_io.c -lm -lfftw3 -lpng -I.

all: histogram generate-spectrogram

clean:
	rm -f histogram
	rm -f generate-spectrogram