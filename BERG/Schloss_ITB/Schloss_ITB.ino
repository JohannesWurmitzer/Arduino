/*
 Name:    Schloss.ino
 Datum:   16.04.2018
 Autor:   Markus Emanuel Wurmitzer / Edmund Titz (Applikation V 0)

  Versionsgeschichte:
  2019-07-23  V 111   JoWu
    - Integrated Scheduler V4.00
    - Info
      Der Sketch verwendet 49840 Bytes (19%) des Programmspeicherplatzes. Das Maximum sind 253952 Bytes.
      Globale Variablen verwenden 5199 Bytes (63%) des dynamischen Speichers, 2993 Bytes für lokale Variablen verbleiben. Das Maximum sind 8192 Bytes.
      
  06.09.2018  V 110   Maximilian Johannes Wurmitzer
    - StrongLink Reader integriert
    - Info
      Der Sketch verwendet 49702 Bytes (19%) des Programmspeicherplatzes. Das Maximum sind 253952 Bytes.
      Globale Variablen verwenden 5191 Bytes (63%) des dynamischen Speichers, 3001 Bytes für lokale Variablen verbleiben. Das Maximum sind 8192 Bytes.

  06.09.2018  V 109   Maximilian Johannes Wurmitzer
    - RTC DS1307 und RTC_PCF8523 alternativ bestückbar
    
  18.07.2018  V 108   Markus Emanuel Wurmitzer
    - Neuer Parameter "mobiles Internet APN Einstellung" eingefügt
    - Bei ungültigem Benutzer / Artikel wird das Lesen der FTP-Datei ausgelöst
    
  05.06.2018  V 107   Markus Emanuel Wurmitzer
    - Zeitüberwachung für den digitalen Hausmeister geändert auf > 100 ms und < 1100 ms
    - Freigabe der RFID-Lesegeräte nach ca. 2 Sekunden, wenn der Schlüssel entfernt wurde, war bisher auf ca. 5
    
  30.05.2018  V 106   Markus Emanuel Wurmitzer
    - Einbindung des Digitaleingangs 30 für den digitalen Hausmeister
    - Ablauf "Digitaler Hausmeister" im Zyklus 1
    - Programmstartmeldung um Softwareversion erweitert   
  
  18.05.2018  V 105   Markus Emanuel Wurmitzer
    - Neustart bei nicht vorhanden RFID-Lesern deaktiviert
    - Einbindung GPRS Modul (verwendet die vierte serielle Schnittstelle)
    - Einbindung der FTP-Funktionalität
  
  08.05.2018  V 104   Markus Emanuel Wurmitzer
    - Neue Befehle zum Lesen und Schreiben von Parametern
    - Neuer Befehl zum Lesen einer EEPROM-Zeile (16 Bytes)
    - Mindestzeit für Freigabe der Leser, damit nicht konstant ausgelöst wird!
    
  04.05.2018  V 103   Markus Emanuel Wurmitzer
    - Neue serielle Kommandos "Motor vorwärts" und "Motor rückwärts"
  
  03.05.2018  V 102   Markus Emanuel Wurmitzer
    - Fehlerbehebung serielle Kommunikation ("If" ausgebessert auf "While")
    - Neues Protokoll "STA" für Statusabfrage eingefügt
    - Bei Statusänderung des Schlosses auf "geschlossen" wird geprüft,
      ob die Artikelnummer noch vorhanden ist -> sonst löschen der Nummer und Eintrag   
    
  29.04.2018  V 101   Edmund Titz
    Definition für Timer3 eingefügt (MOTOR_ISR_TIME_US)
  23.04.2018  V 100   Markus Emanuel Wurmitzer
    Einbindung der seriellen Kommunikation im zyklischen Ablauf 2 (alle 100 ms)
  16.04.2018  V 0     Edmund Titz
    funktionaler Prototyp (offene Punkte "Brownout", "Watchdog")
*/
/*  todo-list
  open, 2019-07-23; JoWu; Serial2.available() Timout to be implemented

*/
// lokale Konstanten
const String lstrVER = String("ITB1_111_D");       // Softwareversion

//
// Include for SL030 I2C
//
#include <Wire.h>       // I2C Library

#include "ArduSched.h" //configure timing inside the header file
#include "MotorLockHbridge.h"
#include "EepromAndRFID_IDs.h"
#include "TimerThree.h"
#include "SoundAndLed.h"
#include "Uhrzeit.h"
#include "Log.h"
#include "GPRS.h"

#include <avr/wdt.h>              // Zeitüberwachung für Neustart

//++++++++++++++++++++
#include <PN532_HSU.h>
#include <PN532_HSU_2nd.h>
#include <PN532.h>


PN532_HSU pn532hsu(Serial1);      //Serial1 User reader
PN532_HSU_2nd pn532hsu2(Serial2); //Serial2 Article reader
PN532 nfc(pn532hsu);
PN532 nfc2(pn532hsu2);
//++++++++++++++++++++

// Macros

//#define SERIAL_DEBUG_ENABLE

//
//RFID definitions:
//
#define STRONGLINK            // define, if StrongLink Reader are used
// Definition for SL030 I2C
#define SL030ADR 0xA0   // standard address for SL030

#define NON_RFID              0
#define USER_RFID             1
#define ARTICLE_RFID          2
#define USER_AND_ARTICLE_RFID 3

#define OUT_TX_USER_READER    18
#define OUT_TX_ARTICLE_READER 16

#define OUT_TMR3_TIMING_SIG   39

#define OUT_SD                53

#define ZT_FRGLG1           2                   // Freigabe des Lesegeräts nachdem ca. X Sekunden nichts erkannt wurde
#define ZT_FRGLG2           2                   // Freigabe des Lesegeräts nachdem ca. X Sekunden nichts erkannt wurde

// serielle Kommunikation
char gbyKom[64];                                // Kommunikation Eingang, maximal 64 Byte, davon gibt es ein Start und Endezeichen!
int giKomIdx;                                   // Kommunikation Index im Eingangsfeld
bool gboKomEin;                                 // Kommunikationseingang erkannt
bool gboKomAus;                                 // Kommunikationsausgang vorhanden
String gstrKomEin;                              // Eingangsdaten
String gstrKomEinBef;                           // Eingangsdaten "Befehl"
String gstrKomEinDat;                           // Eingangsdaten "Daten"
String gstrKomAus;                              // Ausgangsdaten

