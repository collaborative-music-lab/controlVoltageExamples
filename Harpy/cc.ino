typedef struct defCCMsg{
  String name;
  byte val;
};

defCCMsg ccMsg[10] = {
  {"Start"}, {"End"}, {"transpose"}, {"length"}
};


//ccMsgs[0] = "Start";
//ccMsgs[1] = "End";
//ccMsgs[2] = "transpose";
//ccMsgs[3] = "length";
//ccMsgs[4] = "choke";
//ccMsgs[5] = "tempo";
//ccMsgs[6] = "none";
//ccMsgs[7] = "none";
//ccMsgs[8] = "none";

void setupCC(){
  //ccMsg[0].name = "Start";
}

void processCC(byte num, byte val){
  //lcd.clear();
//  lcd.setCursor(0,1);
//  lcd.print( ccMsg[num-20] .name + " ");
  if(num == key.mod.num) handleModWheel(val);
  else if(num >= key.trackR.num) handleButtons(num, val);
  else if(num >= key.dial1 && num < key.dial1 + 8){
  
//    switch(num){
//      case 0: seqStart = val / (128/seqLength); lcd.print( String(seqStart)) ; break;
//      case 1: seqEnd = val / (128/seqLength); lcd.print( String(seqEnd)) ; break;
//      case 2: transpose = ((int)val-64) / 9; lcd.print( String(transpose)) ; break;
//      case 3: curLength = val/16 + 1; lcd.print( String(curLength)) ; break;
//      case 23: minChoke = (127-val) + 0; lcd.print( String(minChoke)) ; break;
//      case 24: tempo = (val*val/30) + 20; lcd.print( String(tempo)) ; break;
//      //case 27: gateMode = val/35; break;
//    }
  } else handleButtons(num, val);
//  byte pad = 10;
//  pad = pad - (sizeof(ccMsg[num-20].name));
//  while(pad > 2) {
//    for(byte i=0;i<pad;i++) lcd.write(16);
//    pad -= 1;
//  }
}

void handleButtons(byte num, byte val){
  if (num == key.arrow.num){
    key.arrow.val = val;
    if(val>0) cur_chan = key.stop.val > 0 ? 3 : 1;
    Serial.println("chan " + String(cur_chan));
  } else if (num == key.stop.num){
    key.stop.val = val;
    if(val>0) cur_chan = key.arrow.val > 0 ? 3 : 2;
    Serial.println("chan " + String(cur_chan));
  }
}

void handlePads(byte num){
  num = num - key.pad1;
  Serial.print(num);
  Serial.print(" ");
  num = key.padMap[num];
  Serial.println(num);
  
  if(cur_chan & 1){
    seq[0].setAux(0, num, !seq[0].getAux(0,num) );
    lcd_update_enable(0,num);
  }
  if(cur_chan>>1 & 1){
    seq[1].setAux(0, num, !seq[1].getAux(0,num) );
    lcd_update_enable(1,num);
  }
}
 void handleModWheel(byte val){
  MIDImessage( nifty.CC, 1, val );
 }
