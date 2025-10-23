#include <Arduino.h>
// 동작1-99(00-99-00-99-...) 반복, LED 반복 / 동작2-0(99-00-99-00-...) 반복, LED 반복

// ===================================================
// [1] 전역 상수 (핀 정의)
//  - 스위치 3개(SW1, SW2, SW3)
//  - 공통 애노드 RGB LED: R(5), G(6), B(7)
//  - BCD to 7-Segment(74LS47) 입력: A(8)~D(11)
//  - 두 자리 FND 자릿수 선택: DIG_TENS(13), DIG_ONES(12)
// ===================================================
#define SW1 2
#define SW2 3
#define SW3 4

#define LED_R 5
#define LED_G 6
#define LED_B 7

#define BCD_A 8
#define BCD_B 9
#define BCD_C 10
#define BCD_D 11

#define DIG_TENS 13
#define DIG_ONES 12

// ===================================================
// [2] 전역 변수
//  - isInitProcess: 초기화/동작3 수행 중 스위치 무시 플래그
//  - lastSWx: loop()에서 "눌렀다 떼는 순간(LOW→HIGH)" 엣지 검출용
//  - currentNumber: 초기화 상태 등 기본 표시 값(예: 00) 유지
// ===================================================
bool isInitProcess = false;     // 초기화 중 또는 동작3 수행 중이면 true
bool lastSW1 = HIGH, lastSW2 = HIGH, lastSW3 = HIGH; // loop용 Edge 검출
int currentNumber = 0;          // 현재 FND 출력 값

// ===================================================
// [3] 함수 선언부
//  - 표시/제어 공통 함수 + 초기화/동작1/동작2/동작3
// ===================================================
void displayDigit(int n);
void displayNumber(int tens, int ones, unsigned long ms);
void clearFND();
void setRGB(bool r, bool g, bool b);
void performInitProcess();
void performAction1();
void performAction2();
void performAction3(int savedNumber);

// ===================================================
// [4] 기본 표시 함수
// ===================================================

// 7세그 한 자리(0~9)의 BCD 코드를 74LS47에 출력
void displayDigit(int n) {
  digitalWrite(BCD_A, n & 0x01);
  digitalWrite(BCD_B, (n >> 1) & 0x01);
  digitalWrite(BCD_C, (n >> 2) & 0x01);
  digitalWrite(BCD_D, (n >> 3) & 0x01);
}

// 두 자리 숫자를 시분할로 ms 시간 동안 표시
// - 공통 애노드 FND에서 자릿수 선택(여기서는 HIGH가 ON으로 사용) + 74LS47 BCD 출력
// - isInitProcess=true일 때 스위치 읽기는 하지만 무시(부하 방지용 더미 read)
void displayNumber(int tens, int ones, unsigned long ms) {
  unsigned long endT = millis() + ms;
  while (millis() < endT) {
    if (isInitProcess) { digitalRead(SW1); digitalRead(SW2); digitalRead(SW3); }

    // 십의 자리 표시
    digitalWrite(DIG_TENS, HIGH); 
    digitalWrite(DIG_ONES, LOW);
    displayDigit(tens); delay(4);

    // 일의 자리 표시
    digitalWrite(DIG_TENS, LOW);  
    digitalWrite(DIG_ONES, HIGH);
    displayDigit(ones); delay(4);
  }
  
  // 시분할 종료 후 두 자리 OFF로 잔상 제거
  digitalWrite(DIG_TENS, LOW); 
  digitalWrite(DIG_ONES, LOW);
}

// 두 자릿수 완전 소등
void clearFND() {
  digitalWrite(DIG_TENS, LOW); 
  digitalWrite(DIG_ONES, LOW);
}

// 공통 애노드 RGB: LOW=ON, HIGH=OFF
//  - r/g/b를 true로 넣으면 해당 색 LOW(점등), false면 HIGH(소등)
void setRGB(bool r, bool g, bool b) {
  digitalWrite(LED_R, r ? LOW : HIGH);
  digitalWrite(LED_G, g ? LOW : HIGH);
  digitalWrite(LED_B, b ? LOW : HIGH);
}

