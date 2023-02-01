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
      Serial.println(cur_note);
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
      delay(5);
      MIDImessage( nifty.NOTE_ON[i], seq[i].get(indexA), 0 );
      prev_note[i] = seq[i].get(indexA);
    } else if(prev_note[i] > 0) {
//      MIDImessage( nifty.NOTE_ON[i],prev_note[i], 0 );
//      prev_note[i] == 0;
    }

    //update lcd for previous step
    //indicate index
    lcd_show_index(i, indexA);
    if(i==0){
      lcdShowActiveStep(i, ledIndexA);
      ledIndexA = indexA;
    }
    else if(i==1){
      lcdShowActiveStep(i, ledIndexB);
      ledIndexB = indexA;
    }
//
//    //get the max velocity between seq A & B
//    if(i<6){
//      //for drums
//      byte vel = valA > 0 ? seq[i].get(0) : 0;
//      if( (valB > 0) && (seq[i].get(1) > vel)) vel = seq[i].get(1);
//      if( vel>0 ) MIDImessage( t8.noteON, t8.seq[i], vel>1?vel:0 );
//    }
//    else if ( i==6 ){
//      //for synth
//      byte vel = valA > 0 ? bassGain[0] : 0;
//      if( (valB > 0) && (bassGain[1] > vel)) vel = bassGain[1];
//      
//      if(vel >0) {
//        //calculate pitch
//        const byte scale[] = {0,2,3,5,7,8,10};
//        MIDImessage( bassON, curNote, 0 );
//  
//        byte pitch = 0;
//        if(seq[i].getAux(0,indexA) == 1) pitch += (seq[6].get(indexA)/18 + 1);
//        if(seq[i].getAux(1,indexB) == 1) pitch += (seq[6].get(indexB)/18 + 1);
//        curNote = scale[pitch % sizeof(scale)] + (pitch / sizeof(scale))*12 + pitchOffset;
//  
//        MIDImessage( bassON, curNote, vel>1?vel:0 );    
//        lcd_printNOTE(curNote);
//      }
//    }
//
//    //light up sequence position
//    if(i==drum){
//      USBmessage(ccMSG, nano.s1 + indexA, 127);
//      USBmessage(ccMSG, nano.m1 + indexB, 127);
//      ledIndexA = indexA;
//      ledIndexB = indexB;
//    }
  }
}

/**************************************************************************************/
//void Sequencer(){
//  static uint32_t controlTimer = 0;
//  int interval = 150;
//
//  static int val= 0;
//  static int index=0;
//  static byte scalePitches[] = {0,0,1,1,2,3,3,4,4,5,5,6,6};
//  static byte scale[]={0,2,4,5,7,9,11};
//  static int octave = 0;
//  static int curVal = 36;
//  static uint32_t prev_pulse = 0;
//  
//  if(prev_pulse != pulses){
//    controlTimer=millis();
//    static uint16_t prevTrigger = 0;
//     index = pulses;
//
//    byte trigger = pulses - prevTrigger >= lengthSeq[index];
//
//    if( trigger == 0 && gateSeq[index] >= 1) MIDImessage(noteOFF, curVal, 0);
//    if( trigger == 0 && gateSeq[index] == 1)  MIDImessage(noteON, curVal, 127 - chokeSeq[index]*minChoke);
//    if( trigger > 0 ) {
//      prevTrigger = pulses;
//      MIDImessage(noteOFF, curVal, 0);
//    
//      byte seqDirection = seqStart < seqEnd;
//  
//      if(seqDirection){
//        index = index >= seqEnd ? seqStart : index<seqStart ? seqStart : index+1;
//      } else index = index <= seqEnd ? seqStart : index > seqStart ? seqStart : index-1;
//  
//      if(keyDown > 0) {
//        if( cur_note % 12 != 6) {
//          sequence[index] = cur_note-24;
//          chokeSeq[index] = choke;
//          lengthSeq[index] = curLength;
//          gateSeq[index] = gateMode;
//        }
//      }
//      curVal = sequence[index];
//      if(curVal % 12 != 10){
//        curVal = scalePitches[curVal % 12] + (curVal/12)*sizeof(scale) + transpose;
//        byte curOctave = curVal/sizeof(scale);
//        curVal = scale[curVal % sizeof(scale)] + (octave+curOctave)*12;
//        MIDImessage(noteON, curVal, 127 - chokeSeq[index]*minChoke);
//        //Serial.println( "gate " + String(gateSeq[index]));
//        lcd.clear();
//        lcd.setCursor(0,0);
//        lcd.print("A:" + String(curVal));
//        lcd.setCursor(0,1);
//        lcd.print("B:" + String(60));
//
//        lcd.setCursor(15,0);
//        lcd.print( String(pulses) );
//      }
//      
//    }//control loop
//    
//    prev_pulse = pulses ;
//    //pulses++;
//  }//timer
//}
