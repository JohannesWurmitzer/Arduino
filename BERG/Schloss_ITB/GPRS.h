/*
 Name:    GPRS.h
 Datum:   17.05.2018
 Autor:   Markus Emanuel Wurmitzer

  Versionsgeschichte:
  2020-05-22  V103  JoWu minor changes
  
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

#define PIN_PWRKEY 23   // ITB Stromversorgung ON_OFF aktivieren/deaktivieren, wenn der Ausgang für ca. 1 Sekunde mit high angesteuert wird 
//#define PIN_PWRKEY 9   // Evalboard Stromversorgung ON_OFF aktivieren/deaktivieren, wenn der Ausgang für ca. 1 Sekunde mit high angesteuert wird 

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Datentypen
*/
enum GPRS_ZM {
  // Initialisierung
  GZM_INIT = 10,                  // Init GSM Module
  GZM_INIT_IPR_RD = 11,           // 
  GZM_INIT_FUN = 12,              // Init GSM Module set functionality +CFUN
  
  GZM_INIT_PWRKEY_ON = 18,        // Init GSM Module Neustart
  GZM_INIT_BOOT_WAIT = 19,        // Init GSM Module Neustart

  // Identifizierung
  GZM_RD_IMEI = 20,               // Init GSM module read IMEI number
  // SIM-Karte prüfen
  GZM_SIM_PIN = 21,
  
  // Netz prüfen
  GZMNEQU = 30,
  GZMNERE = 31,
  GZMNEAN = 32,
  
  // GPRS Verbindungsaufbau - FTP / HTTP - Bearer Service for IP
  GZM_FH_PS   = 40,                 // FTP/HTTP Parameter setzen für Bearer Service
  GZM_FH_VP   = 41,                 // FTP/HTTP Verbindung prüfen, prüfen, ob Bearer steht
  GZM_FH_VE   = 42,                 // FTP/HTTP Verbindung aufbauen
  GZMFHVT = 43,
  
  // GPRS Verbindungsaufbau - TCP / UDP
  GZMTUVE = 50,
  GZMTUVT = 51,
  GZMTUVA = 52,
  GZMTUAD = 53,
  
  // FTP-Verbindung aufbauen / Daten auf den Server übertragen
  GZM_FT_KS = 60,                   // FTP - Konfiguration FTP Serverzugang
  GZM_FT_PUT_KD = 61,               // FTP - Konfiguration Datei für FTPPUT
  GZMFTWD = 62,                     // FTP - Warte auf Daten
  GZM_FTP_PUT_START = 63,           // FTP - Put starten (schreibe Daten) 
  GZM_FTP_PUT_WRITE = 64,           // FTP - Put schreiben
  GZM_FTP_PUT_CLOSE = 65,           // FTP - Put beenden (schreibe Daten)
  
  // FTP-Verbindung / Daten vom Server lesen
  GZMFTKL = 66,
  GZMFTDL = 67,
  GZMFTND = 69,
  
  // TCP-Verbindung aufbauen / Datenaustausch MySQL Server
  GZMSEVE = 71,
  GZMSEDA = 72,
  GZMSEVV = 79,
  
  // unbekannte / ungültige Zustände 
  GZM_UNBE = 255,         // unbekannter Zustand
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
