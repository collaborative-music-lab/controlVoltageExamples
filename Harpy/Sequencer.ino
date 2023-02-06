void Sequencer(){
  //increment main index - rolls over at 256 right now
  static byte cur_index = 0;
  main_index += 1;
  cur_index = globalRepeat == 0 ? main_index : freeze_index + (main_index-freeze_index) % freeze_length;
  if( SERIAL_DEBUG ) if(globalRepeat) Serial.println(String(freeze_index) + " " + String(cur_index));
  if(globalStop == 1) return 0;
  
  static byte ledIndexA = 0;
  static byte ledIndexB = 0;
  
//  //reset leds to indicate sequencer values rather than clock position
//  if( drum == 7 ){ //global view
//      USBmessage(ccMSG, nano.s1 + ledIndexA, ((seqEnable[0] >> ledIndexA) & 1)*127);
//      USBmessage(ccMSG, nano.m1 + ledIndexB, ((seqEnable[1] >> ledIndexB) & 1)*127);
//  }
//  else{
//    USBmessage(ccMSG, nano.s1 + ledIndexA, seq[drum].getAux(0, ledIndexA)*127);
//    USBmessage(ccMSG, nano.m1 + ledIndexB, seq[drum].getAux(1, ledIndexB) *127);
//  }
//
//
//  //calculate sequences for both voices
  for(byte i=0;i<2;i++){
     byte indexA = 0;
     byte indexB = 0;

     indexA = (rotate(cur_index,globalRotate,16)/getDivide(i)) % SEQ_LENGTH;
     indexB = (rotate(cur_index,globalRotate,16)/getDivide(i)) % SEQ_LENGTH;

     //check for a key being pressed and write curNote into current sequence
     if(keys_down > 0){
      if((cur_chan>>i) & 1) seq[i].set(indexA,cur_note-36);
      //Serial.println(cur_note);
     }
//    
//    //check if a step is active
//    //taking into account the subdivision
    byte valA = rotate(cur_index,globalRotate,16) %SEQ_LENGTH % getDivide(i) > 0 ? 0 : seq[i].getAux(0,indexA);
    byte valB = rotate(cur_index,globalRotate,16) %SEQ_LENGTH % getDivide(i)> 0 ? 0 : seq[i].getAux(1,indexB);
    if(((seqEnable >> i) & 1) == 0) valA = 0;
    if(((seqEnable >> i) & 1) == 0) valB = 0;

    if(valA > 0 ) {
      //MIDImessage( nifty.NOTE_ON[i], seq[i].get(ledIndexA), 0 );
      MIDImessage( nifty.NOTE_ON[i], seq[i].get(indexA), 127 );
      scheduleNoteOff( nifty.NOTE_ON[i], seq[i].get(indexA), 0, 5); 
      //prev_note[i] = seq[i].get(indexA);
    } else if(prev_note[i] > 0) {
//      MIDImessage( nifty.NOTE_ON[i],prev_note[i], 0 );
//      prev_note[i] == 0;
    }

    //update lcd for previous step
    //indicate index
    lcd_show_index(i, indexA);
    if(i==0 && ledIndexA != indexA){
      lcdShowActiveStep(i, ledIndexA);
      ledIndexA = indexA;
    }
    else if(i==1 && ledIndexB != indexB){
      lcdShowActiveStep(i, ledIndexB);
      ledIndexB = indexA;
    }

  }
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

void scheduleNoteOff(byte i, byte num, byte val, byte delay){ //channel note number, delay time
  byte newGateWrite = (gateWrite+1)%8;
  if (newGateWrite != gateRead){
    gateDelay[gateWrite].status =i;
    gateDelay[gateWrite].num = num;
    gateDelay[gateWrite].val= val;
    gateDelay[gateWrite].time = millis()+delay;
    gateWrite = newGateWrite;
  } else {
    MIDImessage( gateDelay[gateRead].status, gateDelay[gateRead].num, gateDelay[gateRead].val ); 
    gateRead = (gateRead+1)%8;

    gateDelay[gateWrite].status =i;
    gateDelay[gateWrite].num = num;
    gateDelay[gateWrite].val= val;
    gateDelay[gateWrite].time = millis()+delay;
    gateWrite = newGateWrite;
  }
}

void processNoteOff(){
  if ( gateRead != gateWrite ){
    if( millis() > gateDelay[gateRead].time) {
      MIDImessage( gateDelay[gateRead].status, gateDelay[gateRead].num, gateDelay[gateRead].val ); 
      gateRead = (gateRead+1) % 8; 
      processNoteOff();
    }
  }
}
