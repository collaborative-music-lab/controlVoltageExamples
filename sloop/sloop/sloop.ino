/* Sloop
 * dual function generator
 * eurorack module based on controlVoltage Arduino library
 * Ian Hattwick
 * March 2022
 * 
 * 
Pin mappings:
Pot1 A2
Pot2 A3
Sw1 4/7
Sw2 8/10
CV1 A0
CV2 A1
Out1 DAC
Out2 DAC
 */

#include <controlVoltage.h>
#include "MCP47FEB.h"

mcp47FEB dac(0x60);
int dacOutput[] = {0,0};

//declare env objects with 10-bit depth
controlVoltage env1 = controlVoltage(10);
controlVoltage env2 = controlVoltage(10);

controlVoltage trig1 = controlVoltage();
controlVoltage trig2 = controlVoltage();

const byte SERIAL_DEBUG = 1;

byte cv1 = A0;
byte cv2 = A1;

byte potPin[2] = {A2,A3};
int potRange[2] = {0,512};

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

//  if(!SERIAL_DEBUG) pinMode(0,INPUT);
//  pinMode(0,INPUT);
//  if(!SERIAL_DEBUG) pinMode(1,OUTPUT);
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

  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
//  pinMode(A4,OUTPUT); I2C SDA
//  pinMode(A5,OUTPUT); I2C SCL
  pinMode(A6,INPUT_PULLUP);
  pinMode(A7,INPUT);

  env1.curve(1);
  env2.curve(1);
  dac.begin();
  delay(100);
  env1.sampleRate = 1000;
  env2.sampleRate = 1000;
  Serial.println("setup complete");
}

int riseTime = 100;
  int fallTime = 100;
  int riseTime2 = 100;
  int fallTime2 = 100;

void loop() {
  env1.loop();
  env2.loop();
  
  static int envRiseFall[2] = {100,100};
  static int potVal[2];


  static byte swState[2] = {1,1};
  static int slopeTime = 1; 

  static uint32_t timer = 0;
  int interval = 100;

//read arduino inputs
  if(millis()-timer > interval){
    timer = millis();
    
//    Serial.println("env trigger");

    /*CV*/
    int cvVal1 = map(analogRead(cv1), 0, 800, 0, 1023);
    cvVal1 = constrain(cvVal1,0,1012);
    //Serial.println("cv " + String(cvVal1) + " " + String(cvVal2) );
    float env2scalar = (float)cvVal1/1023 + 0.01;

    /*SWITCHES*/
    for(int i=0;i<2;i++){
      if( digitalRead( sw[i][0]) == 0 ) swState[i] = 0;
      else if ( digitalRead( sw[i][1]) == 0 ) swState[i] = 2;
      else swState[i] = 1;
    }
    if(swState[1] == 0) {env1.curve(0.5); env2.curve(0.5);}
    else if(swState[1] == 1) {env1.curve(1); env2.curve(1);}
    else if(swState[1] == 2) {env1.curve(8); env2.curve(8);}

    if(swState[0] == 0) slopeTime = 1;
    else if(swState[0] == 1) slopeTime = 8;
    else if(swState[0] == 2) slopeTime = 8;

    if( swState[0] == 2) {env1.cycle = 1; env2.cycle = 1;}
    else { env1.cycle = 0; env2.cycle = 0;}

    /*POTS*/
    for(int i=0;i<2;i++){
      potVal[i] = map(analogRead(potPin[i]), potRange[0],potRange[1],0,1023);
      potVal[i] = 1023 - constrain(potVal[i],0,1023);
      //Serial.println( String(potVal[i]));
      //if(potVal[i] > 100) 
        //potVal[i] = ((pow((float(potVal[i]-50))/923.,1.5)))* 1900 + 50;
    }
    riseTime = potVal[0] * slopeTime;
    fallTime = potVal[1]*2 * slopeTime;
    env1.riseTime( riseTime);
    env1.fallTime( fallTime);
    riseTime2 = ( riseTime * env2scalar );
    fallTime2 = ( fallTime * env2scalar);
    env2.riseTime(riseTime2);
    env2.fallTime( fallTime2);

   

//    Serial.println(String(digitalRead(4)) + String(digitalRead(7)) + 
//    String(digitalRead(8)) + String(digitalRead(10)));
    //Serial.println( String(swState[0]) + " " + String(swState[1]));
   // Serial.println( String(slopeTime) + " " + String(swState[1]));
 
//   Serial.println( String(fallTime) + " " + String(fallTime2));
//   Serial.println( String(riseTime) + " " + String(riseTime2));
//   Serial.println();
  }//arduino inputs

 
//  /*CLOCK*/  
  static byte trigInput;
  static byte prevTrig;
  static byte trigReceived = 0;
  
  byte curTrig=0;
  trigInput = digitalRead(cv2);

  static uint32_t trigLedTimer = 0;
  int trigLedInterval = 100;
  byte trigLedState = 0;

  if(prevTrig != trigInput && 
    trigInput == 1) trigReceived = 1;
  else trigReceived = 0;

  prevTrig = trigInput;
  


  if(trigReceived) {
    digitalWrite(9,HIGH);
    trigLedTimer = millis();
    trigLedState = 1;;
    //Serial.println("trig");
    env1.AR(riseTime,fallTime);
    env2.AR(riseTime2, fallTime2);
  }

  if(trigLedState == 0 && (millis()-trigLedTimer > trigLedInterval)){
    trigLedState = 0;
    digitalWrite(9,LOW);
  }
  


  /*CV OUTPUT*/
  static uint32_t signalTimer = 0;
  int signalInterval = 1;
  if( millis()-signalTimer >= signalInterval){
    signalTimer=millis();

    dacOutput[0] =  env1.get() ;
    dacOutput[1] =  env2.get() ;

    dacWrite();
    analogWrite(3, dacOutput[0]/4);
    analogWrite(5, dacOutput[1]/4);
    
    if(0){
      //Serial.print("cv:");
     Serial.println( dacOutput[0]);
     Serial.print(",");
      //Serial.print("trig:");
      Serial.println( dacOutput[1] );
    }
  }
}

void dacWrite(){
  dac.analogWrite(dacOutput[0],dacOutput[1]);
}
