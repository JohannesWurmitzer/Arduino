/*
  Copyright (c) 2017, E.C.E. Wurmitzer GmbH
  All rights reserved.
*/
/*  Function description

  
*/
/*
  File Name:      MotorLock.c
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check I/Os!
  Created by:     Edmund Titz
  Created on:     2017-10-30
  derived from:   
*/

/*
  Versionsgeschichte:
  xx.05.2018  V 102   Markus Emanuel Wurmitzer
    - Zeit Rückwärtsbewegung Maximalwert auf 254 reduziert, damit bei neuem Gerät der Standardwert verwendet wird
    
  08.05.2018  V 101   Markus Emanuel Wurmitzer
    - Zeitüberwachungswerte können durch externe Funktionen eingestellt werden
  
  04.05.2018  V 100   Markus Emanuel Wurmitzer
    - Zeitüberwachung für vorwärts und rückwärts unterschiedlich

  V1.10 2018-04-29  EdTi
    - added motor brake (drive motor in back direction for a short time is breaking)
    - increased speed of state machine from 50ms intervall to 30ms intervall (timer3 interrupt!)
    
  V1.00 2017-10-30  EdTi
    - creation of file
*/

/*  todo-list
  - do not know...
*/

/*
  Includes
*/

#include "MotorLockHbridge.h"

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
  unsigned char gub_Command = WAIT_COMMAND;

  unsigned char gub_StateTesti = WAIT_COMMAND;
  unsigned char gub_StateTesti2 = WAIT_COMMAND;

  unsigned char gub_MotorActive = false;
  unsigned char gub_MotorBreakRequest = false;

  unsigned short guwUWZS;   // Überwachungszeit Schließen
  unsigned short guwUWZO;   // Überwachungszeit Öffnen
  unsigned char gubRFZ;     // Rückfahrzeit

  unsigned short guw_CntDwnMotorBlockingTime = 0;
  unsigned char gub_AddMotorDriveBackTime = 0;
                                                                  
/*
  Private Function Prototypes
*/
  void lockStateMachine();
/*
  Public Function Prototypes
*/
  void MotorLockInit();
  void MotorLockHandler();
  void setMotorLockCommand(unsigned char ubCommand);
  unsigned char setMotor(unsigned char ubDirection);

/*
 implementation of functions
*/
void MotorLockInit(){
  pinMode(OUT_NOT_MOTOR_LOCK, OUTPUT);
  pinMode(OUT_NOT_MOTOR_UNLOCK, OUTPUT);
  pinMode(OUT_LED, OUTPUT);
  pinMode(IN_NOT_LOCK_SWITCH, INPUT);
  pinMode(IN_NOT_MOTOR_SWITCH, INPUT);

  digitalWrite(OUT_NOT_MOTOR_LOCK, HIGH);
  digitalWrite(OUT_NOT_MOTOR_UNLOCK, HIGH);
  digitalWrite(OUT_LED, LOW);

  // Parameterwerte initialisieren
  guwUWZS = TIMEBASE_MOTOR_BLOCKING;
  guwUWZO = TIMEBASE_MOTOR_BLOCKING_OP;
  gubRFZ = TIMEBASE_MOTOR_ADD_DRIVE_BACK;
}

void MotorLockHandler() {

  lockStateMachine();

}

void setMotorLockCommand(unsigned char ubCommand){
  gub_Command = ubCommand;  
}

unsigned char getMotorLockState(){
  if(gub_StateTesti == LOCKED){
    return IS_LOCKED;  
  }
  else if(gub_StateTesti == UNLOCKED){
    return IS_UNLOCKED; 
  }
  else if(gub_StateTesti == WAIT_ARC_LOCKING){
    return WAIT_ARC_CLOSE;  
  }
  else if( (gub_StateTesti == LOCKING) || (gub_StateTesti == UNLOCKING) ||(gub_StateTesti == MOTOR_BLOCKED) ){
    return IN_PROGRESS; 
  }
  else if(gub_StateTesti == LOCK_ERROR){
    return LOCK_ERROR;  
  }
  else if(gub_StateTesti == UNLOCK_ERROR){
    return UNLOCK_ERROR;  
  }
  else{
    return UNDEFINED; 
  }
  
//  return gub_StateTesti;
}