// digitlaler Hausmeister
#define DE_DHM    33  //ITB, sonst 30                            // Digitaleingang des digitalen Hausmeisters

// StrongLink RFID Reader
boolean SL030readPassiveTargetID(uint8_t* puid, uint8_t* uidLength, uint8_t u8MaxLen);
boolean SL032readPassiveTargetID(uint8_t uid[], uint8_t *uidLength, uint8_t u8MaxLen);


void Tmr3_ISR();

void(* resetFunc) (void) = 0; //declare reset function @ address 0
String ID_Konvertierung(uint8_t uiL, uint8_t* uiID);

void setup() {
  //...Power up delay (stable supply on all HW components)
  delay(500);
  //...
  //!!!!!!!!!!!!!!!check brownout setting and add watchdog!!!!!!!!!!!!!!!
  
  pinMode(OUT_TX_ARTICLE_READER, OUTPUT);
  pinMode(OUT_TX_USER_READER, OUTPUT);
  pinMode(OUT_TMR3_TIMING_SIG, OUTPUT);//debug timing
  pinMode(OUT_LED, OUTPUT);//LED on Arduino
  
  digitalWrite(OUT_TX_ARTICLE_READER, HIGH);
  digitalWrite(OUT_TX_USER_READER, HIGH);  
  digitalWrite(OUT_TMR3_TIMING_SIG, LOW);//debug timing
  digitalWrite(OUT_LED, HIGH);
  
  //...Power up delay (stable pin state on all HW components)
  delay(500);
  //...
  digitalWrite(OUT_LED, LOW);

#ifdef SERIAL_DEBUG_ENABLE
  Serial.begin(115200);   // (9600);
  Serial.println("Urban sharing lock");
  Serial.print("Firmware Version: ");
  Serial.println(lstrVER);
#endif 

  // serielle Kommunikation mit PC-Programm "Schlossmeister" initialisieren
  giKomIdx = 0;
#ifndef SERIAL_DEBUG_ENABLE
  Serial.begin(115200);  
#endif

  // digitalen Hausmeister initialisieren
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init DHM");
#endif 
  pinMode(DE_DHM, INPUT);

  // Ablauf initialisieren
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init Scheduler");
#endif 
  ArduSchedInit();

  //++++++++++++++++++++++++++++++++++
  //tested delay (min.:500µs ok, 300µs not ok)//max.:10ms too long,
  //5ms ok (bigger 5ms seems to make a timeout in driver??)

  //init user reader (serial1)
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init NFC 1 User-Reader");
#endif 

#ifdef STRONGLINK            // define, if StrongLink Reader are used
  // setup() for SL030 I2C
 #ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init NFC 1 OneWire I2C");
 #endif
  Wire.begin();         // join i2c bus (address optional for master)
#else
  nfc.begin();
  delayMicroseconds(500);
  uint32_t versiondata = nfc.getFirmwareVersion();
  /*
  //------------------------------------------
  if (! versiondata) {
    Serial.print("Didn't find PN53x board1; system reset now!");
    resetFunc();  //call reset
  }
  */
  //------------------------------------------
  delayMicroseconds(500);
  nfc.SAMConfig();//SAM...Secure Access Module
  delay(1);
#endif
  //init article reader (serial2)
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init NFC 2 Articel-Reader");
#endif 
  nfc2.begin();
  delayMicroseconds(500);
  uint32_t versiondata2 = nfc2.getFirmwareVersion();
  /*
  //------------------------------------------
  if (! versiondata) {
    Serial.print("Didn't find PN53x board2; system reset now!");
    resetFunc();  //call reset
  }
  //------------------------------------------
  */
  delayMicroseconds(500);
  nfc2.SAMConfig();
  delay(1);

  //++++++++++++++++++++++++++++++++++
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init timer 3 Period and Interrupt");
#endif 

  Timer3.initialize(MOTOR_ISR_TIME_US);//30ms; 16bit timer! parameter is long variable in micro seconds
  //TimerThree.setPeriod(30000);//30ms
  Timer3.disablePwm(11);
  Timer3.disablePwm(12);
  Timer3.attachInterrupt(Tmr3_ISR);
  //Timer3.detachInterrupt();

  MotorLockInit();
  SoundAndLedInit();
  OkLedSet(LED_CONST_ON);
  ErrorLedSet(LED_CONST_OFF);
  checkIf_EEPROM_HeaderExistsElseWrite();//new boards will not have valid EEPROM header, so write a header into EEPROM
  delay(500);

  // Uhr initialisieren
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init RTC");
#endif 
  UHR_Init();

  // Log initialisieren
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init Log");
#endif 
  LOG_Init(OUT_SD);

  // GPRS initialisieren
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init GPRS");
#endif 
  GPRS_Init();

  // Zeitüberwachung aktivieren
  MCUSR = 0;

  // Parameter des Motors aus dem EEPROM lesen
  MOT_ParZeiten(1, EEPROM_ParLesen("16+", 1).toInt());
  MOT_ParZeiten(2, EEPROM_ParLesen("16+", 2).toInt());
  MOT_ParZeiten(3, EEPROM_ParLesen("08+", 5).toInt());

  // Seriennummer als Dateiname dem GPRS-Modul übergeben
  GPRS_SetzeDateiname(EEPROM_SNrLesen());
  
  // GPRS APN Zugangsdaten auswählen
  GPRS_APN(EEPROM_ParLesen("08+", 6).toInt());

  // Startmeldung generieren
  LOG_Eintrag("Startvorgang: abgeschlossen (V" + lstrVER + ")");
}

void loop() {
  // put your main code here, to run repeatedly:
  // serielle Eingangsdaten des Modems kontinuierlich abfragen
  GPRS_SerEin();
  
  // Aufrufe der einzelnen Aufgaben
  ArduSchedHandler();  
}


