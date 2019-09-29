#include "Arduino.h"

#define ARDUINO_DUE_ADC_12BIT
#define PO_S11  4
#define PO_S12  5
#define PO_S13  6
#define PO_S14  7
#define PO_S21  8
#define PO_S22  9
#define PO_S23  10
#define PO_S24  11

#define AI_S11  A0

#define AI_S11_IDX  7
#define AI_S12_IDX  6
#define AI_S13_IDX  5
#define AI_S14_IDX  4
#define AI_S21_IDX  3
#define AI_S22_IDX  2
#define AI_S23_IDX  1
#define AI_S24_IDX  0

#define S11_REF_RES 2198      // [Ohm] S11  Reference Resistor
#define S12_REF_RES 2192
#define S13_REF_RES 2194
#define S14_REF_RES 2212
#define S21_REF_RES 2221
#define S22_REF_RES 2192
#define S23_REF_RES 2196
#define S24_REF_RES 2191

#ifdef ARDUINO_DUE_ADC_12BIT
 #define ADC_MAX   4095                // [dig] ADC max value
#else
 #define ADC_MAX   1023                // [dig] ADC max value
#endif
#define ADC_NUM   8                   // amount of digital inputs
#define ADC_AVG   16                  // averaging values

const char gaucSegText[8][4] = {"S11", "S12", "S13", "S14", "S21", "S22", "S23", "S24"};

const unsigned short cgausRefRes[8] = {S11_REF_RES, S12_REF_RES, S13_REF_RES, S14_REF_RES, S21_REF_RES, S22_REF_RES, S23_REF_RES, S24_REF_RES};
const unsigned char cgachDigPoIdx[8] = {PO_S11, PO_S12, PO_S13, PO_S14, PO_S21, PO_S22, PO_S23, PO_S24};
const unsigned char cgausAnaIdx[ADC_NUM] = {AI_S11_IDX, AI_S12_IDX, AI_S13_IDX, AI_S14_IDX, AI_S21_IDX, AI_S22_IDX, AI_S23_IDX, AI_S24_IDX};
unsigned short gausAnaIn[ADC_NUM];      // [dig 10] analog inputs
unsigned short gausAnaInSc[ADC_NUM];    // [dig 16] analog inputs scaled
unsigned short gausResistance[8];       // [Ohm/10] 
unsigned short gausResistanceAmb[8];    // [Ohm/10]
unsigned short gausResistanceDelta[8];  // [Ohm/10]
void adcAIupdate(void);


void setup() {
  // put your setup code here, to run once:
#ifdef ARDUINO_DUE_ADC_12BIT
  analogReadResolution(12);
#endif
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.write("Widerstandsmessung V1.00\n");
  
  pinMode(PO_S11, OUTPUT);
  pinMode(PO_S12, OUTPUT);
  pinMode(PO_S13, OUTPUT);
  pinMode(PO_S14, OUTPUT);
  pinMode(PO_S21, OUTPUT);
  pinMode(PO_S22, OUTPUT);
  pinMode(PO_S23, OUTPUT);
  pinMode(PO_S24, OUTPUT);
}

void loop() {
  char lstr[81];
  char idx;
  // put your main code here, to run repeatedly:

  digitalWrite(PO_S11, 1);
  delay(5);
  adcAIupdate();
//  digitalWrite(PO_S11, 0);
  for (idx = 0; idx < 8; idx++){
    Serial.print(gaucSegText[idx]); Serial.print("_AI = "); Serial.print(gausAnaIn[idx]); Serial.print(" ");
    Serial.print(gaucSegText[idx]); Serial.print("_U = "); Serial.print(gausAnaInSc[idx]); Serial.print(" ");
    Serial.print(gaucSegText[idx]); Serial.print("_R = "); Serial.print(gausResistance[idx]);
    Serial.print(" Delta = "); Serial.print(gausResistanceDelta[idx]);
    Serial.println();
  }
  sprintf(lstr,"%3d -%3d -%3d -%3d", gausResistanceDelta[0] + gausResistanceDelta[7], gausResistanceDelta[1] + gausResistanceDelta[7], gausResistanceDelta[2] + gausResistanceDelta[7], gausResistanceDelta[3] + gausResistanceDelta[7]); Serial.println(lstr);
  sprintf(lstr,"%3d -%3d -%3d -%3d", gausResistanceDelta[0] + gausResistanceDelta[6], gausResistanceDelta[1] + gausResistanceDelta[6], gausResistanceDelta[2] + gausResistanceDelta[6], gausResistanceDelta[3] + gausResistanceDelta[6]); Serial.println(lstr);
  sprintf(lstr,"%3d -%3d -%3d -%3d", gausResistanceDelta[0] + gausResistanceDelta[5], gausResistanceDelta[1] + gausResistanceDelta[5], gausResistanceDelta[2] + gausResistanceDelta[5], gausResistanceDelta[3] + gausResistanceDelta[5]); Serial.println(lstr);
  sprintf(lstr,"%3d -%3d -%3d -%3d", gausResistanceDelta[0] + gausResistanceDelta[4], gausResistanceDelta[1] + gausResistanceDelta[4], gausResistanceDelta[2] + gausResistanceDelta[4], gausResistanceDelta[3] + gausResistanceDelta[4]); Serial.println(lstr);
  Serial.println();
  delay(995);
}

void adcAIupdate(void){
  byte idx;
  byte avgCnt;
  unsigned short usADCValue;

  for (idx = 0; idx < ADC_NUM; idx++){
//    Serial.println(idx);
    digitalWrite(cgachDigPoIdx[idx], 1);
    delay(5);
    usADCValue = 0;
    for (avgCnt = 0; avgCnt < ADC_AVG; avgCnt++){
      usADCValue += analogRead(cgausAnaIdx[idx]);
    }
    digitalWrite(cgachDigPoIdx[idx], 0);
    gausAnaIn[idx] = (usADCValue + ADC_AVG/2) / ADC_AVG;    // [dig 10] analog inputs

    switch(idx){
      case AI_S11_IDX:
      case AI_S12_IDX:
      case AI_S13_IDX:
      case AI_S14_IDX:
      case AI_S21_IDX:
      case AI_S22_IDX:
      case AI_S23_IDX:
      case AI_S24_IDX:
        gausAnaInSc[idx] = ((unsigned long)gausAnaIn[idx] * 3300 + ADC_MAX/2) / ADC_MAX; break;  // [V/100] analog inputs scaled // [A/10] Hallsensor Current Measurement 1
      default: break;
    }
    
    gausResistance[idx] = ((unsigned long)cgausRefRes[idx] * gausAnaIn[idx] + (ADC_MAX - gausAnaIn[idx])/2) / (ADC_MAX - gausAnaIn[idx]);
    if (gausResistanceAmb[idx] == 0){
      gausResistanceAmb[idx] = gausResistance[idx];
    }
    if (gausResistance[idx] > gausResistanceAmb[idx]){
      gausResistanceDelta[idx] = gausResistance[idx] - gausResistanceAmb[idx];
    }
    else{
      gausResistanceDelta[idx] = 0;
    }
  }  
}
