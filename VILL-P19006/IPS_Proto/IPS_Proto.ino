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
  2019-09-08  to much...
  2019-09-08  JoWu First Statemachine Implementation
  
*/

/*
  Includes
*/
#include "ArduSched.h"

#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden

//LiquidCrystal_I2C lcd(0x27, 16, 2); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 
//LiquidCrystal_I2C lcd(0x27, 20, 4); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 
LiquidCrystal_I2C lcd(0x27, 24, 2); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 


// defines
#define SERIAL_DEBUG_ENABLE
#define DO_C_SRS_ON 50

#define DO_REL_6 22
#define DO_REL_5 23
#define DO_REL_4 24
#define DO_REL_3 25
#define DO_REL_2 26
#define DO_REL_1 27

#define DI_1 30
#define DI_2 31
#define DI_3 32
#define DI_4 33
#define DI_5 34
#define DI_6 35

#define AI_HS1      A0  // Hallsensor Current Measurement 1
#define AI_HS2      A1  // Hallsensor Current Measurement 2

#define AI_MA1      A2  // mA Input 1
#define AI_MA2      A3  // mA Input 2

#define AI_T1      A4  // mA Input 1
#define AI_T2      A5  // mA Input 2

#define AI_HV1      A8  // High Voltage Input 1
#define AI_HV2      A9  // High Voltage Input 2

// lokale Konstanten
const String cstrVER = String("1.00");       // Softwareversion


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.write("Hello World 0\n");
  ArduSchedInit();
  
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

  pinMode(DO_C_SRS_ON, OUTPUT);
  digitalWrite(DO_C_SRS_ON, 1);
  delay(1000);
  digitalWrite(DO_C_SRS_ON, 0);
  

  // LCD init
  
  lcd.init(); //Im Setup wird der LCD gestartet 
  lcd.backlight(); //Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus). 
  lcd.setCursor(0, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  lcd.print("Lunzer ECU IPS"); 
  lcd.setCursor(0, 1);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
  lcd.print("FW-Ver.: "); 
  lcd.print(cstrVER);
  delay(2000);
  lcd.setCursor(0, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  lcd.print("                "); 
  lcd.setCursor(0, 1);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
  lcd.print("                ");   
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduSchedHandler();

}

void Task1(void){
  Serial.println("Task 1");
}

void Task2(void){
  Serial.println("Task 2");
}

void Task3(void){
  Serial.println("Task 3");
}

void Task4(void){
  long lulMillis = millis();
  Serial.print("Task 4: ");
  // LCD
  //  Vxxx-Axx,x  Vxxx-Axx,x  
  //  XX-XX-XX-XX XX-XX-XX-XX 
//  lcd.print(10);
  lcd.setCursor(0,0);
  lcd.print("Vxxx-Axx,x  Vxxx-Axx,x  ");
  lcd.setCursor(0,1);
  lcd.print("XX-XX-XX-XX XX-XX-XX-XX ");
  Serial.println(millis()-lulMillis);
}

void Task5(void){
  Serial.println("Task 5");
}

// Task 1000 ms
void Task6(void){
  Serial.println("Task 6");
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

  Serial.print("AI HS 1: ");
  Serial.print(analogRead(AI_HS1));
  Serial.print("  AI HS 2: ");
  Serial.print(analogRead(AI_HS2));
  Serial.println();

  Serial.print("AI mA 1: ");
  Serial.print(analogRead(AI_MA1));
  Serial.print("  AI mA 2: ");
  Serial.print(analogRead(AI_MA2));
  Serial.println();

  digitalWrite(DO_C_SRS_ON, 1);
  delay(5);
  Serial.print("AI T 1: ");
  Serial.print(analogRead(AI_T1));
  Serial.print("  AI T 2: ");
  Serial.print(analogRead(AI_T2));
  Serial.println();
  digitalWrite(DO_C_SRS_ON, 0);

  Serial.print("AI HV 1: ");
  Serial.print(analogRead(AI_HV1));
  Serial.print("  AI HV 2: ");
  Serial.print(analogRead(AI_HV2));
  Serial.println();

}

// Task 500 ms
void Task7(void){
  static int rintTimerTask7;

  Serial.println("Task 7");
  switch(rintTimerTask7){
    case 0:
      digitalWrite(DO_REL_1, 1);  // 
      rintTimerTask7++;
      break;
    case 1:
    case 2:
      digitalWrite(DO_REL_1, 0);  // 
      rintTimerTask7++;
      break;
    default:
      rintTimerTask7 = 0;
      break;
  }
}

// Task 1000 ms
void Task8(void){
  static bool rboolRel2;
  Serial.println("Task 8");
  if (rboolRel2){
    digitalWrite(DO_REL_2, 0);
    rboolRel2 = false;
  }
  else{
    digitalWrite(DO_REL_2, 1);
    rboolRel2 = true;
  }
}
