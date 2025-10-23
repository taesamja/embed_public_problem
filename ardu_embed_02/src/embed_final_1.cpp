#include <Arduino.h>

// 초기화: 88 ON/OFF×2 → FND=50 유지, RGB: (G 1s → B 1s)×2 후 R 점등
// 동작1: SW1 롱프레스(≥1s) 동안 0.5s 간격 감소 + B 점멸, SW1 release로 종료(현재값 유지+R)
//       00 도달 후 계속 1s 유지 → 동작3
// 동작2: SW2 롱프레스(≥1s) 동안 0.5s 간격 증가 + G 점멸, SW2 release로 종료(현재값 유지+R)
//       99 도달 후 계속 1s 유지 → 동작3
// 동작3: (00 1s → 99 1s)×2, RGB: R 1s → G 1s → B 1s → 1s OFF, 완료 후 FND=50, R 점등

// =====================[ 핀 매핑 ]=====================
// - 시험지에서는 핀을 수험자가 선택하도록 되어 있으므로 임의 매핑을 사용합니다.
#define SW1 2
#define SW2 3
#define SW3 4

#define LED_R 5   // 공통 애노드 RGB (LOW = ON)
#define LED_G 6
#define LED_B 7

#define BCD_A 8   // 74LS47 입력 A~D
#define BCD_B 9
#define BCD_C 10
#define BCD_D 11

#define DIG_TENS 13  // 2자리 FND 자릿수 선택 (공통 애노드, HIGH=ON 가정)
#define DIG_ONES 12

// =====================[ 전역 상태 ]====================
// - isBusy: 초기화/동작3 수행 중(true)이면 스위치 입력을 무시(요구사항 반영).
// - currentNumber: 현재 FND 표시값(초기화 완료 후 50 유지).
volatile bool isBusy = false;    
int currentNumber = 50;          

bool lastSW1 = HIGH, lastSW2 = HIGH, lastSW3 = HIGH;
unsigned long pressStart1 = 0;
unsigned long pressStart2 = 0;
unsigned long pressStart3 = 0;

// =====================[ 함수 선언부 ]===================
void setRGB(bool r, bool g, bool b);
void displayDigit(int n);
void clearFND(void);
void displayNumber(int tens, int ones, unsigned long ms);
void showCurrent(unsigned long ms);

void performInitProcess(void);
void performAction1(void);
void performAction2(void);
void performAction3(void);

// =====================[ 유틸/표시 함수 ]================
// 공통 애노드 RGB: LOW=ON, HIGH=OFF
inline void setRGB(bool r, bool g, bool b) {
  digitalWrite(LED_R, r ? LOW : HIGH);
  digitalWrite(LED_G, g ? LOW : HIGH);
  digitalWrite(LED_B, b ? LOW : HIGH);
}

// 7세그 1자리 BCD 출력(0~9)
inline void displayDigit(int n){
  digitalWrite(BCD_A, n & 0x01);
  digitalWrite(BCD_B, (n>>1) & 0x01);
  digitalWrite(BCD_C, (n>>2) & 0x01);
  digitalWrite(BCD_D, (n>>3) & 0x01);
}

// 두 자리 완전 소등(잔상 제거)
inline void clearFND(){
  digitalWrite(DIG_TENS, LOW);
  digitalWrite(DIG_ONES, LOW);
}

// 두 자리 숫자를 ms 동안 시분할 표시
// - 각 자리 약 3ms씩 번갈아 점등 → 총 ms 동안 유지
void displayNumber(int tens, int ones, unsigned long ms){
  unsigned long endT = millis() + ms;
  while (millis() < endT){
    // 십의 자리
    digitalWrite(DIG_TENS, HIGH); 
    digitalWrite(DIG_ONES, LOW);
    displayDigit(tens); delay(3);
    // 일의 자리
    digitalWrite(DIG_TENS, LOW);  
    digitalWrite(DIG_ONES, HIGH);
    displayDigit(ones); delay(3);
  }
  clearFND();
}

// 현재 currentNumber를 짧게 표시(메인 루프용 유지표시)
inline void showCurrent(unsigned long ms){
  displayNumber(currentNumber/10, currentNumber%10, ms);
}

