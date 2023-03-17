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

const byte SERIAL_DEBUG = 0;
uint32_t test_timer = 0;

/*
 *******************************************************************************
 Bobby MIDI sequencer
 - a fork of leonard and Lyra for the Launchpad v3 and the Dreadbox Dysmetria
 based on the controlVoltage arduino library
 initial creation by Ian Hattwick 2023
 for FaMLE (the MIT laptop ensemble)

v1.0 initial
- define pages for storing trigger sequences, pitch sequences, and CV sequences
- and a page for mixing and matching sequences. . . 
 
Arduino pinout (verify):
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
char name[] = "bobby sequencer";
 char version_num[] = "v. 1.0";

//const char deviceVendor[] = {"korg"};
//const char midiDevice[2] = {"nanoKey", "nanoKontrol2"};
void onInit()
{
  lcd.clear();
  char buf[20];
  uint16_t vid = Midi.idVendor();
  uint16_t pid = Midi.idProduct();

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

  delay(5000);
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
  for(byte i=0;i<4;i++) params.seqTiming[i] = 0;

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

  static uint32_t timer = 0;
  if(millis()-timer>0){
    timer = millis();
    processMidiPacket();
    processLED();
  }
  //modOutput();
  //looptimer();
}
