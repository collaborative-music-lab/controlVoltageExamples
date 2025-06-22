/*********************************************
ESP_NOW

receiveCallback() runs on receiving an espnow packet
sentCallback() runs after a espnow msg is sent
broadcast() sends a msg to all espnow devices on the network
*********************************************/

void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength)
{
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

void receiveCallback(const esp_now_recv_info *recvInfo, const uint8_t *data, int len) {
  
  // Serial.print("Received data as integers: ");
  // for (int i = 0; i < dataLen; i++) {
  //   Serial.print(data[i]);
  //   Serial.print(" ");
  // }
  //Serial.println();

  tempo = data[0];
  tempo_period = 60000/(tempo*tempo_divisions);
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

void broadcast(uint16_t count) {
  // Define the broadcast MAC address
  uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  // Set up the peer information structure
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));       // Clear the structure
  memcpy(peerInfo.peer_addr, broadcastAddress, 6); // Copy the MAC address
  
  // New in v3.2: You must explicitly set the channel, interface, and encryption status.
  peerInfo.channel = 0;               // Use the default Wi‑Fi channel (or set it to your channel)
  peerInfo.ifidx   = WIFI_IF_STA;       // Use the station interface (WIFI_IF_STA instead of ESP_IF_WIFI_STA)
  peerInfo.encrypt = false;           // No encryption by default

  // Check if the peer already exists – note: the function call is still similar
  if (!esp_now_is_peer_exist(broadcastAddress)) {
    esp_err_t addResult = esp_now_add_peer(&peerInfo);
    if (addResult != ESP_OK) {
      Serial.print("Failed to add peer: ");
      Serial.println(addResult);
      return;
    }
  }

  // Prepare the data to send.
  // Extract the high and low bytes from the count value.
  uint8_t highByte = (count >> 8) & 0xFF;
  uint8_t lowByte  = count & 0xFF;
  uint8_t dataArray[8] = { tempo, highByte, lowByte, cycle_length, 0, 0, 0, 0 };

  // Send the data over ESP‑NOW
  esp_err_t result = esp_now_send(broadcastAddress, dataArray, sizeof(dataArray));

  // Provide simple debugging feedback
  if (result == ESP_OK) {
    if (SERIAL_DEBUG) Serial.println("Broadcast message success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    Serial.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Unknown error");
  }
}
