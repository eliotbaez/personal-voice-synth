# personal-voice-synth

The plan is for this to be a voice synthesizer with the capability of
being configured based on a profile of your own voice.

There will be several components to this repository.

1. The visualization tools. The spectrum viewer, the spectrogram, and
anything else that makes it easier to write analysis code.
2. The analyzer. This will be in charge of taking in sets of sound files
to create a profile of your voice.
3. The text comprehension system. This will probably consist of a word
interpreter that uses English pronunciation "rules" to guess the
pronunciation of a word/sentence, and a phoneme backend. Think DECTalk
phonemes.
3. The synthesizer. This is the part that uses the voice profile to
generate actual sounds.

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

**Bandwidth of each FFT bin = Fmax/N**

### Interpretation of Cepstrum analysis
I have no idea what I'm doing, so I'll use this section to write notes
to myself to remember how to understand what I intend to do.

Sampling frequency = Fs  
Sample period T = 1/Fs  
Each sample in time signal is of width T  

Window width = N  
Each bin in FFT of time signal is of width df = Fs/N  
(Inverse of this is N/Fs)

Each bin in the cepstrum is of width (N/Fs)/N = T
Therefore each cepstrum bin expresses period in terms of sample period

**Bandwidth of each cepstrum bin is T**

### Normalizing the Fourier transform
To recover the actual amplitude, divide the Fourier transform by N/2.
This is equivalent to multiplying by 2/N.

Normalization means that a sine wave with frequency h and amplitude 1
will show up as the data point at (f, 1).

### Phase from the complex fourier transform
       Im
	  |
	  |      . z = a + b*i
	  |     /:
	  |    / :
      |   /  : b
	  |  /   :
	  | /phi :
	  |/_____:________ Re
	      a

tan(phi) = b / a  =>  phi = arctan(b / a)  
|z|sin(phi) = b  =>  phi = arcsin(b / |z|)  
|z|cos(phi) = a  =>  phi = arccos(a / |z|)

Maybe consider timing each of these methods to see which one is least
computationally intense? Optimization isn't really a bottleneck right
now, so just keep it in the back of your mind.

Something else to consider is division by zero. With real world sound
data, I don't expect to any of the data points to be truly 0.0, but it
is technically possible for the real part to be zero. This makes the 
arctangent method unfeasible, or uncertain at best, since I don't know
whether the standard C atan() properly handles infinity or NaN.

One method to avoid this is to either use acos() or asin(), but there's
still the remote possibility of the absolute amplitude being zero, thus
still resulting in zero division. In this case, it might make sense to 
disregard harmonics with zero amplitude in [Synthesis](#Synthesis), so
as to not pollute the data with results of NaN calculations.

Conclusion: Avoid atan() method, because it can fail division by zero
even when the absolute amplitude is nonzero.

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
