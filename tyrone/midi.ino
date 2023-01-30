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
  Midi.SendData(msg);
  //Serial.println("USB " + String(command) + " " + String(MIDInote) + " " + String(MIDIvelocity));
}
