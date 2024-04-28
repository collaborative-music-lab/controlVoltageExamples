enum EventTypes { MIDI_CLOCK_OUT, AN_CLOCK_HIGH, AN_CLOCK_LOW, RESET_HIGH, RESET_LOW, DISPLAY_UPDATE};
/**************process note offs**********/
struct defScheduledEvent{
  byte status;
  
  byte num;
  byte val;
  uint32_t time;
};
defScheduledEvent scheduledEvent[128];

byte gateWrite = 0;
byte gateRead = 0;

void scheduleEvent(byte i, byte num, byte val, int delay){ //channel note number, delay time
  byte newGateWrite = (gateWrite+1)%8;
  if (newGateWrite != gateRead){
    scheduledEvent[gateWrite].status =i;
    scheduledEvent[gateWrite].num = num;
    scheduledEvent[gateWrite].val= val;
    scheduledEvent[gateWrite].time = delay>5 ? millis()+delay : millis() + 5;
  } else {
    Serial.println("double");
    //MIDImessage( scheduledEvent[gateRead].status, scheduledEvent[gateRead].num, scheduledEvent[gateRead].val ); 
    gateRead = (gateRead+1)%8;

    scheduledEvent[gateWrite].status =i;
    scheduledEvent[gateWrite].num = num;
    scheduledEvent[gateWrite].val= val;
    scheduledEvent[gateWrite].time = delay>5 ? millis()+delay : millis() + 5;
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

void processEvent(){
  if ( gateRead != gateWrite ){
    if( millis() > scheduledEvent[gateRead].time) {
      //MIDImessage( scheduledEvent[gateRead].status, scheduledEvent[gateRead].num, scheduledEvent[gateRead].val ); 
     
      // Serial.print(gateRead);
      // Serial.print(" off ");
      // Serial.print(gateDelay[gateRead].num);
      // Serial.print("  ");
      // Serial.println(gateDelay[gateRead].time);

      gateRead = (gateRead+1) % 8;
      processEvent();
    }
  }
}