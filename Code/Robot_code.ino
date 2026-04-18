#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_PWMServoDriver.h>
#include <cmath>
using namespace std;
Adafruit_PWMServoDriver R_pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver L_pwm = Adafruit_PWMServoDriver(0x41);

typedef struct struct_message {
  int x;
  int y;
  int button;
} struct_message; // Tạo struct giống bên master
struct_message joystickData; // Lưu dữ liệu nhận

void OnDataRecv(const esp_now_recv_info *info, const uint8_t *data, int len)
{
  if (len == sizeof(joystickData)) {
    memcpy(&joystickData, data, sizeof(joystickData));
  }
}

int deadzone = 10;

double anpha;

int DeltaXungR1 = -28; //
int DeltaXungR2 = -40;
int DeltaXungR3 = 0;
int DeltaXungR5 = -3; //
int DeltaXungR6 = 30;
int DeltaXungR7 = 15;
int DeltaXungR9 = -10; //
int DeltaXungR10 = -15;
int DeltaXungR11 = -20;

int DeltaXungL1 = -15; //
int DeltaXungL2 = -10;
int DeltaXungL3 = -15;
int DeltaXungL5 = -13; //
int DeltaXungL6 = -20;
int DeltaXungL7 = 30;
int DeltaXungL9 = 8; //
int DeltaXungL10 = 22;
int DeltaXungL11 = 18;


double limitAngle(double angle) {  
  if (angle > PI)  angle -= 2*PI;
  if (angle < -PI) angle += 2*PI;
  return angle;
}

int cal_a(double anpha, double bc) { 
  anpha = limitAngle(anpha);
  double a = asin(
      (bc * sin(PI - fabs(anpha))) /
      sqrt(49 + bc * bc - 14 * bc * cos(PI - fabs(anpha)))
  );
  if (anpha < 0) {
      a = -a;
  }
  a = round ((a*450)/PI);   
  return a;
}

int cal_b(double anpha, double bc, int curve) { 
  anpha = limitAngle(anpha);
  double L = sqrt(49 + bc * bc - 14 * bc * cos(PI - fabs(anpha))) - 7;

  double h; // Khai báo trước if
  if (curve == 1) 
    h = 8.2 - 2 + (bc - 2)*(bc - 2)/2;  // parabol --> 1
  else if (curve == 2) 
    h = 8.2; // đường thẳng --> 2

  double canhhuyen = sqrt((4.38 + L) * (4.38 + L) + h*h);
  double b = acos((canhhuyen * canhhuyen + 25 - 96.04) / (10 * canhhuyen))
              - atan(h / (4.38 + L))
              - 0.32;
  b = round ((b*450)/PI); 
  return b;
}

int cal_c(double anpha, double bc, int curve){
  anpha = limitAngle(anpha);
  double L = sqrt(49 + bc * bc - 14 * bc * cos(PI - fabs(anpha))) - 7;

  double h; // Khai báo trước if
  if (curve == 1) 
    h = 8.2 - 2 + (bc - 2)*(bc - 2)/2;  // parabol --> 1
  else if (curve == 2) 
    h = 8.2; // đường thẳng --> 2

  double canhhuyen = sqrt((4.38 + L) * (4.38 + L) + h*h);
  double c = acos((121.04 - canhhuyen*canhhuyen)/98) - 1.21;  //121.04 = 9.8^2 + 5^2
  c = round ((c*450)/PI);
  return c;
}

