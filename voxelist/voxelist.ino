/* Voxelizer - quantizer built using the controlVoltage arduino library
 *  Ian Hattwick
 *  August 24, 2022
 *  
 *  Controls:
 *  
 *  Root/Glide knob
 *  - in Root mode, causes 0v to correspond to different notes in the current scale
 *  - in glide mode, causes glide/pormaneto when either the main or offset quantized pitches change
 *  Envelope Decay knob
 *  - an envelope is triggered everytime the main CV detects a new note
 *  - env is not triggered on new offset
 *  Scale switch
 *  - selects between diatonic scale / pentatonic scale / triad
 *  Root/glide mode switch
 *  - sets the mode for the Root/Glide knob between 'root'/'main glide'/'offset glide'
 *  - order is main/root/offset (with root in the middle)
 *  - root value persists when changing to glide modes
 *  - and new root value are only applied when a new main quantized pitch is triggered
 *  - e.g. when an envelope is triggered
 *  - glide values persist when switching to root, but cleared when switching to the
 *    opposite glide mode
 *  Main CV input to quantizer
 *  - CV input here is quantized to the current scale
 *  Quantizer offset CV, with attenuator
 *  - CV input here creates an offset added to the current pitch. This offset is always set to shit diatonically. 
 *  - e.g. using a triad quantization you can shift the triad to any chord in a major key
 *  Main quantizer output
 *  Envelope output
 *  
 *  
Pin mapping
Pot1 A2 - Root/glide
Pot2 A3 - Decay
Sw1 4/7 - scale
Sw2 8/10 -root/glide mode
CV1 A0 - main CV input
CV2 A1 - offset CV input
Out1 DAC - quantized output
Out2 DAC - envelope output

external header for trigger inputs and toggle switches
Mux address pins: 12,1,11,13
Mux analog Input: A6
 */
const byte SERIAL_DEBUG = 0;

#include <controlVoltage.h>
#include "MCP47FEB.h"

//arduino pins
//added before includes to make them available to included files
byte cv1 = A0;
byte cv2 = A1;

byte pot1 = A2;
byte pot2 = A3;
int potRange[2] = {0,500};

byte sw[2][2] = {
  {8,10},
  {4,7}
};
byte curSw[2] = {255,255};

byte led[] = {3,5,6,9};

//DAC setup
mcp47FEB dac(0x60);
int dacOutput[] = {0,0};
const byte dacBitDepth = 10;

//declare CV objects with 10-bit depth
controlVoltage quantized = controlVoltage(dacBitDepth);
controlVoltage quantizeOffset = controlVoltage(dacBitDepth);
controlVoltage env = controlVoltage(dacBitDepth);

//envelope params in ms
int attackTime = 5;
int decayTime = 500;
float envShape = 1.25; //slight exponential

//quantize.h here so it sees above declared variables and objects
#include "quantize.h"

void setup() {
  if ( SERIAL_DEBUG) Serial.begin(115200);

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
  pinMode(A6,INPUT);
  pinMode(A7,INPUT);

  env.curve(envShape);
  fillPitchClasses();
  if ( SERIAL_DEBUG) Serial.println(curPitchSet[0]);
  if ( SERIAL_DEBUG) Serial.println(curPitchSet[1]);

  dac.begin();
  delay(100);
}

