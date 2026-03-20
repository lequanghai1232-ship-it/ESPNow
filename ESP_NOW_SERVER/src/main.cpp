#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <DHT.h>

// ===== CẤU HÌNH =====
#define DHTPIN     4        // GPIO kết nối DHT11
#define DHTTYPE    DHT11

// MAC Address của Receiver (thay bằng MAC thực của board nhận)
uint8_t receiverMAC[] = {0xA4, 0xF0, 0x0F, 0x84, 0x09, 0x24};

// ===== CẤU TRÚC DỮ LIỆU =====
typedef struct {
    float temperature;
    float humidity;
    bool  valid;
} SensorData;

SensorData dataToSend;
DHT dht(DHTPIN, DHTTYPE);

// ===== CALLBACK GỬI =====
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("[SENDER] Trạng thái gửi: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "✅ Thành công" : "❌ Thất bại");
}

void setup() {
    Serial.begin(115200);
    dht.begin();

    // Khởi động WiFi ở chế độ Station (bắt buộc cho ESP-NOW)
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.print("[SENDER] MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Khởi tạo ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("❌ Khởi tạo ESP-NOW thất bại!");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    // Đăng ký Peer (thiết bị nhận)
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("❌ Thêm peer thất bại!");
        return;
    }

    Serial.println("✅ ESP-NOW Sender sẵn sàng!");
}

void loop() {
    // Đọc cảm biến DHT11
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
        Serial.println("⚠️  Lỗi đọc DHT11!");
        dataToSend.valid = false;
    } else {
        dataToSend.temperature = t;
        dataToSend.humidity    = h;
        dataToSend.valid       = true;

        Serial.printf("[SENDER] Nhiệt độ: %.1f°C | Độ ẩm: %.1f%%\n", t, h);
    }

    // Gửi dữ liệu qua ESP-NOW
    esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&dataToSend, sizeof(dataToSend));
    if (result != ESP_OK) {
        Serial.println("❌ Gửi thất bại!");
    }

    delay(1000); // Gửi mỗi 1 giây
}