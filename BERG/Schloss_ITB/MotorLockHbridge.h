/*
  Copyright (c) 2017, E.C.E. Wurmitzer GmbH
  All rights reserved.
*/

/*
  File Name:      MotorLockHbridge.h
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Edmund Titz
  Created on:     2017-11-05
  derived from:   

  Versionsgeschichte:
  08.05.2018  V 101   Markus Emanuel Wurmitzer
    - Zeitüberwachungswerte können durch externe Funktionen eingestellt werden
    
  07.05.2018  V 100   Markus Emanuel Wurmitzer
    - Zeitüberwachung für vorwärts und rückwärts unterschiedlich
  
  02.05.2018  V 101   Markus Emanuel Wurmitzer
    - Neue Funktionen zur Statusabfrage als Zeichenkette eingefügt

*/

/*  todo-list
  - do not know...
*/

/*
  Includes
*/
#include "Arduino.h"
/*
  Macros / Defines
*/
#ifndef MOTORLOCKHBRIDGE_H_
#define MOTORLOCKHBRIDGE_H_

#ifdef __cplusplus
extern "C"{
#endif
	
//general
#define NONE                0
#define NOT_APPLICABLE      0

#define MOTOR_ISR_TIME_US             15000
#define MOTOR_ISR_TIME_MS             (MOTOR_ISR_TIME_US / 1000)

/*
#define TIMEBASE_LED_SLOW             1500 / MOTOR_ISR_TIME_MS //1.5s
#define TIMEBASE_LED_FAST             500 / MOTOR_ISR_TIME_MS//0.5s
*/

//Initialisierungswerte bei ungültigen Parameterwerten
#define TIMEBASE_MOTOR_BLOCKING       555 / MOTOR_ISR_TIME_MS//600ms
#define TIMEBASE_MOTOR_BLOCKING_OP    705 / MOTOR_ISR_TIME_MS//800ms
#define TIMEBASE_MOTOR_ADD_DRIVE_BACK 120 / MOTOR_ISR_TIME_MS//120ms
//konstante Parameter
#define MOTOR_BRAKE_TIME              15 / MOTOR_ISR_TIME_MS//30ms

//definitions for MotorLock status
#define UNDEFINED       0
#define IN_PROGRESS     1
#define WAIT_ARC_CLOSE  2
#define IS_LOCKED       3
#define IS_UNLOCKED     4
#define LOCK_ERROR      5
#define UNLOCK_ERROR    6
 

//definitions for state machine
#define LOCKING       1
//Sub states
  #define DO_NOTHING                0
  #define WAIT_MOTOR_SWITCH_RISING  11
  #define WAIT_MOTOR_SWITCH_FALLING 12
#define WAIT_ARC_LOCKING  2
//Sub states
  #define DO_NOTHING                        0
  #define WAIT_LOCK_SWITCH_FALLING          21
  #define LOCKING_WAIT_MOTOR_SWITCH_RISING  22
  #define LOCKING_WAIT_MOTOR_SWITCH_FALLING 23
#define LOCKED        3

#define UNLOCKING     4
//Sub states
  #define DO_NOTHING                0
  #define UNLOCKING_WAIT_MOTOR_SWITCH_RISING  41
  //#define UNLOCKING_WAIT_LOCK_SWITCH_RISING   42
  #define UNLOCKING_WAIT_MOTOR_SWITCH_FALLING 43
  #define UNLOCKING_WAIT_MOTOR_SWITCH_RISING_2ND  44
  #define UNLOCKING_WAIT_MOTOR_SWITCH_FALLING_2ND  45
#define UNLOCKED      5

#define MOTOR_BLOCKED 6
#define MOTOR_TIMEOUT 7

#define WAIT_COMMAND  8
#define EMERGENCY_STOP_MOTOR  9

#define MOTOR_DIRECT_DIR_LOCK   10
#define MOTOR_DIRECT_DIR_UNLOCK 11

//definitions for set motor function
#define STOP_MOTOR      0
#define LOCK_DIR        1
#define UNLOCK_DIR      2
#define MOT_BREAK_CHECK 3

/*
  Typedefs
*/

/*
  Port pin definitions
*/
//pin definitions for motor handling
#define OUT_NOT_MOTOR_UNLOCK        35//purple wire
#define OUT_NOT_MOTOR_LOCK          34//gray wire
#define IN_NOT_LOCK_SWITCH          36
#define IN_NOT_MOTOR_SWITCH         37

//definition for onboard LED on Arduino Mega 2560
#define OUT_LED 13

/*
  Public Variables
*/

/*
  Public Function Prototypes
*/
	extern void MotorLockInit();
	extern void MotorLockHandler();

  extern void setMotorLockCommand(unsigned char ubCommand);
  extern unsigned char getMotorLockState();
  extern unsigned char getMotorLockState2();

  extern String MOT_ZustandZK(unsigned char ubyStat);
  extern bool MOT_Geschlossen(void);
  extern void MOT_ParZeiten(int iAdr, int iWert);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* MOTORLOCKHBRIDGE_H_ */
