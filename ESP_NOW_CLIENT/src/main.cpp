#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// ===== CẤU TRÚC DỮ LIỆU =====
typedef struct {
    float temperature;
    float humidity;
    bool  valid;
} SensorData;

SensorData receivedData;

// ===== CALLBACK NHẬN - tương thích ESP-IDF v4 và v5 =====
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
void onDataReceived(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    const uint8_t *mac_addr = info->src_addr;
#else
void onDataReceived(const uint8_t *mac_addr, const uint8_t *data, int len) {
#endif
    memcpy(&receivedData, data, sizeof(receivedData));

    Serial.println("=============================");
    Serial.printf("Tu MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        mac_addr[0], mac_addr[1], mac_addr[2],
        mac_addr[3], mac_addr[4], mac_addr[5]);

    if (receivedData.valid) {
        Serial.printf("Nhiet do : %.1f C\n", receivedData.temperature);
        Serial.printf("Do am    : %.1f %%\n", receivedData.humidity);
    } else {
        Serial.println("Du lieu khong hop le!");
    }
    Serial.println("=============================\n");
}

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.print("[RECEIVER] MAC Address: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK) {
        Serial.println("Khoi tao ESP-NOW that bai!");
        return;
    }

    esp_now_register_recv_cb(onDataReceived);
    Serial.println("ESP-NOW Receiver san sang!\n");
}

void loop() {
    // xu ly qua callback
}