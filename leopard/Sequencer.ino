void Sequencer(){
  //increment main index - rolls over at 256 right now
  static byte cur_index = 0;
  main_index += 1;
  cur_index = globalRepeat == 0 ? main_index : freeze_index + (main_index-freeze_index) % freeze_length;
  if( SERIAL_DEBUG ) if(globalRepeat) Serial.println(String(freeze_index) + " " + String(cur_index));
  if(globalStop == 1) return 0;
  
  static byte ledIndex[4];
  static byte prevVal[4];

  
  //reset leds to indicate sequencer values rather than clock position
  if( drum == 7 ){ //global view
      // USBmessage(ccMSG, lp.s1 + ledIndexA, ((seqEnable[0] >> ledIndexA) & 1)*127);
      // USBmessage(ccMSG, lp.m1 + ledIndexB, ((seqEnable[1] >> ledIndexB) & 1)*127);
  }
  else{
    // USBmessage(ccMSG, lp.s1 + ledIndexA, seq[drum].getAux(0, ledIndexA)*127);
    // USBmessage(ccMSG, lp.m1 + ledIndexB, seq[drum].getAux(1, ledIndexB) *127);
  }


  //calculate sequences for percussion voices 0-4
  for(byte i=0;i<4;i++){
    if(cur_index % getDivide(0,i) == 0) updateGridLed(ledIndex[i], prevVal[i], patternColor[i]);

     byte indexA = 0;
     byte indexB = 0; 

     indexA = (rotate(cur_index,globalRotate,8)/getDivide(0,i)) % SEQ_LENGTH;
    
    //for drums
    //byte vel = valA > 0 ? seq[i].get(0) : 0;
    //if( (valB > 0) && (seq[i].get(1) > vel)) vel = seq[i].get(1);
    if(cur_index % getDivide(0,i) == 0){
      //check if a step is active
    //taking into account the subdivision
    byte valA = rotate(cur_index,globalRotate,8) %SEQ_LENGTH % getDivide(0,i) > 0 ? 0 : seq[0].getAux(i,indexA);
    byte valB = rotate(cur_index,globalRotate,8) %SEQ_LENGTH % getDivide(1,i)> 0 ? 0 : seq[i].getAux(1,indexB);
    // if(((seqEnable[0] >> i) & 1) == 0) valA = 0;
    // if(((seqEnable[1] >> i) & 1) == 0) valB = 0;

    if(valA > 0) {
      MIDImessage( nifty.DRUM_ON, nifty.drum[i], 0 ); 
      MIDImessage( nifty.DRUM_ON, nifty.drum[i], 100 );
      //delay(5);
    } else MIDImessage( nifty.DRUM_ON, nifty.drum[i], 0 );

    Serial.print("seq ");
      Serial.print(indexA + (i+4)*16);
      Serial.print(" ");
      Serial.println(valA);

      ledIndex[i] = indexA + (i+4)*8;
      prevVal[i] = valA;
      updateGridLed(ledIndex[i], 1, cursorColor);
    }
    
  }
  
}