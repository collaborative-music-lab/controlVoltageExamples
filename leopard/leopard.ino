#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <Mini_LiquidCrystal_I2C.h>
#include <usbh_midi.h>
#include <usbhub.h>
#include <controlVoltage.h>
#include "launchpad2.h"
#include "nifty.h"
#include "setup.h"


const byte clockPin = 2;
const byte resetPin = 4;

const byte SERIAL_DEBUG = 1;

/*
 *******************************************************************************
 Leopard MIDI sequencer
 based on the controlVoltage arduino library
 initial creation by Ian Hattwick 2023
 for FaMLE (the MIT laptop ensemble)

  takes input from a midi device and creates a sequence
 this example designed to use an Arturia launchpad mini Mk2 to sequence a niftyCase in drum mode
 - e.g. midi channel 10 to niftcase turns the 5 outputs into trigger outputs
 - maybe it could be 4 trigger outputs on channel 10, plus mod out on channel 1?

  To do:
 - TBD

The launchpad grid is split into 2 main sections:
- the bottom 4 rows define 8 beat rhythm patterns
  - pushing any of these buttons toggles that step on or off
  - the four ctl buttons to the right can select a pattern for assigning to a grouping (see below)
- the top four rows are pattern groupings for the 4 trigger outputs
- the right (vertical) ctl buttons select a pattern and effect
  - the bottom four select a pattern, the top four select an effect
- the top (horizontal) ctl buttons are for saving and recalling patterns
- the pattern groupings are in a 4x2 structure (x=4,y=2)
  - the bottom row of the grouping chains four rhythm patterns into a 32-beat pattern
    - pressing one of those buttons assigns the currently selected pattern to that slot
  - the top row of the grouping assigns an effect to the pattern below it
    - pressing one of those buttons assigns the currently selected effect to that slot

pattern effects are applied to a pattern within a grouping. There are four slots, selected with the top 4 ctl buttons
- pressing and holding a pattern select button allows you to modify the effect associated with that button
- the grid turns into vertical fader mode, where each column is an effect and the fader selects the strength of the effect
pattern effects and their parameter:
- slowdown (slowdown ratio)
- speedup (speedup ratio)
- random (percentage of note drop)
- beat repeat (beat to repeat)??
- rotate  (rotation amount)
- ???
- the last two faders might control the modulation output - lfo rate? random S&H?


to be updated:
LCD display:
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

  clearGrid();
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
    // seqChanADivide[i] = 4;
    // seqChanBDivide[i] = 4;
    seq[i].set(0,127);
    seq[i].set(1,100);
  }
  for(byte i=0;i<4;i++) seqDivide[i] = 4;

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
