enum stateName { bOFF, bLOW, bFALLING, bHIGH, bRISING };

stateName clockState = bOFF;
stateName resetState = bOFF;

void clockSetup(){
  pinMode( clockPin, INPUT_PULLUP);
  pinMode( resetPin, INPUT_PULLUP);
  
}

void clockLoop(){
  /*****external clock*****/
  static byte clock_mode = 1; //0 for external, 1 for internal
  byte val = !digitalRead( clockPin );
  if(val == 1) clock_mode = 0;

  switch( clockState ){
    case bOFF: clockState = val == 1 ? bRISING : bFALLING; break;
    case bLOW: clockState = val == 1 ? bRISING : bLOW; break;
    case bFALLING: clockState = val == 1 ? bRISING : bLOW; break;
    case bHIGH: clockState = val == 1 ? bHIGH : bFALLING; break;
    case bRISING: clockState = val == 1 ? bHIGH : bFALLING; break;
  }

  val = !digitalRead( resetPin );
  switch( resetState ){
    case bOFF: resetState = val == 1 ? bRISING : bFALLING; break;
    case bLOW: resetState = val == 1 ? bRISING : bLOW; break;
    case bFALLING: resetState = val == 1 ? bRISING : bLOW; break;
    case bHIGH: resetState = val == 1 ? bHIGH : bFALLING; break;
    case bRISING: resetState = val == 1 ? bHIGH : bFALLING; break;
  }

  /*****internal clock*****/
  static uint32_t control_timer = 0;
  int interval = 450;
  
  if((millis()-control_timer > interval) && clock_mode > 0){
    //subdiv_interval = (millis()-control_timer)/(num_subdiv);
    // control_timer=millis();
    
    clockState = bRISING;
  } else if (clock_mode > 0) clockState = bLOW;

  // Serial.println(millis()); 
  // delay(25);

  /*****all clock*****/
  static uint16_t subdiv_interval;
  static uint32_t prevClock = millis();

  if(clockState == bRISING) {
    subdiv_interval = (millis()-control_timer)/num_subdiv;
    control_timer=millis();
    // Serial.println(millis()-test_timer); 
    // test_timer = millis();
    
    subdiv = 1;
    Sequencer();
    //  static byte pitch = 0;
    // MIDImessage( nifty.DRUM_ON, 36+pitch*12, 0 ); 
    // pitch = pitch>3?0:pitch+1;
    // MIDImessage( nifty.DRUM_ON, 36+pitch*12, 100 );
     
    Serial.print("*");
    //Serial.println(subdiv_interval);
  } else if(subdiv< num_subdiv){
    if(millis() - control_timer > subdiv_interval*subdiv){
      subdiv = subdiv+1;
      Sequencer();
      Serial.print(".");
    }
  }

  if(resetState == bRISING) {
    if( SERIAL_DEBUG ) Serial.println("reset rising");
  }
}
