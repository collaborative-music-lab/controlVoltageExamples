 /* Simple test for analog and digital inputs, and digital outputs
 * 
 */
//#include "m370_lbr.h"
#include "encoder.h"
//https://github.com/atomic14/ESPNowSimpleSample/blob/master/src/main.cpp
#include <Wire.h>

/*********************************************
ENCODERS SETUP 
*********************************************/
//encoders rely on the  ESP32Encoder library
//Esp32Encoder rotaryEncoder = Esp32Encoder(18,2,4);//A,B,Button
//optional divider argument
//arguments:
// - A and B: digital inputs from encoder
// - Switch; pin for encoder switch, or -1 for no switch
// - divider: many encoders put out multiple pulses per detent
//   The divider helps to make encoder increments match detents

encoder enc(32,33,23,2);//A,B,Switch, Divider


/*********************************************
MAIN
*********************************************/

void setup() {
  Serial.begin(115200);


  //for( int i=0;i<NUM_DIGITAL;i++) sw[i].begin();
  enc.begin([]{enc.readEncoder_ISR();});


  Serial.println("Setup complete");
}

void loop() {
  //readSw();
 // readEncoder();
  

}


void PrintDebug(String name, int num, int val){
  Serial.print(name);
  Serial.print(" ");
  Serial.print(num);
  Serial.print(": ");
  Serial.println(val);
}

