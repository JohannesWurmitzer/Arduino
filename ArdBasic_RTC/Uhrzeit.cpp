/*
  Name:    Uhrzeit.cpp
  Datum:   18.04.2018
  Autor:   Markus Emanuel Wurmitzer

  Versionsgeschichte:
  23.04.2018  V 100   Markus Emanuel Wurmitzer
    - Erstellung
  05.09.2018  V 101   Gollner Björn
    - Implementierung überprüfung RTC Chip
*/
#include "Uhrzeit.h"
#include <Wire.h>

//#define RTC_TYPE 1     // 1 ... RTC_DS1307
// 2 ... RTC_PCF8523
/*
   externe Dateien
*/
#include <RTClib.h>


//RTC Check definitionen
#if (ARDUINO >= 100)
#include <Arduino.h> // capital A so it is error prone on case-sensitive filesystems
// Macro to deal with the difference in I2C write functions from old and new Arduino versions.
#define _I2C_WRITE write
#define _I2C_READ  read
#else
#include <WProgram.h>
#define _I2C_DSWRITE send
#define _I2C_READ  receive
#endif
static uint8_t bcd2bin (uint8_t val) {
  return val - 6 * (val >> 4);
}
bool DS = true;
uint8_t startbit = 0;
RTC_DS1307 rtc_ds;
RTC_PCF8523 rtc_pcf;

// remanente Variablen
/*#if RTC_TYPE == 1
  RTC_DS1307 rtc;
  #else
  RTC_PCF8523 rtc;
  #endif*/

bool rboUhrInit = false;

bool rtcbegin(void) {
  Wire.begin();
  return true;
}

// RTC initialisieren
void UHR_Init(void)
{
  if (rtcbegin())
  {
    for (int i = 0; i < 3; i++) {
      Wire.beginTransmission(PCF8523_ADDRESS);
      Wire._I2C_WRITE((byte)0);
      Wire.endTransmission();

      Wire.requestFrom(PCF8523_ADDRESS, 7);
      startbit = startbit + bcd2bin(Wire._I2C_READ() & 0x7F);
      delay(1000);
    }
    if (startbit == 0) {
      DS = false;
    }

    DS = true; // JoWu!!! 2018-10-20, RTC-Type-Detection seams to be wrong ==> remove comment if DS1307

    if (DS) {
      if (!rtc_ds.isrunning())
      {
        rtc_ds.adjust(DateTime(2018, 1, 1, 0, 0, 0));
      }
      rboUhrInit = true;
    } else {
      if (! rtc_pcf.initialized()) {
        rtc_pcf.adjust(DateTime(2018, 1, 1, 0, 0, 0));
        rboUhrInit = true;

      }
    }

  }

}

// Status prüfen
// Rückgabewert:
// true = läuft, false = läuft nicht
bool UHR_Status(void)
{
  if (DS) {
    return rtc_ds.isrunning() && rboUhrInit;
  } else {
    return true;
  }
}

// aktuelles Datum und Uhrzeit lesen
// Rückgabewert:
// aktuelle Uhrzeit, oder "Fehler" wenn diese nicht läuft
String UHR_Lesen(void)
{

  DateTime Zeit;
  if (DS) {
    Zeit = rtc_ds.now();
    //Serial.println("Load DS DateTime");
  } else {
    Zeit = rtc_pcf.now();
    //Serial.println("Load PCF DateTime");
  }

  if (UHR_Status)
  {
    return String(Zeit.day()) + "." + String(Zeit.month()) + "." + String(Zeit.year()) + "_" + String(Zeit.hour()) + ":" + String(Zeit.minute()) + ":" + String(Zeit.second());
    //return String(Zeit.unixtime());
  }
  else
  {
    if (rboUhrInit)
    {
      return "Fehler: Uhr nicht initialisiert";
    }
    return "Fehler: Uhr nicht vorhanden";
  }
}

// aktuelles Datum und Uhrzeit für den Logeintrag lesen
// Rückgabewert:
// aktuelle Uhrzeit, oder "Fehler" wenn diese nicht läuft
String UHR_Logzeit(void)
{
  DateTime Zeit;

  if (DS) {
    Zeit = rtc_ds.now();
    //Serial.println("Load DS DateTime");
  } else {
    Zeit = rtc_pcf.now();
    //Serial.println("Load PCF DateTime");
  }
  String strDatumZeit = String(Zeit.year());
  if (Zeit.month() < 10)
  {
    strDatumZeit += "0";
  }
  strDatumZeit += String(Zeit.month());
  if (Zeit.day() < 10)
  {
    strDatumZeit += "0";
  }
  strDatumZeit += String(Zeit.day());
  strDatumZeit += "_";
  if (Zeit.hour() < 10)
  {
    strDatumZeit += "0";
  }
  strDatumZeit += String(Zeit.hour()) + ":";
  if (Zeit.minute() < 10)
  {
    strDatumZeit += "0";
  }
  strDatumZeit += String(Zeit.minute()) + ":";
  if (Zeit.second() < 10)
  {
    strDatumZeit += "0";
  }
  strDatumZeit += String(Zeit.second());

  return strDatumZeit;
}

// Uhrzeit schreiben
// Parameter:
// DatumZeit = "19.04.2018_11:09:25"
// Rückgabewert:
// geschriebene Uhrzeit, bzw. "Fehler" wenn diese nicht läuft
String UHR_Schreiben(String DatumZeit)
{
  if (UHR_Status)
  {
    if (DS) {
      // vorerst fixe Zeit setzen
      rtc_ds.adjust(DateTime(DatumZeit.substring(6, 10).toInt(), DatumZeit.substring(3, 5).toInt(), DatumZeit.substring(0, 2).toInt(), DatumZeit.substring(11, 13).toInt(), DatumZeit.substring(14, 16).toInt(), DatumZeit.substring(17, 19).toInt()));
      return DatumZeit;
    } else {
      rtc_pcf.adjust(DateTime(DatumZeit.substring(6, 10).toInt(), DatumZeit.substring(3, 5).toInt(), DatumZeit.substring(0, 2).toInt(), DatumZeit.substring(11, 13).toInt(), DatumZeit.substring(14, 16).toInt(), DatumZeit.substring(17, 19).toInt()));
      return DatumZeit;
    }
  }
  else
  {
    if (rboUhrInit)
    {
      return "Fehler: Uhr nicht initialisiert";
    }
    return "Fehler: Uhr nicht vorhanden";
  }
}
