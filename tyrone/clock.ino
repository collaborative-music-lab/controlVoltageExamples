enum stateName { bOFF, bLOW, bFALLING, bHIGH, bRISING };

stateName clockState = bOFF;
stateName resetState = bOFF;

void clockSetup(){
  pinMode( clockPin, INPUT_PULLUP);
  pinMode( resetPin, INPUT_PULLUP);
  
}

void clockLoop(){
  /*****external clock*****/
  byte val = !digitalRead( clockPin );
//  Serial.println(val); 
//  delay(25);
  
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
  int interval = 500;
  
  if(millis()-control_timer > interval){
    subdiv_interval = (millis()-control_timer)/(num_subdiv);
    control_timer=millis();
    
    clockState = bRISING;
  } else clockState = bLOW;

  /*****all clock*****/
  if(clockState == bRISING) {
//    subdiv_interval = (millis()-control_timer)/num_subdiv;
//    control_timer=millis();
    
    subdiv = 1;
    Sequencer();
    if( SERIAL_DEBUG ) Serial.print("*");
    
  } else if(subdiv< num_subdiv){
    if(millis()- subdiv_interval*subdiv > control_timer){
      subdiv = subdiv < 254 ? subdiv+1 : 0;
      Sequencer();
      //Serial.print(" subdiv" + String( subdiv ));
    }
  }

  if(resetState == bRISING) {
    if( SERIAL_DEBUG ) Serial.println("reset rising");
  }
}
