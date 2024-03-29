default: all

generate-histogram: analysis/generate-histogram.c analysis/graphing.c image_io.c sound_io.c windowing.c
	gcc -o generate-histogram analysis/generate-histogram.c analysis/graphing.c image_io.c sound_io.c windowing.c -lm -lfftw3 -lpng -I.

generate-spectrogram: analysis/generate-spectrogram.c analysis/spectrogram.c analysis/graphing.c image_io.c sound_io.c windowing.c
	gcc -o generate-spectrogram analysis/generate-spectrogram.c analysis/spectrogram.c analysis/graphing.c image_io.c sound_io.c windowing.c -lm -lfftw3 -lpng -I.

harmonic-finder: analysis/harmonic-finder.c sound_io.c harmonics.c windowing.c
	gcc -o harmonic-finder analysis/harmonic-finder.c sound_io.c harmonics.c windowing.c -lm -lfftw3 -I.

calibration/correction-factor: calibration/correction-factor.c sound_io.c windowing.c
	gcc -o calibration/correction-factor calibration/correction-factor.c sound_io.c windowing.c -I. -lm -lfftw3

synthesis/synthesize-wave: synthesis/synthesize-wave.c sound_io.c harmonics.c
	gcc -o synthesize-wave synthesis/synthesize-wave.c sound_io.c harmonics.c -I. -lm -lfftw3

synthesis/generate-diphthongs: synthesis/diphthongs.c sound_io.c harmonics.c
	gcc -o synthesis/generate-diphthongs synthesis/diphthongs.c sound_io.c harmonics.c -I. -lm -lfftw3

all: generate-histogram generate-spectrogram harmonic-finder calibration/correction-factor synthesis/synthesize-wave synthesis/generate-diphthongs

clean:
	rm -f generate-histogram
	rm -f generate-spectrogram
	rm -f harmonic-finder
	rm -f calibration/correction-factor
	rm -f synthesis/synthesize-wave
	rm -f synthesize-wave
	rm -f synthesis/generate-diphthongs

clean-csv-data:
	rm -f *.csv synthesis/*.csv