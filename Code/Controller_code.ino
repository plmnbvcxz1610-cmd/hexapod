#include <WiFi.h>
#include <esp_now.h>

// Địa chỉ MAC hexapod (Master)
uint8_t masterAddress[] = {0xF4, 0x65, 0x0B, 0x47, 0xB7, 0x2C};

// Chân đọc joystick
#define X_PIN 34
#define Y_PIN 35
#define SW_PIN 32   // chân nút bấm (SW), nối GND khi nhấn

typedef struct struct_message {
  int x;
  int y;
  int button;   // 0 = nhấn, 1 = thả
} struct_message;

struct_message outgoingData;

// Callback gửi
void OnDataSent(const wifi_tx_info_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Delivery Success");
  } else {
    Serial.println("Delivery Fail");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(SW_PIN, INPUT_PULLUP); // bật pull-up, nhấn = LOW

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Lỗi khởi tạo ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, masterAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Ghép cặp thất bại!");
    return;
  }

  Serial.println("Controller sẵn sàng gửi dữ liệu...");
}

void loop() {
  // Đọc joystick
  int rawX = analogRead(X_PIN);
  int rawY = analogRead(Y_PIN);

  // Chuẩn hóa giá trị joystick về -100 ~ 100
  outgoingData.x = map(rawX, 0, 4095, -100, 100);
  outgoingData.y = map(rawY, 0, 4095, -100, 100);

  // Đọc nút bấm (nhấn = 0, thả = 1)
  outgoingData.button = digitalRead(SW_PIN);

  // Gửi dữ liệu
  esp_err_t result = esp_now_send(masterAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));

  if (result == ESP_OK) {
    Serial.printf("Gửi: X=%d | Y=%d | BTN=%d\n", outgoingData.x, outgoingData.y, outgoingData.button);
  } else {
    Serial.println("Gửi dữ liệu thất bại!");
  }

  delay(100);
}
