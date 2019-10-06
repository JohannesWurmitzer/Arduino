#include "ArdComASC.h"

const String lstrVER = String("ITB1_120_A");       // Softwareversion


void setup() {
  // put your setup code here, to run once:
  ArdComASC_Setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  ArdComASC_Loop();



  
  delay(1000);
}
