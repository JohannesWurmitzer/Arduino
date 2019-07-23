/*
  Copyright (c) 2017, E.C.E. Wurmitzer GmbH
  All rights reserved.
*/

/*
  File Name:      ArduSched.h
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Edmund Titz
  Created on:     2017-10-22
  derived from:   
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
#ifndef ARDUSCHED_H_
#define ARDUSCHED_H_

#ifdef __cplusplus
extern "C"{
#endif
	
//general
#define NONE                0
#define NOT_APPLICABLE      0

//timing definitions
#define SCHEDULER_TICK      10                      //   10ms tick intervall (can be reduced if faster tasks are needed) !derived from oscillator => tolerance of oscillator => no real time! Also jitter!
#define TASK_10_MILLIS      10 / SCHEDULER_TICK     //   10ms => no real time!
#define TASK_50_MILLIS      50 / SCHEDULER_TICK     //   50ms => no real time!
#define TASK_100_MILLIS     100 / SCHEDULER_TICK    //  100ms => no real time!
#define TASK_250_MILLIS     250 / SCHEDULER_TICK    //  250ms => no real time!
#define TASK_500_MILLIS     500 / SCHEDULER_TICK    //  500ms => no real time!
#define TASK_1000_MILLIS    1000 / SCHEDULER_TICK   // 1000ms => no real time!
#define TASK_1_SECOND       TASK_1000_MILLIS        //  1s    => no real time!
#define TASK_60_SECONDS     60 * TASK_1000_MILLIS   // 60s    => no real time!
#define TASK_1_MINUTE       TASK_60_SECONDS         //  1m    => no real time!
#define TASK_60_MINUTES     60 * TASK_1_MINUTE      // 60m    => no real time!
#define TASK_1_HOUR         TASK_60_MINUTES         //  1h    => no real time!
#define TASK_24_HOURS       24 * TASK_1_HOUR        // 24h    => no real time!
#define TASK_1_DAY          TASK_24_HOURS           //  1d    => no real time!
#define TASK_7_DAYS         7 * TASK_1_DAY          //  7d    => no real time!
#define TASK_1_WEEK         TASK_7_DAYS             //  1week => no real time!  1week = 604800000ms => no overflow of long variable within one week

#define TASK1_INTERVAL      TASK_100_MILLIS          //interval for task1 (time distance from call to next call)
#define TASK2_INTERVAL      TASK_250_MILLIS         //interval for task2 (time distance from call to next call)
#define TASK3_INTERVAL      TASK_1_SECOND           //interval for task3 (time distance from call to next call)
#define TASK4_INTERVAL      TASK_1_SECOND           //interval for task4 (time distance from call to next call)
#define TASK5_INTERVAL      TASK_1_SECOND           //interval for task5 (time distance from call to next call)
#define TASK6_INTERVAL      TASK_1_SECOND           //interval for task6 (time distance from call to next call)
#define TASK7_INTERVAL      TASK_1_SECOND           //interval for task7 (time distance from call to next call)
#define TASK8_INTERVAL      TASK_1_SECOND           //interval for task8 (time distance from call to next call)
#define TASK9_INTERVAL      TASK_1_SECOND           //interval for task9 (time distance from call to next call)

//program structure definitions
#define MAX_NUM_OF_TASKS    9                       //defines the maximum allowed number of tasks (could be easily increased with enhencement of code)
#define USED_NUM_OF_TASKS   3                       //defines the number of applied tasks
#define TASK_1              0
#define TASK_2              1
#define TASK_3              2
#define TASK_4              3
#define TASK_5              4
#define TASK_6              5
#define TASK_7              6
#define TASK_8              7
#define TASK_9              8
//#define EN_OUTPUT_TASKTEST_SIGNALS                  //if defined, task 1,2 and 3 outputs "HIGH" for 1ms at the beginning of each task. This is useful
                                                    //for timing tests.
/*
  Typedefs
*/

/*
  Port pin definitions
*/
//Port pins for task timing tests
#define OUT_Task1 41  //Arduino pin number 41 (not available at "Arduino UNO" for example)
#define OUT_Task2 42
#define OUT_Task3 43
#define OUT_Task4 44
#define OUT_Task5 45
#define OUT_Task6 46
#define OUT_Task7 47
#define OUT_Task8 48
#define OUT_Task9 49

/*
  Public Variables
*/

/*
  Public Function Prototypes
*/
	extern void ArduSchedInit();
	extern void ArduSchedHandler();

  extern void Task1();
  extern void Task2();
  extern void Task3();
  extern void Task4();
  extern void Task5();
  extern void Task6();
  extern void Task7();
  extern void Task8();
  extern void Task9();

#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* ARDUSCHED_H_ */
