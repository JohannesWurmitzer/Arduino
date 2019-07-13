/*
 Name:    GroveNFCTest.ino
 Datum:   2018-08-16
 Autor:   Maximilian Johannes Wurmitzer

  2018-08-16  V0  JoWu
    - funktionaler Prototyp
    
    
  todo:
  - offene Punkte "Brownout", "Watchdog"
  
    
*/
#include "PN532_HSU.h"
#include "PN532.h"
#include "NfcAdapter.h"

PN532_HSU interface1(Serial1);
PN532_HSU interface2(Serial2);
NfcAdapter nfc1 = NfcAdapter(interface1);
NfcAdapter nfc2 = NfcAdapter(interface2);

unsigned int uiCounter;

void setup(void) {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  
  Serial.println("NDEF Reader 1");
  nfc1.begin();
  
  delayMicroseconds(500);
//  nfc1.SAMConfig();//SAM...Secure Access Module
  delay(1);


  Serial.println("NDEF Reader 2");
  nfc2.begin();

  uiCounter = 0;
}

void loop(void) {
  // put your main code here, to run repeatedly:
  
    Serial.println("\nScan an NFC 1 tag");
    Serial.println(uiCounter++, DEC);
    
    if (nfc1.tagPresent())
    {
      Serial.println("\nTag 1 present");
        NfcTag tag = nfc1.read();
        tag.print();
    }


    Serial.println("\nScan an NFC 2 tag");
    if (nfc2.tagPresent())
    {
        NfcTag tag = nfc2.read();
        tag.print();
    }

    delay(500);
}
