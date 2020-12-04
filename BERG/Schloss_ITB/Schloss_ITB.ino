/*
 Name:    Schloss_ITB.ino
 Datum:   16.04.2018
 Autor:   (c) 2018 to 2020 Maximilian Johannes Wurmtzer / Markus Emanuel Wurmitzer / Edmund Titz (Applikation V 0)

  Ser-# defined:
    yyVvvvSsss
    20V114S060

    yy ... Year 10s and 1s
    V  ... stands for Version
    vvv .. actual Hardware-Version
    S  ... stands for Serial-Number
    sss .. actual Serial-Number of the device
    
  Deliveries
  2020-11-14  20V120S001 - 20V120S030
    - ITBs delivered to Welando

  Versionsgeschichte:
  2020-XX-XX  V120    JoWu - planned
    Bugs Open:
    - Imp-Report; 2020-09-06; JoWu; OPEN; info messages with prefix #INF
    - Issue-Report; 2020-09-02; JoWu; OPEN; actual workaround - fix the problem of V118pre0 crash with SoundAndLedHandler() in Task3() by moving it back to Task1() -> open issue
    - Bug-Report; 2020-08-16; JoWu; OPEN; programming new users and articels via RF-ID tags using same RF-ID tags leads to multiple entries of same IDs
    
    2020-11-21  V119  JoWu
      - Bugfix release >256 User Entries

    2020-11-21  V118  JoWu
      - first final release of Keylock handling implemented

    2020-11-20  V118pre7    JoWu
      - implemented heartbeat
      - implemented cyclic update of user and article rfids
      - GPRS.cpp V116
          - improvements in FTP handling
          - new FTP download concept with blockwise reading
          - rename file after reading
        Der Sketch verwendet 56714 Bytes (22%) des Programmspeicherplatzes. Das Maximum sind 253952 Bytes.
        Globale Variablen verwenden 6495 Bytes (79%) des dynamischen Speichers, 1697 Bytes für lokale Variablen verbleiben. Das Maximum sind 8192 Bytes.

    2020-11-15  V118pre6    JoWu
      - add Support for HW ITB V1.2
        - #define POLA_SD_RTC_PWR     42        // SD/RTC Power
        - #define POLA_GSM_PWR        43        // GSM Power
        - #define PO_GSM_RST          44        // GSM Reset

    2020-11-09  V118pre6    JoWu
      - GPRS.cpp  V110  JoWu
        - change FTP user to WelAccessClient
        - change Filename of textfile
        - move FTP-Setup to defines
        
        Der Sketch verwendet 53630 Bytes (21%) des Programmspeicherplatzes. Das Maximum sind 253952 Bytes.
        Globale Variablen verwenden 6255 Bytes (76%) des dynamischen Speichers, 1937 Bytes für lokale Variablen verbleiben. Das Maximum sind 8192 Bytes.


    2020-10-19  V118pre5    JoWu
      - first implementation of Key-Lock using KeyLock RF-ID reader
      - SL018 or SL030 as first reader für User-ID
      - SL030 with J2 closed as second reader Key-ID
      
    2020-09-21  V118pre4    JoWu
      - GPRS optimization

    2020-09-02  V118pre3    JoWu
      - show SD-Card Init success with LED blink code
      - Uhrzeit-Module minor changes
      - SoundAndLed-Module instant on/off if constant
      - Log-Module info about success on init
      - GPRS-Module Log-Buffer String object to char array change for stability reason

    2020-09-02  V118pre2    JoWu
      - reintegrated GPRS-Handling
      - included T-Mobile M2M SIM-Card

    2020-08-22  V118pre1    JoWu
      - fix the problem of V118pre0 crash with SoundAndLedHandler() in Task3() by moving it back to Task1() -> open issue
      - optimized SoundAndLedHandler() - Timer1 handling

      Der Sketch verwendet 52468 Bytes (20%) des Programmspeicherplatzes. Das Maximum sind 253952 Bytes.
      Globale Variablen verwenden 5386 Bytes (65%) des dynamischen Speichers, 2806 Bytes für lokale Variablen verbleiben. Das Maximum sind 8192 Bytes.

    2020-08-22  V118pre0    JoWu
      - speedup boot and introduce macro DELAY_POWERUP
      - removed the String() for version
      - removed the String() gstrKomEin
      - replaced position of LOG_Init to enable logs more early
      - moved SoundAndLedHandler() and DHM handling from Task1() to Task3() and speedup Task3() vom 1000 ms to 100 ms to enable this functions
      - speedup Task1() from 100 ms to 25 ms to enable faster communication with PC and GSM
      
      Der Sketch verwendet 52464 Bytes (20%) des Programmspeicherplatzes. Das Maximum sind 253952 Bytes.
      Globale Variablen verwenden 5384 Bytes (65%) des dynamischen Speichers, 2808 Bytes für lokale Variablen verbleiben. Das Maximum sind 8192 Bytes.

    2020-08-16  V117     JoWu
      - Bug-Report; 2020-08-16; JoWu; CLOSED; after programming new users + articels via RF-ID tags leads to crazy readouts of log files
      - .ino minor cosmetic improvements
      Der Sketch verwendet 52432 Bytes (20%) des Programmspeicherplatzes. Das Maximum sind 253952 Bytes.
      Globale Variablen verwenden 5392 Bytes (65%) des dynamischen Speichers, 2800 Bytes für lokale Variablen verbleiben. Das Maximum sind 8192 Bytes.
  
    2020-08-16  V117pre1 JoWu
      - optimize serial communication with "Schlossmeister.exe"

    2020-08-16  V117pre0 JoWu
      Ser-# defined:
        yyVvvvSsss
        20V112S052
    
    - Bug-Report; 2020-08-16; JoWu; OPEN; after programming new users + articels via RF-ID tags leads to crazy readouts of log files
    - Bug-Report; 2020-08-16; JoWu; OPEN; programming new users and articels via RF-ID tags using same RF-ID tags leads to multiple entries of same IDs

    2020-08-16  V116    JoWu
      - add define USE_SL030_OUT, to unuse the OUT pin of the reader because of firmware update there and use of SL018
  
    2020-05-24  V116pre JoWu
    - add selecction of debug output for freeRam();
    - add debug output of User-RFID
    
    2020-05-21  V116pre JoWu
    - introduce void serialEvent3() for modem to get all incomming values, even if buffer for serial input, which ist 64-1 bytes is too small
    
    2020-05-18  V116pre JoWu
    - increase serial buffer (went wrong, so discarded and plan to replace it with another solution
      C:\Users\Johannes\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.2\cores\arduino\HardwareSerial.h
        change in two places
        from: #define SERIAL_TX_BUFFER_SIZE 64
        to: #define SERIAL_TX_BUFFER_SIZE 256  // JoWu, changed from 64

        Der Sketch verwendet 52430 Bytes (20%) des Programmspeicherplatzes. Das Maximum sind 253952 Bytes.
        Globale Variablen verwenden 7042 Bytes (85%) des dynamischen Speichers, 1150 Bytes für lokale Variablen verbleiben. Das Maximum sind 8192 Bytes.
        Wenig Arbeitsspeicher verfügbar, es können Stabilitätsprobleme auftreten.

        so back again
        Der Sketch verwendet 52466 Bytes (20%) des Programmspeicherplatzes. Das Maximum sind 253952 Bytes.
        Globale Variablen verwenden 5506 Bytes (67%) des dynamischen Speichers, 2686 Bytes für lokale Variablen verbleiben. Das Maximum sind 8192 Bytes.
        
  2020-04-28  V115    JoWu
    - add extra LOCK handling
      - output: PO_LOCK_UNLOCK ... to open the extra lock for the key
      - input:  PI_LOCK_STAT_LOCKED ... return of lock status
      - outpu:  PO_LOCK_LED_G ... to show lock status, blink = open, on = closed
    - change output ports for timing information of task and interrupt
    - change port pins for showing battery-charge status to extra pins, which was on RS-485 outputs for the first prototyp
      now we will need the RS-485, so moved to dedicated port pins
  
  2020-03-15  V114    JoWu
    - closed 2020-03-15; JoWu, 2019-07-23; JoWu; Serial2.available() Timout to be implemented
  
  2020-03-14  V113    JoWu
    - introduced SL030 V3.1, which needs to use PI_SL032_OUT,  because does not set the ID back to zero
    - disable "Digitaler Hausmeister" (DE_DHM)
    - add bike charger LED detection 

  2019-10-01  V112    JoWu
    - Integration Uhrzeit V2.00
    - Integration ArdSchedd V5.00
    - set DE_DHM from input 30 to 49, which ist not used now
  
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
  open; 2020-05-18; JoWu; read extra I2C reader for Lock Holder
  closed; 2020-03-15; JoWu, 2019-07-23; JoWu; Serial2.available() Timout to be implemented

*/
// lokale Konstanten
//#include <avr/pgmspace.h>
const /*PROGMEM*/ char lstrVER[] = "ITB1_119_D";       // Softwareversion