void loop() {
  quantized.loop();
  quantizeOffset.loop();
  env.loop();
  
  static uint32_t timer = 0;
  int interval = 20;
  static byte swState[2] = {255,255};
  static uint16_t potVal[2] = {0,0};

//read arduino inputs
  if(millis()-timer > interval){
    timer = millis();

    static int rawPot[2] = {0,0};
    //READ POTENTIOMETERS
    potVal[0] = readPot(0);
    potVal[1] = readPot(1);
    //Serial.println("pots \t" + String(potVal[0]) + "\t" + String(potVal[1]) );

    //pot 1: glide/root
    switch(glideMode){
      case ROOT: newRoot = potVal[0]; break;
      case MAIN_GLIDE: glideTime[0] = pow(potVal[0]>>4,2)/10; break;
      case OFFSET_GLIDE: glideTime[1] = pow(potVal[0]>>4,2)/10; break;
    }
    //Serial.println("glide " + String(glideTime[0]) + " " + String (glideTime[1]));

    //pot 2: decay
    decayTime = pow(potVal[1]>>3,2)/8 + 10;
    
    //PROCESS SWITCHES
    for(int i=0;i<2;i++){
      if( digitalRead( sw[i][0]) == 0 ) swState[i] = 0;
      else if ( digitalRead( sw[i][1]) == 0 ) swState[i] = 2;
      else swState[i] = 1;

      if(swState[i] != curSw[i]){
        curSw[i] = swState[i];

        if(i==0 && swState[0] == 0) setCurScale(0,DIATONIC);
        else if(i==0 &&  swState[0] == 1) setCurScale(0,PENTATONIC);
        else if(i==0 &&  swState[0] == 2) setCurScale(0,TRIAD);

        if(i==1 && swState[1] == 0) setGlideMode(MAIN_GLIDE);
        else if(i==1 && swState[1] == 1) setGlideMode(ROOT);
        else if(i==1 && swState[1] == 2) setGlideMode(OFFSET_GLIDE);
      }
    }
  }//arduino inputs

  //monitor output at signal rate
  static uint32_t signalTimer = 0;
  int signalInterval = 1;
  if( millis()-signalTimer >= signalInterval){
    signalTimer=millis();

    readCV();

    //get cur scale degree
    uint16_t quantVal = constrain(quantized.get() + quantizeOffset.get() - 512,0,(1<<10)-1);
    uint16_t envVal = env.get();


    dacOutput[0] =  quantVal/2;
    dacOutput[1] =  envVal;
    
    dac.analogWrite(dacOutput[1],dacOutput[0]);

    analogWrite(led[1],constrain(dacOutput[0]%256,0,255)>>1);
    analogWrite(led[0],constrain(dacOutput[1]>>2,0,255)>>1);

    if(0){
      Serial.print("cv:\t");
      Serial.print( dacOutput[0]);
      Serial.print("\t\t");
      Serial.println( dacOutput[1] );
    }
  }//signal loop
}

//disabled setting root to 0 when switching modes
//enables root changes to persist across glide modes
void setGlideMode(GLIDE_MODE_SET mode){
  glideMode = mode;
  if ( SERIAL_DEBUG) Serial.println("glide mode " + glideModeNames[glideMode]);
  switch(glideMode){
    case ROOT:
    //glideTime[0] = 0;
    //glideTime[1] = 0;
    break;

    case MAIN_GLIDE:
    //curRoot = 0;
    glideTime[1] = 0;
    break;

    case OFFSET_GLIDE:
    //curRoot = 0;
    glideTime[0] = 0;
    break;
  }
}//setGlideMode

//simple onepole lowpass filter
//alpha is byte from 0-255
uint16_t Onepole(uint32_t cur, uint32_t prev, uint8_t alpha){
  uint32_t val = (cur*(256-alpha) + prev*alpha)>>8;
  return (uint16_t)constrain(val,0,(1<<16)-1);
}

/* readPot(potNum)
 * - scales pot input 
 * - removes exponential curve to flatten it out
 * - filters out noise
 */
uint16_t readPot(byte num){
  static int prevVal[2] = {0,0};
  uint16_t val = (num==0)
    ? map(analogRead(pot1), potRange[0],potRange[1],0,1023) 
    : map(analogRead(pot2), potRange[0],potRange[1],0,1023);
  val = constrain(val,0,1023);
  val = ((val>>2)*(val>>2)) >> 6;
  if(abs((int)val-prevVal[num]) > 10){
    prevVal[num] = val;
  }
  int returnVal = prevVal[num] > 1013 ? 1023 : prevVal[num] < 10 ? 0 : prevVal[num];
  returnVal = 1023 - returnVal;

  return returnVal;
}//readPot
