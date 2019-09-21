/*
  Copyright (c) 2019, Welando GmbH
  All rights reserved.
*/
/*  Function description

  Scheduler based on "millis()" function. It is important that the processing times of all tasks
  are shorter than the shortest task intervall (task1)! Otherwise task executions are time shifted!
  To avoid problems with processing times set "SCHEDULER_TICK" in a good manner. In addition each
  task time definition must be a multiple of "SCHEDULER_TICK". Otherwise rounding errors causes
  time deviations!
  For a proper scheduling, task1 interval must be the shortest, followed by task2 and so on. The
  same intervall for two successive tasks is possible without any problems.

  V4.00: ca. 1814 Bytes Program Memory, 94 Bytes Dynamic Memory
  
  Old: Overhead for task timing: 410 bytes flash, 45 bytes RAM
      Output of IDE with empty tasks and commented "TASK_TEST_OUTPUT_EN", based on Arduino
      Mega 2560:
      
      "Sketch uses 1066 bytes (0%) of program storage space. Maximum is 253952 bytes.
      Global variables use 54 bytes (0%) of dynamic memory, leaving 8138 bytes for
      local variables. Maximum is 8192 bytes."
*/
/*
  File Name:      ArduSched.c
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Edmund Titz
  Created on:     2017-10-22
  derived from:   
*/

/*  History 
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)

  V5.00 2019-09-21  JoWu
    - Added ArduSchedConf.h to enable project-specific Configuration

  V4.00 2019-07-23  JoWu
    - Bugfix; Overflow Handling fixed
    - Improvement; Roughness, agulTaskTimeIntervals moved to flash
    
  V3.00 2017-11-18  EdTi
    - bugfix -> changed "unsigned char agubTaskTimeIntervals" to long variable (agulTaskTimeIntervals) for bigger intervals
    
  V2.00 2017-10-30  EdTi
    - bugfix -> added "rulMillisToNextTickOld" to check if an overflow happens or one task was
      longer than the tick interval. Without this bugfix a task that needs longer processing
      time as the time of the tick interval sets the "rboNextTickOvFlow" bit!

   - Splitted into header and c-file (FW module)
                                                        
  V1.00 2017-10-22  EdTi
    - creation of file
*/

/*  todo-list
  open, 2019-07-23; JoWu; Task-Funktionen extern definieren, eventuell auch void in die Argumente-Liste schreiben
  open, 2019-07-23; JoWu; PROGMEM attribute does not work (https://www.avrfreaks.net/forum/atmega-2560-progmem-problem)
*/

/*
  Includes
*/
#include <avr/pgmspace.h>
#include "ArduSched.h"
#include "Arduino.h"
/*
  Macros / Defines
*/
#define TASK_1              0
#define TASK_2              1
#define TASK_3              2
#define TASK_4              3
#define TASK_5              4
#define TASK_6              5
#define TASK_7              6
#define TASK_8              7
#define TASK_9              8
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

/*
    Private Variables
*/
static boolean rboSchedRunning;             // indicates, that scheduler is already running
static unsigned long rulMillisOld;                  // [ms] old value

unsigned long rulMillisElapsed;              // [ms] time elapsed since last scheduler timer tick
unsigned long rulMillis;    // [ms] milli seconds since boot of µC (the millis() function and
                            //this variable overflow at 4294967295; so 4294967295 + 1 = 0)


/*
  Global Variables (global only in this module [C-File])
  https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
*/
//TaskTimeIntervals for: {    Task1     ,      Task2     ,     Task3      , .....                       ,Task9}:
const /*PROGMEM*/ unsigned long agulTaskTimeIntervals[TASK_MAX_NUM] = {
    TASK1_INTERVAL / SCHEDULER_TICK, 
    TASK2_INTERVAL / SCHEDULER_TICK, 
    TASK3_INTERVAL / SCHEDULER_TICK, 
    TASK4_INTERVAL / SCHEDULER_TICK, 
    TASK5_INTERVAL / SCHEDULER_TICK, 
    TASK6_INTERVAL / SCHEDULER_TICK, 
    TASK7_INTERVAL / SCHEDULER_TICK, 
    TASK8_INTERVAL / SCHEDULER_TICK
    };
  
unsigned long gaulTickCnt[TASK_USED_NUM];  //0 to "TASK_USED_NUM" array elements of "gaulTickCnt" counts up with
                                                                      //each "SCHEDULER_TICK"
  
/*
  Public Function Prototypes
*/
void ArduSchedInit();
void ArduSchedHandler();

/*
  Private Function Prototypes
*/

