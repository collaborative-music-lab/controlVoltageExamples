//helper functions for Copernicus

byte cv1 = A0;
byte cv2 = A1;

byte pot[] = {A2,A3};
int potRange[2] = {0,840};

byte sw[2][2] = {
  {4,7},
 {8,10}
};

byte seqTrig = 3;
byte seqReset = 5;
int seqVals[4][16];
int seqNum = 0;

//mux
byte muxAdd[] = {12,1,11,13};
byte muxIn = A7;
byte muxOut = 11;

byte led[] = {6,9};

//pin change interrupts
//https://thewanderingengineer.com/2014/08/11/arduino-pin-change-interrupts/
//pins: clock 6 PD6, reset 9 PB1
volatile byte clockTrigger = 0;
volatile byte resetTrigger = 0;

void setupInterrupts(){
  cli();
  PCICR |= 0b00000101;    // turn on port b & d
  PCMSK0 |= 0b00000010;    // use masking to turn on pin PB1
  PCMSK2 |= 0b01000000;    // use masking to turn on pins PD6
  sei();
}

ISR(PCINT0_vect){}    // Port B, PCINT0 - PCINT7

ISR(PCINT2_vect){}    // Port D, PCINT16 - PCINT23

void setMuxLED(byte curStep){
  byte muxMap[] = {15,14,11,10,13,12,8,9,6,7,5,3,2,4,1,0};
  
  for(int j=0;j<4;j++){
    //Serial.print(String((muxChannel>>j)&1) + " ");
    digitalWrite(muxAdd[j], (muxMap[curStep]>>j)&1);
  }

  digitalWrite( muxOut, 1);
  
  muxReadEnable = 0;
}

//read mux inputs at every clock input
void readMuxInputs(){
  static byte num = 0;
  byte muxMap[] = {15,14,11,10,13,12,8,9,6,7,5,3,2,4,1,0};

  if(num == 0) digitalWrite( muxOut, 0);

  muxVals[seqNum] = analogRead( muxIn );

  for(int j=0;j<4;j++){
    //Serial.print(String((muxChannel>>j)&1) + " ");
    digitalWrite(muxAdd[j], (muxMap[num]>>j)&1);
  }

  seqVals[num] = muxVals[num]; //to do: enable storing and recalling seqs

  num++;

  if(num>=16){
    num = 0;
    muxReadEnable = 2;
  }
}
