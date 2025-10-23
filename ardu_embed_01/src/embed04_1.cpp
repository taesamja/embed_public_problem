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

// bool isInitProcess = false;   // 초기화 중 여부
// bool lastSW1 = HIGH;
// bool lastSW2 = HIGH;
// bool lastSW3 = HIGH;
// int currentNumber = 0;        // 표시용 전역 변수

// // ---------------- FND 표시 함수 ----------------
// void displayDigit(int n) {
//   digitalWrite(BCD_A, n & 0x01);
//   digitalWrite(BCD_B, (n >> 1) & 0x01);
//   digitalWrite(BCD_C, (n >> 2) & 0x01);
//   digitalWrite(BCD_D, (n >> 3) & 0x01);
// }

// void displayNumber(int tens, int ones, unsigned long ms) {
//   unsigned long endT = millis() + ms;
//   while (millis() < endT) {
//     if (isInitProcess) {  // 초기화 중엔 스위치 입력 무시
//       digitalRead(SW1); digitalRead(SW2); digitalRead(SW3);
//     }
//     // 십의 자리
//     digitalWrite(DIG_TENS, HIGH);
//     digitalWrite(DIG_ONES, LOW);
//     displayDigit(tens);
//     delay(4);
//     // 일의 자리
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

// void setRGB(bool r, bool g, bool b) {
//   // 공통 애노드형 → LOW 켜짐
//   digitalWrite(LED_R, r ? LOW : HIGH);
//   digitalWrite(LED_G, g ? LOW : HIGH);
//   digitalWrite(LED_B, b ? LOW : HIGH);
// }

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

//   // RGB 순차점등 후 R 유지
//   displayNumber(0, 0, 1000);
//   setRGB(true, false, false); delay(1000);
//   setRGB(false, true, false); delay(1000);
//   setRGB(false, false, true); delay(1000);
//   setRGB(false, false, false); delay(1000);
//   setRGB(true, false, false); // R 유지

//   currentNumber = 0;
//   isInitProcess = false;
// }

// // ---------------- 동작 1 ----------------
// void performAction2(); // 먼저 선언해서 순환 참조 해결

// void performAction1() {
//   if (isInitProcess) return;

//   // 1️⃣ “88” 2초 표시 + RGB G→R
//   displayNumber(8, 8, 2000);
//   setRGB(false, true, false); delay(500); // G ON
//   setRGB(true, false, false); delay(500); // R ON

//   // 2️⃣ FND 카운트업 + B 0.5초 ON/OFF
//   bool bState = false;
//   unsigned long prevBTime = millis();
//   unsigned long prevCountTime = millis();
//   int count = 0;
//   digitalWrite(LED_R, HIGH);
//   digitalWrite(LED_G, HIGH);

//   while (count <= 99) {
//     unsigned long now = millis();

//     // B LED 0.5초 ON/OFF
//     if (now - prevBTime >= 500) {
//       bState = !bState;
//       digitalWrite(LED_B, bState ? LOW : HIGH);
//       prevBTime = now;
//     }

//     // 카운트 0.5초마다 증가
//     if (now - prevCountTime >= 500) {
//       count++;
//       prevCountTime = now;
//     }

//     // FND 표시
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

//     // SW2 → 동작 2 즉시 전환
//     if (digitalRead(SW2) == LOW) { performAction2(); return; }
//     // SW3 → 초기화
//     if (digitalRead(SW3) == LOW) { performInitProcess(); return; }
//   }

//   // 3️⃣ 99 표시 + B ON 유지
//   digitalWrite(LED_R, HIGH);
//   digitalWrite(LED_G, HIGH);
//   digitalWrite(LED_B, LOW);
//   currentNumber = 99;
// }

// // ---------------- 동작 2 ----------------
// void performAction2() {
//   if (isInitProcess) return;

//   // 1️⃣ “88” 2초 표시 + B→R 전환
//   displayNumber(8, 8, 2000);
//   setRGB(false, false, true); delay(500); // B ON
//   setRGB(true, false, false); delay(500); // R ON

//   // 2️⃣ FND 카운트다운 + G 0.5초 ON/OFF
//   bool gState = false;
//   unsigned long prevGTime = millis();
//   unsigned long prevCountTime = millis();
//   int count = 99;
//   digitalWrite(LED_R, HIGH);
//   digitalWrite(LED_B, HIGH);

//   while (count >= 0) {
//     unsigned long now = millis();

//     // G LED 0.5초 ON/OFF
//     if (now - prevGTime >= 500) {
//       gState = !gState;
//       digitalWrite(LED_G, gState ? LOW : HIGH);
//       prevGTime = now;
//     }

//     // 0.5초마다 숫자 감소
//     if (now - prevCountTime >= 500) {
//       count--;
//       prevCountTime = now;
//     }

//     // FND 표시
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

//     // SW1 → 동작 1 즉시 전환
//     if (digitalRead(SW1) == LOW) { performAction1(); return; }
//     // SW3 → 초기화
//     if (digitalRead(SW3) == LOW) { performInitProcess(); return; }
//   }

//   // 3️⃣ 00 표시 + G ON 유지
//   setRGB(false, true, false);
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
//     // SW3 → 초기화
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

//   performInitProcess();  // 전원 인가 시 초기화 실행
// }

// // ---------------- loop ----------------
// void loop() {
//   // 기본 표시 유지
//   displayNumber(currentNumber / 10, currentNumber % 10, 10);

//   // SW3: 눌렀다 뗄 때 초기화
//   bool sw3_now = digitalRead(SW3);
//   if (lastSW3 == LOW && sw3_now == HIGH) performInitProcess();
//   lastSW3 = sw3_now;

//   // SW1: 눌렀다 뗄 때 동작 1 수행
//   bool sw1_now = digitalRead(SW1);
//   if (lastSW1 == LOW && sw1_now == HIGH) performAction1();
//   lastSW1 = sw1_now;

//   // SW2: 눌렀다 뗄 때 동작 2 수행
//   bool sw2_now = digitalRead(SW2);
//   if (lastSW2 == LOW && sw2_now == HIGH) performAction2();
//   lastSW2 = sw2_now;
// }
