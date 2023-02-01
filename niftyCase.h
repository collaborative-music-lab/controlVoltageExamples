enum T8_Seqs {KICK, SNARE, CLAP, CHAT, OHAT, TOM, BASS, GLOBAL}; 

struct defNifty{
  //Nifty receives MIDI notes on Chan1 for CV1 and Gate1, and Chan2 for CV2 and Gate2
  const byte NOTE1 = 144;
  const byte NOTE2 = 145;
  const byte CC = 176;
  const byte NOTE[2] = {144,145};
};