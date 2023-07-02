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
    case 104: key.arrow.val = val; if(val>0) cur_chan = key.stop.val > 0 ? 3 : 1;  break;
    case 105: key.stop.val = val; if(val>0) cur_chan = key.arrow.val > 0 ? 3 : 2;  break;
    case 108: key.shift.val = val; printParams(0); printParams(1); LK_LED(SHIFT, val > 0 ? WHITE: OFF, ON); break;
    case 115: key.play.val = val; seqEnable = cur_chan * (val>0); LK_LED(PLAY, val > 0 ? WHITE: OFF, ON); break;
    case 117: key.rec.val = val; LK_LED(REC, val > 0 ? WHITE: OFF, ON); break;

  }
  if(num==104 || num==105){
    LK_LED(A, (cur_chan&1)>0 ? ORANGE: 0, ON);
    LK_LED(B,  (cur_chan&2)>0 ? BLUE: 0, ON);
  }
  else if(num==key.rec.num || num==key.play.num){
    LK_LED_PAD(cur_pitch_preset ,  RED,  (val>0)*1 );
    LK_LED_PAD(cur_seq_preset+8 ,  RED,  (val>0)*1 );
  }

  Serial.println("cc " + String(num) + " " + String(val));
}

//Dials:
//start, end, divide, length ???
void handleDials(byte num, byte val){
  num = num - key.dial1;
  byte _divide[] = {1,2,3,4,6,8,12,16,24,32};

  if(num==0 && params.seqDivide[0] != _divide[val/13]){ params.seqDivide[0] = _divide[val/13]; 
    lcd_string("divideA ", 0,0); lcd_printMIDI( params.seqDivide[0] );
    } else if(num==1 && params.seqStart[0] != (val/8)*2){ params.seqStart[0] = (val/8)*2; 
    lcd_string("startA  ", 0,0); lcd_printMIDI( params.seqStart[0] );
    } else if(num==2 && params.seqEnd[0] != (val/8)*2+1){ params.seqEnd[0] = (val/8)*2+1; 
    lcd_string("endA    ", 0,0); lcd_printMIDI( params.seqEnd[0] );
    } else if(num==3 && params.transpose[0] != char(val/9)-7){ params.transpose[0] = char(val/9)-7; 
    lcd_string("transpA ", 0,0); lcd_printMIDI( params.transpose[0] );
    }else if(num==4 && params.seqDivide[1] != _divide[val/13]){ params.seqDivide[1] = _divide[val/13]; 
    lcd_string("divideB ", 0,1); lcd_printMIDI( params.seqDivide[1] );
    } else if(num==5 && params.seqStart[1] != (val/8)*2){ params.seqStart[1] = (val/8)*2; 
    lcd_string("startB  ", 0,1); lcd_printMIDI( params.seqStart[1] );
    } else if(num==6 && params.seqEnd[1] != (val/9)*2+1){ params.seqEnd[1] = (val/8)*2+1; 
    lcd_string("endB    ", 0,1); lcd_printMIDI( params.seqEnd[1] );
    } else if(num==7 && params.transpose[1] != char(val/9)-7){ params.transpose[1] = char(val/9)-7; 
    lcd_string("transpB ", 0,1); lcd_printMIDI( params.transpose[1] );
    }
}

void handlePads(byte num){
  num = num - key.pad1;
  num = key.padMap[num];

  if(key.rec.val > 0 && key.play.val>0){
    writePreset(num);
    return 0;
  } else if(key.play.val>0){
    readPreset(num);
    return 0;
  }

  //press a pad to jump to that point in the sequence
  //if we are in the latter half of a beat, go to the selected beat on the next clock
  //otherwise go to the selected beat immediately
  for(byte i=0;i<2;i++){
    if(cur_chan>>i & 1){
      if(params.index[i]%2 > 0 ) params.index[i] = params.index[i]%2 + ((num+31)%32)*2;
      else params.index[i] = params.index[i]%2 + num*2;
    }
  }
  
  // if(cur_chan & 1){
  //   seq[0].setAux(0, num, !seq[0].getAux(0,num) );
  //   lcd_update_enable(0,num);
  // }
  // if(cur_chan>>1 & 1){
  //   seq[1].setAux(0, num, !seq[1].getAux(0,num) );
  //   lcd_update_enable(1,num);
  // }
}//pads

 void handleModWheel(byte val){
  MIDImessage( nifty.CC, 1, val );
 }
