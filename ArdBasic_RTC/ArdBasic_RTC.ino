//#include "RTClib.h"
//#include <Wire.h>
#include "Uhrzeit.h"      // only for old ITB and PTP


void setup() {
  // put your setup code here, to run once:
  unsigned long rulMicros;
  Serial.begin(115200);
  Serial.println("ArdBasic_RTC");
  rulMicros = micros();
  UHR_Init();
  rulMicros = micros() - rulMicros;
  Serial.print("UHR_Init() ");Serial.print(rulMicros); Serial.println(" µs");
}

void loop() {
  // put your main code here, to run repeatedly:
  String lstring;
  unsigned long rulMicros;
  rulMicros = micros();
  lstring = UHR_Lesen();
  rulMicros = micros() - rulMicros;
  Serial.print(lstring);
  Serial.print(" - UHR_Lesen() ");Serial.print(rulMicros); Serial.println(" µs");

  rulMicros = micros();
  lstring = UHR_Logzeit();
  rulMicros = micros() - rulMicros;
  Serial.print(lstring);
  Serial.print(" - UHR_Logzeit() ");Serial.print(rulMicros); Serial.println(" µs");
  
  delay(1000);
}