// =====================[ 초기화 ]======================
// 요구사항: 88 1s ON/1s OFF ×2 → FND=50 표시,
// RGB: (G 1s → B 1s)×2 → R ON, 수행 중 스위치 무시, 완료 후 50 + R 유지.
void performInitProcess(){
  isBusy = true;                       // 스위치 무시
  clearFND(); setRGB(false,false,false);
  delay(100);

  // FND: 88 ON/OFF ×2
  displayNumber(8,8,1000); clearFND(); delay(1000);
  displayNumber(8,8,1000); clearFND(); delay(1000);

  // FND=50 유지하며 RGB (G,B)×2
  unsigned long t;
  t=millis(); setRGB(false,true,false);  while(millis()-t<1000) displayNumber(5,0,8);
  t=millis(); setRGB(false,false,true);  while(millis()-t<1000) displayNumber(5,0,8);
  t=millis(); setRGB(false,true,false);  while(millis()-t<1000) displayNumber(5,0,8);
  t=millis(); setRGB(false,false,true);  while(millis()-t<1000) displayNumber(5,0,8);

  // 초기화 상태: 50 + R ON 유지
  currentNumber = 50;
  setRGB(true,false,false);
  isBusy = false;
}

// =====================[ 동작 1 ]======================
// 요구사항: SW1을 1초 이상 누르는 동안 0.5s 간격으로 1씩 감소, RGB B 0.5s 점멸.
// 진행 중 SW2·SW3 무시, SW1을 떼면 종료(현재값 유지+R).
// 00 도달 후 1초 더 누르면 동작3 수행.
void performAction1(){
  if(isBusy) return;                   // 초기화/동작3 중이면 진입 금지

  bool bOn=false;
  unsigned long prevBlink=millis();    // B 점멸 타이머
  unsigned long prevTick=millis();     // 0.5s 카운트 타이머
  int n = currentNumber;               // 현재값에서 시작
  unsigned long reachedZeroAt = 0;     // 00 도달 시각(0=미도달)

  // R/G는 OFF, B만 점멸
  digitalWrite(LED_R, HIGH); 
  digitalWrite(LED_G, HIGH); 
  digitalWrite(LED_B, HIGH);

  while(true){
    unsigned long now = millis();

    // SW1 release → 즉시 종료(현재값 유지 + R 켜기)
    if(digitalRead(SW1)==HIGH){
      currentNumber = n;
      setRGB(true,false,false);
      return;
    }

    // B 0.5초 토글
    if(now - prevBlink >= 500){
      bOn = !bOn;
      digitalWrite(LED_B, bOn ? LOW:HIGH);
      prevBlink = now;
    }

    // 0.5s마다 1 감소 (최소 0까지)
    if(now - prevTick >= 500){
      if(n > 0) n -= 1;
      if(n == 0 && reachedZeroAt==0) reachedZeroAt = now; // 00에 처음 도달
      prevTick = now;
    }

    // 00 상태에서 1초 추가로 계속 누르고 있으면 동작3
    if(n==0 && reachedZeroAt>0 && (now - reachedZeroAt >= 1000)){
      currentNumber = 0;
      performAction3();                // 완료 후 50 + R 상태
      return;
    }

    // 현재 숫자 시분할 표시
    int tens = n/10, ones = n%10;

    digitalWrite(DIG_TENS, HIGH); 
    digitalWrite(DIG_ONES, LOW);  
    displayDigit(tens); delay(3);
    
    digitalWrite(DIG_TENS, LOW);  
    digitalWrite(DIG_ONES, HIGH); 
    displayDigit(ones); delay(3);
  }
}

// =====================[ 동작 2 ]======================
// 요구사항: SW2를 1초 이상 누르는 동안 0.5s 간격으로 1씩 증가, RGB G 0.5s 점멸.
// 진행 중 SW1·SW3 무시, SW2를 떼면 종료(현재값 유지+R).
// 99 도달 후 1초 더 누르면 동작3 수행.
void performAction2(){
  if(isBusy) return;

  bool gOn=false;
  unsigned long prevBlink=millis();    // G 점멸 타이머
  unsigned long prevTick=millis();     // 0.5s 카운트 타이머
  int n = currentNumber;
  unsigned long reached99At = 0;       // 99 도달 시각(0=미도달)

  // R/B는 OFF, G만 점멸
  digitalWrite(LED_R, HIGH); 
  digitalWrite(LED_B, HIGH); 
  digitalWrite(LED_G, HIGH);

  while(true){
    unsigned long now = millis();

    // SW2 release → 즉시 종료(현재값 유지 + R 켜기)
    if(digitalRead(SW2)==HIGH){
      currentNumber = n;
      setRGB(true,false,false);
      return;
    }

    // G 0.5초 토글
    if(now - prevBlink >= 500){
      gOn = !gOn;
      digitalWrite(LED_G, gOn ? LOW:HIGH);
      prevBlink = now;
    }

    // 0.5s마다 1 증가 (최대 99까지)
    if(now - prevTick >= 500){
      if(n < 99) n += 1;
      if(n == 99 && reached99At==0) reached99At = now; // 99에 처음 도달
      prevTick = now;
    }

    // 99 상태에서 1초 추가로 계속 누르고 있으면 동작3
    if(n==99 && reached99At>0 && (now - reached99At >= 1000)){
      currentNumber = 99;
      performAction3();                // 완료 후 50 + R 상태
      return;
    }

    // 현재 숫자 시분할 표시
    int tens = n/10, ones = n%10;

    digitalWrite(DIG_TENS, HIGH); 
    digitalWrite(DIG_ONES, LOW);  
    displayDigit(tens); delay(3);
    
    digitalWrite(DIG_TENS, LOW);  
    digitalWrite(DIG_ONES, HIGH); 
    displayDigit(ones); delay(3);
  }
}

