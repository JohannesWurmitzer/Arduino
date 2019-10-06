#include "bldc-2_protocols.h"

U_PROTOCOL gu_ProtData;    // union for the protocols

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(9600);
}

unsigned char raubProt_Stop[] = {0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC1};
unsigned char raubProt_Run[] = {0x40, 0x1, 0xF4, 0x0, 0x0, 0x0, 0x0, 0x4F};

unsigned char raubProt_6_Run[]  = {0x00, 0x01, 0xF4, 0x00, 0x00, 0xF5};   // Answer 0x00, 0x01, 0xF7, 0x06, 0xC8, 0xC6
unsigned char raubProt_6_Stop[] = {0x00, 0x00, 0x00, 0x00, 0x50, 0x50};   // Answer 0x00, 0x00, 0x00, 0x00, 0xC8, 0xC8



void loop() {
  // put your main code here, to run repeatedly:
  unsigned char i, cnt;
  unsigned char ubRxPC;
  bool lblOnOff;

  while (Serial.available()){
    ubRxPC = Serial.read();
    if (ubRxPC == '1'){
      lblOnOff = true;
    }
    else if (ubRxPC == '0'){
      lblOnOff = false;
    }
//    Serial.print("0x"); Serial.print(ubRxPC, HEX); Serial.print(", ");
  }

  
  while (Serial1.available()){
    ubRxPC = Serial1.read();
    Serial.print("0x"); Serial.print(ubRxPC, HEX); Serial.print(", ");
  }
  
  if (lblOnOff == false){
    for (cnt = 0; cnt < 20; cnt++){
      for (i = 0; i < sizeof(raubProt_6_Stop); i++){
        Serial1.write(raubProt_6_Stop[i]);
      }
      delay(50);
      while (Serial1.available()){
        ubRxPC = Serial1.read();
        Serial.print("0x"); Serial.print(ubRxPC, HEX); Serial.print(", ");
      }
      delay(50);
    }
  }  
  
 if (lblOnOff == true){
  
    for (cnt = 0; cnt < 20; cnt++){
      for (i = 0; i < sizeof(raubProt_6_Run); i++){
        Serial1.write(raubProt_6_Run[i]);
      }
      delay(50);
    
      while (Serial1.available()){
        ubRxPC = Serial1.read();
        Serial.print("0x"); Serial.print(ubRxPC, HEX); Serial.print(", ");
      }
      delay(50);
    }
  }  
}
