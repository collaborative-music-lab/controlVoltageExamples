

void processNoteOn(byte num, byte val){
  if((num+8) % 16 == 0){ //ctlY buttons
    handleCtlY(num/16,val);
  } else handleGrid(num,val);
}

void handleCtlY(byte num, byte val){
  lp.ctlY[num].val = val;
  if(num>3 && val>0) {
    updatePatternSelect(num-4);
    //num_subdiv = num-3;
    char name[] = "pattern ";
    lcd_string(name,0,1);
    lcd_print(num-4+48);
  }
  else {
    processGlobalMsgs(num,val);
  }
}

void processGlobalMsgs(byte num, byte val){
  lp.ctlY[num].val = val;
  char nameStore[] = "store?? ";
  char nameDivide[] = "gDivide ";
  char nameRepeat[] =  "gRepeat ";
  char nameStop[] =  "gStop.  ";
  switch(num){
    case 0:  lcd_string(nameStore,0,1); break;
    case 1:  lcd_string(nameDivide,0,1); globalDivide = val>0 ? 1 : 0; break;
    case 2:  lcd_string(nameRepeat,0,1); globalRepeat = val>0 ? 1 : 0; freeze_index = main_index-freeze_length; break; //does freeze_index wrap?
    case 3: lcd_string(nameStop,0,1);  globalStop = val>0 ? 1 : 0; break;
  }
  Serial.print(num);
  Serial.println(val);
  lcd_printMIDI((val>0));
}

void handleGrid(byte num, byte val){
  if( val == 0) return;
  //convert note nums to 0-63
  num = num - (8* (num/16));
  
  if(num < 32  ){ //pattern groupings

  /*below for working with four rhythm tracks*/
    byte patternNum = (num>15)*2 + (num % 8) / 4;
    if( num%16 < 8) setPatternDivide(patternNum, num);
    else programPatternSeq(patternNum, num);

    /*below for creating a sequence*/
    //seq[0].set(num%8, (num/8)*15 );
    // byte patternNum = (num%8)/2 + (num / 16) * 2;
    // if( patternNum == 0 & num == 0 && seqDivide[0] < 4) seqDivide[0] = 4;
    // else if ( patternNum == 0 & num == 8 && seqDivide[0] != 2) seqDivide[0] = 2;
    // else seqDivide[0] = 1;


  } else{ //rhythm programming
    num = num-32;
    seq[0].setAux( num/8, num%8, !seq[0].getAux(num/8, num%8));
    updateGridLed(num+32, seq[0].getAux(num/8, num%8), patternColor[num/8]);

  }
}

void setPatternDivide(byte num, byte val){
  val = val - (val/16) * 8;
  Serial.println(val);
  lcd_string("seq ",0,0);
  lcd_printMIDI(val/4);
  //enable/disable sequence
  if( val%4 == 0) { 
    params.seqEnable[val/4] = !params.seqEnable[val/4];
    updateGridLed(val + (val>7)*8, params.seqEnable[val/4], effectColor[0]);
    lcd_string(" enab ", 7,0);
    lcd_printMIDI(params.seqEnable[val/4]);
    return;
  }
  //set subdivide
  val = val%4;

  //turn off previous subdivide led
  if( params.seqDivide[num]>1) updateGridLed(num*4 + (params.seqDivide[num]<8?params.seqDivide[num]/2:3) + (num>1)*8, 0, effectColor[2]);
  if( params.seqDivide[num] == 1<<val) params.seqDivide[num] = 1;
  else{
    params.seqDivide[num] = 1<<val;
    updateGridLed(num*4 + (params.seqDivide[num]<8?params.seqDivide[num]/2:3) + (num>1)*8, 1, effectColor[2]);
  }
  Serial.print(val);
  Serial.println(params.seqDivide[num]);
  lcd_string(" divi ", 7,0);
    lcd_printMIDI(params.seqDivide[num]);
}

void programPatternSeq(byte chan, byte num){
  params.seqSelect[chan][num%4] = patternSelect;
  updateGridLed(num, 1, patternColor[patternSelect]);
}

void setupCC() {
  //ccMsg[0].name = "Start";
}

void processCC(byte num, byte val) {
  //the only CC is padX
  //used to store and recall sequences
  //and switch display pages
  
  if(val == 0) return;
  num = num - lp.ctlX[0].num;
  //read/write presets
  //4 banks of 4. bank selected by a,b,c,d, preset selected 
  if(num < 4){
    byte preset_bank = 0;
    byte recall_bank = 1;
    for(byte i=0;i<4;i++) preset_bank = lp.ctlY[i].val > 0 ? i+1 : preset_bank;

    USBmessage(lp.CC, cur_preset + lp.ctlX[0].num,  0);

    if(preset_bank > 0){
      writePreset(num  + (preset_bank-1)*4);
      lcd_string("wrote preset ", 0,0);
      lcd_printMIDI(num  + (preset_bank-1)*4);
    }
    else {
      for(byte i=0;i<4;i++) recall_bank = 1; //lp.ctlY[i+4].val > 0 ? i+1 : 0;
      readPreset(num + (recall_bank-1)*4);
      lcd_string("read preset  ", 0,0);
      lcd_printMIDI(num  + (recall_bank-1)*4);
    }
    USBmessage(lp.CC, cur_preset + lp.ctlX[0].num,  51);
  } //presets
  else if(num < 8){
    USBmessage(lp.CC, launchpad_page + lp.ctlX[0].num + 4,  0);
    launchpad_page = num - 4;
    updateGrid();
    USBmessage(lp.CC, launchpad_page + lp.ctlX[0].num + 4,  48);
  }
}
