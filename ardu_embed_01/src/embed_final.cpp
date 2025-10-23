// #include <Arduino.h>
// // 동작1-99, LED 유지 / 동작2-0, LED 유지
// // ===================================================
// // 🧩 [1] 전역 상수 정의
// // ===================================================
// #define SW1 2
// #define SW2 3
// #define SW3 4

// #define LED_R 5
// #define LED_G 6
// #define LED_B 7

// #define BCD_A 8
// #define BCD_B 9
// #define BCD_C 10
// #define BCD_D 11

// #define DIG_TENS 13
// #define DIG_ONES 12

// // ===================================================
// // 🧩 [2] 전역 변수 정의
// // ===================================================
// bool isInitProcess = false;   // 초기화 또는 동작3 수행 중 여부
// bool lastSW1 = HIGH;          // 스위치 상태 기억 (엣지 검출용)
// bool lastSW2 = HIGH;
// bool lastSW3 = HIGH;
// int currentNumber = 0;        // 현재 표시중인 숫자

// // ===================================================
// // 🧩 [3] 함수 선언부
// // ===================================================

// // 🔹 FND 관련
// void displayDigit(int n);
// void displayNumber(int tens, int ones, unsigned long time_ms);
// void clearFND();

// // 🔹 RGB 제어
// void setRGB(bool r, bool g, bool b);

// // 🔹 전체 동작 함수
// void performInitProcess();
// void performAction1();
// void performAction2();
// void performAction3(int savedNumber);

// // ===================================================
// // 🧩 [4] 함수 정의부
// // ===================================================

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
//     if (isInitProcess) { digitalRead(SW1); digitalRead(SW2); digitalRead(SW3); }

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
//   // 공통 애노드형 (VCC 공통 → LOW = ON)
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

//   // ▶ FND “88” → 소등 2회
//   displayNumber(8, 8, 1000);
//   clearFND(); delay(1000);
//   displayNumber(8, 8, 1000);
//   clearFND(); delay(1000);

//   // ▶ “00” 표시 유지하며 RGB 순차점등
//   unsigned long tStart = millis();

//   // R 1초
//   tStart = millis(); setRGB(true, false, false);
//   while (millis() - tStart < 1000) displayNumber(0, 0, 10);

//   // G 1초
//   tStart = millis(); setRGB(false, true, false);
//   while (millis() - tStart < 1000) displayNumber(0, 0, 10);

//   // B 1초
//   tStart = millis(); setRGB(false, false, true);
//   while (millis() - tStart < 1000) displayNumber(0, 0, 10);

//   // OFF 1초
//   tStart = millis(); setRGB(false, false, false);
//   while (millis() - tStart < 1000) displayNumber(0, 0, 10);

//   // ▶ 마지막 상태: “00” + R ON 유지
//   setRGB(true, false, false);
//   currentNumber = 0;
//   isInitProcess = false;
// }

// // ---------------- 동작 1 ----------------
// void performAction1() {
//   if (isInitProcess) return;

//   // ▶ “88” 2초 표시 + G/R 점등
//   displayNumber(8, 8, 2000);
//   setRGB(false, true, false); delay(500);
//   setRGB(true, false, false); delay(500);

//   bool bState = false;
//   unsigned long prevBTime = millis(), prevCountTime = millis();
//   int count = 0;
//   digitalWrite(LED_R, HIGH);
//   digitalWrite(LED_G, HIGH);

//   while (count <= 99) {
//     unsigned long now = millis();

//     // ▶ B LED 0.5초 ON/OFF
//     if (now - prevBTime >= 500) {
//       bState = !bState;
//       digitalWrite(LED_B, bState ? LOW : HIGH);
//       prevBTime = now;
//     }
//     // ▶ 0.5초 마다 숫자 증가
//     if (now - prevCountTime >= 500) { count++; prevCountTime = now; }

//     int tens = count / 10;
//     int ones = count % 10;
//     digitalWrite(DIG_TENS, HIGH); digitalWrite(DIG_ONES, LOW);
//     displayDigit(tens); delay(4);
//     digitalWrite(DIG_TENS, LOW); digitalWrite(DIG_ONES, HIGH);
//     displayDigit(ones); delay(4);

//     // ▶ SW1 → 동작3 일시정지
//     if (digitalRead(SW1) == LOW) performAction3(count);

//     // ▶ SW2 → 동작2 전환
//     if (digitalRead(SW2) == LOW) { performAction2(); return; }
//     // ▶ SW3 → 초기화 전환
//     if (digitalRead(SW3) == LOW) { performInitProcess(); return; }
//   }

//   // ▶ 종료: B LED ON 유지, 숫자 99
//   digitalWrite(LED_R, HIGH);
//   digitalWrite(LED_G, HIGH);
//   digitalWrite(LED_B, LOW);
//   currentNumber = 99;
// }

