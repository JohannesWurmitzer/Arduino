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

#define DO_REL_1 22           // Relais 1 Output
#define DO_REL_2 23           // Relais 2 Output
#define DO_REL_3 24           // Relais 3 Output
#define DO_REL_4 25           // Relais 4 Output
#define DO_REL_5 26           // Relais 5 Output
#define DO_REL_6 27           // Relais 6 Output

#define DI_1 30               // Digital Input 1
#define DI_2 31               // Digital Input 2
#define DI_3 32               // Digital Input 3
#define DI_4 33               // Digital Input 4
#define DI_5 34               // Digital Input 5
#define DI_6 35               // Digital Input 6

#define DI_1_IDX  0           // Digital Input 1 Index
#define DI_2_IDX  1           // Digital Input 2 Index
#define DI_3_IDX  2           // Digital Input 3 Index
#define DI_4_IDX  3           // Digital Input 4 Index
#define DI_5_IDX  4           // Digital Input 5 Index
#define DI_6_IDX  5           // Digital Input 6 Index


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

#define AI_HV1_IDX  4         // High Voltage Input 1
#define AI_HV2_IDX  5         // High Voltage Input 2


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

#define ADC_MAX   1023                // [dig] ADC max value
#define ADC_NUM   8                   // amount of digital inputs
#define ADC_AVG   16                  // averaging values
unsigned short gusAnaIn[ADC_NUM];     // [dig 10] analog inputs
unsigned short gusAnaInSc[ADC_NUM];   // [dig 16] analog inputs scaled

unsigned short gusResistance[2];      // [Ohm/10] 
signed gsTemp[2];                     // [°C/10]

/*
    Private Variables
*/

// IPS Statemachine

byte byStatZone[2][4];         // Zonenstatus

byte bySM_IPS_act = 0;    // State Machine Actual State
byte bySM_time = 0;       // State Machine Timer

#define SM_IPS_ON     0   // we are on, no generator voltage
#define SM_IPS_RDY    1   // generator voltage okay

#define SM_IPS_TST_AI 2  // test anti-ice zone
#define SM_IPS_TST_IN 3  // test inner zone
#define SM_IPS_TST_OU 4  // test outer zone
#define SM_IPS_TST_TA 5  // test tail zone

#define SM_IPS_TST_SHOWTEMP 10  //  test show temperature

#define SM_IPS_ON_AI 6  // on anti-ice zone
#define SM_IPS_ON_IN 7  // on ai + inner zone
#define SM_IPS_ON_OU 8  // on ai + outer zone
#define SM_IPS_ON_TA 9  // on ai + tail zone


#define SM_IPS_TST_TIM    2 // [s]  Time Test
#define SM_IPS_ON_TIM_AI  5 // [s]  Time Anti Ice
#define SM_IPS_ON_TIM_OT  3 // [s]  Time Others

#define IPS_DI_IDX_LDI_TEST        DI_1_IDX
#define IPS_DI_IDX_LDI_ON          DI_2_IDX
#define IPS_DI_IDX_DEICE_LOW       DI_3_IDX
#define IPS_DI_IDX_DEICE_HIGH      DI_4_IDX
#define IPS_DI_IDX_NO_LOAD_ON_GEAR DI_5_IDX

#define IPS_RELAIS_AI   DO_REL_1    // Relais Anti-Ice
#define IPS_RELAIS_IN   DO_REL_2    // Relais DeIce-Inner
#define IPS_RELAIS_OU   DO_REL_3    // Relais DeIce-Outer
#define IPS_RELAIS_TA   DO_REL_4    // Relais DeIce-Tail

int intHeartbeat = 0;     //

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

void ipsDispMain(void);
void ipsSM(void);

