enum T8_Seqs {KICK, SNARE, CLAP, CHAT, OHAT, TOM, BASS, GLOBAL}; 

struct defNifty{
  //Nifty receives MIDI notes on Chan1 for CV1 and Gate1, and Chan2 for CV2 and Gate2
  const byte NOTE1_ON = 144;
  const byte NOTE2_ON = 145;
  const byte CC = 176;
  const byte NOTE_ON[2] = {144,145};
  const byte NOTE_OFF[2] = {128,129};
};
