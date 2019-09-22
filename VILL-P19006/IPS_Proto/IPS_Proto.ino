/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer
  All rights reserved.
*/
/*  Function description

  Sketch for Lunzer IPS (ice protection system) Prototype
  
*/
/*
  File Name:      Lunzer_IPS.ino
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     JoWu
  Created on:     2019-03-18
  derived from:   none
*/

/*  History 

  V1  2019-09-08  JoWu
    - Creation
*/

/*  todo-list
  2019-09-08  void adcAIupdate(void) massive problems!
  2019-09-08  JoWu First Statemachine Implementation
  
*/

/*
  Includes
*/
#include "Arduino.h"
#include "ArdSched.h"  // Scheduler
#include "ArdDan.h"     // Display alphanumerical

/*
  Macros / Defines
*/
// defines
#define SERIAL_DEBUG_ENABLE

/*
  Port pin definitions
*/
#define PO_DO_C_SRS_ON 50      // Current Source On for Pt-100 measurement

#define DO_REL_6 22           // Relais 6 Output
#define DO_REL_5 23           // Relais 5 Output
#define DO_REL_4 24           // Relais 4 Output
#define DO_REL_3 25           // Relais 3 Output
#define DO_REL_2 26           // Relais 2 Output
#define DO_REL_1 27           // Relais 1 Output

#define DI_1 30               // Digital Input 1
#define DI_2 31               // Digital Input 2
#define DI_3 32               // Digital Input 3
#define DI_4 33               // Digital Input 4
#define DI_5 34               // Digital Input 5
#define DI_6 35               // Digital Input 6

#define AI_HS1      A0        // Hallsensor Current Measurement 1
#define AI_HS2      A1        // Hallsensor Current Measurement 2

#define AI_MA1      A2        // mA Input 1
#define AI_MA2      A3        // mA Input 2

#define AI_T1       A4        // Pt100 Input 1
#define AI_T2       A5        // Pt100 Input 2

#define AI_HV1      A8        // High Voltage Input 1
#define AI_HV2      A9        // High Voltage Input 2


#define AI_HS1_IDX  0         // Hallsensor Current Measurement 1
#define AI_HS2_IDX  1         // Hallsensor Current Measurement 2

#define AI_MA1_IDX  2         // mA Input 1
#define AI_MA2_IDX  3         // mA Input 2

#define AI_T1_IDX   6         // Pt100 Input 1
#define AI_T2_IDX   7         // Pt100 Input 2

#define AI_HV1_IDX  4        // High Voltage Input 1
#define AI_HV2_IDX  5        // High Voltage Input 2



/*
  Typedefs
*/

/*
  External Variables Protoypes
*/

/*
  External Function Header Prototypes
*/


/*
  Public Variables
*/
#define DIG_DI_NUM  6                // amount of digital inputs
#define DIG_DI_DEB_TIM  5             // [tick] debounce time
byte gubDigInStatus[DIG_DI_NUM];      // digital Input Status, 0 ... off, 1 ... on
char gubDigInDebounce[DIG_DI_NUM];    // inc/dec for debounce, tbd...

#define ADC_MAX   1023               // [dig] ADC max value
#define ADC_NUM   8                  // amount of digital inputs
#define ADC_AVG   16                 // averaging values
unsigned short gusAnaIn[ADC_NUM];    // [dig 10] analog inputs
unsigned short gusAnaInSc[ADC_NUM];  // [dig 16] analog inputs scaled

/*
    Private Variables
*/

// lokale Konstanten
const char cachVER[] = "1.00";       // Softwareversion

/*
  Public Function Prototypes
*/

/*
  Private Function Prototypes
*/
void adcAIupdate(void);
void digDIupdate(void);
void RelaisTestSeq(void);
void serialSendStatus(void);

