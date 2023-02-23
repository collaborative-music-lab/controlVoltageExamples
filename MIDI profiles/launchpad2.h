/*
launchpad2.h

cc bindings for the launchpad v2 (round aux buttons)
based on MK2 Programmer’s Reference Manual:
https://fael-downloads-prod.focusrite.com/customer/prod/s3fs-public/downloads/Launchpad%20MK2%20Programmers%20Reference%20Manual%20v1.03.pdf
*/

struct defMidiMsg{
	//for midi controls for which we want to keep track of their current state
  byte num;
  byte val;
};

struct defLaunchpad2{
  //to save memory we just store the note# for the first pad for 8 rows
	 byte pad1[8] = {0,16,32,48,64,80,96,112};
	 //ctlY pads are the vertical row of pads on the right
	 byte ctlY[8] = {
	 	defMidiMsg(8,0),
	 	defMidiMsg(24,0),
	 	defMidiMsg(40,0),
	 	defMidiMsg(56,0),
	 	defMidiMsg(72,0),
	 	defMidiMsg(88,0),
	 	defMidiMsg(104,0),
	 	defMidiMsg(102,0)
	 };
	 //ctlX are the horizontal pads on the top
	 //the ctlX are sequential CC messages
	 byte ctlX[8] = {
	 	defMidiMsg(104,0),
	 	defMidiMsg(105,0),
	 	defMidiMsg(106,0),
	 	defMidiMsg(107,0),
	 	defMidiMsg(108,0),
	 	defMidiMsg(109,0),
	 	defMidiMsg(110,0),
	 	defMidiMsg(111,0)
	 };
  
	 //MIDI status bytes
	 const byte KEY_ON = 144;
	 const byte KEY_OFF = 128;
	 const byte CC = 176;
};