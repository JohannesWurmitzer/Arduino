/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer (Welando GmbH)
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
      Output of IDE with empty tasks and commented "TASK_TEST_PO_EN", based on Arduino
      Mega 2560:
      
      "Sketch uses 1066 bytes (0%) of program storage space. Maximum is 253952 bytes.
      Global variables use 54 bytes (0%) of dynamic memory, leaving 8138 bytes for
      local variables. Maximum is 8192 bytes."
*/
/*
  File Name:      ArdSched.c
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Maximilian Johannes Wurmitzer
  Created on:     2019-07-23
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
    
*/

/*  todo-list
  open, 2019-07-23; JoWu; Task-Funktionen extern definieren, eventuell auch void in die Argumente-Liste schreiben
  open, 2019-07-23; JoWu; PROGMEM attribute does not work (https://www.avrfreaks.net/forum/atmega-2560-progmem-problem)
*/

/*
  Includes
*/
#include <avr/pgmspace.h>
#include "ArdSched.h"
#include "Arduino.h"
/*
  Macros / Defines
*/


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
unsigned long gaulTaskTime[TASK_USED_NUM];  // [µs] Time consumed by task

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
bool gboolTaskRdy[TASK_USED_NUM];

/*
  Public Function Prototypes
*/
void ArdSchedSetup();
void ArdSchedLoop();

/*
  Private Function Prototypes
*/

/*
 implementation of public functions
*/
void ArdSchedSetup(){
  // serielle Kommunikation initialisieren
  //Serial.begin(9600);

#ifdef TASK_TEST_PO_EN
  pinMode(PO_TaskBusy, OUTPUT);
  pinMode(PO_Task1, OUTPUT);
  pinMode(PO_Task2, OUTPUT);
  pinMode(PO_Task3, OUTPUT);
  pinMode(PO_Task4, OUTPUT);
  pinMode(PO_Task5, OUTPUT);
  pinMode(PO_Task6, OUTPUT);
  pinMode(PO_Task7, OUTPUT);
  pinMode(PO_Task8, OUTPUT);
  
  digitalWrite(PO_TaskBusy, LOW);
  digitalWrite(PO_Task1, LOW);
  digitalWrite(PO_Task2, LOW);
  digitalWrite(PO_Task3, LOW);
  digitalWrite(PO_Task4, LOW);
  digitalWrite(PO_Task5, LOW);
  digitalWrite(PO_Task6, LOW);
  digitalWrite(PO_Task7, LOW);
  digitalWrite(PO_Task8, LOW);
#endif
  memset(gaulTickCnt, 0, sizeof(gaulTickCnt));
  memset(gboolTaskRdy, 0, sizeof(gboolTaskRdy));
  
  rboSchedRunning = false;
}

void ArdSchedLoop() {
  unsigned char lubIdx;       //just an index for example usable for loops

#ifdef TASK_TEST_PO_EN
//  digitalWrite(PO_TaskBusy, HIGH);
#endif
  //  generate scheduler tick interval and increase tick counters
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

  // set the right task functions ready
  for(lubIdx=0; lubIdx < TASK_USED_NUM; lubIdx++){
    if(gaulTickCnt[lubIdx] >= agulTaskTimeIntervals[lubIdx]){
      //call function
      gaulTickCnt[lubIdx] = 0;
      gboolTaskRdy[lubIdx] = true;
    } 
  }
#ifdef TASK_TEST_PO_EN
//  digitalWrite(PO_TaskBusy, LOW);
#endif

}

byte rbyTaskIdRunning;
unsigned long rulTimeTaskStart, rulTimeTaskStop;