/*
 implementation of public functions
*/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.write("Hello World 0\n");
  
  pinMode(DO_REL_1, OUTPUT);
  pinMode(DO_REL_2, OUTPUT);
  pinMode(DO_REL_3, OUTPUT);
  pinMode(DO_REL_4, OUTPUT);
  pinMode(DO_REL_5, OUTPUT);
  pinMode(DO_REL_6, OUTPUT);  

  pinMode(DI_1, INPUT);
  pinMode(DI_2, INPUT);
  pinMode(DI_3, INPUT);
  pinMode(DI_4, INPUT);
  pinMode(DI_5, INPUT);
  pinMode(DI_6, INPUT); 

  pinMode(PO_DO_C_SRS_ON, OUTPUT);
  digitalWrite(PO_DO_C_SRS_ON, 1);
  delay(1000);
  digitalWrite(PO_DO_C_SRS_ON, 0);
  

  // LCD init
  ArdDanSetup();
  DanWrite(0,0,"Lunzer ECU IPS"); 
  DanWrite(0,1,"FW-Ver.: "); 
  DanWrite(9,1,cachVER);
  DanUpdate();
  delay(2000);
//  DanClear();
  DanWrite(0,0,"                       ");
  DanWrite(0,1,"                       ");
  DanUpdate();
  ArdSchedSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  ArdSchedLoop();
  delay(1);
}

void Task1(void){
//  Serial.println("Task 1");
  digDIupdate();
}

void Task2(void){
//  Serial.println("Task 2");
  adcAIupdate();
}

void Task3(void){
//  Serial.println("Task 3");
}

void Task4(void){
  long lulMillis = millis();
//  Serial.print("Task 4: ");

  // LCD
  //  Vxxx-Axx,x  Vxxx-Axx,x  
  //  XX-XX-XX-XX XX-XX-XX-XX 
  DanWrite(0,0,"Vxxx-Axx,x  Vxxx-Axx,x  ");
  DanWrite(0,1,"XX-XX-XX-XX XX-XX-XX-XX ");
//  DanUpdate();
  ArdDanTask();
//  Serial.println(millis()-lulMillis);
}

void Task5(void){
//  Serial.println("Task 5");
}

// Task 1000 ms
void Task6(void){
//  Serial.println("Task 6");
  serialSendStatus();
}

// Task 500 ms
void Task7(void){
  static int rintTimerTask7;

//  Serial.println("Task 7");
  RelaisTestSeq();  
  switch(rintTimerTask7){
    case 0:
//      digitalWrite(DO_REL_1, 1);  // 
      rintTimerTask7++;
      break;
    case 1:
    case 2:
//      digitalWrite(DO_REL_1, 0);  // 
      rintTimerTask7++;
      break;
    default:
      rintTimerTask7 = 0;
      break;
  }
}

// Task 1000 ms
void Task8(void){
  int idx;
  Serial.print("Task 8 - times: ");
  for (idx = 0; idx < TASK_USED_NUM; idx ++){
    Serial.print(" ");
    Serial.print(gaulTaskTime[idx]);
  }
  Serial.println();

}

/*
 implementation of private functions
*/

