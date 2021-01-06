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

  2021-01-06  V1.11 JoWu  check status in SL030readPassiveTargetID

  2020-02-19  V1.10 JoWu  add timeout

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

#ifdef PN532_USED
#include <PN532_HSU.h>      // high speed UART
#include <PN532.h>

PN532_HSU pn532hsu1(Serial1);      //Serial1 User reader
PN532 nfc1(pn532hsu1);

PN532_HSU pn532hsu2(Serial2);      //Serial1 User reader
PN532 nfc2(pn532hsu2);
#endif

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
struct sSLRFID_Data gsSL032Data;


/*
    Private Variables
*/
unsigned char const caubSelectCard[3] =      {0xBA,0x02,0x01 };       

/*
  Global Variables (global only in this module [C-File])
  https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
*/

  

/*
  Public Function Prototypes
*/
void ArdRFID_SL_Setup();
void ArdRFID_SL_Loop();

#ifdef PN532_USED  
void ArdRFID_PN532_Setup();
boolean PN532nfc1readPassiveTargetID(uint8_t* puid, uint8_t* puidLength, uint8_t u8MaxLen);
#endif
/*
  Private Function Prototypes
*/
uint8_t SL032_ReadUid(uint8_t* puid);
void SL032_SendCom(unsigned char const*g_cCommand);

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
  Wire.setClock(400000);
  Wire.begin();         // join i2c bus (address optional for master)

  memset(&gsSL032Data, 0, sizeof(gsSL032Data));
  // setup() for SL032 UART
 #ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init NFC 2 UART");
 #endif
  Serial2.begin(115200);
}

#ifdef PN532_USED  
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
#endif
void ArdRFID_SL_Loop() {
#define RFID_SL_INIT    0
#define RFID_SL_SCAN    1
#define RFID_SL_RESCAN  2
#define RFID_SL_ERROR   4

}



/*
 implementation of private functions
*/

// I2C RF-ID Reader
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
      u8Status = Wire.read();
      puid[0] = Wire.read();
      puid[1] = Wire.read();
      puid[2] = Wire.read();
      puid[3] = Wire.read();
      puid[4] = 0;
      puid[5] = 0;
      puid[6] = 0;
      while(Wire.available()) Wire.read();
      *puidLength = 4;
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("Status: ");
  Serial.println(u8Status);
#endif  
      if (u8Status == 0x01){
        return false;           
      }
      else{
        return true;           
      }
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

boolean SL032readPassiveTargetID(uint8_t* puid, uint8_t* uidLength, uint8_t u8MaxLen)
{
  unsigned char i;
  // check, if something is in serial buffer, if yes, then read/clear it
  i=0;
#ifdef SERIAL_DEBUG_ENABLE
 if (Serial2.available())
 {
  i=1;
  Serial.print("S2Buff:");
 }
#endif
while (Serial2.available())
  {
#ifdef SERIAL_DEBUG_ENABLE
    Serial.print(Serial2.read(), HEX);
    Serial.print(" ");
#else
    Serial2.read();
#endif
  }
#ifdef SERIAL_DEBUG_ENABLE
    if (i>0)
    {
      Serial.println();
    }
#endif  
  SL032_SendCom(caubSelectCard);                             //
  Serial2.flush();
  *uidLength = SL032_ReadUid(puid);
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("UIDlen:");
  Serial.print(*uidLength);
  Serial.print(" ");
  for (i=0; i<*uidLength; i++)
  {
    Serial.print(puid[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
#endif

  if (*uidLength > 0){
    return true;
  }
  else{
    return false;
  }  
}

////////////////////////////////////////////////////////////
//Send command to SL032
////////////////////////////////////////////////////////////
void SL032_SendCom(unsigned char const *g_cCommand)
{    
     unsigned char i,chkdata,sendleg;
     while (Serial2.available()) i = Serial2.read();
#ifdef SERIAL_DEBUG_ENABLE
      Serial.print("SL032_SendCom: ");
#endif
     sendleg = *(g_cCommand+1) + 1;
     
     chkdata = 0;
     for(i=0; i<sendleg; i++)
     {    
         chkdata ^= *(g_cCommand+i);
      Serial2.print((char)*(g_cCommand+i));
#ifdef SERIAL_DEBUG_ENABLE
      Serial.print(*(g_cCommand+i), HEX);
      Serial.print(" ");
#endif      
     }
     
      Serial2.print((char)chkdata);
#ifdef SERIAL_DEBUG_ENABLE
      Serial.println(chkdata, HEX);
#endif      
}

// return len of UID, 4 or 7
uint8_t SL032_ReadUid(uint8_t* puid){
unsigned char u8Len;
unsigned char u8ProtNr;
unsigned char u8Status;
unsigned char i = 0;

  puid[0] = 0;
  puid[1] = 0;
  puid[2] = 0;
  puid[3] = 0;
  puid[4] = 0;
  puid[5] = 0;
  puid[6] = 0;
  
  do{
   delay(1); 
  } while(Serial2.available() < 2 && i++ < 100);    // !!!JoWu: Dead End, if no response // aprox. 50 ms delay if no Tag, 20 ms delay, if Tag present
  delay(5);
  Serial.print("SL32 delay: ");  Serial.println(i);
  // check for 0xBD protocol
  if (Serial2.available()>=2 && Serial2.read() == 0xBD){
    // read len
    u8Len = Serial2.read();
#ifdef SERIAL_DEBUG_ENABLE
    Serial.print("SL32 Len: ");  Serial.println(u8Len);
#endif      
    if (Serial2.available() == u8Len){
      u8ProtNr = Serial2.read();
      u8Status = Serial2.read();
      if (u8Len == 8)
      {
        puid[0] = Serial2.read();
        puid[1] = Serial2.read();
        puid[2] = Serial2.read();
        puid[3] = Serial2.read();
        puid[4] = 0;
        puid[5] = 0;
        puid[6] = 0;
        return 4;
      }
      else if (u8Len == 11)
      {
        puid[0] = Serial2.read();
        puid[1] = Serial2.read();
        puid[2] = Serial2.read();
        puid[3] = Serial2.read();
        puid[4] = Serial2.read();
        puid[5] = Serial2.read();
        puid[6] = Serial2.read();
        return 7;
      }
      else
      {
        return 0;
      }
    }
    else{
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

#ifdef PN532_USED
// UART Reader 1
bool PN532nfc1readPassiveTargetID(uint8_t* puid, uint8_t* puidLength, uint8_t u8MaxLen){
  bool lboRFID_Chip_Detected;
  lboRFID_Chip_Detected = nfc1.readPassiveTargetID(PN532_MIFARE_ISO14443A, puid, puidLength, 50);
  return lboRFID_Chip_Detected;
}

// UART Reader 2
bool PN532nfc2readPassiveTargetID(uint8_t* puid, uint8_t* puidLength, uint8_t u8MaxLen){
  bool lboRFID_Chip_Detected;
  lboRFID_Chip_Detected = nfc2.readPassiveTargetID(PN532_MIFARE_ISO14443A, puid, puidLength, 50);
  return lboRFID_Chip_Detected;
}
#endif