// // ---------------- 동작 2 ----------------
// void performAction2() {
//   if (isInitProcess) return;  // 초기화 중에는 실행 금지

//   // 1️⃣ “88” 2초 표시 + RGB B→R 전환
//   displayNumber(8, 8, 2000);
//   setRGB(false, false, true); delay(500); // B ON
//   setRGB(true, false, false); delay(500); // R ON

//   // 2️⃣ FND 카운트다운(99→0) + G 0.5초 ON/OFF 반복
//   bool gState = false;
//   unsigned long prevGTime = millis();
//   unsigned long prevCountTime = millis();
//   int count = 99;

//   // R, B OFF로 확보
//   digitalWrite(LED_R, HIGH);
//   digitalWrite(LED_B, HIGH);

//   while (count >= 0) {
//     unsigned long now = millis();

//     // ---- G LED 0.5초 ON/OFF 토글 ----
//     if (now - prevGTime >= 500) {
//       gState = !gState;
//       digitalWrite(LED_G, gState ? LOW : HIGH); // 공통 애노드 → LOW=ON
//       prevGTime = now;
//     }

//     // ---- FND 0.5초마다 1씩 감소 ----
//     if (now - prevCountTime >= 500) {
//       count--;
//       prevCountTime = now;
//     }

//     // ---- FND 현재 숫자 표시 ----
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

//     // ---- [동작3] 자기 자신 버튼 눌리면 일시정지 ----
//     if (digitalRead(SW2) == LOW) {
//       performAction3(count);
//     }

//     // ---- [동작1] 전환 ----
//     if (digitalRead(SW1) == LOW) {
//       performAction1();
//       return;
//     }

//     // ---- [초기화] ----
//     if (digitalRead(SW3) == LOW) {
//       performInitProcess();
//       return;
//     }
//   }

//   // 3️⃣ 카운트 완료 후 “00” 표시 + G LED ON 유지
//   setRGB(false, true, false);  // G ON (공통 애노드 → LOW=ON)
//   currentNumber = 0;

//   // ✅ loop()로 복귀 → 다른 스위치 입력 바로 감지 가능
//   return;
// }

// // ---------------- 동작 3 (일시정지) ----------------
// void performAction3(int savedNumber) {
//   isInitProcess = true;  // 동작3 수행 중 모든 스위치 입력 무시
//   int tens = savedNumber / 10;
//   int ones = savedNumber % 10;

//   for (int i = 0; i < 2; i++) {
//     // ▶ FND + R LED 2초 점등
//     setRGB(true, false, false); // R ON
//     unsigned long onTimer = millis() + 2000;
//     while (millis() < onTimer) {
//       digitalWrite(DIG_TENS, HIGH);
//       digitalWrite(DIG_ONES, LOW);
//       displayDigit(tens);
//       delay(4);
//       digitalWrite(DIG_TENS, LOW);
//       digitalWrite(DIG_ONES, HIGH);
//       displayDigit(ones);
//       delay(4);
//     }

//     // ▶ 2초 소등
//     setRGB(false, false, false);
//     unsigned long offTimer = millis() + 2000;
//     while (millis() < offTimer) {
//       clearFND();
//       delay(10);
//     }
//   }
//   isInitProcess = false;  // 정상 복귀
// }

// // ===================================================
// // 🧩 [5] setup / loop
// // ===================================================
// void setup() {
//   pinMode(SW1, INPUT); 
//   pinMode(SW2, INPUT); 
//   pinMode(SW3, INPUT);

//   pinMode(LED_R, OUTPUT); 
//   pinMode(LED_G, OUTPUT); 
//   pinMode(LED_B, OUTPUT);

//   pinMode(BCD_A, OUTPUT); 
//   pinMode(BCD_B, OUTPUT);
//   pinMode(BCD_C, OUTPUT); 
//   pinMode(BCD_D, OUTPUT);

//   pinMode(DIG_TENS, OUTPUT); 
//   pinMode(DIG_ONES, OUTPUT);
  
//   performInitProcess();
// }

// void loop() {
//   displayNumber(currentNumber / 10, currentNumber % 10, 10);

//   // ▶ SW3: 초기화
//   bool sw3_now = digitalRead(SW3);
//   if (lastSW3 == LOW && sw3_now == HIGH) performInitProcess();
//   lastSW3 = sw3_now;

//   // ▶ SW1: 동작1
//   bool sw1_now = digitalRead(SW1);
//   if (lastSW1 == LOW && sw1_now == HIGH) performAction1();
//   lastSW1 = sw1_now;

//   // ▶ SW2: 동작2
//   bool sw2_now = digitalRead(SW2);
//   if (lastSW2 == LOW && sw2_now == HIGH) performAction2();
//   lastSW2 = sw2_now;
// }