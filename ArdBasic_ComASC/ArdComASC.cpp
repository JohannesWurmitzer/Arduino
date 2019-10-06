/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer (Welando GmbH)
  All rights reserved.
*/
/*
  Name:    ArdComASC.cpp
  Datum:   2019-10-02
  Autor:   Markus Emanuel Wurmitzer
*/
/*  History - Headerfile should have the version of the associated module file
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)

  2019-10-01  V 2.00  Maximilian Johannes Wurmitzer
    
  23.04.2018  V 100   Markus Emanuel Wurmitzer
    - Erstellung
*/
#include "ArdComASC.h"


/*
   externe Dateien
*/

// static Variables
// serielle Kommunikation
char gbyKom[64];                                // Kommunikation Eingang, maximal 64 Byte, davon gibt es ein Start und Endezeichen!
int giKomIdx;                                   // Kommunikation Index im Eingangsfeld
bool gboKomEin;                                 // Kommunikationseingang erkannt
bool gboKomAus;                                 // Kommunikationsausgang vorhanden
String gstrKomEin;                              // Eingangsdaten
String gstrKomEinBef;                           // Eingangsdaten "Befehl"
String gstrKomEinDat;                           // Eingangsdaten "Daten"
String gstrKomAus;                              // Ausgangsdaten





// RTC initialisieren
void ArdComASC_Setup(void){

}

