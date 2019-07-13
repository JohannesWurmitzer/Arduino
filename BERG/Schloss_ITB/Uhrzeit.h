/*
 Name:    Uhrzeit.h
 Datum:   18.04.2018
 Autor:   Markus Emanuel Wurmitzer

  Hinweise zur Anwendung:
  Die DS1307RTC wird über die SD Leitung (siehe Belegung Uno / Mega2560) an den Arduiono angebunden!

  Versionsgeschichte:
  23.04.2018  V 100   Markus Emanuel Wurmitzer
    - Erstellung
*/

/*
 * eigebundene Dateien
 */
#include "Arduino.h"

/*
 * Makros
*/
#ifndef UHRZEIT_H_
#define UHRZEIT_H_

#ifdef __cplusplus
extern "C"{
#endif

/*
  öffentliche Funktionen
*/
extern void UHR_Init(void);
extern bool UHR_Aktiv(void);
extern String UHR_Lesen(void);
extern String UHR_Logzeit(void);
extern String UHR_Schreiben(String DatumZeit);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* UHRZEIT_H_ */
