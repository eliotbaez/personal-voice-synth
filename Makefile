default: analyzer

analyzer: main.c graphing.c png_export.c sound_io.c
	gcc -o analyzer main.c graphing.c png_export.c sound_io.c -lm -lfftw3 -lpng

spectrogram: spectrogram_test.c spectrogram.c graphing.c png_export.c sound_io.c
	gcc -o generate-spectrogram spectrogram_test.c spectrogram.c graphing.c png_export.c sound_io.c -lm -lfftw3 -lpng