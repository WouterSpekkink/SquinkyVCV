# src folder
This folder contains the top-level Module source code. Almost all of our code that is specific to VCV Rack is in this folder.

Usually all of the code the actually generates and processes audio is elsewhere.
## Experimental Modules
We currently have one of them.
## Singing Filter
Like the Vocal Animator, this is a filter bank tuned to the formant frequencies of typical singing voices.

Contorls:
* Fc control moves all the filters up and down and the standard 1 "volt" per octave.
* Vowel control smoothly interpolates between 'a', 'e', 'i', 'o', and 'u'.
* Model control selects different models: bass, tenor, countertenor, alto, and soprano.
* Brightness control gradually boosts the level of the higher formants. When it is all the way down, the filer gains are set by the singing models in the module, which typically fall off with increasing frequency. As this control is increased the gain of the high formant filters is brought up to match the F1 formant filter.

The **LEDs across the top** indicate which formant is currently being "sung".

## Getting good sounds from the vocal filter
The input signal must have harmonics throughout the vocal range (500 Hz to 4 kHz). A fairly low-frequency pulse wave is a reasonable model from human vocal cords, so that's a good place to start.

Almost any wide-band input will do something interesting with the filter.

Running some kinds of modulation CV into the formant control is quite effective. Since the filter will interpolate between formants, it responds smoothly to modulation.

We have been unable to get "realistic" singing from the filter, but it does sound more like singing if the pitch of the input signal is more or less in the range of the singing model. There are many references on the Internet for the pitch ranges of singers.

