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
// bool lastSW1 = HIGH;          // 스위치 이전 상태 저장용
// bool lastSW3 = HIGH;

// int currentNumber = 0;        // 표시값 또는 동작 제어용 변수

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
//     if (isInitProcess) {  // 초기화 중엔 SW 무시
//       digitalRead(SW1); digitalRead(SW2); digitalRead(SW3);
//     }
//     // 십의자리
//     digitalWrite(DIG_TENS, HIGH);
//     digitalWrite(DIG_ONES, LOW);
//     displayDigit(tens);
//     delay(4);
//     // 일의자리
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
//   // 공통 애노드형 기준 → LOW가 켜짐
//   digitalWrite(LED_R, r ? LOW : HIGH);
//   digitalWrite(LED_G, g ? LOW : HIGH);
//   digitalWrite(LED_B, b ? LOW : HIGH);
// }

// void rgbShowWhileFND(bool r, bool g, bool b, unsigned long time_ms) {
//   unsigned long endT = millis() + time_ms;
//   setRGB(r, g, b);  // 공통 애노드형이므로 LOW ON
//   while (millis() < endT) {
//     displayNumber(0, 0, 10);
//   }
// }

// // ---------------- 초기화 동작 ----------------
// void performInitProcess() {
//   isInitProcess = true;
//   clearFND();
//   setRGB(false, false, false);  // 전부 OFF
//   delay(500);

//   // FND 88 → 꺼짐 2회
//   displayNumber(8, 8, 1000);
//   clearFND(); delay(1000);
//   displayNumber(8, 8, 1000);
//   clearFND(); delay(1000);

//   // RGB 순차점등 후 R 유지
//   displayNumber(0, 0, 1000);
//   rgbShowWhileFND(true, false, false, 1000);   // R
//   rgbShowWhileFND(false, true, false, 1000);   // G
//   rgbShowWhileFND(false, false, true, 1000);   // B
//   rgbShowWhileFND(false, false, false, 1000);  // OFF
//   setRGB(true, false, false);                  // R 유지

//   currentNumber = 0;
//   isInitProcess = false;
// }

// // ---------------- 동작 1 ----------------
// void performAction1() {
//   if (isInitProcess) return; // 초기화 중 실행 금지

//   // 1️⃣ 88 2초 표시
//   displayNumber(8, 8, 2000);
//   setRGB(false, false, false);

//   // 2️⃣ G 0.5초
//   unsigned long gEnd = millis() + 500;
//   setRGB(false, true, false); // G ON (공통애노드 → LOW=ON)
//   while (millis() < gEnd) {
//     displayNumber(8, 8, 10);
//   }

//   // 3️⃣ R 0.5초
//   unsigned long rEnd = millis() + 500;
//   setRGB(true, false, false); // R ON
//   while (millis() < rEnd) {
//     displayNumber(8, 8, 10);
//   }

//   // 4️⃣ FND 카운트업 + B 0.5초 ON/OFF 반복
//   bool bState = false;
//   unsigned long prevBTime = millis();     // B 토글 타이머
//   unsigned long prevCountTime = millis(); // 숫자 증가 타이머
//   int count = 0;

//   // R,G OFF로 확보
//   digitalWrite(LED_R, HIGH);
//   digitalWrite(LED_G, HIGH);

//   while (count <= 99) {
//     unsigned long now = millis();

//     // ---- B 0.5초 ON/OFF 토글 ----
//     if (now - prevBTime >= 500) {
//       bState = !bState;
//       digitalWrite(LED_B, bState ? LOW : HIGH); // LOW=ON
//       prevBTime = now;
//     }

//     // ---- 숫자 0.5초마다 1씩 증가 ----
//     if (now - prevCountTime >= 500) {
//       count++;
//       prevCountTime = now;
//     }

//     // ---- 현재 숫자 표시 ----
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

//     // ---- SW3 눌리면 즉시 초기화 ----
//     if (digitalRead(SW3) == LOW) {
//       performInitProcess();
//       return;
//     }
//   }

//   // 5️⃣ FND 99 유지 + B LED 계속 깜빡임 유지
//   int tens = 9, ones = 9;
//   prevBTime = millis();
//   bState = false;

//   while (true) {
//     unsigned long now = millis();

//     // // ---- B 0.5초 간격으로 계속 깜빡임 ----
//     // if (now - prevBTime >= 500) {
//     //   bState = !bState;
//     //   digitalWrite(LED_B, bState ? LOW : HIGH); // LOW=ON
//     //   prevBTime = now;
//     // }

//     // 🔹 다른 LED OFF, B LED ON 유지 (공통 애노드형 → LOW=ON)
//     digitalWrite(LED_R, HIGH);
//     digitalWrite(LED_G, HIGH);
//     digitalWrite(LED_B, LOW);

//     // ---- FND 99 계속 표시 ----
//     digitalWrite(DIG_TENS, HIGH);
//     digitalWrite(DIG_ONES, LOW);
//     displayDigit(tens);
//     delay(4);
//     digitalWrite(DIG_TENS, LOW);
//     digitalWrite(DIG_ONES, HIGH);
//     displayDigit(ones);
//     delay(4);

//     // SW3 눌리면 초기화
//     if (digitalRead(SW3) == LOW) {
//       performInitProcess();
//       return;
//     }
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

//   performInitProcess();  // 전원 인가 시 자동 초기화 1회 실행
// }

// // ---------------- loop ----------------
// void loop() {
//   // 항상 현재 숫자 유지 표시
//   displayNumber(currentNumber / 10, currentNumber % 10, 10);

//   // SW3: 눌렀다 뗄 때 초기화
//   bool sw3_now = digitalRead(SW3);
//   if (lastSW3 == LOW && sw3_now == HIGH) performInitProcess();
//   lastSW3 = sw3_now;

//   // SW1: 눌렀다 뗄 때 동작 1 수행
//   bool sw1_now = digitalRead(SW1);
//   if (lastSW1 == LOW && sw1_now == HIGH) performAction1();
//   lastSW1 = sw1_now;
// }
