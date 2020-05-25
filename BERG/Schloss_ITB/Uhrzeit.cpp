/*
  Copyright (c) 2019, Maximilian Johannes Wurmitzer (Welando GmbH)
  All rights reserved.
*/
/*
  Name:    Uhrzeit.cpp
  Datum:   18.04.2018
  Autor:   Markus Emanuel Wurmitzer
*/
/*  History - Headerfile should have the version of the associated module file
    (Versioning: VX.YZ: X..increase for big change or bugfix; Y..incr. for enhanced functionality;
     Z..incr. for structure or documentation changes)

  2020-05-21  V 201 JoWu
    - remove '_" underscore from date-time string

  2019-10-01  V 2.00  Maximilian Johannes Wurmitzer
    
  23.04.2018  V 100   Markus Emanuel Wurmitzer
    - Erstellung
*/
/*
  todo
  2020-05-21; JoWu; check values to avoid 3 digit values in date and time fields 
 
*/
#include "Uhrzeit.h"

#define RTC_TYPE_AUTO     0
#define RTC_TYPE_DS1307   1
#define RTC_TYPE_PCF8523  2

#define RTC_TYPE RTC_TYPE_AUTO    // 0 ... auto detect DS1307 and PCF8523
                                  // 1 ... RTC_DS1307
                                  // 2 ... RTC_PCF8523

//#define RTC_MILLIS              // define, if the millisecond based rtc should be used and the external real rtc is just used for backup

/*
   externe Dateien
*/
#include <RTClib.h>

// static Variables
#if RTC_TYPE == RTC_TYPE_AUTO
  unsigned char  ubrtcType;
#endif

#if RTC_TYPE == RTC_TYPE_AUTO
 RTC_DS1307   rtc_ds;         // RTC DS1307
 RTC_PCF8523  rtc_pcf;        // RTC PCF8523
#elif RTC_TYPE == RTC_TYPE_DS1307
 RTC_DS1307 rtc;              // RTC DS1307
#elif RTC_TYPE == RTC_TYPE_PCF8523
 RTC_PCF8523 rtc;             // RTC PCF8523
#endif
#ifdef RTC_MILLIS
 RTC_Millis rtc_mil;           // RTC based on milliseconds
#endif


bool rboUhrInit = false;

// RTC initialisieren
void UHR_Init(void){
#if RTC_TYPE == RTC_TYPE_AUTO
  DateTime ldtDS_old, ldtDS_new;
  DateTime ldtPC_old, ldtPC_new;
  unsigned char i;
#endif

#if RTC_TYPE == RTC_TYPE_AUTO
  rtc_ds.begin();
  ldtDS_old = rtc_ds.now();
  rtc_pcf.begin();
  ldtPC_old = rtc_pcf.now();
  i = 0;
  do{
    delay(100);
    ldtDS_new = rtc_ds.now();
    ldtPC_new = rtc_pcf.now();
  }while (i++ < 12 && (ldtPC_new.second() != (ldtPC_old.second()+1)%60) && (ldtDS_new.second() != (ldtDS_old.second()+1)%60));
  Serial.print("i = "); Serial.println(i);
  Serial.print("RTC PCF8523 "); Serial.print(ldtPC_old.second()); Serial.print(" "); Serial.println(ldtPC_new.second());
  Serial.print("RTC DS1307  "); Serial.print(ldtDS_old.second()); Serial.print(" "); Serial.println(ldtDS_new.second());
  if (ldtPC_new.second() == (ldtPC_old.second()+1)%60){
    Serial.println("RTC PCF8523 detected"); Serial.print(ldtPC_old.second()); Serial.print(" "); Serial.println(ldtPC_new.second());
    ubrtcType = RTC_TYPE_PCF8523;
    if (!rtc_pcf.initialized()){
      rtc_pcf.adjust(DateTime(2019, 1, 1, 0, 0, 0));
      Serial.println("RTC PCF8523 not initialiced, so tryed to initialice");
    }
    rboUhrInit = true;
  }
  else if (ldtDS_new.second() == (ldtDS_old.second()+1)%60){
    Serial.println("RTC DS1307 detected"); Serial.print(ldtDS_old.second()); Serial.print(" "); Serial.println(ldtDS_new.second());
    ubrtcType = RTC_TYPE_DS1307;
    if (!rtc_ds.isrunning()){
      rtc_ds.adjust(DateTime(2019, 1, 1, 0, 0, 0));
      Serial.println("RTC DS1307 not initialiced so tryed to initialice");
    }
    rboUhrInit = true;
  }
  else{
    Serial.println("RTC not detected default DS1307");
    ubrtcType = RTC_TYPE_DS1307;
    if (!rtc_ds.isrunning()){
      rtc_ds.adjust(DateTime(2019, 1, 1, 0, 0, 0));
      Serial.println("RTC DS1307 tryed to initialice");
    }
    rboUhrInit = true;
  }
#elif RTC_TYPE == RTC_TYPE_DS1307
  rtc.begin();
  if (!rtc.isrunning()){
    rtc.adjust(DateTime(2019, 1, 1, 0, 0, 0));
    Serial.println("RTC DS1307 not running, tryed to initialice");
  }
  rboUhrInit = true;
#elif RTC_TYPE == RTC_TYPE_PCF8523
  Serial.println("RTC PCF8523 begin");
  rtc.begin();
  if (!rtc.initialized()) {
    rtc.adjust(DateTime(2019, 1, 1, 0, 0, 0));
    Serial.println("RTC PCF8523 not initialiced, tryed to initialice");
  }
  rboUhrInit = true;
#endif

#ifdef RTC_MILLIS
 #if RTC_TYPE == RTC_TYPE_AUTO
  if (ubrtcType == RTC_TYPE_DS1307) {
    rtc_mil.adjust(rtc_ds.now());
  }
  else if(ubrtcType == RTC_TYPE_PCF8523){
    rtc_mil.adjust(rtc_pcf.now());
  }
 #elif RTC_TYPE == RTC_TYPE_DS1307
  rtc_mil.adjust(rtc.now());
 #elif RTC_TYPE == RTC_TYPE_PCF8523
  rtc_mil.adjust(rtc.now());
 #endif
#endif
}

