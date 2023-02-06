#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <usbh_midi.h>
#include <usbhub.h>
#include <controlVoltage.h>
#include "launchkey.h"
#include "niftyCase.h"

const byte SERIAL_DEBUG = 1;

const byte clockPin = 2;
const byte resetPin = 4;
/*
 *******************************************************************************
HARPY MIDI sequencer

 takes input from a midi keyboard and creates a sequence
 this example designed to use a Launchkey Mini to control a cre8audio Nifty Case
 - the nifty case is interesting as it has 2 CV/Gate outputs, on MIDI channel 1 & 2

To Do:
- almost everything
- fix note off messaging
- add dial funtionality
- change it so there is a 'record' mode which enters rests when no note ons are active
- maybe pads could be remapped in this case?

 Controls:
 Pads 0-7 are step enables for SeqA / CV1
 Pads 8-15 are step enables for SeqB / CV2
 the arrow button selects SeqA
 the stop button selects SeqB
 - holding arrow + stop selects both sequences

 play initiates beat repeat
 rec + pad reads a stored sequence into the currently selected sequencer
 rec + play + pad stores the sequence in the currently selected sequencer into that pad location

 dial 0-3 control SeqA, dial 0-4 control SeqB
 0/4: transpose
 1/5: subdivide
 2/6: rotate
 3/7: gate length

 touching the mod strip records a CV sequence in 16th notes
 - this sequence is low-passed? maybe
 - once the mod strip is touched it records for 8 beats
 - maybe this should be integrated with rec/play buttons for more nuanced control?
 - obviously much TBD

 pitchbend transposes the selected sequence momentarily.
 - the most recently selected note is the only one to have an effect

 the keyboard writes notes into the selected sequence(s)
 - when a note is down when the sequencer moves to a new step then that note is recorded into the sequence
 - obviously the 8-step limitation is arbitrary. . . . maybe it would be fun to allow for longer / changeable length sequences?


 pins:
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
LiquidCrystal_I2C lcd(0x23F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//controlVoltage
controlVoltage mod; //mod signal

const byte SEQ_LENGTH = 16;

Sequencer8bit<SEQ_LENGTH> seq[2];

byte seqEnable = 3; //bits control enable

byte cur_chan = 1;
byte keys_down = 0;
byte cur_note = 60;
byte cur_note_prev = 60; //might be interesting to keep track of last two notes pressed?
byte prev_note[2];

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

void onInit()
{
//  char buf[20];
//  uint16_t vid = Midi.idVendor();
//  uint16_t pid = Midi.idProduct();
//  sprintf(buf, "VID:%04X, PID:%04X", vid, pid);
//  Serial.println(buf); 
}

void setup()
{

  //pins setup
  pinMode(2, INPUT_PULLUP);
  pinMode(3,OUTPUT);
  digitalWrite(3,LOW);
  pinMode(4, INPUT_PULLUP);
  //pin 5 MIDI output
  pinMode(6, OUTPUT);
  digitalWrite(6,LOW);
  pinMode(7, OUTPUT);
  digitalWrite(7,HIGH);
  
  if (SERIAL_DEBUG) Serial.begin(115200);

  MIDI.begin(31250);

  if (Usb.Init() == -1) {
    while (1); //halt
  }//if (Usb.Init() == -1...
  delay( 200 );

  // Register onInit() function
  Midi.attachOnInit(onInit);

  //LCD
  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  //scanI2C();

  for(byte i=0;i<SEQ_LENGTH;i++){
    seq[0].setAux(0, i, 1 );
    //seq[1].setAux(0, i, 1 );
  }

}

void loop()
{
  //chan2.loop(); 
  Usb.Task();
  if ( Midi ) {
    MIDI_poll();
  }
  clockLoop();
  processNoteOff();
}
