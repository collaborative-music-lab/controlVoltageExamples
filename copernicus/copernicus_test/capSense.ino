void capSetup(){
  byte mpr_found = cap.begin(0x5A);

  static byte capSenseTestCounter = 0;
  while (!mpr_found) {
    
    Serial.println("MPR121 not found, check wiring?");
    capSenseTestCounter++;
    if(capSenseTestCounter > 20 ) break;
    delay(100);
    mpr_found = cap.begin(0x5A);
  }
  if( mpr_found = cap.begin(0x5A)){
    Serial.println("MPR121 found!");
  } else Serial.println("MPR121 not found. . . ");
  
  cap.chargeCurrent(16); //0-63, default 16
  cap.chargeTime(4);// 0-7, def 1
  cap.num_electrodes = 8;
  

  cap.enableGPIO(4,7);
  cap.enableGPIO(5,7);
  cap.enableGPIO(6,7);
  cap.enableGPIO(7,7);
}

void capLoop(){
  static uint16_t cap_vals = 0;

  uint16_t vals = cap.touched();
  if(vals != cap_vals){
    cap_vals = vals;
    Serial.println("touch " + String(cap_vals, BIN));
  }
}
