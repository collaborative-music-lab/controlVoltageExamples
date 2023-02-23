/*
O-coast.h

MIDI bindings for the make noise 0-coast.

THe 0-coast midi settings are set using the PGMA and PGMB buttons. Refer to the
0-coast manual to learn more about midi settings. I recommend the following settings:

(Page 2- PGMA pulses, MIDI_B Off) Legato mode is set to {OFF} <PGMB is off>
(Page 4- PGMA On, MIDI_B pulses) MIDI B CV source is Mod Wheel <PGMB pulses>
(Page 5- PGMA On, MIDI_B flashes) MIDI B gate source is Velocity>50 <PGMB flashes>


*/

struct defOCoast{
  //Nifty receives MIDI notes on Chan1 for CV1 and Gate1, and Chan2 for CV2 and Gate2
  const byte NOTE_ON = 144;
  const byte NOTE_OFF = 128;
  const byte CC = 176; //for mod wheel
};