/*
 implementation of public functions
*/
void ArduSchedInit(){
  // serielle Kommunikation initialisieren
  //Serial.begin(9600);

#ifdef TASK_TEST_OUTPUT_EN
  pinMode(OUT_TaskBusy, OUTPUT);
  pinMode(OUT_Task1, OUTPUT);
  pinMode(OUT_Task2, OUTPUT);
  pinMode(OUT_Task3, OUTPUT);
  pinMode(OUT_Task4, OUTPUT);
  pinMode(OUT_Task5, OUTPUT);
  pinMode(OUT_Task6, OUTPUT);
  pinMode(OUT_Task7, OUTPUT);
  pinMode(OUT_Task8, OUTPUT);
  
  digitalWrite(OUT_TaskBusy, LOW);
  digitalWrite(OUT_Task1, LOW);
  digitalWrite(OUT_Task2, LOW);
  digitalWrite(OUT_Task3, LOW);
  digitalWrite(OUT_Task4, LOW);
  digitalWrite(OUT_Task5, LOW);
  digitalWrite(OUT_Task6, LOW);
  digitalWrite(OUT_Task7, LOW);
  digitalWrite(OUT_Task8, LOW);
#endif
  memset(gaulTickCnt, 0, sizeof(gaulTickCnt));
  rboSchedRunning = false;
}

void ArduSchedHandler() {
  unsigned char lubIdx;       //just an index for example usable for loops

#ifdef TASK_TEST_OUTPUT_EN
//  digitalWrite(OUT_TaskBusy, HIGH);
#endif
  //****************************generate scheduler tick interval and increase tick counters***************************
  rulMillis = millis();
  rulMillis = rulMillis; //+ 4294960000; //4294967295

  if (rboSchedRunning){
    // Scheduler is running, so we have an old time
    if (rulMillis >= rulMillisOld){
      // we don't have an overrun
      rulMillisElapsed += rulMillis - rulMillisOld;
    }
    else{
      // we have a 32 Bit overrun
      rulMillisElapsed += rulMillis + (0 - rulMillisOld);
    }
  }
  else{
    // Scheduler is not running, so we use the actual time since reset as old time
    rulMillisElapsed = 0;
    rboSchedRunning = true;
  }
  rulMillisOld = rulMillis;
  if (rulMillisElapsed > 1000){
    rulMillisElapsed = 1000;
  }
  if(rulMillisElapsed >= SCHEDULER_TICK){
    rulMillisElapsed -= SCHEDULER_TICK;
    for(lubIdx=0; lubIdx < TASK_USED_NUM; lubIdx++){
      gaulTickCnt[lubIdx]++; //increase tick count (separated for each task to avoid additional overflow handling) 
    }
  }
  //******************************************************************************************************************

  //***************************************call time triggered task functions*****************************************

  for(lubIdx=0; lubIdx < TASK_USED_NUM; lubIdx++){
    if(gaulTickCnt[lubIdx] >= agulTaskTimeIntervals[lubIdx]){
      //call function
      gaulTickCnt[lubIdx] = 0;

      switch(lubIdx){
        case TASK_1:
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_TaskBusy, HIGH);
          digitalWrite(OUT_Task1, HIGH);
#endif
          Task1();  // call external function
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_Task1, LOW);
#endif
        break;

        case TASK_2:
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_TaskBusy, HIGH);
          digitalWrite(OUT_Task2, HIGH);
#endif
          Task2();
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_Task2, LOW);
#endif
        break;

        case TASK_3:
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_TaskBusy, HIGH);
          digitalWrite(OUT_Task3, HIGH);
#endif
          Task3();
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_Task3, LOW);
#endif
        break;

        case TASK_4:
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_TaskBusy, HIGH);
          digitalWrite(OUT_Task4, HIGH);
#endif
          Task4();
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_TaskBusy, HIGH);
          digitalWrite(OUT_Task4, LOW);
#endif
        break;

        case TASK_5:
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_TaskBusy, HIGH);
          digitalWrite(OUT_Task5, HIGH);
#endif
          Task5();
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_Task5, LOW);
#endif
        break;

        case TASK_6:
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_TaskBusy, HIGH);
          digitalWrite(OUT_Task6, HIGH);
#endif
          Task6();
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_Task6, LOW);
#endif
        break;

        case TASK_7:
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_TaskBusy, HIGH);
          digitalWrite(OUT_Task7, HIGH);
#endif
          Task7();
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_Task7, LOW);
#endif
        break;

        case TASK_8:
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_TaskBusy, HIGH);
          digitalWrite(OUT_Task8, HIGH);
#endif
          Task8();
#ifdef TASK_TEST_OUTPUT_EN
          digitalWrite(OUT_Task8, LOW);
#endif
        break;

        default:
        break;
      }
    } 
  }
  //******************************************************************************************************************
#ifdef TASK_TEST_OUTPUT_EN
  digitalWrite(OUT_TaskBusy, LOW);
#endif

}

/*
 implementation of private functions
*/
//void ArduSchedIncTick(unsigned long ){
//}
