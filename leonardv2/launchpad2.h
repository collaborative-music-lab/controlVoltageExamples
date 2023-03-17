/*
launchpad2.h

cc bindings for the launchpad v2 (round aux buttons)
based on MK2 Programmer’s Reference Manual:
https://fael-downloads-prod.focusrite.com/customer/prod/s3fs-public/downloads/Launchpad%20MK2%20Programmers%20Reference%20Manual%20v1.03.pdf

available LED colors:
0 (0,0),  1 (1,0),  2 (2,0), 3 (3,0), 16 (0,1), 32 (0,2), 48 (0,3)
plus all combinations of [1,2,3] and [16,32,48]
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
   //ctlY are note messages
	 defMidiMsg ctlY[8] = {
	 	{8,0},
	 	{24,0},
	 	{40,0},
	 	{56,0},
	 	{72,0},
	 	{88,0},
	 	{104,0},
	 	{120,0}
	 };
	 //ctlX are the horizontal pads on the top
	 //the ctlX are sequential CC messages
	 defMidiMsg ctlX[8] = {
	 	{104,0},
	 	{105,0},
	 	{106,0},
	 	{107,0},
	 	{108,0},
	 	{109,0},
	 	{110,0},
	 	{111,0}
	 };
  
	 //MIDI status bytes
  //launchpad by default uses MIDI channel 1 for everything
	 const byte KEY_ON = 144;
	 const byte KEY_OFF = 128;
	 const byte CC = 176;
};