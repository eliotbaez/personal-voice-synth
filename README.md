# configurable-synth

The plan is for this to be a voice synthesizer with the capability of
being configured based on a profile of your own voice.

## Voice Profiles

A voice profile will be a set of voice samples for the purpose of
analysis. My plan is to model the specification for these samples off
of the original DECTalk phonemes, presented as a set of sound files.

Sound samples should be provided as a complete set of these files in a
zip file structure, to be loaded and analyzed by a program.

This program will create a set of parameters based on the acoustic
composition of the different samples. These parameters will then be used
by the synthesizer to determine what amplitudes at which to emit the
different harmonics of the fundamental frequency.

The voice profile will include a noise sample (silence by the speaker).

## Analysis

The voice samples will be analyzed by means of a Fast Fourier Transform.
The Fourier Transform of the noise sample will be subtracted from that
of the phoneme samples, in order to have more accurate harmonic
detection.

I'm not yet sure what method to use to find the peaks of the fourier
transform. Maybe I'll end up using a library for this? Maybe some kind
of calculus library?

For debugging purposes, I intend to allow the analyzer to output a
spectrogram graphically, just so I know everything is working. I don't
want to have to paste  text files into Excel or Desmos every time I want
to take a look at a graphical Fourier Transform.

### reading the fourier transform image
**bins are in units of cycles per sample period**
sample rate = 1.024kHz = 1024 Sa/s
F index is half of 
(0->1023) / (1024 * 1/1024) 
0->1023 hz

sample rate = 2.048kHz = 2048 Sa/s
F index is half of
(0->2043) / 

sample rate = 44.1kHz = 44100 Sa/s
Sample period = 1/44100 s
F index is half of
(0->44099) / 

cycle per T
1 / T
1 / (1/Fs)
unit = Fs

N = number of sample points
Fs = sample rate
T = 1/Fs = sample period
Fmax = Fs/2
Fmin = 0
Number of bins = number of sample points
Bandwidth of each bin = Fmax/N

## Synthesis

The synthesis is arguably both the easiest and most difficult part.
The voice will be synthesized by adding sine waves tuned to the
different harmonics of the fundamental frequency. This part is pretty
simple, as it's mostly just sin() calls with extra transformation.

Generating the sound is one challenge, however. The other is exporting
the sound. I have two options, which are not mutually exclusive:

1. Export the generated sound to a sound file, like WAV.
2. Play the sound live on the system, using the local sound API.

Generating a WAV is probably the simplest option, since it is a fair bit
more portable than using a sound API native to or compatible with the
operating system. However, it's also less convenient, especially if you
want to use this as a standalone real-time synthesizer.

I'll probably start by generating a block of PCM data in memory from
whatever the wave synthesis produces, that way it already exists in a
form that can (hopefully) be easily written as a WAV file or streamed
directly through the sound API.

### synthesizing a sine wave with real time frequency
All trig functions are in radians  
f = frequency  (Hz)
Tau = 2 * Pi  
t = time (s)  
ts = time (Sa)
A = amplitude (dimensionless)  
Fs = sample (Sa/s)  

Note that:  
ts (Sa) = t (s) * Fs (Sa/s)  
t (s) = ts (Sa) / ( Fs (Sa/s) )  

s(t) = A * sin(Tau * t * f)
s(ts) = A * sin(Tau * ts/Fs * f)

Note that in the above two equations, phase is not necessarily preserved
with discrete changes in frequency over time. See the section about
theh use of differentials (integration) to preserve phase in variable
frequency sinusoids.