//
// Include for SL030 I2C
//
#include <SPI.h>

#include <Wire.h>       // I2C Library
//#include <SD.h>
#include "ArdSched.h" //configure timing inside the header file
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
#define USE_RFID_KEYLOCK            // define, if Keylock RFID Reader is used

#define USE_SL030_OUT               // define, if the SL030_OUT should be used, was needed because of some firmware Issue of this devices, used as User-Reader

//#define SERIAL_DEBUG_ENABLE
//#define SERIAL_DEBUG_FREE_RAM       // show free ram if defined
//#define PROTOCOL_DEBUG_FREE_RAM     // show free ram in protocols

//#define   ARDSCHED_TEST           // define to show task times

#define DELAY_POWERUP     10  // [ms] was 500 ms in the past

#define POLA_SD_RTC_PWR     42        // SD/RTC Power
#define POLA_GSM_PWR        43        // GSM Power
#define PO_GSM_RST          44        // GSM Reset

// LOCK for key holder
//#define PO_LOCK_UNLOCK       29     // LOCK unlock command
#define PO_LOCK_UNLOCK       33     // LOCK unlock command
#define PI_LOCK_STAT_LOCKED  38     // LOCK status locked 
//#define PO_LOCK_LED_G        47     // LOCK LED green
#define PO_LOCK_LED_G        29     // LOCK LED green