// Schlosszustand als Zeichenkette ausgeben
// Parameter:
// ubyStat = auszuwertender Status
// Rückgabewert:
// Schlosszustand
String MOT_ZustandZK(unsigned char ubyStat)
{
    String strZustand = "unbekannt";

    switch(ubyStat)
    {
      case UNDEFINED:
        strZustand = "undefiniert";
        break;
      case IN_PROGRESS:
        strZustand = "in Bewegung";
        break;        
      case WAIT_ARC_CLOSE:
        strZustand = "wartet auf Verschluss";
        break;
      case IS_LOCKED:
        strZustand = "geschlossen";
        break;
      case IS_UNLOCKED:
        strZustand = "offen";
        break;
      case LOCK_ERROR:
        strZustand = "Fehler beim Sperren";
        break;
      case UNLOCK_ERROR:
        strZustand = "Fehler beim Entsperren";
        break;
    }   
    return strZustand;
}

unsigned char getMotorLockState2(){
  return gub_StateTesti2; 
}

// Zustandsabfrage verschlossen
// Rückgabewert:
// wahr = verschlossen, falsch = nicht verschlossen
bool MOT_Geschlossen(void)
{
  return (gub_StateTesti == LOCKED);
}

void lockStateMachine(){
  static unsigned char rub_State = WAIT_COMMAND;
  static unsigned char rub_PreviousState = WAIT_COMMAND;
  static boolean rbo_StateIsInit = 0;
  static unsigned char rub_lockingPhase = DO_NOTHING;
                                                                                static unsigned char rub_lockingPhaseOld = DO_NOTHING;
  //static unsigned short ruw_CntDwnMotorBlockingTime = (unsigned short)TIMEBASE_MOTOR_BLOCKING;
  static unsigned char rub_MotorSwitchHigh = false;
  static unsigned char rub_MotorSwitchStateOld = false;
  static unsigned char rub_LockSwitchHigh = false;
  static unsigned char rub_LockSwitchStateOld = false;
  static boolean rbo_MotorBlocked = false;
  static boolean rbo_MotorTimeout = false;
  static boolean rbo_MotorTimeoutFlag = false;
  //static unsigned char rub_AddMotorDriveBackTime = 0;
    
  if(gub_Command == EMERGENCY_STOP_MOTOR){
    gub_Command = WAIT_COMMAND;

    if(rub_State == EMERGENCY_STOP_MOTOR){
      rub_State = WAIT_COMMAND;
    } 
    else{
      rub_State = EMERGENCY_STOP_MOTOR;  
    }
    rbo_StateIsInit = 0;
  }
  else if(gub_Command == LOCKING){
    gub_Command = WAIT_COMMAND;
    rub_State = LOCKING; 
    rbo_StateIsInit = 0; 
  }
  else if(gub_Command == UNLOCKING){
    gub_Command = WAIT_COMMAND;
    rub_State = UNLOCKING;
    rbo_StateIsInit = 0;  
  }
  else if(gub_Command == MOTOR_DIRECT_DIR_LOCK){
    gub_Command = WAIT_COMMAND;
    rub_State = MOTOR_DIRECT_DIR_LOCK;
    rbo_StateIsInit = 0;  
  }
  else if(gub_Command == MOTOR_DIRECT_DIR_UNLOCK){
    gub_Command = WAIT_COMMAND;
    rub_State = MOTOR_DIRECT_DIR_UNLOCK;
    rbo_StateIsInit = 0;  
  }
  else{
    
  }

  gub_StateTesti = rub_State;
  gub_StateTesti2 = rub_lockingPhase;
  switch(rub_State){
//-------------------------------------------------------------------------------
    case WAIT_COMMAND:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;
      }
    break;
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
    case EMERGENCY_STOP_MOTOR:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1; 
        setMotor(STOP_MOTOR);
      }
    break;
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------lock:    
    case LOCKING:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;

        rub_LockSwitchHigh = digitalRead(IN_NOT_LOCK_SWITCH);
        rub_LockSwitchStateOld = rub_LockSwitchHigh;
        rub_MotorSwitchHigh = digitalRead(IN_NOT_MOTOR_SWITCH);
        rub_MotorSwitchStateOld = rub_MotorSwitchHigh;
        setMotor(LOCK_DIR);
        if(rub_MotorSwitchHigh){//open switch
          rub_lockingPhase = WAIT_MOTOR_SWITCH_FALLING;  
        }
        else{//closed switch
          rub_lockingPhase = WAIT_MOTOR_SWITCH_RISING;  
        }
      }
      else{
        //rub_lockingPhase = DO_NOTHING; 
//        rbo_StateIsInit = 0; 
      }
      rub_lockingPhaseOld = DO_NOTHING;
      
      switch(rub_lockingPhase){
        //...............................................................
        case DO_NOTHING:
        break;
        //...............................................................
        //...............................................................
        case WAIT_MOTOR_SWITCH_RISING:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
          rub_MotorSwitchHigh = digitalRead(IN_NOT_MOTOR_SWITCH);
          if( (rub_MotorSwitchStateOld != rub_MotorSwitchHigh) && rub_MotorSwitchHigh ){//rising edge
            rub_MotorSwitchStateOld = rub_MotorSwitchHigh;
            rub_lockingPhase = WAIT_MOTOR_SWITCH_FALLING; 
          }
        break;
        //...............................................................
        //...............................................................
        case WAIT_MOTOR_SWITCH_FALLING:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
          if( (rbo_MotorTimeout == true) || (rbo_MotorBlocked == true) ){
            rbo_MotorTimeout = false;
            rbo_MotorBlocked = false;
            gub_AddMotorDriveBackTime = gubRFZ;
            setMotor(UNLOCK_DIR);
          }
          rub_MotorSwitchHigh = digitalRead(IN_NOT_MOTOR_SWITCH);
          if( (rub_MotorSwitchStateOld != rub_MotorSwitchHigh) && !rub_MotorSwitchHigh ){//falling edge
          //if(!rub_MotorSwitchHigh){
            setMotor(STOP_MOTOR);
            rub_LockSwitchHigh = digitalRead(IN_NOT_LOCK_SWITCH);
            if(rub_LockSwitchHigh){
              rub_lockingPhase = DO_NOTHING;
              rub_State = WAIT_ARC_LOCKING;
              rbo_StateIsInit = 0;  
            }
            else{
              if(rub_LockSwitchStateOld == rub_LockSwitchHigh){
                rub_LockSwitchStateOld = rub_LockSwitchHigh;
                rub_lockingPhase = DO_NOTHING;
                rub_State = LOCKED;
                rbo_StateIsInit = 0;
              }
              else{
                rub_lockingPhase = DO_NOTHING;
                rub_State = WAIT_ARC_LOCKING;
                rbo_StateIsInit = 0; 
              }
            }
          }
        break;
        //...............................................................
        //...............................................................
        default:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
        break;
        //...............................................................
      }
    break;
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
    case WAIT_ARC_LOCKING:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;
        rub_lockingPhase = WAIT_LOCK_SWITCH_FALLING;
        rub_lockingPhaseOld = DO_NOTHING;
      }
  
      switch(rub_lockingPhase){
        //...............................................................
        case DO_NOTHING:

        break;
        //...............................................................
        //...............................................................
        case WAIT_LOCK_SWITCH_FALLING:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
          if(!digitalRead(IN_NOT_LOCK_SWITCH)){
            setMotor(LOCK_DIR);
            rub_lockingPhase = LOCKING_WAIT_MOTOR_SWITCH_RISING; 
          }
        break;
        //...............................................................
        //...............................................................
        case LOCKING_WAIT_MOTOR_SWITCH_RISING:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
          rub_MotorSwitchHigh = digitalRead(IN_NOT_MOTOR_SWITCH);
          if(rub_MotorSwitchHigh){
            rub_lockingPhase = LOCKING_WAIT_MOTOR_SWITCH_FALLING; 
          }
        break;
        //...............................................................
        //...............................................................
        case LOCKING_WAIT_MOTOR_SWITCH_FALLING:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
          rub_MotorSwitchHigh = digitalRead(IN_NOT_MOTOR_SWITCH);
          if(!rub_MotorSwitchHigh){
            setMotor(STOP_MOTOR);
            rub_lockingPhase = DO_NOTHING;
            rub_State = LOCKED;
            rbo_StateIsInit = 0; 
          }
        break;
        //...............................................................
        //...............................................................
        default:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
        break;
        //...............................................................
      }
    break;
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
    case LOCKED:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;
      }
    break;
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------unlock:
    case UNLOCKING:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;
        rub_LockSwitchHigh = digitalRead(IN_NOT_LOCK_SWITCH);
        rub_LockSwitchStateOld = rub_LockSwitchHigh;
        rub_MotorSwitchHigh = digitalRead(IN_NOT_MOTOR_SWITCH);
        rub_MotorSwitchStateOld = rub_MotorSwitchHigh;
        setMotor(UNLOCK_DIR);

        if(rub_MotorSwitchHigh){//open switch
          rub_lockingPhase = UNLOCKING_WAIT_MOTOR_SWITCH_FALLING;  
        }
        else{//closed switch
          rub_lockingPhase = UNLOCKING_WAIT_MOTOR_SWITCH_RISING;  
        }
      }

      switch(rub_lockingPhase){
        //...............................................................
        case DO_NOTHING:
        break;
        //...............................................................
        //...............................................................
        case UNLOCKING_WAIT_MOTOR_SWITCH_RISING:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
          rub_MotorSwitchHigh = digitalRead(IN_NOT_MOTOR_SWITCH);
          if( (rub_MotorSwitchStateOld != rub_MotorSwitchHigh) && rub_MotorSwitchHigh ){//rising edge
            rub_MotorSwitchStateOld = rub_MotorSwitchHigh;
            rub_lockingPhase = UNLOCKING_WAIT_MOTOR_SWITCH_FALLING; 
          }
        break;
        //...............................................................
        //...............................................................
/*        case UNLOCKING_WAIT_LOCK_SWITCH_RISING:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
          if(digitalRead(IN_NOT_LOCK_SWITCH)){
            rub_lockingPhase = UNLOCKING_WAIT_MOTOR_SWITCH_FALLING;  
          }
        break;
*/        //...............................................................
        //...............................................................
        case UNLOCKING_WAIT_MOTOR_SWITCH_FALLING:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }

          if( (rbo_MotorTimeout == true) || (rbo_MotorBlocked == true) ){
            
            rbo_MotorTimeoutFlag = true;//in this case it is timeout that deliver a blocked!!
            rbo_MotorTimeout = false;
            rbo_MotorBlocked = false;
            gub_AddMotorDriveBackTime = gubRFZ;
            setMotor(LOCK_DIR);
          }
          rub_MotorSwitchHigh = digitalRead(IN_NOT_MOTOR_SWITCH);
          if( (rub_MotorSwitchStateOld != rub_MotorSwitchHigh) && (!rub_MotorSwitchHigh ) ){//falling edge
                                                        
            rub_MotorSwitchStateOld = rub_MotorSwitchHigh;
            //++++++++++++++++++++++++++++++++++++++++++++++++++
            setMotor(STOP_MOTOR);
            //to avoid timeout motor in standard moving
            /*
            guw_CntDwnMotorBlockingTime = (unsigned short)TIMEBASE_MOTOR_BLOCKING;
            gub_AddMotorDriveBackTime = 0;
            */
            //++++++++++++++++++++++++++++++++++++++++++++++++++
            rub_LockSwitchHigh = digitalRead(IN_NOT_LOCK_SWITCH);
            
            if( (rub_LockSwitchHigh != rub_LockSwitchStateOld) || (rbo_MotorTimeoutFlag == true)){
              rub_LockSwitchStateOld = rub_LockSwitchHigh;
              rbo_MotorTimeoutFlag = false;
              rub_lockingPhase = DO_NOTHING;
              rub_State = UNLOCKED;
              rbo_StateIsInit = 0;  
            }
            else{
              setMotor(UNLOCK_DIR);
              rub_lockingPhase = UNLOCKING_WAIT_MOTOR_SWITCH_RISING_2ND;
            }
          }
          
        break;
        //...............................................................
        //...............................................................
        case UNLOCKING_WAIT_MOTOR_SWITCH_RISING_2ND:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
          rub_MotorSwitchHigh = digitalRead(IN_NOT_MOTOR_SWITCH);
          if( (rub_MotorSwitchStateOld != rub_MotorSwitchHigh) && rub_MotorSwitchHigh ){//rising edge
            rub_MotorSwitchStateOld = rub_MotorSwitchHigh;
            rub_lockingPhase = UNLOCKING_WAIT_MOTOR_SWITCH_FALLING_2ND; 
          }
        break;
        //...............................................................
        //...............................................................
        case UNLOCKING_WAIT_MOTOR_SWITCH_FALLING_2ND:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
          if( (rbo_MotorTimeout == true) || (rbo_MotorBlocked == true) ){
            
            //rbo_MotorTimeoutFlag = true;//in this case it is timeout that deliver a blocked!!
            rbo_MotorTimeout = false;
            rbo_MotorBlocked = false;
            gub_AddMotorDriveBackTime = gubRFZ;
            setMotor(LOCK_DIR);
          }
           
          rub_MotorSwitchHigh = digitalRead(IN_NOT_MOTOR_SWITCH);
          if( (rub_MotorSwitchStateOld != rub_MotorSwitchHigh) && !rub_MotorSwitchHigh ){//falling edge
            rub_MotorSwitchStateOld = rub_MotorSwitchHigh;
            setMotor(STOP_MOTOR);
            rub_lockingPhase = DO_NOTHING;
            rub_State = UNLOCKED;
            rbo_StateIsInit = 0;  
          }
        break;
        //...............................................................
        //...............................................................
        default:
          if(rub_lockingPhaseOld != rub_lockingPhase){
            rub_lockingPhaseOld = rub_lockingPhase;
          }
        break;
        //...............................................................
      }
    break;
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
    case UNLOCKED:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;
      }
    break;
