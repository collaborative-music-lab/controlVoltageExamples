/*
Pot1 A2
Pot2 A3
Sw1 4/7
Sw2 8/10
CV1 A0
CV2 A1
Out1 DAC
Out2 DAC

external header for trigger inputs and toggle switches
Mux address pins: 12,1,11,13
Mux analog Input: A6
 */

#include <controlVoltage.h>
#include "MCP47FEB.h"

const byte SERIAL_DEBUG = 0;

mcp47FEB dac(0x60);
int dacOutput[] = {0,0};

//declare env objects with 10-bit depth
controlVoltage env1 = controlVoltage(10);
controlVoltage env2 = controlVoltage(10);

controlVoltage trig1 = controlVoltage();
controlVoltage trig2 = controlVoltage();


byte cv1 = A0;
byte cv2 = A1;

byte pot1 = A2;
byte pot2 = A3;
int potRange[2] = {0,840};

byte sw[2][2] = {
  {4,7},
 {8,10}
};

byte extTrig = 0;
byte seqReset = 1;

//mux
byte muxAdd[] = {12,1,11,13};
byte muxIn = A6;

byte led[] = {3,5,6,9};

void setup() {
  if ( SERIAL_DEBUG) Serial.begin(115200);

  if(!SERIAL_DEBUG) pinMode(0,INPUT);
  pinMode(0,INPUT);
  if(!SERIAL_DEBUG) pinMode(1,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,INPUT_PULLUP);
  pinMode(8,INPUT_PULLUP);
  pinMode(9,OUTPUT);
  pinMode(10,INPUT_PULLUP);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);

  pinMode(A0,INPUT_PULLUP);
  pinMode(A1,INPUT_PULLUP);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
//  pinMode(A4,OUTPUT); I2C SDA
//  pinMode(A5,OUTPUT); I2C SCL
  pinMode(A6,INPUT_PULLUP);
  pinMode(A7,INPUT);

  env1.curve(2);
  env2.curve(2);
  dac.begin();
  delay(100);
}