void Task1(){//configured with 100ms interval (inside ArduSched.h)
  static int liZtDHM = 0;     // Überwachungszeit Signal "Digitaler Hausmeister" aktiv
  bool lboDHM = false;        // Signal "Digitaler Hausmeister" aktiv         
  
#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  digitalWrite(OUT_Task1, HIGH);
#endif
  //insert code or function to call here:
  SoundAndLedHandler();

  // GPRS Zustandmaschine
  GPRS_Zustandsmaschine();
  
  // serielle Kommunikation
  // auf Dateneingang warten
  while (Serial.available() > 0)
  {
    // Eingangsbyte lesen
    gbyKom[giKomIdx] = Serial.read();

    // Protokollanfang prüfen
    if (gbyKom[0] == '#')
    {
      // Protokollende abfangen
      if (gbyKom[giKomIdx] == 13)
      {
        gboKomEin = true;
      }
      
      // Index erhöhen
      giKomIdx++;   
  
      // ungültige Daten erhalten, Inhalt löschen
      if ((giKomIdx >= 64) && !gboKomEin)
      {    
        giKomIdx = 0;
      }
    }
   }

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
      gstrKomAus += EEPROM_SNrLesen();
    }
    else if (gstrKomEinBef == "SES")
    {
      // Seriennummer schreiben
      gstrKomAus += EEPROM_SNrSchreiben(gstrKomEinDat);
      // Neue Seriennummer übernehmen
      GPRS_SetzeDateiname(EEPROM_SNrLesen());
    }    
    else if (gstrKomEinBef == "TKA")
    {
      // Test "keine Antwort"
      gboKomAus = false;
    }
    else if (gstrKomEinBef == "RST")
    {
      // Neustart durchführen nach X ms
      wdt_enable(WDTO_250MS);
    }
    else if (gstrKomEinBef == "ZTL")
    {
      // Uhrzeit lesen
      gstrKomAus += UHR_Lesen();
    }
    else if (gstrKomEinBef == "ZTS")
    {
      // Uhrzeit schreiben
      gstrKomAus += UHR_Schreiben(gstrKomEinDat);
    }
    else if (gstrKomEinBef == "STA")
    {
      // Statusabfrage generieren
      gstrKomAus += EEPROM_StatusLesen(); 
    }    
    else if (gstrKomEinBef == "AAn")
    {
      // Artikelanzahl auslesen
      gstrKomAus += String(EEPROM_AnzEintraege('A'));
    }
    else if (gstrKomEinBef == "BAn")
    {
      // Benutzeranzahl auslesen
      gstrKomAus += String(EEPROM_AnzEintraege('B'));
    }
    else if (gstrKomEinBef == "AxL")
    {
      // Artikel Nummer X auslesen
      gstrKomAus += gstrKomEinDat + EEPROM_LiesEintrag('A', gstrKomEinDat.toInt());
    }
        else if (gstrKomEinBef == "AN+")
    {
      // neuen Artikel anlegen
      gstrKomAus += EEPROM_NeuEintrag('A', gstrKomEinDat);
    }
    else if (gstrKomEinBef == "AN-")
    {
      // Artikel entfernen
      gstrKomAus += EEPROM_EntfEintrag('A', gstrKomEinDat.toInt());
    }
    else if (gstrKomEinBef == "ANL")
    {
      // Artikelnummern löschen
      gstrKomAus += EEPROM_EntfNummern('A');
    }   
    else if (gstrKomEinBef == "BxL")
    {
      // Benutzer Nummer X auslesen
      gstrKomAus += gstrKomEinDat + EEPROM_LiesEintrag('B', gstrKomEinDat.toInt());
    }
    else if (gstrKomEinBef == "BN+")
    {
      // neuen Benutzer anlegen
      gstrKomAus += EEPROM_NeuEintrag('B', gstrKomEinDat);
    }
    else if (gstrKomEinBef == "BN-")
    {
      // Benutzer entfernen
      gstrKomAus += EEPROM_EntfEintrag('B', gstrKomEinDat.toInt());
    }
    else if (gstrKomEinBef == "BNL")
    {
      // Benutzernummern löschen
      gstrKomAus += EEPROM_EntfNummern('B');
    }
    else if (gstrKomEinBef == "DAn")
    {
      // Dateianzahl auslesen
      gstrKomAus += LOG_DatAnz();
    }
    else if (gstrKomEinBef == "DXL")
    {
      // Dateinamen auslesen
      gstrKomAus += LOG_DatName(gstrKomEinDat.toInt());
    }
    else if (gstrKomEinBef == "IXL")
    {
      // Dateiinhalt auslesen
      gstrKomAus += LOG_DatInhalt(gstrKomEinDat.substring(0, gstrKomEinDat.indexOf(" ")), gstrKomEinDat.substring(gstrKomEinDat.indexOf(" "), gstrKomEinDat.length()).toInt());
    }
    else if (gstrKomEinBef == "DXE")
    {
      // Datei entfernen
      gstrKomAus += Log_DatEntf(gstrKomEinDat);
    }
    else if (gstrKomEinBef == "MVW")
    {
      // Motor vorwärts bewegen
      setMotorLockCommand(MOTOR_DIRECT_DIR_LOCK);
    }
    else if (gstrKomEinBef == "MRW")
    {
      // Motor vorwärts bewegen
      setMotorLockCommand(MOTOR_DIRECT_DIR_UNLOCK);
    }
    else if (gstrKomEinBef == "PAL")
    {
      // Parameter lesen
      gstrKomAus += gstrKomEinDat + " " + EEPROM_ParLesen(gstrKomEinDat.substring(0,3), gstrKomEinDat.substring(4).toInt());
    }
    else if (gstrKomEinBef == "PAS")
    {
      // Parameter schreiben
      int iAdr =  gstrKomEinDat.substring(4, gstrKomEinDat.lastIndexOf(" ")).toInt();
      int iWert = gstrKomEinDat.substring(gstrKomEinDat.lastIndexOf(" ") + 1).toInt();
      String strTyp = gstrKomEinDat.substring(0,3);
      
      gstrKomAus += EEPROM_ParSchreiben(strTyp, iAdr, iWert);

      // Parameter im Ablauf übernehmen
      if (strTyp == "16+")
      {
        switch (iAdr)
        {
          case 1:
            // Motor: Überwachungszeit Schließen
            MOT_ParZeiten(1, iWert);
            break;
          case 2:
            // Motor: Überwachungszeit Öffnen
            MOT_ParZeiten(2, iWert);
            break;
        }
      }
      else if (strTyp == "08+")
      {
        switch (iAdr)
        {
          case 5:
            // Motor: Rückfahrzeit
            MOT_ParZeiten(3, iWert);
            break;
          case 6:
            // mobiles Internet APN Einstellung
            GPRS_APN(iWert);
            break;          
        }
      }
    }
    else if (gstrKomEinBef == "EZL")
    {
      // EEPROM-Zeile (8-Bytes) lesen
      gstrKomAus += EEPROM_ZeileLesen(gstrKomEinDat.toInt());
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

  // "Digitaler Hausmeister"
  lboDHM = (digitalRead(DE_DHM) == HIGH);

  if (lboDHM)
  {
      // Zähler erhöhen
      if (liZtDHM < 11)
      {   
        liZtDHM++;
      }
  }
  else
  {
      // Schloss entsperren, wenn die Zeit des Signals im Bereich 400 bis 600 ms liegt
      if ((liZtDHM >= 2) && (liZtDHM <= 10))
      {
        // Entsperren
        OkLedSet(LED_TAG_OK);
        ErrorLedSet(LED_TAG_OK);
        //add check with getMotorLockState();!!!
        setMotorLockCommand(UNLOCKING);
        // letzten registrierten Benutzerzugriff im EEPROM sichern
        EEPROM_LetzterZugriff('B', 0, NULL);
        // Logeintrag: Hausmeistersignal erkannt, entsperren
        LOG_Eintrag("Digitaler Hausmeister: entsperren aktiviert");        
      }
      liZtDHM = 0;
  }

  #ifdef EN_OUTPUT_TASKTEST_SIGNALS
  //delay(1);                              //emulation of task processing time; should be replaced with program code
  digitalWrite(OUT_Task1, LOW);
  #endif

}

