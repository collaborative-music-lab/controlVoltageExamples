//https://github.com/atomic14/ESPNowSimpleSample/blob/master/src/main.cpp
#include <WiFi.h>
#include <esp_now.h>

byte IS_CLOCK_SOURCE = 1;

//pin definitions
byte clock_out = 27;
byte reset_out = 32;
byte midi_out = 17;
byte midi_in = 16;


uint32_t led_onset = 0;
int led_blink_time = 50;
byte led_state = 0;
byte led_pin = 2;
uint32_t clock_counter = 0;
int tempo = 100;
uint16_t tempo_period = 30000/tempo;
uint32_t last_clock_receive_time = 0;
uint32_t metro_timer = 1000;
uint16_t cycle_length = 12;

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
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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

void setup()
{
  pinMode( led_pin, OUTPUT);
  pinMode( clock_out, OUTPUT);
  pinMode( reset_out, OUTPUT);
  pinMode( midi_out, OUTPUT);

  Serial.begin(115200);
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
}

void loop()
{
  static uint32_t timer = 0;
  int interval = 120;

  if( IS_CLOCK_SOURCE ){
    if(millis()-timer > tempo_period){
      timer = millis();
      broadcast(clock_counter++);
    }
  }

  else{
    if(millis()-last_clock_receive_time > tempo_period+1){
      sendClock(0);
    }
  }

  checkLed();
}

void triggerClock(){
  led_onset = millis();
  led_state = 1;
  led_blink_time = tempo_period/2;
  digitalWrite( led_pin, LOW);
  digitalWrite( clock_out, HIGH);
}

void triggerReset(){
  led_onset = millis();
  led_state = 1;
  led_blink_time = tempo_period/2;
  digitalWrite( reset_out, HIGH);
}

void checkLed(){
  if(led_state == 1){
    if(millis() - last_clock_receive_time > led_blink_time){
      led_state = 0;
      digitalWrite( led_pin, HIGH);
      digitalWrite( clock_out, LOW);
      digitalWrite( reset_out, LOW);
    }
  }
}

void sendClock(uint32_t num){
  //num is 0 for internal clock, the current beat number for external clock
  last_clock_receive_time = millis();

  if( num == 0 ){ //internal clock fired
    clock_counter++;
  } else { clock_counter = num; }

  triggerClock();
  if( (clock_counter % 8) == 0) triggerReset();
 
  Serial.print ("count " + String(clock_counter) + " ");
  Serial.print ("source " + String(num) + " ");
  Serial.print ("tempo " + String(tempo) + " ");
  Serial.print ("period " + String(tempo_period) + " ");
  Serial.println( millis() - last_clock_receive_time);
}