// Status prüfen
// Rückgabewert:
// true = läuft, false = läuft nicht
bool UHR_Status(void)
{
 #if RTC_TYPE == RTC_TYPE_AUTO
  if (ubrtcType == RTC_TYPE_DS1307) {
    return rtc_ds.isrunning() && rboUhrInit;
  }
  else if(ubrtcType == RTC_TYPE_PCF8523){
    return rtc_pcf.initialized() && rboUhrInit;
  }
  else return false;
 #elif RTC_TYPE == RTC_TYPE_DS1307
    return rtc.isrunning() && rboUhrInit;
 #elif RTC_TYPE == RTC_TYPE_PCF8523
    return rtc.initialized() && rboUhrInit;
 #endif
}

// aktuelles Datum und Uhrzeit lesen
// Rückgabewert:
// aktuelle Uhrzeit, oder "Fehler" wenn diese nicht läuft
String UHR_Lesen(void)
{
  DateTime Zeit;

 #if RTC_TYPE == RTC_TYPE_AUTO
  if (ubrtcType == RTC_TYPE_DS1307) {
    Zeit = rtc_ds.now();
  }
  else if(ubrtcType == RTC_TYPE_PCF8523){
    Zeit = rtc_pcf.now();
  }
 #elif RTC_TYPE == RTC_TYPE_DS1307
    Zeit = rtc.now();
 #elif RTC_TYPE == RTC_TYPE_PCF8523
    Zeit = rtc.now();
 #endif

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
// aktuelle Uhrzeit "YYYYMMDD HH:MM:SS, oder "Fehler" wenn diese nicht läuft
String UHR_Logzeit(void){
  DateTime Zeit;
  
  char lStr[20];
  String strDatumZeit;  
 #if RTC_TYPE == RTC_TYPE_AUTO
  if (ubrtcType == RTC_TYPE_DS1307) {
    Zeit = rtc_ds.now();
  }
  else if(ubrtcType == RTC_TYPE_PCF8523){
    Zeit = rtc_pcf.now();
  }
 #elif RTC_TYPE == RTC_TYPE_DS1307
    Zeit = rtc.now();
 #elif RTC_TYPE == RTC_TYPE_PCF8523
    Zeit = rtc.now();
 #endif

  sprintf(lStr,"%04d%02d%02d %02d:%02d:%02d",Zeit.year(), Zeit.month(), Zeit.day(), Zeit.hour(), Zeit.minute(), Zeit.second());
  //return Zeit.timestamp();
  strDatumZeit = lStr;
  return strDatumZeit;
}

// Uhrzeit schreiben
// Parameter:
// DatumZeit = "19.04.2018_11:09:25"
// Rückgabewert:
// geschriebene Uhrzeit, bzw. "Fehler" wenn diese nicht läuft
String UHR_Schreiben(String DatumZeit)
{
  if (UHR_Status){
    // Zeit setzen
#if RTC_TYPE == RTC_TYPE_AUTO
    if (ubrtcType == RTC_TYPE_DS1307) {
      rtc_ds.adjust(DateTime(DatumZeit.substring(6, 10).toInt(), DatumZeit.substring(3, 5).toInt(), DatumZeit.substring(0, 2).toInt(), DatumZeit.substring(11, 13).toInt(), DatumZeit.substring(14, 16).toInt(), DatumZeit.substring(17, 19).toInt()));
    }
    else if(ubrtcType == RTC_TYPE_PCF8523){
      rtc_pcf.adjust(DateTime(DatumZeit.substring(6, 10).toInt(), DatumZeit.substring(3, 5).toInt(), DatumZeit.substring(0, 2).toInt(), DatumZeit.substring(11, 13).toInt(), DatumZeit.substring(14, 16).toInt(), DatumZeit.substring(17, 19).toInt()));
    }
#elif RTC_TYPE == RTC_TYPE_DS1307
    rtc.adjust(DateTime(DatumZeit.substring(6, 10).toInt(), DatumZeit.substring(3, 5).toInt(), DatumZeit.substring(0, 2).toInt(), DatumZeit.substring(11, 13).toInt(), DatumZeit.substring(14, 16).toInt(), DatumZeit.substring(17, 19).toInt()));
#elif RTC_TYPE == RTC_TYPE_PCF8523
    rtc.adjust(DateTime(DatumZeit.substring(6, 10).toInt(), DatumZeit.substring(3, 5).toInt(), DatumZeit.substring(0, 2).toInt(), DatumZeit.substring(11, 13).toInt(), DatumZeit.substring(14, 16).toInt(), DatumZeit.substring(17, 19).toInt()));
#endif
    return DatumZeit;
  }
  else
  {
    if (rboUhrInit){
      return "Fehler: Uhr nicht initialisiert";
    }
    else{
      return "Fehler: Uhr nicht vorhanden";
    }
  }
}