bool ArdSchedTaskRdyStart(byte TaskId){
  if (TaskId < TASK_USED_NUM && gboolTaskRdy[TaskId]){
    rulTimeTaskStart = micros();
    rbyTaskIdRunning = TaskId;
    gboolTaskRdy[TaskId] = false;
    switch(TaskId){
      case TASK_1:
#ifdef TASK_TEST_PO_EN
        digitalWrite(PO_TaskBusy, HIGH);
        digitalWrite(PO_Task1, HIGH);
#endif
      break;

      case TASK_2:
#ifdef TASK_TEST_PO_EN
        digitalWrite(PO_TaskBusy, HIGH);
        digitalWrite(PO_Task2, HIGH);
#endif
      break;

      case TASK_3:
#ifdef TASK_TEST_PO_EN
        digitalWrite(PO_TaskBusy, HIGH);
        digitalWrite(PO_Task3, HIGH);
#endif
      break;

      case TASK_4:
#ifdef TASK_TEST_PO_EN
        digitalWrite(PO_TaskBusy, HIGH);
        digitalWrite(PO_Task4, HIGH);
#endif
      break;

      case TASK_5:
#ifdef TASK_TEST_PO_EN
        digitalWrite(PO_TaskBusy, HIGH);
        digitalWrite(PO_Task5, HIGH);
#endif
      break;

      case TASK_6:
#ifdef TASK_TEST_PO_EN
        digitalWrite(PO_TaskBusy, HIGH);
        digitalWrite(PO_Task6, HIGH);
#endif
      break;

      case TASK_7:
#ifdef TASK_TEST_PO_EN
        digitalWrite(PO_TaskBusy, HIGH);
        digitalWrite(PO_Task7, HIGH);
#endif
      break;

      case TASK_8:
#ifdef TASK_TEST_PO_EN
        digitalWrite(PO_TaskBusy, HIGH);
        digitalWrite(PO_Task8, HIGH);
#endif
      break;

      default:
      break;
    }
    return true;
  }
  else{
    return false;
  }  
}
extern void ArdSchedTaskStop(void){
  switch(rbyTaskIdRunning){
    case TASK_1:
#ifdef TASK_TEST_PO_EN
      digitalWrite(PO_Task1, LOW);
#endif
    break;

    case TASK_2:
#ifdef TASK_TEST_PO_EN
      digitalWrite(PO_Task2, LOW);
#endif
    break;

    case TASK_3:
#ifdef TASK_TEST_PO_EN
      digitalWrite(PO_Task3, LOW);
#endif
    break;

    case TASK_4:
#ifdef TASK_TEST_PO_EN
      digitalWrite(PO_Task4, LOW);
#endif
    break;

    case TASK_5:
#ifdef TASK_TEST_PO_EN
      digitalWrite(PO_Task5, LOW);
#endif
    break;

    case TASK_6:
#ifdef TASK_TEST_PO_EN
      digitalWrite(PO_Task6, LOW);
#endif
    break;

    case TASK_7:
#ifdef TASK_TEST_PO_EN
      digitalWrite(PO_Task7, LOW);
#endif
    break;

    case TASK_8:
#ifdef TASK_TEST_PO_EN
      digitalWrite(PO_Task8, LOW);
#endif
    break;

    default:
    break;
  }
  if (rbyTaskIdRunning < TASK_USED_NUM){
    rulTimeTaskStop = micros();
    if (rulTimeTaskStop >= rulTimeTaskStart){
      // we don't have an overrun
      if (gaulTaskTime[rbyTaskIdRunning] < rulTimeTaskStop - rulTimeTaskStart){
        gaulTaskTime[rbyTaskIdRunning] = rulTimeTaskStop - rulTimeTaskStart;
      }
    }
    else{
      // we have a 32 Bit overrun
      if (gaulTaskTime[rbyTaskIdRunning] < rulTimeTaskStop + (0 - rulTimeTaskStart)){
        gaulTaskTime[rbyTaskIdRunning] = rulTimeTaskStop + (0 - rulTimeTaskStart);
      }
    }
  }
  rbyTaskIdRunning = 255;
#ifdef TASK_TEST_PO_EN
  digitalWrite(PO_TaskBusy, LOW);
#endif 
}

/*
 implementation of private functions
*/
//void ArdSchedIncTick(unsigned long ){
//}