void Task1(void);
void Task2(void);
void Task3(void);
void Task4(void);
void Task5(void);
void Task6(void);
void Task7(void);
void Task8(void);

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
  if (ArdSchedTaskRdyStart(TASK_1)){
    Task1();
    ArdSchedTaskStop();
  }
  if (ArdSchedTaskRdyStart(TASK_2)){
    Task2();
    ArdSchedTaskStop();
  }
  if (ArdSchedTaskRdyStart(TASK_3)){
    Task3();
    ArdSchedTaskStop();
  }
  if (ArdSchedTaskRdyStart(TASK_4)){
    Task4();
    ArdSchedTaskStop();
  }
  if (ArdSchedTaskRdyStart(TASK_5)){
    Task5();
    ArdSchedTaskStop();
  }
  if (ArdSchedTaskRdyStart(TASK_6)){
    Task6();
    ArdSchedTaskStop();
  }
  if (ArdSchedTaskRdyStart(TASK_7)){
    Task7();
    ArdSchedTaskStop();
  }
  if (ArdSchedTaskRdyStart(TASK_8)){
    Task8();
    ArdSchedTaskStop();
  }

  // Loops of Basic Software
  ArdDanLoop();

  // Delay just for fun...
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
  ipsDispMain();

  // Display Task
  ArdDanTask();
}

void Task4(void){
  long lulMillis = millis();
//  Serial.print("Task 4: ");

//  Serial.println(millis()-lulMillis);
}

