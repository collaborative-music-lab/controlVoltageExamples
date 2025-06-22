#include "driver/gpio.h"

void IRAM_ATTR ReadButton_ISR();

void encoderSetup(){
  enc.begin([]{enc.readEncoder_ISR();});
  attachInterrupt(digitalPinToInterrupt(19), ReadButton_ISR, CHANGE);
}

//optomized by chatGPT
void IRAM_ATTR ReadButton_ISR()
{	
	// Read input pin state using direct port manipulation
	byte state = fastDigitalRead(19); // Assuming pin 19
	
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