void Task2(){//configured with 250ms interval (inside ArduSched.h)
  //+++++++++++++++++++++++++++++++++++
  //static uint8_t dummyID_Article[][7] =  {{0xDF, 0x53, 0xA6, 0x59},{0xE0, 0x48, 0x9A, 0x1B},{0x45, 0x95, 0xA5, 0x59},{0xD0, 0xE7, 0x08, 0x1B},{0xB7, 0xD9, 0xA4, 0xA9},{0xD0, 0xB7, 0x7B, 0x1B},{0xD0, 0x9C, 0xAF, 0x1B}};
  //static uint8_t dummyID_User[][7] =     {{0xF5, 0xD1, 0xCE, 0xB0},{0x75, 0x12, 0x47, 0xBE},{0xC6, 0xB7, 0xBC, 0xBB},{0x45, 0x54, 0xBF, 0xB0},{0xF5, 0xFB, 0x52, 0xBE},{0x9B, 0x87, 0x83, 0xB9},{0x06, 0x3E, 0xB3, 0xBB}};
  
  static uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  static uint8_t uid2[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  static uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  static uint8_t uidLength2;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  static unsigned char rub_aktiveRfidReader = USER_AND_ARTICLE_RFID;
  static unsigned char rub_TaskDelayUserRFID = 0;
  static unsigned char rub_TaskDelayArticleRFID = 1;
  boolean lbo_DetectRFID_Chip;
  boolean lbo_DetectRFID_Chip2;
  static boolean rbo_RFID_ChipRemoved = true;
  static boolean rbo_RFID_ChipRemoved2 = true;
  unsigned char lub_RFID_UserIdValid;
  unsigned char lub_RFID_ArticleIdValid;
  unsigned char lub_RFID_MasterIdValid;
  static boolean rboTeachUserIfNotExists = false;
  static boolean rboTeachArticleIfNotExists = false;

  static unsigned char lubMotStatusAlt = UNDEFINED;
  unsigned char lubMotStatus;

  static int riFrgL1 = 0;     // Freigabezähler Leser 1
  static int riFrgL2 = 0;     // Freigabezähler Leser 2
  
  //+++++++++++++++++++++++++++++++++++
#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  digitalWrite(OUT_Task2, HIGH);
#endif
  //insert code or function to call here:
  //+++++++++++++++++++++++++++++++++++
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  if(rub_TaskDelayUserRFID){
    rub_TaskDelayUserRFID--;  
  }
  else{
    rub_TaskDelayUserRFID = 1;
    if(rub_aktiveRfidReader & USER_RFID){
#ifdef STRONGLINK            // define, if StrongLink Reader is used
      lbo_DetectRFID_Chip = SL030readPassiveTargetID(&uid[0], &uidLength, 50);
#else
      lbo_DetectRFID_Chip = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 50);
#endif
      if(lbo_DetectRFID_Chip){
        // Freigabezeit, wenn nötig, neu starten
        if (riFrgL1)
        {
          riFrgL1 = ZT_FRGLG1;
        }
        
        if(rbo_RFID_ChipRemoved == true){
          rbo_RFID_ChipRemoved = false;
          Beeper(BEEP_DETECT_TAG);
          OkLedSet(LED_TAG_CHECK);
          ErrorLedSet(LED_TAG_CHECK);
#ifdef SERIAL_DEBUG_ENABLE
          Serial.print("\n\rID:");
          for (uint8_t i=0; i < uidLength; i++){
            if(uid[i] <= 0x0F){
              Serial.print(" 0x0");Serial.print(uid[i], HEX);              
            }
            else{
              Serial.print(" 0x");Serial.print(uid[i], HEX); 
            }
            Serial.print(",");            
          }
          Serial.print("\n\r");
#endif   
          if( (rboTeachUserIfNotExists == false) && (rboTeachArticleIfNotExists == false) ){
            lub_RFID_UserIdValid = checkUserID(uidLength, &uid[0]);
            if(lub_RFID_UserIdValid){
              OkLedSet(LED_TAG_OK);
              ErrorLedSet(LED_TAG_OK);
              //add check with getMotorLockState();!!!
              setMotorLockCommand(UNLOCKING);
              // letzten registrierten Benutzerzugriff im EEPROM sichern
              EEPROM_LetzterZugriff('B', uidLength, uid);
              // Logeintrag: gültiger Benutzer erkannt, entsperren
              LOG_Eintrag("Benutzeranmeldung: entsperren aktiviert (" + ID_Konvertierung(uidLength, uid) + ")");
              // Freigabezähler setzen, um ein erneutes Auslesen vorerst zu deaktivieren
              riFrgL1 = ZT_FRGLG1;
            }
            else{//if user not found in EEPROM, check if it is a master ID
              lub_RFID_MasterIdValid = checkMasterID(uidLength, &uid[0]);
              switch(lub_RFID_MasterIdValid){
                case TEACH_USER_ID_MASTER:
                  Beeper(BEEP_TEACH_USER);
                  OkLedSet(LED_TEACH_USER);
                  ErrorLedSet(LED_TEACH_USER);
                  rboTeachUserIfNotExists = true;

                  // Logeintrag: neue Benutzer einlernen
                  LOG_Eintrag("Benutzeranmeldung: Benutzer einlernen (" + ID_Konvertierung(uidLength, uid) + ")");
                break;

                case TEACH_ARTICLE_ID_MASTER:
                  Beeper(BEEP_TEACH_ARTICLE);
                  OkLedSet(LED_TEACH_ARTICLE);
                  ErrorLedSet(LED_TEACH_ARTICLE);
                  rboTeachArticleIfNotExists = true;

                  // Logeintrag: neue Artikel einlernen
                  LOG_Eintrag("Benutzeranmeldung: Artikel einlernen (" + ID_Konvertierung(uidLength, uid) + ")");
                break;

                case ERASE_IDS_MASTER:
                  // Logeintrag: Löschvorgang ausgelöst
                  LOG_Eintrag("Benutzeranmeldung: Benutzer und Artikel entfernen (" + ID_Konvertierung(uidLength, uid) + ")");

                  // löschen der Benutzer und Artikel durchführen
                  digitalWrite(OUT_ERROR_LED, HIGH);
                  eraseEEPROM_AndReWriteHeader();//block the program execution for approx 12s during EEPROM erase!!
                  Beeper(BEEP_ERASE_IDS_END);
                  OkLedSet(LED_ERASE_IDS_END);
                  ErrorLedSet(LED_ERASE_IDS_END);
                break;

                case NO_MASTER:
                  Beeper(BEEP_UNKNOWN_ID);
                  OkLedSet(LED_TAG_NOK);
                  ErrorLedSet(LED_TAG_NOK);

                  // Logeintrag: ungültige Nummer erkannt
                  LOG_Eintrag("Benutzeranmeldung: unbekannt (" + ID_Konvertierung(uidLength, uid) + ")");

                  // Benutzerdatei erneut vom Server auslesen
                  GPRS_DateiLesen('B');
                break;

                default:
                  //should not be reached
                break;
              }
            }
          }
          else{//teach user or article is active
            lub_RFID_MasterIdValid = checkMasterID(uidLength, &uid[0]);//check if teach mode end is wished
            if(lub_RFID_MasterIdValid == TEACH_USER_ID_MASTER){
              if(rboTeachUserIfNotExists == true){
                Beeper(BEEP_TEACH_USER_END);
                OkLedSet(LED_TEACH_USER_END);
                ErrorLedSet(LED_TEACH_USER_END);
                rboTeachUserIfNotExists = false;

                // Logeintrag: Lernvorgang beendet
                LOG_Eintrag("Benutzeranmeldung: Benutzer Lernvorgang beendet (" + ID_Konvertierung(uidLength, uid) + ")");
              }     
            }
            else if(lub_RFID_MasterIdValid == TEACH_ARTICLE_ID_MASTER){
              if(rboTeachArticleIfNotExists == true){
                Beeper(BEEP_TEACH_ARTICLE_END);
                OkLedSet(LED_TEACH_ARTICLE_END);
                ErrorLedSet(LED_TEACH_ARTICLE_END);
                rboTeachArticleIfNotExists = false; 

                // Logeintrag: Lernvorgang beendet
                LOG_Eintrag("Benutzeranmeldung: Artikel Lernvorgang beendet (" + ID_Konvertierung(uidLength, uid) + ")");                
              }  
            }
            else if(lub_RFID_MasterIdValid == ERASE_IDS_MASTER){
              //do nothing! No erase during teach mode is wished! 
            }
            else{//teach in new ID
              if(rboTeachUserIfNotExists == true){
                writeEEPROM_UserID(uidLength, &uid[0]);

                // Logeintrag: neuer Benutzer angelegt
                LOG_Eintrag("Benutzeranmeldung: neuer Benutzer (" + ID_Konvertierung(uidLength, uid) + ")");
              }
              else if(rboTeachArticleIfNotExists == true){
                writeEEPROM_ArticleID(uidLength, &uid[0]);   

                // Logeintrag: neuer Artikel angelegt
                LOG_Eintrag("Benutzeranmeldung: neuer Artikel (" + ID_Konvertierung(uidLength, uid) + ")");                
              }
            }
          }
        }
      }
      else{
        // Verzögerungszeit um ein erneutes versehentliches Entsperren zu verhindern
        if (riFrgL1)
        {
          riFrgL1--;
        }
        if (!riFrgL1)
        {
          rbo_RFID_ChipRemoved = true;  
        }
      }
    }
  }
//###########################################################################################################################
  if(rub_TaskDelayArticleRFID){
    rub_TaskDelayArticleRFID--;  
  }
  else{
    rub_TaskDelayArticleRFID = 1;
    if(rub_aktiveRfidReader & USER_RFID){   // !!! JoWu, why is macro USER_RFID used?
#ifdef STRONGLINK            // define, if StrongLink Reader is used
      lbo_DetectRFID_Chip2 = SL032readPassiveTargetID(&uid2[0], &uidLength2, 50);
#else
      lbo_DetectRFID_Chip2 = nfc2.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid2[0], &uidLength2, 50);
#endif      
      if(lbo_DetectRFID_Chip2){
        // Freigabezeit, wenn nötig, neu starten
        if (riFrgL2)
        {
          riFrgL2 = ZT_FRGLG2;
        }
                
        if(rbo_RFID_ChipRemoved2 == true){
          rbo_RFID_ChipRemoved2 = false;
          Beeper(BEEP_DETECT_TAG);
          OkLedSet(LED_TAG_CHECK);
          ErrorLedSet(LED_TAG_CHECK);
#ifdef SERIAL_DEBUG_ENABLE
          Serial.print("\n\rID:");
          for (uint8_t i=0; i < uidLength2; i++){
            if(uid2[i] <= 0x0F){
              Serial.print(" 0x0");Serial.print(uid2[i], HEX);              
            }
            else{
              Serial.print(" 0x");Serial.print(uid2[i], HEX); 
            }
            Serial.print(",");            
          }
          Serial.print("\n\r");
#endif   
          if( (rboTeachUserIfNotExists == false) && (rboTeachArticleIfNotExists == false) ){
            lub_RFID_ArticleIdValid = checkArticleID(uidLength2, &uid2[0]);
            if(lub_RFID_ArticleIdValid){
              OkLedSet(LED_TAG_OK);
              ErrorLedSet(LED_TAG_OK);
              //add check with getMotorLockState();!!!
              setMotorLockCommand(LOCKING);
              // letzten registrierten Artikelzugriff im EEPROM sichern
              EEPROM_LetzterZugriff('A', uidLength2, uid2);
              // Logeintrag: gültiger Artikel erkannt, sperren aktiviert
              LOG_Eintrag("Artikelanmeldung: sperren aktiviert (" + ID_Konvertierung(uidLength2, uid2) + ")");
              // Freigabezähler setzen, um ein erneutes Auslesen vorerst zu deaktivieren
              riFrgL2 = ZT_FRGLG2;
            }
            else{//if user not found in EEPROM, check if it is a master ID
              lub_RFID_MasterIdValid = checkMasterID(uidLength2, &uid2[0]);
              switch(lub_RFID_MasterIdValid){
                case TEACH_USER_ID_MASTER:
                  Beeper(BEEP_TEACH_USER);
                  OkLedSet(LED_TEACH_USER);
                  ErrorLedSet(LED_TEACH_USER);
                  rboTeachUserIfNotExists = true;  

                  // Logeintrag: neue Benutzer einlernen
                  LOG_Eintrag("Artikelanmeldung: Benutzer einlernen (" + ID_Konvertierung(uidLength2, uid2) + ")");                  
                break;

                case TEACH_ARTICLE_ID_MASTER:
                  Beeper(BEEP_TEACH_ARTICLE);
                  OkLedSet(LED_TEACH_ARTICLE);
                  ErrorLedSet(LED_TEACH_ARTICLE);
                  rboTeachArticleIfNotExists = true;

                  // Logeintrag: neue Artikel einlernen
                  LOG_Eintrag("Artikelanmeldung: Artikel einlernen (" + ID_Konvertierung(uidLength2, uid2) + ")");                  
                break;

                case ERASE_IDS_MASTER:
                  // Logeintrag: Löschvorgang ausgelöst
                  LOG_Eintrag("Artikelanmeldung: Benutzer und Artikel entfernen (" + ID_Konvertierung(uidLength2, uid2) + ")");

                  // Löschvorgang
                  digitalWrite(OUT_ERROR_LED, HIGH);
                  eraseEEPROM_AndReWriteHeader();//block the program execution for approx 12s during EEPROM erase!!
                  Beeper(BEEP_ERASE_IDS_END);
                  OkLedSet(LED_ERASE_IDS_END);
                  ErrorLedSet(LED_ERASE_IDS_END);
                break;

                case NO_MASTER:
                  Beeper(BEEP_UNKNOWN_ID);
                  OkLedSet(LED_TAG_NOK);
                  ErrorLedSet(LED_TAG_NOK);  

                  // Logeintrag: ungültige Nummer erkannt
                  LOG_Eintrag("Artikelanmeldung: unbekannt (" + ID_Konvertierung(uidLength2, uid2) + ")");

                  // Artikeldatei erneut vom Server auslesen
                  GPRS_DateiLesen('A');
                break;

                default:
                  //should not be reached
                break;
              }
            }
          }
          else{//teach user or article is active
            lub_RFID_MasterIdValid = checkMasterID(uidLength2, &uid2[0]);//check if teach mode end is wished
            if(lub_RFID_MasterIdValid == TEACH_USER_ID_MASTER){
              if(rboTeachUserIfNotExists == true){
                Beeper(BEEP_TEACH_USER_END);
                OkLedSet(LED_TEACH_USER_END);
                ErrorLedSet(LED_TEACH_USER_END);
                rboTeachUserIfNotExists = false; 

                // Logeintrag: Lernvorgang beendet
                LOG_Eintrag("Artikelanmeldung: Benutzer Lernvorgang beendet (" + ID_Konvertierung(uidLength2, uid2) + ")");                
              }     
            }
            else if(lub_RFID_MasterIdValid == TEACH_ARTICLE_ID_MASTER){
              if(rboTeachArticleIfNotExists == true){
                Beeper(BEEP_TEACH_ARTICLE_END);
                OkLedSet(LED_TEACH_ARTICLE_END);
                ErrorLedSet(LED_TEACH_ARTICLE_END);
                rboTeachArticleIfNotExists = false; 

                // Logeintrag: Lernvorgang beendet
                LOG_Eintrag("Artikelanmeldung: Artikel Lernvorgang beendet (" + ID_Konvertierung(uidLength2, uid2) + ")");                
              }  
            }
            else if(lub_RFID_MasterIdValid == ERASE_IDS_MASTER){
              //do nothing! No erase during teach mode is wished! 
            }
            else{//teach in new ID
              if(rboTeachUserIfNotExists == true){
                writeEEPROM_UserID(uidLength2, &uid2[0]);   

                // Logeintrag: neuer Benutzer angelegt
                LOG_Eintrag("Artikelanmeldung: neuer Benutzer (" + ID_Konvertierung(uidLength2, uid2) + ")");
              }
              else if(rboTeachArticleIfNotExists == true){
                writeEEPROM_ArticleID(uidLength2, &uid2[0]);

                // Logeintrag: neuer Artikel angelegt
                LOG_Eintrag("Artikelanmeldung: neuer Artikel (" + ID_Konvertierung(uidLength2, uid2) + ")");                       
              }
            }
          }
        }
      }
      else{
        // Verzögerungszeit um ein erneutes versehentliches Entsperren zu verhindern
        if (riFrgL2)
        {
          riFrgL2--;
        }
        if (!riFrgL2)
        {
          rbo_RFID_ChipRemoved2 = true;  
        }
      }
    }
  }
  //+++++++++++++++++++++++++++++++++++

  // Schlosszustand prüfen
  lubMotStatus = getMotorLockState();

  // Logeintrag generieren und letzten Zustand im EEPROM sichern
  if (lubMotStatus != lubMotStatusAlt)
  {
    lubMotStatusAlt = lubMotStatus;     
    EEPROM_Schlossstatus(lubMotStatus);
    LOG_Eintrag("Schlosszustand: " + MOT_ZustandZK(lubMotStatus));

    // Prüfung auf Artikelverlust nach dem Schließen
    if (MOT_Geschlossen() && rbo_RFID_ChipRemoved2)
    {
      // Das Schloss ist zu, der Artikel aber nicht eingelegt
      EEPROM_LetzterZugriff('A', 0, NULL);
      LOG_Eintrag("Fehler: Artikel nicht eingelegt");
    }    
  }

