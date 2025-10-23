// #include <Arduino.h>

// // ------------------ 핀 설정 ------------------
// #define BCD_A 8
// #define BCD_B 9
// #define BCD_C 10
// #define BCD_D 11

// #define DIG_TENS 13   // 십의 자리 (왼쪽 FND 공통)
// #define DIG_ONES 12   // 일의 자리 (오른쪽 FND 공통)

// // ------------------ 74LS47 제어 ------------------
// void displayDigit(int n) {
//   digitalWrite(BCD_A, n & 0x01);
//   digitalWrite(BCD_B, (n >> 1) & 0x01);
//   digitalWrite(BCD_C, (n >> 2) & 0x01);
//   digitalWrite(BCD_D, (n >> 3) & 0x01);
// }

// // ------------------ 두 자리 시분할 표시 ------------------
// void displayNumber(int tens, int ones, unsigned long duration_ms) {
//   unsigned long endT = millis() + duration_ms;

//   while (millis() < endT) {
//     // 🔹 십의 자리 표시 (왼쪽)
//     digitalWrite(DIG_TENS, HIGH);  // 십의 자리 ON
//     digitalWrite(DIG_ONES, LOW);   // 일의 자리 OFF 
//     displayDigit(tens);
//     delay(4);

//     // 🔹 일의 자리 표시 (오른쪽)
//     digitalWrite(DIG_TENS, LOW);   // 십의 자리 OFF
//     digitalWrite(DIG_ONES, HIGH);  // 일의 자리 ON
//     displayDigit(ones);
//     delay(4);
//   }
// }

// // ------------------ 완전 소등 ------------------
// void clearFND() {
//   digitalWrite(DIG_TENS, LOW);
//   digitalWrite(DIG_ONES, LOW);
// }

// // ------------------ 메인 루틴 ------------------
// void setup() {
//   pinMode(BCD_A, OUTPUT);
//   pinMode(BCD_B, OUTPUT);
//   pinMode(BCD_C, OUTPUT);
//   pinMode(BCD_D, OUTPUT);
//   pinMode(DIG_TENS, OUTPUT);
//   pinMode(DIG_ONES, OUTPUT);

//   clearFND();  // 초기 표시 꺼두기

//   // 1️⃣ “88” 1초 점등
//   displayNumber(8, 8, 1000);

//   // 2️⃣ 완전 소등 1초
//   clearFND();
//   delay(1000);

//   // 3️⃣ 다시 “88” 1초
//   displayNumber(8, 8, 1000);

//   // 4️⃣ 완전 소등 1초
//   clearFND();
//   delay(1000);

//   // 5️⃣ 마지막 “00” 표시 유지
//   while (true) {
//     displayNumber(0, 0, 10);
//   }
// }

// void loop() {
//   // 사용 안 함
// }
