/*
 Name:    Log.h
 Datum:   19.04.2018
 Autor:   Markus Emanuel Wurmitzer

  Hinweise zur Anwendung:
  Aktuell wird die Karte nicht erneut erkannt, wenn sie während des Betriebs abgesteckt wird!

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
#ifndef LOG_H_
#define LOG_H_

#ifdef __cplusplus
extern "C"{
#endif

/*
  öffentliche Funktionen
*/
extern void LOG_Init(int pin);
extern String LOG_DatAnz(void);
extern String LOG_DatName(int iID);
extern String LOG_DatInhalt(String strDatei, int iID);
extern String Log_DatEntf(String strDatei);
extern void LOG_Eintrag(String strMeldung);

/*
 * Funktionen
 */
void LOG_ReInit(); 

#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* LOG_H_ */
