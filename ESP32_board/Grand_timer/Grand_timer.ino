#include <esp32-hal-timer.h>
hw_timer_t * timerPPQ = NULL; // Define a pointer to the timer
hw_timer_t * timerQN = NULL; // Define a pointer to the timer

volatile byte timerFlag = 0;
volatile byte timerFlagPPQ = 0;
volatile byte PPQcounter = 0;

int PPQ_timer[24];
int tempo = 120;
int beat_period = 1/tempo;

const byte CLOCK = 16;
const byte MIDI_OUT = 17;
const byte DAC_0 = 25;
const byte DAC_1 = 26;

void IRAM_ATTR onTimerQN() {
  timerFlag=1;
  // Your interrupt service routine code here
}

void setup() {
  Serial.begin(115200);

  pinMode(CLOCK, OUTPUT);
  pinMode(DAC_0, OUTPUT);
  pinMode(DAC_1, OUTPUT);
  pinMode(MIDI_OUT, OUTPUT);
  
  // Set up the hardware timer
  setupTimerQN(timerQN, tempo, 1);
  setTempo(120);
}

void loop() {
  // Your main loop code here
  static uint32_t counter = 0;

  if(timerFlag){
    timerFlag = 0;
    Serial.print(micros()-counter);
    counter = micros();
    Serial.println(" printing in periodic function.");
    digitalWrite( CLOCK, !digitalRead(CLOCK));
    digitalWrite( DAC_0, !digitalRead(DAC_0));
    digitalWrite( DAC_1, !digitalRead(DAC_1));
  }
}

void setupTimerQN(hw_timer_t * _timer, int bpm, int div){
  // Set up the hardware timer
  uint32_t interval = 60000/bpm;
  interval = (interval * 1000) / div;
  Serial.println("timer " + String( bpm ) + " " + String(interval));
  _timer = timerBegin(0, 800, true); // Timer 0, 80 prescaler, count up
  timerAttachInterrupt(_timer, &onTimerQN, true); // Attach the interrupt
  timerAlarmWrite(_timer, interval/10, true); // Set alarm to trigger every 1000 microseconds
  timerAlarmEnable(_timer); // Enable the alarm
}

void setTempo(int val){
  tempo = val;
  beat_period = 60000/tempo;
  for(int i=0;i<24;i++) {
    PPQ_timer[i] = millis() + i*beat_period/24;
    Serial.println(PPQ_timer[i] - millis());
  }
}
