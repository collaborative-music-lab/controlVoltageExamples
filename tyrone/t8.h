enum T8_Seqs {KICK, SNARE, CLAP, CHAT, OHAT, TOM, BASS, GLOBAL}; 
//byte drum=0;
const String T8_names[8] = {"KICK", "SNARE", "CLAP", "CHAT", "OHAT", "TOM", "BASS", "GLOBAL"};

struct defT8{
  const byte bd = 36;
  const byte sd = 38;
  const byte hc = 50;
  const byte ch = 42;
  const byte oh = 46;
  const byte tom = 45;
  const byte bass = 2;
  const byte seq[7] = {bd,sd,hc,ch,oh,tom,bass};
  const byte noteON = 153;
};
