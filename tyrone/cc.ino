typedef struct defCCMsg {
  String name;
  byte val;
};

defCCMsg ccMsg[10] = {
  {"Start"}, {"End"}, {"transpose"}, {"length"}
};


void setupCC() {
  //ccMsg[0].name = "Start";
}

void processCC(byte num, byte val) {
  if (num >= nano.fader1 && num <= nano.fader1 + 7 ) handleFaders(num,val);
  else if (num >= nano.dial1 && num <= nano.dial1 + 7 ) handleDials(num,val);
  else if( num >= 41 && num <= 46) handleTransport(num, val); 
  else if( num >= 58 && num <= 62) handleTransport(num,val); 

  //strip note off for toggle switches
  if(val==0) return 0;
  if(num >= 64 && num <= 71) selectSequence(num);
  else if (num >= nano.s1 && num <= nano.s1+7 ) setSeq(0,num);
  else if (num >= nano.m1 && num <= nano.m1+7 ) setSeq(1,num);

}

//sequence selection using r keys
//or recall a preset if cycle is held down
void selectSequence(byte num){
  //cycle enables saving presets
  if( nano.cycle.val>0){
    writePreset(num - nano.r1);
    return 0;
  }
  else if( nano.rec.val > 0){
    readPreset(num - nano.r1);
    return 0;
  }
  
  //select drum sequence or global
  byte temp = GLOBAL;
  lcd.setCursor(0,0);
  switch(num-nano.r1){
    case 0: temp=0; lcd.write('k'); break;
    case 1: temp=1; lcd.write('s'); break;
    case 2: temp=2; lcd.write('p'); break;
    case 3: temp=3; lcd.write('c'); break;
    case 4: temp=4; lcd.write('o'); break;
    case 5: temp=5; lcd.write('t'); break;
    case 6: temp=6; lcd.write('b'); break;
    case 7: temp=7; break;
  }
  if(temp == drum || temp == 7){ //enter global mode
    USBmessage(ccMSG, drum+64, 0);
    drum = 7;
    lcd.setCursor(0,0);
    lcd.write('g');
    lcd_printEnable(0);
    lcd_printEnable(1);
  } else { //enter a drum voice edit mode
    USBmessage(ccMSG, drum+64, 0);
    drum = temp;
    USBmessage(ccMSG, drum+64, 127);
    lcd_print_subdiv(seqchan);
    lcd_printGains();
  }
  updateSeqLEDs();
}

//using s and m rows to enter a sequence
void setSeq(byte seqNum, byte num){
  //if a drum is selected this enables steps
  if(drum<7){
    if(seqNum==0){
      byte _step = num - nano.s1;
      seq[drum].setAux(0, _step, !seq[drum].getAux(0, _step));
      USBmessage(ccMSG, num, seq[drum].getAux(0, _step) * 127);
      setDivideTarget(drum,0);
    } else {
      byte _step = num - nano.m1;
      seq[drum].setAux(1, _step, !seq[drum].getAux(1, _step));
      USBmessage(ccMSG, num, seq[drum].getAux(1, _step) * 127);
      setDivideTarget(drum,1);
    }
  } else{
    //in global mode we turn enables on and off
    if(seqNum==0){
      byte _step = num - nano.s1;
      seqEnable[0] = seqEnable[0] ^ 1<<_step;
      USBmessage(ccMSG, num,  ((seqEnable[0]>>_step) & 1) * 127);
      setDivideTarget(_step,0);
      lcd_printEnable(0);
    } else {
      byte _step = num - nano.m1;
      seqEnable[1] = seqEnable[1] ^ 1<<_step;
      USBmessage(ccMSG, num,  ((seqEnable[1]>>_step) & 1) * 127);
      setDivideTarget(_step,1);
      lcd_printEnable(1);
    }
  }
}

void handleFaders(byte num, byte val){
  num = num - nano.fader1;
  seq[6].set(num,val);
}

void handleDials(byte num, byte val){
  //dials set the velocity for SEQ A&B
  num = num - nano.dial1;
  
  if(num < NUM_SEQS) {
    if(num<6){ //drum voices
      if(val<64){
        seq[num].set(1,val*2 + 1);
        seq[num].set(0,127);
      } else{
        seq[num].set(0, 127 - (val-64)*2 );
        seq[num].set(1,127);
      }   
      lcd_printGains();
    } else{ //synth voice
      if(val<64){
        bassGain[1] = val*2 + 1;
        bassGain[0] = 127;
      } else{
        bassGain[0] = 127 - (val-64)*2;
        bassGain[1] = 127;
      }   
      lcd_printGains();
    }
  } else if (num == 7) globalRotate = val / (128/SEQ_LENGTH);
}

void handleTransport(byte num, byte val){
  //Serial.println("Handle " + String(num)+ " " + String(val));
  
  if(num == nano.rec.num) {
    nano.rec.val = val; //rec enables preset recall
    lcd_printPreset();
  }
  
  else if(num == nano.play.num) { //repeat
    if(val>0)freeze_index = main_index;
    globalRepeat = val>0;
    nano.play.val = !nano.play.val;
  }
  
  else if(num == nano.stop.num) { //stop
    globalStop = val>0;
    nano.stop.val = !nano.stop.val;
  }
  
  else if(num == nano.ff.num) { //change playback rate
    globalDivide = val > 0 ? 1 : 0;
    nano.ff.val = !nano.ff.val;
  }
  else if(num == nano.rew.num) {
    globalDivide = val > 0 ? -1 : 0;
    nano.rew.val = !nano.rew.val;
  }

  //TBD
  else if(num == nano.markerL.num) nano.markerL.val = !nano.markerL.val;
  else if(num == nano.markerR.num) nano.markerR.val = !nano.markerR.val;
  else if(num == nano.set.num) nano.set.val = !nano.set.val;
  
  else if(num == nano.cycle.num) {
    nano.cycle.val = !nano.cycle.val; //cycle enables preset writing
    lcd_printPreset();
  }
  
  //change subdivide
  else if(num == nano.trackL.num && val>0) incStepSize(seqdrum,1);
  else if(num == nano.trackR.num && val>0) incStepSize(seqdrum,-1);
}

void incStepSize(byte num, char inc){
  seq[num].subDivide += inc;
  seq[num].subDivide = seq[num].subDivide < 1 ? 1 : seq[num].subDivide;

  if( seqchan == 0 ) {
    seqChanADivide[drum] += inc;
    if( seqChanADivide[drum] < 1 ) seqChanADivide[drum] = 1;
  }
  else {
    seqChanBDivide[drum] += inc;
    if( seqChanBDivide[drum] < 1 ) seqChanBDivide[drum] = 1;
  }
  lcd_print_subdiv(seqchan);
}
