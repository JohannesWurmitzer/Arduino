/*
  Copyright (c) 2017, E.C.E. Wurmitzer GmbH
  All rights reserved.
*/
/*  Function description
  
*/
/*
  File Name:      SoundAndLed.c
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Edmund Titz
  Created on:     2017-11-17
  derived from:   
*/

/*  History 
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)

  2020-09-06  V111    JoWu
    - add instant write of okay and error led in case of constant on or constant off

  V1.10 2020-08-23  JoWu
    - optimized Timer1 handling
    - add missing default action to state machine to enhance stability

  V1.00 2017-11-17  EdTi
    - creation of file
*/

/*  todo-list
  - do not know...
*/

/*
  Includes
*/

#include "SoundAndLed.h"

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
unsigned char gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;
unsigned char gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
unsigned char gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE; 


static unsigned char rub_SoundAndLedState = SAL_IDLE;
static unsigned char rub_BeeperOnCntDwnSteps300ms = 0;
static unsigned char rub_BeeperOffCntDwnSteps300ms = 0;
static unsigned char rub_ErrLED_OnCntDwnSteps300ms = 0;//255 constant ON (==> rub_ErrLED_OffSteps300ms is ignored)
static unsigned char rub_ErrLED_OffCntDwnSteps300ms = 0;
static unsigned char rub_OkLED_OnCntDwnSteps300ms = 0;//255 constant ON (==> rub_OkLED_OffSteps300ms is ignored)
static unsigned char rub_OkLED_OffCntDwnSteps300ms = 0;
static unsigned char rub_BeeperSubPhase = 0;
static unsigned char rub_ErrLED_SubPhase = 0;
static unsigned char rub_OkLED_SubPhase = 0;
/*
  Private Function Prototypes
*/
  void BeeperOn(unsigned short uwFrequency);
  void BeeperOff();  
/*
  Public Function Prototypes
*/
  void SoundAndLedInit();
  void SoundAndLedHandler(); 
  void Beeper(unsigned char ub_SoundSequence);   
  void ErrorLedSet(unsigned char ub_LED_Sequence);
  void OkLedSet(unsigned char ub_LED_Sequence);
  void Tmr1_ISR();
