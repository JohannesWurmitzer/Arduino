/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer (Welando GmbH)
  All rights reserved.
*/

/*
  File Name:      ArdDanConf.h
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
#ifndef ARDDANCONF_H_
#define ARDDANCONF_H_
#define ARDDANCONF_V 100     // Arduino Scheduler Config Version
#ifdef __cplusplus
extern "C"{
#endif
/*
  Includes
*/
/*
  Macros / Defines
*/
#define DAN_LCD_COLS  24              // [-] number of colums of characters
#define DAN_LCD_ROWS  2               // [-] number of rows of characters

/*
  Typedefs
*/

/*
  Port pin definitions
*/


#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* ARDDANCONF_H_ */
