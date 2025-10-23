// #include <Arduino.h>

// // ========== 핀 정의 ==========
// #define LED_R 5
// #define LED_G 6
// #define LED_B 7

// #define BCD_A 8
// #define BCD_B 9
// #define BCD_C 10
// #define BCD_D 11
// #define DIG_TENS 13
// #define DIG_ONES 12

// #define SW1 2
// #define SW2 3
// #define SW3 4

// bool isInitProcess = false;     // 초기화 중 여부
// bool lastSW3 = HIGH;            // SW3 이전상태 저장용

// int currentNumber = 0;          // 동작 1, 2 시 표시용 FND 숫자

// // ---------- 74LS47 제어 ----------
// void displayDigit(int n) {
//   digitalWrite(BCD_A, n & 0x01);
//   digitalWrite(BCD_B, (n >> 1) & 0x01);
//   digitalWrite(BCD_C, (n >> 2) & 0x01);
//   digitalWrite(BCD_D, (n >> 3) & 0x01);
// }

// // ---------- 2자리 표시 ----------
// void displayNumber(int tens, int ones, unsigned long ms) {
//   unsigned long endT = millis() + ms;
//   while (millis() < endT) {
//     if (isInitProcess) {            // 초기화 중엔 스위치 입력 무시
//       digitalRead(SW1); digitalRead(SW2); digitalRead(SW3);
//     }
//     digitalWrite(DIG_TENS, HIGH);
//     digitalWrite(DIG_ONES, LOW);
//     displayDigit(tens);
//     delay(4);
//     digitalWrite(DIG_TENS, LOW);
//     digitalWrite(DIG_ONES, HIGH);
//     displayDigit(ones);
//     delay(4);
//   }
// }

// void clearFND() {
//   digitalWrite(DIG_TENS, LOW);
//   digitalWrite(DIG_ONES, LOW);
// }

// void setRGB(bool r, bool g, bool b) {
//   digitalWrite(LED_R, r ? HIGH : LOW);
//   digitalWrite(LED_G, g ? HIGH : LOW);
//   digitalWrite(LED_B, b ? HIGH : LOW);
// }

// void rgbShowWhileFND(bool r, bool g, bool b, unsigned long ms) {
//   unsigned long endT = millis() + ms;
//   setRGB(!r, !g, !b);
//   while (millis() < endT) {
//     displayNumber(0, 0, 10);
//   }
// }

// // ---------- [초기화 동작] ----------
// void performInitProcess() {
//   isInitProcess = true;

//   clearFND();
//   setRGB(true, true, true);
//   delay(500);

//   displayNumber(8, 8, 1000);
//   clearFND(); delay(1000);

//   displayNumber(8, 8, 1000);
//   clearFND(); delay(1000);

//   displayNumber(0, 0, 1000);

//   rgbShowWhileFND(true, false, false, 1000);   // R
//   rgbShowWhileFND(false, true, false, 1000);   // G
//   rgbShowWhileFND(false, false, true, 1000);   // B
//   rgbShowWhileFND(false, false, false, 1000);  // OFF
//   rgbShowWhileFND(true, false, false, 1000);   // R유지

//   // 초기화 완료 후 기본 상태 복귀
//   currentNumber = 0;
//   isInitProcess = false;
// }

// // ---------- [동작1] 예시 ----------
// void performAction1() {
//   for (int i = 0; i < 50; i++) {  // 수행 도중 SW3 누르면 즉시 초기화
//     if (digitalRead(SW3) == LOW) {
//       performInitProcess();
//       return;
//     }
//     currentNumber++;
//     if (currentNumber > 99) currentNumber = 0;
//     int tens = currentNumber / 10;
//     int ones = currentNumber % 10;
//     displayNumber(tens, ones, 20);
//   }
// }

// // ---------- [동작2] 예시 ----------
// void performAction2() {
//   for (int i = 0; i < 50; i++) {  // 수행 도중 SW3 누르면 즉시 초기화
//     if (digitalRead(SW3) == LOW) {
//       performInitProcess();
//       return;
//     }
//     currentNumber--;
//     if (currentNumber < 0) currentNumber = 99;
//     int tens = currentNumber / 10;
//     int ones = currentNumber % 10;
//     displayNumber(tens, ones, 20);
//   }
// }

// // ---------- setup ----------
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
//   pinMode(SW1, INPUT_PULLUP);
//   pinMode(SW2, INPUT_PULLUP);
//   pinMode(SW3, INPUT_PULLUP);

//   performInitProcess();  // 전원 인가 시 초기화 1회 수행
// }

// // ---------- loop ----------
// void loop() {
//   displayNumber(currentNumber / 10, currentNumber % 10, 10);

//   bool sw3_now = digitalRead(SW3);
//   if (lastSW3 == LOW && sw3_now == HIGH) performInitProcess();
//   lastSW3 = sw3_now;

//   // [동작 1]
//   if (digitalRead(SW1) == LOW) performAction1();
//   // [동작 2]
//   if (digitalRead(SW2) == LOW) performAction2();
// }
