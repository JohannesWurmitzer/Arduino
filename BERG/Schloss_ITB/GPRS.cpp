/*
 Name:    GPRS.cpp
 Datum:   17.05.2018
 Autor:   Markus Emanuel Wurmitzer

  Versionsgeschichte:

  2021-04-25  V120  JoWu
    - optimized noInterrupts(); position to not block the system too much, which lead to corruption while UART communication with PC application
  
  2021-04-11  V119  JoWu
    - add ftp user "ftpurbansharingft" for field test
    - changed subdomain from "server" to "welaccess"

  2021-01-17  V118  JoWu
    - add welaccess server entry

  2021-01-11  V117  JoWu
    - move static RAM to Flash
    
  2020-11-20  V116  JoWu
    - improvements in FTP handling
    - new FTP download concept with blockwise reading
    - rename file after reading
  
  2020-11-17  V115  JoWu
    - improvements in ID reading via FTP
  
  2020-11-16  V114  JoWu
    - improvements in FTP handling

  2020-11-15  V112  JoWu
    - change filename of textfile to
      [SerialNo]_[ImeiNo]_[ImsiNo].txt
    - add reading IMSI-Number
    - change filename of user and article data
    
  2020-11-09  V111  JoWu
    - change filename of textfile

  2020-10-26  V110  JoWu
    - change FTP user to WelAccessClient
    - change Filename of textfile
    - move FTP-Setup to macro defines

  2020-09-21  V109  JoWu
    - add error handling in GZM_FTP_PUT_START
    - move FTP-String generation from GZM_FTP_PUT_START to GZM_FTP_PUT_WRITE to not loose data in case of connection or file opening problems

  2020-09-06  V108  JoWu
    - replaced lfstrEintrag String object by char array 

  2020-09-02  V107  JoWu
    - add T-Mobild M2M APN
    - reintroduced ringbuffer for GPRS-Log
    - introduced GPRS_LOGBUF_SIZE to set size of ringbuffer
    - reduced size of ringbuffer from 20 to 10 because of ram issues

  2020-08-17  V106  JoWu
    - return from void GPRS_Logeintrag(String strEintrag) before adding string because of memory problems
  
  2020-05-22  V105  JoWu
    released
    
    planned
    - get IMEI Number
    - get IMSI Number
    - get Telephone Number
  
  2020-05-21  V104  JoWu
    - PIN_PWRKEY instead of PIN_DEAKT
    - massive changes and speedup
    - implemented lboATOK, lboATERROR to make the statemachin more easy

  2020-05-21  V103  JoWu
    - renamed and extended debug output
    - little improvement in GPRS_SerEin(), but many improvements possible
  
  18.07.2018  V 102   Markus Emanuel Wurmitzer
    - Parameter mobiles Internet APN Einstellung eingebunden
    - FTP Benutzerdatei herunterladen eingefügt
    - EEPROM Zugriffe für die Benutzer- und Artikelnummern eingefügt
    - Funktion zum Setzen des Merkers "FTP Datei herunterladen" eingefügt
    
  30.05.2018  V 101   Markus Emanuel Wurmitzer
    - PIN_DEAKT eingefügt
  
  18.05.2018  V 100   Markus Emanuel Wurmitzer
    - Erstellung
*/

/*
  todo
  2021-01-17  JoWu  maybe we shou implement multiple server choice, but reflashing would also work in most cases
  2020-11-09  JoWu  upload user and article data
  2020-08-30  JoWu  file:///C:/Program%20Files%20(x86)/Arduino/reference/www.arduino.cc/en/Reference/AttachGPRS.html
  2020-05-24  JoWu  GSM Module answer "\r\nERROR" should be logged
  2020-05-22  JoWu  State GZM_FT_PU aufteilen
  2020-05-22  JoWu  check functionality GPRS_DateiLesen()
  2020-05-22  JoWu  IMPORTANT indexOf() does only look for the indes of one character!
  2020-05-21  JoWu  introduce or improve line by line handling
  2020-05-21  JoWu  implement SMS handling
  2020-05-21  JoWu  implement new AT-Command handler
  2020-05-21  JoWu  move "binäre Kommunikation aktivieren" out of interrupt service handler
  
*/

#include "GPRS.h"
#include "EepromAndRFID_IDs.h"

#if 0
#define DEBUG_MAWU      // debug info from Markus, to be checked and renamed
#define DEBUG_ECHO      // echo all received data from SIM900
#define DEBUG_ECHO_BIN  // echo all received binary data from SIM900
#define DEBUG_ECHO_CMD  // echo all output commands to SIM900
#endif

// ftp server settings
#if 0   // define 1, if use of urban sharing server (field test)
 #define FTP_SERVER      "welaccess.wurmitzer.net"
 #define FTP_USER        "ftpurbansharingft"
 #define FTP_PWD         "i0F_e6i0"
#endif
#if 0   // define 1, if use of fieldserver
 #define FTP_SERVER      "welaccess.wurmitzer.net"
 #define FTP_USER        "ftpwelaccessiotclient"
 #define FTP_PWD         "0B7v*fs1"
#endif
#if 1   // define 1, if use of testserver (Sandbox)
 #define FTP_SERVER      "welaccess.wurmitzer.net"
 #define FTP_USER        "ftpwelaccessiotclientsb"
 #define FTP_PWD         "0B7v*fs1"
#endif
#if 0   // define 1, if use of old testserver
 #define FTP_SERVER      "wp011.webpack.hosteurope.de"
 #define FTP_USER        "ftp12069872-martin"
 #define FTP_PWD         "ai901!MK"
#endif
#if 0   // define 1, if use of old fieldserver
 #define FTP_SERVER      "wp011.webpack.hosteurope.de"
 #define FTP_USER        "ftp12069872-WelAccessClient"
 #define FTP_PWD         "WelAccessClientIoT001!"
#endif

#define GPRS_APN_TMOBILE //
#define GPRS_APN_M2M     //

#define GPRS_LOGBUF_SIZE  6 //10    // [num] Entries in Log-Buffer
#define GPRS_LOGBUF_SIZE_ENTRY  90 // [bytes] size of one entry in Log-Buffer

#define GSM_UART_RX_BUFFER_SIZE 256 //512

void EepromWriteFtpId(void);

/*
 * externe Dateien
 */

// remanente Variablen
static GPRS_ZM leModZM = GZM_UNBE;    // GSM-Module Zustandsmaschine, aktiver Schritt
static GPRS_ZM leModZMNeu = GZM_INIT; // GSM-Module Zustandsmaschine, nächster Schritt

