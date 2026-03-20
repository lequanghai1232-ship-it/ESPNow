#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// âœ… MAC RECEIVER
uint8_t receiverMAC[] = {0x00, 0x4B, 0x12, 0x3B, 0x64, 0xAC};

typedef struct {
  int temp;
  int hum;
} Data;

Data data;

// Callback gá»­i
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAIL");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);

  Serial.println("=== SENDER ===");

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init FAILED");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Add peer FAILED");
    return;
  }

  Serial.println("Sender ready");
}

void loop() {
  data.temp = random(25, 35);
  data.hum  = random(60, 80);

  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));

  if (result == ESP_OK) {
    Serial.println("Send OK");
  } else {
    Serial.println("Send Error");
  }

  delay(2000);
}