/*
 implementation of functions
*/
//***********************************************************************************
void SoundAndLedInit(){
  pinMode(OUT_BEEPER, OUTPUT);
  pinMode(OUT_ERROR_LED, OUTPUT);
  pinMode(OUT_OK_LED, OUTPUT);

  digitalWrite(OUT_BEEPER, LOW);
  digitalWrite(OUT_ERROR_LED, LOW);
  digitalWrite(OUT_OK_LED, HIGH);
  
  // https://www.pjrc.com/teensy/td_libs_TimerOne.html
  Timer1.initialize(1000);//1000µs (1kHz); 16bit timer! parameter is long variable in micro seconds
        //Timer1.setPeriod(3000);//3000µs (333.3Hz)
  Timer1.attachInterrupt(Tmr1_ISR);
  Timer1.stop();
}
//***********************************************************************************
void SoundAndLedHandler(){//called each 100ms
/* moved variables definitions to the module variables definiton block
  static unsigned char rub_SoundAndLedState = SAL_IDLE;
  static unsigned char rub_BeeperOnCntDwnSteps300ms = 0;
  static unsigned char rub_BeeperOffCntDwnSteps300ms = 0;
  static unsigned char rub_ErrLED_OnCntDwnSteps300ms = 0;//255 constant ON (==> rub_ErrLED_OffSteps300ms is ignored)
  static unsigned char rub_ErrLED_OffCntDwnSteps300ms = 0;
  static unsigned char rub_OkLED_OnCntDwnSteps300ms = 0;//255 constant ON (==> rub_OkLED_OffSteps300ms is ignored)
  static unsigned char rub_OkLED_OffCntDwnSteps300ms = 0;
  static unsigned char rub_BeeperSubPhase = 0;
  static unsigned char rub_ErrLED_SubPhase = 0;
  static unsigned char rub_OkLED_SubPhase = 0;
*/
  //-------------------------------------------------
  if(gub_BeeperSequence == BEEP_DETECT_TAG){//this is to have highest priority for Tag detect signal
    rub_SoundAndLedState = SAL_HANDLE_BEEPER;  
  }
  switch(rub_SoundAndLedState){
    //............................................................................................
    case SAL_IDLE:
      if(gub_BeeperSequence != SAL_SUB_BEEPER_IDLE){
        rub_SoundAndLedState = SAL_HANDLE_BEEPER;  
      }
      else if(gub_ErrLED_Sequence != SAL_SUB_ERR_LED_IDLE){
        rub_SoundAndLedState = SAL_HANDLE_ERR_LED;  
      }
      else if(gub_OkLED_Sequence != SAL_SUB_OK_LED_IDLE){
        rub_SoundAndLedState = SAL_HANDLE_OK_LED; 
      }
    break;
    //............................................................................................
    case SAL_HANDLE_BEEPER:
      switch(gub_BeeperSequence){
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case SAL_SUB_BEEPER_IDLE:
          //gub_BeeperSequence = BEEP_DETECT_TAG;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case BEEP_DETECT_TAG:
          switch(rub_BeeperSubPhase){
            //----------------------------------------------------------------
            case 0:
              BeeperOn(1000);//1000Hz
              rub_BeeperOnCntDwnSteps300ms = 1;//600ms
              rub_BeeperSubPhase = 1;
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperSubPhase = 0;
                gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;  
              }
            break;
            //----------------------------------------------------------------
            default:
              BeeperOff();
              rub_BeeperSubPhase = 0;
              gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case BEEP_UNKNOWN_ID:
          switch(rub_BeeperSubPhase){
            //----------------------------------------------------------------
            case 0:
              BeeperOn(333);//333Hz
              rub_BeeperOnCntDwnSteps300ms = 2;//900ms
              rub_BeeperSubPhase = 1;
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                //rub_BeeperOffCntDwnSteps300ms--;
                rub_BeeperSubPhase = 0;
                gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;  
              }
            break;
            //----------------------------------------------------------------
            default:
              BeeperOff();
              rub_BeeperSubPhase = 0;
              gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case BEEP_TEACH_USER://2 piep high
          switch(rub_BeeperSubPhase){
            //----------------------------------------------------------------
            case 0:
              BeeperOn(2000);//2000Hz
              rub_BeeperOnCntDwnSteps300ms = 1;//600ms
              rub_BeeperSubPhase = 1;
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 2; 
              }
            break;
            //----------------------------------------------------------------
            case 2:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2500);//2500Hz
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 3;   
              }
            break;
            //----------------------------------------------------------------
            case 3:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 4;  
              }
            break;
            //----------------------------------------------------------------
            case 4:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2500);//2500Hz
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 5;   
              }
            break;
            //----------------------------------------------------------------
            case 5:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 0; 
                gub_BeeperSequence = SAL_SUB_BEEPER_IDLE; 
              }
            break;
            //----------------------------------------------------------------
            default:
              BeeperOff();
              rub_BeeperSubPhase = 0;
              gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case BEEP_TEACH_USER_END://2 piep low
          switch(rub_BeeperSubPhase){
            //----------------------------------------------------------------
            case 0:
              BeeperOn(1500);
              rub_BeeperOnCntDwnSteps300ms = 1;//600ms
              rub_BeeperSubPhase = 1;
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 2; 
              }
            break;
            //----------------------------------------------------------------
            case 2:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2000);
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 3;   
              }
            break;
            //----------------------------------------------------------------
            case 3:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 4;  
              }
            break;
            //----------------------------------------------------------------
            case 4:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2000);
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 5;   
              }
            break;
            //----------------------------------------------------------------
            case 5:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 0; 
                gub_BeeperSequence = SAL_SUB_BEEPER_IDLE; 
              }
            break;
            //----------------------------------------------------------------
            default:
              BeeperOff();
              rub_BeeperSubPhase = 0;
              gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case BEEP_TEACH_ARTICLE://3 piep high
          switch(rub_BeeperSubPhase){
            //----------------------------------------------------------------
            case 0:
              BeeperOn(2000);//2000Hz
              rub_BeeperOnCntDwnSteps300ms = 1;//600ms
              rub_BeeperSubPhase = 1;
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 2; 
              }
            break;
            //----------------------------------------------------------------
            case 2:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2500);//2500Hz
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 3;   
              }
            break;
            //----------------------------------------------------------------
            case 3:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 4;  
              }
            break;
            //----------------------------------------------------------------
            case 4:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2500);//2500Hz
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 5;   
              }
            break;
            //----------------------------------------------------------------
            case 5:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 6; 
              }
            break;
            //----------------------------------------------------------------
            case 6:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2500);//2500Hz
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 7;   
              }
            break;
            //----------------------------------------------------------------
            case 7:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 0; 
                gub_BeeperSequence = SAL_SUB_BEEPER_IDLE; 
              }
            break;
            //----------------------------------------------------------------
            default:
              BeeperOff();
              rub_BeeperSubPhase = 0;
              gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case BEEP_TEACH_ARTICLE_END://3 piep low
          switch(rub_BeeperSubPhase){
            //----------------------------------------------------------------
            case 0:
              BeeperOn(1500);
              rub_BeeperOnCntDwnSteps300ms = 1;//600ms
              rub_BeeperSubPhase = 1;
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 2; 
              }
            break;
            //----------------------------------------------------------------
            case 2:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2000);
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 3;   
              }
            break;
            //----------------------------------------------------------------
            case 3:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 4;  
              }
            break;
            //----------------------------------------------------------------
            case 4:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2000);
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 5;   
              }
            break;
            //----------------------------------------------------------------
            case 5:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 6; 
              }
            break;
            //----------------------------------------------------------------
            case 6:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2000);
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 7;   
              }
            break;
            //----------------------------------------------------------------
            case 7:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 0; 
                gub_BeeperSequence = SAL_SUB_BEEPER_IDLE; 
              }
            break;
            //----------------------------------------------------------------
            default:
              BeeperOff();
              rub_BeeperSubPhase = 0;
              gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case BEEP_ERASE_IDS://4 piep high
          switch(rub_BeeperSubPhase){
            //----------------------------------------------------------------
            case 0:
              BeeperOn(2000);//2000Hz
              rub_BeeperOnCntDwnSteps300ms = 1;//600ms
              rub_BeeperSubPhase = 1;
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 2; 
              }
            break;
            //----------------------------------------------------------------
            case 2:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2500);//2500Hz
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 3;   
              }
            break;
            //----------------------------------------------------------------
            case 3:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 4;  
              }
            break;
            //----------------------------------------------------------------
            case 4:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2500);//2500Hz
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 5;   
              }
            break;
            //----------------------------------------------------------------
            case 5:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 6; 
              }
            break;
            //----------------------------------------------------------------
            case 6:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2500);//2500Hz
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 7;   
              }
            break;
            //----------------------------------------------------------------
            case 7:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 8; 
              }
            break;
            //----------------------------------------------------------------
            case 8:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2500);//2500Hz
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 9;   
              }
            break;
            //----------------------------------------------------------------
            case 9:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 0; 
                gub_BeeperSequence = SAL_SUB_BEEPER_IDLE; 
              }
            break;
            //----------------------------------------------------------------
            default:
              BeeperOff();
              rub_BeeperSubPhase = 0;
              gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case BEEP_ERASE_IDS_END://4 piep low
          switch(rub_BeeperSubPhase){
            //----------------------------------------------------------------
            case 0:
              BeeperOn(1500);
              rub_BeeperOnCntDwnSteps300ms = 1;//600ms
              rub_BeeperSubPhase = 1;
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 2; 
              }
            break;
            //----------------------------------------------------------------
            case 2:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2000);
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 3;   
              }
            break;
            //----------------------------------------------------------------
            case 3:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 4;  
              }
            break;
            //----------------------------------------------------------------
            case 4:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2000);
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 5;   
              }
            break;
            //----------------------------------------------------------------
            case 5:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 6; 
              }
            break;
            //----------------------------------------------------------------
            case 6:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2000);
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 7;   
              }
            break;
            //----------------------------------------------------------------
            case 7:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 8; 
              }
            break;
            //----------------------------------------------------------------
            case 8:
              if(rub_BeeperOffCntDwnSteps300ms){
                rub_BeeperOffCntDwnSteps300ms--; 
              }
              else{
                BeeperOn(2000);
                rub_BeeperOnCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 9;   
              }
            break;
            //----------------------------------------------------------------
            case 9:
              if(rub_BeeperOnCntDwnSteps300ms){
                rub_BeeperOnCntDwnSteps300ms--; 
              }
              else{
                BeeperOff();
                rub_BeeperOffCntDwnSteps300ms = 0;//300ms
                rub_BeeperSubPhase = 0; 
                gub_BeeperSequence = SAL_SUB_BEEPER_IDLE; 
              }
            break;
            //----------------------------------------------------------------
            default:
              BeeperOff();
              rub_BeeperSubPhase = 0;
              gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        default:
          BeeperOff();
          rub_BeeperSubPhase = 0;
          gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;  
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
      }
      if( (gub_BeeperSequence == SAL_SUB_BEEPER_IDLE) && (gub_ErrLED_Sequence == SAL_SUB_ERR_LED_IDLE) && (gub_OkLED_Sequence == SAL_SUB_OK_LED_IDLE) ){
        rub_SoundAndLedState = SAL_IDLE;  
      }
      else{
        rub_SoundAndLedState = SAL_HANDLE_ERR_LED; 
      }
    break;
    //............................................................................................
    case SAL_HANDLE_ERR_LED:
      switch(gub_ErrLED_Sequence){
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case SAL_SUB_ERR_LED_IDLE:
          //gub_ErrLED_Sequence = LED_CONST_OFF;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_CONST_OFF:
          digitalWrite(OUT_ERROR_LED, LOW);
          gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_CONST_ON:
          digitalWrite(OUT_ERROR_LED, HIGH);
          gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TAG_CHECK:
          digitalWrite(OUT_ERROR_LED, LOW);
          gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;  
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TAG_OK:
          digitalWrite(OUT_ERROR_LED, LOW);
          gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TAG_NOK:
          switch(rub_ErrLED_SubPhase){
            //----------------------------------------------------------------
            case 0:
              digitalWrite(OUT_ERROR_LED, HIGH);
              rub_ErrLED_OnCntDwnSteps300ms = 5;//1800ms
              rub_ErrLED_SubPhase = 1;
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_ErrLED_OnCntDwnSteps300ms){
                rub_ErrLED_OnCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_ERROR_LED, LOW);
                rub_ErrLED_OffCntDwnSteps300ms = 3;//1200ms
                rub_ErrLED_SubPhase = 2;  
              }
            break;
            //----------------------------------------------------------------
            case 2:
              if(rub_ErrLED_OffCntDwnSteps300ms){
                rub_ErrLED_OffCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_OK_LED, HIGH);
                rub_ErrLED_SubPhase = 0;
                gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE; 
              }
            break;
            //----------------------------------------------------------------
            default:
              digitalWrite(OUT_ERROR_LED, LOW);
              digitalWrite(OUT_OK_LED, HIGH);
              rub_ErrLED_SubPhase = 0;
              gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TEACH_USER:
          switch(rub_ErrLED_SubPhase){
            //----------------------------------------------------------------
            case 0:
              if(rub_ErrLED_OffCntDwnSteps300ms){
                rub_ErrLED_OffCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_ERROR_LED, HIGH);
                rub_ErrLED_OnCntDwnSteps300ms = 0;//300ms
                rub_ErrLED_SubPhase = 1;  
              }
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_ErrLED_OnCntDwnSteps300ms){
                rub_ErrLED_OnCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_ERROR_LED, LOW);
                rub_ErrLED_OffCntDwnSteps300ms = 1;//600ms
                rub_ErrLED_SubPhase = 0;
                //gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;  
              }
            break;
            //----------------------------------------------------------------
            default:
              digitalWrite(OUT_ERROR_LED, LOW);
              rub_ErrLED_SubPhase = 0;
              gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TEACH_USER_END:
          digitalWrite(OUT_ERROR_LED, LOW);
          gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TEACH_ARTICLE:
          switch(rub_ErrLED_SubPhase){
            //----------------------------------------------------------------
            case 0:
              if(rub_ErrLED_OffCntDwnSteps300ms){
                rub_ErrLED_OffCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_ERROR_LED, HIGH);
                rub_ErrLED_OnCntDwnSteps300ms = 1;//600ms
                rub_ErrLED_SubPhase = 1;  
              }
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_ErrLED_OnCntDwnSteps300ms){
                rub_ErrLED_OnCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_ERROR_LED, LOW);
                rub_ErrLED_OffCntDwnSteps300ms = 2;//900ms
                rub_ErrLED_SubPhase = 0;
                //gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;  
              }
            break;
            //----------------------------------------------------------------
            default:
              digitalWrite(OUT_ERROR_LED, LOW);
              rub_ErrLED_SubPhase = 0;
              gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TEACH_ARTICLE_END:
          digitalWrite(OUT_ERROR_LED, LOW);
          gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_ERASE_IDS:
          switch(rub_ErrLED_SubPhase){
            //----------------------------------------------------------------
            case 0:
              if(rub_ErrLED_OffCntDwnSteps300ms){
                rub_ErrLED_OffCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_ERROR_LED, HIGH);
                rub_ErrLED_OnCntDwnSteps300ms = 2;//900ms
                rub_ErrLED_SubPhase = 1;  
              }
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_ErrLED_OnCntDwnSteps300ms){
                rub_ErrLED_OnCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_ERROR_LED, LOW);
                rub_ErrLED_OffCntDwnSteps300ms = 3;//1200ms
                rub_ErrLED_SubPhase = 0;
                //gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;  
              }
            break;
            //----------------------------------------------------------------
            default:
              digitalWrite(OUT_ERROR_LED, LOW);
              rub_ErrLED_SubPhase = 0;
              gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_ERASE_IDS_END:
          digitalWrite(OUT_ERROR_LED, LOW);
          gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_SYSTEM_ERROR:
          digitalWrite(OUT_ERROR_LED, HIGH);
          gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        default:
          digitalWrite(OUT_ERROR_LED, LOW);
          gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
      }
      if( (gub_BeeperSequence == SAL_SUB_BEEPER_IDLE) && (gub_ErrLED_Sequence == SAL_SUB_ERR_LED_IDLE) && (gub_OkLED_Sequence == SAL_SUB_OK_LED_IDLE) ){
        rub_SoundAndLedState = SAL_IDLE;  
      }
      else{
        rub_SoundAndLedState = SAL_HANDLE_OK_LED;  
      }
    break;
    //............................................................................................
    case SAL_HANDLE_OK_LED:
      switch(gub_OkLED_Sequence){
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case SAL_SUB_OK_LED_IDLE:
          //gub_OkLED_Sequence = LED_CONST_OFF;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_CONST_OFF:
          digitalWrite(OUT_OK_LED, LOW);
          gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_CONST_ON:
          digitalWrite(OUT_OK_LED, HIGH);
          gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TAG_CHECK:
          digitalWrite(OUT_OK_LED, digitalRead(OUT_OK_LED) ^ 1);//300ms toggle
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TAG_OK:
          digitalWrite(OUT_OK_LED, HIGH);
          gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TAG_NOK:
          digitalWrite(OUT_OK_LED, LOW);
          gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TEACH_USER:
          switch(rub_OkLED_SubPhase){
            //----------------------------------------------------------------
            case 0:
              if(rub_OkLED_OffCntDwnSteps300ms){
                rub_OkLED_OffCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_OK_LED, HIGH);
                rub_OkLED_OnCntDwnSteps300ms = 0;//300ms
                rub_OkLED_SubPhase = 1;  
              }
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_OkLED_OnCntDwnSteps300ms){
                rub_OkLED_OnCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_OK_LED, LOW);
                rub_OkLED_OffCntDwnSteps300ms = 1;//600ms
                rub_OkLED_SubPhase = 0;
                //gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;  
              }
            break;
            //----------------------------------------------------------------
            default:
              digitalWrite(OUT_OK_LED, LOW);
              rub_OkLED_SubPhase = 0;
              gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TEACH_USER_END:
          digitalWrite(OUT_OK_LED, HIGH);
          gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TEACH_ARTICLE:
          switch(rub_OkLED_SubPhase){
            //----------------------------------------------------------------
            case 0:
              if(rub_OkLED_OffCntDwnSteps300ms){
                rub_OkLED_OffCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_OK_LED, HIGH);
                rub_OkLED_OnCntDwnSteps300ms = 1;//600ms
                rub_OkLED_SubPhase = 1;  
              }
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_OkLED_OnCntDwnSteps300ms){
                rub_OkLED_OnCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_OK_LED, LOW);
                rub_OkLED_OffCntDwnSteps300ms = 2;//900ms
                rub_OkLED_SubPhase = 0;
                //gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;  
              }
            break;
            //----------------------------------------------------------------
            default:
              digitalWrite(OUT_OK_LED, LOW);
              rub_OkLED_SubPhase = 0;
              gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_TEACH_ARTICLE_END:
          digitalWrite(OUT_OK_LED, HIGH);
          gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_ERASE_IDS:
          switch(rub_OkLED_SubPhase){
            //----------------------------------------------------------------
            case 0:
              if(rub_OkLED_OffCntDwnSteps300ms){
                rub_OkLED_OffCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_OK_LED, HIGH);
                rub_OkLED_OnCntDwnSteps300ms = 2;//900ms
                rub_OkLED_SubPhase = 1;  
              }
            break;
            //----------------------------------------------------------------
            case 1:
              if(rub_OkLED_OnCntDwnSteps300ms){
                rub_OkLED_OnCntDwnSteps300ms--; 
              }
              else{
                digitalWrite(OUT_OK_LED, LOW);
                rub_OkLED_OffCntDwnSteps300ms = 3;//1200ms
                rub_OkLED_SubPhase = 0;
                //gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;  
              }
            break;
            //----------------------------------------------------------------
            default:
              digitalWrite(OUT_OK_LED, LOW);
              rub_OkLED_SubPhase = 0;
              gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
            break;
            //----------------------------------------------------------------
          }
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_ERASE_IDS_END:
          digitalWrite(OUT_OK_LED, HIGH);
          gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        case LED_SYSTEM_ERROR:
          digitalWrite(OUT_OK_LED, LOW);
          gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        default:
          digitalWrite(OUT_OK_LED, HIGH);
          gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
        break;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
      }
      if( (gub_BeeperSequence == SAL_SUB_BEEPER_IDLE) && (gub_ErrLED_Sequence == SAL_SUB_ERR_LED_IDLE) && (gub_OkLED_Sequence == SAL_SUB_OK_LED_IDLE) ){
        rub_SoundAndLedState = SAL_IDLE;  
      }
      else{
        rub_SoundAndLedState = SAL_HANDLE_BEEPER;  
      }
    break;
    //............................................................................................
    default:
      digitalWrite(OUT_BEEPER, LOW);
      digitalWrite(OUT_ERROR_LED, LOW);
      digitalWrite(OUT_OK_LED, HIGH);
      rub_SoundAndLedState = SAL_IDLE;
    break;
    //............................................................................................
  }
}
//***********************************************************************************
void Beeper(unsigned char ub_SoundSequence){
//could be simplified by just copying the parameter!
  switch(ub_SoundSequence){
    case BEEP_DETECT_TAG:
      gub_BeeperSequence = BEEP_DETECT_TAG;
    break;

    case BEEP_UNKNOWN_ID:
      gub_BeeperSequence = BEEP_UNKNOWN_ID;
    break;

    case BEEP_TEACH_USER:
      gub_BeeperSequence = BEEP_TEACH_USER;
    break;

    case BEEP_TEACH_USER_END:
      gub_BeeperSequence = BEEP_TEACH_USER_END;
    break;

    case BEEP_TEACH_ARTICLE:
      gub_BeeperSequence = BEEP_TEACH_ARTICLE;
    break;

    case BEEP_TEACH_ARTICLE_END:
      gub_BeeperSequence = BEEP_TEACH_ARTICLE_END;
    break;

    case BEEP_ERASE_IDS:
      gub_BeeperSequence = BEEP_ERASE_IDS;
    break;

    case BEEP_ERASE_IDS_END:
      gub_BeeperSequence = BEEP_ERASE_IDS_END;
    break;

    default:
      gub_BeeperSequence = SAL_SUB_BEEPER_IDLE;
    break;
  }
}
//***********************************************************************************
void ErrorLedSet(unsigned char ub_LED_Sequence){
//could be simplified by just copying the parameter!  
  switch(ub_LED_Sequence){
    case LED_CONST_OFF:
      gub_ErrLED_Sequence = LED_CONST_OFF;    
      digitalWrite(OUT_ERROR_LED, LOW);
    break;

    case LED_CONST_ON:
      gub_ErrLED_Sequence = LED_CONST_ON;
      digitalWrite(OUT_ERROR_LED, HIGH);
    break;

    case LED_TAG_CHECK:
      gub_ErrLED_Sequence = LED_TAG_CHECK;
    break;

    case LED_TAG_OK:
      gub_ErrLED_Sequence = LED_TAG_OK;
    break;

    case LED_TAG_NOK:
      gub_ErrLED_Sequence = LED_TAG_NOK;
    break;

    case LED_TEACH_USER:
      gub_ErrLED_Sequence = LED_TEACH_USER;
    break;

    case LED_TEACH_USER_END:
      gub_ErrLED_Sequence = LED_TEACH_USER_END;
    break;

    case LED_TEACH_ARTICLE:
      gub_ErrLED_Sequence = LED_TEACH_ARTICLE;
    break;

    case LED_TEACH_ARTICLE_END:
      gub_ErrLED_Sequence = LED_TEACH_ARTICLE_END;
    break;

    case LED_ERASE_IDS:
      gub_ErrLED_Sequence = LED_ERASE_IDS;
    break;

    case LED_ERASE_IDS_END:
      gub_ErrLED_Sequence = LED_ERASE_IDS_END;
    break;

    case LED_SYSTEM_ERROR:
      gub_ErrLED_Sequence = LED_SYSTEM_ERROR;
    break;

    default:
      gub_ErrLED_Sequence = SAL_SUB_ERR_LED_IDLE;
    break;
  }    
}
//***********************************************************************************
void OkLedSet(unsigned char ub_LED_Sequence){
//could be simplified by just copying the parameter!  
  switch(ub_LED_Sequence){
    case LED_CONST_OFF:
      gub_OkLED_Sequence = LED_CONST_OFF;
      digitalWrite(OUT_OK_LED, LOW);
    break;

    case LED_CONST_ON:
      gub_OkLED_Sequence = LED_CONST_ON;
      digitalWrite(OUT_OK_LED, HIGH);
    break;

    case LED_TAG_CHECK:
      gub_OkLED_Sequence = LED_TAG_CHECK;
    break;

    case LED_TAG_OK:
      gub_OkLED_Sequence = LED_TAG_OK;
    break;

    case LED_TAG_NOK:
      gub_OkLED_Sequence = LED_TAG_NOK;
    break;

    case LED_TEACH_USER:
      gub_OkLED_Sequence = LED_TEACH_USER;
    break;

    case LED_TEACH_USER_END:
      gub_OkLED_Sequence = LED_TEACH_USER_END;
    break;

    case LED_TEACH_ARTICLE:
      gub_OkLED_Sequence = LED_TEACH_ARTICLE;
    break;

    case LED_TEACH_ARTICLE_END:
      gub_OkLED_Sequence = LED_TEACH_ARTICLE_END;
    break;

    case LED_ERASE_IDS:
      gub_OkLED_Sequence = LED_ERASE_IDS;
    break;

    case LED_ERASE_IDS_END:
      gub_OkLED_Sequence = LED_ERASE_IDS_END;
    break;

    case LED_SYSTEM_ERROR:
      gub_OkLED_Sequence = LED_SYSTEM_ERROR;
    break;

    default:
      gub_OkLED_Sequence = SAL_SUB_OK_LED_IDLE;
    break;
  }  
}
//***********************************************************************************
#define ONE_SECOND_IN_MICCRO_SECONDS  1000000
void BeeperOn(unsigned short uwFrequency){
  unsigned long lul_1sInMicros;
  unsigned short luw_TmrIntervallInMicros;
  //-------------------------------------------------
  lul_1sInMicros = ONE_SECOND_IN_MICCRO_SECONDS;
  luw_TmrIntervallInMicros = (lul_1sInMicros + (uwFrequency / 2)) / uwFrequency;
  Timer1.setPeriod((unsigned short)luw_TmrIntervallInMicros);
}
//***********************************************************************************
void BeeperOff(){
  //-------------------------------------------------
  Timer1.stop();
  digitalWrite(OUT_BEEPER, LOW);
}
//***********************************************************************************
void Tmr1_ISR(){
  static bool boToggle;
#ifdef EN_OUTPUT_TIMINGTEST_SIGNALS 
  //digitalWrite(40, digitalRead(40) ^ 1); 
  digitalWrite(OUT_TM_TEST_SIG, HIGH);
#endif
  //insert code or function to call here:
  digitalWrite(OUT_BEEPER, boToggle);
  boToggle = !boToggle;
#ifdef EN_OUTPUT_TIMINGTEST_SIGNALS  
  //delayMicroseconds(5000);
  digitalWrite(OUT_TM_TEST_SIG, LOW);
#endif
}
//***********************************************************************************
