
#include "ArduSched.h"   // Arduino Scheduler

//definition for onboard LED on Arduino Mega 2560
#define OUT_LED 13

#define ARD_SCHED_VERSION   300

#ifndef ARDUSCHED_V
 #define ARDUSCHED_V  300
#endif
#if ARDUSCHED_V >= 400 
extern unsigned long lulMillis;    // [ms] milli seconds since boot of �C (the millis() function and
extern unsigned long rulMillisElapsed;              // [ms] time elapsed since last scheduler timer tick
#endif

void setup() {
  // put your setup code here, to run once:
  ArduSchedInit();
  pinMode(LED_BUILTIN, OUTPUT);
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
  
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduSchedHandler();
}

void Task1(){//configured with 100ms interval (inside ArduSched.h)
}

void Task2(){//configured with 250ms interval (inside ArduSched.h)
  delay(250-240);
}

void Task3(){//configured with 1000ms interval (inside ArduSched.h)
#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  digitalWrite(OUT_Task3, HIGH); 
#endif
  //insert code or function to call here:
  digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
  Serial.println(millis());
#if ARDUSCHED_V >= 400 
  Serial.println(lulMillis);
  Serial.println(rulMillisElapsed);
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

#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  //delay(1);                              //emulation of task processing time; should be replaced with program code
  digitalWrite(OUT_Task3, LOW);
#endif
}
/*void Task4(){
  //insert code or function to call here:
}
void Task5(){
  //insert code or function to call here:
}
void Task6(){
  //insert code or function to call here:
}
void Task7(){
  //insert code or function to call here:
}
void Task8(){
  //insert code or function to call here:
}
void Task9(){
  //insert code or function to call here:
}*/

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

