void lcd_print(char num){
  lcd.write(num);
}

// void lcd_print_subdiv(byte chan){
  
//   lcd.setCursor(3,chan);
//   if( chan == 0 ) lcd.write(  seqDivide[drum] + 48 );
//   else lcd.write(  seqChanBDivide[drum] + 48 );
// }

// void lcd_printGains(){
//   lcd.setCursor(5,0);
//   if(drum < 6) lcd_printMIDI( seq[drum].get( 0 ) );
//   else if( drum == 6 ) lcd_printMIDI( bassGain[0] );
  
//   lcd.setCursor(5,1);
//   if(drum < 6) lcd_printMIDI( seq[drum].get( 1 ) );
//   else if( drum == 6 ) lcd_printMIDI( bassGain[1] );
// }

void lcd_printMIDI(byte val){
  lcd.write(  val/100 + 48 );
  lcd.write(  val/10 % 10 + 48);
  lcd.write(  val% 10 + 48);
}

void lcd_printNOTE(byte val){
  lcd.setCursor(0,1);
  lcd.write(  val/10 % 10 + 48);
  lcd.write(  val% 10 + 48);
}

void lcd_printEnable(byte chan){
  lcd.setCursor(9,chan);
  for(int i=0;i<NUM_SEQS;i++){
    if( (params.seqEnable[chan]>>i) & 1 ) {
//      switch(i){
//        case 0: lcd.write('k'); break;
//        case 1: lcd.write('s'); break;
//        case 2: lcd.write('p'); break;
//        case 3: lcd.write('c'); break;
//        case 4: lcd.write('o'); break;
//        case 5: lcd.write('t'); break;
//        case 6: lcd.write('b'); break;
//      }
      lcd.write('*');
    }
    else lcd.write(45);
  }
}

void lcd_printPreset(){
  lcd.setCursor(0,0);
  lcd.write('P');
  lcd.write(cur_preset + 48);
}
