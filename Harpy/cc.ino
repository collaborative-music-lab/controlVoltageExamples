typedef struct defCCMsg{
  String name;
  byte val;
};

void setupCC(){
  //ccMsg[0].name = "Start";
}

void processCC(byte num, byte val){
  //lcd.clear();
//  lcd.setCursor(0,1);
//  lcd.print( ccMsg[num-20] .name + " ");
  if(num == key.mod.num) handleModWheel(val);
  else if(num >= key.trackR.num) handleButtons(num, val);
  else if(num >= key.dial1 && num < key.dial1 + 8) handleDials(num,val);
  else handleButtons(num, val);
//  byte pad = 10;
//  pad = pad - (sizeof(ccMsg[num-20].name));
//  while(pad > 2) {
//    for(byte i=0;i<pad;i++) lcd.write(16);
//    pad -= 1;
//  }
}

void handleButtons(byte num, byte val){
  switch(num){
    case key.arrow.num:  if(val>0) cur_chan = key.stop.val > 0 ? 3 : 1; break;
    case key.stop.num: key.stop.val = val; if(val>0) cur_chan = key.arrow.val > 0 ? 3 : 2; break;
    case key.rec.num: key.rec.val = val; break;
  }
}

void handleDials(byte num, byte val){
  num = num - key.dial1;
   switch(num){
     case 0: seqDivide[0] = val/8 + 1; lcd.setCursor(15,0); lcd.write( seqDivide[0]+48) ; break;
     case 4: seqDivide[1] = val/8 + 1; lcd.setCursor(15,1); lcd.write( seqDivide[1]+48) ; break;
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
