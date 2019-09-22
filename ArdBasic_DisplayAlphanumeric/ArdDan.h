/*
  Copyright (c) 2019, Welando GmbH
  All rights reserved.
*/

/*
  File Name:      ArduDan.h
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Edmund Titz
  Created on:     2017-10-22
  derived from:   
*/
/*  History - Headerfile should have the version of the associated module file
    Versioning: VX.YZ
      X..increase for big change or bugfix
      Y..incr. for enhanced functionality
      Z..incr. for structure or documentation changes)

  V1.00 2019-09-22  JoWu
    - Creation
     
*/
/*  todo-list
  - do not know...
*/
#ifndef ARDDAN_H_
#define ARDDAN_H_
#define ARDDAN_V 100     // Arduino Scheduler Version
#ifdef __cplusplus
extern "C"{
#endif
/*
  Includes
*/
/*
  Macros / Defines
*/

#include "ArdDanConf.h"  // User Configuration for each Project

/*
  Typedefs
*/

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
extern void ArdDanSetup();
extern void ArdDanLoop();
extern void ArdDanTask();

// to calculate differences between timer values if they can roll over
extern unsigned long tools_ulSubRollOver(unsigned long Minuend, unsigned long Subtrahend);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* ARDDAN_H_ */
