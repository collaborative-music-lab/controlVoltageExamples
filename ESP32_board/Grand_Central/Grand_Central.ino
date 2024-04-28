/* 
DMX Wifi Server 2

Implements ESP-NOW
 */

#include <WiFi.h>
#include <esp_now.h>

const byte NUM_CLIENTS = 8;

uint8_t client[8][6] = {
  {0x84, 0xF3, 0xEB, 0x1C, 0x39, 0x9C},
  {0xBC, 0xDD, 0xC2, 0x0D, 0xA2, 0xA1},
  {0xBC, 0xFF, 0x4D, 0x24, 0x7D, 0x17},
  {0x94, 0xB9, 0x7E, 0x17, 0x48, 0x5D},

  {0xBC, 0xFF, 0x4D, 0x24, 0xF4, 0xB8},
  {0xBC, 0xFF, 0x4D, 0x24, 0xD8, 0x3F},
  {0x84, 0xF3, 0xEB, 0x59, 0xFD, 0xCD},
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};


//data to receive via esp now
typedef struct struct_message{
  byte smoothing;
  byte length;
  byte val[60];
} struct_message;

struct_message data;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0) return;
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}




void setup() {
  
  Serial.begin(115200);
  Serial.println();
  delay(1000);
  
  Serial.print("Configuring access point...");
    // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(client[0], ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {

  static uint32_t timer = 0;
  int interval = 50;
  static byte valR = 0;
  static byte valG = 20;
  static byte valB = 100;
  static byte val[] = {0,100,200,20,30,50};
  static byte dir[] = {0,0,0,1,1,1};
  static byte inc[] = {1,3,2,8,2,15};
  static byte channel = 1;


  if(millis()-timer>interval){
    timer=millis();

    for(byte i=0;i<6;i++){
      val[i] = dir[i]==0 ? val[i]-inc[i] : val[i]+inc[i];
      if(val[i]>250) dir[i]=0;
      else if(val[i]<5) dir[i]=1;
      Serial.print( String(val[i]) + " ");
    }

    //Udp.beginPacket(BroadcastIP, BroadcastPort); // subnet Broadcast IP and port
    // Udp.beginPacket(RemoteIP, BroadcastPort); // subnet Broadcast IP and port
    
    // data.val(11); //RGB mode?
    data.smoothing = 250;
    byte len = 0;

    for(int i=0;i<4;i++){
      data.val[i*3] = val[0];
      data.val[i*3+1] = val[1];
      data.val[i*3+2] = val[2];
      len+=3;
    }
    for(int i=0;i<4;i++){
      data.val[i*3 + 12] = val[4];
      data.val[i*3+1 + 12] = val[5];
      data.val[i*3+2 + 12] = val[6];
      len+=3;
    }
    data.length = len;

    // Udp.endPacket();

    // Send message via ESP-NOW
    for(int i=5;i<NUM_CLIENTS;i++) esp_now_send(client[i], (uint8_t *) &data, sizeof(data));
    
    Serial.println("broadcasting");
  }
  channel = channel>6 ? 0 : channel+1;
}
