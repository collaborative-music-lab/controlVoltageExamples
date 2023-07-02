/*
 * Data stored in EEPROM:

Top row of pads: pitch sequences (always 32 steps)
- 32 bytes for value
- 4*uint32_t (16 bytes) for enable and accent
- total of 48 bytes
- stored at intervals of 50

Bottom row of pads: sequencer params in params struct
- 2x9 bytes, for now- reserved 20 bytes

EEPROM locations:
- pitches(8 sequences): 0 - 399, 50 byte increments
- params(8 presets): 400-559, 20 byte increments

using EEPROM libraries ability to store data structures 
https://docs.arduino.cc/learn/built-in-libraries/eeprom
 */

 void eepromSetup(){
  readPreset(0);
 }

 void readPreset(byte num){
   if(num <8){
    int address = num < 8 ? num * 50 : 0;
    EEPROM.get( address, seq[0].val );
    EEPROM.get( address + sizeof(seq[0].val), seq[0]._aux );
    lcd_printMIDI(num);
    lcd_string(":read  pitch", 3,0);
    cur_pitch_preset = num;
   } else {
    int address = num < 16 ? (num-8) * 20 + 400 : 400;
    EEPROM.get( address, params );
    lcd_printMIDI(num-8);
    lcd_string(":read  seq", 3,1);
    cur_seq_preset = num-8;
   }

  //  int address = num < 20 ? num * 50 : 950;
  //  EEPROM.get( address, params );
  //  EEPROM.get( address + sizeof(params), seq[0].val );
  //  EEPROM.get( address + sizeof(params) + sizeof(seq[0].val), seq[0]._aux );

  // Serial.print( address + sizeof(params) + sizeof(seq[0].val) + sizeof(seq[0]._aux))  ;
  // Serial.println("read from " + String(address));

  // updateGrid();
  // cur_pitch_preset = num;
 }

 void writePreset(byte num){
   if(num <8){
     int address = num < 8 ? num * 50 : 0;
    EEPROM.put( address, seq[0].val );
    EEPROM.put( address + sizeof(seq[0].val), seq[0]._aux );
    lcd_printMIDI(num);
    // lcd_string(":wrote pitch", 3,0);
    cur_pitch_preset = num;
   } else {
    int address = num < 16 ? (num-8) * 20 + 400 : 400;
    EEPROM.put( address, params );
    // lcd_printMIDI(num-8);
    // lcd_string(":wrote seq", 3,1);
    cur_seq_preset = num-8;
   }
 }
