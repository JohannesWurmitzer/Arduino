#include "ArdSched.h"   // Arduino Scheduler
#include "ArdRFID_SL.h" // RFID Stronlink

//#define ARDSCHED_TEST

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.println("Setup Arduino Scheduler");
  digitalWrite(LED_BUILTIN, LOW);
  ArdSchedSetup();
  digitalWrite(LED_BUILTIN, HIGH);
  ArdRFID_SL_Setup();

#ifdef PN532_USED  
  ArdRFID_PN532_Setup();
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
  unsigned char i;
  unsigned char aubID[8+42];
  unsigned char ubIDLength;
  delay(1);
  Serial.println("SL030 i2c: Read");
  if (SL030readPassiveTargetID(aubID, &ubIDLength, 8)){
    Serial.print("SL030 i2c: Tag ");
    Serial.print(ubIDLength); Serial.print(" ");
    for (i=0; i<ubIDLength; i++){
      Serial.print(aubID[i],HEX); Serial.print(" ");
    }
    Serial.println();
  }
  else{
    Serial.println("SL030 i2c: No Tag detected");
  }
}

void Task2(void){//configured with 250ms interval (inside ArduSched.h)
#ifdef ARDUSCHED_TEST
#else
#endif
//  delay(2);
#ifdef PN532_USED  
  unsigned char i;
  unsigned char aubID[8+42];
  unsigned char ubIDLength;
//  ArdRFID_PN532_Setup();
  if (PN532nfc1readPassiveTargetID(&aubID[0], &ubIDLength, 50)){
    Serial.print("NFC532 1 UART: Tag ");
    Serial.print(ubIDLength); Serial.print(" ");
    for (i=0; i<ubIDLength; i++){
      Serial.print(aubID[i],HEX); Serial.print(" ");
    }
    Serial.println();
  }
  else{
    Serial.println("NFC532 1 UART: No Tag detected");
  }
#endif
}

void Task3(void){//configured with 1000ms interval (inside ArduSched.h)
//  delay(3);
  //insert code or function to call here:
  digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
#ifdef ARDSCHED_TEST
  Serial.println("Task 3");
#endif
#ifdef PN532_USED  
  unsigned char i;
  unsigned char aubID[8+42];
  unsigned char ubIDLength;
//  ArdRFID_PN532_Setup();
  if (PN532nfc2readPassiveTargetID(&aubID[0], &ubIDLength, 50)){
    Serial.print("NFC532 2 UART: Tag ");
    Serial.print(ubIDLength); Serial.print(" ");
    for (i=0; i<ubIDLength; i++){
      Serial.print(aubID[i],HEX); Serial.print(" ");
    }
    Serial.println();
  }
  else{
    Serial.println("NFC532 2 UART: No Tag detected");
  }
#endif
}
void Task4(void){
  //insert code or function to call here:
#ifdef ARDSCHED_TEST
  Serial.println("Task 4");
#else
#endif
//  delay(4);
  unsigned char i;
  unsigned char aubID[8+42];
  unsigned char ubIDLength;
  Serial.println("SL032 uart: Read");
  if (SL032readPassiveTargetID(aubID, &ubIDLength, 8)){
    Serial.print("SL032 uart: Tag ");
    Serial.print(ubIDLength); Serial.print(" ");
    for (i=0; i<ubIDLength; i++){
      Serial.print(aubID[i],HEX); Serial.print(" ");
    }
    Serial.println();
  }
  else{
    Serial.println("SL032 uart: No Tag detected");
  }


}
void Task5(void){
  //insert code or function to call here:
#ifdef ARDSCHED_TEST
  Serial.println("Task 5");
#else
#endif
//  delay(5);
#ifdef PN532_USED  
  ArdRFID_PN532_Setup();
#endif
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
  ArdRFID_SL_Setup();
}
void Task8(void){
  int idx;
  //insert code or function to call here:
#ifndef ARDSCHED_TEST
  Serial.println("Task 8");
  Serial.print("- times: ");
  for (idx = 0; idx < TASK_USED_NUM; idx ++){
    Serial.print(gaulTaskTime[idx]);
    gaulTaskTime[idx] = 0;
    Serial.print("µs ");
  }
  Serial.println();
  delay(6);
#else
  delay(8);
#endif
}