#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  //delay(1);                              //emulation of task processing time; should be replaced with program code
  digitalWrite(OUT_Task2, LOW);
#endif
}

void Task3(){//configured with 1000ms interval (inside ArduSched.h)
#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  digitalWrite(OUT_Task3, HIGH); 
#endif
  //insert code or function to call here:
  digitalWrite(OUT_LED, digitalRead(OUT_LED) ^ 1);

  //beeper demo..................
  //Beeper(BEEP_DETECT_TAG);
  //Beeper(BEEP_UNKNOWN_ID);
  //Beeper(BEEP_TEACH_USER);
  //Beeper(BEEP_TEACH_USER_END);
  //Beeper(BEEP_TEACH_ARTICLE);
  //Beeper(BEEP_TEACH_ARTICLE_END);
  //Beeper(BEEP_ERASE_IDS);
  //Beeper(BEEP_ERASE_IDS_END);
  //.............................
  
  //LED demo..................
  //OkLedSet(LED_CONST_OFF);
  //ErrorLedSet(LED_CONST_OFF);
  //OkLedSet(LED_CONST_ON);
  //ErrorLedSet(LED_CONST_ON);
  //OkLedSet(LED_TAG_CHECK);
  //ErrorLedSet(LED_TAG_CHECK);
  //OkLedSet(LED_TAG_OK);
  //ErrorLedSet(LED_TAG_OK);
  //OkLedSet(LED_TAG_NOK);
  //ErrorLedSet(LED_TAG_NOK);
  //OkLedSet(LED_TEACH_USER);
  //ErrorLedSet(LED_TEACH_USER);
  //OkLedSet(LED_TEACH_USER_END);
  //ErrorLedSet(LED_TEACH_USER_END);
  //OkLedSet(LED_TEACH_ARTICLE);
  //ErrorLedSet(LED_TEACH_ARTICLE);
  //OkLedSet(LED_TEACH_ARTICLE_END);
  //ErrorLedSet(LED_TEACH_ARTICLE_END);
  //OkLedSet(LED_ERASE_IDS);
  //ErrorLedSet(LED_ERASE_IDS);
  //OkLedSet(LED_ERASE_IDS_END);
  //ErrorLedSet(LED_ERASE_IDS_END);
  //OkLedSet(LED_SYSTEM_ERROR);
  //ErrorLedSet(LED_SYSTEM_ERROR);
  //.............................

#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  //delay(1);                              //emulation of task processing time; should be replaced with program code
  digitalWrite(OUT_Task3, LOW);
#endif
}
/*void Task4(){
  //insert code or function to call here:
}
void Task5(){
  //insert code or function to call here:
}
void Task6(){
  //insert code or function to call here:
}
void Task7(){
  //insert code or function to call here:
}
void Task8(){
  //insert code or function to call here:
}
void Task9(){
  //insert code or function to call here:
}*/

