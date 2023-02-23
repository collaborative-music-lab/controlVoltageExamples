/*
cc bindings for the launchkey mini mk3

NOTE: the launchkey mini cannot send and receive on the same USB MIDI port, e.g.
- in order to control leds you must use 'midi cable 1' per the USBH_MIDI spec:
  - https://github.com/YuuichiAkagawa/USBH_MIDI/issues/55
- LED control example https://gist.github.com/llimllib/13a391bc52aea9af3c8e81a315647240
- launchpad programmers guide
- https://github.com/giezu/LaunchkeyMiniMK3 
  - example implementation and explanation
*/

struct defMidiMsg{
  byte num;
  byte val;
};

enum LK_BUTTONS {PAD, A, B, REC, PLAY, SHIFT};
enum LK_COLOR {OFF, WHITE, RED, GREEN, BLUE, ORANGE, PINK, PURPLE};
enum LK_FLASH {ON, BLINK, PULSE2};

const byte lk_color[] = {0, 3, 5, 17, 45, 9, 53, 44};

struct defLaunchkey{
  //to save memory we just store the cc for the first of 8 elements
	 byte dial1 = 21;
	 //the pads have staggered note numbers. . . so need to store them all :-(
   byte pad1 = 36;
   byte padOffset[16] = {4,5,6,7,12,13,14,15,0,1,2,3,8,9,10,11};
	 byte pad[16] = {40,41,42,43,48,49,50,51,36,37,38,39,44,45,46,47};
   byte padMap[16] = {8,9,10,11,0,1,2,3,12,13,14,15,4,5,6,7};
	 defMidiMsg mod = {1, 0};
	 defMidiMsg bend = {1, 0}; //uses special midi message type
	 defMidiMsg play = {115, 0};
	 defMidiMsg rec = {117, 0};
	 defMidiMsg stop = {105, 0};
	 defMidiMsg arrow = {104, 0};
   defMidiMsg trackL = {103, 0};
   defMidiMsg trackR = {102, 0};
   defMidiMsg shift = {108, 0};
  

	 //MIDI status bytes
	 const byte KEY_ON = 144; //chan 1
	 const byte PAD_ON = 153; //chan 10
  const byte LED = 153; //chan 10
	 const byte KEY_OFF = 128;
	 const byte PAD_OFF = 135;
	 const byte PITCHBEND = 224; 
	 const byte CC = 176;
   const byte CC16 = 176;

  //message to switch launchkey to control mode
  //this is necessary to be able to control the LEDs
  const byte controlMode[3] = {159, 12, 127}; //status (note on chan16), note#, velocity)
};

defLaunchkey key2;

