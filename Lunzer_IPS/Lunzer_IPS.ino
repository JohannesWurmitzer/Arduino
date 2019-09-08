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
  V2  2019-03-28  JoWu First Statemachine Implementation

  V1  2019-03-28  JoWu
    - Creation
*/

/*  todo-list
  2019-03-28  add speed switch, maybe we use AI instead of DI because of clamp
  
*/

/*
  Includes
*/
#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden

LiquidCrystal_I2C lcd(0x27, 16, 2); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 
//LiquidCrystal_I2C lcd(0x27, 20, 4); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 



#include <OneWire.h>
// DS18S20 Temperaturchip i/o
OneWire ds(22);  // an pin 10


/*
  Macros
*/
#define GEN_V_MIN   120   // [V/10] 24 V    minimale Generatorspannung
#define GEN_V_HYS   120   // [V/10]  5 V    Hysterese minimale Generatorspannung

#define DO_REL_1 8        // Heizung Tragfläche Rechts Innen
#define DO_REL_2 9        // Heizung Tragfläche Rechts Außen
#define DO_REL_3 10       // Heizung Tragfläche Rechts Tail
#define DO_REL_4 11       // Reserve
#define DO_REL_5 12       // Reserve
#define DO_REL_6 13       // LED-Taster

#define DI_1  2           // Digital Input 1
#define DI_2  3           // Digital Input 2
#define DI_3  4           // Digital Input 3
#define DI_4  5           // Digital Input 4
#define DI_5  6           // Digital Input 5
#define DI_6  7           // Digital Input 6

int sensorPin = A0;    // select the input pin for the current sensor
int aiPinGenU = A1;    // analog input pin for Generator Voltage

int val = 0;      // variable to store the read value

#define SERIAL_DEBUG_ENABLE

// lokale Konstanten
const String cstrVER = String("0.01");       // Softwareversion

int intCurrentValue = 0;  // [dig] variable to store the value coming from the current sensor
int intGenUValue = 0;     // [dig] variable for generator voltage value

int intCurrentValuetA = 0;  // [A/10] current value
int intGenUValueV = 0;      // [V] voltage value

byte byStatZone[4];         // Zonenstatus

byte bySM_IPS_act = 0;
byte bySM_time = 0;

#define SM_IPS_ON     0   // we are on, no generator voltage
#define SM_IPS_RDY    1   // generator voltage okay

#define SM_IPS_TST_AI 10  // test anti-ice zone
#define SM_IPS_TST_IN 11  // test inner zone
#define SM_IPS_TST_OU 12  // test outer zone
#define SM_IPS_TST_TA 13  // test tail zone

#define SM_IPS_ON_AI 20  // on anti-ice zone
#define SM_IPS_ON_IN 21  // on ai + inner zone
#define SM_IPS_ON_OU 22  // on ai + outer zone
#define SM_IPS_ON_TA 23  // on ai + tail zone

#define SM_IPS_TST_TIM    2 // [s]
#define SM_IPS_ON_TIM_AI  5 // [s]
#define SM_IPS_ON_TIM_OT  3 // [s]

int intHeartbeat = 0;     //

