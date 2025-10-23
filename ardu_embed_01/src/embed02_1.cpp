// #include <Arduino.h>

// // ====== 핀 설정 ======
// #define SW1 2
// #define SW2 3
// #define SW3 4

// #define LED_R 5
// #define LED_G 6
// #define LED_B 7

// void setup() {
//   // 스위치 입력 (내부 풀업 저항 사용)
//   pinMode(SW1, INPUT_PULLUP);
//   pinMode(SW2, INPUT_PULLUP);
//   pinMode(SW3, INPUT_PULLUP);

//   // RGB LED 출력
//   pinMode(LED_R, OUTPUT);
//   pinMode(LED_G, OUTPUT);
//   pinMode(LED_B, OUTPUT);
  
//   Serial.begin(9600);
//   Serial.println("==== SW1, SW2, SW3 TEST START ====");
// }

// void loop() {
//   // 스위치 상태 읽기 (PULLUP이므로, 눌렀을 때 LOW)
//   bool sw1 = digitalRead(SW1);
//   bool sw2 = digitalRead(SW2);
//   bool sw3 = digitalRead(SW3);

//   // SW1 테스트: RED LED 제어
//   if (sw1 == LOW) {
//     digitalWrite(LED_R, HIGH);
//     Serial.println("SW1 PRESSED");
//   } else {
//     digitalWrite(LED_R, LOW);
//   }

//   // SW2 테스트: GREEN LED 제어
//   if (sw2 == LOW) {
//     digitalWrite(LED_G, HIGH);
//     Serial.println("SW2 PRESSED");
//   } else {
//     digitalWrite(LED_G, LOW);
//   }

//   // SW3 테스트: BLUE LED 제어
//   if (sw3 == LOW) {
//     digitalWrite(LED_B, HIGH);
//     Serial.println("SW3 PRESSED");
//   } else {
//     digitalWrite(LED_B, LOW);
//   }

//   delay(100); // 너무 빠른 반복 방지
// }
