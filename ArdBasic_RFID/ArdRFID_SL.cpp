/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer (Welando GmbH)
  All rights reserved.
*/
/*  Function description

  RF-ID Reader Functions for Stronglink Reader
*/
/*
  File Name:      ArdRFID_SL.c
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Maximilian Johannes Wurmitzer
  Created on:     2019-10-01
  derived from:   
*/

/*  History 
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)

  2019-10-01  V1.00  JoWu
    - Creation
    
*/

/*  todo-list
  open, 2019-10-01; JoWu; too much...

*/

/*
  Includes
*/
#include "ArdRFID_SL.h"
#include "Arduino.h"
#include <Wire.h>

#include <PN532_HSU.h>      // high speed UART
#include <PN532.h>

PN532_HSU pn532hsu1(Serial1);      //Serial1 User reader
PN532 nfc1(pn532hsu1);

PN532_HSU pn532hsu2(Serial2);      //Serial1 User reader
PN532 nfc2(pn532hsu2);

/*
  Macros / Defines
*/
#define SERIAL_DEBUG_ENABLE       // define for debug reasons

#define RFID_SL_INIT    0
#define RFID_SL_SCAN    1
#define RFID_SL_RESCAN  2
#define RFID_SL_ERROR   4


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


struct sSLRFID_Data gsSL030Data;


/*
    Private Variables
*/

/*
  Global Variables (global only in this module [C-File])
  https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
*/

  

/*
  Public Function Prototypes
*/
void ArdRFID_SL_Setup();
void ArdRFID_SL_Loop();

void ArdRFID_PN532_Setup();
boolean PN532nfc1readPassiveTargetID(uint8_t* puid, uint8_t* puidLength, uint8_t u8MaxLen);

/*
  Private Function Prototypes
*/

/*
 implementation of public functions
*/
void ArdRFID_SL_Setup(){
  // serielle Kommunikation initialisieren
  //Serial.begin(115200);
  memset(&gsSL030Data, 0, sizeof(gsSL030Data));
  // setup() for SL030 I2C
 #ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init NFC 1 OneWire I2C");
 #endif
  Wire.begin();         // join i2c bus (address optional for master)  
}

void ArdRFID_PN532_Setup(){
  Serial.println("NFC532 1 User-Reader: Init");
  nfc1.begin();
  delayMicroseconds(500);
  uint32_t versiondata = nfc1.getFirmwareVersion();
  Serial.print("NFC532 1 Firmware: "); Serial.println(versiondata);
  /*
  //------------------------------------------
  if (! versiondata) {
    Serial.print("Didn't find PN53x board1; system reset now!");
    resetFunc();  //call reset
  }
  */
  //------------------------------------------
  delayMicroseconds(500);
  nfc1.SAMConfig();//SAM...Secure Access Module
//  delay(1);

  Serial.println("NFC532 2 Articel-Reader: Init");
  nfc2.begin();
  delayMicroseconds(500);
  /*uint32_t*/ versiondata = nfc2.getFirmwareVersion();
  Serial.print("NFC532 2 Firmware: "); Serial.println(versiondata);
  /*
  //------------------------------------------
  if (! versiondata) {
    Serial.print("Didn't find PN53x board1; system reset now!");
    resetFunc();  //call reset
  }
  */
  //------------------------------------------
  delayMicroseconds(500);
  nfc2.SAMConfig();//SAM...Secure Access Module
//  delay(1);

}

void ArdRFID_SL_Loop() {
#define RFID_SL_INIT    0
#define RFID_SL_SCAN    1
#define RFID_SL_RESCAN  2
#define RFID_SL_ERROR   4

}



/*
 implementation of private functions
*/
boolean SL030readPassiveTargetID(uint8_t* puid, uint8_t* puidLength, uint8_t u8MaxLen)
{
  unsigned char u8Len;
  unsigned char u8ProtNr;
  unsigned char u8Status;
  *puidLength = 0;  
  // Select Mifare card  
#ifdef SERIAL_DEBUG_ENABLE
  u8Len = 0;
  if(Wire.available()){
    u8Len = 1;
    Serial.print("Clear I2C ");
  }
#endif
  while(Wire.available()){
#ifdef SERIAL_DEBUG_ENABLE
    Serial.print(Wire.read(), HEX);
    Serial.print(" ");
#else
    Wire.read();
#endif
  }
#ifdef SERIAL_DEBUG_ENABLE
  if (u8Len){
    Serial.println();
  }
#endif
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("I2C Transmit ");
#endif
  Wire.beginTransmission(SL030ADR/2); // transmit to device #SL030ADR
  Wire.write(0x01);      // len
  Wire.write(0x01);      // cmd Select Mifare Card
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println(Wire.endTransmission(true));     // stop transmitting
#else
  Wire.endTransmission(true);     // stop transmitting
#endif
  delay(30);             // < 50 ms seams to be critical
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("I2C Read ");
#endif
  u8Len = Wire.requestFrom(SL030ADR/2, 10, true);    // request 10 byte from slave device #SL030ADR, which is the max length of the protocol
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println(u8Len);
#endif

#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("I2C available: ");
  Serial.println(Wire.available());     // Wenn != 10, dann hat die I2C Kommunikation nicht funktioniert
#endif

  u8Len = Wire.read();
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("Len: ");
  Serial.println(u8Len);
//  Serial.print(" ");
#endif  
  if (u8Len == 7)
  {
      Wire.read();
      Wire.read();
      puid[0] = Wire.read();
      puid[1] = Wire.read();
      puid[2] = Wire.read();
      puid[3] = Wire.read();
      puid[4] = 0;
      puid[5] = 0;
      puid[6] = 0;
      while(Wire.available()) Wire.read();
      *puidLength = 4;
      return true;           
  }
  else if(u8Len == 10)
  {
      Wire.read();
      Wire.read();
      puid[0] = Wire.read();
      puid[1] = Wire.read();
      puid[2] = Wire.read();
      puid[3] = Wire.read();
      puid[4] = Wire.read();
      puid[5] = Wire.read();
      puid[6] = Wire.read();
      while(Wire.available()) Wire.read();
      *puidLength = 7;  
      return true;    
  }
  else{
    puid[0] = 0;
    puid[1] = 0;
    puid[2] = 0;
    puid[3] = 0;
    puid[4] = 0;
    puid[5] = 0;
    puid[6] = 0;
    while(Wire.available()) Wire.read();
    *puidLength = 0;  
    return false;
  }
}

bool PN532nfc1readPassiveTargetID(uint8_t* puid, uint8_t* puidLength, uint8_t u8MaxLen){
  bool lboRFID_Chip_Detected;
  lboRFID_Chip_Detected = nfc1.readPassiveTargetID(PN532_MIFARE_ISO14443A, puid, puidLength, 50);
  return lboRFID_Chip_Detected;
}

bool PN532nfc2readPassiveTargetID(uint8_t* puid, uint8_t* puidLength, uint8_t u8MaxLen){
  bool lboRFID_Chip_Detected;
  lboRFID_Chip_Detected = nfc2.readPassiveTargetID(PN532_MIFARE_ISO14443A, puid, puidLength, 50);
  return lboRFID_Chip_Detected;
}
