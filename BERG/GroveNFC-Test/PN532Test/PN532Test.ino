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

PN532_HSU pn532hsu(Serial1);
PN532_HSU interface2(Serial2);
PN532 nfc1(pn532hsu);
//NfcAdapter nfc2 = NfcAdapter(interface2);

unsigned int uiCounter;

void setup(void) {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  
  Serial.println("NDEF Reader 1");
  nfc1.begin();
  
  delayMicroseconds(500);
  nfc1.SAMConfig();//SAM...Secure Access Module
  delay(1);

/*
  Serial.println("NDEF Reader 2");
  nfc2.begin();
*/
  uiCounter = 0;
}


void loop(void) {
  static uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  static uint8_t uid2[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  static uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  static uint8_t uidLength2;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  // put your main code here, to run repeatedly:
  
    Serial.println("\nScan an NFC 1 tag");
    Serial.println(uiCounter++, DEC);
    
    if (nfc1.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 50))
    {
      Serial.println("\nTag 1 present");

          Serial.print("\n\rID:");
          for (uint8_t i=0; i < uidLength; i++){
            if(uid[i] <= 0x0F){
              Serial.print(" 0x0");Serial.print(uid[i], HEX);              
            }
            else{
              Serial.print(" 0x");Serial.print(uid[i], HEX); 
            }
            Serial.print(",");            
          }
          Serial.print("\n\r");
        
    }

/*
    Serial.println("\nScan an NFC 2 tag");
    if (nfc2.tagPresent())
    {
        NfcTag tag = nfc2.read();
        tag.print();
    }
*/
    delay(500);
}
