/*
  Copyright (c) 2017, E.C.E. Wurmitzer GmbH
  All rights reserved.
*/

/*
  File Name:      SoundAndLed.h
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Edmund Titz
  Created on:     2017-11-17
  derived from:   
*/

/*  History - Headerfile should have the version of the associated module file
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)
     
  2020-02-28  V1.00 JoWu
    - change port pin for OUT_TM_TEST_SIG
  
*/
/*  todo-list
  - do not know...
*/

/*
  Includes
*/
#include "Arduino.h"
#include "TimerOne.h"
 
/*
  Macros / Defines
*/
#ifndef SOUNDANDLED_H_
#define SOUNDANDLED_H_

#ifdef __cplusplus
extern "C"{
#endif
	
//general
#define NONE                    0
#define NOT_APPLICABLE          0

//definitions for state machine
#define SAL_IDLE                0
#define SAL_HANDLE_BEEPER       1
  #define SAL_SUB_BEEPER_IDLE   11
#define SAL_HANDLE_ERR_LED      2
  #define SAL_SUB_ERR_LED_IDLE  21
#define SAL_HANDLE_OK_LED       3
  #define SAL_SUB_OK_LED_IDLE   31

//definitions for beeper set function
#define BEEP_DETECT_TAG         2
#define BEEP_UNKNOWN_ID         3
#define BEEP_TEACH_USER         4
#define BEEP_TEACH_USER_END     5
#define BEEP_TEACH_ARTICLE      6
#define BEEP_TEACH_ARTICLE_END  7
#define BEEP_ERASE_IDS          8
#define BEEP_ERASE_IDS_END      9

//definitions for OK and ERROR LED set function
#define LED_CONST_OFF           2
#define LED_CONST_ON            3
#define LED_TAG_CHECK           4
#define LED_TAG_OK              5
#define LED_TAG_NOK             6
#define LED_TEACH_USER          7
#define LED_TEACH_USER_END      8
#define LED_TEACH_ARTICLE       9
#define LED_TEACH_ARTICLE_END   10
#define LED_ERASE_IDS           11
#define LED_ERASE_IDS_END       12
#define LED_SYSTEM_ERROR        13

/*
  Typedefs
*/

/*
  Port pin definitions
*/
//pin definitions for beeper and LED handling
#define OUT_BEEPER      31//Piezzo beeper
#define OUT_ERROR_LED   32//red LED
#define OUT_OK_LED      30//green LED //JoWu ITB-Board, sonst 33
#define OUT_TM_TEST_SIG 11  // 40//output to check processing time of timer1 ISR
//#define EN_OUTPUT_TIMINGTEST_SIGNALS                //if defined, timer1 ISR outputs "HIGH" at the beginning of ISR and LOW at the end. This is useful
                                                    //for timing tests.
/*
  Public Variables
*/

/*
  Public Function Prototypes
*/
  extern void SoundAndLedInit();
  extern void SoundAndLedHandler(); 
  extern void Beeper(unsigned char ub_SoundSequence);   
  extern void ErrorLedSet(unsigned char ub_LED_Sequence);
  extern void OkLedSet(unsigned char ub_LED_Sequence);
  
#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* SOUNDANDLED_H_ */
