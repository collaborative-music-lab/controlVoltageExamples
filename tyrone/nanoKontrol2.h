/*
cc bindings for the korg nanoKontrol2

*/
struct defMidiMsg{
  byte num;
  byte val;
};

struct defKorgNanokontrol2{
  //to save memory we just store the cc for the first of 8 elements
	 byte s1 = 32;
	 byte m1 = 48;
	 byte r1 = 64;
	 byte fader1 = 0;
	 byte dial1 = 16;
	 defMidiMsg stop = {42, 0};
	 defMidiMsg play = {41, 0};
	 defMidiMsg rew = {43, 0};
	 defMidiMsg ff = {44, 0};
	 defMidiMsg rec = {45, 0};
	 defMidiMsg markerL = {61, 0};
	 defMidiMsg markerR = {62, 0};
	 defMidiMsg set = {60, 0};
	 defMidiMsg cycle = {46, 0};
	 defMidiMsg trackL = {58, 0};
	 defMidiMsg trackR = {59, 0};
};