void Task5(void){
//  Serial.println("Task 5");
  ipsSM();


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
//  RelaisTestSeq();  
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

// Variables for IPS Statemachine


// IPS Display Main Screen
void ipsDispMain(void){
  char strText[25];
  char i, idx;
  short lsTemp;
  // LCD
  //  Vxxx-Axx,xSTVxxx-Axx,x  
  //  XX-XX-XX-XXSXX-XX-XX-XX
  DanWrite(0,0,"Vxxx-Axx,x  Vxxx-Axx,x  ");
  DanWrite(0,1,"XX-XX-XX-XX XX-XX-XX-XX ");

  DanWrite(0,0,"Vxxx-Axx,x  Vxxx-Txx,x  ");

  
  
//  sprintf(strText,"V%03d-A%02d,%d  ",gusAnaInSc[AI_HV1_IDX]/10,gusAnaInSc[AI_HS1_IDX]/10, gusAnaInSc[AI_HS1_IDX]%10);
//  DanWrite(0,0,strText);

  for (i=0; i<2; i++){
    // Generator Spannungsanzeige
    sprintf(strText,"V%03d",gusAnaInSc[AI_HV1_IDX+i]/10);
    DanWrite(0+12*i,0,strText);
  
    DanWrite(4+12*i,0,"-");
  
    if (bySM_IPS_act == SM_IPS_TST_SHOWTEMP){
      // Temperaturanzeige
      dtostrf((float)gsTemp[i]/10, 5, 1, strText);
      DanWrite(5+12*i,0,"T");
      DanWrite(6+12*i,0,strText);
    }
    else{
      if (bySM_IPS_act >= SM_IPS_TST_AI && bySM_IPS_act <= SM_IPS_TST_TA){
        // Bereicht Widerstandsanzeige
        sprintf(strText,"R%02d,%d",gusResistance[i]/10, gusResistance[i]%10);  DanWrite(5+12*i,0,strText);
      }
      else{
        // Generator Stromanzeige
        sprintf(strText,"A%02d,%d",gusAnaInSc[AI_HS1_IDX+i]/10, gusAnaInSc[AI_HS1_IDX+i]%10);  DanWrite(5+12*i,0,strText);
      }
    }
  }
  
  //gusResistance[2];      // [Ohm/10] 

  //DanWrite(12,0,strText);

  for (idx = 0; idx<2; idx++){
    for (i = 0; i<4; i++){
      switch(byStatZone[idx][i]){
        case 0:  // OFF
          DanWrite(idx*12+i*3,1,"__"); 
          break;
        case 1:  // ON
          DanWrite(idx*12+i*3,1,"ON");
          break;
        case 2:  // OK
          DanWrite(idx*12+i*3,1,"OK");
          break;
        default:
          DanWrite(idx*12+i*3,1,"FF");
          break;
      }
      if (i < 3) DanWrite(idx*12+2+i*3,1,"-");
    }
  }
/*
  // State Machine Time Left
  sprintf(strText,"%2d",bySM_time);  DanWrite(10, 0, strText);
  // State Machine State Left
  sprintf(strText,"%d",bySM_IPS_act % 10);  DanWrite(11, 1, strText);
*/
}

#define IPS_DI_IDX_LDI_TEST        DI_1_IDX
#define IPS_DI_IDX_LDI_ON          DI_2_IDX
#define IPS_DI_IDX_DEICE_LOW       DI_3_IDX
#define IPS_DI_IDX_DEICE_HIGH      DI_4_IDX
#define IPS_DI_IDX_NO_LOAD_ON_GEAR DI_5_IDX


#define GEN_V_MIN   600   // [V/10] 24.0 V    minimale Generatorspannung
#define GEN_V_HYS   120   // [V/10]  5.0 V    Hysterese minimale Generatorspannung

#define IPS_C_MIN   50    // [A/10]  5.0 V    minimaler Strom pro Heizelement

void ipsSM(void){
  unsigned char idx;
  int intGenUValueV[2];      // [V/10] voltage value
  int intCurrentValuetA[2];  // [A/10] current value

  intGenUValueV[0] = gusAnaInSc[AI_HV1_IDX];            // read generator voltage
  intCurrentValuetA[0] = gusAnaInSc[AI_HS1_IDX];         // read system current

  intGenUValueV[1] = gusAnaInSc[AI_HV2_IDX];            // read generator voltage
  intCurrentValuetA[1] = gusAnaInSc[AI_HS2_IDX];         // read system current

  if (bySM_time < 99){
    bySM_time++;
  }
  switch(bySM_IPS_act){
    case SM_IPS_ON:   // we are on, no generator voltage
      if ((intGenUValueV[0] > GEN_V_MIN) || (intGenUValueV[1] > GEN_V_MIN) /*|| 1*/){
        bySM_IPS_act = SM_IPS_RDY;
        bySM_time = 0;
      }
      break;
    case SM_IPS_RDY:  // generator voltage okay
      if ((intGenUValueV[0] < GEN_V_MIN - GEN_V_HYS) && (intGenUValueV[1] < GEN_V_MIN - GEN_V_HYS)){
        bySM_IPS_act = SM_IPS_ON;
        bySM_time = 0;
      }
      else if (gubDigInStatus[IPS_DI_IDX_LDI_TEST]){
        // LDI Test Start
        byStatZone[0][0] = 0; byStatZone[1][0] = 0;
        byStatZone[0][1] = 0; byStatZone[1][1] = 0;
        byStatZone[0][2] = 0; byStatZone[1][2] = 0;
        byStatZone[0][3] = 0; byStatZone[1][3] = 0;
        bySM_IPS_act = SM_IPS_TST_AI;
        bySM_time = 0;
      }
      else if (gubDigInStatus[IPS_DI_IDX_LDI_ON]){
        // LDI On
        byStatZone[0][0] = 1; byStatZone[1][0] = 1;
        byStatZone[0][1] = 1; byStatZone[1][1] = 1;    // !!! not on, if DeIce OFF!!!
        byStatZone[0][2] = 0; byStatZone[1][2] = 0;
        byStatZone[0][3] = 0; byStatZone[1][3] = 0;
        bySM_IPS_act = SM_IPS_ON_IN;
        bySM_time = 0;
      }
      break;

    case SM_IPS_TST_AI:  // test anti-ice zone
    case SM_IPS_TST_IN:  // test inner zone
    case SM_IPS_TST_OU:  // test outer zone
    case SM_IPS_TST_TA:  // test tail zone
      if (bySM_time >= SM_IPS_TST_TIM){
        for (idx = 0; idx < 2; idx++){
          if (intCurrentValuetA[idx] > IPS_C_MIN){
            byStatZone[idx][bySM_IPS_act - SM_IPS_TST_AI] = 2;         // Heizelement OK Okay
          }
          else{
            byStatZone[idx][bySM_IPS_act - SM_IPS_TST_AI] = 255;       // Heizelement FF Failure
          }
        }
        if (bySM_IPS_act == SM_IPS_TST_TA){
          bySM_time = 0;
          bySM_IPS_act = SM_IPS_TST_SHOWTEMP; //SM_IPS_RDY;
        }
        else{
          bySM_IPS_act++;
          bySM_time = 0;
        }
      }
      break;

    case SM_IPS_TST_SHOWTEMP: // test show temperature
      if (!gubDigInStatus[IPS_DI_IDX_LDI_TEST]){
        bySM_time = 0;
        bySM_IPS_act = SM_IPS_RDY;
      }
      break;
      
    case SM_IPS_ON_AI:  // on anti-ice zone
      if (bySM_time >= SM_IPS_ON_TIM_AI){
          bySM_IPS_act++;
          byStatZone[0][1] = 1; byStatZone[1][1] = 1;
          bySM_time = 0;
      }
      break;
    case SM_IPS_ON_IN:  // on ai + inner zone
    case SM_IPS_ON_OU:  // on ai + outer zone
    case SM_IPS_ON_TA:  // on ai + tail zone
      if (bySM_time >= SM_IPS_ON_TIM_OT){
        byStatZone[0][1] = 0; byStatZone[1][1] = 0;
        byStatZone[0][2] = 0; byStatZone[1][2] = 0;
        byStatZone[0][3] = 0; byStatZone[1][3] = 0;
        if (bySM_IPS_act == SM_IPS_ON_TA){
          // check, if we stay on or go to ready
          if (gubDigInStatus[IPS_DI_IDX_LDI_ON]){
            bySM_IPS_act = SM_IPS_ON_AI;
            bySM_time = 0;
          }
          else{
            bySM_IPS_act = SM_IPS_RDY;
            byStatZone[0][0] = 0; byStatZone[1][0] = 0;
            bySM_time = 0;
          }
        }
        else{
          bySM_IPS_act++;
          byStatZone[0][bySM_IPS_act - SM_IPS_ON_AI] = 1; byStatZone[1][bySM_IPS_act - SM_IPS_ON_AI] = 1;
          bySM_time = 0;
        }
      }
      break;
  }

  if (bySM_IPS_act == SM_IPS_TST_AI
    || (bySM_IPS_act >= SM_IPS_ON_AI && bySM_IPS_act <= SM_IPS_ON_TA)
    ){
      digitalWrite(IPS_RELAIS_AI, 1);  //   Relais Anti Ice On
    }
  else{
      digitalWrite(IPS_RELAIS_AI, 0);  //   Relais Anti Ice Off
  }

  if (bySM_IPS_act == SM_IPS_TST_IN
    || (bySM_IPS_act == SM_IPS_ON_IN)
    ){
      digitalWrite(IPS_RELAIS_IN, 1);  //   Relais Inner On
    }
  else{
      digitalWrite(IPS_RELAIS_IN, 0);  //   Relais Inner Off
  }

  if (bySM_IPS_act == SM_IPS_TST_OU
    || (bySM_IPS_act == SM_IPS_ON_OU)
    ){
      digitalWrite(IPS_RELAIS_OU, 1);  //   Relais Outer On
    }
  else{
      digitalWrite(IPS_RELAIS_OU, 0);  //   Relais Outer Off
  }

  if (bySM_IPS_act == SM_IPS_TST_TA
    || (bySM_IPS_act == SM_IPS_ON_TA)
    ){
      digitalWrite(IPS_RELAIS_TA, 1);  //   Relais Tail On
    }
  else{
      digitalWrite(IPS_RELAIS_TA, 0);  //   Relais Tail Off
  }    
}



void adcAIupdate(void){
  byte idx;
  byte avgCnt;
  unsigned short usADCValue;

  for (idx = 0; idx < ADC_NUM; idx++){
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
      case AI_HS2_IDX: gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 1515*1.063*2 + ADC_MAX/2) / ADC_MAX; break;  // [dig 16] analog inputs scaled // [A/10] Hallsensor Current Measurement 2
      case AI_MA1_IDX: //gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 2273 + ADC_MAX/2) / ADC_MAX; break;  // [dig 16] analog inputs scaled // [mA/100] mA Input 1
      case AI_MA2_IDX: gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 2273 + ADC_MAX/2) / ADC_MAX; break;  // [dig 16] analog inputs scaled // [mA/100] mA Input 1
      case AI_T1_IDX:  //gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 24611 + ADC_MAX/2) / ADC_MAX; break; // [dig 16] analog inputs scaled // [Ohm/100] Pt100 Input 1
      case AI_T2_IDX:  gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 24611 + ADC_MAX/2) / ADC_MAX; break; // [dig 16] analog inputs scaled // [Ohm/100] Pt100 Input 2
      case AI_HV1_IDX: //gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 1113 + ADC_MAX/2) / ADC_MAX; break;   // [dig 16] analog inputs scaled // [V/10] High Voltage Input 1
      case AI_HV2_IDX: gusAnaInSc[idx] = ((unsigned long)gusAnaIn[idx] * 1113*1.08 + ADC_MAX/2) / ADC_MAX; break;   // [dig 16] analog inputs scaled // [V/10] High Voltage Input 2
      default: break;
    }
    
    gusResistance[0] = (gusAnaInSc[AI_HV1_IDX]* 10 + gusAnaInSc[AI_HS1_IDX]/2) / gusAnaInSc[AI_HS1_IDX];
    gusResistance[1] = (gusAnaInSc[AI_HV2_IDX]* 10 + gusAnaInSc[AI_HS2_IDX]/2) / gusAnaInSc[AI_HS2_IDX];

    if (gusAnaInSc[AI_MA1_IDX] < 400){
      gsTemp[0] = -200;
    }
    else if (gusAnaInSc[AI_MA1_IDX] > 2000){
      gsTemp[0] = +200;
    }
    else{
      gsTemp[0] = short(((unsigned long) 400 * (gusAnaInSc[AI_MA1_IDX] - 400) + 800) / 1600) - 200;
    }

    if (gusAnaInSc[AI_MA2_IDX] < 400){
      gsTemp[1] = -200;
    }
    else if (gusAnaInSc[AI_MA2_IDX] > 2000){
      gsTemp[1] = +200;
    }
    else{
      gsTemp[1] = short(((unsigned long) 400 * (gusAnaInSc[AI_MA2_IDX] - 400) + 800) / 1600) - 200;
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
  Serial.print(analogRead(AI_HS1)); Serial.print(", "); Serial.print(gusAnaIn[AI_HS1_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_HS1_IDX]); Serial.print(" A/10, "); Serial.print(gusResistance[0]); Serial.print(" Ohm/10)");
  Serial.print("  AI HS 2: ");
  Serial.print(analogRead(AI_HS2)); Serial.print(", "); Serial.print(gusAnaIn[AI_HS2_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_HS2_IDX]); Serial.print(" A/10, "); Serial.print(gusResistance[1]); Serial.print(" Ohm/10)");
  Serial.println();

  Serial.print("AI mA 1: ");
  Serial.print(analogRead(AI_MA1)); Serial.print(", "); Serial.print(gusAnaIn[AI_MA1_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_MA1_IDX]); Serial.print(" mA/100, "); Serial.print(gsTemp[0]);
  Serial.print("  AI mA 2: ");
  Serial.print(analogRead(AI_MA2)); Serial.print(", "); Serial.print(gusAnaIn[AI_MA2_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_MA2_IDX]); Serial.print(" mA/100, "); Serial.print(gsTemp[1]);
  Serial.println();

  digitalWrite(PO_DO_C_SRS_ON, 1);
  delay(5);
  Serial.print("AI T 1: ");
  Serial.print(analogRead(AI_T1)); Serial.print(", "); Serial.print(gusAnaIn[AI_T1_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_T1_IDX]); Serial.print(" Ohm");
  Serial.print("  AI T 2: ");
  Serial.print(analogRead(AI_T2)); Serial.print(", "); Serial.print(gusAnaIn[AI_T2_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_T2_IDX]); Serial.print(" Ohm");
  Serial.println();
  digitalWrite(PO_DO_C_SRS_ON, 0);

  Serial.print("AI HV 1: ");
  Serial.print(analogRead(AI_HV1)); Serial.print(", "); Serial.print(gusAnaIn[AI_HV1_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_HV1_IDX]); Serial.print(" V/10");
  Serial.print("  AI HV 2: ");
  Serial.print(analogRead(AI_HV2)); Serial.print(", "); Serial.print(gusAnaIn[AI_HV2_IDX]); Serial.print(", "); Serial.print(gusAnaInSc[AI_HV2_IDX]); Serial.print(" V/10");
  Serial.println();
}
