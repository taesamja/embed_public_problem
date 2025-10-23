// #include <Arduino.h>
// // 초기화: 88 ON/OFF×2 → FND=50 유지, RGB: (G 1s → B 1s)×2 후 R 점등
// // 동작1: SW1 롱프레스(≥1s) 동안 0.5s 간격 감소 + B 점멸, SW1 release로 종료(현재값 유지+R)
// //       00 도달 후 계속 1s 유지 → 동작3
// // 동작2: SW2 롱프레스(≥1s) 동안 0.5s 간격 증가 + G 점멸, SW2 release로 종료(현재값 유지+R)
// //       99 도달 후 계속 1s 유지 → 동작3
// // 동작3: (00 1s → 99 1s)×2, RGB: R 1s → G 1s → B 1s → 1s OFF, 완료 후 FND=50, R 점등

// // =====================[ 핀 매핑 ]=====================
// #define SW1 2
// #define SW2 3
// #define SW3 4

// #define LED_R 5   // 공통 애노드 RGB (LOW = ON)
// #define LED_G 6
// #define LED_B 7

// #define BCD_A 8   // 74LS47 입력 A~D
// #define BCD_B 9
// #define BCD_C 10
// #define BCD_D 11

// #define DIG_TENS 13  // FND 자릿수 선택 (공통 애노드, HIGH=ON 가정)
// #define DIG_ONES 12

// // =====================[ 전역 상태 ]====================
// volatile bool isBusy = false;    // 초기화/동작3 등 스위치 무시 중
// int currentNumber = 50;          // 현재 표시 값(초기화 완료 후 50)
// bool lastSW1 = HIGH, lastSW2 = HIGH, lastSW3 = HIGH;

// unsigned long pressStart1 = 0;
// unsigned long pressStart2 = 0;
// unsigned long pressStart3 = 0;

// // 함수 선언부
// void setRGB(bool r, bool g, bool b);
// void displayDigit(int n);
// void clearFND(void);
// void displayNumber(int tens, int ones, unsigned long ms);
// void showCurrent(unsigned long ms);

// void performInitProcess(void);
// void performAction1(void);
// void performAction2(void);
// void performAction3(void);

// // =====================[ 유틸/표시 함수 ]================
// inline void setRGB(bool r, bool g, bool b) {
//   digitalWrite(LED_R, r ? LOW : HIGH);
//   digitalWrite(LED_G, g ? LOW : HIGH);
//   digitalWrite(LED_B, b ? LOW : HIGH);
// }

// inline void displayDigit(int n){
//   digitalWrite(BCD_A, n & 0x01);
//   digitalWrite(BCD_B, (n>>1) & 0x01);
//   digitalWrite(BCD_C, (n>>2) & 0x01);
//   digitalWrite(BCD_D, (n>>3) & 0x01);
// }

// inline void clearFND(){
//   digitalWrite(DIG_TENS, LOW);
//   digitalWrite(DIG_ONES, LOW);
// }

// // 두 자리 숫자를 ms 동안 시분할 표시 (isBusy일 때도 내부에서만 스위치 읽기)
// void displayNumber(int tens, int ones, unsigned long ms){
//   unsigned long endT = millis() + ms;
//   while (millis() < endT){
//     // 십의 자리
//     digitalWrite(DIG_TENS, HIGH); 
//     digitalWrite(DIG_ONES, LOW);
//     displayDigit(tens); delay(3);
//     // 일의 자리
//     digitalWrite(DIG_TENS, LOW);  
//     digitalWrite(DIG_ONES, HIGH);
//     displayDigit(ones); delay(3);
//   }
//   clearFND();
// }

// // 현재 currentNumber를 아주 짧게 유지 표시 (loop용)
// inline void showCurrent(unsigned long ms){
//   displayNumber(currentNumber/10, currentNumber%10, ms);
// }

// // =====================[ 초기화 ]======================
// // 88 1s ON/1s OFF ×2 → FND=50 유지
// // RGB: (G 1s → B 1s)×2 → R ON
// void performInitProcess(){
//   isBusy = true;

//   clearFND(); setRGB(false,false,false);
//   delay(100);