void setup() {
  R_pwm.begin();
  L_pwm.begin();
  R_pwm.setPWMFreq(50);
  L_pwm.setPWMFreq(50);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Lỗi khởi tạo ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop(){
  int x1 = - joystickData.x;
  int y1 = joystickData.y;

  // áp dụng deadzone
  if (abs(x1) < deadzone) x1 = 0;
  if (abs(y1) < deadzone) y1 = 0;

  if ( x1 == 0 && y1 == 0) {
    Serial.println("Stop");
    
    L_pwm.setPWM(5, 0, 375 + DeltaXungL5 );
    L_pwm.setPWM(6, 0, 375 + DeltaXungL6 );
    L_pwm.setPWM(7, 0, 375 + DeltaXungL7 );

    R_pwm.setPWM(1, 0, 375 + DeltaXungR1 );
    R_pwm.setPWM(2, 0, 375 + DeltaXungR2 );
    R_pwm.setPWM(3, 0, 375 + DeltaXungR3 );

    R_pwm.setPWM(9, 0, 375 + DeltaXungR9 );
    R_pwm.setPWM(10, 0, 375 + DeltaXungR10 );
    R_pwm.setPWM(11, 0, 375 + DeltaXungR11 );


    L_pwm.setPWM(1, 0, 375 + DeltaXungL1 );
    L_pwm.setPWM(2, 0, 375 + DeltaXungL2 );
    L_pwm.setPWM(3, 0, 375 + DeltaXungL3 );

    L_pwm.setPWM(9, 0, 375 + DeltaXungL9 );
    L_pwm.setPWM(10, 0, 375 + DeltaXungL10 );
    L_pwm.setPWM(11, 0, 375 + DeltaXungL11 );

    R_pwm.setPWM(5, 0, 375 + DeltaXungR5 );
    R_pwm.setPWM(6, 0, 375 + DeltaXungR6 );
    R_pwm.setPWM(7, 0, 375 + DeltaXungR7 );
  }
  else {
    // B1
    if ( joystickData.button == 0 ){ // nút đã nhấn 
      if ( x1 >= 0){
        for (double bc = 0.25; bc <= 4; bc += 0.25) {
          
          L_pwm.setPWM(5, 0, 375 + DeltaXungL5 + cal_a(-PI/2, bc));
					L_pwm.setPWM(6, 0, 375 + DeltaXungL6 + cal_b(-PI/2, bc, 1));
					L_pwm.setPWM(7, 0, 375 + DeltaXungL7 - cal_c(-PI/2, bc, 1));

					R_pwm.setPWM(1, 0, 375 + DeltaXungR1 + cal_a(-PI/2, bc));
					R_pwm.setPWM(2, 0, 375 + DeltaXungR2 - cal_b(-PI/2, bc, 1));
					R_pwm.setPWM(3, 0, 375 + DeltaXungR3 + cal_c(-PI/2, bc, 1));

					R_pwm.setPWM(9, 0, 375 + DeltaXungR9 + cal_a(-PI/2, bc));
					R_pwm.setPWM(10, 0, 375 + DeltaXungR10 - cal_b(-PI/2, bc, 1));
					R_pwm.setPWM(11, 0, 375 + DeltaXungR11 + cal_c(-PI/2, bc, 1));

					L_pwm.setPWM(1, 0, 375 + DeltaXungL1 + cal_a(-PI/2, 4 - bc));
					L_pwm.setPWM(2, 0, 375 + DeltaXungL2 + cal_b(-PI/2, 4 - bc, 2));
					L_pwm.setPWM(3, 0, 375 + DeltaXungL3 - cal_c(-PI/2, 4 - bc, 2));

					L_pwm.setPWM(9, 0, 375 + DeltaXungL9 + cal_a(-PI/2, 4 - bc));
					L_pwm.setPWM(10, 0, 375 + DeltaXungL10 + cal_b(-PI/2, 4 - bc, 2));
					L_pwm.setPWM(11, 0, 375 + DeltaXungL11 - cal_c(-PI/2, 4 - bc, 2));

					R_pwm.setPWM(5, 0, 375 + DeltaXungR5 + cal_a(-PI/2, 4 - bc));
					R_pwm.setPWM(6, 0, 375 + DeltaXungR6 - cal_b(-PI/2, 4 - bc, 2));
					R_pwm.setPWM(7, 0, 375 + DeltaXungR7 + cal_c(-PI/2, 4 - bc, 2));

          delay(10);
        }
      }
      if ( x1 < 0){
        for (double bc = 0.25; bc <= 4; bc += 0.25) {
          
					L_pwm.setPWM(5, 0, 375 + DeltaXungL5 + cal_a(PI/2, bc));
					L_pwm.setPWM(6, 0, 375 + DeltaXungL6 + cal_b(PI/2, bc, 1));
					L_pwm.setPWM(7, 0, 375 + DeltaXungL7 - cal_c(PI/2, bc, 1));

					R_pwm.setPWM(1, 0, 375 + DeltaXungR1 + cal_a(PI/2, bc));
					R_pwm.setPWM(2, 0, 375 + DeltaXungR2 - cal_b(PI/2, bc, 1));
					R_pwm.setPWM(3, 0, 375 + DeltaXungR3 + cal_c(PI/2, bc, 1));

					R_pwm.setPWM(9, 0, 375 + DeltaXungR9 + cal_a(PI/2, bc));
					R_pwm.setPWM(10, 0, 375 + DeltaXungR10 - cal_b(PI/2, bc, 1));
					R_pwm.setPWM(11, 0, 375 + DeltaXungR11 + cal_c(PI/2, bc, 1));

					L_pwm.setPWM(1, 0, 375 + DeltaXungL1 + cal_a(PI/2, 4 - bc));
					L_pwm.setPWM(2, 0, 375 + DeltaXungL2 + cal_b(PI/2, 4 - bc, 2));
					L_pwm.setPWM(3, 0, 375 + DeltaXungL3 - cal_c(PI/2, 4 - bc, 2));

					L_pwm.setPWM(9, 0, 375 + DeltaXungL9 + cal_a(PI/2, 4 - bc));
					L_pwm.setPWM(10, 0, 375 + DeltaXungL10 + cal_b(PI/2, 4 - bc, 2));
					L_pwm.setPWM(11, 0, 375 + DeltaXungL11 - cal_c(PI/2, 4 - bc, 2));

					R_pwm.setPWM(5, 0, 375 + DeltaXungR5 + cal_a(PI/2, 4 - bc));
					R_pwm.setPWM(6, 0, 375 + DeltaXungR6 - cal_b(PI/2, 4 - bc, 2));
					R_pwm.setPWM(7, 0, 375 + DeltaXungR7 + cal_c(PI/2, 4 - bc, 2));

          delay(10);
        }
      }
    }
    if ( joystickData.button == 1 ){ // nút chưa nhấn 
      x1 = - joystickData.x;
      y1 = joystickData.y; 
      anpha = atan2(y1, x1);
      Serial.println(anpha, 6);
      for (double bc = 0.25; bc <= 4; bc += 0.25) {
        
        L_pwm.setPWM(5, 0, 375 + DeltaXungL5 + cal_a(anpha - PI, bc));
        L_pwm.setPWM(6, 0, 375 + DeltaXungL6 + cal_b(anpha - PI, bc, 1));
        L_pwm.setPWM(7, 0, 375 + DeltaXungL7 - cal_c(anpha - PI, bc, 1));

        R_pwm.setPWM(1, 0, 375 + DeltaXungR1 + cal_a(anpha - PI/3, bc));
        R_pwm.setPWM(2, 0, 375 + DeltaXungR2 - cal_b(anpha - PI/3, bc, 1));
        R_pwm.setPWM(3, 0, 375 + DeltaXungR3 + cal_c(anpha - PI/3, bc, 1));

        R_pwm.setPWM(9, 0, 375 + DeltaXungR9 + cal_a(anpha + PI/3, bc));
        R_pwm.setPWM(10, 0, 375 + DeltaXungR10 - cal_b(anpha + PI/3, bc, 1));
        R_pwm.setPWM(11, 0, 375 + DeltaXungR11 + cal_c(anpha + PI/3, bc, 1));

        L_pwm.setPWM(1, 0, 375 + DeltaXungL1 + cal_a(anpha - 2*PI/3, 4 - bc));
        L_pwm.setPWM(2, 0, 375 + DeltaXungL2 + cal_b(anpha - 2*PI/3, 4 - bc, 2));
        L_pwm.setPWM(3, 0, 375 + DeltaXungL3 - cal_c(anpha - 2*PI/3, 4 - bc, 2));

        L_pwm.setPWM(9, 0, 375 + DeltaXungL9 + cal_a(anpha + 2*PI/3, 4 - bc));
        L_pwm.setPWM(10, 0, 375 + DeltaXungL10 + cal_b(anpha + 2*PI/3, 4 - bc, 2));
        L_pwm.setPWM(11, 0, 375 + DeltaXungL11 - cal_c(anpha + 2*PI/3, 4 - bc, 2));

        R_pwm.setPWM(5, 0, 375 + DeltaXungR5 + cal_a(anpha, 4 - bc));
        R_pwm.setPWM(6, 0, 375 + DeltaXungR6 - cal_b(anpha, 4 - bc, 2));
        R_pwm.setPWM(7, 0, 375 + DeltaXungR7 + cal_c(anpha, 4 - bc, 2));
    
        delay(10);
      }
    }

    // B2
    if ( joystickData.button == 0 ){ // nút đã nhấn
      if ( x1 >= 0){
        for (double bc = 0.25; bc <= 4; bc += 0.25) {

					L_pwm.setPWM(1, 0, 375 + DeltaXungL1 + cal_a(-PI/2, bc));
					L_pwm.setPWM(2, 0, 375 + DeltaXungL2 + cal_b(-PI/2, bc, 1));
					L_pwm.setPWM(3, 0, 375 + DeltaXungL3 - cal_c(-PI/2, bc, 1));

					L_pwm.setPWM(9, 0, 375 + DeltaXungL9 + cal_a(-PI/2, bc));
					L_pwm.setPWM(10, 0, 375 + DeltaXungL10 + cal_b(-PI/2, bc, 1));
					L_pwm.setPWM(11, 0, 375 + DeltaXungL11 - cal_c(-PI/2, bc, 1));

					R_pwm.setPWM(5, 0, 375 + DeltaXungR5 + cal_a(-PI/2, bc));
					R_pwm.setPWM(6, 0, 375 + DeltaXungR6 - cal_b(-PI/2, bc, 1));
					R_pwm.setPWM(7, 0, 375 + DeltaXungR7 + cal_c(-PI/2, bc, 1));

					L_pwm.setPWM(5, 0, 375 + DeltaXungL5 + cal_a(-PI/2, 4 - bc));
					L_pwm.setPWM(6, 0, 375 + DeltaXungL6 + cal_b(-PI/2, 4 - bc, 2));
					L_pwm.setPWM(7, 0, 375 + DeltaXungL7 - cal_c(-PI/2, 4 - bc, 2));

					R_pwm.setPWM(1, 0, 375 + DeltaXungR1 + cal_a(-PI/2, 4 - bc));
					R_pwm.setPWM(2, 0, 375 + DeltaXungR2 - cal_b(-PI/2, 4 - bc, 2));
					R_pwm.setPWM(3, 0, 375 + DeltaXungR3 + cal_c(-PI/2, 4 - bc, 2));

					R_pwm.setPWM(9, 0, 375 + DeltaXungR9 + cal_a(-PI/2, 4 - bc));
					R_pwm.setPWM(10, 0, 375 + DeltaXungR10 - cal_b(-PI/2, 4 - bc, 2));
					R_pwm.setPWM(11, 0, 375 + DeltaXungR11 + cal_c(-PI/2, 4 - bc, 2));

          delay(10);
        }
      }
      if ( x1 < 0){
        for (double bc = 0.25; bc <= 4; bc += 0.25) {
			
					L_pwm.setPWM(1, 0, 375 + DeltaXungL1 + cal_a(PI/2, bc));
					L_pwm.setPWM(2, 0, 375 + DeltaXungL2 + cal_b(PI/2, bc, 1));
					L_pwm.setPWM(3, 0, 375 + DeltaXungL3 - cal_c(PI/2, bc, 1));

					L_pwm.setPWM(9, 0, 375 + DeltaXungL9 + cal_a(PI/2, bc));
					L_pwm.setPWM(10, 0, 375 + DeltaXungL10 + cal_b(PI/2, bc, 1));
					L_pwm.setPWM(11, 0, 375 + DeltaXungL11 - cal_c(PI/2, bc, 1));

					R_pwm.setPWM(5, 0, 375 + DeltaXungR5 + cal_a(PI/2, bc));
					R_pwm.setPWM(6, 0, 375 + DeltaXungR6 - cal_b(PI/2, bc, 1));
					R_pwm.setPWM(7, 0, 375 + DeltaXungR7 + cal_c(PI/2, bc, 1));

					L_pwm.setPWM(5, 0, 375 + DeltaXungL5 + cal_a(PI/2, 4 - bc));
					L_pwm.setPWM(6, 0, 375 + DeltaXungL6 + cal_b(PI/2, 4 - bc, 2));
					L_pwm.setPWM(7, 0, 375 + DeltaXungL7 - cal_c(PI/2, 4 - bc, 2));

					R_pwm.setPWM(1, 0, 375 + DeltaXungR1 + cal_a(PI/2, 4 - bc));
					R_pwm.setPWM(2, 0, 375 + DeltaXungR2 - cal_b(PI/2, 4 - bc, 2));
					R_pwm.setPWM(3, 0, 375 + DeltaXungR3 + cal_c(PI/2, 4 - bc, 2));

					R_pwm.setPWM(9, 0, 375 + DeltaXungR9 + cal_a(PI/2, 4 - bc));
					R_pwm.setPWM(10, 0, 375 + DeltaXungR10 - cal_b(PI/2, 4 - bc, 2));
					R_pwm.setPWM(11, 0, 375 + DeltaXungR11 + cal_c(PI/2, 4 - bc, 2));

          delay(10);
        }
      }
    } 
    if ( joystickData.button == 1 ){ // nút chưa nhấn  
      x1 = - joystickData.x;
      y1 = joystickData.y;  
      anpha = atan2(y1, x1);
      Serial.println(anpha, 6);
      for (double bc = 0.25; bc <= 4; bc += 0.25) {

        L_pwm.setPWM(1, 0, 375 + DeltaXungL1 + cal_a(anpha - 2*PI/3, bc));
        L_pwm.setPWM(2, 0, 375 + DeltaXungL2 + cal_b(anpha - 2*PI/3, bc, 1));
        L_pwm.setPWM(3, 0, 375 + DeltaXungL3 - cal_c(anpha - 2*PI/3, bc, 1));

        L_pwm.setPWM(9, 0, 375 + DeltaXungL9 + cal_a(anpha + 2*PI/3, bc));
        L_pwm.setPWM(10, 0, 375 + DeltaXungL10 + cal_b(anpha + 2*PI/3, bc, 1));
        L_pwm.setPWM(11, 0, 375 + DeltaXungL11 - cal_c(anpha + 2*PI/3, bc, 1));

        R_pwm.setPWM(5, 0, 375 + DeltaXungR5 + cal_a(anpha, bc));
        R_pwm.setPWM(6, 0, 375 + DeltaXungR6 - cal_b(anpha, bc, 1));
        R_pwm.setPWM(7, 0, 375 + DeltaXungR7 + cal_c(anpha, bc, 1));

        L_pwm.setPWM(5, 0, 375 + DeltaXungL5 + cal_a(anpha - PI, 4 - bc));
        L_pwm.setPWM(6, 0, 375 + DeltaXungL6 + cal_b(anpha - PI, 4 - bc, 2));
        L_pwm.setPWM(7, 0, 375 + DeltaXungL7 - cal_c(anpha - PI, 4 - bc, 2));

        R_pwm.setPWM(1, 0, 375 + DeltaXungR1 + cal_a(anpha - PI/3, 4 - bc));
        R_pwm.setPWM(2, 0, 375 + DeltaXungR2 - cal_b(anpha - PI/3, 4 - bc, 2));
        R_pwm.setPWM(3, 0, 375 + DeltaXungR3 + cal_c(anpha - PI/3, 4 - bc, 2));

        R_pwm.setPWM(9, 0, 375 + DeltaXungR9 + cal_a(anpha + PI/3, 4 - bc));
        R_pwm.setPWM(10, 0, 375 + DeltaXungR10 - cal_b(anpha + PI/3, 4 - bc, 2));
        R_pwm.setPWM(11, 0, 375 + DeltaXungR11 + cal_c(anpha + PI/3, 4 - bc, 2));

        delay(10);
      }  
    }
  }
}
