/*
 Name:    Log.cpp
 Datum:   18.04.2018
 Autor:   Markus Emanuel Wurmitzer

  Versionsgeschichte:

  2020-09-06  V105  JoWu
    - add return value of success info of LOG_Init()

  2020-08-16  V104  JoWu  
    - reformat function LOG_DatInhalt() to make it more readable while searching the read bug
    - moved "File" objects from static local to global
    
  2020-05-21  V103  JoWu  remove '_' underscore in date-time format string
  
  2018-10-06  V102  JoWu  add serial debug log
  18.05.2018  V 101   Markus Emanuel Wurmitzer
    - Einbindung der GPRS-FTP Funktion
    
  23.04.2018  V 100   Markus Emanuel Wurmitzer
    - Erstellung
*/
#include "Log.h"
#include "Uhrzeit.h"
#include "GPRS.h"

/*
 * externe Dateien
 */
#include <SPI.h>
#include <SD.h>

//#define SERIAL_DEBUG_ENABLE

// remanente Variablen
bool rboLogInit;
int riLogPin;

File LogDatei;          // actual used Log-File
File Verzeichnis;       // actual used Log-Verzeichnis
File LogReadDatei;      // actual used Log-File for Read-Out

// SD Karte initialisieren
// Parameter:
// pin = verwendeter Pin ist abhängig vom Arduino
bool LOG_Init(int pin){
  byte lbyInitCnt;
  riLogPin = pin;
  delay(200);
#if 0
//  void clearSD()
  {
    byte sd = 0;
    pinMode(riLogPin, OUTPUT);
    SPI.begin();
    digitalWrite(SS, LOW);
    lbyInitCnt = 0;
    while (lbyInitCnt != 255)
    {
      sd = SPI.transfer(255);
      if (sd == 255) lbyInitCnt++;
      Serial.print(" sd=");
      Serial.print(sd);
    }
    digitalWrite(SS, HIGH);
  }
  SPI.end();
  delay(200);
#endif
  lbyInitCnt = 0;
  do{
    pinMode(riLogPin, OUTPUT);
    digitalWrite(riLogPin, HIGH);
    delay(1);
    rboLogInit = SD.begin(riLogPin);
//    rboLogInit = SD.begin(SPI_FULL_SPEED, riLogPin);
//    rboLogInit = SD.begin(SPI_HALF_SPEED, riLogPin);
//    rboLogInit = SD.begin(SPI_QUARTER_SPEED, riLogPin);
  
#ifdef SERIAL_DEBUG_ENABLE
    if (!rboLogInit){
      Serial.print("ERR; Init: SD.begin()"); Serial.print(SD.error()); Serial.println();
      SD.end();
    }
    else{
      Serial.println("MSG; SD Init Okay");
      SD.end();
    }
#endif
  }while(!rboLogInit && lbyInitCnt++ < 2);
  return(rboLogInit);
}

// SD Karte erneut reinitialisieren
// https://openlabpro.com/guide/interfacing-microcontrollers-with-sd-card/
// https://electronics.stackexchange.com/questions/77417/what-is-the-correct-command-sequence-for-microsd-card-initialization-in-spi

void LOG_ReInit(void){
  rboLogInit = SD.begin(riLogPin);  
//  rboLogInit = SD.begin(SPI_HALF_SPEED, riLogPin);  
#ifdef SERIAL_DEBUG_ENABLE
  if (!rboLogInit){
    Serial.print("ERR; ReInit: SD.begin()"); Serial.print(SD.error()); Serial.println();
  }
#endif
}

// Dateianzahl auslesen
// Rückgabewert:
// Anzahl oder Fehlermeldung
String LOG_DatAnz(void){ 
  // erneut versuchen die Karte zu initialisieren
  if (!rboLogInit)
  {
    LOG_ReInit();
  }

  // Dateianzahl auslesen
  if (rboLogInit)
  {
    int iAnz = 0;
    File Verzeichnis = SD.open("/");
    Verzeichnis.rewindDirectory();
    bool boDatei = true;
    while(boDatei)
    {
      File Datei = Verzeichnis.openNextFile();
      if (Datei)
      {
        if (!Datei.isDirectory())
        {
          iAnz++;
        }
        Datei.close();
      }
      else
      {
        boDatei = false;
      }
    }
    Verzeichnis.close();
    return String(iAnz);
  }
  return "Fehler: Karte nicht vorhanden";
}

