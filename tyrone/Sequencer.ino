void Sequencer(){
  //increment main index - rolls over at 256 right now
  static byte cur_index = 0;
  main_index += 1;
  cur_index = globalRepeat == 0 ? main_index : freeze_index + (main_index-freeze_index) % freeze_length;
  if( SERIAL_DEBUG ) if(globalRepeat) Serial.println(String(freeze_index) + " " + String(cur_index));
  if(globalStop == 1) return 0;
  
  static byte ledIndexA = 0;
  static byte ledIndexB = 0;
  
  //reset leds to indicate sequencer values rather than clock position
  if( drum == 7 ){ //global view
      USBmessage(ccMSG, nano.s1 + ledIndexA, ((seqEnable[0] >> ledIndexA) & 1)*127);
      USBmessage(ccMSG, nano.m1 + ledIndexB, ((seqEnable[1] >> ledIndexB) & 1)*127);
  }
  else{
    USBmessage(ccMSG, nano.s1 + ledIndexA, seq[drum].getAux(0, ledIndexA)*127);
    USBmessage(ccMSG, nano.m1 + ledIndexB, seq[drum].getAux(1, ledIndexB) *127);
  }


  //calculate sequences for percussion voices 0-5
  for(byte i=0;i<NUM_SEQS;i++){
     byte indexA = 0;
     byte indexB = 0;

     indexA = (rotate(cur_index,globalRotate,8)/getDivide(0,i)) % SEQ_LENGTH;
     indexB = (rotate(cur_index,globalRotate,8)/getDivide(1,i)) % SEQ_LENGTH;
    
    //check if a step is active
    //taking into account the subdivision
    byte valA = rotate(cur_index,globalRotate,8) %SEQ_LENGTH % getDivide(0,i) > 0 ? 0 : seq[i].getAux(0,indexA);
    byte valB = rotate(cur_index,globalRotate,8) %SEQ_LENGTH % getDivide(1,i)> 0 ? 0 : seq[i].getAux(1,indexB);
    if(((seqEnable[0] >> i) & 1) == 0) valA = 0;
    if(((seqEnable[1] >> i) & 1) == 0) valB = 0;

    //get the max velocity between seq A & B
    if(i<6){
      //for drums
      byte vel = valA > 0 ? seq[i].get(0) : 0;
      if( (valB > 0) && (seq[i].get(1) > vel)) vel = seq[i].get(1);
      if( vel>0 ) MIDImessage( t8.noteON, t8.seq[i], vel>1?vel:0 );
    }
    else if ( i==6 ){
      //for synth
      byte vel = valA > 0 ? bassGain[0] : 0;
      if( (valB > 0) && (bassGain[1] > vel)) vel = bassGain[1];
      
      if(vel >0) {
        //calculate pitch
        const byte scale[] = {0,2,3,5,7,8,10};
        MIDImessage( bassON, curNote, 0 );
  
        byte pitch = 0;
        if(seq[i].getAux(0,indexA) == 1) pitch += (seq[6].get(indexA)/18 + 1);
        if(seq[i].getAux(1,indexB) == 1) pitch += (seq[6].get(indexB)/18 + 1);
        curNote = scale[pitch % sizeof(scale)] + (pitch / sizeof(scale))*12 + pitchOffset;
  
        MIDImessage( bassON, curNote, vel>1?vel:0 );    
        lcd_printNOTE(curNote);
      }
    }

    //light up sequence position
    if(i==drum){
      USBmessage(ccMSG, nano.s1 + indexA, 127);
      USBmessage(ccMSG, nano.m1 + indexB, 127);
      ledIndexA = indexA;
      ledIndexB = indexB;
    }
  }
  
  if( drum == GLOBAL){
      ledIndexA = (main_index/4) % SEQ_LENGTH;
      ledIndexB = (main_index/4) % SEQ_LENGTH;
      USBmessage(ccMSG, nano.s1 + ledIndexA, 127);
      USBmessage(ccMSG, nano.m1 + ledIndexB, 127); 
  }
}

void setDrumSeq(byte _drum, byte num, byte step, byte val){
  seq[_drum].setAux(num,step,val>0);
}
