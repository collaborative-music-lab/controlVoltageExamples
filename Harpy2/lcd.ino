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

void lcd_string(char text[], byte posX, byte posY){
  lcd.setCursor(posX, posY);
  for(int i=0;i<strlen(text);i++){
    lcd.write(text[i]);
  }
}

void lcd_printMIDI(byte val){
  lcd.write(  val/100 + 48 );
  lcd.write(  val/10 % 10 + 48);
  lcd.write(  val% 10 + 48);
}