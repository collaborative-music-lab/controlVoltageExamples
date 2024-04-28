 /* Grand Central
 * 
 * 
 */
byte SERIAL_DEBUG = 0;
byte IS_CLOCK_SOURCE = 1;

#include "grand.h"

/*********************************************
MAIN
*********************************************/

void setup() {
  Serial.begin(115200);

  //initialize inputs
  pinMode( led_pin, OUTPUT);
  pinMode( CLOCK, OUTPUT);
  pinMode( MIDI_OUT, OUTPUT);
  pinMode( DAC_0, OUTPUT);
  pinMode( DAC_1, OUTPUT);
  pinMode(19, INPUT); //button
  enc.begin([]{enc.readEncoder_ISR();});
  attachInterrupt(digitalPinToInterrupt(19), ReadButton_ISR, CHANGE);

  delay(100);
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  // Output my MAC address - useful for later
  Serial.print("My MAC Address is: ");
  Serial.println(WiFi.macAddress());
  // shut down wifi
  WiFi.disconnect();
  // startup ESP Now
  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESPNow Init Success");
    esp_now_register_recv_cb(receiveCallback);
    esp_now_register_send_cb(sentCallback);
  }
  else
  {
    Serial.println("ESPNow Init Failed");
    delay(3000);
    ESP.restart();
  }

  preferences.begin("my-app", false); // Open preferences with the app name, read-only

  // Store data
  // preferences.putString("name", "John Doe");
  // preferences.putUInt("age", 30);
  // preferences.putFloat("temperature", 25.5);
  tempo = preferences.getInt("tempo", 110);
  tempo_period = 15000/tempo;
  readArray("storedTempo", storedTempo, sizeof(storedTempo) / sizeof(storedTempo[0]));
  cycle_length = preferences.getInt("cycle_length", 16);
  readArray("storedCycleLength", storedCycleLength, sizeof(storedCycleLength) / sizeof(storedCycleLength[0]));

  setupGraphics();
  drawTempo(tempo);
  drawCycleLength(cycle_length);
  updateParamNames();
  drawChannel(0);

  Serial.println("Setup complete");
}

void loop() {
  //readSw();
  readEncoder();
  readButton();
  

  static uint32_t timer = 0;
  int interval = 120;

  if( IS_CLOCK_SOURCE ){
    if(millis()-timer > tempo_period){
      timer = millis();
      broadcast(clock_counter++);
      sendClock(clock_counter);
      //drawBeat(clock_counter / tempo_divisions % cycle_length);
      processDac();
    }
  }
  else{
    if(millis()-last_clock_receive_time > tempo_period+1){
      sendClock(0);
      drawBeat(clock_counter % cycle_length);
    }
  }

  static uint32_t PPQ_timer = 0;
  if( micros() - PPQ_timer > PPQ_period_micros){
    PPQ_timer = micros();
    PPQ_counter = PPQ_counter < 23 ? PPQ_counter + 1 : 23;
  }


  checkLed();
  storeParams();
}

void checkLed(){
  if(led_state == 1){
    if(millis() - last_clock_receive_time > led_blink_time){
      led_state = 0;
      digitalWrite( led_pin, HIGH);
      digitalWrite( CLOCK, HIGH);
      // digitalWrite( clock_out, LOW);
      // digitalWrite( reset_out, LOW);
    }
  }
}

