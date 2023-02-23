void updateGridLed(byte num, byte val, byte color){
  USBmessage(lp.KEY_ON, num%8 + num/8*16,  color*(val>0));
  // Serial.print("updateGrid ");
  // Serial.print(num%8 + (num/8*16));
  // Serial.print(" ");
  // Serial.println(color);
}

void updatePatternSelect(byte num){
  patternSelect = num;
  for(byte i=0;i<4;i++){
    if(i==num) USBmessage(lp.KEY_ON, lp.ctlY[i+4].num, patternColor[i]);
    else USBmessage(lp.KEY_ON, lp.ctlY[i+4].num, 0);
  }
  //Serial.println(String(num) + " " + String(patternColor[num]));
}

// void updateEffectSelect(byte num){
//   effectSelect = num;
//   for(byte i=0;i<4;i++){
//     if(i==num) USBmessage(lp.KEY_ON, lp.ctlY[i].num, effectColor[i]);
//     else USBmessage(lp.KEY_ON, lp.ctlY[i].num, 0);
//   }
//   //Serial.println(String(num) + " " + String(effectColor[num]));
// }

void clearGrid(){
  for( byte i=0;i<8;i++){
    for( byte j=0;j<8;j++){
      updateGridLed(i+j*8,0,0);
      // delay(1);
    }
    for( byte j=0;j<4;j++){
      seq[0].setAux(j,i,0);
    }
  }
}

//update grid updates all of the LEDs when a preset is recalled or a page is selected
void updateGrid(){
  Serial.println(launchpad_page);
  if(launchpad_page == 0){
    char name[] = "trig";
    lcd_string(name,0,1);
    for( byte i=0;i<4;i++){
      for( byte j=0;j<8;j++){
        updateGridLed(i+j*4 + 32,seq[0].getAux(i,j),patternColor[i]);
      }
      for( byte j=0;j<4;j++){
        //handle clock divide and enable and seq select LEDs here
      }
    }
  }else if(launchpad_page == 1){
    char name[] = "mod ";
    lcd_string(name,0,1);
    for( byte i=0;i<4;i++){
      for( byte j=0;j<8;j++){
        updateGridLed(i+j*4 + 32,seq[0].getAux(i,j),patternColor[i]);
      }
      for( byte j=0;j<4;j++){
        //handle clock divide and enable and seq select LEDs here
      }
    }
  } else if(launchpad_page == 2){
    char name[] = "CV A";
    lcd_string(name,0,1);
    for( byte i=0;i<4;i++){
      for( byte j=0;j<8;j++){
        updateGridLed(i+j*4 + 32,seq[0].getAux(i,j),patternColor[i]);
      }
      for( byte j=0;j<4;j++){
        //handle clock divide and enable and seq select LEDs here
      }
    }
  } else if(launchpad_page == 3){
    char name[] = "CV B";
    lcd_string(name,0,1);
    for( byte i=0;i<4;i++){
      for( byte j=0;j<8;j++){
        updateGridLed(i+j*4 + 32,seq[0].getAux(i,j),patternColor[i]);
      }
      for( byte j=0;j<4;j++){
        //handle clock divide and enable and seq select LEDs here
      }
    }
  }
}

//chan is channel A or B, num is the selected drum
byte getDivide(byte chan, byte num){
  char val = 0;
  if(globalDivide==0) val = params.seqDivide[num] ;
  else {
    if( chan == 0 ) val = globalDivide == 1 ? params.seqDivide[num]/2 : params.seqDivide[num]*2;
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

void looptimer(){
  static uint16_t count;
  static uint32_t timer;

  if(millis()-timer > 250) {
    timer = millis();
    Serial.println("count " + String(count));
    count = 0;
  }
  count++;
}

