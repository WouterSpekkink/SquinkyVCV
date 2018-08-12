# Notes about the creation of Functional VCO-1

## About the Original
Fundamental VCO-1 is a very high quality, excellent sounding VCO. It does exactly what it claim to do with very little digital artifacts. VCO-1 is a very popular module; for this reason it seemed like a good candidate for a CPU diet. VCV users continue to complain about popping and clicking with large patches, so we hope improvements to these popular modules will help. 

Fundamental VCO-1 uses 16X oversampling to generate standard waveforms, in both "analog" and "digital" versions. The oversampling keeps the aliasing low, allows hard and soft sync with low aliasing, and suppresses aliasing from audio rate modulation.

As such, Fundamental VCO-1 is a very good substitute for an analog VCO. The only down-side is that the 16X oversampling increases the CPU usage dramatically.

In revamping this VCO, we wanted to try as much as possible to preserve the sound exactly like the original. We did not add any new features, or try to make anything "better". And when we put the code on a diet we did not want to lower the sound quality of thsse classic module.

## Initial Measurements

It is difficult to accurately measure the CPU usage of a VCV module, although since version 0.6 there have been CPU meters which are very useful for getting an overall picture of CPU usage. But the CPU meters do not enable stable, accurate, and repeatable measurements.

So we more or less run the plugins in an isolated test framework. This lets us get good measurements. The down side is that the isolated system is different from running in VCV, and the numbers won’t correlate exactly.
We use an arbitrary scale for our measurements, where "100" means that the plugin under test seems to be using 1% of the available CPU on one core of our quite old Intel Core i5 Windows7 computer.

Here are the initial measurements we took before any optimizations were done, along with some Squinky Labs modules for reference:

* Fundamental VCO-1, all outputs patched, digital: 798
* Fundamental VCO-1, saw only, digital: 489
* Fundamental VCO-1, saw only, analog: 270
* SL Formants: 84.1
* SL Growler: 50.9
* SL Chopper: 14.9
* SL Booty Shifter: 11.2
* SL Colors: 11.6

Fundamental VCO-1 uses a LOT of CPU. Since it is so heavy in its CPU usage, we thought it would be easy to make it much faster. But it was not as easy as we had hoped.

## General approach to optimization

Every theory must be validated by experiment. So we look at the code, formulate a theory, throw together a simplified implementation of the theory, and compare before and after measurements.

If the CPU usage goes down a lot, the experiment is a success, and we try to make a full implementation that preserves the drop in CPU usage without compromising the quality.

Then repeat this process over and over until done.

## What we did to EvilVCO

EvenVCO has a pretty simple waveform generation loop that runs at sample rate. All waveforms are calculated in that loop, and then all are run though their own MinBLEP modules. There is of course some math to convert all the control voltages and parameters into an oscillator period. This too runs at sample rate.

A few theories suggested themselves right away:
* Cosf (cosine function) is very slow, and is called every sample.
* Powf (exponential function) is slow, and is called every sample.
* All of the MinBleps are run every sample, even if their output is not connected.
* All of the waveforms are calculated, even if they are not used.

Quick experiments showed that removing cosf and replacing it with a constant sped things up a lot. Same with removing powf. Cutting out unused waveform generation and MinBLEP sped things up a lot.

So we ended up doing the following:
* Replace the calls to cosf and expf with lookup tables. We used linear interpolation to make them accurate.
* Completely refactored the waveform generation so no extra work is done for outputs that are not connected.
* Made dedicated waveform generation routines for the special case(s) of a single waveform.

Most of this was relatively easy, as we had a lot of the necessary code already from other Squinky Labs modules. It did turn out that our exponential lookup table wasn’t accurate enough for us, so we spent a lot of time making and testing a better one.

## What we did to Fundamental VCO-1

VCO-1 already had some optimization. Although the waveform generation runs for all waveforms the decimation filters are only run for outputs that are connected. The decimation filters are the same ones used by the VCV Rack audio engine, so we assumed they are linear phase FIR filters, as is customary for high quality sample rate conversion.

But, since this is an oversampling VCO, the waveform generation is running at 16X sample rate. Any extra work in this “inner loop” is going to be magnified by 16.

That said, our experiments showed few surprises.

* Since cosf() is called all the time in the 16X waveform generation loop, it was a no-brainer to replace it with the same sin lookup table we use in most of our modules. Likewise, we made sure that cosf is only called if the SIN output is connected.
* The powf() call is also slow and it was worthwhile getting rid of the powf call, although the gain was enormous since powf is only called once per sample.
* We refactored the inner loop to make sure that the waveform generation is only done for waveforms whose output is patched.
* The decimation filters use a lot of CPU, so we replaced the stock ones with simple 6-pole butterworth lowpass filters.
* It would of course save CPU to reduce the oversampling rate, but we did not want to decrease the quality.

Again, most of the software required was already in the code-base for our other modules. The one thing that was difficult here was devising test software to measure the aliasing. We wanted to be sure that our faster decimation filters were not increasing the level of aliasing. While this new alias test is not perfect, it did give us confidence that we were not increasing the aliasing with our substituted filters.

## Results

Before and after:

* Fundamental VCO-1, all outputs patched, digital: 798 -> 187.8 (X4.2)
* Fundamental VCO-1, saw only, digital: 489 -> 83.7 (X5.8)
* Fundamental VCO-1, saw only, analog: 270 -> 83 (X2.3)