void setup() {
  // put your setup code here, to run once:
#ifdef SERIAL_DEBUG_ENABLE
  Serial.begin(115200);   // (9600);
  Serial.println("Lunzer ECU IPS");
  Serial.print("Firmware Version: ");
  Serial.println(cstrVER);
#endif 

  pinMode(DO_REL_1, OUTPUT);
  pinMode(DO_REL_2, OUTPUT);
  pinMode(DO_REL_3, OUTPUT);
  pinMode(DO_REL_4, OUTPUT);
  pinMode(DO_REL_5, OUTPUT);
  pinMode(DO_REL_6, OUTPUT);
  
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  pinMode(3, INPUT);
  pinMode(2, INPUT); 

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

  Serial.print("Heartbeat:");
  Serial.println(intHeartbeat);
  if (intHeartbeat & 1){
    digitalWrite(DO_REL_6, HIGH);
  }
  else{
    digitalWrite(DO_REL_6, LOW);
  }

  // Temperaturmessung
  
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
 
  if ( !ds.search(addr)) {
      Serial.print("Keine weiteren Addressen.\n");
      ds.reset_search();
      return;
  }
 
  Serial.print("R=");
  for( i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC nicht gültig!\n");
      return;
  }
 
  if ( addr[0] == 0x10) {
      Serial.print("Gerät ist aus der DS18S20 Familie.\n");
  }
  else if ( addr[0] == 0x28) {
      Serial.print("Gerät ist aus der DS18B20 Familie.\n");
  }
  else {
      Serial.print("Gerätefamilie nicht erkannt : 0x");
      Serial.println(addr[0],HEX);
      return;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start Konvertierung, mit power-on am Ende
 
  delay(900);     // 750ms sollten ausreichen
  // man sollte ein ds.depower() hier machen, aber ein reset tut das auch
 
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Wert lesen
 
  Serial.print("P=");
  Serial.print(present,HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print( OneWire::crc8( data, 8), HEX);
  
  Serial.println();

  LowByte = data[0];
  HighByte = data[1];

  TReading = (HighByte << 8) + LowByte;

  SignBit = TReading & 0x8000;  // test most sig bit

  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }

  Tc_100 = (6 * TReading) + TReading / 4;    // mal (100 * 0.0625) oder 6.25

/* Für DS18S20 folgendes verwenden Tc_100 = (TReading*100/2);    */
  Whole = Tc_100 / 100;  // Ganzzahlen und Brüche trennen
  Fract = Tc_100 % 100;
  Serial.print("Temp: ");
  if (SignBit) // negative Werte ermitteln
  {
     Serial.print("-");
  }

  Serial.print(Whole);
  Serial.print(".");
  if (Fract < 10)
  {
     Serial.print("0");
  }
  Serial.print(Fract);
  Serial.print("°C");
  Serial.print("\n");

#if 0
  val = !digitalRead(DI_1);   // read the input pin
  digitalWrite(DO_REL_1, val);  // sets the LED to the button's value
  val = !digitalRead(DI_2);   // read the input pin
  digitalWrite(DO_REL_2, val);  // sets the LED to the button's value
  val = !digitalRead(DI_3);   // read the input pin
  digitalWrite(DO_REL_3, val);  // sets the LED to the button's value
  val = !digitalRead(DI_4);   // read the input pin
  digitalWrite(DO_REL_4, val);  // sets the LED to the button's value
#endif
  val = !digitalRead(DI_5);   // read the input pin
  digitalWrite(DO_REL_5, val);  // sets the LED to the button's value
#if 0  
  val = !digitalRead(DI_6);   // read the input pin
  digitalWrite(DO_REL_6, val);  // sets the LED to the button's value
#endif

#if 0
  lcd.setCursor(0,0); // Angabe des Cursorstartpunktes oben links.
  lcd.print("Cnt:"); // Ausgabe des Textes "Nachricht".
  lcd.print(intHeartbeat); // Ausgabe des Textes "Nachricht".
  lcd.setCursor(0,1); // Angabe des Cursorstartpunktes oben links.
  lcd.print("Tmp:");
  lcd.print(Whole);
  lcd.print(".");
  lcd.print(Fract);
#endif

  // Stromwert messen
  intCurrentValue = 0;
  for (i=0; i<16; i++){
    intCurrentValue += analogRead(sensorPin);
  }
  intCurrentValue = intCurrentValue / 16;
  
  Serial.print("Current-Value:");
  Serial.print(intCurrentValue);
  Serial.print("   ");
  Serial.print((long)intCurrentValue * 10204 / 4808);
  Serial.println();

  // 1. Zeile

  lcd.setCursor(6,0); // Angabe des Cursorstartpunktes oben links.
  lcd.print("LA");
  intCurrentValuetA = ((long)intCurrentValue * 10204 / 4808);
  if (intCurrentValuetA > 999) intCurrentValuetA = 999;
  if (intCurrentValuetA < 10){
    lcd.print(" ");
  }
  else if (intCurrentValuetA < 100){
    lcd.print(" ");
  }
  lcd.print(intCurrentValuetA / 10);
  lcd.print(".");
  lcd.print(intCurrentValuetA % 10);

  // Spannungswert
  intGenUValue = analogRead(aiPinGenU);
  Serial.print("Generator Voltage Value:");
  Serial.print(intGenUValue);
  Serial.print("   ");
  Serial.print((long)intGenUValue * 1175 / 10230);
  Serial.println();
  Serial.println();
  
  intGenUValueV = (long)intGenUValue * 1175 / 10230;
  lcd.setCursor(0,0); // Angabe des Cursorstartpunktes oben links.
  lcd.print("LV");
  if (intGenUValueV > 999) intGenUValueV = 999;
  if (intGenUValueV < 10){
    lcd.print("  ");
  }
  else if (intGenUValueV < 100){
    lcd.print(" ");
  }
  lcd.print(intGenUValueV);

  if (bySM_time < 99){
    bySM_time++;
  }
  switch(bySM_IPS_act){

    case SM_IPS_ON:   // we are on, no generator voltage
      if (intGenUValueV > GEN_V_MIN || 1){
        bySM_IPS_act = SM_IPS_RDY;
        bySM_time = 0;
      }
      break;
    case SM_IPS_RDY:  // generator voltage okay
      if (intGenUValueV < GEN_V_MIN - GEN_V_HYS){
        bySM_IPS_act = SM_IPS_ON;
        bySM_time = 0;
      }
      else if (!digitalRead(DI_6)){
        byStatZone[0] = 0;
        byStatZone[1] = 0;
        byStatZone[2] = 0;
        byStatZone[3] = 0;
        bySM_IPS_act = SM_IPS_TST_AI;
        bySM_time = 0;
      }
      else if (!digitalRead(DI_5)){
        byStatZone[0] = 1;
        byStatZone[1] = 1;
        byStatZone[2] = 0;
        byStatZone[3] = 0;
        bySM_IPS_act = SM_IPS_ON_IN;
        bySM_time = 0;
      }
      break;

    case SM_IPS_TST_AI:  // test anti-ice zone
    case SM_IPS_TST_IN:  // test inner zone
    case SM_IPS_TST_OU:  // test outer zone
    case SM_IPS_TST_TA:  // test tail zone
      if (bySM_time >= SM_IPS_TST_TIM){
        if (intCurrentValuetA > 50){
          byStatZone[bySM_IPS_act - SM_IPS_TST_AI] = 2;
        }
        else{
          byStatZone[bySM_IPS_act - SM_IPS_TST_AI] = 255;
        }
        if (bySM_IPS_act == SM_IPS_TST_TA){
          bySM_IPS_act = SM_IPS_RDY;
        }
        else{
          bySM_IPS_act++;
          bySM_time = 0;
        }
      }
      break;
      
    case SM_IPS_ON_AI:  // on anti-ice zone
      if (bySM_time >= SM_IPS_ON_TIM_AI){
          bySM_IPS_act++;
          byStatZone[1] = 1;
          bySM_time = 0;
      }
      break;
    case SM_IPS_ON_IN:  // on ai + inner zone
    case SM_IPS_ON_OU:  // on ai + outer zone
    case SM_IPS_ON_TA:  // on ai + tail zone
      if (bySM_time >= SM_IPS_ON_TIM_OT){
        byStatZone[1] = 0;
        byStatZone[2] = 0;
        byStatZone[3] = 0;
        if (bySM_IPS_act == SM_IPS_ON_TA){
          if (!digitalRead(DI_5)){
            bySM_IPS_act = SM_IPS_ON_AI;
            bySM_time = 0;
          }
          else{
            bySM_IPS_act = SM_IPS_RDY;
            byStatZone[0] = 0;
            bySM_time = 0;
          }
        }
        else{
          bySM_IPS_act++;
          byStatZone[bySM_IPS_act - SM_IPS_ON_AI] = 1;
          bySM_time = 0;
        }
      }
      break;
  }

  if (bySM_IPS_act == SM_IPS_TST_AI
    || (bySM_IPS_act >= SM_IPS_ON_AI && bySM_IPS_act <= SM_IPS_ON_TA)
    ){
      digitalWrite(DO_REL_1, 1);  // 
    }
  else{
      digitalWrite(DO_REL_1, 0);  // 
  }

  if (bySM_IPS_act == SM_IPS_TST_IN
    || (bySM_IPS_act == SM_IPS_ON_IN)
    ){
      digitalWrite(DO_REL_2, 1);  // 
    }
  else{
      digitalWrite(DO_REL_2, 0);  // 
  }

  if (bySM_IPS_act == SM_IPS_TST_OU
    || (bySM_IPS_act == SM_IPS_ON_OU)
    ){
      digitalWrite(DO_REL_3, 1);  // 
    }
  else{
      digitalWrite(DO_REL_3, 0);  // 
  }

  if (bySM_IPS_act == SM_IPS_TST_TA
    || (bySM_IPS_act == SM_IPS_ON_TA)
    ){
      digitalWrite(DO_REL_4, 1);  // 
    }
  else{
      digitalWrite(DO_REL_4, 0);  // 
  }

  lcd.setCursor(14,0); // Angabe des Cursorstartpunktes oben links.
  if (bySM_IPS_act < 10){
    lcd.print(" ");
  }
  lcd.print(bySM_IPS_act);

  lcd.setCursor(14,1); // Angabe des Cursorstartpunktes oben links.
  if (bySM_IPS_act < 10){
    lcd.print(" ");
  }
  lcd.print(bySM_time);
  
  // 2. Zeile
  lcd.setCursor(0,1); // Angabe des Cursorstartpunktes oben links.
  
  for (i = 0; i<4; i++){
    switch(byStatZone[i]){
      case 0:  // OFF
        lcd.print("__"); 
        break;
      case 1:  // ON
        lcd.print("ON");
        break;
      case 2:  // OK
        lcd.print("OK");
        break;
      default:
        lcd.print("FF");
        break;
    }
    if (i < 3) lcd.print("-");
  }

  intHeartbeat++;
}



