void Sequencer(){
  //increment main index - rolls over at 256 right now
   if(reset_flag){
    main_index = 0;
    reset_flag = 0;
  }
  else main_index += 1;

  //cur_index allows for manipulation of global parameters like freeze and beat repeat without affecting main_index count
  static byte cur_index = 0;
  cur_index = globalRepeat == 0 ? main_index : freeze_index + (main_index-freeze_index) % freeze_length;
  if( SERIAL_DEBUG ) if(globalRepeat) Serial.println(String(freeze_index) + " " + String(cur_index));
  if(globalStop == 1) return 0;
  
  static byte ledIndex[4];
  static byte prevVal[4];
  byte val[4];
  byte indexA[4];
  static byte prevNoteState[4];

  //optional mod wheel sequence
  //MIDImessage( nifty.CC, 1, seq[0].get(cur_index%8) );

  processNoteOff();

  //calculate sequences for percussion voices 0-4
  for(byte i=0;i<4;i++){
    if(params.seqEnable[i] > 0){
      if(cur_index % getDivide(0,i) == 0) {

        byte indexSelect = (rotate(cur_index,globalRotate,8)/getDivide(0,i)) % 32;
        indexA[i] = indexSelect%8;
        
        val[i] = rotate(cur_index,globalRotate,8) %SEQ_LENGTH % getDivide(0,i) > 0 ? 0 : seq[0].getAux(params.seqSelect[i][indexSelect/8],indexA[i]);
        
        if(val[i] > 0) {
          if(1){
            MIDImessage( nifty.DRUM_ON, nifty.drum[i], 100 );
            //scheduleNoteOff( nifty.DRUM_ON, nifty.drum[i], 0, 5); 
            scheduleNoteOff( nifty.DRUM_ON, nifty.drum[i], 0, (beat_length/num_subdiv * params.seqDivide[i])/2); 
            
          } else {
              (prevNoteState[i] == 1);
              MIDImessage( nifty.DRUM_ON, nifty.drum[i], 0 );
              delayMicroseconds(250);
              MIDImessage( nifty.DRUM_ON, nifty.drum[i], 100 );
              prevNoteState[i] = 1;
              // scheduleNoteOff( nifty.DRUM_ON, nifty.drum[i], 0, beat_length/params.seqDivide[i]/2); 
              //scheduleNoteOff( nifty.DRUM_ON, nifty.drum[i], 0, 5); 
              // Serial.print(i); 
              // Serial.print(" "); 
              // Serial.println(millis()-test_timer);
          } 
        }//for note ons
        else if (prevNoteState[i] == 1) {
              MIDImessage( nifty.DRUM_ON, nifty.drum[i], 0 );
              prevNoteState[i] = 0;
          } //for note offs
      }//divide
    }//enable
  } //midi out
  for(byte i=0;i<4;i++){
    if(params.seqEnable[i] > 0){
      if(cur_index % getDivide(0,i) == 0) {
        updateGridLed(ledIndex[i] + i*8 + 32, seq[0].getAux(i,ledIndex[i]), patternColor[i]);
        ledIndex[i] = prevVal[i] + (i+4)*8;
        prevVal[i] = val[i];
        updateGridLed(indexA[i] + i*8 + 32, 1, cursorColor);
        ledIndex[i] = indexA[i];
      } //divide
    }//enable
  } //lighting out
  
}

/**************process note offs**********/
struct defMidiPacketDelay{
  byte status;
  byte num;
  byte val;
  uint32_t time;
};
defMidiPacketDelay gateDelay[8];

byte gateWrite = 0;
byte gateRead = 0;

void scheduleNoteOff(byte i, byte num, byte val, int delay){ //channel note number, delay time
  byte newGateWrite = (gateWrite+1)%8;
  if (newGateWrite != gateRead){
    gateDelay[gateWrite].status =i;
    gateDelay[gateWrite].num = num;
    gateDelay[gateWrite].val= val;
    gateDelay[gateWrite].time = delay>5 ? millis()+delay : millis() + 5;
  } else {
    Serial.println("double");
    MIDImessage( gateDelay[gateRead].status, gateDelay[gateRead].num, gateDelay[gateRead].val ); 
    gateRead = (gateRead+1)%8;

    gateDelay[gateWrite].status =i;
    gateDelay[gateWrite].num = num;
    gateDelay[gateWrite].val= val;
    gateDelay[gateWrite].time = delay>5 ? millis()+delay : millis() + 5;
  }
  // Serial.print(gateWrite);
  // Serial.print(" on ");
  // Serial.print(num);
  // Serial.print("  ");
  // Serial.println(delay);
  // Serial.print(gateDelay[gateWrite].time);
  // Serial.print("  ");
  // Serial.println(millis());

  gateWrite = newGateWrite;
}

void processNoteOff(){
  if ( gateRead != gateWrite ){
    if( millis() > gateDelay[gateRead].time) {
      MIDImessage( gateDelay[gateRead].status, gateDelay[gateRead].num, gateDelay[gateRead].val ); 
     
      // Serial.print(gateRead);
      // Serial.print(" off ");
      // Serial.print(gateDelay[gateRead].num);
      // Serial.print("  ");
      // Serial.println(gateDelay[gateRead].time);

      gateRead = (gateRead+1) % 8;
      processNoteOff();
    }
  }
}