void Tmr3_ISR(){
#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  //digitalWrite(OUT_TMR3_TIMING_SIG, digitalRead(OUT_TMR3_TIMING_SIG) ^ 1); 
  digitalWrite(OUT_TMR3_TIMING_SIG, HIGH);
#endif

  //insert code or function to call here:
  MotorLockHandler();//called all 50 or 100ms (depending on beeper frequency -> SWITCH2)
  
#ifdef EN_OUTPUT_TASKTEST_SIGNALS  
  //delayMicroseconds(1000);
  digitalWrite(OUT_TMR3_TIMING_SIG, LOW);
#endif
}

// Feld mit der Identifikationsnummer in eine Zeichenkette umwandeln
// Parameter:
// uiL = Länge (4 oder 7 Bytes)
// uiID = Zeiger auf das Datenfeld
// Rückgabewert:
// Identifikationsnummer als Zeichenkette im hexadezimalen Format
String ID_Konvertierung(uint8_t uiL, uint8_t* uiID)
{
  String strID;

  for (int i = 0; i < uiL; i++)
  {
    if (i > 0)
    {
      strID += " ";
    }
    if (uiID[i] < 16)
    {
      strID += "0";
    }
    strID += String(uiID[i], HEX);
  }

  return strID;
}

//
//  RFID Reader StrongLink
//
//
boolean SL030readPassiveTargetID(uint8_t* puid, uint8_t* uidLength, uint8_t u8MaxLen)
{
  unsigned char u8Len;
  unsigned char u8ProtNr;
  unsigned char u8Status;
  *uidLength = 0;  
  // Select Mifare card  
#ifdef SERIAL_DEBUG_ENABLE
  u8Len = 0;
  if(Wire.available()){
    u8Len = 1;
    Serial.print("Clear I2C ");
  }
#endif
  while(Wire.available()){
#ifdef SERIAL_DEBUG_ENABLE
    Serial.print(Wire.read(), HEX);
    Serial.print(" ");
#else
    Wire.read();
#endif
  }
#ifdef SERIAL_DEBUG_ENABLE
  if (u8Len){
    Serial.println();
  }
#endif
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("I2C Transmit");
#endif
  Wire.beginTransmission(SL030ADR/2); // transmit to device #SL030ADR
  Wire.write(0x01);      // len
  Wire.write(0x01);      // cmd Select Mifare Card
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println(Wire.endTransmission(true));     // stop transmitting
#else
  Wire.endTransmission(true);     // stop transmitting
#endif
  delay(30);             // < 50 ms seams to be critical
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("I2C Read");
#endif
  u8Len = Wire.requestFrom(SL030ADR/2, 10, true);    // request 10 byte from slave device #SL030ADR, which is the max length of the protocol
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println(u8Len);
#endif

#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("I2C available: ");
  Serial.println(Wire.available());
#endif

  u8Len = Wire.read();
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("Len: ");
  Serial.println(u8Len);
//  Serial.print(" ");
#endif  
  if (u8Len == 7)
  {
      Wire.read();
      Wire.read();
      puid[0] = Wire.read();
      puid[1] = Wire.read();
      puid[2] = Wire.read();
      puid[3] = Wire.read();
      puid[4] = 0;
      puid[5] = 0;
      puid[6] = 0;
      while(Wire.available()) Wire.read();
      *uidLength = 4;
      return true;           
  }
  else if(u8Len == 10)
  {
      Wire.read();
      Wire.read();
      puid[0] = Wire.read();
      puid[1] = Wire.read();
      puid[2] = Wire.read();
      puid[3] = Wire.read();
      puid[4] = Wire.read();
      puid[5] = Wire.read();
      puid[6] = Wire.read();
      while(Wire.available()) Wire.read();
      *uidLength = 7;  
      return true;    
  }
  else{
    puid[0] = 0;
    puid[1] = 0;
    puid[2] = 0;
    puid[3] = 0;
    puid[4] = 0;
    puid[5] = 0;
    puid[6] = 0;
    while(Wire.available()) Wire.read();
    *uidLength = 0;  
    return false;
  }

}
////////////////////////////////////////////////////////////
//Send command to SL032
////////////////////////////////////////////////////////////
void SL032_SendCom(unsigned char *g_cCommand);                                           

