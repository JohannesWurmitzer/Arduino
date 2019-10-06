/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer (Welando GmbH)
  All rights reserved.
*/

/*
  File Name:      ArdComASCConf.h
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Maximilian Johannes Wurmitzer
  Created on:     2019-10-02
  derived from:   
*/
/*  History - Headerfile should have the version of the associated module file
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)

  V1.00 2019-10-02  JoWu
    - Creation
     
*/
/*  todo-list
  - do not know...
*/
#ifndef ARDCOMASCCONF_H_
#define ARDCOMASCCONF_H_
#define ARDCOMASCCONF_V 100     // Arduino Communication ASC Config Version
#ifdef __cplusplus
extern "C"{
#endif
/*
  Includes
*/
/*
  Macros / Defines
*/
#define COM_ASC_PORT        0         // 0 ... Serial, 1 ... Serial1, 2 ... Serial2
#define COM_ASC_BAUDRATE    115200    // [bps] Baudrate

/*
  Typedefs
*/

/*
  Port pin definitions
*/
#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* ARDSCHEDCONF_H_ */
