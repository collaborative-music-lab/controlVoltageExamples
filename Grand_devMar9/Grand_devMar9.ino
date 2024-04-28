#include <Ticker.h>

const byte CLOCK = 16;
const byte MIDI_OUT = 17;
const byte DAC_0 = 25;
const byte DAC_1 = 26;

const int PPQ = 24;  // Pulses per quarter note

volatile unsigned long ticks = 0; // Count of ticks
volatile unsigned long tickInterval = 60000000 / (120 * PPQ); // Initial tick interval (for 120 BPM)
volatile bool tickState = false; // State of the tick
volatile byte timer_flag = 0;

Ticker timer;

void onTick() {
  ticks++; // Increment ticks count
  tickState = !tickState; // Toggle tick state

  // Output tick state (e.g., to trigger external device)
  digitalWrite(CLOCK, tickState);
  timer_flag = 1;
}

void setup() {
  Serial.begin(115200);

  pinMode(CLOCK, OUTPUT);
  pinMode(DAC_0, OUTPUT);
  pinMode(DAC_1, OUTPUT);
  pinMode(MIDI_OUT, OUTPUT);

  // Attach interrupt to the ticker
  timer.attach(tickInterval / 1000, onTick); // Convert microseconds to milliseconds
}

void loop() {
    noInterrupts(); // Disable interrupts to ensure atomic access
  if (timer_flag) {
    timer_flag = 0;
    interrupts(); // Re-enable interrupts after accessing/modifying timer_flag
    Serial.println("Timer triggered");
  } else {
    interrupts(); // Re-enable interrupts if timer_flag is not set
  }
  // Example: Change BPM dynamically (adjust every 5 seconds)
  static unsigned long lastChangeTime = 0;
  if (millis() - lastChangeTime >= 5000) {
    // Calculate new tick interval based on BPM
    int bpm = 100 + random(20); // Random BPM between 100 and 120 for demonstration
    tickInterval = 60000000 / (bpm * PPQ); // Calculate tick interval based on BPM
    timer.attach(tickInterval / 1000, onTick); // Update ticker interval
    lastChangeTime = millis();
    Serial.print("New BPM: " + String(tickInterval / 1000) + " ");
    Serial.println(bpm);
  }

  // Add your application logic here
}
