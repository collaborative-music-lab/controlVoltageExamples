# Copernicues Cartesian Sequencer

Hi! You might by asking what a cartesian sequencer is. Well, what I do is store a 4x4 grid of values, and you can traverse across this grid at different rates in the X and Y direction. So kinda like traversing a map! There are other cartesian sequencers out there - notably the Make Noise René - but every sequencer is unique, just like people! So you'll have a unique experience playing with me.

## Traversing the grid

I can only be in one location in the grid at a time - with a unique XY coordinate. Everytime I receive a pulse I will move in both the X and Y directions - but won't often move in both directions at the same rate. Instead, I have an X:Y ratio control to set the relative rate of motion of X and Y. This basically adds a clock divider to one of the two directions, so a ratio of 1:4 will take one clock to move in the X dimension and 4 clocks to move in the Y direction. A ratio of 4:1 would do the opposite. 

You can set my ratio directly by using the ratio knob, or when you input a CV then the ratio knob becomes an attenuator for the CV signal. I think 2.5v will be a 1:1 ratio - but you might want to play around and see for yourself.

## The things I say

I put out a couple of signals. The two main ones are CV signals for pitch and envelope.

### Pitch output
 The pitch signal is determined by the quantize switch, which offers 4, 7, or 12 note pitch sets. There is also an offset CV input which adds to the value of the current XY location.

4 notes: seventh chord, a dominant 7 if no offset is added
7 notes: a diatonic scale, set to major if no offset is added
12 notes: a chromatic scale. The offset will transpose this in the steps of a major scale 

Note that no matter what the quantization switch is set to, the offset will always transpose in the steps of a major scale.

### Envelope output
I output a simple decay envelope with a potentiometer to set the decay time. At minimum decay time this signal is basically a trigger. The maximum decay time is approximately 5 seconds

### Modifying the output
I have a couple of ways to modify my output signals. All of these can be enabled per step by using the dedicated touch controls while the sequence is playing:
* Glide (Portamento): create a smooth transition between pitch CV. The glide potentiometer sets the glide time
* Choke: makes the decay envelope very short and percussive
* Rest: skip triggering a new envelope on this step.

There is also a clear control which removes any modifications on the current step.

## States
I have three states, A, B, & C. Each state remembers all of my parameters - so when you recall a state the pots and switches will most likely be at the wrong values! Any time you modify one of these it will update the value of the current state. You can press the save touch control to save the current state, or hold save and another state to copy the current state to that state (but really, that sounds more complicated than it needs to be).

## Controls
To make it clearer, here is a complete list of my inputs and outputs:

Inputs:
* Clock jack: increments the X and Y position according to the current ratio
* Reset jack: resets the sequence to the first (0:0) step
* Ratio pot: set the clock divider ratio X:Y. At 12:00 the ratio is 1:1 (every clock increments both X and Y). Past 12:00 the X divider increases to 1.5,2,3,4,5,6,7,8,10,12,16, infinity. Moving lower than 12:00 does the same for the Y dimension.
* Decay pot: sets the decay time of the envelope
* Glide pot: sets the glide time
* glide, choke, reset, clear touch controls: when the position moves to a step it checks to see if any of these are touched, and if so applies there values to that step. It remembers glide, choke, and rest settings for that step until the clear touch control is pressed for that step.
* A, B, C: recall saved states
* Save: save the current state, or if save and another state are held it copies the current state into the selected state

Outputs:
* pitch jack: quantized CV
* envelope: decay envelope
* X jack: outputs a trigger signal when I move in the X dimension
* Y jack: outputs a trigger signal when I move in the Y dimension
