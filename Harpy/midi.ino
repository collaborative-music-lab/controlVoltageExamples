// Poll USB MIDI Controler and send to serial MIDI
void MIDI_poll()
{
  char buf[16];
  uint8_t bufMidi[MIDI_EVENT_PACKET_SIZE];
  uint16_t  rcvd;

  if (Midi.RecvData( &rcvd,  bufMidi) == 0 ) {
    if( bufMidi[1] == 144 ){ //note on channel 1
      sprintf(buf, " %s %d %d %d", "NOTEON 1", bufMidi[1], bufMidi[2], bufMidi[0]);
      Serial.println(buf);
      cur_note_prev = cur_note;
      cur_note = bufMidi[2];
      keys_down = keys_down < 2 ? keys_down + 1: 2;
      Serial.println(keys_down);
    } 
    
    else if( bufMidi[1] == 153 ){ //note on channel 10
      sprintf(buf, " %s %d %d %d", "NOTEON-10", bufMidi[1],bufMidi[2], bufMidi[0]);
      if(bufMidi[2] >= key.pad1) handlePads(bufMidi[2]); //in cc tab
    } 
    else if( bufMidi[1] == 128 ){ //note off
      //sprintf(buf, " %s %d %d %d", "NOTEOFF 1", bufMidi[1],bufMidi[2], bufMidi[0]);
      keys_down = keys_down > 0 ? keys_down - 1: 0;
      Serial.println(keys_down);
    } 
    else if( bufMidi[1] == 137 ){ //note off
      //sprintf(buf, " %s %d %d %d", "NOTEOFF 10", bufMidi[1],bufMidi[2], bufMidi[0]);
    } 
    
    else if( bufMidi[1] == 176 ){ //cc
      sprintf(buf, " %s %d %d", "CC", bufMidi[1], bufMidi[2], bufMidi[3]);
      processCC(bufMidi[2], bufMidi[3]);
    }else if( bufMidi[1] == 224 ){ //pitchbend
      sprintf(buf, " %s %d %d", "pitchbend", bufMidi[1], bufMidi[2], bufMidi[3]);
      //processCC(bufMidi[2], bufMidi[3]);
    } 
    else if( bufMidi[1] == 248 || 0 ){ //ignore
      return 0;
    }  else { //other
      sprintf(buf, " %s %02X %02X %02X %02X", "misc", bufMidi[0], bufMidi[1], bufMidi[2], bufMidi[3]);
      //Serial.println(buf);
//    if(bufMidi[0] != 15){
//      for (int i = 0; i < MIDI_EVENT_PACKET_SIZE; i++) {
//        sprintf(buf, " %02X", bufMidi[i]);
//        Serial.print(buf);
//      }
//      Serial.println("");
//    }
    }
    Serial.println(buf);
  }
}

//send MIDI message
void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  MIDI.write(command);//send note on or note off command 
  MIDI.write(MIDInote);//send pitch data
  MIDI.write(MIDIvelocity);//send velocity data
  Serial.println("Midi " + String(command) + " " + String(MIDInote) + " " + String(MIDIvelocity));
}

void USBmessage(byte command, byte MIDInote, byte MIDIvelocity){
  byte msg[] = {command, MIDInote, MIDIvelocity};
  Midi.SendData(msg);
  //Serial.println("USB " + String(command) + " " + String(MIDInote) + " " + String(MIDIvelocity));
}