// ===================================================
// [5] 동작 3 (일시정지)
//  - 동작1 중 SW1 release, 동작2 중 SW2 release로 진입
//  - 현재 FND 숫자와 RGB R을 2초 ON/2초 OFF × 2회
//  - 수행 중(isInitProcess=true) 스위치 입력 무시
//  - 완료 후(isInitProcess=false) 원래 동작으로 복귀
// ===================================================
void performAction3(int savedNumber) {
  isInitProcess = true;  // 스위치 무효화

  int tens = savedNumber / 10;
  int ones = savedNumber % 10;

  for (int i = 0; i < 2; i++) {
    // ▶ R + 현재 숫자 2초 점등
    setRGB(true, false, false);
    unsigned long onEnd = millis() + 2000;
    while (millis() < onEnd) {
      digitalWrite(DIG_TENS, HIGH); 
      digitalWrite(DIG_ONES, LOW);
      displayDigit(tens); delay(4);
      
      digitalWrite(DIG_TENS, LOW);  
      digitalWrite(DIG_ONES, HIGH);
      displayDigit(ones); delay(4);
    }

    // ▶ 2초 소등 (FND/RGB)
    setRGB(false, false, false);
    unsigned long offEnd = millis() + 2000;
    while (millis() < offEnd) {
      clearFND(); delay(10);
    }
  }

  isInitProcess = false;  // 원래 동작 재개 (동작1/2 내부 루프 계속 진행)
}

// ===================================================
// [6] 초기화 동작
//  - 88 1초 ON/1초 OFF ×2 → “00” 표시 유지하며 R→G→B 각 1초 → 1초 OFF → R 켜기
//  - 초기화 중(isInitProcess=true)에는 모든 스위치 입력 무시
// ===================================================
void performInitProcess() {
  isInitProcess = true;
  clearFND(); setRGB(false, false, false);
  delay(500);

  // 88 → OFF ×2
  displayNumber(8, 8, 1000); clearFND(); delay(1000);
  displayNumber(8, 8, 1000); clearFND(); delay(1000);

  // “00” 표시 + RGB R→G→B 순차 1초씩 + 1초 소등 (표시 유지 위해 displayNumber 반복)
  unsigned long t;
  t = millis(); setRGB(true,  false, false); while (millis()-t <1000) displayNumber(0,0,10);
  t = millis(); setRGB(false, true,  false); while (millis()-t <1000) displayNumber(0,0,10);
  t = millis(); setRGB(false, false,  true); while (millis()-t <1000) displayNumber(0,0,10);
  t = millis(); setRGB(false, false, false); while (millis()-t <1000) displayNumber(0,0,10);

  // 초기화 완료 상태: R ON + “00” 유지
  setRGB(true, false, false);
  currentNumber = 0;
  isInitProcess = false;
}

// ===================================================
// [7] 동작 1 (FND 증가 반복, RGB B 점멸)
//  - 진입: 초기화 상태 또는 동작2 수행 중 SW1 release
//  - 88 2초 → G 0.5 → R 0.5 → (B 0.5 점멸 + 00→99 증가 0.5초 간격) 무한 반복
//  - 진행 중 SW1 release → 동작3(일시정지), SW2 release → 동작2 즉시 전환, SW3 release → 초기화
// ===================================================
void performAction1() {
  if (isInitProcess) return;
  // 내부 루프에서도 "눌렀다 떼는 순간"을 잡기 위해 이전 상태(pSWx) 보관
  bool pSW1=digitalRead(SW1), pSW2=digitalRead(SW2), pSW3=digitalRead(SW3);

  // ▶ 88 2초
  displayNumber(8,8,2000);

  // ▶ 88 유지하며 G 0.5 → R 0.5 (표시 유지 위해 displayNumber 반복)
  unsigned long tEnd = millis()+500; 
  setRGB(false,true,false);
  while(millis()<tEnd){displayNumber(8,8,10);}
  
  tEnd=millis()+500; 
  setRGB(true,false,false);
  while(millis()<tEnd){displayNumber(8,8,10);}

  // ▶ B 점멸 + 00→99 카운트업 반복
  bool bOn=false; 
  unsigned long prevBlink=millis(), prevCount=millis(); 
  int count=0;
  // 다른 색 OFF 확보
  digitalWrite(LED_R,HIGH); 
  digitalWrite(LED_G,HIGH);

  while(true){
    unsigned long now=millis();

    // B 0.5초 토글
    if(now-prevBlink>=500){
        bOn=!bOn; 
        digitalWrite(LED_B,bOn?LOW:HIGH); 
        prevBlink=now;
    }

    // 0.5초마다 00→99 순환 증가
    if(now-prevCount>=500){
        count=(count+1)%100; 
        prevCount=now;
    }

    // 현재 숫자 시분할 표시
    int tens=count/10, ones=count%10;
    digitalWrite(DIG_TENS,HIGH);
    digitalWrite(DIG_ONES,LOW); 
    displayDigit(tens); delay(4);
    
    digitalWrite(DIG_TENS,LOW); 
    digitalWrite(DIG_ONES,HIGH); 
    displayDigit(ones); delay(4);

    // 내부 엣지 검출: SW1(일시정지), SW2(전환), SW3(초기화)
    bool n1=digitalRead(SW1), n2=digitalRead(SW2), n3=digitalRead(SW3);
    if(pSW1==LOW && n1==HIGH) performAction3(count);      // 동작3 수행 후 재개
    if(pSW2==LOW && n2==HIGH){ performAction2(); return;} // 동작2로 즉시 전환
    if(pSW3==LOW && n3==HIGH){ performInitProcess(); return;} // 초기화
    pSW1=n1; pSW2=n2; pSW3=n3;
  }
}

