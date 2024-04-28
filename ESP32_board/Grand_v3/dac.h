struct defDac{
  byte pin;
  int inc;
  uint16_t val;
};

defDac dacs[2] = {{25,110},{26,503}};

void processDac(){
  uint16_t val = 0;
  for(byte i=0;i<1;i++){
    dacs[i].val = (dacs[i].val + dacs[i].inc);
    val = dacs[i].val>>8;
    //val = 200;
    dacWrite( dacs[i].pin, val );
    //Serial.print(" " + String(val));
  }
  for(byte i=1;i<2;i++){
    dacs[i].val = (dacs[i].val + dacs[i].inc);
    val = dacs[i].val>>8;
    //
    dacWrite( dacs[i].pin, val );
    //Serial.print(" " + String(val));
  }
  //Serial.println();
}