boolean gboolKeyLockOpenCmd;        // open keylock command
static byte gbyLockOpenTimer;       // [500 ms] Open-Timer Lock

// Battery charge status
#define PO_BS_LED_R         49      // Battery charge status LED red
#define PO_BS_LED_G         48      // Battery charge status LED green

// RS-485 defines
#define PO_RS485_TX_ON  25
#define PO_RS485_RX_ON  27
#define PO_RS485_TX     18
#define PO_RS485_RX     19

//
//RFID definitions:
//
#define STRONGLINK            // define, if StrongLink Reader are used
// Definition for SL030 I2C
#define SL030ADR 0xA0         // standard address for SL030
#define SL030ADR_KEY 0xA2     // address for SL030 for KeyReader
#define PI_SL030_OUT 24       // User-Reader Tag within detection range

#define NON_RFID              0
#define USER_RFID             1
#define ARTICLE_RFID          2
#define USER_AND_ARTICLE_RFID 3

#define OUT_TX_USER_READER    18
#define OUT_TX_ARTICLE_READER 16

#define OUT_TMR3_TIMING_SIG   12  // 39

#define OUT_SD                53      // SPI SS for SD-Card used as CS-signal

#define ZT_FRGLG1           2                   // Freigabe des Lesegeräts nachdem ca. X Sekunden nichts erkannt wurde
#define ZT_FRGLG2           2                   // Freigabe des Lesegeräts nachdem ca. X Sekunden nichts erkannt wurde

// serielle Kommunikation
#define ITB_COM_BUF_RX_SIZE 64                  // RX-Buffer size for communication with PC

char gbyKom[ITB_COM_BUF_RX_SIZE];               // Kommunikation Eingang, maximal 64 Byte, davon gibt es ein Start und Endezeichen!
int giKomIdx;                                   // Kommunikation Index im Eingangsfeld
bool gboKomEin;                                 // Kommunikationseingang erkannt
bool gboKomAus;                                 // Kommunikationsausgang vorhanden
String gstrKomEinBef;                           // Eingangsdaten "Befehl"
String gstrKomEinDat;                           // Eingangsdaten "Daten"
String gstrKomAus;                              // Ausgangsdaten

// digitlaler Hausmeister
#define DE_DHM    46  //ITB auf dummy 49, sonst 30                            // Digitaleingang des digitalen Hausmeisters

// KeyLock
#define KEY_OKAY_TIMEOUT  5
int rbyKeyOkay = KEY_OKAY_TIMEOUT;


// StrongLink RFID Reader
boolean SL030readPassiveTargetID(uint8_t u8SL030Adr, uint8_t* puid, uint8_t* uidLength, uint8_t u8MaxLen);
boolean SL032readPassiveTargetID(uint8_t uid[], uint8_t *uidLength, uint8_t u8MaxLen);


void Tmr3_ISR();

