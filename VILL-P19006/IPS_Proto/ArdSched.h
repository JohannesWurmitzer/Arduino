/*
  Copyright (c) 2019, Welando GmbH
  All rights reserved.
*/

/*
  File Name:      ArdSched.h
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Edmund Titz
  Created on:     2017-10-22
  derived from:   
*/
/*  History - Headerfile should have the version of the associated module file
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)

  V5.00 2019-07-23  JoWu
    - Moved Config to ArdSchedConf.h
    
  V4.00 2019-07-23  JoWu
    - Improvement; Coding info, Introduced ARDUSCHED_V for version information
     
*/
/*  todo-list
  - do not know...
*/
#ifndef ARDSCHED_H_
#define ARDSCHED_H_
#define ARDSCHED_V 500     // Arduino Scheduler Version
#ifdef __cplusplus
extern "C"{
#endif
/*
  Includes
*/
#include "Arduino.h"
/*
  Macros / Defines
*/
#define TASK_MAX_NUM        8                     // defines the maximum allowed number of tasks (could be easily increased with enhancement of code)
#define SCHEDULER_TICK      5                     // [ms] tick intervall (can be reduced if faster tasks are needed) !derived from oscillator => tolerance of oscillator => no real time! Also jitter!

#define TASK_1              0
#define TASK_2              1
#define TASK_3              2
#define TASK_4              3
#define TASK_5              4
#define TASK_6              5
#define TASK_7              6
#define TASK_8              7

#include "ArdSchedConf.h"  // User Configuration for each Project

/*
  Typedefs
*/

/*
  Port pin definitions
*/

/*
  Public Variables
*/
extern unsigned long gaulTaskTime[TASK_USED_NUM];  // [µs] time consumed by tasks

/*
  Public Function Prototypes required
*/

/*
extern void Task1(void);
extern void Task2(void);
extern void Task3(void);
extern void Task4(void);
extern void Task5(void);
extern void Task6(void);
extern void Task7(void);
extern void Task8(void);
*/

extern bool ArdSchedTaskRdyStart(byte TaskId);
extern void ArdSchedTaskStop(void);

/*
  Public Function Prototypes provided
*/
extern void ArdSchedSetup();
extern void ArdSchedLoop();



#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* ARDSCHED_H_ */
