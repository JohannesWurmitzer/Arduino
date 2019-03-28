/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer
  All rights reserved.
*/
/*  Function description

  Sketch for Lunzer Anti-Ice Prototype
  
*/
/*
  File Name:      Lunzer_Anti-Ice.ino
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     JoWu
  Created on:     2019-03-18
  derived from:   none
*/

/*  History 


  V1  2019-03-18  JoWu
    - Creation
*/

/*  todo-list
  - do not know, too much...
*/

/*
  Includes
*/
#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden

LiquidCrystal_I2C lcd(0x27, 16, 2); //Hier wird festgelegt um was für einen Display es sich handelt. In diesem Fall eines mit 16 Zeichen in 2 Zeilen und der HEX-Adresse 0x27. Für ein vierzeiliges I2C-LCD verwendet man den Code "LiquidCrystal_I2C lcd(0x27, 20, 4)" 



#include <OneWire.h>
// DS18S20 Temperaturchip i/o
OneWire ds(22);  // an pin 10


/*
  Macros
*/
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
int val = 0;      // variable to store the read value

#define SERIAL_DEBUG_ENABLE

// lokale Konstanten
const String cstrVER = String("0.01");       // Softwareversion

int zahl;
int intCurrentValue = 0;  // variable to store the value coming from the current sensor


void setup() {
  // put your setup code here, to run once:
#ifdef SERIAL_DEBUG_ENABLE
  Serial.begin(115200);   // (9600);
  Serial.println("Lunzer Anti ICE");
  Serial.print("Firmware Version: ");
  Serial.println(cstrVER);
#endif 

  pinMode(DO_REL_1, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  pinMode(3, INPUT);
  pinMode(2, INPUT); 

  lcd.init(); //Im Setup wird der LCD gestartet 
  lcd.backlight(); //Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus). 
  lcd.setCursor(0, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  lcd.print("Lunzer AntiIce"); 
  lcd.setCursor(0, 1);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
  lcd.print("Version 1"); 
  delay(2000);
  lcd.setCursor(0, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  lcd.print("                "); 
  lcd.setCursor(0, 1);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
  lcd.print("                "); 
  zahl = 0;  
}

void loop() {
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
 
  delay(1000);     // 750ms sollten ausreichen
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


 
  Serial.println();


  
  // put your main code here, to run repeatedly:
  Serial.println("Läuft\n");
  delay(500);
  
  digitalWrite(DO_REL_1, HIGH);
  delay(500);
  digitalWrite(DO_REL_1, LOW);
  delay(500);

  val = digitalRead(DI_1);   // read the input pin
  digitalWrite(DO_REL_1, val);  // sets the LED to the button's value
  val = !digitalRead(DI_2);   // read the input pin
  digitalWrite(DO_REL_2, val);  // sets the LED to the button's value
  val = !digitalRead(DI_3);   // read the input pin
  digitalWrite(DO_REL_3, val);  // sets the LED to the button's value
  val = !digitalRead(DI_4);   // read the input pin
  digitalWrite(DO_REL_4, val);  // sets the LED to the button's value
  val = !digitalRead(DI_5);   // read the input pin
  digitalWrite(DO_REL_5, val);  // sets the LED to the button's value
  val = !digitalRead(DI_6);   // read the input pin
  digitalWrite(DO_REL_6, val);  // sets the LED to the button's value

  lcd.setCursor(0,0); // Angabe des Cursorstartpunktes oben links.
  lcd.print("Cnt:"); // Ausgabe des Textes "Nachricht".
  lcd.print(zahl); // Ausgabe des Textes "Nachricht".
  zahl = zahl + 1;
  lcd.setCursor(0,1); // Angabe des Cursorstartpunktes oben links.
  lcd.print("Tmp:");
  lcd.print(Whole);
  lcd.print(".");
  lcd.print(Fract);

  intCurrentValue = analogRead(sensorPin);
  Serial.print("Current-Value:");
  Serial.print(intCurrentValue);
  Serial.print("   ");
  Serial.print((long)intCurrentValue * 1000 / 4808);
  Serial.println();

  lcd.setCursor(8,0); // Angabe des Cursorstartpunktes oben links.
  lcd.print("Cur:");
  lcd.print((long)intCurrentValue * 1000 / 4808);
  Serial.print("  ");
  
}