// Dateinamen auslesen
// Parameter:
// iID = Nummer in der Liste
// Rückgabewert:
// Dateiname oder Fehler
String LOG_DatName(int iID){
  static int iIDAkt = 0;
  String strAntwort = String(iID) + " ";
  String strDatei;
  
  // erneut versuchen die Karte zu initialisieren
  if (!rboLogInit)
  {
    LOG_ReInit();
    if (Verzeichnis)
    {
      Verzeichnis.close();
    }
  }


  if (rboLogInit)
  { 
    // Verzeichnis öffnen / erneut öffnen  
    if ((!Verzeichnis) || (iIDAkt >= iID))
    {
      if (iIDAkt > 0)
      {
        Verzeichnis.close();
      }      
      Verzeichnis = SD.open("/");
      Verzeichnis.rewindDirectory();
      iIDAkt = 0;
    }

    // Dateien durchgehen
    while (iIDAkt < iID)
    {
      File Datei = Verzeichnis.openNextFile();     
      if (!Datei)
      {
        return strAntwort + "Fehler: Datei nicht vorhanden";
      }
      if (!Datei.isDirectory())
      {
        iIDAkt++;
        strDatei = Datei.name();
      }
      Datei.close();
    }
    return strAntwort + strDatei;
  }

  return strAntwort + "Fehler: Karte nicht vorhanden";
}

// Zeile aus einer Datei auslesen
// Parameter:
// strDatei = Dateiname
// iID = Zeilennummer
// Rückgabewert:
// Zeile oder Fehlermeldung
String LOG_DatInhalt(String strDatei, int iID){
  static int iIDAkt = 0;
  String strAntwort = strDatei + " " + String(iID) + " ";
  String strZeile;
  // erneut versuchen die Karte zu initialisieren
  if (!rboLogInit){
    LOG_ReInit();
//    Serial.println("ERR; Log.c; LOG_ReInit()");
  }

  if (rboLogInit){
    if (LogReadDatei){
      String strAktDatei = LogReadDatei.name();
      if (strAktDatei != strDatei){
        LogReadDatei.close();
      }
    }

    if (!LogReadDatei){
      LogReadDatei = SD.open(strDatei,FILE_READ);
      iIDAkt = 0;
    }

    if (LogReadDatei){
      if (iIDAkt >= iID){
        LogReadDatei.seek(0);
        iIDAkt = 0;
//        Serial.println("ERR; Log.c; iIDAkt >= iID");
      }
  
      while (iIDAkt < iID){ 
        if (!LogReadDatei.available()){
          LogReadDatei.close();
          return strAntwort + "Dateiende";
        }
        char byZeichen = LogReadDatei.read();
        if (byZeichen == 10){
          // Zeilenende
          iIDAkt++;        
        }
        else if (byZeichen != 13){
          // Antwort verlängern
          if (iIDAkt == iID-1){
            strZeile += String(byZeichen);
          }
        }
      }
      return strAntwort + strZeile;
    }
    return strAntwort + "Fehler: Datei nicht vorhanden";
  }
  return strAntwort + "Fehler: Karte nicht vorhanden";
}

// Datei entfernen
// Parameter:
// strDatei = Dateiname
// Rückgabewert:
// Meldung
String Log_DatEntf(String strDatei){
  // erneut versuchen die Karte zu initialisieren
  if (!rboLogInit)
  {
    LOG_ReInit();
  }

  if (rboLogInit)
  {
    // Datei prüfen
    if (SD.exists(strDatei))
    {
      // Datei entfernen
      if (SD.remove(strDatei))
      {
        return "- " + strDatei;
      }
      return "Fehler: Datei nicht entfernt";
    }
    return "Fehler: Datei nicht vorhanden";
  }
  return "Fehler: Karte nicht vorhanden";
}

// Logeintrag schreiben
// Parameter:
// strMeldung = Text, der in die Logdatei geschrieben werden muss
void LOG_Eintrag(String strMeldung){
    String strZeit = UHR_Logzeit();
    String strDatei = strZeit.substring(0, strZeit.indexOf(" ")) + ".LOG";

    // korrekte Datei öffnen
    // SD-Card limit 8.3 format for filenames
    if ((!LogDatei) || (String(LogDatei.name()) != strDatei))
    {
      if (LogDatei)
      {
        LogDatei.close();
      }
      LogDatei = SD.open(strDatei, FILE_WRITE);
    }

    // Eintrag schreiben
    LogDatei.println(strZeit.substring(strZeit.indexOf(" ") + 1) + " " + strMeldung);
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("Log:");
  Serial.println(strZeit.substring(strZeit.indexOf(" ") + 1) + " " + strMeldung);
#endif 
    
    LogDatei.flush();

    // FTP Eintrag generieren
    GPRS_Logeintrag(strZeit + char(9) + strMeldung + "\r\n");
}
