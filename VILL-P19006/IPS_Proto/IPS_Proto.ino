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
#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden

//LiquidCrystal_I2C lcd(0x27, 16, 2); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 
//LiquidCrystal_I2C lcd(0x27, 20, 4); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 
LiquidCrystal_I2C lcd(0x27, 24, 2); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 


// defines
#define SERIAL_DEBUG_ENABLE

#define DO_REL_1 22
#define DO_REL_2 23
#define DO_REL_3 24
#define DO_REL_4 25
#define DO_REL_5 26
#define DO_REL_6 27

#define DI_1 30
#define DI_2 31
#define DI_3 32
#define DI_4 33
#define DI_5 34
#define DI_6 35

#define AI_HV1      8
#define AI_HV2      9

// lokale Konstanten
const String cstrVER = String("1.00");       // Softwareversion


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
    
  if (digitalRead(DI_1)){
    Serial.print(" DI1 = 1");
  }
  else{
    Serial.print(" DI1 = 0");
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

  // LCD
  //  Vxxx-Axx,x  Vxxx-Axx,x  
  //  XX-XX-XX-XX XX-XX-XX-XX 
//  lcd.print(10);
  lcd.setCursor(0,0);
  lcd.print("Vxxx-Axx,x  Vxxx-Axx,x  ");
  lcd.setCursor(0,1);
  lcd.print("XX-XX-XX-XX XX-XX-XX-XX ");
  digitalWrite(DO_REL_1, 1);  // 
  delay(500);
  digitalWrite(DO_REL_1, 0);  // 
  delay(1000);
}
