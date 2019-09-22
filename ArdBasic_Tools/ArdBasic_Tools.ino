
#include "ArdTools.h"

unsigned long ulTimerValueOld;
unsigned long ulTimerValueNew;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  ulTimerValueOld = 0-10;
  ulTimerValueNew = 10;
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(ulTimerValueNew++);
  Serial.print("-");
  Serial.print(ulTimerValueOld++);
  Serial.print("=");
  Serial.println(tools_ulSubRollOver(ulTimerValueNew, ulTimerValueOld));
  delay(1000);
}
