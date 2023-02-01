//chan is channel A or B, num is the selected drum
byte getDivide(byte chan){
  char val = 0;
  if(globalDivide==0) val = chan==0 ? seqDivide[chan] : seqDivide[chan];
  else {
    val = globalDivide == 1 ? seqDivide[chan]/2 : seqDivide[chan]*2;
  }
  val = val > 0 ? val :  1;
  return val;
}

byte rotate(byte val, char rotation, byte limit){
  //val is limited to a byte
  val = val + rotation * freeze_length;
  //val = val % limit;
  return val;
}

void setDivideTarget(byte _drum, byte chan){
//  seqchan = chan;
//  seqdrum = _drum;
//  
//  lcd.setCursor(2, chan==0 );
//  lcd.write(16);
//  lcd.setCursor(2, chan==1 );
//  switch(_drum){
//    case 0: lcd.write('k'); break;
//    case 1: lcd.write('s'); break;
//    case 2: lcd.write('p'); break;
//    case 3: lcd.write('c'); break;
//    case 4: lcd.write('o'); break;
//    case 5: lcd.write('t'); break;
//    case 6: lcd.write('b'); break;
//  }
//
//  lcd.setCursor(3,chan);
//  if( chan == 0 ) lcd.write(  seqChanADivide[_drum] + 48 );
//  else lcd.write(  seqChanBDivide[_drum] + 48 );
}


void updateSeqLEDs(){
  for(byte i=0;i<8;i++){

//      USBmessage(ccMSG, nano.s1 + i, seq[drum].getAux(0,i)*127 );
//      USBmessage(ccMSG, nano.m1 + i, seq[drum].getAux(1,i)*127 );
//  }
//    else {
//      USBmessage(ccMSG, nano.s1 + i, (seqEnable[0]>>i & 1)*127 );
//      USBmessage(ccMSG, nano.m1 + i, (seqEnable[1]>>i & 1)*127 );
//    }
//    //delay(5);
  }
}


////use to print the contents of an array to serial monitor
//void printArray(int *array, byte size){
//  Serial.print("sequence: ");
//  for(int i=0;i<size;i++){
//    Serial.print(String(array[i]) + "\t");
//  }
//  Serial.println();
//}

void scanI2C(){
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(100);    
}
