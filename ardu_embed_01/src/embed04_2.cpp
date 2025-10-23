// #include <Arduino.h>

// // ---------------- 핀 정의 ----------------
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

// bool isInitProcess = false;  
// bool lastSW1 = HIGH;
// bool lastSW2 = HIGH;
// bool lastSW3 = HIGH;
// int currentNumber = 0;

// // ---------------- FND 표시 ----------------
// void displayDigit(int n) {
//   digitalWrite(BCD_A, n & 0x01);
//   digitalWrite(BCD_B, (n >> 1) & 0x01);
//   digitalWrite(BCD_C, (n >> 2) & 0x01);
//   digitalWrite(BCD_D, (n >> 3) & 0x01);
// }

// void displayNumber(int tens, int ones, unsigned long time_ms) {
//   unsigned long endT = millis() + time_ms;
//   while (millis() < endT) {
//     if (isInitProcess) {  // 초기화 중 SW무시
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
//   digitalWrite(DIG_TENS, LOW);
//   digitalWrite(DIG_ONES, LOW);
// }

// void clearFND() {
//   digitalWrite(DIG_TENS, LOW);
//   digitalWrite(DIG_ONES, LOW);
// }

// // ---------------- RGB 제어 ----------------
// void setRGB(bool r, bool g, bool b) {
//   // 공통 애노드형 (VCC공통 → LOW 켜짐)
//   digitalWrite(LED_R, r ? LOW : HIGH);
//   digitalWrite(LED_G, g ? LOW : HIGH);
//   digitalWrite(LED_B, b ? LOW : HIGH);
// }

// // ---------------- 동작 3 (일시정지) ----------------
// void performAction3(int savedNumber) {
//   isInitProcess = true;  // 동작3 수행 중 모든 SW무시
//   int tens = savedNumber / 10;
//   int ones = savedNumber % 10;

//   for (int i = 0; i < 2; i++) {
//     // FND + R LED 2초 점등
//     setRGB(true, false, false); // R ON
//     unsigned long onTime = millis() + 2000;
//     while (millis() < onTime) {
//       digitalWrite(DIG_TENS, HIGH);
//       digitalWrite(DIG_ONES, LOW);
//       displayDigit(tens);
//       delay(4);
//       digitalWrite(DIG_TENS, LOW);
//       digitalWrite(DIG_ONES, HIGH);
//       displayDigit(ones);
//       delay(4);
//     }

//     // 2초 소등
//     setRGB(false, false, false);
//     unsigned long offTime = millis() + 2000;
//     while (millis() < offTime) {
//       clearFND();
//       delay(10);
//     }
//   }
//   isInitProcess = false;  // 복귀 가능
// }

// // ---------------- 초기화 동작 ----------------
// // ---------------- 초기화 동작 ----------------
// void performInitProcess() {
//   isInitProcess = true;
//   clearFND();
//   setRGB(false, false, false);
//   delay(500);

//   // FND “88” → 꺼짐 2회
//   displayNumber(8, 8, 1000);
//   clearFND(); delay(1000);
//   displayNumber(8, 8, 1000);
//   clearFND(); delay(1000);

//   // ✅ 숫자 00과 함께 RGB 순차 점등 (FND 유지)
//   unsigned long tStart = millis();

//   // R 1초
//   tStart = millis();
//   setRGB(true, false, false);
//   while (millis() - tStart < 1000) {
//     displayNumber(0, 0, 10);   // FND "00" 유지
//   }

//   // G 1초
//   tStart = millis();
//   setRGB(false, true, false);
//   while (millis() - tStart < 1000) {
//     displayNumber(0, 0, 10);
//   }

//   // B 1초
//   tStart = millis();
//   setRGB(false, false, true);
//   while (millis() - tStart < 1000) {
//     displayNumber(0, 0, 10);
//   }

//   // OFF 1초
//   tStart = millis();
//   setRGB(false, false, false);
//   while (millis() - tStart < 1000) {
//     displayNumber(0, 0, 10);
//   }

//   // ✅ 마지막은 R ON, 00 표시 상태 유지
//   setRGB(true, false, false);
//   currentNumber = 0;
//   isInitProcess = false;
// }

// // ---------------- 동작 2 선언 ----------------
// void performAction2();

// // ---------------- 동작 1 ----------------
// void performAction1() {
//   if (isInitProcess) return;

//   displayNumber(8, 8, 2000);
//   setRGB(false, true, false); delay(500);  // G ON
//   setRGB(true, false, false); delay(500);  // R ON

