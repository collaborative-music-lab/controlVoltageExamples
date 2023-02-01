/*
cc bindings for the launchkey mini
*/
struct defMidiMsg{
  byte num;
  byte val;
};

struct defLaunchkey{
  //to save memory we just store the cc for the first of 8 elements
	 byte dial1 = 21;
	 //the pads have staggered note numbers. . . so need to store them all :-(
	 byte pad[16] = {40,41,42,43,48,49,50,51,36,37,38,39,44,45,46,47};
	 defMidiMsg mod = {1, 0};
	 defMidiMsg bend = {1, 0}; //uses special midi message type
	 defMidiMsg play = {115, 0};
	 defMidiMsg rec = {117, 0};
	 defMidiMsg stop = {105, 0};
	 defMidiMsg arrow = {104, 0};
   defMidiMsg trackL = {103, 0};
   defMidiMsg trackR = {102, 0};
  

	 //MIDI status bytes
	 const byte KEY_ON = 144;
	 const byte PAD_ON = 153;
	 const byte KEY_OFF = 128;
	 const byte PAD_OFF = 135;
	 const byte PITCHBEND = 224; 
	 const byte CC = 176;
};