void adcAIupdate(void){
  byte idx;
  byte avgCnt;
  unsigned short usADCValue;

  for (idx = 0; idx < ADC_NUM-1; idx++){
//    Serial.println(idx);
    if (idx == AI_T1_IDX /*|| idx == AI_T2_IDX*/){
      digitalWrite(PO_DO_C_SRS_ON, 1);
      delay(2);
    }
    usADCValue = 0;
    for (avgCnt = 0; avgCnt < ADC_AVG; avgCnt++){
      switch(idx){
        case AI_HS1_IDX: usADCValue += analogRead(AI_HS1); break;
        case AI_HS2_IDX: usADCValue += analogRead(AI_HS2); break;
        case AI_MA1_IDX: usADCValue += analogRead(AI_MA1); break;
        case AI_MA2_IDX: usADCValue += analogRead(AI_MA2); break;
        case AI_T1_IDX: usADCValue += analogRead(AI_T1); break;
        case AI_T2_IDX: usADCValue += analogRead(AI_T2); break;
        case AI_HV1_IDX: usADCValue += analogRead(AI_HV1); break;
        case AI_HV2_IDX: usADCValue += analogRead(AI_HV2); break;
//        default: usADCValue = 0; break;
      }
    }
    if (/*idx == AI_T1_IDX || */idx == AI_T2_IDX){
      digitalWrite(PO_DO_C_SRS_ON, 0);
    }
    gusAnaIn[idx] = (usADCValue + ADC_AVG/2) / ADC_AVG;    // [dig 10] analog inputs

    switch(idx){
      case AI_HS1_IDX: //gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 1515 + ADC_MAX/2) / ADC_MAX; break;  // [dig 16] analog inputs scaled // [A/10] Hallsensor Current Measurement 1
      case AI_HS2_IDX: gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 1515 + ADC_MAX/2) / ADC_MAX; break;  // [dig 16] analog inputs scaled // [A/10] Hallsensor Current Measurement 2
      case AI_MA1_IDX: //gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 2273 + ADC_MAX/2) / ADC_MAX; break;  // [dig 16] analog inputs scaled // [mA/100] mA Input 1
      case AI_MA2_IDX: gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 2273 + ADC_MAX/2) / ADC_MAX; break;  // [dig 16] analog inputs scaled // [mA/100] mA Input 1
      case AI_T1_IDX:  //gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 24611 + ADC_MAX/2) / ADC_MAX; break; // [dig 16] analog inputs scaled // [Ohm/100] Pt100 Input 1
      case AI_T2_IDX:  gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 24611 + ADC_MAX/2) / ADC_MAX; break; // [dig 16] analog inputs scaled // [Ohm/100] Pt100 Input 2
      case AI_HV1_IDX: //gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 111 + ADC_MAX/2) / ADC_MAX; break;   // [dig 16] analog inputs scaled // [V] High Voltage Input 1
      case AI_HV2_IDX: gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 111 + ADC_MAX/2) / ADC_MAX; break;   // [dig 16] analog inputs scaled // [V] High Voltage Input 2
      default: break;
    }

/*    if (idx == AI_HS1_IDX || idx == AI_HS2_IDX){
      gusAnaInSc[idx] = (unsigned short)((unsigned long)gusAnaIn[idx] * 1515 + ADC_MAX/2) / ADC_MAX;  // [dig 16] analog inputs scaled // [A/10] Hallsensor Current Measurement 1/2
    }
    if (idx == AI_MA1_IDX || idx == AI_MA2_IDX){
      gusAnaInSc[idx] = (unsigned short)((unsigned long)gusAnaIn[idx] * 2273 + ADC_MAX/2) / ADC_MAX;  // [dig 16] analog inputs scaled // [mA/100] mA Input 1/2
    }
    if (idx == AI_T1_IDX || idx == AI_T2_IDX){
      gusAnaInSc[idx] = (unsigned short)((unsigned long)gusAnaIn[idx] * 24611 + ADC_MAX/2) / ADC_MAX; // [dig 16] analog inputs scaled // [Ohm/100] Pt100 Input 2
    }
    if (idx == AI_HV1_IDX || idx == AI_HV2_IDX){
      gusAnaInSc[idx] = (unsigned short)((unsigned long)gusAnaIn[idx] * 111 + ADC_MAX/2) / ADC_MAX;   // [dig 16] analog inputs scaled // [V] High Voltage Input 1/2
    }*/
  }  
//  idx++;
}

void digDIupdate(void){
  byte idx;
  byte byDIState;
  
  for (idx = 0; idx < DIG_DI_NUM; idx++){
    switch(idx){
      case 0: byDIState = digitalRead(DI_1); break;
      case 1: byDIState = digitalRead(DI_2); break;
      case 2: byDIState = digitalRead(DI_3); break;
      case 3: byDIState = digitalRead(DI_4); break;
      case 4: byDIState = digitalRead(DI_5); break;
      case 5: byDIState = digitalRead(DI_6); break;
      default:
        byDIState = 0;
      break;
    }
    gubDigInStatus[idx] = byDIState;     // digital Input Status
  }
}