//   bool bState = false;
//   unsigned long prevBTime = millis();
//   unsigned long prevCountTime = millis();
//   int count = 0;
//   digitalWrite(LED_R, HIGH);
//   digitalWrite(LED_G, HIGH);

//   while (count <= 99) {
//     unsigned long now = millis();

//     if (now - prevBTime >= 500) {
//       bState = !bState;
//       digitalWrite(LED_B, bState ? LOW : HIGH);
//       prevBTime = now;
//     }
//     if (now - prevCountTime >= 500) {
//       count++;
//       prevCountTime = now;
//     }

//     int tens = count / 10;
//     int ones = count % 10;
//     digitalWrite(DIG_TENS, HIGH);
//     digitalWrite(DIG_ONES, LOW);
//     displayDigit(tens);
//     delay(4);
//     digitalWrite(DIG_TENS, LOW);
//     digitalWrite(DIG_ONES, HIGH);
//     displayDigit(ones);
//     delay(4);

//     // SW1 자기 자신 → 동작3
//     if (digitalRead(SW1) == LOW) {
//       performAction3(count);
//     }
//     // SW2 → 동작2 즉시 전환
//     if (digitalRead(SW2) == LOW) { performAction2(); return; }
//     // SW3 → 초기화
//     if (digitalRead(SW3) == LOW) { performInitProcess(); return; }
//   }

//   digitalWrite(LED_R, HIGH);
//   digitalWrite(LED_G, HIGH);
//   digitalWrite(LED_B, LOW); // B ON 유지
//   currentNumber = 99;
// }

// // ---------------- 동작 2 ----------------
// void performAction2() {
//   if (isInitProcess) return;

//   displayNumber(8, 8, 2000);
//   setRGB(false, false, true); delay(500); // B ON
//   setRGB(true, false, false); delay(500); // R ON

//   bool gState = false;
//   unsigned long prevGTime = millis();
//   unsigned long prevCountTime = millis();
//   int count = 99;
//   digitalWrite(LED_R, HIGH);
//   digitalWrite(LED_B, HIGH);

//   while (count >= 0) {
//     unsigned long now = millis();

//     if (now - prevGTime >= 500) {
//       gState = !gState;
//       digitalWrite(LED_G, gState ? LOW : HIGH);
//       prevGTime = now;
//     }
//     if (now - prevCountTime >= 500) {
//       count--;
//       prevCountTime = now;
//     }

//     int tens = count / 10;
//     int ones = count % 10;
//     digitalWrite(DIG_TENS, HIGH);
//     digitalWrite(DIG_ONES, LOW);
//     displayDigit(tens);
//     delay(4);
//     digitalWrite(DIG_TENS, LOW);
//     digitalWrite(DIG_ONES, HIGH);
//     displayDigit(ones);
//     delay(4);

//     // SW2 자기 자신 → 동작3
//     if (digitalRead(SW2) == LOW) {
//       performAction3(count);
//     }
//     // SW1 → 동작1 즉시 전환
//     if (digitalRead(SW1) == LOW) { performAction1(); return; }
//     // SW3 → 초기화
//     if (digitalRead(SW3) == LOW) { performInitProcess(); return; }
//   }

//   setRGB(false, true, false); // G ON 유지
//   currentNumber = 0;
//   int tens = 0, ones = 0;
//   while (true) {
//     digitalWrite(DIG_TENS, HIGH);
//     digitalWrite(DIG_ONES, LOW);
//     displayDigit(tens);
//     delay(4);
//     digitalWrite(DIG_TENS, LOW);
//     digitalWrite(DIG_ONES, HIGH);
//     displayDigit(ones);
//     delay(4);
//     if (digitalRead(SW3) == LOW) { performInitProcess(); return; }
//   }
// }

// // ---------------- setup ----------------
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
//   performInitProcess();
// }

// // ---------------- loop ----------------
// void loop() {
//   // 기본 상태 표시
//   displayNumber(currentNumber / 10, currentNumber % 10, 10);

//   // SW3: 초기화
//   bool sw3_now = digitalRead(SW3);
//   if (lastSW3 == LOW && sw3_now == HIGH) performInitProcess();
//   lastSW3 = sw3_now;

//   // SW1: 동작1
//   bool sw1_now = digitalRead(SW1);
//   if (lastSW1 == LOW && sw1_now == HIGH) performAction1();
//   lastSW1 = sw1_now;

//   // SW2: 동작2
//   bool sw2_now = digitalRead(SW2);
//   if (lastSW2 == LOW && sw2_now == HIGH) performAction2();
//   lastSW2 = sw2_now;
// }
