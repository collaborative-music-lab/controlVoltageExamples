#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <usbh_midi.h>
#include <usbhub.h>
#include <controlVoltage.h>
#include "launchkey.h"
#include "niftyCase.h"
#include "setup.h"
#include <EEPROM.h>


const byte SERIAL_DEBUG = 1;

const byte clockPin = 2;
const byte resetPin = 4;

const char INSTRUMENT_DEFINITION = "Harpy v2.0";

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

void onInit()
{
  delay(250);
  lcd.clear();
  delay(250);
  lcd_string(INSTRUMENT_DEFINITION, 0,0);

  USBmessage2(key.controlMode[0], key.controlMode[1], key.controlMode[2]);
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

  seq[0]._aux[0] = 0;
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