unsigned char rbyGsmSmSubState;       // GSM Statemachine SubState

static bool lboKomBin;                // binäre Kommunikation ist aktiv
static byte raubGsmUartRxBuffer[GSM_UART_RX_BUFFER_SIZE];          // binäre Kommunikation Eingangspuffer
static int ruwGsmUartRxBufferIndex;   // Index im GSM UART Buffer, gleichzeitig Anzahl und Länge der binären Eingangsdaten
static String lstrKomEin;             // ASCII-Text Kommunikation Eingangsdaten          
static byte lbyKomZt;                 // Überwachungszeit Kommunikationseingang
static bool lboKomEin;                // Protokolleingang erkannt

static String lstrDatei;              // Dateiname für den FTP-Zugriff
static bool lboDatei;                 // Dateiname hat sich geändert

static char lfstrEintrag[GPRS_LOGBUF_SIZE][GPRS_LOGBUF_SIZE_ENTRY];       // Ringpuffer Daten für die Übertragung zum Server
static int liESID = 0;                // Ringpuffer Index des letzten zu übertragenden (geschriebenen) Eintrags
static int liETID = 0;                // Ringpuffer Index des letzten transferierten Eintrags

static String lstrFTP;                // Daten zusammengefasst für die / aus der FTP Übertragung
static int liAPN = 0;                 // ausgewählter APN
static bool lboDatBL = true;          // Datei Benutzer auslesen
static bool lboDatAL = true;          // Datei Artikel auslesen
static int liEDID = 0;                // Eingangsdaten ID-Nummer (Benutzer / Artikel)
static int liIDID = 0;                // interne Daten ID-Nummer (Benutzer / Artikel)
static unsigned char lubyTyp;         // Datentyp für die Benutzer- und Artikelverwaltung

byte gbyGSMModuleLatestRxByte;        // latest received byte from GSM Module

bool rboSendFTPData;                  // Cmd to send FTP-Data
bool rboSentFTPData;                  // Response sent FTP-data

char gacGsmIMEI[16];                  // GSM IMEI Nummer
char gacGsmIMSI[16];                  // GSM IMSI Nummer
char gscGsmCNUM[16];                  // GSM CNUM Nummer


unsigned long rulTxTimeStamp;         // [ms] Timestamp stored when protocol was sent
unsigned short ruwRxTimer;            // [ms] Time since Tx Timestamp

// GPRS initialisieren
void GPRS_Init(void){ 
  Serial3.begin(19200);
  pinMode(PIN_PWRKEY, OUTPUT);
  digitalWrite(PIN_PWRKEY, LOW);

}

// GPRS Eingangsdaten lesen
// diese Aufruf muss außerhalb des normalen Zyklus aufgerufen werden
void GPRS_SerEin(void){
//void Serial3Event(){
  // Dateneingang prüfen
  while(Serial3.available())
  {
    gbyGSMModuleLatestRxByte = Serial3.read();
    if (lboKomBin){
      // binäre Daten
#ifdef DEBUG_ECHO_BIN
      Serial.print((char)gbyGSMModuleLatestRxByte);
#endif
      raubGsmUartRxBuffer[ruwGsmUartRxBufferIndex++] = gbyGSMModuleLatestRxByte;
    }
    else{
      // ASCII Text
#ifdef DEBUG_ECHO
      Serial.print((char)gbyGSMModuleLatestRxByte);
#endif
      if (ruwGsmUartRxBufferIndex < GSM_UART_RX_BUFFER_SIZE){
        raubGsmUartRxBuffer[ruwGsmUartRxBufferIndex++] = gbyGSMModuleLatestRxByte;
//        lstrKomEin += char(gbyGSMModuleLatestRxByte);
      }
      lstrKomEin += char(gbyGSMModuleLatestRxByte);
    }

    // binäre Kommunikation aktivieren    !!!JoWu should be moved to statemachine
    if ((leModZM == GZMSEVE) && (lstrKomEin.endsWith(F("\r\nCONNECT OK\r\n")))){
      lboKomBin = true;
    }
    
    lbyKomZt = 0;
    lboKomEin = false;
  }
}