// ===================================================
// [8] 동작 2 (FND 감소 반복, RGB G 점멸)
//  - 진입: 초기화 상태 또는 동작1 수행 중 SW2 release
//  - 88 2초 → B 0.5 → R 0.5 → (G 0.5 점멸 + 99→00 감소 0.5초 간격) 무한 반복
//  - 진행 중 SW2 release → 동작3(일시정지), SW1 release → 동작1 즉시 전환, SW3 release → 초기화
// ===================================================
void performAction2() {
  if (isInitProcess) return;
  bool pSW1=digitalRead(SW1), pSW2=digitalRead(SW2), pSW3=digitalRead(SW3);

  // ▶ 88 2초
  displayNumber(8,8,2000);

  // ▶ 88 유지하며 B 0.5 → R 0.5
  unsigned long tEnd=millis()+500; 
  setRGB(false,false,true);
  while(millis()<tEnd){displayNumber(8,8,10);}
  
  tEnd=millis()+500; 
  setRGB(true,false,false);
  while(millis()<tEnd){displayNumber(8,8,10);}

  // ▶ G 점멸 + 99→00 카운트다운 반복
  bool gOn=false; 
  unsigned long prevBlink=millis(), prevCount=millis(); 
  int count=99;
  
  // 다른 색 OFF 확보
  digitalWrite(LED_R,HIGH); 
  digitalWrite(LED_B,HIGH);

  while(true){
    unsigned long now=millis();

    // G 0.5초 토글
    if(now-prevBlink>=500){
        gOn=!gOn; 
        digitalWrite(LED_G,gOn?LOW:HIGH); 
        prevBlink=now;
    }

    // 0.5초마다 99→00 순환 감소
    if(now-prevCount>=500){
        count=(count==0)?99:(count-1); 
        prevCount=now;
    }

    // 현재 숫자 시분할 표시
    int tens=count/10, ones=count%10;
    digitalWrite(DIG_TENS,HIGH);
    digitalWrite(DIG_ONES,LOW); 
    displayDigit(tens); delay(4);
    
    digitalWrite(DIG_TENS,LOW); 
    digitalWrite(DIG_ONES,HIGH); 
    displayDigit(ones); delay(4);

    // 내부 엣지 검출: SW2(일시정지), SW1(전환), SW3(초기화)
    bool n1=digitalRead(SW1), n2=digitalRead(SW2), n3=digitalRead(SW3);
    if(pSW2==LOW && n2==HIGH) performAction3(count);      // 동작3 수행 후 재개
    if(pSW1==LOW && n1==HIGH){ performAction1(); return;} // 동작1로 즉시 전환
    if(pSW3==LOW && n3==HIGH){ performInitProcess(); return;} // 초기화
    pSW1=n1; pSW2=n2; pSW3=n3;
  }
}

// ===================================================
// [9] setup / loop
// ===================================================
void setup() {
  // 스위치: 내부 풀업 사용(눌림=LOW)
  pinMode(SW1, INPUT_PULLUP); 
  pinMode(SW2, INPUT_PULLUP); 
  pinMode(SW3, INPUT_PULLUP);

  // RGB 및 74LS47/FND 출력
  pinMode(LED_R, OUTPUT); 
  pinMode(LED_G, OUTPUT); 
  pinMode(LED_B, OUTPUT);
  
  pinMode(BCD_A, OUTPUT); 
  pinMode(BCD_B, OUTPUT); 
  pinMode(BCD_C, OUTPUT); 
  pinMode(BCD_D, OUTPUT);
  
  pinMode(DIG_TENS, OUTPUT); 
  pinMode(DIG_ONES, OUTPUT);

  // 전원 인가 시 반드시 초기화 수행
  performInitProcess();
}

void loop() {
  // 기본 숫자 유지 표시(초기화 상태 등: currentNumber 사용)
  displayNumber(currentNumber/10, currentNumber%10, 10);

  // loop 레벨 "눌렀다 떼는 순간" 엣지 검출
  bool sw1=digitalRead(SW1);
  bool sw2=digitalRead(SW2);
  bool sw3=digitalRead(SW3);

  if(lastSW3==LOW&&sw3==HIGH) performInitProcess(); // 언제든 초기화
  if(lastSW1==LOW&&sw1==HIGH) performAction1();     // 초기화상태/동작2중 → 동작1
  if(lastSW2==LOW&&sw2==HIGH) performAction2();     // 초기화상태/동작1중 → 동작2

  lastSW1=sw1; lastSW2=sw2; lastSW3=sw3;
}