//-------------------------------------------------------------------------------
/*
//-------------------------------------------------------------------------------blocking:
    case MOTOR_BLOCKED:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;
        rub_MotorSwitchStateOld = rub_MotorSwitchHigh;
        ruw_CntDwnMotorBlockingTime = (unsigned short)TIMEBASE_MOTOR_BLOCKING; 
      }
    break;
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------blocking:
    case MOTOR_TIMEOUT:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;
        rub_MotorSwitchStateOld = rub_MotorSwitchHigh;
        ruw_CntDwnMotorBlockingTime = (unsigned short)TIMEBASE_MOTOR_BLOCKING; 
      }
    break;
//-------------------------------------------------------------------------------
*/
//-------------------------------------------------------------------------------motor direct:
    case MOTOR_DIRECT_DIR_LOCK:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;
        setMotor(LOCK_DIR); 
      }
    break;
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
    case MOTOR_DIRECT_DIR_UNLOCK:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;
        setMotor(UNLOCK_DIR);
      }
    break;
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
    default:
      if(!rbo_StateIsInit){
        rbo_StateIsInit = 1;
      }
    break;
  } 
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------check motor:
  if(gub_MotorActive){
    //motor is blocking (or timeout) if motor switch do not change inside 600ms!!!
    if(rub_MotorSwitchHigh == rub_MotorSwitchStateOld){
      if(guw_CntDwnMotorBlockingTime > 0){
        guw_CntDwnMotorBlockingTime--;  
      }
      else if(gub_AddMotorDriveBackTime){
        gub_AddMotorDriveBackTime--;
      }
    }
    else{
      rub_MotorSwitchStateOld = rub_MotorSwitchHigh;
      /*
      guw_CntDwnMotorBlockingTime = (unsigned short)TIMEBASE_MOTOR_BLOCKING; 
      gub_AddMotorDriveBackTime = 0; 
      */
    }
    if( (guw_CntDwnMotorBlockingTime + gub_AddMotorDriveBackTime) == 0){
      //guw_CntDwnMotorBlockingTime = (unsigned short)TIMEBASE_MOTOR_BLOCKING;
      //gub_AddMotorDriveBackTime = 0;
      rub_LockSwitchHigh = digitalRead(IN_NOT_LOCK_SWITCH);
      rub_LockSwitchStateOld = rub_LockSwitchHigh;
      if(rub_LockSwitchHigh){//blocking
        setMotor(STOP_MOTOR); 
        rbo_MotorBlocked = true; 
        rbo_MotorTimeout = false;
      }
      else{//timeout
        setMotor(STOP_MOTOR);
        rbo_MotorTimeout = true; 
        rbo_MotorBlocked = false; 
      }
    }   
  }
  /*
  else{
    guw_CntDwnMotorBlockingTime = (unsigned short)TIMEBASE_MOTOR_BLOCKING;
    gub_AddMotorDriveBackTime = 0;  
  }
  */

  if(gub_MotorBreakRequest == true){
    if(setMotor(MOT_BREAK_CHECK) == 0){//breaking motor is finished
      gub_MotorBreakRequest = false;
    }
  }
}