void(* resetFunc) (void) = 0; //declare reset function @ address 0
String ID_Konvertierung(uint8_t uiL, uint8_t* uiID);

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
void setup() {
  //...Power up delay (stable supply on all HW components)
//  delay(DELAY_POWERUP);
  //...
  //!!!!!!!!!!!!!!!check brownout setting and add watchdog!!!!!!!!!!!!!!!

  // Init serial communication  
  Serial.begin(115200);

/*
 // Test SD-Card - Init
  SD.end();
  if (SD.begin(SPI_HALF_SPEED, OUT_SD)){
#ifdef SERIAL_DEBUG_ENABLE
    Serial.println("SD; Init Okay");
#endif
  }
  else{
#ifdef SERIAL_DEBUG_ENABLE
    Serial.print("SD; Init Failed"); Serial.print(SD.error()); Serial.println();
#endif
  }
*/

  //
  // init Port Pins
  //
  
  // init LED Output for Bike Charger Signal
  pinMode (PO_RS485_TX_ON, OUTPUT);
  pinMode (PO_RS485_RX_ON, OUTPUT);
  pinMode (PO_RS485_TX, OUTPUT);
  pinMode (PO_RS485_RX, INPUT);
  digitalWrite(PO_RS485_RX_ON, HIGH);
  digitalWrite(PO_RS485_TX_ON, HIGH);
  digitalWrite(PO_RS485_TX, HIGH);  // red

  // init LED for Battery-Status
  pinMode (PO_BS_LED_R, OUTPUT);
  pinMode (PO_BS_LED_G, OUTPUT);
  digitalWrite(PO_BS_LED_R, HIGH);
  digitalWrite(PO_BS_LED_G, HIGH);

  // init LOCK
  pinMode (PO_LOCK_UNLOCK, OUTPUT);               // LOCK unlock command
  digitalWrite(PO_LOCK_UNLOCK, LOW);
  pinMode (PI_LOCK_STAT_LOCKED, INPUT);           // LOCK status locked 

  pinMode (PO_LOCK_LED_G, OUTPUT);                // LOCK LED green
  digitalWrite(PO_LOCK_LED_G, HIGH);

  // init GSM
  digitalWrite(PO_GSM_RST, LOW);
  pinMode(PO_GSM_RST, OUTPUT);
  digitalWrite(PO_GSM_RST, LOW);

  digitalWrite(POLA_GSM_PWR, HIGH);
  pinMode(POLA_GSM_PWR, OUTPUT);
  digitalWrite(POLA_GSM_PWR, HIGH);

  // init SD/RTC
  digitalWrite(POLA_SD_RTC_PWR, HIGH);
  pinMode(POLA_SD_RTC_PWR, OUTPUT);
  digitalWrite(POLA_SD_RTC_PWR, HIGH);
  // switch off SD/RTC for proper restart
  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);
  digitalWrite(20, LOW);
  digitalWrite(21, LOW);
  digitalWrite(POLA_SD_RTC_PWR, LOW);
  delay(500);
  digitalWrite(POLA_SD_RTC_PWR, HIGH);
  pinMode(20, INPUT);
  pinMode(21, INPUT);
  digitalWrite(20, HIGH);
  digitalWrite(21, HIGH);
  
  // Articel Reader 
  pinMode(OUT_TX_ARTICLE_READER, OUTPUT);
  pinMode(OUT_TX_USER_READER, OUTPUT);
  pinMode(OUT_TMR3_TIMING_SIG, OUTPUT);//debug timing
  pinMode(OUT_LED, OUTPUT);//LED on Arduino
  
  digitalWrite(OUT_TX_ARTICLE_READER, HIGH);
  digitalWrite(OUT_TX_USER_READER, HIGH);  
  digitalWrite(OUT_TMR3_TIMING_SIG, LOW);//debug timing
  digitalWrite(OUT_LED, HIGH);
  
  //...Power up delay (stable pin state on all HW components)
  delay(DELAY_POWERUP);
  //...
  digitalWrite(OUT_LED, LOW);

#ifdef SERIAL_DEBUG_ENABLE
  Serial.begin(115200);   // (9600);
  Serial.println("Urban sharing lock");
  Serial.print("Firmware Version: ");
  Serial.println(lstrVER);
