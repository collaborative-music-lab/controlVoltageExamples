#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <Mini_LiquidCrystal_I2C.h>
//#include <LiquidCrystal_I2C_Hangul.h>
#include <usbh_midi.h>
#include <usbhub.h>
#include <controlVoltage.h>
#include "nanoKontrol2.h"
#include "t8.h"


const byte clockPin = 2;
const byte resetPin = 4;

const byte SERIAL_DEBUG = 0;

/*
 *******************************************************************************
 Tyrone MIDI sequencer
 based on the controlVoltage arduino library
 initial creation by Ian Hattwick 2023
 for FaMLE (the MIT laptop ensemble)

  takes input from a midi device and creates a sequence
 this example designed to use a korg nanoKontrol to sequence a Roland T8

  To do:
 - !IMPORTANT: find a way to scale velocity range for each voice
 - fix double triggering on longer clock divisoins (>4)
 - seq enable buttons in global mode should enable for changing subdivide - its wonky now
 - some unused buttons
 - lots of weird LED behaviour
 - make rotate knob only affect selected channel?
 - some way of changing freeze_length?
 - make the lowest pitch of the bass equal to the root (0) of the scale in Sequencer()
 - make repeat affect the previous beat (the one you just heard) rather than the upcoming beat
 - test preset storage. . .
 - some way to clear a sequence?
 - subdivide 3 actually generates [3,3,2] pattern. This might be worth fixing?

 6 drum, 1 bass, and 1 global channel
 Select channels using the 'R' buttons
 - ch0: Kick drum
 - ch1: Snare drum
 - ch2: hand claP
 - ch3: Closed hat
 - ch4: Open hat
 - ch5: Tom
 - ch6: Bass
 - ch7: Global (also toggle to global by deselecting any channel)

Each channel has:
- 2 8-step rhythm sequencers, seqA and seqB
  - row 'S' and 'M' set enables for seqA and seqB respectively
  - after interacting with either seq you can change its suddivide using the trackL and trackR buttons
  - tyrone has a built in x4 clock multiplier. Subdivide of 4 will follow the clock
- one dial sets velocity of seqA and seqB
  - with dial in middle both seqs are full velocity - rotating L or R attenuates one channels velocity
  - if seqA and seqB both have a trigger, the final velocity is the max of seqA or seqB

The bass channel also has a pitch sequencer using the faders
- the faders always affect the bass pitch, no matter what channel is selected
- when a bass step is active, its fader sets the pitch
- the final pitch is the sum of seqA and seqB's pitch
- with velocity knob turned hard L or R, seqA or seqB will trigger note-offs for rests. . 

The global channel lets you enable/disable seqA and seqB for each channel
- toggle the seqs on and off

Transport controls:
- rew and ff speed up or slow down the sequence
- the stop button pauses the sequencer
- the play button repeats the current beat

Preset management
- 8 presets are available
- hold Record and press one of the 'R' buttons to recall a preset
- hold CYCLE and press one of the 'R' buttons to store a preset
- the current preset is displayed on the top left of the LCD

LCd display:
- the LCD displays are:
- (top row) 
  - first two chars: [selected channel, presetNumber]
  - channel params for seqA: [indicate if seqA is active for channel subdivide, subdivide for seqA, seqA velocity]
  - seqA enables for all channels
- (bottom row) 
  - first two digits: [note number for bass synth]
  - channel params for seqB: [indicate if seqB is active for channel subdivide, subdivide for seqB, seqB velocity]
  - seqB enables for all channels



Ardunio pinout (verify):
 midi sequencer pins:
 d0,d1: USB serial debug
 
 d2: trigger input
 d3: GND
 d4: reset input
 
 d5: 3.5mm midi out
 d6: GND
 d7: 5v
 
 *******************************************************************************
 */

//LCD Display
Mini_LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//Mini_LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C_Hangul lcd(0x3F,16,2); 

const byte NUM_SEQS = 7;
const byte SEQ_LENGTH = 8;
    
Sequencer8bit<8> seq[NUM_SEQS]; 

byte cur_preset = 0;

//keeping track of indexes
byte main_index = 0;
byte seqIndex[7][2];
byte freeze_index = 0;
byte freeze_length = 4;
//subdividers
byte seqChanADivide[7];
byte seqChanBDivide[7];
char globalDivide = 0;
byte globalRotate = 0;
byte globalStop = 0;
byte globalRepeat = 0;

//clock parameters
//subdiv multiples clock input
uint16_t subdiv_interval;
uint8_t subdiv = 0;
uint8_t num_subdiv = 2; //must be 1 or greater
uint16_t beat_length = 150;
uint8_t reset_flag = 0;
uint8_t clock_state = 0;

//selecting sequences to edit
byte drum=7;
byte seqchan=0; //last seq channel edited
byte seqdrum=0; //last synth voice edited
byte seqEnable[2] = {127,127};

//bass synth params
byte bassGain[2] = {127,100};
byte curNote = 0;
byte pitchOffset = 36;

defKorgNanokontrol2 nano;
const defT8 t8;

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

//const char deviceVendor[] = {"korg"};
//const char midiDevice[2] = {"nanoKey", "nanoKontrol2"};
void onInit()
{
  lcd.clear();
  char buf[20];
  uint16_t vid = Midi.idVendor();
  uint16_t pid = Midi.idProduct();
//  switch(vid){
//    //case 0x0944: lcd.print("Korg "); break;
//    default: lcd.print("Unknown vender " + String(vid)); break;
//  }
//  switch (pid){
//    //case 0x0117: lcd.print("nanoKontrol2 "); break;
//    default: lcd.print("Unknown device " + String(pid)); break;
//  }
  //sprintf(buf, "VID:%04X, PID:%04X", vid, pid);
  //lcd.println("connected"); 
  lcd.setCursor(0,1);
  //lcd.print("midi sequencer");  
  lcd.write(0);  
   delay(500);
  lcd.clear();
}

void setup()
{
  //pins setup
  pinMode(clockPin, INPUT_PULLUP);
  pinMode(3,OUTPUT);
  digitalWrite(3,LOW);
  pinMode(resetPin, INPUT_PULLUP);
  //pin 5 MIDI output
  pinMode(6, OUTPUT);
  digitalWrite(6,LOW);
  pinMode(7, OUTPUT);
  digitalWrite(7,HIGH);
  
  if( SERIAL_DEBUG ) Serial.begin(115200);

  //LCD
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  if( SERIAL_DEBUG ) Serial.begin(115200);
  //for(int i=0;i<8;i++) lcd.createChar(i+1 , customchar[i]);
  
    MIDI.begin(31250);
//
  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );

  // Register onInit() function
  Midi.attachOnInit(onInit);

  for(byte i=0;i<NUM_SEQS;i++) {
    //seq[i].subDivide = 4;
    seqChanADivide[i] = 4;
    seqChanBDivide[i] = 4;
    seq[i].set(0,127);
    seq[i].set(1,100);
  }

  //scanI2C();

  if( SERIAL_DEBUG ) Serial.println("setup complete");

  eepromSetup();
}

void loop()
{
  //chan2.loop(); 
  //Sequencer();
  Usb.Task();
  if ( Midi ) {
    MIDI_poll();
  }
  clockLoop();
}