unsigned char setMotor(unsigned char ubDirection){
  static unsigned char rub_LastMotorCommand = STOP_MOTOR;
  static unsigned char rub_BreakMotor = 0;
  static unsigned char rub_MotorBrakeCntDwn = MOTOR_BRAKE_TIME;

  if( (ubDirection == LOCK_DIR) || (ubDirection == UNLOCK_DIR) ){//end breaking
    gub_MotorBreakRequest = false;
    rub_BreakMotor = 1; 
    rub_MotorBrakeCntDwn = MOTOR_BRAKE_TIME;   
  }

  switch(ubDirection){
    case STOP_MOTOR:
      if (gub_MotorActive)
      {
        gub_MotorBreakRequest = true;
        rub_BreakMotor = 1;//breaking is driving motor in back direction for a short time (see definition MOTOR_BRAKE_TIME)
        if(rub_LastMotorCommand == LOCK_DIR){
          digitalWrite(OUT_NOT_MOTOR_LOCK, HIGH);
          digitalWrite(OUT_NOT_MOTOR_UNLOCK, LOW);
        }
        else if(rub_LastMotorCommand == UNLOCK_DIR){
          digitalWrite(OUT_NOT_MOTOR_UNLOCK, HIGH);
          digitalWrite(OUT_NOT_MOTOR_LOCK, LOW); 
        }
        gub_MotorActive = false;//set to false because breaking is no driving of motor (relevant for timeout and blocking detection)
      }
    break;

    case LOCK_DIR:
      rub_LastMotorCommand = ubDirection;
      digitalWrite(OUT_NOT_MOTOR_UNLOCK, HIGH);
      digitalWrite(OUT_NOT_MOTOR_LOCK, LOW);
      gub_MotorActive = true;
      guw_CntDwnMotorBlockingTime = guwUWZS;
    break;

    case UNLOCK_DIR:
      rub_LastMotorCommand = ubDirection;
      digitalWrite(OUT_NOT_MOTOR_LOCK, HIGH);
      digitalWrite(OUT_NOT_MOTOR_UNLOCK, LOW);
      gub_MotorActive = true;
      guw_CntDwnMotorBlockingTime = guwUWZO;
    break;

    case MOT_BREAK_CHECK:
      if(rub_BreakMotor){
        if(rub_MotorBrakeCntDwn){
          rub_MotorBrakeCntDwn--;    
        }
        else{//Motor off (no driveing and no braking, just short circuit thru Hbridge)
          digitalWrite(OUT_NOT_MOTOR_LOCK, HIGH);
          digitalWrite(OUT_NOT_MOTOR_UNLOCK, HIGH);
          rub_BreakMotor = 0;  
          rub_MotorBrakeCntDwn = MOTOR_BRAKE_TIME;
        }
      }
    break;
    
    default:
      digitalWrite(OUT_NOT_MOTOR_LOCK, HIGH);
      digitalWrite(OUT_NOT_MOTOR_UNLOCK, HIGH);
      gub_MotorActive = false;
    break;
  }
  return rub_BreakMotor;
}

// Parameterwert schreiben
// Parameter:
// iAdr = Adresse des Paramters (1 = Überwachungszeit Schließen, 2 = Überwachungszeit Öffnen, 3 = Rückfahrzeit)
// iWert = neuer Wert
// ACHTUNG: die internen Zeiten müssen als Zyklen angegeben werden!
void MOT_ParZeiten(int iAdr, int iWert)
{
  switch (iAdr)
  {
    case 1:
      // Überwachungszeit Schließen
      if ((iWert > 0) && (iWert <= 1000))
      {
        guwUWZS = (unsigned short)iWert / MOTOR_ISR_TIME_MS;
      }
      break;
    case 2:
      // Überwachungszeit Öffnen
      if ((iWert > 0) && (iWert <= 1000))
      {
        guwUWZO = (unsigned short)iWert / MOTOR_ISR_TIME_MS;
      }
      break;
    case 3:
      //Rückfahrzeit
      if ((iWert > 0) && (iWert <= 254))
      {
        gubRFZ = (unsigned char)iWert / MOTOR_ISR_TIME_MS;
      }
      break;
  }
}
