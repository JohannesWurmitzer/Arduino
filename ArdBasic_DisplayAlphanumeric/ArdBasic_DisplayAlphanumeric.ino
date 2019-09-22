#include "ArdTools.h"
#include "ArdDan.h"
#include "Arduino.h"

int iLoopCnt;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ArdDanSetup();
  iLoopCnt = 0;

}

void loop() {
  unsigned long ulTimeStart, ulTimeStop;
  // put your main code here, to run repeatedly:
  iLoopCnt++;
  if (iLoopCnt >= 1000){
    ulTimeStart = micros();
    void ArdDanTask();
    ulTimeStop = micros();
    iLoopCnt = 0;
    Serial.print("ArdDanTask: ");
    Serial.print(tools_ulSubRollOver(ulTimeStop, ulTimeStart));
    Serial.println(" us");
  } 
  else{
    ArdDanLoop();
  }
  delay(1);
}