#endif 
  // Sound and LED Handler init();
  SoundAndLedInit();
  ErrorLedSet(LED_CONST_OFF);
  OkLedSet(LED_CONST_ON);
  delay(100);
  OkLedSet(LED_CONST_OFF);
  delay(100);
  OkLedSet(LED_CONST_ON);
  delay(100);
  OkLedSet(LED_CONST_OFF);
  
  // Log initialisieren
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init Log");
#endif 
  if (LOG_Init(OUT_SD)){
    OkLedSet(LED_CONST_ON);
  }
  else{
    ErrorLedSet(LED_CONST_ON);
    delay(100);
    ErrorLedSet(LED_CONST_OFF);
    delay(100);
    ErrorLedSet(LED_CONST_ON);
    delay(100);
    ErrorLedSet(LED_CONST_OFF);
    delay(100);
    ErrorLedSet(LED_CONST_ON);
    OkLedSet(LED_CONST_OFF);
  }  
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
  
#ifdef SERIAL_DEBUG_FREE_RAM
  Serial.write("FreeRam: ");  Serial.println(freeRam());
#endif
  ArdSchedSetup();

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
  Wire.setClock(400000);
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
//  SoundAndLedInit();
  OkLedSet(LED_CONST_ON);
  ErrorLedSet(LED_CONST_OFF);
  checkIf_EEPROM_HeaderExistsElseWrite();//new boards will not have valid EEPROM header, so write a header into EEPROM
  delay(DELAY_POWERUP);

  // Uhr initialisieren
#ifdef SERIAL_DEBUG_ENABLE
  Serial.println("Init RTC");
#endif 
  UHR_Init();

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
  LOG_Eintrag("Bootvorgang: abgeschlossen (V" + String(lstrVER) + ")");
}
void serialEvent3(){
  GPRS_SerEin();
}
void loop() {
  // put your main code here, to run repeatedly:
  // serielle Eingangsdaten des Modems kontinuierlich abfragen
//  GPRS_SerEin();
  
  // Scheduler
  ArdSchedLoop();
  if (ArdSchedTaskRdyStart(TASK_1)){ Task1(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_2)){ Task2(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_3)){ Task3(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_4)){ Task4(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_5)){ Task5(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_6)){ Task6(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_7)){ Task7(); ArdSchedTaskStop(); }
  if (ArdSchedTaskRdyStart(TASK_8)){
    Task8(); ArdSchedTaskStop();
#ifdef SERIAL_DEBUG_FREE_RAM
    Serial.write("FreeRam: ");  Serial.println(freeRam());
#endif
  }
}


