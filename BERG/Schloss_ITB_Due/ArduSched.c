/*
  Copyright (c) 2017, E.C.E. Wurmitzer GmbH
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

  Overhead for task timing: 410 bytes flash, 45 bytes RAM
      Output of IDE with empty tasks and commented "EN_OUTPUT_TASKTEST_SIGNALS", based on Arduino
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
  - do not know...
*/

/*
  Includes
*/

#include "ArduSched.h"

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

/*
  Global Variables (global only in this module [C-File])
*/
                                 //TaskTimeIntervals for: {    Task1     ,      Task2     ,     Task3      , .....                       ,Task9}:
  unsigned long agulTaskTimeIntervals[MAX_NUM_OF_TASKS] = {TASK1_INTERVAL, TASK2_INTERVAL, TASK3_INTERVAL, NONE, NONE, NONE, NONE, NONE, NONE};
  
  unsigned long agulTickCnt[MAX_NUM_OF_TASKS] = {0,0,0,0,0,0,0,0,0};  //0 to "USED_NUM_OF_TASKS" array elements of "agulTickCnt" counts up with
                                                                      //each "SCHEDULER_TICK"
                                                                      
/*
  Private Function Prototypes
*/
  
/*
  Public Function Prototypes
*/
  void ArduSchedInit();
  void ArduSchedHandler();
  void Task1();
  void Task2();
  void Task3();
  void Task4();
  void Task5();
  void Task6();
  void Task7();
  void Task8();
  void Task9();

/*
 implementation of functions
*/
void ArduSchedInit(){
  // serielle Kommunikation initialisieren
  //Serial.begin(9600);

#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  pinMode(OUT_Task1, OUTPUT);
  pinMode(OUT_Task2, OUTPUT);
  pinMode(OUT_Task3, OUTPUT);
  
  digitalWrite(OUT_Task1, LOW);
  digitalWrite(OUT_Task2, LOW);
  digitalWrite(OUT_Task3, LOW);
#endif
}

void ArduSchedHandler() {
  unsigned long lulMillis;                             //milli seconds since boot of �C (the millis() function and
                                                      //this variable overflow at 4294967295; so 4294967295 + 1 = 0)
  static unsigned long rulMillisToNextTick = 0;       //value to generate next tick; increase with "SCHEDULER_TICK";
                                                      //overflow at 4294967295
  static unsigned long rulMillisToNextTickOld = 0;    //to check if an overflow happens or one task was longer than
                                                      //the tick interval
  
  static boolean rboNextTickOvFlow = 0;               //indicates a variable overflow of "rulMillisToNextTick"                
  
  unsigned char lubIdx;                               //just an index for example usable for loops
  
  //****************************generate scheduler tick interval and increase tick counters***************************
  lulMillis = millis();

  if(rboNextTickOvFlow && (lulMillis <= rulMillisToNextTick))//reset overflow bit
  {
    rboNextTickOvFlow = false;
  }
  if(!rboNextTickOvFlow && (lulMillis >= rulMillisToNextTick)){
    for(lubIdx=0; lubIdx < USED_NUM_OF_TASKS; lubIdx++){
      agulTickCnt[lubIdx]++; //increase tick count (separated for each task to avoid additional overflow handling) 
    }
    rulMillisToNextTickOld = rulMillisToNextTick;
    rulMillisToNextTick += SCHEDULER_TICK; 
    if (rulMillisToNextTick < lulMillis)//variable overflow or any task processing time was longer than tick interval
    {
      if(rulMillisToNextTick < rulMillisToNextTickOld){//rulMillisToNextTick overflow
        rboNextTickOvFlow = true;
      }
    }
  }
  //******************************************************************************************************************

  //***************************************call time triggered task functions*****************************************

  for(lubIdx=0; lubIdx < USED_NUM_OF_TASKS; lubIdx++){
    if(agulTickCnt[lubIdx] >= agulTaskTimeIntervals[lubIdx]){
      //call function
      agulTickCnt[lubIdx] = 0;

      switch(lubIdx){
        case TASK_1:
          Task1();
        break;

        case TASK_2:
          Task2();
        break;

        case TASK_3:
          Task3();
        break;

        case TASK_4:
          Task4();
        break;

        case TASK_5:
          Task5();
        break;

        case TASK_6:
          Task6();
        break;

        case TASK_7:
          Task7();
        break;

        case TASK_8:
          Task8();
        break;

        case TASK_9:
          Task9();
        break;

        default:
        break;
      }
    } 
  }
  //******************************************************************************************************************

}