// GPRS Zustandsmaschine
// Überwachungszeiten sind intern in Zyklen angegeben!
void GPRS_Zustandsmaschine(void)
{
  static bool boZMSR = false;       // Zustandsmaschinenschritt rücksetzen, bzw. neu starten
  static String strZMKomAus;        // Zustandsmaschine Ausgangsdaten
  
  static bool lboZMKom = false;     // Zustandsmaschine Kommunikation ist aktiv
  static byte byZMZt = 0;           // Zustandsmaschine Überwachungszeit aktueller Schritt
  static byte byZMKomAZt = 3;       // Zustandsmaschine Antwortzeit

  bool boZMNeu = false;             // Zustandsmaschinenschritt erster Aufruf
  bool boZMZt = false;              // Zustandsmaschinenschritt Zeit abgelaufen

  bool lboATOK;                     // Answer OK after an AT command is here
  bool lboATERROR;                  // Answer ERROR after an AT command is here

  unsigned int luwGsmUartRxDataSize;  // actual amount of GSM UART Rx Data

  // copy ISR GSM UART Rx Buffer Index to local size variable
  luwGsmUartRxDataSize = ruwGsmUartRxBufferIndex;

  //
  // Protokollabschluss prüfen
  
  if (lboZMKom){
    // Zeitüberwachung Protokollende
    if (lbyKomZt < byZMKomAZt){     
      lbyKomZt++;

      if (lbyKomZt >= byZMKomAZt){
        // Merker setzen
        lboKomEin = true;       // !!!JoWu, eigentlich sollte hier ein Timeout stehen

#ifdef DEBUG_MAWU
        // Protokoll ausgeben - ASCII Text
        if (lstrKomEin.length() > 0){        
          //Serial.print("<- " + lstrKomEin);
        }
        // Protokoll ausgeben - binäre Daten als hexadezimale Zeichenkette
        else if (luwGsmUartRxDataSize > 0){
          Serial.print(F("<-"));
          for (int i = 0; i < luwGsmUartRxDataSize; i++){
            Serial.print(F(" "));
            if (raubGsmUartRxBuffer[i] < 16){
              Serial.print(F("0"));
            }
            Serial.print(String(raubGsmUartRxBuffer[i], HEX));
          }
          Serial.println();
        }
#endif
      }
    }
  }  

  // Schrittkette Modem
  if ((leModZM != leModZMNeu) || boZMSR){
#ifdef DEBUG_MAWU
    if (leModZM != leModZMNeu){
      Serial.print(F("SM: ")); Serial.println(leModZMNeu);
    }
#endif
    // Neuer Schritt oder Schritt zurückgesetzt
    rboSendFTPData = false;
    rboSentFTPData = false;
    
    if (!boZMSR){
      // Ausgangsprotokoll nur bei Schrittwechsel löschen
      strZMKomAus = "";
      byZMKomAZt = 3;         // [100 ms] Standardantwortzeit des Modems in Zyklen, diese muss bei langsamen Befehlen angepasst werden
    }
    // Merker zurücksetzen
    boZMNeu = true;         // Schritt neu gestartet
    boZMSR = false;         // Schritt rücksetzen
    
    lboZMKom = false;       // Kommunikation eingeleitet
    
    leModZM = leModZMNeu;   // aktueller Schritt
    noInterrupts();
    lstrKomEin = "";        // Eingangsprotokolldaten
    interrupts();
    lboKomEin = false;      // Eingangsprotokoll erkannt
    lbyKomZt = 0;           // Kommunikationszeit Eingangsprotokollende
    
    byZMZt = 20;            // Standardüberwachungszeit pro Schritt in Zyklen !!!JoWu gefährlich bei Verwendung einer anderen Zeit im Schritt!!!
    //Serial.println((String)F("Neuer Schritt ") + String(leModZM));
  }
  else{
    // Schrittzeit reduzieren, Zustandsmaschinenschritt Timeout generieren
    ruwRxTimer = millis()-rulTxTimeStamp;

    if (byZMZt){
      byZMZt--;
    }
  }
  
  // prüfen ob Zustandsmaschinenschritt Zeit abgelaufen
  boZMZt = (byZMZt == 0);


  // prüfen, ob eine Antwort vom GSM-Modul vollständig vorliegt
  lboATOK = 0;
  lboATERROR = 0;
  if (gbyGSMModuleLatestRxByte == '\n'){
    if (lboZMKom && lstrKomEin.indexOf(F("OK\r\n"))>=0){
      lboATOK = 1;
      // trim leading \r\n of new message
      while (lstrKomEin.indexOf("\n")>=0 && lstrKomEin.indexOf("\n")<2){
        noInterrupts();
        lstrKomEin.remove(0, lstrKomEin.indexOf("\n")+1);
        interrupts();
      }
    }
    else if (lboZMKom && lstrKomEin.endsWith(F("ERROR\r\n"))){         // ERROR should be evaluated and mybe logged
      lboATERROR = 1;
    }
  }
  //
  // Zustandsmaschine
  //
  switch(leModZM){
    // Initialisierung
    case GZM_INIT:
      if (!lboZMKom){
        strZMKomAus = F("ATE0");       // JoWu: Echo 1 seams to be important now for functionality, 2020-05-22; JoWu; solved, not relevant anymore
      }
      else if (lboATOK){
        leModZMNeu = GZM_INIT_IPR_RD;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT_PWRKEY_ON;
      }
      break;
  
    // Initialisierung: Baudrate prüfen
    case GZM_INIT_IPR_RD:
      if (!lboZMKom){
        strZMKomAus = F("AT+IPR?");
      }
      else if (lboATOK){
        leModZMNeu = GZM_INIT_FUN;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break;

    // Initialisierung: Funktionalität setzen
    case GZM_INIT_FUN:
      if (!lboZMKom){
        strZMKomAus = F("AT+CFUN=1");
      }
      else if (lboATOK){
        leModZMNeu = GZM_RD_IMEI;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break;

    // Initialisierung: Modul neu starten
    case GZM_INIT_PWRKEY_ON:
      if (boZMNeu){
        digitalWrite(PIN_PWRKEY, HIGH);                 
        byZMZt = 10;    // 1000 ms
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        digitalWrite(PIN_PWRKEY, LOW);
        leModZMNeu = GZM_INIT_BOOT_WAIT;
      }
      break;
      
    // Init: Wait for GSM Modul to Boot
    case GZM_INIT_BOOT_WAIT:
      if (boZMNeu){
      }
      else if (boZMZt){
        leModZMNeu = GZM_INIT;
      }
      break;

    // Read IMEI number
    case GZM_RD_IMEI:
      if (!lboZMKom){
        strZMKomAus = F("AT+GSN");
      }
      else if (lboATOK){
        if (lstrKomEin.indexOf("\r") == 15){
          lstrKomEin.toCharArray(gacGsmIMEI, 15+1);
#ifdef DEBUG_MAWU
          Serial.print(F("IMEI: ")); Serial.println(gacGsmIMEI);
#endif
        }
        leModZMNeu = GZM_RD_IMSI;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break;

    // Read IMSI number
    case GZM_RD_IMSI:
      if (!lboZMKom){
        strZMKomAus = F("AT+CIMI");
      }
      else if (lboATOK){
        if (lstrKomEin.indexOf("\r") == 15){
          lstrKomEin.toCharArray(gacGsmIMSI, 15+1);
#ifdef DEBUG_MAWU
          Serial.print(F("IMSI: ")); Serial.println(gacGsmIMSI);
#endif
        }
        leModZMNeu = GZM_SIM_PIN;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break;

    // SIM-Status abfragen
    case GZM_SIM_PIN:
      if (!lboZMKom){
        strZMKomAus = F("AT+CPIN?");
      }
      else if (lboATOK && lstrKomEin.startsWith(F("+CPIN: READY"))){
        leModZMNeu = GZMNEQU;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break;

    // Netz: Verbindungsqualität prüfen
    case GZMNEQU:
      if (!lboZMKom){
        strZMKomAus = F("AT+CSQ");
      }
      else if (lboATOK){
        leModZMNeu = GZMNERE;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }        
      break;

    // Netz: Registrierung prüfen
    case GZMNERE:
      if (!lboZMKom){
        strZMKomAus = F("AT+CREG?");
      }
      else if (lboATOK){
        if (lstrKomEin.startsWith(F("+CREG: 0,1")) || lstrKomEin.startsWith(F("+CREG: 0,5"))){
          // Registrierung erfolgreich
          leModZMNeu = GZMNEAN;
        }
        else{
          // Registrierung fehlgeschlagen, erneut Verbindung prüfen
          boZMSR = true;
        }
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break;    

    // Netz: Anbindung prüfen
    case GZMNEAN:
      if (!lboZMKom){
        strZMKomAus = F("AT+CGATT?");
      }
      else if (lboATOK && lstrKomEin.startsWith(F("+CGATT: 1"))){
        leModZMNeu = GZM_FH_PS;
        //leModZMNeu = GZMTUVE;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break; 

    // GPRS FTP / HTTP: Parameter setzen
    case GZM_FH_PS:
      if (!lboZMKom){
        if (strZMKomAus == ""){
          strZMKomAus = F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");    // \" inserts " into the string
        }
        else if (strZMKomAus.indexOf(F("CONTYPE")) >= 0){
          if (liAPN == 1){
            // HoT
            strZMKomAus = F("AT+SAPBR=3,1,\"APN\",\"webaut\"");
          }
          else if (liAPN == 2)
          {
            // Standardzugang T-Mobile
            strZMKomAus = F("AT+SAPBR=3,1,\"APN\",\"gprsinternet\"");
          }
          else
          {
            // Standardzugang T-Mobile M2M - APN
            strZMKomAus = F("AT+SAPBR=3,1,\"APN\",\"m2m.public.at\"");
          }
        }
        else if (strZMKomAus.indexOf(F("APN")) >= 0)
        {
          if (liAPN == 1)
          {
            strZMKomAus = F("AT+SAPBR=3,1,\"USER\",\"\"");  
          }
          else if(liAPN == 2)
          {
            strZMKomAus = F("AT+SAPBR=3,1,\"USER\",\"t-mobile\"");
          }
          else
          {
            // Standardzugang T-Mobile M2M - Username
            strZMKomAus = F("AT+SAPBR=3,1,\"USER\",\"\"");
          }        }
        else if (strZMKomAus.indexOf(F("USER")) >= 0)
        {
          if (liAPN == 1)
          {
            strZMKomAus = F("AT+SAPBR=3,1,\"PWD\",\"\"");
          }
          else if(liAPN == 2)
          {
            strZMKomAus = F("AT+SAPBR=3,1,\"PWD\",\"tm\"");
          }
          else
          {
            // Standardzugang T-Mobile M2M - Passwort
            strZMKomAus = F("AT+SAPBR=3,1,\"PWD\",\"\"");
          }
        }
      }
      else if (lboATERROR){
        // Modul neu starten
        leModZMNeu = GZM_INIT_PWRKEY_ON;
      }
      else if (lboATOK){
        if (strZMKomAus.indexOf(F("PWD")) >= 0){
          // Verbindungsaufbau beginnen
          leModZMNeu = GZM_FH_VP; // GZM_FH_VE;
        }
        else{
          // Parametrierung noch nicht abgeschlossen
          boZMSR = true;
        }              
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }        
      break;    

    // FTP/HTTP Verbindung prüfen (Bearer open)
    case GZM_FH_VP:
      if (boZMNeu){
        strZMKomAus = F("AT+SAPBR=2, 1");
      }
      else if (lboATOK){
        if (lstrKomEin.startsWith(F("+SAPBR: 1,1,"))){
          // Verbindung steht schon
          leModZMNeu = GZM_FT_KS;
        }
        else{
          leModZMNeu = GZM_FH_VE;           // Verbindungsaufbau beginnen
        }
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_FH_VE;           // Verbindungsaufbau beginnen
      }        
      break;

    // GPRS FTP / HTTP: Verbindung aufbauen
    case GZM_FH_VE:
      if (!lboZMKom){
        strZMKomAus = F("AT+SAPBR=1,1");
        // Überwachungszeiten vergrößern
        byZMKomAZt = 30;
        byZMZt = 50;
      }
      else if (lboATERROR){
        // Verbindungsversuch gescheitert, Trennvorgang versuchen
        leModZMNeu = GZMFHVT;
#ifdef DEBUG_MAWU
        Serial.println(F("fault"));
#endif
      }
      else if (lboATOK){
        // Verbindungsversuch erfolgreich
        leModZMNeu = GZM_FT_KS;
#ifdef DEBUG_MAWU
        Serial.println(F("success"));
#endif
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break;

    // GPRS FTP / HTTP: Verbindung trennen
    case GZMFHVT:
      if (!lboZMKom){
          strZMKomAus = F("AT+SAPBR=0,1");
          // Überwachungszeiten vergrößern
          byZMKomAZt = 30;
          byZMZt = 50;
      }
      else if (lboATOK || lboATERROR){
        // neuer Verbindungsversuch
        leModZMNeu = GZM_FH_VE;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break;

    // GPRS TCP / UDP: Verbindung herstellen
    case GZMTUVE:
      if (!lboZMKom)
      {
        // für TCP und UDP Verbindungen
        strZMKomAus = F("AT+CSTT=\"gprsinternet\",\"t-mobile\",\"tm\"");
      }
      else if (lboKomEin)
      {
        if (lstrKomEin.indexOf(F("OK")) >= 0)
        {
          // Verbindungsaufbau erfolgreich
          leModZMNeu = GZMTUVA;
        }
        else
        {
          // Verbindungsaufbau fehlgeschlagen, Trennvorgang einleiten
          leModZMNeu = GZMTUVT;
        }
      }
      else if (boZMZt)
      {
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }   
      break;

    // GPRS TCP / UDP: Verbindung trennen
    case GZMTUVT:
      if (!lboZMKom)
      {
        strZMKomAus = F("AT+CIPSHUT");         
      }    
      else if (lboKomEin && (lstrKomEin.indexOf(F("OK")) >= 0))
      {
        leModZMNeu = GZMTUVE;
      }
      else if (boZMZt)
      {
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }        
      break;

    // GPRS TCP / UDP: Verbindung aktivieren
    case GZMTUVA:
      if (!lboZMKom)
      {
        if (strZMKomAus == "")
        {
          // Verbindung aktivieren
          strZMKomAus = F("AT+CIICR");
        }
        else if (strZMKomAus.indexOf(F("CIICR")) >= 0)
        {
          // IP-Adresse abfragen
          strZMKomAus = F("AT+CIFSR");
        }
      }
      else if (lboKomEin)
      {
        if ((strZMKomAus.indexOf(F("CIFSR")) >= 0) && (lstrKomEin.indexOf(F("ERROR")) < 0))
        {
          //  TCP Verbindung öffnen
          leModZMNeu = GZMSEVE;
        }
        else if (lstrKomEin.indexOf(F("OK")) >= 0)
        {
          // nächsten Befehl senden
          boZMSR = true;
        }
      }        
      else if (boZMZt)
      {
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }        
      break;   

    // FTP-Verbindung: Konfiguration Serverzugang
    case GZM_FT_KS:
      if (!lboZMKom){
        if (strZMKomAus == ""){
          // FTP-Profil aktivieren
          strZMKomAus = F("AT+FTPCID=1");
        }
        else if (strZMKomAus.indexOf(F("CID")) >= 0){
          // Serveradresse einstellen
//          strZMKomAus = F("AT+FTPSERV=\"" "wp011.webpack.hosteurope.de" "\"");
          strZMKomAus = F("AT+FTPSERV=\"" FTP_SERVER "\"");
        }
        else if (strZMKomAus.indexOf(F("SERV")) >= 0){
          // Serverschnittstelle eintragen
          strZMKomAus = F("AT+FTPPORT=21");
        }
        else if (strZMKomAus.indexOf(F("PORT")) >= 0){
          // Benutzernamen eintragen
//          strZMKomAus = F("AT+FTPUN=\"ftp12069872-martin\"");
          strZMKomAus = F("AT+FTPUN=\"" FTP_USER "\"");
        }
        else if (strZMKomAus.indexOf(F("PUN")) >= 0){
          // Passwort eintragen
//          strZMKomAus = "F(AT+FTPPW=\"ai901!MK\"");
          strZMKomAus = F("AT+FTPPW=\"" FTP_PWD "\"");
        }
      }
      else if (lboATOK){
        if (strZMKomAus.indexOf(F("PW")) >= 0){
          // Dateikonfiguration vornehmen
          leModZMNeu = GZM_FT_PUT_KD;          
        }
        else{
          // nächstes Kommando senden
          boZMSR = true;
        }
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }        
      break;
     
    // FTP-Verbindung: Konfiguration Datei
    case GZM_FT_PUT_KD:
      if (!lboZMKom){
        if (strZMKomAus == ""){
          // Zugriffsart "anhängen"
          strZMKomAus = F("AT+FTPPUTOPT=\"APPE\"");
        }
        else if (strZMKomAus.indexOf(F("OPT")) >= 0){
          // Dateinamen eintragen
          strZMKomAus = (String)F("AT+FTPPUTNAME=\"") + lstrDatei + "\"";         
          strZMKomAus.replace(".", "_" + String(gacGsmIMEI) + "_" + String(gacGsmIMSI)+".");
//          strZMKomAus = (String)F("AT+FTPPUTNAME=\"") + String(gacGsmIMEI) + "_" + lstrDatei + "\"";         
        }
        else if (strZMKomAus.indexOf(F("NAME")) >= 0){
          // Verzeichnis eintragen
          strZMKomAus = F("AT+FTPPUTPATH=\"/WelAccUpl/\"");            
        }
      }
      else if (lboATOK){
        if (strZMKomAus.indexOf(F("PATH")) >= 0){
          // Datenübertragung durchführen
          leModZMNeu = GZMFTWD;
          lboDatei = false;  
        }
        else{
          // nächstes Kommando senden
          boZMSR = true;
        }      
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }        
      break;

    // FTP-Verbindung: Warten auf Daten
    case GZMFTWD:
      if (boZMNeu){
        // FTP Daten nur alle X Zyklen senden
        byZMZt = 100;
      }
      else if (lboDatei){
        // Dateiname geändert
        leModZMNeu = GZM_FT_PUT_KD;
      }
      else if (boZMZt || liESID != liETID){
        // Zeit abgelaufen, auf neue Daten prüfen
        if (liESID != liETID){
          // Daten für den FTP Server vorhanden
          //Serial.println("FTP Daten vorhanden: " + String(liESID) + "/" + String(liETID));       
          lstrFTP = "";
//          while(liESID != liETID){
//            lstrFTP += String(lfstrEintrag[liETID]);  // nächsten String aus der Liste nehmen und in den FTP-String schreiben
//            liETID = (liETID+1) % GPRS_LOGBUF_SIZE;   // maximal GPRS_LOGBUF_SIZE Schreibeinträge
//          }
          //Serial.println("FTP Daten: " + lstrFTP);
          leModZMNeu = GZM_FTP_PUT_START;
        }
        else if (lboDatBL || lboDatAL){
          // Benutzerdatei auslesen
          leModZMNeu = GZM_FTP_GET_KD;
          lboDatei = true;
        }
        else{
          // Schritt neu starten
          boZMSR = true;
        }
      }
      break;


    // FTP-Verbindung: FTP-PUT Datenübertragung starten START
    // https://www.edaboard.com/threads/sim900-at-commands-for-ftp.277914/
    case GZM_FTP_PUT_START:
      if (!lboZMKom){
          strZMKomAus = F("AT+FTPPUT=1");  
        // Überwachungszeiten vergrößern
        byZMKomAZt = 50;
        byZMZt = 150;
      }
      else if (lboATOK){
        // wait for the allowed buffer size
        if (lstrKomEin.indexOf(F("+FTPPUT:1,1,")) >= 0){
          // maybe we should store the buffer size
          leModZMNeu = GZM_FTP_PUT_WRITE;
        }
        else if (lstrKomEin.indexOf(F("+FTPPUT:1,")) >= 0){
          // we got an error
          leModZMNeu = GZM_FH_VP;   // get back to state to check the bearer
        }
  
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZMNERE; // get back to check registration, old state was: GZMFTWD;
      }        
      break;


    // FTP-Verbindung: FTP-PUT Datenübertragung durchführen WRITE
    case GZM_FTP_PUT_WRITE:
      if (boZMNeu){
        // Überwachungszeiten vergrößern
        byZMKomAZt = 100;
        byZMZt = 150;
      }
      if (!lboZMKom){
        // Einträge zusammenfassen
        // Einträge übertragen
        while(liESID != liETID){
          lstrFTP += String(lfstrEintrag[liETID]);  // nächsten String aus der Liste nehmen und in den FTP-String schreiben
          liETID = (liETID+1) % GPRS_LOGBUF_SIZE;   // maximal GPRS_LOGBUF_SIZE Schreibeinträge
        }
        strZMKomAus = (String)F("AT+FTPPUT=2,") + String(lstrFTP.length());
      }
      else if (lboATOK){
        // Datenübertragung abgeschlossen
        leModZMNeu = GZM_FTP_PUT_CLOSE;          
      }
      else if (lstrKomEin.indexOf(F("+FTPPUT:2,")) >= 0){
          rboSendFTPData = true;     // send FTP-Data
      }
      if (boZMZt)
      {
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZMFTWD;
      }        
      break;


    // FTP-Verbindung: FTP-PUT Datenübertragung beenden CLOSE
    case GZM_FTP_PUT_CLOSE:
      if (boZMNeu){
        // Überwachungszeiten vergrößern
        byZMKomAZt = 50;
        byZMZt = 150;
      }
      if (!lboZMKom){
        // Datentransfer beenden
        strZMKomAus = F("AT+FTPPUT=2,0");
      }
      else if (lboATOK){
        if (lstrKomEin.indexOf(F("+FTPPUT:1,0")) >= 0){
//          Serial.println(F("closed"));
          // Datenübertragung abgeschlossen
          leModZMNeu = GZMFTWD;          
        }
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZMFTWD;
      }        
      break;

    // FTP-Verbindung: Konfiguration Lesedatei (Benutzer- / Artikelliste)
    case GZM_FTP_GET_KD:
      if (!lboZMKom)
      {
        if (strZMKomAus == "")
        {
          // Dateinamen eintragen
          String strDat = lstrDatei;
          if (lboDatBL){
            strDat.replace(F(".txt"),F("_u.txt"));
          }
          else{
            strDat.replace(F(".txt"),F("_a.txt"));
          }
          strZMKomAus = (String)F("AT+FTPGETNAME=\"") + strDat + "\"";         
        }
        else if (strZMKomAus.indexOf(F("GETNAME")) >= 0)
        {
          // Dateinamen eintragen
          String strDat = lstrDatei;
          if (lboDatBL){
            strDat.replace(F(".txt"),F("_u_d.txt"));
          }
          else{
            strDat.replace(F(".txt"),F("_a_d.txt"));
          }
          strZMKomAus = (String)F("AT+FTPPUTNAME=\"") + strDat + "\"";         
        }
        else if (strZMKomAus.indexOf(F("PUTNAME")) >= 0)
        {
          // Verzeichnis eintragen
          if (lboDatBL){
            strZMKomAus = F("AT+FTPGETPATH=\"/WelAccDow/\"");            
          }
          else{
            strZMKomAus = F("AT+FTPGETPATH=\"/WelAccDow/\"");
          }
        }
        else if (strZMKomAus.indexOf(F("GETPATH")) >= 0)
        {
          // Verzeichnis eintragen
          if (lboDatBL){
            strZMKomAus = F("AT+FTPPUTPATH=\"/WelAccDow/\"");            
          }
          else{
            strZMKomAus = F("AT+FTPPUTPATH=\"/WelAccDow/\"");
          }
        }
      }
      else if (lboKomEin && (lstrKomEin.indexOf(F("OK")) >= 0))
      {
        if (strZMKomAus.indexOf(F("PUTPATH")) >= 0)
        {
          // Datenübertragung durchführen
          leModZMNeu = GZMFTDL;
//          lboDatei = false;  
        }
        else
        {
          // nächstes Kommando senden
          boZMSR = true;
        }      
      }
      else if (boZMZt)
      {
        // Zeitüberwachung hat ausgelöst, zurück zum Warteschritt
        leModZMNeu = GZMFTWD;
      }        
      break;

    // FTP-Verbindung: Datei auslesen
    case GZM_FTP_GET_LINE_BY_LINE:
      if (boZMNeu){
        rbyGsmSmSubState = 0;
      }
      else{
      }
      switch (rbyGsmSmSubState){
        case 0:
         break;
      }
      break;

    // FTP-Verbindung: Datei auslesen
    case GZMFTDL:
//      Serial.print(F("_"));
      if (!lboZMKom){
        if (strZMKomAus == ""){
          // Auslesen der Datei beginnen
          strZMKomAus = F("AT+FTPGET=1");
        }
        else if (strZMKomAus.indexOf(F("GET")) >= 0){        
          // Datei auslesen
          strZMKomAus = F("AT+FTPGET=2,32");
        }
        // Überwachungszeiten vergrößern
        byZMKomAZt = 20;
        byZMZt = 100;
      }
      else if (lstrKomEin.indexOf(F("+FTPGET:1,0")) >= 0){
#ifdef DEBUG_MAWU
        Serial.println(F("closed"));
#endif
        // Datenübertragung abgeschlossen
        if (lboDatBL){
          lboDatBL = false;
        }
        else{
          lboDatAL = false;
        }
//        leModZMNeu = GZMFTWD;
        leModZMNeu = GZM_FTP_GET_RENAME;
      }
      else if (lboKomEin && (lstrKomEin.indexOf(F("OK")) >= 0)){
//        Serial.print(F("."));
        if ((lstrKomEin.indexOf(F(":1,1")) >= 0)){
          // Datentransfer gestartet / abgeschlossen
          boZMSR = true;    // Zustandsmaschine zurücksetzen, wozu eigentlich?
        }
        else if ((lstrKomEin.indexOf(F(":1,")) >= 0) && strZMKomAus.endsWith(F("FTPGET=1"))){
          // Fehler beim Öffnen der Datei
#ifdef DEBUG_MAWU
          Serial.println(F("Fehler"));
#endif
          if (lboDatBL){
            lboDatBL = false;
          }
          else{
            lboDatAL = false;
          }
          leModZMNeu = GZMFTWD;          
        }
        else if (lstrKomEin.indexOf(F("GET:2,0")) >= 0){
          // no more data
//          Serial.println(F("finished"));
          // do nothing, because waiting for "+FTPGET:1,0"
          // leModZMNeu = GZMFTWD;
        }
        else if (lstrKomEin.indexOf(F("GET:2,")) >= 0){
          boZMSR = true;    // Zustandsmaschine zurücksetzen to read next data, if there is one
          // Neue Datenerhalten - Dateiinhalt extrahieren
          if (lstrKomEin.indexOf(F("ID")) >= 0){
            // erste Zeile
            if (lstrKomEin.indexOf(F("1\t")) >= 0){
              lstrFTP = lstrKomEin.substring(lstrKomEin.indexOf(F("1\t")), lstrKomEin.length() - 6);
              
              liEDID = 0;
              liIDID = 0;
              EepromWriteFtpId();
//              leModZMNeu = GZM_FTP_GET_CLOSE;
            }
            else{
              // keine Nummern in der Datei
              if (lboDatBL){
                lboDatBL = false;
              }
              else{
                lboDatAL = false;
              }
              leModZMNeu = GZMFTWD;
            }
          }
          else{
              // nicht erste Zeile, also nicht erster Datensatz
              lstrKomEin = lstrKomEin.substring(lstrKomEin.indexOf(F("+FTPGET:2")), lstrKomEin.length());
#ifdef DEBUG_MAWU
//              Serial.print (F("KomEin: ")); Serial.println(lstrKomEin);
#endif
              lstrKomEin = lstrKomEin.substring(lstrKomEin.indexOf(F("\n"))+1, lstrKomEin.length());
#ifdef DEBUG_MAWU
//              Serial.print (F("KomEin ltrim: ")); Serial.println(lstrKomEin);
#endif
              lstrKomEin = lstrKomEin.substring(0, lstrKomEin.length() - 6);
#ifdef DEBUG_MAWU
//              Serial.print (F("KomEin rtrim: ")); Serial.println(lstrKomEin);
#endif
              lstrFTP = lstrFTP + lstrKomEin;
              EepromWriteFtpId();
          }
        }
      }
      else if (boZMZt)
      {
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZMFTWD;
      }        
      break;      

    case GZM_FTP_GET_CLOSE:
      if (boZMNeu){
        // Überwachungszeiten vergrößern
        byZMKomAZt = 50;
        byZMZt = 150;
      }
      if (!lboZMKom){
        // Datentransfer beenden
        strZMKomAus = F("AT+FTPGET=2,256");
      }
      else if (lboATOK){
        if (lstrKomEin.indexOf(F("+FTPGET:1,0")) >= 0){
#ifdef DEBUG_MAWU
          Serial.println(F("closed"));
#endif
          // Datenübertragung abgeschlossen
          leModZMNeu = GZMFTND;          
        }
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZMFTWD;
      }        
      break;

    case GZM_FTP_GET_RENAME:
      if (boZMNeu){
        // Überwachungszeiten vergrößern
        byZMKomAZt = 50;
        byZMZt = 150;
      }
      if (!lboZMKom){
        // File which was read before to be renamed
        strZMKomAus = F("AT+FTPRENAME");
      }
      else if (lboATOK){
        if (lstrKomEin.indexOf(F("+FTPRENAME:1,0")) >= 0){
#ifdef DEBUG_MAWU
          Serial.println(F("renamed"));
#endif
          // Datenübertragung abgeschlossen
          leModZMNeu = GZMFTWD;          
        }
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZMFTWD;
      }        
      break;

      
    case GZM_FTP_QUIT:
      if (boZMNeu){
        // Überwachungszeiten vergrößern
        byZMKomAZt = 50;
        byZMZt = 150;
      }
      if (!lboZMKom){
        // Datentransfer beenden
        strZMKomAus = F("AT+FTPQUIT");
      }
      else if (lboATOK){
        if (lstrKomEin.indexOf(F("+FTP")) >= 0){
#ifdef DEBUG_MAWU
          Serial.println(F("quit"));
#endif
          // Datenübertragung abgeschlossen
          leModZMNeu = GZMFTWD;          
        }
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZMFTWD;
      }        
      break;

    // FTP-Verbindung: Neue Daten vom Server gelesen (Benutzerdaten)
    case GZMFTND:
      if ((liEDID == 0) && (liIDID == 0))
      {
#ifdef DEBUG_MAWU
        Serial.print(F("FTP-String: "));
        Serial.println(lstrFTP);
#endif
        if (lboDatBL)
        {
          lubyTyp = 'B';
        }
        else
        {
          lubyTyp = 'A';
        }
      }    
      
      if (liIDID < EEPROM_AnzEintraege(lubyTyp))
      {
        // interne Daten prüfen
        liIDID++;

        // Eintrag lesen und vergleichen
        if (lstrFTP.indexOf(EEPROM_LiesEintrag(lubyTyp, liIDID).substring(1)) < 0)
        {
          // Eintrag entfernen, da er nicht vorhanden ist
          EEPROM_EntfEintrag(lubyTyp, liIDID);
          liIDID--;
#ifdef  DEBUG_MAWU
          Serial.println((String)F("Eintrag entfernt: ") + String(liIDID));
#endif
        }        
      }
      else
      {
        // externe Daten einfügen, Position des ersten Datensatzes extrahieren
        liEDID = lstrFTP.indexOf(F("\t"), liEDID + 1) + 1;
        EEPROM_NeuEintrag(lubyTyp, lstrFTP.substring(liEDID, liEDID + 11));
        liIDID++;
        
#ifdef  DEBUG_MAWU        
        Serial.print(F("FTP-SubString:"));
        Serial.println(lstrFTP.substring(liEDID, liEDID + 11));
#endif        
        
        // Prüfen, ob alle Einträge bearbeitet wurden
        if (lstrFTP.indexOf(F("\t"), liEDID) < 0)
        {
          if (lboDatBL)
          {
            lboDatBL = false;
          }
          else
          {
            lboDatAL = false;
          }
          leModZMNeu = GZMFTWD;
        } 
      }      
      break;
      

    // Serververbindung: aufbauen
    case GZMSEVE:
      if (!lboZMKom)
      {
        strZMKomAus = F("AT+CIPSTART=\"TCP\",\"wp12069872.server-he.de\",\"3306\"");
      }
      else if ((lboKomEin) && (lstrKomEin.indexOf(F("CONNECT OK")) >= 0))
      {
        leModZMNeu = GZMSEDA;
      }
      else if (boZMZt)
      {
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }           
      break;

    // Serververbindung: offen / Datenaustausch
    case GZMSEDA:      
      // konstant auf Dateneingang prüfen
      strZMKomAus = "";
      lboZMKom = true;
      if (lboKomEin)
      {       
        // Verbindungsverlust erkennen
        if ((!lboKomBin) && (lstrKomEin.indexOf(F("CLOSED")) >= 0))
        {
          leModZMNeu = GZMSEVV;
        }

        // neue Daten auswerten

        // Datenlänge rücksetzen
        luwGsmUartRxDataSize = 0;
        ruwGsmUartRxBufferIndex = 0;    // !!! JoWu
        // Merker der binären Daten löschen
        lboKomBin = false;

        // Protokolleingang rücksetzen
        lboKomEin = false;
      }       
      break;

    // Serververbindung: Verbindungsverlust
    case GZMSEVV:
        // erneuter Verbindungsaufbau
        leModZMNeu = GZMSEVE;
      break;

    // unbekannter Schritt
    case GZM_UNBE:
      break;

    // ungültig
    default:
      leModZMNeu = GZM_UNBE;
      break;    
  }

  //
  // Daten senden
  if (!lboZMKom && (strZMKomAus.length() > 0)){
    Serial3.println(strZMKomAus);
#ifdef DEBUG_ECHO_CMD
    Serial.println(F("-> ") + strZMKomAus);
#endif
    rulTxTimeStamp = millis();
    lboZMKom = true;    // we sent a protocol to the GSM Module, so we are waiting for an answer
  }
  else if (rboSendFTPData & !rboSentFTPData){
    rboSentFTPData = true;
    // send FTP-Data
    Serial3.println(lstrFTP);
#ifdef DEBUG_ECHO_CMD
    Serial.println(F("-> ") + lstrFTP);
#endif    
  }
}

// Dateinamen für die FTP Zugriffe setzen
// strDatei = Dateiname
// ACHTUNG: Es wird keine Überprüfung auf gültige Zeichen durchgeführt!
void GPRS_SetzeDateiname(String strDatei){
  lstrDatei = strDatei + (String)F(".txt");
  lboDatei = true;
}

// Neuen Eintrag zum Hochladen auf den Server setzen
// strEintrag = Hochzuladener Eintrag
// ACHTUNG: wenn der Speicher nicht transferiert wurde, werden die Daten nicht mehr übernommen!
void GPRS_Logeintrag(String strEintrag)
{
//  return;
  int iH = (liESID + 1) % GPRS_LOGBUF_SIZE;

  // Prüfung, ob Einträge gespeichert werden dürfen
  if (iH != liETID)
  {
    strEintrag.toCharArray(lfstrEintrag[liESID],GPRS_LOGBUF_SIZE_ENTRY);
    liESID = iH;
  }
}

// APN umstellen
// iAPN = APN Einstellung (1 = HoT, 2 = T-Mobile, X = T-Mobile M2M)
void GPRS_APN(int iAPN)
{
  liAPN = iAPN;
}

// Merker Datei vom FTP herunterladen setzen
// ubyDatei = gewünschte Datei
void GPRS_DateiLesen(char ubyDatei)
{
  switch(ubyDatei)
  {
    case 'B':
      // Benutzerdatei herunterladen
      lboDatBL = true;
      break;

    case 'A':
      // Artikeldatei herunterladen
      lboDatAL = true;
      break;      
  }
}


void EepromWriteFtpId(void){
#ifdef DEBUG_MAWU
    Serial.print(F("FTP-String: (")); Serial.print(lstrFTP); Serial.println(")");
#endif
  if ((liEDID == 0) && (liIDID == 0)){
    if (lboDatBL){
      lubyTyp = 'B';
    }
    else{
      lubyTyp = 'A';
    }
  }    
  
  while((lstrFTP.indexOf(F("\t")) >= 0) && (lstrFTP.length() > (lstrFTP.indexOf(F("\t")) + 11))){
    lstrFTP = lstrFTP.substring(lstrFTP.indexOf(F("\t"))+1);
#ifdef DEBUG_MAWU
    Serial.print(F("Index: ")); Serial.print(liIDID+1); Serial.print(F(" ")); Serial.println(lstrFTP.substring(0, 11));
    Serial.print(EEPROM_LiesEintrag(lubyTyp, liIDID+1).substring(1,12));
#endif
    while(liIDID < EEPROM_AnzEintraege(lubyTyp) && !lstrFTP.substring(0,11).equals(EEPROM_LiesEintrag(lubyTyp, liIDID+1).substring(1,12))){
      EEPROM_EntfEintrag(lubyTyp, liIDID+1);
      Serial.println(F(" wrong"));
#ifdef  DEBUG_MAWU
      Serial.println(F("Eintrag entfernt: ") + String(liIDID));
#endif
    }
    if (liIDID >= EEPROM_AnzEintraege(lubyTyp) || !lstrFTP.substring(0,11).equals(EEPROM_LiesEintrag(lubyTyp, liIDID+1).substring(1,12))){
      EEPROM_NeuEintrag(lubyTyp, lstrFTP.substring(0, 11));
#ifdef  DEBUG_MAWU
      Serial.println((String)F("Eintrag eingetragen: ") + String(liIDID) + " " + lstrFTP.substring(0, 11));
#endif
    }
    liIDID++;
/*
    while (liIDID < EEPROM_AnzEintraege(lubyTyp)){
      // interne Daten prüfen
      liIDID++;

      // Eintrag lesen und vergleichen
      while (lstrFTP.substring(0,11) != EEPROM_LiesEintrag(lubyTyp, liIDID).substring(1)) < 0){
      // Eintrag entfernen, da er nicht vorhanden ist
      liIDID--;
    }        
  }
  else{
    // externe Daten einfügen, Position des ersten Datensatzes extrahieren
    liEDID = lstrFTP.indexOf("\t", liEDID + 1) + 1;
    EEPROM_NeuEintrag(lubyTyp, lstrFTP.substring(liEDID, liEDID + 11));
    liIDID++;



*/
  }
  return;
  if (liIDID < EEPROM_AnzEintraege(lubyTyp)){
    // interne Daten prüfen
    liIDID++;

    // Eintrag lesen und vergleichen
    if (lstrFTP.indexOf(EEPROM_LiesEintrag(lubyTyp, liIDID).substring(1)) < 0){
      // Eintrag entfernen, da er nicht vorhanden ist
      EEPROM_EntfEintrag(lubyTyp, liIDID);
      liIDID--;
#ifdef  DEBUG_MAWU
      Serial.println((String)F("Eintrag entfernt: ") + String(liIDID));
#endif
    }        
  }
  else{
    // externe Daten einfügen, Position des ersten Datensatzes extrahieren
    liEDID = lstrFTP.indexOf("\t", liEDID + 1) + 1;
    EEPROM_NeuEintrag(lubyTyp, lstrFTP.substring(liEDID, liEDID + 11));
    liIDID++;
    
#ifdef  DEBUG_MAWU        
    Serial.print(F("FTP-SubString:"));
    Serial.println(lstrFTP.substring(liEDID, liEDID + 11));
#endif        
    
    // Prüfen, ob alle Einträge bearbeitet wurden
    if (lstrFTP.indexOf("\t", liEDID) < 0){
      if (lboDatBL){
        lboDatBL = false;
      }
      else{
        lboDatAL = false;
      }
      leModZMNeu = GZMFTWD;
    } 
  }
  
}
