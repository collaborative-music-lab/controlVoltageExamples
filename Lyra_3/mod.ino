

void modOutput(byte step){
   MIDImessage( nifty.CC, 1, params.mod_vals[step] );
  // static uint16_t val = 0;
  // static uint16_t prev_val = 0;
  // static uint32_t timer = 0;
  // static byte direction = 0;
  // byte rate = 2;

  // if(millis()-timer > 1000/127/rate){
  //   timer = millis();
    
  //   val = direction == 0 ? val+1 : val-1;
  //   direction = val > 125 ? 1 : val < 2 ? 0 : direction;

  //   if( val != prev_val) MIDImessage( nifty.CC, 1, val );
  //   prev_val = val;
  // }
}