//   // FND: 88 ON/OFF ×2
//   displayNumber(8,8,1000); clearFND(); delay(1000);
//   displayNumber(8,8,1000); clearFND(); delay(1000);

//   // FND=50 유지하며 RGB 시퀀스 (G,B)×2
//   unsigned long t;
//   t=millis(); setRGB(false,true,false);  while(millis()-t<1000) displayNumber(5,0,8);
//   t=millis(); setRGB(false,false,true);  while(millis()-t<1000) displayNumber(5,0,8);
//   t=millis(); setRGB(false,true,false);  while(millis()-t<1000) displayNumber(5,0,8);
//   t=millis(); setRGB(false,false,true);  while(millis()-t<1000) displayNumber(5,0,8);

//   // 초기화 상태
//   currentNumber = 50;
//   setRGB(true,false,false);
//   isBusy = false;
// }

// // =====================[ 동작 1 ]======================
// // SW1을 누르고 있는 동안 수행. 0.5초 간격 감소 + B 점멸.
// // 00 도달 후 계속 1초 유지 → 동작3. SW1 release → 종료(R 유지).
// void performAction1(){
//   if(isBusy) return;
//   // 다른 스위치 입력은 명시적으로 무시
//   bool bOn=false;
//   unsigned long prevBlink=millis();
//   unsigned long prevTick=millis();
//   int n = currentNumber; // 시작값에서 진행

//   unsigned long reachedZeroAt = 0; // 00 도달 시각(0이면 아직 아님)

//   // R/G는 소등, B만 점멸
//   digitalWrite(LED_R, HIGH); digitalWrite(LED_G, HIGH); digitalWrite(LED_B, HIGH);

//   while(true){
//     unsigned long now = millis();

//     // SW1 상태 확인 (다른 스위치는 무시)
//     if(digitalRead(SW1)==HIGH){ // release → 종료
//       currentNumber = n;
//       setRGB(true,false,false);
//       return;
//     }

//     // B 0.5초 토글
//     if(now - prevBlink >= 500){
//       bOn = !bOn;
//       digitalWrite(LED_B, bOn ? LOW:HIGH);
//       prevBlink = now;
//     }

//     // 카운트 다운(0.5s)
//     if(now - prevTick >= 500){
//       if(n > 0) n -= 1;
//       // 00 도달 시각 기록
//       if(n == 0 && reachedZeroAt==0) reachedZeroAt = now;
//       prevTick = now;
//     }

//     // 00에서 1초 추가 유지 시 동작3
//     if(n==0 && reachedZeroAt>0 && (now - reachedZeroAt >= 1000)){
//       currentNumber = 0;
//       performAction3();
//       return; // 초기화 상태로 복귀됨
//     }

//     // 현재 표시
//     int tens = n/10, ones = n%10;

//     digitalWrite(DIG_TENS, HIGH); 
//     digitalWrite(DIG_ONES, LOW); 
//     displayDigit(tens); delay(3);

//     digitalWrite(DIG_TENS, LOW);  
//     digitalWrite(DIG_ONES, HIGH); 
//     displayDigit(ones); delay(3);
//   }
// }

// // =====================[ 동작 2 ]======================
// // SW2을 누르고 있는 동안 수행. 0.5초 간격 증가 + G 점멸.
// // 99 도달 후 계속 1초 유지 → 동작3. SW2 release → 종료(R 유지).
// void performAction2(){
//   if(isBusy) return;
//   bool gOn=false;
//   unsigned long prevBlink=millis();
//   unsigned long prevTick=millis();
//   int n = currentNumber;

//   unsigned long reached99At = 0;

//   // R/B는 소등, G만 점멸
//   digitalWrite(LED_R, HIGH); digitalWrite(LED_B, HIGH); digitalWrite(LED_G, HIGH);

//   while(true){
//     unsigned long now = millis();

//     // SW2 상태 확인
//     if(digitalRead(SW2)==HIGH){ // release → 종료
//       currentNumber = n;
//       setRGB(true,false,false);
//       return;
//     }

//     // G 0.5초 토글
//     if(now - prevBlink >= 500){
//       gOn = !gOn;
//       digitalWrite(LED_G, gOn ? LOW:HIGH);
//       prevBlink = now;
//     }

