void LK_LED(LK_BUTTONS num, LK_COLOR val, LK_FLASH mode){
  if(num == PAD) USBmessage2(key.LED, key.pad1+key.padOffset[num], lk_color[val]);
  else if( num == A) USBmessage2(key.CC + mode, key.arrow.num , lk_color[val]);
  else if( num == B) USBmessage2(key.CC + mode, key.stop.num , lk_color[val]);
  else if( num == REC) USBmessage2(key.CC+15, key.rec.num, (lk_color[val]>0)*127);
  else if( num == PLAY) USBmessage2(key.CC+15, key.play.num, (lk_color[val]>0)*127);
  else if( num == SHIFT) USBmessage2(key.CC, key.shift.num, (lk_color[val]>0)*127);
}

void LK_LED_PAD(LK_BUTTONS num, LK_COLOR val, LK_FLASH mode){
  USBmessage2(key.LED, key.pad1+key.padOffset[num], lk_color[val]);
}

//chan is channel A or B, num is the selected drum
byte getDivide(byte chan){
  char val = 0;
  if(globalDivide==0) val = chan==0 ? params.seqDivide[chan] : params.seqDivide[chan];
  else {
    val = globalDivide == 1 ? params.seqDivide[chan]/2 : params.seqDivide[chan]*2;
  }
  val = val > 0 ? val :  1;
  return val;
}

byte rotate(byte val, char rotation){
  val = val + rotation * freeze_length;
  return val;
}

void printParams(byte i){
  Serial.println("divide " + String(params.seqDivide[i]));
  Serial.println("start " + String(params.seqStart[i]));
  Serial.println("end " + String(params.seqEnd[i]));
  Serial.println("divide " + String(params.seqDivide[i]));
  Serial.println("divide " + String(params.seqDivide[i]));
}

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
