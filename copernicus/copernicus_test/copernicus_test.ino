

/* Copernicus - 4x4 cartesian sequencer test
 *  - Ian Hattwick Aug 20 2022
 * 
 * Controls:
 * Pot0 (A2): envelope length
 * Pot1 (A3): glide time
 * Sw0 (4,7): FWD/STOP/REV
 * Sw1 (8,10): quantize mode (triad, pentatonic, major)
 * CV In 0 (A0) : pitch offset (pre-quantize, shifts in major scale degrees)
 * CV In 1 (A1) : divide ratio between X/Y channel. 
 * - Includes attenuator control to set basic offset
 * Out 0 (DAC 0) : Envelope
 * Out 1 (DAC 1) : Quantized CV
 * 
 * Trig Input (3)
 * Reset Input (5)
 * 
 * MUX Address pins : 0,1,13,12
 * Sequencer Pot input: A7
 * Sequence LED output: 11
 * 

 */

#include <controlVoltage.h>
#include "MCP47FEB.h"
#include "HI_MPR121.h"
#include "copernicus.h"

#include <avr/interrupt.h>

mcp47FEB dac(0x60);
int dacOutput[] = {0,0};
HI_MPR121 cap;

//declare CV objects with 10-bit depth
controlVoltage env = controlVoltage(10);
controlVoltage pitch = controlVoltage(10);

controlVoltage trig1 = controlVoltage();
controlVoltage trig2 = controlVoltage();

const byte SERIAL_DEBUG = 1;

void setup() {
  if ( SERIAL_DEBUG) Serial.begin(115200);

  if(!SERIAL_DEBUG) pinMode(0,OUTPUT);//Mux0
  if(!SERIAL_DEBUG) pinMode(1,OUTPUT);//Mux1
  pinMode(2,OUTPUT); //DAC
  pinMode(3,INPUT_PULLUP); //envelope led?
  pinMode(4,INPUT_PULLUP); //sw1a
  pinMode(5,OUTPUT); //pitch led?
  pinMode(6,INPUT_PULLUP); //trigX
  pinMode(7,INPUT_PULLUP); //sw1b
  pinMode(8,INPUT_PULLUP); //sw2a
  pinMode(9,INPUT_PULLUP); //trigY
  pinMode(10,INPUT_PULLUP); //sw2b
  pinMode(11,OUTPUT); //Mux3
  pinMode(12,INPUT_PULLUP); //
  pinMode(13,OUTPUT); //Mux2

  pinMode(A0,INPUT); //CV
  pinMode(A1,INPUT); //CV
  pinMode(A2,INPUT_PULLUP); //POT
  pinMode(A3,INPUT_PULLUP); //POT
//  pinMode(A4,OUTPUT); I2C SDA
//  pinMode(A5,OUTPUT); I2C SCL
  pinMode(A6,INPUT_PULLUP); //reset?
  pinMode(A7,INPUT); //Sequener Pot

  env.curve(2);
  dac.begin();
  capSetup();
  delay(100);

 
}


byte seqStep = 0;

