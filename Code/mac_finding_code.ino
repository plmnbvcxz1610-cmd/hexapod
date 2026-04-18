#include <WiFi.h>

// Thay bằng WiFi của bạn
const char* ssid = "VIETTEL";
const char* password = "123456a@";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Dang ket noi WiFi...");
  WiFi.begin(ssid, password);   // Bắt đầu kết nối

  // Chờ kết nối
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nDa ket noi WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());   // In IP khi kết nối thành công
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress()); // In MAC để bạn dùng
}

void loop() {
  // Bạn có thể thêm code ở đây, ví dụ gửi/nhận dữ liệu qua WiFi
}