// =====================[ 동작 3 ]======================
// 요구사항: FND가 00일 때 SW1≥1s 또는 99일 때 SW2≥1s →
// FND: 00 1s, 99 1s ×2, RGB: R 1s → G 1s → B 1s → 1s OFF.
// 완료 후 초기화 상태(50, R ON)로 복귀. 수행 중 스위치 무시.
void performAction3(){
  isBusy = true;  // 스위치 무시

  // 4구간(각 1초): 0:R + 00, 1:G + 99, 2:B + 00, 3:OFF + 99
  for(int step=0; step<4; ++step){
    // RGB 설정
    if(step==0) setRGB(true,false,false);     // R
    else if(step==1) setRGB(false,true,false);// G
    else if(step==2) setRGB(false,false,true);// B
    else setRGB(false,false,false);           // OFF

    // FND 표시(짝수 스텝=00, 홀수 스텝=99)
    int tens = (step%2==0)? 0 : 9;
    int ones = (step%2==0)? 0 : 9;
    displayNumber(tens, ones, 1000);
  }

  // 완료 후 초기화 상태로
  currentNumber = 50;
  setRGB(true,false,false);
  isBusy = false;
}

// =====================[ setup / loop ]================
void setup(){
  // 스위치: 내부 풀업 사용(눌림=LOW)
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);

  // RGB/BCD/FND 출력
  pinMode(LED_R, OUTPUT); 
  pinMode(LED_G, OUTPUT); 
  pinMode(LED_B, OUTPUT);

  pinMode(BCD_A, OUTPUT); 
  pinMode(BCD_B, OUTPUT); 
  pinMode(BCD_C, OUTPUT); 
  pinMode(BCD_D, OUTPUT);
  
  pinMode(DIG_TENS, OUTPUT); 
  pinMode(DIG_ONES, OUTPUT);

  // 초기 표시 정리 후 초기화 시퀀스
  setRGB(false,false,false); 
  clearFND();
  performInitProcess();                  // 전원 인가 시 초기화
}

void loop(){
  // 기본 표시 유지(초기화 상태 등)
  showCurrent(8);

  // 현재 스위치 상태(엣지 검출 및 롱프레스 판정용)
  bool s1 = digitalRead(SW1);
  bool s2 = digitalRead(SW2);
  bool s3 = digitalRead(SW3);

  // SW3: 언제든 눌렀다 떼면 초기화(롱프레스 요구 없음), 단 isBusy면 무시
  if(lastSW3==LOW && s3==HIGH && !isBusy){
    performInitProcess();
  }

  // 롱프레스 타이밍 기록 및 진입 판단
  unsigned long now = millis();
  if(!isBusy){
    // SW1 롱프레스: FND==00이면 동작3 우선, 아니면 동작1
    if(s1==LOW){
      if(pressStart1==0) pressStart1 = now;
      if(now - pressStart1 >= 1000){
        if(currentNumber==0) performAction3();
        else                 performAction1();
        pressStart1 = 0; // 처리 후 리셋
      }
    }else pressStart1 = 0;

    // SW2 롱프레스: FND==99이면 동작3 우선, 아니면 동작2
    if(s2==LOW){
      if(pressStart2==0) pressStart2 = now;
      if(now - pressStart2 >= 1000){
        if(currentNumber==99) performAction3();
        else                  performAction2();
        pressStart2 = 0;
      }
    }else pressStart2 = 0;
  }

  lastSW1 = s1; lastSW2 = s2; lastSW3 = s3;
}
