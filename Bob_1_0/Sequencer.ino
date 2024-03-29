void Sequencer(){
  //increment main index - rolls over at 256 right now
  main_index += 1;
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

  //calculate sequences for percussion voices 0-4
  for(byte i=0;i<4;i++){
    if(params.seqEnable[i] > 0){
      if(cur_index % getDivide(0,i) == 0) {

        byte indexSelect = (rotate(cur_index,globalRotate,8)/getDivide(0,i)) % 32;
        indexA[i] = indexSelect%8;
        
        if(i==0){
          val[i] = rotate(cur_index,globalRotate,8) %SEQ_LENGTH % getDivide(0,i) > 0 ? 0 : _melody[_active_melody][indexA[i]];
          
          if(val[i] > 0) {
            byte cur_note = _scales[_cur_scale][val[i]];
            Serial.println(cur_note);
            scheduleMidiEvent( nifty.NOTE1_ON, cur_note, 100, params.seqTiming[i] );
            
            int _gate = (beat_length/num_subdiv * getDivide(0, i));
            _gate = 0;

            if(_gate<25) scheduleMidiEvent( nifty.NOTE1_ON, cur_note, 0, (int)params.seqTiming[i] + 25); 
            else scheduleMidiEvent( nifty.NOTE1_ON, cur_note, 0, (int)params.seqTiming[i] + _gate - 25); 
            //Serial.println("sch_midi " + String(i) +" "+ String(_gate) + " "+ String(params.seqTiming[i]));
          }
        }

        else if(i>1){ //gates
          val[i] = rotate(cur_index,globalRotate,8) %SEQ_LENGTH % getDivide(0,i) > 0 ? 0 : seq[0].getAux(params.seqSelect[i][indexSelect/8],indexA[i]);
          
          if(val[i] > 0) {
            scheduleMidiEvent( nifty.DRUM_ON, nifty.drum[i], 100, params.seqTiming[i] );
            
            int _gate = (beat_length/num_subdiv * getDivide(0, i));

            if(_gate<25) scheduleMidiEvent( nifty.DRUM_ON, nifty.drum[i], 0, (int)params.seqTiming[i] + 25); 
            else scheduleMidiEvent( nifty.DRUM_ON, nifty.drum[i], 0, (int)params.seqTiming[i] + _gate - 25); 
            //Serial.println("sch_midi " + String(i) +" "+ String(_gate) + " "+ String(params.seqTiming[i]));
          }
        }
      }//divide
    }//enable
  } //midi out
  for(byte i=0;i<4;i++){
    if(params.seqEnable[i] > 0 && launchpad_page == 0){
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

