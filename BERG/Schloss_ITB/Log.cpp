/*
 Name:    Log.cpp
 Datum:   18.04.2018
 Autor:   Markus Emanuel Wurmitzer

  Versionsgeschichte:
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
//#include <SPI.h>
#include <SD.h>

//#define SERIAL_DEBUG_ENABLE

// remanente Variablen
bool rboLogInit;
int riLogPin;

// SD Karte initialisieren
// Parameter:
// pin = verwendeter Pin ist abhängig vom Arduino
void LOG_Init(int pin)
{ 
  riLogPin = pin;
  pinMode(riLogPin, OUTPUT);
  digitalWrite(riLogPin, HIGH);
  rboLogInit = SD.begin(riLogPin);
//  rboLogInit = SD.begin(SPI_HALF_SPEED, riLogPin);

#ifdef SERIAL_DEBUG_ENABLE
  if (!rboLogInit){
    Serial.println("ERR; Init: SD.begin()");
  }
#endif
}

// SD Karte erneut reinitialisieren
void LOG_ReInit(void)
{
  rboLogInit = SD.begin(riLogPin);  
//  rboLogInit = SD.begin(SPI_HALF_SPEED, riLogPin);  
#ifdef SERIAL_DEBUG_ENABLE
  if (!rboLogInit){
    Serial.println("ERR; ReInit: SD.begin()");
  }
#endif
}

// Dateianzahl auslesen
// Rückgabewert:
// Anzahl oder Fehlermeldung
String LOG_DatAnz(void)
{ 
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
String LOG_DatName(int iID)
{
  static int iIDAkt = 0;
  static File Verzeichnis;
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
String LOG_DatInhalt(String strDatei, int iID)
{
  static File Datei;
  static int iIDAkt = 0;
  String strAntwort = strDatei + " " + String(iID) + " ";
  String strZeile;

  // erneut versuchen die Karte zu initialisieren
  if (!rboLogInit)
  {
    LOG_ReInit();
  }


  if (rboLogInit)
  {
  if (Datei)
  {
    String strAktDatei = Datei.name();
    if (strAktDatei != strDatei)
    {
      Datei.close();
    }
  }

  if (!Datei)
  {
    Datei = SD.open(strDatei,FILE_READ);
    iIDAkt = 0;
  }

  if (Datei)
  {
    if (iIDAkt >= iID)
    {
      Datei.seek(0);
    }

    while (iIDAkt < iID)
    { 
      if (!Datei.available())
      {
        Datei.close();
        return strAntwort + "Dateiende";
      }
      char byZeichen = Datei.read();
      if (byZeichen == 10)
      {
        // Zeilenende
        iIDAkt++;        
      }     
      else if (byZeichen != 13)
      {
        // Antwort verlängern
        strZeile += String(byZeichen);
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
String Log_DatEntf(String strDatei)
{
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
void LOG_Eintrag(String strMeldung)
{
    static File Datei;
    String strZeit = UHR_Logzeit();
    String strDatei = strZeit.substring(0, strZeit.indexOf(" ")) + ".LOG";

    // korrekte Datei öffnen
    if ((!Datei) || (String(Datei.name()) != strDatei))
    {
      if (Datei)
      {
        Datei.close();
      }
      Datei = SD.open(strDatei, FILE_WRITE);
    }

    // Eintrag schreiben
    Datei.println(strZeit.substring(strZeit.indexOf(" ") + 1) + " " + strMeldung);
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("Log:");
  Serial.println(strZeit.substring(strZeit.indexOf(" ") + 1) + " " + strMeldung);
#endif 
    
    Datei.flush();

    // FTP Eintrag generieren
    GPRS_Logeintrag(strZeit + char(9) + strMeldung + "\r\n");
}
