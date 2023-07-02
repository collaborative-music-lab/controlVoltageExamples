struct defParams{
  byte seqLength[2] = {32,32};
  byte seqOffset[2] = {0,0};
  byte seqDivide[2] = {4,4};
  byte seqMode[2] = {0,0};
  byte seqStart[2] = {0,16};
  byte seqEnd[2] = {15,31};
  byte index[2];
  byte inc[2] = {1,1};
  byte resetCondition[2] = {0,1}; //0=external, 1=other sequence
  char transpose[2] = {0,0};
};
defParams params;

//LCD Display
LiquidCrystal_I2C lcd(0x23F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//controlVoltage
controlVoltage mod; //mod signal

const byte SEQ_LENGTH = 32;

Sequencer8bit<SEQ_LENGTH> seq[2];

byte seqEnable = 0; //bits control enable

byte cur_chan = 3;
byte keys_down = 0;
byte cur_note = 60;
byte cur_note_prev = 60; //might be interesting to keep track of last two notes pressed?
byte prev_note[2];
byte cur_pitch_preset = 0;
byte cur_seq_preset = 0;

//keeping track of indexes
byte main_index = 0;
byte seqIndex[2];
byte freeze_index = 0;
byte freeze_length = 4;
//subdividers
byte seqDivide[2];
char globalDivide = 0;
byte globalRotate = 0;
byte globalStop = 0;
byte globalRepeat = 0;
uint16_t beat_length = 150;
byte reset_flag = 0;

//clock parameters
//subdiv multiples clock input
uint8_t subdiv = 0;
uint8_t num_subdiv = 4; //must be 1 or greater

//USB Host
USB Usb;
USBHub Hub(&Usb);
USBH_MIDI  Midi(&Usb);

defLaunchkey key;
defNifty nifty;

//MIDI output
SoftwareSerial MIDI(14, 5); // RX, TX

void MIDI_poll();

//Sequencer
uint32_t tempo = 100;