unsigned char const SelectCard[3] =      {0xBA,0x02,0x01 };       
void SL032_SendCom(unsigned char *g_cCommand)
{    
     unsigned char i,chkdata,sendleg;
#ifdef SERIAL_DEBUG_ENABLE
      Serial.print("SL032_SendCom: ");
#endif
     sendleg = *(g_cCommand+1) + 1;
     
     chkdata = 0;
     for(i=0; i<sendleg; i++)
     {    
         chkdata ^= *(g_cCommand+i);
      Serial2.print((char)*(g_cCommand+i));
#ifdef SERIAL_DEBUG_ENABLE
      Serial.print(*(g_cCommand+i), HEX);
      Serial.print(" ");
#endif      
     }
     
      Serial2.print((char)chkdata);
#ifdef SERIAL_DEBUG_ENABLE
      Serial.println(chkdata, HEX);
#endif      
}

// return len of UID, 4 or 7
uint8_t SL032_ReadUid(uint8_t* puid){
unsigned char u8Len;
unsigned char u8ProtNr;
unsigned char u8Status;

      puid[0] = 0;
      puid[1] = 0;
      puid[2] = 0;
      puid[3] = 0;
      puid[4] = 0;
      puid[5] = 0;
      puid[6] = 0;
  while(Serial2.available() == 0);    // !!!JoWu: Dead End, if no response
  
  // check for 0xBD protocol
  if (Serial2.read() == 0xBD){
    while(Serial2.available() == 0);
    // read len
    u8Len = Serial2.read();
    while(Serial2.available() != u8Len);
    u8ProtNr = Serial2.read();
    u8Status = Serial2.read();
    if (u8Len == 8)
    {
      puid[0] = Serial2.read();
      puid[1] = Serial2.read();
      puid[2] = Serial2.read();
      puid[3] = Serial2.read();
      puid[4] = 0;
      puid[5] = 0;
      puid[6] = 0;
      return 4;
    }
    else if (u8Len == 11)
    {
      puid[0] = Serial2.read();
      puid[1] = Serial2.read();
      puid[2] = Serial2.read();
      puid[3] = Serial2.read();
      puid[4] = Serial2.read();
      puid[5] = Serial2.read();
      puid[6] = Serial2.read();
      return 7;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

boolean SL032readPassiveTargetID(uint8_t* puid, uint8_t* uidLength, uint8_t u8MaxLen)
{
  unsigned char i;
  // check, if something is in serial buffer, if yes, then read/clear it
  i=0;
#ifdef SERIAL_DEBUG_ENABLE
 if (Serial2.available())
 {
  i=1;
  Serial.print("S2Buff:");
 }
#endif
while (Serial2.available())
  {
#ifdef SERIAL_DEBUG_ENABLE
    Serial.print(Serial2.read(), HEX);
    Serial.print(" ");
#else
    Serial2.read();
#endif
  }
#ifdef SERIAL_DEBUG_ENABLE
    if (i>0)
    {
      Serial.println();
    }
#endif  
  SL032_SendCom(SelectCard);                             //
  Serial2.flush();
  *uidLength = SL032_ReadUid(puid);
#ifdef SERIAL_DEBUG_ENABLE
  Serial.print("UIDlen:");
  Serial.print(*uidLength);
  Serial.print(" ");
  for (i=0; i<*uidLength; i++)
  {
    Serial.print(puid[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
#endif

  if (*uidLength > 0){
    return true;
  }
  else{
    return false;
  }  
}






