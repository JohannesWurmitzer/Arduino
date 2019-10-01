/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer (Welando GmbH)
  All rights reserved.
*/

/*
  File Name:      ArdSched.h
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Maximilian Johannes Wurmitzer
  Created on:     2019-07-23
  derived from:   
*/
/*  History - Headerfile should have the version of the associated module file
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)

  V5.00 2019-07-23  JoWu
    - Moved Config to ArdSchedConf.h
    
  2019-10-01  V1.00  JoWu
    - Creation
     
*/
/*  todo-list
  - do not know...
*/
#ifndef ARDRFID_SL_H_
#define ARDRFID_SL_H_
#define ARDRFID_SL_V 100     // ArdRFID_SL Version
//#ifdef __cplusplus
//extern "C"{
//#endif
/*
  Includes
*/
#include "Arduino.h"
/*
  Macros / Defines
*/

#include "ArdRFID_SLConf.h"  // User Configuration for each Project

/*
  Typedefs
*/
struct sSLRFID_Data{
  bool boolAvailable;
  bool boolTagDetected;
  bool boolTagDetectionLost;
  unsigned char ubState;                //
  unsigned long ulTimeStamp;            //
  unsigned char ubTagIDsize;            // validated Tag ID size
  unsigned char aubTagID[7];            // validated Tag ID
  unsigned char ubScannedTagIDsize;     // Scanned Tag ID size
  unsigned char aubScannedTagID[7];     // Scanned Tag ID
  unsigned char ubDetectionDelay;       // [ms] Detection Delay, set to 0, if not used
  unsigned char ubDetectionLossDelay;   // [ms] Detection Loss Delay, set to 0, if not used
};

/*
  Port pin definitions
*/

/*
  Public Variables
*/

/*
  Public Function Prototypes required
*/


/*
  Public Function Prototypes provided
*/
extern void ArdRFID_SL_Setup();
extern void ArdRFID_SL_Loop();

extern void ArdRFID_PN532_Setup();

boolean SL030readPassiveTargetID(uint8_t* puid, uint8_t* puidLength, uint8_t u8MaxLen);
boolean PN532nfc1readPassiveTargetID(uint8_t* puid, uint8_t* puidLength, uint8_t u8MaxLen);
boolean PN532nfc2readPassiveTargetID(uint8_t* puid, uint8_t* puidLength, uint8_t u8MaxLen);

//#ifdef __cplusplus
//} // extern "C"
//#endif 

#endif /* ARDRFID_SL_H_ */
