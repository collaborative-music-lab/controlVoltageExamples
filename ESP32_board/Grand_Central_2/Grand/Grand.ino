 /* Simple test for analog and digital inputs, and digital outputs
 * 
 */
//#include "m370_lbr.h"
#include "encoder.h"
//https://github.com/atomic14/ESPNowSimpleSample/blob/master/src/main.cpp
#include <WiFi.h>
#include <esp_now.h>
#include <Preferences.h>
Preferences preferences;
#include "graphics.h"
#include "userInput.h"
#include <Wire.h>


 byte SERIAL_DEBUG = 1;
 byte IS_CLOCK_SOURCE = 1;

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
DIGITAL INPUT SETUP
*********************************************/
const byte NUM_DIGITAL = 1;

// m370_digitalInput sw[NUM_DIGITAL] = {
//   m370_digitalInput(19,500),//pin, rate(Hz)
// };


/*********************************************
CLOCK SETUP
*********************************************/

//pin definitions
// byte clock_out = pMISO;
// byte reset_out = pCLK;
// byte midi_out = 17;
// byte midi_in = 16;


uint32_t led_onset = 0;
int led_blink_time = 50;
byte led_state = 0;
byte led_pin = 2;
uint32_t clock_counter = 0;

const byte CLOCK = 16;
const byte MIDI_OUT = 17;
const byte DAC_0 = 25;
const byte DAC_1 = 26;

/*********************************************
ESP_NOW
*********************************************/
void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen){
  
  // Serial.print("Received data as integers: ");
  // for (int i = 0; i < dataLen; i++) {
  //   Serial.print(data[i]);
  //   Serial.print(" ");
  // }
  //Serial.println();

  tempo = data[0];
  tempo_period = 30000 / tempo;
  clock_counter = (data[1]<<8) + data[2];
  cycle_length = data[3];
  
  sendClock(clock_counter);
}

// callback when data is sent
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status)
{
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  // Serial.print("Last Packet Sent to: ");
  // Serial.println(macStr);
  // Serial.print("Last Packet Send Status: ");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void broadcast(uint16_t count)
{
  // this will broadcast a message to everyone in range
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
  if (!esp_now_is_peer_exist(broadcastAddress))
  {
    esp_now_add_peer(&peerInfo);
  }

  uint8_t lowByte = count & 0xFF;

  // Extract high byte (most significant byte)
  uint8_t highByte = (count >> 8) & 0xFF;

   // Populate the array with sample data
  byte dataArray[8] = {tempo, highByte, lowByte, cycle_length, 0,0,0,0};

  esp_err_t result = esp_now_send(broadcastAddress, dataArray, sizeof(dataArray));
  // and this will send a message to a specific device
  /*uint8_t peerAddress[] = {0x3C, 0x71, 0xBF, 0x47, 0xA5, 0xC0};
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, peerAddress, 6);
  if (!esp_now_is_peer_exist(peerAddress))
  {
    esp_now_add_peer(&peerInfo);
  }
  esp_err_t result = esp_now_send(peerAddress, (const uint8_t *)message.c_str(), message.length());*/
  if (result == ESP_OK)
  {
    Serial.println("Broadcast message success");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_INIT)
  {
    Serial.println("ESPNOW not Init.");
  }
  else if (result == ESP_ERR_ESPNOW_ARG)
  {
    Serial.println("Invalid Argument");
  }
  else if (result == ESP_ERR_ESPNOW_INTERNAL)
  {
    Serial.println("Internal Error");
  }
  else if (result == ESP_ERR_ESPNOW_NO_MEM)
  {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
  {
    Serial.println("Peer not found.");
  }
  else
  {
    Serial.println("Unknown error");
  }
}


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
  //for( int i=0;i<NUM_DIGITAL;i++) sw[i].begin();
  enc.begin([]{enc.readEncoder_ISR();});

  delay(1000);
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESPNow Example");
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
  

  static uint32_t timer = 0;
  int interval = 120;

  if( IS_CLOCK_SOURCE ){
    if(millis()-timer > tempo_period){
      timer = millis();
      //broadcast(clock_counter++);
      //sendClock(clock_counter);
      drawBeat(clock_counter);
    }
  }
  else{
    if(millis()-last_clock_receive_time > tempo_period+1){
      sendClock(0);
      drawBeat(clock_counter);
    }
  }

  checkLed();
  storeParams();
}

// void readSw(){
//   static int count[4];
  
//   for(int i=0;i<NUM_DIGITAL;i++){
//     sw[i].loop();
//     if( sw[i].available() ){
//       byte val = sw[i].getState();
//       if( val != sw_state[i]){
//         sw_state[i] = val;
//         sw_pushed[i] = val;
//         updateUI();
//       }
//       if( SERIAL_DEBUG ) {
//         PrintDebug("sw",i,val);
//       }
//       else {
//         // comms.outu8(i+10);
//         // comms.outu16(outVal);
//         // comms.send();
//       }
//     }
//   }
// }

void readEncoder(){
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
      updateUI();
      if(SERIAL_DEBUG){
        Serial.print("count: ");
        Serial.println(val);
      }
      else{
      // 
      }
    }
  }

  switch(curB){
    case 0: //DOWN
    break;

    case 1: //PUSHED
    enc_switch_pushed = 1;
    //updateUI();
    if(SERIAL_DEBUG) Serial.println("PUSHED");
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
    if(SERIAL_DEBUG) Serial.println("RELEASED");
    else{
      // comms.outu8(31);
      // comms.outu16(0);
      // comms.send();
    }
    break;
  }//switch
}

void PrintDebug(String name, int num, int val){
  Serial.print(name);
  Serial.print(" ");
  Serial.print(num);
  Serial.print(": ");
  Serial.println(val);
}

void triggerClock(){
  led_onset = millis();
  led_state = 1;
  led_blink_time = tempo_period/2;
  // digitalWrite( led_pin, LOW);
  // digitalWrite( clock_out, HIGH);
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

void checkLed(){
  if(led_state == 1){
    if(millis() - last_clock_receive_time > led_blink_time){
      led_state = 0;
      digitalWrite( led_pin, HIGH);
      // digitalWrite( clock_out, LOW);
      // digitalWrite( reset_out, LOW);
    }
  }
}

void sendClock(uint32_t num){
  //num is 0 for internal clock, the current beat number for external clock
  last_clock_receive_time = millis();
  tempo_period = 15000/tempo;

  if( num == 0 ){ //internal clock fired
    clock_counter++;
  } else { clock_counter = num; }


  if( clock_counter % clock_divider == 0) triggerClock();
  //if( (clock_counter % 8) == 0) triggerReset();
 
  Serial.print ("count " + String(clock_counter) + " ");
  Serial.print ("source " + String(num) + " ");
  Serial.print ("tempo " + String(tempo) + " ");
  Serial.print ("period " + String(tempo_period) + " ");
  Serial.println ("length " + String(cycle_length) + " ");
}
