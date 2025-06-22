/* grand.h

global variables
imports/includes
timer setup / callbacks

 */

 /*********************************************
global variables
*********************************************/

uint32_t led_onset = 0;
int led_blink_time = 50;
byte led_state = 0;
byte led_pin = 2;
uint32_t clock_counter = 0;

const byte CLOCK = 16;
const byte MIDI_OUT = 17;
const byte DAC_0 = 25;
const byte DAC_1 = 26;

int16_t tempo = 100; //in bpm
int16_t storedTempo[4];
byte tempo_divisions = 24;
uint16_t beat_period_ms = 60000/tempo; //in milliseconds
uint32_t PPQ_period_micros = (60000*1000)/(tempo*24);
uint8_t PPQ_counter = 0;

uint32_t tempo_period = 60000/(tempo*tempo_divisions); //in micros, assuming 24 PPQ
uint32_t last_clock_receive_time = 0;
uint32_t metro_timer = 1000;
int16_t cycle_length = 12;
int16_t storedCycleLength[4];

//setup clock divider variable as enum;
byte clock_divider = 24;
byte NUM_CLOCK_DIVISIONS = 12;

byte CLOCK_DIVIDER_VALUES[] = { 96, 72, 48, 24, 16, 12, 8,6,4,3,2,1 };
byte CLOCK_DIVIDER_INDEX = 0;

const char* CLOCK_DIVIDER_NAMES[] = {
    "1N", "2D", "2N", "4N", "4T", "8N", "8T", "16N", "16T", "32N", "32T", "64N"
};

//PARAAMETER NAMES AND VALUES
enum PARAM_NAME {
  TEMPO, DIVISIONS, PHRASE_LENGTH, CHANNEL, LFO_A_RATE, LFO_B_RATE
};
PARAM_NAME active_param = TEMPO;
byte NUM_PARAM_NAMES = 6;

byte MIDI_CHANNEL = 1;

typedef enum {
	DOWN = 0,
	PUSHED = 1,
	UP = 2,
	RELEASED = 3,
} BUTTON_STATE;

volatile BUTTON_STATE play_button = UP;
volatile byte play_button_change = 0;

// /*********************************************
// INCLUDES
// *********************************************/
// // Create an instance for the MIDI output UART (using UART1).
// // Pin 5 will be used for TX. We are not using an RX, so set it to -1.
// HardwareSerial MidiSerial(1);

// #include <esp32-hal-timer.h>
// //can't use Ticker.h because of need for microsecond precision
// hw_timer_t * mainClock = NULL; // Define a pointer to the timer
// hw_timer_t * mainClockRelease = NULL; // Define a pointer to the timer
// hw_timer_t * swingClock = NULL; // Define a pointer to the timer
// hw_timer_t * swingClockRelease = NULL; // Define a pointer to the timer


#include "soc/gpio_reg.h"
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/soc.h"
#include "soc/periph_defs.h"

// Fast GPIO read for ESP32 (works for GPIO 0–39)
inline __attribute__((always_inline)) uint8_t fastDigitalRead(uint8_t pin) {
  if (pin < 32) {
    return (READ_PERI_REG(GPIO_IN_REG) >> pin) & 0x01;
  } else if (pin < 40) {
    return (READ_PERI_REG(GPIO_IN1_REG) >> (pin - 32)) & 0x01;
  } else {
    return 0; // invalid pin
  }
}
