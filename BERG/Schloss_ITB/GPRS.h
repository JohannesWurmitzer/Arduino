/*
 Name:    GPRS.h
 Datum:   17.05.2018
 Autor:   Markus Emanuel Wurmitzer

  Versionsgeschichte:
  18.07.2018  V 102   Markus Emanuel Wurmitzer
    - GPRS_APN zur Einstellung des zu verwendenden APNs eingefügt
    - FTP Datei vom Server lesen
    - Funktion zum Setzen des Merkers "Datei von FTP Server lesen"
    
  30.05.2018  V 101   Markus Emanuel Wurmitzer
    - Digitalausgang Neustart auf 23 geändert von 9
    
  18.05.2018  V 100   Markus Emanuel Wurmitzer
    - Erstellung
*/

/*
 * eigebundene Dateien
 */
#include "Arduino.h"

/*
 * Makros
*/
#ifndef GPRS_H_
#define GPRS_H_

#define PIN_DEAKT 23   // Stromversorgung deaktivieren, wenn der Ausgang für ca. 1 Sekunde angesteuert wird 

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Datentypen
*/
enum GPRS_ZM {
  // Initialisierung
  GZMINIT = 10,
  GZMIKOM = 11,
  GZMIFUN = 12,
  GZMINEU = 19,
  // SIM-Karte prüfen
  GZMSIMP = 20,
  // Netz prüfen
  GZMNEQU = 30,
  GZMNERE = 31,
  GZMNEAN = 32,
  // GPRS Verbindungsaufbau - FTP / HTTP
  GZMFHPS = 40,
  GZMFHVE = 41,
  GZMFHVT = 42,
  // GPRS Verbindugnsaufbau - TCP / UDP
  GZMTUVE = 50,
  GZMTUVT = 51,
  GZMTUVA = 52,
  GZMTUAD = 53,
  // FTP-Verbindung aufbauen / Daten auf den Server übertragen
  GZMFTKS = 60,
  GZMFTKD = 61,
  GZMFTWD = 62,
  GZMFTDA = 63,
// FTP-Verbindung / Daten vom Server lesen
  GZMFTKL = 66,
  GZMFTDL = 67,
  GZMFTND = 69,
  
  // TCP-Verbindung aufbauen / Datenaustausch MySQL Server
  GZMSEVE = 71,
  GZMSEDA = 72,
  GZMSEVV = 79,
  // unbekannte / ungültige Zustände
  GZMUNBE = 255,
};

/*
  öffentliche Funktionen
*/
extern void GPRS_Init(void);
extern void GPRS_SerEin(void);
extern void GPRS_Zustandsmaschine(void);
extern void GPRS_SetzeDateiname(String strDatei);
extern void GPRS_Logeintrag(String strEintrag);
extern void GPRS_APN(int iAPN);
extern void GPRS_DateiLesen(char ubyDatei);

#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* GPRS_H_ */