void ArdComASC_Loop(void){
  // serielle Kommunikation
  // auf Dateneingang warten
  
  gboKomEin = ArdComASCFrameAddByte(Serial);

  // Protokoll auswerten
  if (gboKomEin)
  {
    // Zeichenkette ohne Startzeichen erstellen
    gstrKomEin = String(gbyKom);
    gstrKomEinBef = gstrKomEin.substring(1,4);
    gstrKomEinDat = gstrKomEin.substring(5, giKomIdx - 1);
    gstrKomAus = String("#") + gstrKomEinBef + ' ';

    gboKomAus = true;

    // Befehle abfragen
    if (gstrKomEinBef == "VER")
    {
      // Versionsnummer abfragen
      gstrKomAus += lstrVER;
    }
    else if (gstrKomEinBef == "SER")
    {
      // Seriennummer abfragen
//      gstrKomAus += EEPROM_SNrLesen();
    }
    else if (gstrKomEinBef == "SES")
    {
      // Seriennummer schreiben
//      gstrKomAus += EEPROM_SNrSchreiben(gstrKomEinDat);
      // Neue Seriennummer übernehmen
//      GPRS_SetzeDateiname(EEPROM_SNrLesen());
    }    
    else if (gstrKomEinBef == "TKA")
    {
      // Test "keine Antwort"
      gboKomAus = false;
    }
    else if (gstrKomEinBef == "RST")
    {
      // Neustart durchführen nach X ms
//      wdt_enable(WDTO_250MS);
    }
    else if (gstrKomEinBef == "ZTL")
    {
      // Uhrzeit lesen
//      gstrKomAus += UHR_Lesen();
    }
    else if (gstrKomEinBef == "ZTS")
    {
      // Uhrzeit schreiben
//      gstrKomAus += UHR_Schreiben(gstrKomEinDat);
    }
    else if (gstrKomEinBef == "STA")
    {
      // Statusabfrage generieren
//      gstrKomAus += EEPROM_StatusLesen(); 
    }    
    else if (gstrKomEinBef == "AAn")
    {
      // Artikelanzahl auslesen
//      gstrKomAus += String(EEPROM_AnzEintraege('A'));
    }
    else if (gstrKomEinBef == "BAn")
    {
      // Benutzeranzahl auslesen
//      gstrKomAus += String(EEPROM_AnzEintraege('B'));
    }
    else if (gstrKomEinBef == "AxL")
    {
      // Artikel Nummer X auslesen
//      gstrKomAus += gstrKomEinDat + EEPROM_LiesEintrag('A', gstrKomEinDat.toInt());
    }
    else if (gstrKomEinBef == "AN+")
    {
      // neuen Artikel anlegen
//      gstrKomAus += EEPROM_NeuEintrag('A', gstrKomEinDat);
    }
    else if (gstrKomEinBef == "AN-")
    {
      // Artikel entfernen
//      gstrKomAus += EEPROM_EntfEintrag('A', gstrKomEinDat.toInt());
    }
    else if (gstrKomEinBef == "ANL")
    {
      // Artikelnummern löschen
//      gstrKomAus += EEPROM_EntfNummern('A');
    }   
    else if (gstrKomEinBef == "BxL")
    {
      // Benutzer Nummer X auslesen
//      gstrKomAus += gstrKomEinDat + EEPROM_LiesEintrag('B', gstrKomEinDat.toInt());
    }
    else if (gstrKomEinBef == "BN+")
    {
      // neuen Benutzer anlegen
//      gstrKomAus += EEPROM_NeuEintrag('B', gstrKomEinDat);
    }
    else if (gstrKomEinBef == "BN-")
    {
      // Benutzer entfernen
//      gstrKomAus += EEPROM_EntfEintrag('B', gstrKomEinDat.toInt());
    }
    else if (gstrKomEinBef == "BNL")
    {
      // Benutzernummern löschen
//      gstrKomAus += EEPROM_EntfNummern('B');
    }
    else if (gstrKomEinBef == "DAn")
    {
      // Dateianzahl auslesen
//      gstrKomAus += LOG_DatAnz();
    }
    else if (gstrKomEinBef == "DXL")
    {
      // Dateinamen auslesen
//      gstrKomAus += LOG_DatName(gstrKomEinDat.toInt());
    }
    else if (gstrKomEinBef == "IXL")
    {
      // Dateiinhalt auslesen
//      gstrKomAus += LOG_DatInhalt(gstrKomEinDat.substring(0, gstrKomEinDat.indexOf(" ")), gstrKomEinDat.substring(gstrKomEinDat.indexOf(" "), gstrKomEinDat.length()).toInt());
    }
    else if (gstrKomEinBef == "DXE")
    {
      // Datei entfernen
//      gstrKomAus += Log_DatEntf(gstrKomEinDat);
    }
    else if (gstrKomEinBef == "MVW")
    {
      // Motor vorwärts bewegen
//      setMotorLockCommand(MOTOR_DIRECT_DIR_LOCK);
    }
    else if (gstrKomEinBef == "MRW")
    {
      // Motor vorwärts bewegen
//      setMotorLockCommand(MOTOR_DIRECT_DIR_UNLOCK);
    }
    else if (gstrKomEinBef == "PAL")
    {
      // Parameter lesen
//      gstrKomAus += gstrKomEinDat + " " + EEPROM_ParLesen(gstrKomEinDat.substring(0,3), gstrKomEinDat.substring(4).toInt());
    }
    else if (gstrKomEinBef == "PAS")
    {
      // Parameter schreiben
      int iAdr =  gstrKomEinDat.substring(4, gstrKomEinDat.lastIndexOf(" ")).toInt();
      int iWert = gstrKomEinDat.substring(gstrKomEinDat.lastIndexOf(" ") + 1).toInt();
      String strTyp = gstrKomEinDat.substring(0,3);
      
//      gstrKomAus += EEPROM_ParSchreiben(strTyp, iAdr, iWert);

      // Parameter im Ablauf übernehmen
      if (strTyp == "16+")
      {
        switch (iAdr)
        {
          case 1:
            // Motor: Überwachungszeit Schließen
//            MOT_ParZeiten(1, iWert);
            break;
          case 2:
            // Motor: Überwachungszeit Öffnen
//            MOT_ParZeiten(2, iWert);
            break;
        }
      }
      else if (strTyp == "08+")
      {
        switch (iAdr)
        {
          case 5:
            // Motor: Rückfahrzeit
//            MOT_ParZeiten(3, iWert);
            break;
          case 6:
            // mobiles Internet APN Einstellung
//            GPRS_APN(iWert);
            break;          
        }
      }
    }
    else if (gstrKomEinBef == "EZL")
    {
      // EEPROM-Zeile (8-Bytes) lesen
//      gstrKomAus += EEPROM_ZeileLesen(gstrKomEinDat.toInt());
    }
    else
    {
      // unbekannter Befehl
      gstrKomAus += "?";
    }   

    // Antwort senden
    if (gboKomAus)
    {
      Serial.print(gstrKomAus + '\r');
      gboKomAus = false;
    }
    
    // Variablen zurücksetzen
    giKomIdx = 0;
    gboKomEin = false;
  }
}
