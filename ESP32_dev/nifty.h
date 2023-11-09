/*
niftyCase.h

MIDI bindings for the cre8audio niftyCase.

Resonds to 4 kinds of MIDI messages:
- note on/off on Ch1 and Ch2 for the two cv outputs on niftyCase
- note on/off on Ch3 (paraphonic, not implemented here)
- note on/off on Ch10 - sends triggers out all 5 CV outputs 
- cc msg 1 (mod wheel) on channel 1
*/

struct defNifty{
  //Nifty receives MIDI notes on Chan1 for CV1 and Gate1, and Chan2 for CV2 and Gate2
  const byte NOTE1_ON = 144;
  const byte NOTE2_ON = 145;
  const byte CC = 176; //ch 1
  const byte NOTE[2] = {144,145};
  const byte DRUM_ON = 153; //ch 10
  //const byte DRUM_ON = 153; //ch 10
  const byte drum[5] = {36,37,38,39,40};
};