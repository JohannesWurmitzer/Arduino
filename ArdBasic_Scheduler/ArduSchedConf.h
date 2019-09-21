/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer
  All rights reserved.
*/

/*
  File Name:      ArduSchedConf.h
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Maximilian Johannes Wurmitzer
  Created on:     2019-09-22
  derived from:   
*/
/*  History - Headerfile should have the version of the associated module file
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)

  V1.00 2019-09-22  JoWu
    - Creation
     
*/
/*  todo-list
  - do not know...
*/
#ifndef ARDUSCHEDCONF_H_
#define ARDUSCHEDCONF_H_
#define ARDUSCHEDCONF_V 100     // Arduino Scheduler Config Version
#ifdef __cplusplus
extern "C"{
#endif
/*
  Includes
*/
/*
  Macros / Defines
*/
#define TASK_10_MILLIS      10                    //   10ms => no real time!
#define TASK_50_MILLIS      50                    //   50ms => no real time!
#define TASK_100_MILLIS     100                   //  100ms => no real time!
#define TASK_250_MILLIS     250                   //  250ms => no real time!
#define TASK_500_MILLIS     500                   //  500ms => no real time!
#define TASK_1000_MILLIS    1000                  // 1000ms => no real time!
#define TASK_1_SECOND       TASK_1000_MILLIS      //  1s    => no real time!
#define TASK_60_SECONDS     60 * TASK_1000_MILLIS // 60s    => no real time!
#define TASK_1_MINUTE       TASK_60_SECONDS       //  1m    => no real time!
#define TASK_60_MINUTES     60 * TASK_1_MINUTE    // 60m    => no real time!
#define TASK_1_HOUR         TASK_60_MINUTES       //  1h    => no real time!
#define TASK_24_HOURS       24 * TASK_1_HOUR      // 24h    => no real time!
#define TASK_1_DAY          TASK_24_HOURS         //  1d    => no real time!
#define TASK_7_DAYS         7 * TASK_1_DAY        //  7d    => no real time!
#define TASK_1_WEEK         TASK_7_DAYS           //  1week => no real time!  1week = 604800000ms => no overflow of long variable within one week


#define TASK1_INTERVAL      10         // [ms] interval for task1 (time distance from call to next call)
#define TASK2_INTERVAL      25         // [ms] interval for task2 (time distance from call to next call)
#define TASK3_INTERVAL      50        // [ms] interval for task3 (time distance from call to next call)
#define TASK4_INTERVAL      100        // [ms] interval for task4 (time distance from call to next call)
#define TASK5_INTERVAL      150        // [ms] interval for task5 (time distance from call to next call)
#define TASK6_INTERVAL      200        // [ms] interval for task6 (time distance from call to next call)
#define TASK7_INTERVAL      250        // [ms] interval for task7 (time distance from call to next call)
#define TASK8_INTERVAL      300        // [ms] interval for task8 (time distance from call to next call)

//program structure definitions
#define TASK_USED_NUM   8                       //defines the number of applied tasks


#define TASK_TEST_OUTPUT_EN             // if defined, OUT_TaskX outputs "HIGH" while Task is active
/*
  Typedefs
*/

/*
  Port pin definitions
*/
//Port pins for task timing tests
#define OUT_TaskBusy  40
#define OUT_Task1     41  //Arduino pin number 41 (not available at "Arduino UNO" for example)
#define OUT_Task2     42
#define OUT_Task3     43
#define OUT_Task4     44
#define OUT_Task5     45
#define OUT_Task6     46
#define OUT_Task7     47
#define OUT_Task8     48



#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* ARDUSCHEDCONF_H_ */
