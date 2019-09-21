
#include "ArduSched.h"   // Arduino Scheduler

#define ARDUSCHED_TEST

#ifndef ARDUSCHED_V
 #define ARDUSCHED_V  300
#endif
#if ARDUSCHED_V >= 400 
extern unsigned long rulMillis;    // [ms] milli seconds since boot of µC (the millis() function and
extern unsigned long rulMillisElapsed;              // [ms] time elapsed since last scheduler timer tick
#endif
#define OUT_TMR3_TIMING_SIG 40

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(OUT_TMR3_TIMING_SIG, OUTPUT);
  ArduSchedInit();
#ifdef ARDUSCHED_TEST
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);

  Serial.begin(115200);  
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduSchedHandler();
}

void Task1(void){//configured with 100ms interval (inside ArduSched.h)
  delay(1);
}

void Task2(void){//configured with 250ms interval (inside ArduSched.h)
#ifdef ARDUSCHED_TEST
  delay(250-245);
#endif
}

void Task3(void){//configured with 1000ms interval (inside ArduSched.h)
  delay(1);
  //insert code or function to call here:
  digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
#ifdef ARDUSCHED_TEST
  Serial.println(millis());
 #if ARDUSCHED_V >= 400 
  Serial.println(rulMillis);
  Serial.println(rulMillisElapsed);
 #endif
#endif
  //beeper demo..................
  //Beeper(BEEP_DETECT_TAG);
  //Beeper(BEEP_UNKNOWN_ID);
  //Beeper(BEEP_TEACH_USER);
  //Beeper(BEEP_TEACH_USER_END);
  //Beeper(BEEP_TEACH_ARTICLE);
  //Beeper(BEEP_TEACH_ARTICLE_END);
  //Beeper(BEEP_ERASE_IDS);
  //Beeper(BEEP_ERASE_IDS_END);
  //.............................
  
  //LED demo..................
  //OkLedSet(LED_CONST_OFF);
  //ErrorLedSet(LED_CONST_OFF);
  //OkLedSet(LED_CONST_ON);
  //ErrorLedSet(LED_CONST_ON);
  //OkLedSet(LED_TAG_CHECK);
  //ErrorLedSet(LED_TAG_CHECK);
  //OkLedSet(LED_TAG_OK);
  //ErrorLedSet(LED_TAG_OK);
  //OkLedSet(LED_TAG_NOK);
  //ErrorLedSet(LED_TAG_NOK);
  //OkLedSet(LED_TEACH_USER);
  //ErrorLedSet(LED_TEACH_USER);
  //OkLedSet(LED_TEACH_USER_END);
  //ErrorLedSet(LED_TEACH_USER_END);
  //OkLedSet(LED_TEACH_ARTICLE);
  //ErrorLedSet(LED_TEACH_ARTICLE);
  //OkLedSet(LED_TEACH_ARTICLE_END);
  //ErrorLedSet(LED_TEACH_ARTICLE_END);
  //OkLedSet(LED_ERASE_IDS);
  //ErrorLedSet(LED_ERASE_IDS);
  //OkLedSet(LED_ERASE_IDS_END);
  //ErrorLedSet(LED_ERASE_IDS_END);
  //OkLedSet(LED_SYSTEM_ERROR);
  //ErrorLedSet(LED_SYSTEM_ERROR);
  //.............................
  Serial.println("Task 3");
}
void Task4(void){
  //insert code or function to call here:
  Serial.println("Task 4");
  delay(1);
}
void Task5(void){
  //insert code or function to call here:
  Serial.println("Task 5");
  delay(1);
}
void Task6(void){
  //insert code or function to call here:
  Serial.println("Task 6");
  delay(1);
}
void Task7(void){
  //insert code or function to call here:
  Serial.println("Task 7");
  delay(1);
}
void Task8(void){
  //insert code or function to call here:
  Serial.println("Task 8");
  delay(1);
}

void Tmr3_ISR(){
#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  //digitalWrite(OUT_TMR3_TIMING_SIG, digitalRead(OUT_TMR3_TIMING_SIG) ^ 1); 
  digitalWrite(OUT_TMR3_TIMING_SIG, HIGH);
#endif

  //insert code or function to call here:
//  MotorLockHandler();//called all 50 or 100ms (depending on beeper frequency -> SWITCH2)
  
#ifdef EN_OUTPUT_TASKTEST_SIGNALS  
  //delayMicroseconds(1000);
  digitalWrite(OUT_TMR3_TIMING_SIG, LOW);
#endif
}