//     // 카운트 업(0.5s)
//     if(now - prevTick >= 500){
//       if(n < 99) n += 1;
//       if(n == 99 && reached99At==0) reached99At = now;
//       prevTick = now;
//     }

//     // 99에서 1초 추가 유지 시 동작3
//     if(n==99 && reached99At>0 && (now - reached99At >= 1000)){
//       currentNumber = 99;
//       performAction3();
//       return; // 초기화 상태로 복귀됨
//     }

//     // 현재 표시
//     int tens = n/10, ones = n%10;

//     digitalWrite(DIG_TENS, HIGH); 
//     digitalWrite(DIG_ONES, LOW); 
//     displayDigit(tens); delay(3);

//     digitalWrite(DIG_TENS, LOW);  
//     digitalWrite(DIG_ONES, HIGH); 
//     displayDigit(ones); delay(3);
//   }
// }

// // =====================[ 동작 3 ]======================
// // (00↔99) 1초씩 ×2, RGB: R→G→B→OFF 각 1초, 완료 후 초기화 상태(50, R ON)
// void performAction3(){
//   isBusy = true;

//   // 4구간(각 1초): RGB 스텝 0:R, 1:G, 2:B, 3:OFF
//   // FND는 00,99,00,99 순서로 1초씩
//   for(int step=0; step<4; ++step){
//     // RGB 설정
//     if(step==0) setRGB(true,false,false);
//     else if(step==1) setRGB(false,true,false);
//     else if(step==2) setRGB(false,false,true);
//     else setRGB(false,false,false);

//     // FND 표시
//     int tens = (step%2==0)? 0 : 9;
//     int ones = (step%2==0)? 0 : 9;
//     displayNumber(tens, ones, 1000);
//   }

//   // 완료 후 초기화 상태로
//   currentNumber = 50;
//   setRGB(true,false,false);
//   isBusy = false;
// }

// // =====================[ setup / loop ]================
// void setup(){
//   pinMode(SW1, INPUT_PULLUP);
//   pinMode(SW2, INPUT_PULLUP);
//   pinMode(SW3, INPUT_PULLUP);

//   pinMode(LED_R, OUTPUT); 
//   pinMode(LED_G, OUTPUT); 
//   pinMode(LED_B, OUTPUT);

//   pinMode(BCD_A, OUTPUT); 
//   pinMode(BCD_B, OUTPUT); 
//   pinMode(BCD_C, OUTPUT); 
//   pinMode(BCD_D, OUTPUT);

//   pinMode(DIG_TENS, OUTPUT); 
//   pinMode(DIG_ONES, OUTPUT);

//   setRGB(false,false,false); 
//   clearFND();

//   performInitProcess(); // 전원 인가 시 초기화
// }

// void loop(){
//   // 기본 표시 유지(초기화 상태 등)
//   showCurrent(8);

//   // 현재 스위치 상태
//   bool s1 = digitalRead(SW1);
//   bool s2 = digitalRead(SW2);
//   bool s3 = digitalRead(SW3);

//   // SW3: 언제든 눌렀다 떼면 초기화(롱프레스 요구 없음)
//   if(lastSW3==LOW && s3==HIGH && !isBusy){
//     performInitProcess();
//   }

//   // 롱프레스 시작/유지 타이밍 기록
//   unsigned long now = millis();
//   if(!isBusy){
//     // SW1
//     if(s1==LOW){
//       if(pressStart1==0) pressStart1 = now;
//       // FND==00이면 동작3 우선
//       if(now - pressStart1 >= 1000){
//         if(currentNumber==0){
//           performAction3();
//         }else{
//           performAction1();
//         }
//         pressStart1 = 0; // 처리 후 리셋
//       }
//     }else pressStart1 = 0;

//     // SW2
//     if(s2==LOW){
//       if(pressStart2==0) pressStart2 = now;
//       // FND==99이면 동작3 우선
//       if(now - pressStart2 >= 1000){
//         if(currentNumber==99){
//           performAction3();
//         }else{
//           performAction2();
//         }
//         pressStart2 = 0;
//       }
//     }else pressStart2 = 0;
//   }

//   lastSW1 = s1; lastSW2 = s2; lastSW3 = s3;
// }
