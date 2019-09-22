/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer (Welando GmbH)
  All rights reserved.
*/
/*  Function description

  Basic Software Tools
  Arduino Display Alphanumerical
  
  V1.00: ca. 1814 Bytes Program Memory, 94 Bytes Dynamic Memory
  
*/
/*
  File Name:      ArdDan.c
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Maximilian Johannes Wurmitzer
  Created on:     2019-09-22
  derived from:   
*/

/*  History 
    Versioning: VX.YZ: 
    X..increase for big change or bugfix
    Y..incr. for enhanced functionality
    Z..incr. for structure or documentation changes)

  V1.00 2019-09-22  JoWu
    - Creation

*/

/*  todo-list
  open, 2019-09-22; JoWu; nothing or too much
  open, 2019-07-23; JoWu; PROGMEM attribute does not work (https://www.avrfreaks.net/forum/atmega-2560-progmem-problem)
*/

/*
  Includes
*/
//#include <avr/pgmspace.h>
#include "ArdDan.h"
#include "Arduino.h"

#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden

LiquidCrystal_I2C lcd(0x27, DAN_LCD_COLS, DAN_LCD_ROWS); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 


/*
  Macros / Defines
*/

/*
  Typedefs
*/

/*
  External Variables Protoypes
*/

/*
  Port pin definitions
*/

/*
  External Function Header Prototypes
*/


/*
  Public Variables
*/

/*
    Private Variables
*/
char achLCDText[DAN_LCD_ROWS][DAN_LCD_COLS + 1];
/*
  Global Variables (global only in this module [C-File])
  https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
*/

/*
  Public Function Prototypes
*/
void ArdDanSetup();
void ArdDanLoop();
void ArdDanTask();

void DanUpdate(void);

void DanWrite(char x, char y, char *txt);
void DanClear(void);

/*
  Private Function Prototypes
*/

/*
 implementation of public functions
*/

void ArdDanSetup(){
  memset(achLCDText, ' ', sizeof(achLCDText));
  lcd.init(); //Im Setup wird der LCD gestartet 
  lcd.backlight(); //Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus). 
  lcd.setCursor(0, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  lcd.print("LCD i2c ");
  lcd.print(DAN_LCD_COLS);
  lcd.print("x");
  lcd.print(DAN_LCD_ROWS);
  lcd.setCursor(0, 1);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
  lcd.print("Drv V");
  lcd.print(ARDDAN_V);
}

void ArdDanLoop(){
}

void ArdDanTask(){
  DanUpdate();
}

void DanUpdate(void){
  char idx;
  for (idx = 0; idx < DAN_LCD_ROWS; idx++){
    achLCDText[idx][DAN_LCD_COLS] = 0;
    lcd.setCursor(0, idx);
    lcd.print(achLCDText[idx]);
  }
  
}
void DanWrite(char x, char y, char *txt){
  if (x >= 0 && x < DAN_LCD_COLS && y >= 0 && y < DAN_LCD_ROWS){
    while(x < DAN_LCD_COLS && *txt != 0){
      achLCDText[y][x++] = *txt++;
    }
  }
}

void DanClear(void){
  memset(achLCDText, ' ', sizeof(achLCDText));
}

/*
 implementation of private functions
*/
