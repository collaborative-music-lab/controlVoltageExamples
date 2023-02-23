void Sequencer(){
  //increment main index - rolls over at 256 right now
  static byte cur_index = 0;
  main_index += 1;
  cur_index = globalRepeat == 0 ? main_index : freeze_index + (main_index-freeze_index) % freeze_length;
  if( SERIAL_DEBUG ) if(globalRepeat) Serial.println(String(freeze_index) + " " + String(cur_index));
  if(globalStop == 1) return 0;
  
  //updates indexes for both voices
  static byte subIndex[2];
  byte seqNewStep[2];
  for(byte i=0;i<2;i++){
    //byte valA = rotate(indexA,globalRotate)  % getDivide(i) > 0 ? 0 : seq[0].getAux(0,indexA);
    if( subIndex[i] % getDivide(i) == 0 ){
      if( params.inc[i] > 0){
        params.index[i] = params.index[i]>=params.seqLength[i]-1 ? 
        params.seqStart[i] : 
        params.index[i] == params.seqEnd[i] ? 
        //params.index[i] = params.index[i]>=params.seqEnd[i] ? 
        //params.seqStart[i] : params.index[i] < params.seqStart[i] ? 
        params.seqStart[i] :
        params.index[i] + params.inc[i];
      } else{
        params.index[i] = params.index[i]>=params.seqLength[i]-1 ? 
        params.seqEnd[i] :
        params.index[i] == params.seqStart[i] ? 
        //params.seqEnd[i] : params.index[i] <= params.seqStart[i] ? 
        params.seqEnd[i]:
        params.index[i] - params.inc[i];
      }
      seqNewStep[i] = 1;
    } else seqNewStep[i] = 0;
    subIndex[i] += 1;
  }//indexes 

  //check for reset conditions
  for(byte i=0;i<2;i++){
    if( seqNewStep[i] == 1){
      if(params.resetCondition[!i]==1){
        if(params.inc[i] > 0 && params.index[i] == params.seqStart[i]) {
          params.index[!i] = params.inc[i] > 0 ? params.seqStart[!i] : params.seqEnd[!i];
          seqNewStep[!i] = 1;
          subIndex[!i] = 1;
        }
        else if(params.inc[i] < 0 && params.index[i] == params.seqEnd[i] ) {
          params.index[!i] = params.inc[i] > 0 ? params.seqStart[!i] : params.seqEnd[!i];
          seqNewStep[!i] = 1;
          subIndex[!i] = 1;
        }        
        //Serial.println("reset " + String(!i));
      }
    }//reset
  }

  //  //calculate sequences for both voices
  for(byte i=0;i<2;i++){
    if( seqNewStep[i] == 1){
      byte indexA = params.index[i];
      static byte ledIndex[2] = {0,0};
      //Serial.println(String(i) + String(indexA));

      //check if rec button is held down
      //if so, record into current sequence
      if(key.rec.val > 0 && key.play.val == 0){
        if((cur_chan>>i) & 1) {
          seq[0].set(indexA,cur_note-36);
          if( keys_down > 0) {
            // seq[0].set(indexA,cur_note-36);
            seq[0].setAux(0,indexA,1);
            Serial.println("rec " + String(i) + String(indexA) + String(seq[i]._aux[0]));
          } else {
            seq[0].setAux(0,indexA,0);
          }
        }
      }
  //    
  //    //check if a step is active
  //    //taking into account the subdivision
      //byte valA = rotate(indexA,globalRotate)  % getDivide(i) > 0 ? 0 : seq[0].getAux(0,indexA);
      byte valA = seq[0].getAux(0,indexA);
      if(((seqEnable >> i) & 1) == 1) valA = 0;

      //the niftycase is pretty particular about getting note off msgs
      //the below implementation is the only one that consistently works
      //without hanging the niftycase
      static byte prev_note[2];
      if(valA > 0 ) {
        //MIDImessage( nifty.NOTE_ON[i], seq[i].get(ledIndex[i]), 0 );
        if( prev_note[i] < 255) MIDImessage( nifty.NOTE_ON[i], prev_note[i], 0 );
        scheduleNoteOff( nifty.NOTE_ON[i], seq[0].get(indexA), 127, 2); 
        prev_note[i] = seq[0].get(indexA);
      } else  {
          if( prev_note[i] < 255) MIDImessage( nifty.NOTE_ON[i],prev_note[i], 0 );
          prev_note[i] = 255;
          //MIDImessage( nifty.NOTE_ON[i], seq[0].get(indexA), 0 );
      }

      //LED index display
      byte padNum = indexA / (params.seqLength[i]/16);

      if(ledIndex[i] != padNum){
          LK_LED_PAD(ledIndex[i] ,  OFF,  ON);
        ledIndex[i] = padNum;
        }
      if( i==0 ) LK_LED_PAD(padNum % 16,  ORANGE,  ON);
      else if( i==1 ) LK_LED_PAD(padNum % 16,  BLUE,  ON);
    }//if new step for this seq
  }// for all seqs
}//sequencer

/**************process note offs**********/
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
    //Serial.println("double");
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