void Task1(){//configured with 25 ms (old: 100ms) interval (inside ArduSched.h)
  static byte lbyTaskCounter;

  if (lbyTaskCounter++ % 4 == 0){
    //insert code or function to call here:
    SoundAndLedHandler();
  
    // GPRS Zustandmaschine
    GPRS_Zustandsmaschine();
  }
  // serielle Kommunikation
  // auf Dateneingang warten
  while (Serial.available() > 0){
    // Eingangsbyte lesen
    
    gbyKom[giKomIdx] = Serial.read();
    // if we got '#' we should reset the frame, if this is not done yet
    if (gbyKom[giKomIdx] == '#' && gbyKom[0] != '#'){
      giKomIdx = 0;
      gbyKom[0] = '#';
    }
    // Protokollanfang prüfen
    if (gbyKom[0] == '#'){
      // Protokollende abfangen
      if (gbyKom[giKomIdx] == 13){
        gboKomEin = true;
      }
      
      // Index erhöhen
      giKomIdx++;   
  
      // ungültige Daten erhalten, Inhalt löschen
      if ((giKomIdx >= ITB_COM_BUF_RX_SIZE) && !gboKomEin){    
        giKomIdx = 0;
      }
    }
   }

  // Protokoll auswerten
  if (gboKomEin){
    // Zeichenkette ohne Startzeichen erstellen
    gstrKomEinBef = String(gbyKom).substring(1,4);
    gstrKomEinDat = String(gbyKom).substring(5, giKomIdx - 1);
    gstrKomAus = String("#") + gstrKomEinBef + ' ';

    gboKomAus = true;

    // Befehle abfragen
    if (gstrKomEinBef == "VER"){
      // Versionsnummer abfragen
#if 0     // just for some module power cycle tests
      // Test GSM Rest Pin
/*
      digitalWrite(PO_GSM_RST, HIGH);
      delay(500);
      digitalWrite(PO_GSM_RST, LOW);
*/
/*      
 *    digitalWrite(POLA_GSM_PWR, LOW);
      delay(500);
      digitalWrite(POLA_GSM_PWR, HIGH);
*/
/*
      pinMode(20, OUTPUT);
      pinMode(21, OUTPUT);
      digitalWrite(20, LOW);
      digitalWrite(21, LOW);
      SPI.end();
      pinMode(53, OUTPUT);
      pinMode(51, OUTPUT);
      pinMode(52, OUTPUT);      
      digitalWrite(53, LOW);
      digitalWrite(51, LOW);
      digitalWrite(52, LOW);
      
      digitalWrite(POLA_SD_RTC_PWR, LOW);               // switch off SD/RTC for test
      delay(1000);
      digitalWrite(POLA_SD_RTC_PWR, HIGH);
      pinMode(20, INPUT);
      pinMode(21, INPUT);
      digitalWrite(20, HIGH);
      digitalWrite(21, HIGH);
      Wire.begin();
      LOG_Init(OUT_SD);
*/
#endif
      gstrKomAus += lstrVER;
#ifdef PROTOCOL_DEBUG_FREE_RAM
      gstrKomAus += "FreeRam: ";
      gstrKomAus += freeRam();
#endif
    }
    else if (gstrKomEinBef == "SER"){
      // Seriennummer abfragen
      gstrKomAus += EEPROM_SNrLesen();
    }
    else if (gstrKomEinBef == "SES"){
      // Seriennummer schreiben
      gstrKomAus += EEPROM_SNrSchreiben(gstrKomEinDat);
      // Neue Seriennummer übernehmen
      GPRS_SetzeDateiname(EEPROM_SNrLesen());
    }    
    else if (gstrKomEinBef == "TKA"){
      // Test "keine Antwort"
      gboKomAus = false;
    }
    else if (gstrKomEinBef == "RST"){
      // Neustart durchführen nach X ms
      wdt_enable(WDTO_250MS);
    }
    else if (gstrKomEinBef == "ZTL"){
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
//      LOG_Init(OUT_SD);   // test JoWu
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
#ifdef PROTOCOL_DEBUG_FREE_RAM
      gstrKomAus += "FreeRam: ";
      gstrKomAus += freeRam();
#endif
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
}

void Task2(){//configured with 250ms interval (inside ArduSched.h)
  //+++++++++++++++++++++++++++++++++++
  //static uint8_t dummyID_Article[][7] =  {{0xDF, 0x53, 0xA6, 0x59},{0xE0, 0x48, 0x9A, 0x1B},{0x45, 0x95, 0xA5, 0x59},{0xD0, 0xE7, 0x08, 0x1B},{0xB7, 0xD9, 0xA4, 0xA9},{0xD0, 0xB7, 0x7B, 0x1B},{0xD0, 0x9C, 0xAF, 0x1B}};
  //static uint8_t dummyID_User[][7] =     {{0xF5, 0xD1, 0xCE, 0xB0},{0x75, 0x12, 0x47, 0xBE},{0xC6, 0xB7, 0xBC, 0xBB},{0x45, 0x54, 0xBF, 0xB0},{0xF5, 0xFB, 0x52, 0xBE},{0x9B, 0x87, 0x83, 0xB9},{0x06, 0x3E, 0xB3, 0xBB}};
  
  static uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  static uint8_t uidKey[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  static uint8_t uid2[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  static uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  static uint8_t uidLengthKey;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  static uint8_t uidLength2;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  static unsigned char rub_aktiveRfidReader = USER_AND_ARTICLE_RFID;
  static unsigned char rub_TaskDelayUserRFID = 0;
  static unsigned char rub_TaskDelayArticleRFID = 1;
  boolean lbo_DetectRFID_Chip;
  boolean rbo_DetectRFID_ChipKey;
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
  int i;
  //+++++++++++++++++++++++++++++++++++

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
 #ifdef USE_RFID_KEYLOCK
      // generate Key Okay Timeout
      if (rbyKeyOkay){
        rbyKeyOkay--;
      }
      // check for Key (Keyreader)
      memset(uidKey, 0, sizeof(uidKey));
      rbo_DetectRFID_ChipKey = 0;
      rbo_DetectRFID_ChipKey = SL030readPassiveTargetID(SL030ADR_KEY, &uidKey[0], &uidLengthKey, 50);
      if (rbo_DetectRFID_ChipKey){
        for (i = 0; i < uidLengthKey; i++){
          Serial.print(uidKey[i], HEX); Serial.print(" ");
        }
        Serial.println();
      }
      if(rbo_DetectRFID_ChipKey){
//        if(rbo_RFID_ChipRemoved == true){
//          rbo_RFID_ChipRemoved = false;
          if (checkArticleID(uidLengthKey, &uidKey[0])){
            if (rbyKeyOkay == 0){
              Beeper(BEEP_DETECT_TAG);
//              OkLedSet(LED_TAG_CHECK);
            }
            rbyKeyOkay = KEY_OKAY_TIMEOUT;
          }
          else{
            Beeper(BEEP_UNKNOWN_ID);
            rbyKeyOkay = 0;
          }
//        }
      }
      if (!rbyKeyOkay){
        if (digitalRead(PI_LOCK_STAT_LOCKED)){
          gboolKeyLockOpenCmd = true; // Open Lock, open KeyLock
        }
      }
 #endif
      // check for User
      lbo_DetectRFID_Chip = SL030readPassiveTargetID(SL030ADR, &uid[0], &uidLength, 50);
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
//            lub_RFID_UserIdValid = false;           // JoWu; for debugging
            if(lub_RFID_UserIdValid){
              OkLedSet(LED_TAG_OK);
              ErrorLedSet(LED_TAG_OK);
              //add check with getMotorLockState();!!!
              setMotorLockCommand(UNLOCKING);
              gboolKeyLockOpenCmd = true; // Open Lock, open KeyLock
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

}

void Task3(){//configured with 1000ms interval (inside ArduSched.h)
  static int liZtDHM = 0;     // Überwachungszeit Signal "Digitaler Hausmeister" aktiv
  bool lboDHM = false;        // Signal "Digitaler Hausmeister" aktiv
  
  //insert code or function to call here:
  digitalWrite(OUT_LED, digitalRead(OUT_LED) ^ 1);

  //insert code or function to call here:
//  SoundAndLedHandler();

  // "Digitaler Hausmeister"
  lboDHM = (digitalRead(DE_DHM) == HIGH);
  lboDHM = false;                           // 2020-03-14; JoWu; disable "Digitaler Hausmeister"
  
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

}
void Task4(){
  static byte byToggle;
  //insert code or function to call here:
  if (digitalRead(PI_LOCK_STAT_LOCKED)){
    digitalWrite(PO_LOCK_LED_G, HIGH);
  }
  else{
    if (byToggle){
      byToggle = 0;
      digitalWrite(PO_LOCK_LED_G, LOW);
    }
    else{
      byToggle = 1;
      digitalWrite(PO_LOCK_LED_G, HIGH);
    }
  }
  if (gbyLockOpenTimer){
    gboolKeyLockOpenCmd = false;
    gbyLockOpenTimer--;
    if (gbyLockOpenTimer){
      digitalWrite(PO_LOCK_UNLOCK, HIGH);
    }
    else{
      digitalWrite(PO_LOCK_UNLOCK, LOW);
    }
  }
  else{
    digitalWrite(PO_LOCK_UNLOCK, LOW);
    if (gboolKeyLockOpenCmd){
      gbyLockOpenTimer = 3;
      digitalWrite(PO_LOCK_UNLOCK, HIGH);
    }
  }
}
unsigned long rulLifeCheckMillisOld;
unsigned short ruwLifeCheckTimer;
unsigned short ruwDownloadTimer;
void Task5(){
  //insert code or function to call here:
    if (millis()-rulLifeCheckMillisOld > 60000){
      rulLifeCheckMillisOld = millis();
      ruwLifeCheckTimer++;
      ruwDownloadTimer++;
      if (ruwLifeCheckTimer >= 2*60){
        LOG_Eintrag("Sys: Heartbeat(" + String(rulLifeCheckMillisOld) + ")");
        ruwLifeCheckTimer = 0;
      }
      if (ruwDownloadTimer >= 6*60){
        ruwDownloadTimer = 0;
        LOG_Eintrag("WelAcc: Initiate Download");
        GPRS_DateiLesen('A');
        GPRS_DateiLesen('B');
      }
    }
}
void Task6(){
  //insert code or function to call here:
}
void Task7(){
  //insert code or function to call here:
}
void Task8(){
  static byte byToggle;
  
  //insert code or function to call here:
#ifdef ARDSCHED_TEST
  unsigned char idx;
  Serial.println("Task 8");
  Serial.print("- times: ");
  for (idx = 0; idx < TASK_USED_NUM; idx ++){
    Serial.print(gaulTaskTime[idx]);
    gaulTaskTime[idx] = 0;
    Serial.print("µs ");
  }
  Serial.println();
#endif  
#if 0
  if (/*byToggle*/digitalRead(PI_LOCK_STAT_LOCKED)){
    digitalWrite(PO_LOCK_UNLOCK, HIGH);
    byToggle = 0;
  }
  else{
    digitalWrite(PO_LOCK_UNLOCK, LOW);
    byToggle = 1;
  }
#endif
}

int iSensorBrightness;

void Tmr3_ISR(){
#ifdef EN_OUTPUT_TASKTEST_SIGNALS
  //digitalWrite(OUT_TMR3_TIMING_SIG, digitalRead(OUT_TMR3_TIMING_SIG) ^ 1); 
  digitalWrite(OUT_TMR3_TIMING_SIG, HIGH);
#endif

  //insert code or function to call here:
  MotorLockHandler();//called all 50 or 100ms (depending on beeper frequency -> SWITCH2)

  // Bike charger LED detection
  iSensorBrightness = analogRead(0);
  if (iSensorBrightness > 800){
    // LED OFF
    digitalWrite(PO_RS485_TX_ON, LOW);
    digitalWrite(PO_RS485_TX, HIGH);  // red
    
    digitalWrite(PO_BS_LED_R, LOW);
    digitalWrite(PO_BS_LED_G, LOW);
    
  }
  else if(iSensorBrightness > 500){
    // LED GREEN
    digitalWrite(PO_RS485_TX_ON, HIGH);
    digitalWrite(PO_RS485_TX, LOW);   // green
    digitalWrite(PO_BS_LED_R, LOW);
    digitalWrite(PO_BS_LED_G, HIGH);
  }
  else{
    // LED RED
    digitalWrite(PO_RS485_TX_ON, HIGH);
    digitalWrite(PO_RS485_TX, HIGH);  // red
    digitalWrite(PO_BS_LED_R, HIGH);
    digitalWrite(PO_BS_LED_G, LOW);
  }
  
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
boolean SL030readPassiveTargetID(uint8_t u8SL030Adr, uint8_t* puid, uint8_t* uidLength, uint8_t u8MaxLen)
{
  unsigned char u8Len;
  unsigned char u8ProtNr;
  unsigned char u8Status;
#ifdef USE_SL030_OUT
  if (u8SL030Adr == SL030ADR){
    for (u8Len = 0; u8Len < 10; u8Len++){
      if (digitalRead(PI_SL030_OUT)){
  #ifdef SERIAL_DEBUG_ENABLE
        Serial.println("RFID: No User detected");
  #endif
        return(false);
        delay(1);
      }
    }
  }
#endif
//  Serial.println("RFID: User detected");
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
  Wire.beginTransmission(u8SL030Adr/2); // transmit to device #SL030ADR
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
  u8Len = Wire.requestFrom(u8SL030Adr/2, 10, true);    // request 10 byte from slave device #SL030ADR, which is the max length of the protocol
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
      Wire.endTransmission(true);     // stop transmitting
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
      Wire.endTransmission(true);     // stop transmitting
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
    Wire.endTransmission(true);     // stop transmitting
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
unsigned char u8Preamble;
unsigned char u8Len;
unsigned char u8ProtNr;
unsigned char u8Status;
unsigned char au8Frame[11];
  // set serial timeout to 50 ms (measured 45 ms)
  Serial2.setTimeout(50);
  
      puid[0] = 0;
      puid[1] = 0;
      puid[2] = 0;
      puid[3] = 0;
      puid[4] = 0;
      puid[5] = 0;
      puid[6] = 0;
//  while(Serial2.available() == 0);    // !!!JoWu: Dead End, if no response
  
  // check for 0xBD protocol
  if (Serial2.readBytes(&u8Preamble, 1) != 0x01){
    return(0);
  }
  // check if we have an answer to 0xBA Request gives the right preamble 0xBD
  if (u8Preamble == 0xBD){
    // read len
    if (Serial2.readBytes(&u8Len, 1) != 0x01){
      return(0);
    }
    if ( (u8Len != 8) && (u8Len != 11)){
      return(0);
    }
    if (Serial2.readBytes(&au8Frame[0], u8Len) != u8Len){
      return(0);
    }
    
    u8ProtNr = au8Frame[0];
    u8Status = au8Frame[1];
    if (u8Len == 8)
    {
      puid[0] = au8Frame[2];
      puid[1] = au8Frame[3];
      puid[2] = au8Frame[4];
      puid[3] = au8Frame[5];
      puid[4] = 0;
      puid[5] = 0;
      puid[6] = 0;
      return 4;
    }
    else if (u8Len == 11)
    {
      puid[0] = au8Frame[2];
      puid[1] = au8Frame[3];
      puid[2] = au8Frame[4];
      puid[3] = au8Frame[5];
      puid[4] = au8Frame[6];
      puid[5] = au8Frame[7];
      puid[6] = au8Frame[8];
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
