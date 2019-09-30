
#include "ArdSched.h"   // Arduino Scheduler

#define ARDSCHED_TEST

#ifndef ARDSCHED_V
 #define ARDSCHED_V  300
#endif
#if ARDSCHED_V >= 400 
extern unsigned long rulMillis;    // [ms] milli seconds since boot of µC (the millis() function and
extern unsigned long rulMillisElapsed;              // [ms] time elapsed since last scheduler timer tick
#endif
#define OUT_TMR3_TIMING_SIG 40

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(OUT_TMR3_TIMING_SIG, OUTPUT);

  // Arduino Scheduler init
  ArdSchedSetup();

  
#ifdef ARDSCHED_TEST
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

  // Arduino Scheduler Loop
  ArdSchedLoop();
  if (ArdSchedTaskRdyStart(TASK_1)){ Task1(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_2)){ Task2(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_3)){ Task3(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_4)){ Task4(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_5)){ Task5(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_6)){ Task6(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_7)){ Task7(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_8)){ Task8(); ArdSchedTaskStop(); }

}

void Task1(void){//configured with 100ms interval (inside ArduSched.h)
  delay(1);
}

void Task2(void){//configured with 250ms interval (inside ArduSched.h)
#ifdef ARDUSCHED_TEST
//  delay(250-245);
  delay(2);
#else
  delay(2);
#endif
}

void Task3(void){//configured with 1000ms interval (inside ArduSched.h)
  delay(3);
  //insert code or function to call here:
  digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
#ifdef ARDSCHED_TEST
  Serial.println("Task 3");
  Serial.println(millis());
 #if ARDSCHED_V >= 400 
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
}
void Task4(void){
  //insert code or function to call here:
#ifdef ARDSCHED_TEST
  Serial.println("Task 4");
#else
#endif
  delay(4);
}
void Task5(void){
  //insert code or function to call here:
#ifdef ARDSCHED_TEST
  Serial.println("Task 5");
#else
#endif
  delay(5);
}
void Task6(void){
  //insert code or function to call here:
#ifdef ARDSCHED_TEST
  Serial.println("Task 6");
#else
#endif
  delay(6);
}
void Task7(void){
  //insert code or function to call here:
#ifdef ARDSCHED_TEST
  Serial.println("Task 7");
#else
#endif
  delay(7);
}
void Task8(void){
  int idx;
  //insert code or function to call here:
#ifdef ARDSCHED_TEST
  Serial.println("Task 8");
  Serial.print("- times: ");
  for (idx = 0; idx < TASK_USED_NUM; idx ++){
    Serial.print(gaulTaskTime[idx]);
    Serial.print("µs ");
  }
  Serial.println();
#else
  delay(8);
#endif
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


/*

*/
