#include <sys/stat.h>
#include <exception>
#ifndef USER_INPUT_H
#define USER_INPUT_H

 byte INPUT_SERIAL_DEBUG = 1;

void updateUI(int val);

byte enc_switch_state = 0;
byte enc_switch_pushed = 0;
byte sw_state[4];
byte sw_pushed[4];
int enc_state = 0;
byte updateParams = 0;
uint32_t parameterStoreTimeout = 0;

typedef enum {
  EDIT_TEMPO, EDIT_LFO_RATE_A, EDIT_LFO_RATE_B, EDIT_CLOCK_DIV, EDIT_CHAN
} UI_STATES;
byte NUM_UI_STATES = 5;
UI_STATES ui_state = EDIT_LFO_RATE_A;

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
  updateUI(0);
}

void updateSw(byte num, byte state){
  sw_state[num] = state;
  updateUI(0);
}

//EDIT_TEMPO, EDIT_LFO_RATE_A, EDIT_LFO_RATE_B, EDIT_CLOCK_DIV, EDIT_CHAN
void updateUI(int val){
  switch( active_param ){
    case TEMPO:
      if(val != 0){ 
        tempo += val;
        updateParamValue( TEMPO, tempo);
        if( INPUT_SERIAL_DEBUG) Serial.println("TEMP0" + String(tempo));
      }
      break;

    case LFO_A_RATE:
      if(val != 0){ 
        dacs[0].inc += enc_state*16; 
        updateParamValue( LFO_A_RATE, dacs[0].inc / 16);
        if( INPUT_SERIAL_DEBUG) Serial.println("LFOA" + String(dacs[0].inc / 16));
      }
      break;

    case LFO_B_RATE:
      if(val != 0){ 
        dacs[1].inc += enc_state*16; 
        updateParamValue( LFO_B_RATE, dacs[1].inc / 16);
        if( INPUT_SERIAL_DEBUG) Serial.println("LFOB" + String(dacs[1].inc / 16));
      }
      break;

    case DIVISIONS:
      if(val != 0){
        CLOCK_DIVIDER_INDEX = (CLOCK_DIVIDER_INDEX + NUM_CLOCK_DIVISIONS + val) % NUM_CLOCK_DIVISIONS;
        clock_divider = CLOCK_DIVIDER_VALUES[ CLOCK_DIVIDER_INDEX ];
        if( INPUT_SERIAL_DEBUG) {
          Serial.println( String(CLOCK_DIVIDER_INDEX) + " " + 
            String(CLOCK_DIVIDER_VALUES[ CLOCK_DIVIDER_INDEX ]) + " " +
            CLOCK_DIVIDER_NAMES[ CLOCK_DIVIDER_INDEX] );
        }
        //clock_divider = static_cast<CLOCK_DIVISIONS>((clock_divider + val - Note64N) % (Note1N - Note64N + 1) + Note64N);
        if( INPUT_SERIAL_DEBUG) Serial.println(clock_divider);
        updateParamString( DIVISIONS, CLOCK_DIVIDER_NAMES[ CLOCK_DIVIDER_INDEX ] );
        if( INPUT_SERIAL_DEBUG) Serial.println("DIVS" + String(clock_divider));
      }
      break;
    case PHRASE_LENGTH:
      if(val != 0){
        cycle_length = cycle_length + val;
        cycle_length = cycle_length < 1 ? 1 : cycle_length > 32 ? 32 : cycle_length;
        updateParamValue( PHRASE_LENGTH, cycle_length);
        if( INPUT_SERIAL_DEBUG) Serial.println("length" + String(cycle_length));
      }
      break;
    case CHANNEL:
      if(val != 0){
        MIDI_CHANNEL = MIDI_CHANNEL + val;
        MIDI_CHANNEL = MIDI_CHANNEL < 0 ? 0 : MIDI_CHANNEL > 3 ? 3 : MIDI_CHANNEL;
        updateParamValue( CHANNEL, MIDI_CHANNEL);
        if( INPUT_SERIAL_DEBUG) Serial.println("chan" + String(MIDI_CHANNEL));
      }
      break;
  }

  // Serial.println("updateUI");
  // updateParams = 1;
  // parameterStoreTimeout = millis();
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
void updateMode(int next){
  // if(next < 0) currentMode = TEMPO;
  // currentMode = next;
}

void readEncoder(){
  static uint32_t ttimer = 0;
  static int couter = 0;
  couter++;

  // if(millis() - ttimer > 500) {
  //   ttimer = millis();
  //   Serial.println("timer " + String(couter));
  //   couter = 0;
  // }

  static byte enc_counter = 0;
  byte curB = enc.button(); //get current button state
  //four button states:
  // - 0 for button is being held down
  // - 1 for button transition from not pushed to pushed
  // - 2 for button is not being held down
  // - 3 for button transition from pushed to not pushed

  int val = enc.delta(); //get encoder  count
  if(val!= 0){
    enc_counter ++;
    if(enc_counter > 3){
      enc_counter = 0;
      enc_state = val;
      updateUI(val);
      if(INPUT_SERIAL_DEBUG){
        Serial.print("count: ");
        Serial.println(val);
      }
      else{
      // 
      }
    }
    val = 0;
  }

  switch(curB){
    case 0: //DOWN
    break;

    case 1: //PUSHED
    enc_switch_pushed = 1;
    updateParamName( active_param, 0);
    active_param = static_cast<PARAM_NAME>((active_param + 1) % (NUM_PARAM_NAMES));
    //ui_state = static_cast<UI_STATES>((ui_state + 1) % (NUM_UI_STATES));
    //ui_state = (ui_state + 1) % NUM_UI_STATES;
    Serial.println("PUSHED " + String(active_param));
    updateParamName( active_param, 1);
    display.display();
    //updateUI(0);
    if(INPUT_SERIAL_DEBUG) Serial.println("PUSHED");
    else{
      // comms.outu8(31);
      // comms.outu16(1);
      // comms.send();
    }
    break;

    case 2: //UP
   
    break;

    case 3: //RELEASED
    enc_switch_pushed = 0;
    if(INPUT_SERIAL_DEBUG) Serial.println("RELEASED");
    else{
      // comms.outu8(31);
      // comms.outu16(0);
      // comms.send();
    }
    break;
  }//switch
}

//optomized by chatGPT
void IRAM_ATTR ReadButton_ISR()
{	
	// Read input pin state using direct port manipulation
	byte state = (GPIO.in >> 19) & 0x01; // Assuming pin 19
	
  // DOWN = 0,
	// PUSHED = 1,
	// UP = 2,
	// RELEASED = 3,

	// Update play_button state based on input state
	switch (((play_button<2) << 1) | state) {
		case 0:  // DOWN + LOW
      play_button = DOWN; 
			break;
		case 3:  // UP + HIGH
			play_button = UP; 
			break;
		case 1:  // PUSHED (DOWN + HIGH)
			play_button = PUSHED;
      play_button_change = 1;
			break;
		default: // RELEASED (UP + LOW)
			play_button = RELEASED;
      play_button_change = 1;
	}
}

void readButton(){
  if( play_button_change == 1){
    play_button_change = 0;
    Serial.println("button " + String (play_button) );
    if(play_button == PUSHED ) play_button = DOWN;
    else if (play_button == RELEASED ) play_button = UP;
  }
  // Serial.println( String(play_button ) + " " + String( digitalRead(19)) );
  // if(play_button == PUSHED ) play_button = DOWN;
  // else if (play_button == RELEASED ) play_button = UP;
  // delay(250);
}

#endif