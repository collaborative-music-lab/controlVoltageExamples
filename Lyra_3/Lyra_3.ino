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
uint32_t test_timer = 0;

char name[] = "lyra sequencer";
 char version_num[] = "v. 3.0";

/*
 *******************************************************************************
 Lyra MIDI sequencer
 - a fork of Leonard for the Launchpad v3 and the Dreadbox Dysmetria
 based on the controlVoltage arduino library
 initial creation by Ian Hattwick 2023
 for FaMLE (the MIT laptop ensemble)

  v1.0 initial
 v2.0:
 - added buffering for LED msgs
 - changed buffering of output MIDI msgs for better performance
 - all outputs are now gates for the full length of the subdivision
 v3.0
 - adds support for midi clock, mod output, and piutch CV output

  takes input from a midi device and creates a sequence
 this example designed to use an Arturia launchpad mini Mk2 to sequence a niftyCase in drum mode
 - e.g. midi channel 10 to niftcase turns the 5 outputs into trigger outputs
 - maybe it could be 4 trigger outputs on channel 10, plus mod out on channel 1?

  To do:
 - update LED updates for enable, divide, and patternSquence (top 4 rows) when preset is changed
 - add cursors on pattern grouping rows to indicate which pattern is playing
 - enable toggling a pattern on/off, allowing for pattern rests
  *  e.g. tapping a pattern grouping button twice disables that pattern, so the sequence doesn't play for that 8 beats
 - implement mod wheel functionality
  * maybe have ctlX[7] switch to mod mode?
  * enable using grid as sliders for setting mod value
  * so many options for how to do this. . . .
 - schedule LCD msgs to not interfere with timing

The launchpad grid is split into 2 main sections:
- the bottom 4 rows define 8 beat rhythm patterns
  - pushing any of these buttons toggles that step on or off
  - the four ctl buttons to the right of the patterns select a pattern for assigning to a grouping (see below)
- the top four rows are pattern groupings for the 4 trigger outputs
- the pattern groupings are in a 4x2 structure (x=4,y=2)
  - the bottom row of the grouping chains four rhythm patterns into a 32-beat pattern
    - pressing one of those buttons assigns the currently selected pattern to that slot
  - the top row of the grouping sets grouping enable (red button) or subdivide (orange buttons)
- the top (horizontal) ctl buttons are for saving and recalling patterns

The top 4 ctl buttons select:
0 (top): hold and press a horizontal ctl button to save a preset
1: subdivide all seqs by 2
2: beat repeat
3: pause

OLD IDEAS BELOW - NOT CURRENT
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
  //lcd.write(0);  
   delay(500);
  lcd.clear();

  clearGrid();

  for(byte i=0;i<4;i++) {
    params.seqDivide[i] = 4;
    updateGridLed(i*4 + 2 + (i>1)*8, 1 , effectColor[2]); //seqDivide
    updateGridLed(i*4 + (i>1)*8, 1, effectColor[0]); //seqEnable

    for(byte j=0;j<4;j++){
      params.seqSelect[i][j] = i;
      updateGridLed(i*4 + (i>1)*8 + 8 + j, 1, patternColor[i]);
    }
  }

  lcd_string(name,0, 0);
  lcd_string(version_num,0, 1);
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

  //scanI2C();

  if( SERIAL_DEBUG ) Serial.println("setup complete");

  eepromSetup();

  for(int i=0;i<4;i++) {
    params.seqTiming[i] = 0;
    params.seqEnable[i] = 127;
  }

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

  static uint32_t timer = 0;
  if(millis()-timer>0){
    timer = millis();
    processNoteOff();
    processMidiPacket();
    processLED();
  }
  //modOutput();
  //looptimer();
}
