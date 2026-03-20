#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

typedef struct {
  int temp;
  int hum;
} Data;

Data data;

// Callback nhÃ¡ÂºÂ­n
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  Serial.print("From: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  Serial.print("Temp: ");
  Serial.print(data.temp);
  Serial.print(" | Hum: ");
  Serial.println(data.hum);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);

  Serial.println("=== RECEIVER ===");
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init FAILED");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Receiver ready");
}

void loop() {
}