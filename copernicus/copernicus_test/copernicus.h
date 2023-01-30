//helper functions for Copernicus

//7   9   4   11, 2   13  15  6,  10  8   1   3,  12  14  5   16
//11  5   12   3,  0   8   1   10,  2   9   4   0,  6   0   7   0

//16 5 12 3 14 1 10 8 15 6 11 4 13 2 9 7

byte cv1 = A0;
byte cv2 = A1;

byte pot[] = {A2,A3};
int potRange[2] = {0,840};

byte sw[2][2] = {
  {4,7},
 {8,10}
};

byte swState[2];

byte seqTrig = A6;
byte seqReset = 12;
byte trigX = 6;
byte trigY = 9;

int seqVals[4][16];
int seqNum = 0;

//mux
byte muxAdd[] = {0,1,13,11};
byte muxIn = A7;
byte muxReadEnable = 0;
int muxVals[16];

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
  byte muxMap[] = {5,13,3,11,1,9,15,7, 14,6,10,2,12,4,8,0};
  
  for(int j=0;j<4;j++){
    //Serial.print(String((muxChannel>>j)&1) + " ");
    digitalWrite(muxAdd[j], (muxMap[curStep]>>j)&1);
  }

  
  muxReadEnable = 0;
}

//read mux inputs at every clock input
void readMuxInputs(){
  static byte num = 0;
  byte muxMap[] = {15,14,11,10,13,12,8,9,6,7,5,3,2,4,1,0};


  muxVals[seqNum] = analogRead( muxIn );

  for(int j=0;j<4;j++){
    //Serial.print(String((muxChannel>>j)&1) + " ");
    digitalWrite(muxAdd[j], (muxMap[num]>>j)&1);
  }

  seqVals[seqNum][num] = muxVals[num]; //to do: enable storing and recalling seqs

  num++;

  if(num>=16){
    num = 0;
    muxReadEnable = 2;
  }
}

//read a single mux input
void readMuxInput(byte num){
  byte muxMap[] = {2,11,15,13,  3,10,14,12,  1,7,8,9,  0,4,5,6};
  //byte muxMap[] = {0,1,2,3,  4,5,6,7,  8,9,10,11,  12,13,14,15};

  for(int j=0;j<4;j++){
    //Serial.print(String((muxChannel>>j)&1) + " ");
    digitalWrite(muxAdd[j], ((muxMap[num])>>j)&1);
    //Serial.println(String(muxAdd[j]) + " " + String((muxMap[num]>>j)&1));
  }

  muxVals[num] = analogRead( muxIn );

  seqVals[seqNum][num] = muxVals[num]; //to do: enable storing and recalling seqs

}

void updateSwState(){
  for(int i=0;i<2;i++){
      if( digitalRead( sw[i][0]) == 0 ) swState[i] = 0;
      else if ( digitalRead( sw[i][1]) == 0 ) swState[i] = 2;
      else swState[i] = 1;
    }
}
