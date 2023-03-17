//LCD Display
Mini_LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//Seqeuncer definition
const byte NUM_SEQS = 1;
const byte SEQ_LENGTH = 8;
Sequencer8bit<8> seq[NUM_SEQS]; 

byte cur_preset = 0;

struct defParams{
  byte seqEnable[4] = {127,127,127,127};
  byte seqSelect[4][4];
  byte seqDivide[4];
  byte seqTiming[4] = {0,0,0,0};
};
defParams params;

//keeping track of indexes
byte main_index = 0;
byte freeze_index = 0;
byte freeze_length = 4;
//subdividers
char globalDivide = 0;
byte globalRotate = 0;
byte globalStop = 0;
byte globalRepeat = 0;

//clock parameters
//subdiv multiples clock input
uint16_t subdiv_interval;
uint8_t subdiv = 1;
uint8_t num_subdiv = 2; //must be 1 or greater
uint16_t beat_length = 150;

//selecting sequences to edit
byte seqchan=0; //last seq channel edited
byte seqdrum=0; //last synth voice edited

byte patternSelect = 0;
byte effectSelect = 0;

//launchpad definitions
defLaunchpad2 lp;
const byte patternColor[4] = {51,48,17,3};
const byte effectColor[4] = {1,3,16,19};
const byte cursorColor = 16;
byte launchpad_page = 0;

//nifty 
const defNifty nifty;

//USB Host
USB Usb;
USBHub Hub(&Usb);
USBH_MIDI  Midi(&Usb);

//MIDI output
SoftwareSerial MIDI(14, 5); // RX, TX

void MIDI_poll();

//Sequencer
uint32_t tempo = 100;
