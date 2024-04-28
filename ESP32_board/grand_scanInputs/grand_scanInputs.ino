const int numPins = 36; // Number of digital pins on ESP32
const int flashPins[] = {1,3,6, 7, 8, 9,10,11}; //pins to not scan

void setup() {
  Serial.begin(115200);
  
  // Initialize all digital pins as inputs, except TX, RX, and flash pins
  for (int pin = 0; pin < numPins; pin++) {
    if (pin != 1 && pin != 3 && !isFlashPin(pin)) {
      pinMode(pin, INPUT);
    }
  }
}

void loop() {
  static int previousState[numPins]; // Array to store previous state of each pin

  // Check each pin for state change
  for (int pin = 0; pin < numPins; pin++) {
    if (pin != 1 && pin != 3 && !isFlashPin(pin)) {
      int currentState = digitalRead(pin);
      
      // If state changed, print pin number
      if (currentState != previousState[pin]) {
        Serial.print("Pin ");
        Serial.print(pin);
        Serial.print(" changed state to ");
        Serial.println(currentState);
        previousState[pin] = currentState;
      }
    }
  }
  
  delay(10); // Adjust delay as needed to debounce inputs and reduce CPU usage
}

bool isFlashPin(int pin) {
  // Check if pin is connected to flash
  for (int i = 0; i < sizeof(flashPins)/sizeof(flashPins[0]); i++) {
    if (pin == flashPins[i]) {
      return true;
    }
  }
  return false;
}
