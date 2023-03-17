// Poll USB MIDI Controler and send to serial MIDI
void MIDI_poll()
{
  char buf[16];
  uint8_t bufMidi[MIDI_EVENT_PACKET_SIZE];
  uint16_t  rcvd;

  if (Midi.RecvData( &rcvd,  bufMidi) == 0 ) {
    /******NOTEON********/
    if( bufMidi[0] == 9 ){ //note on
      sprintf(buf, " %s %d %d", "NOTE", bufMidi[2], bufMidi[3]);
      processNoteOn(bufMidi[2], bufMidi[3]);
      Serial.println(buf);
    } 
    /******NOTE OFF********/
    else if( bufMidi[0] == 8 ){ //note off
      sprintf(buf, " %s %d %d", "NOTE", bufMidi[2],bufMidi[3], 0);
      Serial.println(buf);
    } 
    /******CC MSG********/
    else if( bufMidi[0] == 11 ){ //cc
      sprintf(buf, " %s %d %d %d", "CC", bufMidi[1], bufMidi[2], bufMidi[3]);
      Serial.println(buf);
      processCC(bufMidi[2], bufMidi[3]);
    } else for (int i = 0; i < MIDI_EVENT_PACKET_SIZE; i++) {
      sprintf(buf, " %02X", bufMidi[i]);
      Serial.print(buf);
    }
  }
}

//send MIDI message
void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  MIDI.write(command);//send note on or note off command 
  MIDI.write(MIDInote);//send pitch data
  MIDI.write(MIDIvelocity);//send velocity data
  //Serial.println("Midi " + String(command) + " " + String(MIDInote) + " " + String(MIDIvelocity));
}

void USBmessage(byte command, byte MIDInote, byte MIDIvelocity){
  byte msg[] = {command, MIDInote, MIDIvelocity};
  Midi.SendData(msg, 1);
  //Serial.println("USB " + String(command) + " " + String(MIDInote) + " " + String(MIDIvelocity));
}


/************** schedule midi event **********/
/*
Due to the buffer size constraints we are only scheduling 16 midi events at a time.
If 16 events are already scheduled, then we will send out the event soonest in time to clear up a spot
and then schedule the new event in that slot
*/
struct defMidiEventPacket{
  byte active;
  byte status;
  byte num;
  byte val;
  uint32_t time;
};
const byte midi_event_buffer_size = 16;
defMidiEventPacket midiPacket[midi_event_buffer_size];

byte midiPacketWrite = 0;
byte midiPacketRead = 0;

void scheduleMidiEvent(byte status, byte num, byte val, int delay){ //channel note number, delay time
  uint32_t nextEventTime = 0;
  if(delay == 0){
    MIDImessage( status, num, val ); 
    return 0;
  }

  for(byte i=0;i<midi_event_buffer_size; i++){
    if(midiPacket[i].active == 0){
      midiPacket[i].active = 1;
      midiPacket[i].status = status;
      midiPacket[i].num = num;
      midiPacket[i].val= val;
      midiPacket[i].time =  millis()+delay;
      return 0;
    } else if(nextEventTime < midiPacket[i].time){
      nextEventTime = i;
    }
  } 
  // if no empty slot was found
    Serial.println("double");
    MIDImessage( midiPacket[nextEventTime].status, midiPacket[nextEventTime].num, midiPacket[nextEventTime].val ); 

    midiPacket[nextEventTime].active = 1;
    midiPacket[nextEventTime].status = status;
    midiPacket[nextEventTime].num = num;
    midiPacket[nextEventTime].val= val;
    midiPacket[nextEventTime].time =  millis()+delay;
  
  // Serial.print(midiPacketWrite);
  // Serial.print(" on ");
  // Serial.print(num);
  // Serial.print("  ");
  // Serial.println(delay);
  // Serial.print(midiPacket[midiPacketWrite].time);
  // Serial.print("  ");
  // Serial.println(millis());
}

void processMidiPacket(){
  for(byte i=0;i<midi_event_buffer_size;i++){
    if(midiPacket[i].active == 1){
        if( millis() > midiPacket[i].time) {
          MIDImessage( midiPacket[i].status, midiPacket[i].num, midiPacket[i].val ); 
          midiPacket[i].active = 0;
        
          // Serial.print("midi ");
          // Serial.print(i);
          // Serial.print("  ");
          // Serial.print(midiPacket[i].num);
          // Serial.print("  ");
          // Serial.println(params.seqTiming[i]);

          // midiPacketRead = (midiPacketRead+1) % midi_event_buffer_size;
          // processMidiPacket();
        
      }
    }
  }
}