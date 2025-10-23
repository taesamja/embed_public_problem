// #include <Arduino.h>

// // ========== 핀 정의 ==========
// #define LED_R 5
// #define LED_G 6
// #define LED_B 7

// #define BCD_A 8
// #define BCD_B 9
// #define BCD_C 10
// #define BCD_D 11

// #define DIG_TENS 13   // 십의 자리 (왼쪽, 공통 VCC)
// #define DIG_ONES 12   // 일의 자리 (오른쪽, 공통 VCC)


// // ========== 74LS47 제어 ==========
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
//     // 십의 자리 (왼쪽)
//     digitalWrite(DIG_TENS, HIGH);  
//     digitalWrite(DIG_ONES, LOW);   
//     displayDigit(tens);
//     delay(4);

//     // 일의 자리 (오른쪽)
//     digitalWrite(DIG_TENS, LOW);   
//     digitalWrite(DIG_ONES, HIGH);  
//     displayDigit(ones);
//     delay(4);
//   }
// }

// // ========== FND 완전 소등 ==========
// void clearFND() {
//   digitalWrite(DIG_TENS, LOW);
//   digitalWrite(DIG_ONES, LOW);
// }

// // ========== RGB 제어 ==========
// void setRGB(bool r, bool g, bool b) {
//   // 공통 캐소드형 기준 (GND 공통)
//   digitalWrite(LED_R, r ? HIGH : LOW);
//   digitalWrite(LED_G, g ? HIGH : LOW);
//   digitalWrite(LED_B, b ? HIGH : LOW);
// }

// // 🔹 RGB LED 1초 동안 점등하면서 FND 00 유지
// void rgbShowWhileFND(bool r, bool g, bool b, unsigned long ms) {
//   unsigned long endT = millis() + ms;
//   setRGB(!r, !g, !b);
//   while (millis() < endT) {
//     displayNumber(0, 0, 10); // FND "00" 유지
//   }
// }

// // ========== 메인 루틴 ==========
// void setup() {
//   pinMode(BCD_A, OUTPUT);
//   pinMode(BCD_B, OUTPUT);
//   pinMode(BCD_C, OUTPUT);
//   pinMode(BCD_D, OUTPUT);
//   pinMode(DIG_TENS, OUTPUT);
//   pinMode(DIG_ONES, OUTPUT);
//   pinMode(LED_R, OUTPUT);
//   pinMode(LED_G, OUTPUT);
//   pinMode(LED_B, OUTPUT);

//   clearFND();
//   setRGB(true, true, true); // RGB OFF
//   delay(500);

//   // 1️⃣ FND "88" 1초 점등
//   displayNumber(8, 8, 1000);
//   // 2️⃣ 소등 1초
//   clearFND(); delay(1000);

//   // 3️⃣ 다시 "88" 1초 점등
//   displayNumber(8, 8, 1000);
//   // 4️⃣ 소등 1초
//   clearFND(); delay(1000);

//   // 5️⃣ “00” 표시 + RGB 시퀀스
//   // ---------------------------
//   displayNumber(0, 0, 1000);  // FND "00" 유지

//   rgbShowWhileFND(true, false, false, 1000);   // R 1초
//   rgbShowWhileFND(false, true, false, 1000);   // G 1초
//   rgbShowWhileFND(false, false, true, 1000);   // B 1초
//   rgbShowWhileFND(false, false, false, 1000);  // OFF 1초
//   rgbShowWhileFND(true, false, false, 1000);   // 다시 R 유지
//   // ---------------------------

//   // 마지막 상태 유지
//   while (true) displayNumber(0, 0, 10);
// }

// void loop() {}