void loop() {
  env1.loop();
  env2.loop();
  
  static int envDecay[2] = {100,100};
  static int potVal[2];

  static uint32_t timer = 0;
  int interval = 20;
  static byte swState[2] = {1,1};
  static byte clockDivide = 1;
  static byte numSteps = 8;

  //int tempo = map(potVal[0],850, 20,500, 20);

  byte muxMap[] = {15,14,11,10,13,12,8,9,6,7,5,3,2,4,1,0};
  static byte channel = 0;
  static int muxVals[16];
  static int stepOffset = 0;

  static int muxChannel = 0;

//read arduino inputs
  if(millis()-timer > interval){
    timer = millis();

    potVal[0] = map(analogRead(pot1), potRange[0],potRange[1],0,1023);
    potVal[1] = map(analogRead(pot2), potRange[0],potRange[1],0,1023);
    potVal[0] = 1023 - constrain(potVal[0],0,1023);
    potVal[1] = 1023 - constrain(potVal[1],0,1023);
    envDecay[0] = map(potVal[0], 0, 1023, 25, 2000);
    envDecay[1] = map(potVal[1], 0, 1023, 25, 2000);

    int cvVal = map(analogRead(cv2), 0, 800, 0, 1023);
    cvVal = constrain(cvVal,0,1023);
    stepOffset = map(cvVal,0,1023,0,7);
    //Serial.println("pot " + String(potVal[0]) + " " + String(potVal[1]) );

    //Serial.print(String(muxChannel) + " ");
    for(int j=0;j<4;j++){
      //Serial.print(String((muxChannel>>j)&1) + " ");
      digitalWrite(muxAdd[j], (muxMap[muxChannel]>>j)&1);
    }

    muxVals[muxChannel] = analogRead(muxIn);
    //Serial.println(muxVals[muxChannel]);
  
    muxChannel = muxChannel >= 15 ? 0 : muxChannel + 1;

    for(int i=0;i<2;i++){
      if( digitalRead( sw[i][0]) == 0 ) swState[i] = 0;
      else if ( digitalRead( sw[i][1]) == 0 ) swState[i] = 2;
      else swState[i] = 1;
    }
    if(swState[1] == 0) clockDivide=1;
    else if(swState[1] == 1) clockDivide=2;
    else if(swState[1] == 2) clockDivide=4;

    if(swState[0] == 0) numSteps=0;
    else if(swState[0] == 1) numSteps=2;
    else if(swState[0] == 2) numSteps=3;

//    Serial.print(clockDivide);
//    Serial.print(" ");
//    Serial.println(numSteps);

    digitalWrite(led[3], LOW);
  }//arduino inputs

 
  /*CLOCK*/  
  //trigInput [0] = clock, [1] = reset
  static byte trigInput[2] = {1,1};
  static byte prevTrigVal[2] = {1,1};

  trigInput[1] = analogRead(A7)>700;
  trigInput[0] = digitalRead(0);
  for(int i=0;i<2;i++) trigInput[i] = trigInput[i]==0; //invert inputs

  //Serial.println("\t\t\t trigRaw " + String(trigInput[0]) + " " + String(trigInput[1]));
  //delay(20);
//  digitalWrite(led[3],trigInput[0]); //clock led
//  digitalWrite(led[2],trigInput[1]); //reset led
  
  static int clockReceived = 0 ;
  static int resetReceived = 0 ;
  static int trigReceived = 0;
  static byte curStep = 0;
  static byte trigCounter = 0;

//Serial.println("trig1 " + String(prevTrigVal[1]) + " " + String(trigInput[1]) + " " + String(resetReceived));
  
  if((prevTrigVal[1] == 0) && (trigInput[1] == 1) && (resetReceived==0)){
    resetReceived = 1;
    prevTrigVal[0] = 0;
    trigReceived = 0;
    if ( SERIAL_DEBUG)Serial.println("reset received");
  }
  //Serial.println("trig0 " + String(prevTrigVal[0]) + " " + String(trigInput[0]) + " " + String(trigReceived));
  if((prevTrigVal[0] == 0) && (trigInput[0] == 1) && (trigReceived==0)){
    trigReceived = 1;
    //Serial.println("trigger received");
  }
  for(int i=0;i<2;i++) prevTrigVal[i] = trigInput[i];
 
  if( trigInput[0] == 0 ) trigReceived = 0;

  if(trigReceived) {
    trigReceived = 0;
    trigCounter++;

    if(resetReceived==1){
        resetReceived = 0;
        trigCounter = 0;
        curStep = 0;
      }

    if ( SERIAL_DEBUG) Serial.println("trigReceived " + String(trigCounter) );
    if(trigCounter >= 64) trigCounter = 0;
    if(trigCounter % clockDivide == 0) clockReceived = 1;
    else clockReceived = 0;
  }//trigReceived
  
  if(clockReceived == 1){
    clockReceived = 0;
    if ( SERIAL_DEBUG) Serial.println("curStep " + String(curStep) + " offset " + String(stepOffset));
     /*update clock LED*/
    digitalWrite(led[3], HIGH);

    /*check current value for this step*/
    byte sumStep = (curStep + stepOffset) % 8;
    /*indicate if we are at step 1*/
    if(sumStep == 0) digitalWrite(led[2], HIGH);
    else  digitalWrite(led[2],LOW);
    
    byte hatStep = (sumStep + numSteps) % 8;
    byte kickVal = muxVals[sumStep*2]<200;
    byte hatVal = muxVals[hatStep*2+1]<200;
    //Serial.println("sum " + String(sumStep) + " hat " + String(hatStep));
    
    /*then write current step to output*/
    if(kickVal > 0){
      env1.AR(5,envDecay[0]);
    }
    if(hatVal > 0){
      env2.AR(5,envDecay[1]);
    }

    if(curStep >= 7 ) curStep = 0;
    else curStep += 1;
  }//clock received

  //monitor output at signal rate
  static uint32_t signalTimer = 0;
  int signalInterval = 1;
  if( millis()-signalTimer >= signalInterval){
    signalTimer=millis();

    dacOutput[0] =  env1.get() ;
    dacOutput[1] =  env2.get() ;
    analogWrite(led[0], dacOutput[0]>>4);
    analogWrite(led[1], dacOutput[1]>>4);
    
    dacWrite();
    
    signalTimer = millis();
    if(0){
      Serial.print("cv:");
      Serial.print( dacOutput[0]);
      Serial.print(",");
      Serial.print("trig:");
      Serial.println( dacOutput[1] );
    }
  }//signal loop
}


void dacWrite(){
  dac.analogWrite(dacOutput[0],dacOutput[1]);
}
