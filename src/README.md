# src folder
This folder contains the top-level Module source code. Almost all of our code that is specific to VCV Rack is in this folder.

Usually all of the code the actually generates and processes audio is elsewhere.
## Experimental Modules
We currently have one of them.
### Singing Filter
Like the Vocal Animator, this is a filter bank tuned to the formant frequencies of typical singing voices.

The Fc VC/control moves all the filters up and down.

The vowel control smoothly interpolates between 'a', 'e', 'i', 'o', and 'u'.

The model control selects different models: bass, tenor, countertenor, alto, and soprano.

The brightness control gradually boosts the level of the higher formants. When it is all the way down, the filer gains are set by the singing models in the module, which typically fall off with increasing frequency. As this control is increased the gain of the high formant filters is brought up to match the F1 formant filter.


