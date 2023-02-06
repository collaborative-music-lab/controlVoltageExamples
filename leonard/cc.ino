

void processNoteOn(byte num, byte val){
  if((num+8) % 16 == 0){ //ctlY buttons
    handleCtlY(num/16,val);
  } else handleGrid(num,val);
}

void handleCtlY(byte num, byte val){
  lp.ctlY[num].val = val;
  if(num>3 && val>0) {
    updatePatternSelect(num-4);
    num_subdiv = num-3;
  }
  else {
    processGlobalMsgs(num,val);
  }
}

void processGlobalMsgs(byte num, byte val){
  switch(num){
    case 0:  lp.ctlY[0].val = val; break;
    case 1:  globalDivide = val>0 ? 2 : 1; break;
    case 2:  globalRepeat = val>0 ? 1 : 0; freeze_index = main_index; break;
    case 3:  globalStop = val>0 ? 1 : 0; break;
  }
  Serial.print(num);
  Serial.println(val);
}

void handleGrid(byte num, byte val){
  if( val == 0) return 0;
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
  //enable/disable sequence
  if( val%4 == 0) { 
    params.seqEnable[val/4] = !params.seqEnable[val/4];
    updateGridLed(val + (val>7)*8, params.seqEnable[val/4], effectColor[0]);
    return 0;
  }
  //set subdivide
  val = val%4;
  val = val;
  Serial.print(val);
  Serial.println(params.seqDivide[num]);
  //turn off previous subdivide led
  if( params.seqDivide[num]>1) updateGridLed(num*4 + (params.seqDivide[num]<8?params.seqDivide[num]/2:3) + (num>1)*8, 0, effectColor[2]);
  if( params.seqDivide[num] == 1<<val) params.seqDivide[num] = 1;
  else{
    params.seqDivide[num] = 1<<val;
    updateGridLed(num*4 + (params.seqDivide[num]<8?params.seqDivide[num]/2:3) + (num>1)*8, 1, effectColor[2]);
  }
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
  
  if(val == 0) return 0;
  USBmessage(lp.CC, cur_preset + lp.ctlX[0].num,  0);

  if(lp.ctlY[0].val > 0){
    writePreset(num - lp.ctlX[0].num);
  }
  else readPreset(num - lp.ctlX[0].num);

  USBmessage(lp.CC, cur_preset + lp.ctlX[0].num,  51);

}
