/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer (Welando GmbH)
  All rights reserved.
*/

/*
  File Name:      ArdRFID_SLConf.h
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Maximilian Johannes Wurmitzer
  Created on:     2019-10-01
  derived from:   
*/
/*  History - Headerfile should have the version of the associated module file
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)

  2019-10-01  V1.00  JoWu
    - Creation
     
*/
/*  todo-list
  - do not know...
*/
#ifndef ARDRFID_SLCONF_H_
#define ARDRFID_SLCONF_H_
#define ARDRFID_SLCONF_V 100     // RFID Config Version
//#ifdef __cplusplus
//extern "C"{
//#endif
/*
  Includes
*/
/*
  Macros / Defines
*/
// Definition for SL030 I2C
#define SL030ADR 0xA0   // standard address for SL030
#ifdef ARDUINO_SAM_DUE
#else
  #define PN532           // define, if PN532 is used
#endif
/*
  Typedefs
*/

/*
  Port pin definitions
*/
//Port pins for task timing tests
#define USE_SL030                  // define, if used
#define PO_SL030_IN  10            // define, if used
#define PO_SL030_OUT 11            // define, if used


//#ifdef __cplusplus
//} // extern "C"
//#endif 

#endif /* ARDSCHEDCONF_H_ */
