void lcd_show_index(byte chan, byte num){
  lcd.setCursor(num,chan);
  if(cur_chan >> chan & 1 ) lcd.write('#');
  else  lcd.write('_');
  //Serial.println("index " + String(num) + " " + String(chan));
}

void lcdShowActiveStep( byte chan, byte num){
  lcd.setCursor(num, chan);
  if( seq[chan].getAux(0,num) > 0 ) lcd.write('*');
  else lcd.write('-');
}

void lcd_update_enable(byte chan, byte num){
  lcd.setCursor(num,chan);
  if( seq[chan].getAux(0,num) > 0 ) lcd.write('*');
  else lcd.write('-');
}
