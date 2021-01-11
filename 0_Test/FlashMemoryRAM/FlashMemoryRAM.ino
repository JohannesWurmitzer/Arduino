/*
Sketch uses 1956 bytes (0%) of program storage space. Maximum is 253952 bytes.
Global variables use 216 bytes (2%) of dynamic memory, leaving 7976 bytes for local variables. Maximum is 8192 bytes.
*/

#include <avr/pgmspace.h>;

#define SW_VERSION  "ITB1_120pre1_D"

const char cstrVersion[] PROGMEM = SW_VERSION;       // Softwareversion


char buffer[30];

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void LOG_Eintrag(String strMeldung){
  Serial.println(strMeldung);
}

String retString(String strInput){
  return F("Return-Value");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(F("Hello World!"));
}

void loop() {
  char strBuffer[20];
  int i;
  char charFlash;
  
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.println(F("hello again..."));
  for (i=0; i<5; i++){
    charFlash = pgm_read_byte_near(cstrVersion+i);
    Serial.print(charFlash);
  }
  Serial.println();
  
  strcpy_P(buffer, cstrVersion);
  Serial.println(buffer);
  Serial.print(F("SW_VERSION: " SW_VERSION));
  Serial.print(F("Free-RAM: ")); Serial.println(freeRam());
  String strObject = F("strObject-Version: ");
  Serial.print(F("Free-RAM: ")); Serial.println(freeRam());
  strObject += F(" und noch ein Hallo");
  Serial.print(F("Free-RAM: ")); Serial.println(freeRam());
  LOG_Eintrag(strObject);
  strObject = F("123456789012345678901234567890");
  Serial.print(F("Free-RAM: ")); Serial.println(freeRam());
  Serial.print(F("PSTR-Test: ")); LOG_Eintrag(PSTR("ein Log-Eintrag")); // fails!
  Serial.print(F("F-Test: ")); LOG_Eintrag(F("ein Log-Eintrag"));
  Serial.print(F("(String)F-Test: ")); LOG_Eintrag((String)F("ein Log-Eintrag")); // same as without String-Object
  Serial.print(F("retString: ")); Serial.println(retString(F("Caller")));
  Serial.println();
}
