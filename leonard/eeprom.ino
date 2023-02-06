/*
 * Data stored in EEPROM:
 * 

 params struct //24 bytes
 seq[0].aux 0-3 //4 bytes
 seq[0].val //8 bytes

 sequences are stored at increments of 50

using EEPROM libraries ability to store data structures 
https://docs.arduino.cc/learn/built-in-libraries/eeprom
 */

 void eepromSetup(){
  readPreset(0);
 }

 void readPreset(byte num){
  //  if(num == cur_preset){
  //    updateGrid();
  //    return 0;
  //  }

   int address = num < 20 ? num * 50 : 950;
   EEPROM.get( address, params );
   EEPROM.get( address + sizeof(params), seq[0].val );
   EEPROM.get( address + sizeof(params) + sizeof(seq[0].val), seq[0]._aux );

  Serial.print( address + sizeof(params) + sizeof(seq[0].val) + sizeof(seq[0]._aux))  ;
  Serial.println("read from " + String(address));

  updateGrid();
  cur_preset = num;
 }

 void writePreset(byte num){
  int address = num < 20 ? num * 50 : 950;
   EEPROM.put( address, params );
   EEPROM.put( address + sizeof(params), seq[0].val );
   EEPROM.put( address + sizeof(params) + sizeof(seq[0].val), seq[0]._aux );

   Serial.print( address + sizeof(params) + sizeof(seq[0].val) + sizeof(seq[0]._aux))  ;
  Serial.println("written to " + String(address));

  //update preset led
  USBmessage(lp.CC, cur_preset + lp.ctlX[0].num,  0);
  cur_preset = num;
  USBmessage(lp.CC, cur_preset + lp.ctlX[0].num,  51);

 }
