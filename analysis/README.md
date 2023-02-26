# analysis

## Analysis plan

I have a couple of ideas for how we can proceed with voice analysis.
None of these are necessarily mutually exclusive; I'm just listing
potentially useful ideas.

One method would be getting a sufficiently long sample of the user
pronouncing each of the DECTALK phonemes, then interpolating between
all those samples to generate speech. This method is straightforward
but requires a lot of user data.

Another method involves getting a better understanding of the user's
oral cavity. This would be done by getting an "h sample" while making
different shapes with their mouth. The h sample would allow us to get
a spectral profile of the user's mouth for specific shapes. Afterward,
get a few samples of the user saying the schwa sound in varying pitches.
We can use cepstral analysis to determine the pitch (if necessary).
Using the known information about the user's spectral profile for the
schwa sound from the h sample data, we can normalize the voiced schwa
sound by dividing its spectrum by the envelope of the voiceless schwa
spectrum. This would give us base tone that we could then modulate using
the envelope of different mouth shapes. This method is significantly
more analytical, but would provide a deeper learning experience for me.
In addition, this would allow us to simulate different voice timbres
(like a bright vs a dark tone), which might be useful for musical
synthesis. 

I'll probably add more ideas here as they arise.