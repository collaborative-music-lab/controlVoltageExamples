//LCD Display
Mini_LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//Seqeuncer definition
const byte NUM_SEQS = 1;
const byte SEQ_LENGTH = 8;
Sequencer8bit<8> seq[NUM_SEQS]; 

byte cur_preset = 0;

//keeping track of indexes
byte main_index = 0;
byte seqIndex[7][2];
byte freeze_index = 0;
byte freeze_length = 4;
//subdividers
byte seqDivide[4];
byte seqChanBDivide[7];
char globalDivide = 0;
byte globalRotate = 0;
byte globalStop = 0;
byte globalRepeat = 0;

//clock parameters
//subdiv multiples clock input
uint16_t subdiv_interval;
uint8_t subdiv = 0;
uint8_t num_subdiv = 4; //must be 1 or greater

//selecting sequences to edit
byte drum=7;
byte seqchan=0; //last seq channel edited
byte seqdrum=0; //last synth voice edited
byte seqEnable[2] = {127,127};

byte patternSelect = 0;
byte effectSelect = 0;

//bass synth params
byte bassGain[2] = {127,100};
byte curNote = 0;
byte pitchOffset = 36;

//launchpad definitions
defLaunchpad2 lp;
const byte patternColor[4] = {16,48,50,51};
const byte effectColor[4] = {1,3,17,19};
const byte cursorColor = 32;

//nifty 
const defNifty nifty;

//USB Host
USB Usb;
USBHub Hub(&Usb);
USBH_MIDI  Midi(&Usb);

//MIDI output
SoftwareSerial MIDI(14, 5); // RX, TX
const byte noteON = 144;//144 = 10010000 in binary, note on command
const byte noteOFF = 128;//128 = 10000000 in binary, note off command
const byte bassON = 145;//144 = 10010000 in binary, note on command
const byte bassOFF = 129;//128 = 10000000 in binary, note off command
const byte ccMSG = 176; 

void MIDI_poll();

//Sequencer
uint32_t tempo = 100;
