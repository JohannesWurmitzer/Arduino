#include "Arduino.h"
#include "ArdTools.h"
#include "ArdDan.h"

int iLoopCnt;
char strText[49];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ArdDanSetup();
  sprintf(strText,"Lunzer ECU IPS");
  DanWrite(0,0,strText);
  sprintf(strText,"Version XYZ");
  DanWrite(0,1,strText);
  DanWrite(8,1,"1.00");
  iLoopCnt = 0;

}

void loop() {
  unsigned long ulTimeStart, ulTimeStop;
  // put your main code here, to run repeatedly:
  iLoopCnt++;
  if (iLoopCnt >= 1000){
    ulTimeStart = micros();
    ArdDanTask();
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
