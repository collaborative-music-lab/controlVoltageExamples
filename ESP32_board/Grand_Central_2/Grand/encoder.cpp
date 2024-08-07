// based on https://github.com/igorantolic/ai-esp32-rotary-encoder
// which was itself based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code
// 
// 

#include "encoder.h"

void IRAM_ATTR encoder::readEncoder_ISR()
{	
	portENTER_CRITICAL_ISR(&(this->mux));
  this->ISRflag = 1;
	this->old_AB <<= 2;                   //remember previous state
  this->old_AB;
	int8_t ENC_PORT = (
	  (digitalRead(this->encoderBPin)) ? (1 << 1) : 0) + 
	  ((digitalRead(this->encoderAPin)) ? (1 << 0) : 0
	  );
	this->old_AB |= ( ENC_PORT & 0x03 );  //add current state
	this->encoder0Pos += ( this->enc_states[( this->old_AB & 0x0f )]);		
	portEXIT_CRITICAL_ISR(&(this->mux));
  Serial.print("x");
  Serial.print(digitalRead(32));
  Serial.println(digitalRead(33));
}//readEncoder_ISR

encoder::encoder(uint8_t _APin, uint8_t _BPin, uint8_t _ButtonPin, uint8_t _divider)
{
	this->old_AB = 0;
	
	this->encoderAPin = _APin;
	this->encoderBPin = _BPin;
	this->encoderButtonPin = _ButtonPin;
  this->divider = _divider;
	pinMode(this->encoderAPin, INPUT);
	pinMode(this->encoderBPin, INPUT);

}//encoder

//encoder::encoder(uint8_t _APin, uint8_t _BPin, uint8_t _ButtonPin, uint8_t _divider)
//{
//  this->old_AB = 0;
//  
//  this->encoderAPin = _APin;
//  this->encoderBPin = _BPin;
//  this->encoderButtonPin = _ButtonPin;
//  this->divider =_divider;
//  pinMode(this->encoderAPin, INPUT);
//  pinMode(this->encoderBPin, INPUT);
//
//}//encoder


int16_t encoder::readEncoder()
{
  int out;
  portENTER_CRITICAL(&mux);
  // if (this->ISRflag){
  //   this->ISRflag=0;
  //   Serial.println("flag");
  // }
  out =  this->encoder0Pos;
  this->encoder0Pos = 0;
  ISRflag = 0;
  portEXIT_CRITICAL(&mux);
  //Serial.println(out);
	return (out);
}

int16_t encoder::delta() {   return (readEncoder()); }

int32_t encoder::count()
{
  int encoderDelta = readEncoder();
  if (encoderDelta == 0) return this->_count ;
  int prevRaw = this->_raw;
  this->_raw+=encoderDelta;
  if(  abs(this->_raw) > this->divider-1 || (abs(this->_raw)-abs(prevRaw)<0)){
    this->_raw=0;
    encoderDelta > 0 ? this->_count++ : this->_count--;
  }
  return (this->_count);
}

int16_t encoder::encoderChanged() 
{
	int16_t _encoder0Pos = readEncoder();
	
	int16_t encoder0Diff = _encoder0Pos - this->lastReadEncoder0Pos;

	this->lastReadEncoder0Pos = _encoder0Pos;
	return encoder0Diff;
}//readEncoder

void encoder::begin(void (*ISR_callback)(void))
{
	this->previous_butt_state = 0;
	pinMode(this->encoderButtonPin, INPUT);
  pinMode(this->encoderAPin, INPUT);
  pinMode(this->encoderBPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(encoderAPin), ISR_callback, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderBPin), ISR_callback, CHANGE);

  Serial.println(encoderAPin);
  Serial.println(encoderBPin);
}//begin

byte encoder::available(){
  return ISRflag;
}

ButtonState encoder::button()
{
	uint8_t butt_state = 255;
  byte curReading = digitalRead(this->encoderButtonPin);
  this->button_history = (this->button_history<<1) + curReading;
  if(this->button_history == this->onPattern ) butt_state = 1;
  else if (this->button_history == this->offPattern ) butt_state = 0;

  if( butt_state  <=1  ){
    previous_butt_state = butt_state;
     switch(butt_state){
      case 1: return  BUT_PUSHED;
      case 0: return BUT_RELEASED;
     }
  }else {
    switch(previous_butt_state){
      case 0: return  BUT_DOWN;
      case 1: return BUT_UP;
     }
  }
}//button
