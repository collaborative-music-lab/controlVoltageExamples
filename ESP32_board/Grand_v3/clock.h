/*********************************************
CLOCK.h

*********************************************/

void sendClock(uint32_t num);
void triggerClock();
void triggerReset();

void sendClock(uint32_t num){
  //num is 0 for internal clock, the current beat number for external clock
  last_clock_receive_time = millis();
  //Serial.println(String(tempo_period)+ " tempo " + String(tempo) + " " + String(tempo_divisions));
  // tempo_period = 60000/(tempo*tempo_divisions);
  tempo_period = 60000/(tempo*4);

  if( num == 0 ){ //internal clock fired
    clock_counter++;
  } else { clock_counter = num; }


  if( clock_counter % clock_divider == 0) triggerClock();
  //if( (clock_counter % 8) == 0) triggerReset();
  if ( SERIAL_DEBUG) {
    Serial.print ("count " + String(clock_counter) + " ");
    Serial.print ("source " + String(num) + " ");
    Serial.print ("tempo " + String(tempo) + " ");
    Serial.print ("period " + String(tempo_period) + " ");
    Serial.println ("length " + String(cycle_length) + " ");
  }
}

void triggerClock(){
  led_onset = millis();
  led_state = 1;
  led_blink_time = tempo_period/2;
  //digitalWrite( led_pin, LOW);
  digitalWrite( CLOCK, LOW);
  drawBeat((clock_counter / clock_divider) % cycle_length);
  if(cycle_length > 0){
    if( clock_counter % (cycle_length*4) == 0) triggerReset();
  }
}

void triggerReset(){
  led_onset = millis();
  led_state = 1;
  led_blink_time = tempo_period/2;
  //digitalWrite( reset_out, HIGH);
  
}

//untested withpoint to function
void setupTimer(hw_timer_t * _timer, void (*func)(), int bpm, int div){
  // Set up the hardware timer
  uint32_t interval = 60000/bpm;
  interval = (interval * 1000) / div;
  Serial.println("timer " + String( bpm ) + " " + String(interval));
  _timer = timerBegin(0, 800, true); // Timer 0, 80 prescaler, count up
  timerAttachInterrupt(_timer, func, true); // Attach the interrupt
  timerAlarmWrite(_timer, interval/10, true); // Set alarm to trigger every 1000 microseconds
  timerAlarmEnable(_timer); // Enable the alarm
}

//TIMER ISRS
volatile byte mainClockFlag = 0;
void IRAM_ATTR onMainClock() {
  mainClockFlag=1;
  digitalWrite(CLOCK, HIGH);
  // Your interrupt service routine code here
}

volatile byte mainClockReleaseFlag = 0;
void IRAM_ATTR onMainClockRelease() {
  mainClockFlag=1;
  digitalWrite(CLOCK, LOW);
  // Your interrupt service routine code here
}

volatile byte swingClockFlag = 0;
void IRAM_ATTR onSwingClock() {
  swingClockFlag=1;
  digitalWrite(CLOCK, HIGH);
  // Your interrupt service routine code here
}

volatile byte swingClockReleaseFlag = 0;
void IRAM_ATTR onSwingClockRelease() {
  swingClockReleaseFlag=1;
  digitalWrite(CLOCK, LOW);
  // Your interrupt service routine code here
}