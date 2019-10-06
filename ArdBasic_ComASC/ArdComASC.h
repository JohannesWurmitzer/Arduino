/*
 Name:    ArdComASC.h
 Datum:   2019-10-02
 Autor:   Maximilian Johannes Wurmitzer

  Versionsgeschichte:
  23.04.2018  V 100   Markus Emanuel Wurmitzer
    - Erstellung
*/

/*
 * eigebundene Dateien
 */
#include "Arduino.h"

#include "ArdComASCConf.h"


/*
 * Makros
*/
#ifndef ARDCOMASC_H_
#define ARDCOMASC_H_

/*
  öffentliche Funktionen
*/
#ifdef __cplusplus
extern "C"{
#endif
extern void ArdComASC_Setup(void);
extern void ArdComASC_Loop(void);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* UHRZEIT_H_ */
