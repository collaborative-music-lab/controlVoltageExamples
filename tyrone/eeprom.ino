/*
 * Data stored in EEPROM:
 * 
byte seqChanADivide[7];
byte seqChanBDivide[7];
byte seqEnable[2] = {127,127};
byte bassGain[2] = {127,100};

seq[drum].val(0-7) (8 bytes per seq)
seq[drum].getAux(0); (1 byte per seq)
seq[drum].getAux(1); (1 byte per seq)

total byte stored: 90?
bytes availble in EEPROM: 1023?
8 Slots = 8*90 = 720
each slot begins at 0, 100, 200, etc? 10 slots available. . . .
 */

 void eepromSetup(){
  readPreset(0);
 }

 void readPreset(byte num){
  int index = num*100;
  for(byte i=0;i<NUM_SEQS;i++) {
    seqChanADivide[i] = EEPROM.read(i + index);
    if( SERIAL_DEBUG ) Serial.println("seqADivide " + String(i) + " " + String(seqChanADivide[i]));
  }
  index += NUM_SEQS;
  for(byte i=0;i< NUM_SEQS;i++) {
    seqChanBDivide[i] = EEPROM.read(i + index);
    if( SERIAL_DEBUG ) Serial.println("seqBDivide " + String(i) + " " + String(seqChanBDivide[i]));
  }
  index += NUM_SEQS;
  for(byte i=0;i< 2;i++) {
    seqEnable[i] = EEPROM.read(i + index);
    if( SERIAL_DEBUG ) Serial.println("seqEnable " + String(i) + " " + String(seqEnable[i]));
  }
  index += 2;
  for(byte i=0;i< 2;i++) {
    bassGain[i] = EEPROM.read(i + index);
    if( SERIAL_DEBUG ) Serial.println("bassGain " + String(i) + " " + String(bassGain[i]));
  }
  index += 2;
  
  for(byte i=0;i<NUM_SEQS;i++) {
    for(byte j=0;j< SEQ_LENGTH;j++){
      seq[i].set(j, EEPROM.read(i*SEQ_LENGTH + j + index)); 
      //Serial.println(i*SEQ_LENGTH + j + index);
    }
  }
  index += (NUM_SEQS * SEQ_LENGTH);

  //aux channels are stored as bit arrays
  //we will convert the stored byte to bits
  for(byte i=0;i< NUM_SEQS;i++) {
    byte val = EEPROM.read(i+index);
    if( SERIAL_DEBUG ) Serial.print("Aux  " + String(i) + " ");
    if( SERIAL_DEBUG ) Serial.print(val, BIN);
    if( SERIAL_DEBUG ) Serial.print(" ");
    for(byte j=0;j<SEQ_LENGTH;j++) {
      seq[i].setAux(0,j, (val>>j) & 1);
      if( SERIAL_DEBUG ) Serial.print(seq[i].getAux(0,j));
    }
    if( SERIAL_DEBUG ) Serial.println();
  }
  index += NUM_SEQS;
  for(byte i=0;i< NUM_SEQS;i++) {
    byte val = EEPROM.read(i+index);
    if( SERIAL_DEBUG ) Serial.print("Aux  " + String(i) + " ");
    if( SERIAL_DEBUG ) Serial.print(val, BIN);
    if( SERIAL_DEBUG ) Serial.print(" ");
    for(byte j=0;j<SEQ_LENGTH;j++) {
      seq[i].setAux(1,j, (val>>j) & 1);
      if( SERIAL_DEBUG ) Serial.print(seq[i].getAux(1,j));
    }
    if( SERIAL_DEBUG ) Serial.println();
  }
  index += NUM_SEQS;
  
  if( SERIAL_DEBUG ) Serial.println("Preset " + String(num) + " read");
  if( SERIAL_DEBUG ) Serial.println("recalled " + String(index - num*100) + " bytes");
  cur_preset = num;
  lcd_printPreset();
 }

 void writePreset(byte num){
  int index = num*100;
  for(byte i=0;i<NUM_SEQS;i++) EEPROM.update(i + index, seqChanADivide[i]);
  index += NUM_SEQS;
  for(byte i=0;i< NUM_SEQS;i++) EEPROM.update(i + index, seqChanBDivide[i]);
  index += NUM_SEQS;

  for(byte i=0;i< 2;i++) EEPROM.update(i + index, seqEnable[i]);
  index += 2;
  for(byte i=0;i< 2;i++) EEPROM.update(i + index, bassGain[i]);
  index += 2;

  for(byte i=0;i<NUM_SEQS;i++) {
    for(byte j=0;j< SEQ_LENGTH;j++){
      EEPROM.update(i*SEQ_LENGTH + j + index, seq[i].get(j));
    }
  }
  index += (NUM_SEQS * SEQ_LENGTH);
 
  //aux channels are stored as bit arrays
  //we will convert them d from bits to a byte before storing
  for(byte i=0;i< NUM_SEQS;i++) {
    byte val = 0; 
    for(byte j=0;j<SEQ_LENGTH;j++) val += seq[i].getAux(0, j) << j;
    EEPROM.update( i+index, val);
  }
  index += NUM_SEQS;
  for(byte i=0;i< NUM_SEQS;i++) {
    byte val = 0; 
    for(byte j=0;j<SEQ_LENGTH;j++) val += seq[i].getAux(1, j) << j;
    EEPROM.update( i+index, val);
  }
  index += NUM_SEQS;

  if( SERIAL_DEBUG ) Serial.println("Preset " + String(num) + " saved");
  if( SERIAL_DEBUG ) Serial.println("wrote " + String(index - num*100) + " bytes");
 }
