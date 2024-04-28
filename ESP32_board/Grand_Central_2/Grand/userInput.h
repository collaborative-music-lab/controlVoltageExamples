#include <exception>
#ifndef USER_INPUT_H
#define USER_INPUT_H

 byte INPUT_SERIAL_DEBUG = 1;

void updateUI();

int16_t tempo = 100;
int16_t storedTempo[4];
uint16_t tempo_period = 15000/tempo;
uint32_t last_clock_receive_time = 0;
uint32_t metro_timer = 1000;
int16_t cycle_length = 12;
int16_t storedCycleLength[4];
byte clock_divider = 2;

byte enc_switch_state = 0;
byte enc_switch_pushed = 0;
byte sw_state[4];
byte sw_pushed[4];
int enc_state = 0;
byte updateParams = 0;
uint32_t parameterStoreTimeout = 0;

// Function to save an array to preferences
void saveArray(const char* key, const int16_t* array, size_t size) {
  preferences.putBytes(key, reinterpret_cast<const uint8_t*>(array), size * sizeof(int16_t));
}

//prototypes if necessary
int16_t updateTempo(int16_t cur_tempo, int val);
int16_t updateCycleLength(int16_t cur, int val);

// Function to read an array from preferences
void readArray(const char* key, int16_t* array, size_t size) {
  size_t length = preferences.getBytesLength(key);
  if (length == size * sizeof(uint16_t)) {
    preferences.getBytes(key, reinterpret_cast<int8_t*>(array), length);
  } else {
    // Handle error: Incorrect array length retrieved from preferences
    Serial.println("Error: Incorrect array length retrieved from preferences");
  }
}

void updateEncSw(byte state){
  enc_switch_state = state;
  updateUI();
}

void updateSw(byte num, byte state){
  sw_state[num] = state;
  updateUI();
}

void updateUI(){
  if( enc_state != 0 && !enc_switch_pushed ){
    tempo = updateTempo(tempo, enc_state * (tempo/60 + 1));
    if( INPUT_SERIAL_DEBUG) Serial.println("tempo " + String(tempo) + " " + String(enc_state * (tempo/60 + 1)));
    enc_state = 0;
  } else if( enc_state != 0 && enc_switch_pushed){
    enc_state = enc_state * (cycle_length/8 + 1);
    cycle_length = cycle_length + enc_state;
    if( cycle_length > 32) cycle_length = (cycle_length/4) * 4;
    else if( cycle_length > 16) cycle_length = (cycle_length/2) * 2;
    else if( cycle_length > 8) cycle_length = (cycle_length/2) * 2;
    
    cycle_length = cycle_length < 0 ? 0 : cycle_length > 128 ? 128 : cycle_length;
    updateCycleLength(cycle_length, 0);
    if( INPUT_SERIAL_DEBUG) Serial.println("cycle length " + String(cycle_length));
    enc_state = 0;
  }

  for( int i=0;i<4;i++){
    if(sw_pushed[i] && enc_switch_pushed){
      storedTempo[i] = tempo;
      storedCycleLength[i] = cycle_length;
      if( INPUT_SERIAL_DEBUG) Serial.println("stored " + String(i) + " " + String(storedTempo[i]));
    } else if (sw_pushed[i]){
      tempo = updateTempo(storedTempo[i], 0 );
      cycle_length = storedCycleLength[i];
      if( INPUT_SERIAL_DEBUG) Serial.println("recalled " + String(i) + " " + String(storedTempo[i]));
    }
    sw_pushed[i] = 0;
  }
  Serial.println("updateUI");
  updateParams = 1;
  parameterStoreTimeout = millis();
}

int16_t updateTempo(int16_t cur_tempo, int val){
  cur_tempo = cur_tempo + val;
  drawTempo((int)cur_tempo);
  return cur_tempo;
}

int16_t updateCycleLength(int16_t cur, int val){
  cur = cur + val;
  drawCycleLength((int)cur);
  return cur;
}

void storeParams(){
  if(updateParams && millis() - parameterStoreTimeout > 10000){
    updateParams = 0;

    preferences.putInt("tempo", tempo);
    preferences.putInt("cycle_length", cycle_length);
    saveArray("storedTempo", storedTempo, sizeof(storedTempo) / sizeof(storedTempo[0]));
    saveArray("storedCycleLength", storedCycleLength, sizeof(storedCycleLength) / sizeof(storedCycleLength[0]));

    if( INPUT_SERIAL_DEBUG) Serial.println("stored params");
  }
}

//need to clear prev mode, and set new mode
void updateMode(Mode next){
  if(next < 0) currentMode = TEMPO;
  currentMode = next;
}

#endif