void RelaisTestSeq(void){
  static char rchSM;

  switch(rchSM){
    case 0:
      digitalWrite(DO_REL_1, 1);
      break;
    case 1:
      digitalWrite(DO_REL_1, 0);
      break;
    case 2:
      digitalWrite(DO_REL_2, 1);
      break;
    case 3:
      digitalWrite(DO_REL_2, 0);
      break;
    case 4:
      digitalWrite(DO_REL_3, 1);
      break;
    case 5:
      digitalWrite(DO_REL_3, 0);
      break;
    case 6:
      digitalWrite(DO_REL_4, 1);
      break;
    case 7:
      digitalWrite(DO_REL_4, 0);
      break;
    case 8:
      digitalWrite(DO_REL_5, 1);
      break;
    case 9:
      digitalWrite(DO_REL_5, 0);
      break;
    case 10:
      digitalWrite(DO_REL_6, 1);
      break;
    case 11:
      digitalWrite(DO_REL_6, 0);
      rchSM = -1;
      break;
      
    default:
      rchSM = -1;
  }
  rchSM++;
}

void serialSendStatus(void){
  byte idx;
  for (idx = 0; idx < DIG_DI_NUM; idx++){
    Serial.print("DI"); Serial.print(idx+1); Serial.print(" = "); Serial.print(gubDigInStatus[idx]); Serial.print(", ");
  }
  Serial.println();
/*  
  if (digitalRead(DI_1)){
    Serial.print("DI1 = 1");
  }
  else{
    Serial.print("DI1 = 0");
  }
  if (digitalRead(DI_2)){
    Serial.print(" DI2 = 1");
  }
  else{
    Serial.print(" DI2 = 0");
  }
  if (digitalRead(DI_3)){
    Serial.print(" DI3 = 1");
  }
  else{
    Serial.print(" DI3 = 0");
  }
  if (digitalRead(DI_4)){
    Serial.print(" DI4 = 1");
  }
  else{
    Serial.print(" DI4 = 0");
  }
  if (digitalRead(DI_5)){
    Serial.print(" DI5 = 1");
  }
  else{
    Serial.print(" DI5 = 0");
  }
  if (digitalRead(DI_6)){
    Serial.print(" DI6 = 1");
  }
  else{
    Serial.print(" DI6 = 0");
  }
  Serial.println();
*/
  Serial.print("AI HS 1: ");
  Serial.print(analogRead(AI_HS1)); Serial.print(", "); Serial.print(gusAnaIn[AI_HS1_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_HS1_IDX]);
  Serial.print("  AI HS 2: ");
  Serial.print(analogRead(AI_HS2)); Serial.print(", "); Serial.print(gusAnaIn[AI_HS2_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_HS2_IDX]);
  Serial.println();

  Serial.print("AI mA 1: ");
  Serial.print(analogRead(AI_MA1)); Serial.print(", "); Serial.print(gusAnaIn[AI_MA1_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_MA1_IDX]);
  Serial.print("  AI mA 2: ");
  Serial.print(analogRead(AI_MA2)); Serial.print(", "); Serial.print(gusAnaIn[AI_MA2_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_MA2_IDX]);
  Serial.println();

  digitalWrite(PO_DO_C_SRS_ON, 1);
  delay(5);
  Serial.print("AI T 1: ");
  Serial.print(analogRead(AI_T1)); Serial.print(", "); Serial.print(gusAnaIn[AI_T1_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_T1_IDX]);
  Serial.print("  AI T 2: ");
  Serial.print(analogRead(AI_T2)); Serial.print(", "); Serial.print(gusAnaIn[AI_T2_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_T2_IDX]);
  Serial.println();
  digitalWrite(PO_DO_C_SRS_ON, 0);

  Serial.print("AI HV 1: ");
  Serial.print(analogRead(AI_HV1)); Serial.print(", "); Serial.print(gusAnaIn[AI_HV1_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_HV1_IDX]);
  Serial.print("  AI HV 2: ");
  Serial.print(analogRead(AI_HV2)); Serial.print(", "); Serial.print(gusAnaIn[AI_HV2_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_HV2_IDX]);
  Serial.println();
}