void loop() {
  capLoop();
  monitorLoop();
//  env.loop();
//  pitch.loop();
//  
//  static int envDecay[2] = {100,100};
//  static int potVal[2];
//
//  static uint32_t timer = 0;
//  int interval = 20;
//  static byte swState[2] = {1,1};
//  static byte clockDivide = 1;
//  static byte numSteps = 8;
//
//  static byte channel = 0;
//  
//  static int stepOffset = 0;
//
//  static int muxChannel = 0;
//
//  static int clockReceived = 0 ;
//  int trigReceived = 0;
//  static byte curStep = 0;
//  static byte trigCounter = 0;
//
////read arduino inputs
//  if(millis()-timer > interval){
//    timer = millis();
//
//    potVal[0] = map(analogRead(pot[1]), potRange[0],potRange[1],0,1023);
//    potVal[1] = map(analogRead(pot[2]), potRange[0],potRange[1],0,1023);
//    potVal[0] = 1023 - constrain(potVal[0],0,1023);
//    potVal[1] = 1023 - constrain(potVal[1],0,1023);
//    envDecay[0] = map(potVal[0], 0, 1023, 25, 2000);
//    envDecay[1] = map(potVal[1], 0, 1023, 25, 2000);
//
//    int cvVal = map(analogRead(cv2), 0, 800, 0, 1023);
//    cvVal = constrain(cvVal,0,1023);
//    stepOffset = map(cvVal,0,1023,0,7);
//    //Serial.println("pot " + String(potVal[0]) + " " + String(potVal[1]) );
//
//
//    for(int i=0;i<2;i++){
//      if( digitalRead( sw[i][0]) == 0 ) swState[i] = 0;
//      else if ( digitalRead( sw[i][1]) == 0 ) swState[i] = 2;
//      else swState[i] = 1;
//    }
//    if(swState[1] == 0) clockDivide=1;
//    else if(swState[1] == 1) clockDivide=2;
//    else if(swState[1] == 2) clockDivide=4;
//
//    if(swState[0] == 0) numSteps=0;
//    else if(swState[0] == 1) numSteps=2;
//    else if(swState[1] == 2) numSteps=3;
//
////    Serial.print(clockDivide);
////    Serial.print(" ");
////    Serial.println(numSteps);
//
//    digitalWrite(led[2], LOW);
//  }//arduino inputs
//
//  if(muxReadEnable == 1) readMuxInputs();
//  else if (muxReadEnable == 2) setMuxLED(curStep);
//
// 
//  /*CLOCK*/  
//
//  static byte trigInput[2] = {1,1};
//  static byte prevTrigVal[2] = {1,1};
//  
//  byte curTrig[2] = {0,0};
//  trigInput[0] = analogRead(A7)>200;
//  trigInput[1] = digitalRead(0);
//  
////  for(int i=0;i<2;i++){
////    if(trigInput[i] == 0){
////      digitalWrite(led[i+2], HIGH);
////      delay(100);
////      digitalWrite(led[i+2], LOW);
////    }
////  }
//
//  
//
//  for(int i=0;i<2;i++){
//    if(prevTrigVal[i] != trigInput[i] && 
//      trigInput[i] == 1 &&
//      trigReceived == 0) {
//        trigReceived = 1;
//      }
//    prevTrigVal[i] = trigInput[i];
//  }
//  if( trigInput[0] == 0 ) trigReceived = 0;
//  if( trigInput[1] == 0 ) trigCounter = 0;
//
//  if(trigReceived) {
//    trigCounter++;
//    if(trigCounter >= 64) trigCounter = 0;
//    //if(trigCounter % clockDivide[0] == 0) clockReceived[0] = 1;
//    if(trigCounter % clockDivide == 0) clockReceived = 1;
//}
//  
//  if(clockReceived == 1){
//    clockReceived = -1;
//
//    env.AR(5,envDecay[0]);
//
//    pitch.midi(seqVals[seqStep][seqNum]/70);
//
//     seqStep = (seqStep) % 16;
//  }//clock received
//
//  //monitor output at signal rate
//  static uint32_t signalTimer = 0;
//  int signalInterval = 100;
//  if( millis()-signalTimer >= signalInterval){
//    signalTimer=millis();
//
//    dacOutput[0] =  env.get() ;
//    dacOutput[1] =  pitch.get() ;
//    
//    dacWrite();
//    
//    signalTimer = millis();
//    if(0){
//      Serial.print("pitch:");
//      Serial.print( dacOutput[0]);
//      Serial.print(",");
//      Serial.print("env:");
//      Serial.println( dacOutput[1] );
//    }
//  }//signal loop
}

void dacWrite(){
  dac.analogWrite(dacOutput[0],dacOutput[1]);
}

void monitorLoop(){
  static uint32_t timer=0;
  int interval = 50;

  static byte trigVal[4];
   byte trigPin[] = {6,9,12,A6};
   byte val = 0;
   for(int i=0;i<4;i++) {
    val = digitalRead(trigPin[i]);
    if(i==3) val = analogRead(trigPin[i]) > 500;
    if(val != trigVal[i]){
      Serial.println("trig " + String(trigPin[i]) + " " + val);
      trigVal[i] = val;
    }
   }

  if(millis()-timer>interval){
    timer=millis();

    //pots
    //Serial.println("Pot:\t" + String(analogRead(pot[0])) + "\t" + String(analogRead(pot[1])));
    Serial.println("CV:\t" + String(analogRead(cv1)) + "\t" + String(analogRead(cv2)));
    updateSwState();
    //Serial.println("Sw:\t" + String(swState[0]) + "\t" + String(swState[1]));
    
//    if ( SERIAL_DEBUG) Serial.end();
//    if ( SERIAL_DEBUG) delay(500);
    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);
    static int num = 0;
    for(int i=0;i<16;i++) readMuxInput(i);
    num++;
    num = num > 15 ? 0 : num;
//    setMuxLED(muxVals[num]/65);
//    if ( SERIAL_DEBUG) Serial.begin(115200);
//    if ( SERIAL_DEBUG) delay(500);
   
//    for(int i=0;i<16;i++){
//      Serial.print(String(muxVals[i]) + "\t");
//      if(i%4==3) Serial.println();
//    }
    //Serial.println(num);
    setMuxLED(num);
    //delay(200);
    ////setMuxLED(muxVals[num]/65);
    //delay(500);

    static byte capLed = 0;
    for(int i=0;i<4;i++) cap.setGPIO(i+4,((capLed+8) >> i) & 1);
    cap.updateGPIO();
    //Serial.println("Cap led " + String(capLed));
    capLed = capLed>7 ? 0 : capLed+1;
     
  }
}
