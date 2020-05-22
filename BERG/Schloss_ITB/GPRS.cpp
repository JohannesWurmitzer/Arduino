/*
 Name:    GPRS.cpp
 Datum:   17.05.2018
 Autor:   Markus Emanuel Wurmitzer

  Versionsgeschichte:
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
  2020-05-22  JoWu  State GZMFTDA aufteilen
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

/*
 * externe Dateien
 */

// remanente Variablen
static GPRS_ZM leModZM = GZM_UNBE;    // GSM-Module Zustandsmaschine, aktiver Schritt
static GPRS_ZM leModZMNeu = GZM_INIT; // GSM-Module Zustandsmaschine, nächster Schritt

static bool lboKomBin;                // binäre Kommunikation ist aktiv
static byte lfbyKomEin[512];          // binäre Kommunikation Eingangspuffer
static int liKomEinL = 0;             // länge der binären Eingangsdaten
static String lstrKomEin;             // ASCII-Text Kommunikation Eingangsdaten          
static byte lbyKomZt;                 // Überwachungszeit Kommunikationseingang
static bool lboKomEin;                // Protokolleingang erkannt

static String lstrDatei;              // Dateiname für den FTP-Zugriff
static bool lboDatei;                 // Dateiname hat sich geändert

static String lfstrEintrag[20];       // Ringpuffer Daten für die Übertragung zum Server
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
      lfbyKomEin[liKomEinL++] = gbyGSMModuleLatestRxByte;
    }
    else{
      // ASCII Text
#ifdef DEBUG_ECHO
      Serial.print((char)gbyGSMModuleLatestRxByte);
#endif
      lstrKomEin += char(gbyGSMModuleLatestRxByte);
    }

    // binäre Kommunikation aktivieren    !!!JoWu should be moved to statemachine
    if ((leModZM == GZMSEVE) && (lstrKomEin.endsWith("CONNECT OK\r\n"))){
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
  
  // Schrittkette Modem
  if ((leModZM != leModZMNeu) || boZMSR){
    // Neuer Schritt oder Schritt zurückgesetzt
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
    lstrKomEin = "";        // Eingangsprotokolldaten
    lboKomEin = false;      // Eingangsprotokoll erkannt
    lbyKomZt = 0;           // Kommunikationszeit Eingangsprotokollende
    
    byZMZt = 20;            // Standardüberwachungszeit pro Schritt in Zyklen !!!JoWu gefährlich bei Verwendung einer anderen Zeit im Schritt!!!
    //Serial.println("Neuer Schritt " + String(leModZM));
  }
  else{
    // Schrittzeit reduzieren, Zustandsmaschinenschritt Timeout generieren
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
    if (lboZMKom && lstrKomEin.endsWith("OK\r\n")){
      lboATOK = 1;
    }
    else if (lboZMKom && lstrKomEin.endsWith("ERROR\r\n")){
      lboATERROR = 1;
    }
  }
  //
  // Zustandsmaschine
  //
  switch(leModZM)
  {
    // Initialisierung
    case GZM_INIT:
      if (!lboZMKom){
        strZMKomAus = "ATE0";       // JoWu: Echo 1 seams to be important now for functionality
      }
      else if (lboATOK){
        leModZMNeu = GZMIKOM;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT_PWRKEY_ON;
      }
      break;
  
    // Initialisierung: Baudrate prüfen
    case GZMIKOM:
      if (!lboZMKom){
        strZMKomAus = "AT+IPR?";
      }
      else if (lboATOK){
        leModZMNeu = GZMIFUN;
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break;

    // Initialisierung: Funktionalität setzen
    case GZMIFUN:
      if (!lboZMKom){
        strZMKomAus = "AT+CFUN=1";
      }
      else if (lboATOK){
        leModZMNeu = GZMSIMP;
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
      
    // SIM-Status abfragen
    case GZMSIMP:
      if (!lboZMKom){
        strZMKomAus = "AT+CPIN?";
      }
      else if (lboATOK && lstrKomEin.startsWith("\r\n+CPIN: READY")){
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
        strZMKomAus = "AT+CSQ";
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
        strZMKomAus = "AT+CREG?";
      }
      else if (lboKomEin){
        if (lboATOK && lstrKomEin.startsWith("\r\n+CREG: 0,1") || lstrKomEin.startsWith("\r\n+CREG: 0,5")){
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
        strZMKomAus = "AT+CGATT?";
      }
      else if (lboATOK && lstrKomEin.startsWith("\r\n+CGATT: 1")){
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
          strZMKomAus = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"";    // \" inserts " into the string
        }
        else if (strZMKomAus.indexOf("CONTYPE") >= 0){
          if (liAPN == 1){
            // HoT
            strZMKomAus = "AT+SAPBR=3,1,\"APN\",\"webaut\"";
          }
          else
          {
            // Standardzugang T-Mobile
            strZMKomAus = "AT+SAPBR=3,1,\"APN\",\"gprsinternet\"";
          }
        }
        else if (strZMKomAus.indexOf("APN") >= 0)
        {
          if (liAPN == 1)
          {
            strZMKomAus = "AT+SAPBR=3,1,\"USER\",\"\"";  
          }
          else
          {
            strZMKomAus = "AT+SAPBR=3,1,\"USER\",\"t-mobile\"";
          }
        }
        else if (strZMKomAus.indexOf("USER") >= 0)
        {
          if (liAPN == 1)
          {
            strZMKomAus = "AT+SAPBR=3,1,\"PWD\",\"\"";
          }
          else
          {
            strZMKomAus = "AT+SAPBR=3,1,\"PWD\",\"tm\"";
          }
        }
      }
      else if (lboATERROR){
        // Modul neu starten
        leModZMNeu = GZM_INIT_PWRKEY_ON;
      }
      else if (lboATOK){
        if (strZMKomAus.indexOf("PWD") >= 0){
          // Verbindungsaufbau beginnen
          leModZMNeu = GZMFHVE; //GZM_FH_PS_W;
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

    // FTP/HTTP Parameter Set Wait
    case GZM_FH_PS_W:
      if (boZMNeu){
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZMFHVE;           // Verbindungsaufbau beginnen
      }        
      break;

    // GPRS FTP / HTTP: Verbindung aufbauen
    case GZMFHVE:
      if (!lboZMKom){
        strZMKomAus = "AT+SAPBR=1,1";
        // Überwachungszeiten vergrößern
        byZMKomAZt = 30;
        byZMZt = 50;
      }
      else if (lboATERROR){
        // Verbindungsversuch gescheitert, Trennvorgang versuchen
        leModZMNeu = GZMFHVT;
        Serial.println("fault");
      }
      else if (lboATOK){
        // Verbindungsversuch erfolgreich
        leModZMNeu = GZMFTKS;
        Serial.println("success");
      }
      else if (boZMZt){
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZM_INIT;
      }
      break;

    // GPRS FTP / HTTP: Verbindung trennen
    case GZMFHVT:
      if (!lboZMKom){
          strZMKomAus = "AT+SAPBR=0,1";
          // Überwachungszeiten vergrößern
          byZMKomAZt = 30;
          byZMZt = 50;
      }
      else if (lboATOK || lboATERROR){
        // neuer Verbindungsversuch
        leModZMNeu = GZMFHVE;
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
        strZMKomAus = "AT+CSTT=\"gprsinternet\",\"t-mobile\",\"tm\"";
      }
      else if (lboKomEin)
      {
        if (lstrKomEin.indexOf("OK") >= 0)
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
        strZMKomAus = "AT+CIPSHUT";         
      }    
      else if (lboKomEin && (lstrKomEin.indexOf("OK") >= 0))
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
          strZMKomAus = "AT+CIICR";
        }
        else if (strZMKomAus.indexOf("CIICR") >= 0)
        {
          // IP-Adresse abfragen
          strZMKomAus = "AT+CIFSR";
        }
      }
      else if (lboKomEin)
      {
        if ((strZMKomAus.indexOf("CIFSR") >= 0) && (lstrKomEin.indexOf("ERROR") < 0))
        {
          //  TCP Verbindung öffnen
          leModZMNeu = GZMSEVE;
        }
        else if (lstrKomEin.indexOf("OK") >= 0)
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
    case GZMFTKS:
      if (!lboZMKom){
        if (strZMKomAus == ""){
          // FTP-Profil aktivieren
          strZMKomAus = "AT+FTPCID=1";
        }
        else if (strZMKomAus.indexOf("CID") >= 0){
          // Serveradresse einstellen
          strZMKomAus = "AT+FTPSERV=\"wp011.webpack.hosteurope.de\"";
        }
        else if (strZMKomAus.indexOf("SERV") >= 0){
          // Serverschnittstelle eintragen
          strZMKomAus = "AT+FTPPORT=21";
        }
        else if (strZMKomAus.indexOf("PORT") >= 0){
          // Benutzernamen eintragen
          strZMKomAus = "AT+FTPUN=\"ftp12069872-martin\"";
        }
        else if (strZMKomAus.indexOf("PUN") >= 0){
          // Passwort eintragen
          strZMKomAus = "AT+FTPPW=\"ai901!MK\"";
        }
      }
      else if (lboATOK){
        if (strZMKomAus.indexOf("PW") >= 0){
          // Dateikonfiguration vornehmen
          leModZMNeu = GZMFTKD;          
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
    case GZMFTKD:
      if (!lboZMKom){
        if (strZMKomAus == ""){
          // Zugriffsart "anhängen"
          strZMKomAus = "AT+FTPPUTOPT=\"APPE\"";
        }
        else if (strZMKomAus.indexOf("OPT") >= 0){
          // Dateinamen eintragen
          strZMKomAus = "AT+FTPPUTNAME=\"" + lstrDatei + "\"";         
        }
        else if (strZMKomAus.indexOf("NAME") >= 0){
          // Verzeichnis eintragen
          strZMKomAus = "AT+FTPPUTPATH=\"/Daten/\"";            
        }
      }
      else if (lboATOK){
        if (strZMKomAus.indexOf("PATH") >= 0){
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
        leModZMNeu = GZMFTKD;
      }
      else if (boZMZt){
        // Zeit abgelaufen, auf neue Daten prüfen
        if (liESID != liETID){
          // Daten für den FTP Server vorhanden
          //Serial.println("FTP Daten vorhanden: " + String(liESID) + "/" + String(liETID));       
          lstrFTP = "";
          while(liESID != liETID){
            lstrFTP += lfstrEintrag[liETID++];
            liETID = liETID % 20;
          }
          //Serial.println("FTP Daten: " + lstrFTP);
          leModZMNeu = GZMFTDA;
        }
        else if (lboDatBL || lboDatAL){
          // Benutzerdatei auslesen
          leModZMNeu = GZMFTKL;
        }
        else{
          // Schritt neu starten
          boZMSR = true;
        }
      }
      break;


    // FTP-Verbindung: Datenübertragung durchführen
    case GZMFTDA:
      if (!lboZMKom){
        if (strZMKomAus == ""){
          // Datentransfer zur Datei beginnen
          strZMKomAus = "AT+FTPPUT=1";  
        }
        else if (strZMKomAus.indexOf("PUT=1") >= 0){
          // Einträge zusammenfassen
          
          // Einträge übertragen
          strZMKomAus = "AT+FTPPUT=2," + String(lstrFTP.length());
        }
        else if (strZMKomAus.indexOf("PUT=2") >= 0)
        {
          //Datenübertragung
          strZMKomAus = lstrFTP;
        }
        else
        {
          // Datentransfer beenden
          strZMKomAus = "AT+FTPPUT=2,0";
        }
        // Überwachungszeiten vergrößern
        byZMKomAZt = 50;
        byZMZt = 150;
      }
      else if (lboKomEin){
        if ((lstrKomEin.indexOf("OK") >= 0) && (lstrKomEin.indexOf(":1,1,") >= 0))
        {
          // Datentransfer gestartet / abgeschlossen
          boZMSR = true;
        }
        else if (lstrKomEin.indexOf(":2,") >= 0)
        {
          // Datentransfer durchzuführen / durchgeführt
          boZMSR = true;
        }
        else if (lstrKomEin.indexOf(":1,0") >= 0)
        {
          // Datenübertragung abgeschlossen
          leModZMNeu = GZMFTWD;          
        }
      }
      else if (boZMZt)
      {
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZMFTWD;
      }        
      break;

    // FTP-Verbindung: Konfiguration Lesedatei (Benutzer- / Artikelliste)
    case GZMFTKL:
      if (!lboZMKom)
      {
        if (strZMKomAus == "")
        {
          // Dateinamen eintragen
          String strDat = lstrDatei;
          if (lboDatBL)
          {
            strDat.replace("txt","ben");
          }
          else
          {
            strDat.replace("txt","art");
          }
          strZMKomAus = "AT+FTPGETNAME=\"" + strDat + "\"";         
        }
        else if (strZMKomAus.indexOf("NAME") >= 0)
        {
          // Verzeichnis eintragen
          if (lboDatBL)
          {
            strZMKomAus = "AT+FTPGETPATH=\"/Benutzer/\"";            
          }
          else
          {
            strZMKomAus = "AT+FTPGETPATH=\"/Artikel/\"";
          }
        }
      }
      else if (lboKomEin && (lstrKomEin.indexOf("OK") >= 0))
      {
        if (strZMKomAus.indexOf("PATH") >= 0)
        {
          // Datenübertragung durchführen
          leModZMNeu = GZMFTDL;
          lboDatei = false;  
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
    case GZMFTDL:
      if (!lboZMKom)
      {
        if (strZMKomAus == "")
        {
          // Auslesen der Datei beginnen
          strZMKomAus = "AT+FTPGET=1";  
        }
        else if (strZMKomAus.indexOf("GET=1") >= 0)
        {        
          // Datei auslesen
          strZMKomAus = "AT+FTPGET=2,1024";
        }
        // Überwachungszeiten vergrößern
        byZMKomAZt = 20;
        byZMZt = 100;
      }
      else if (lboKomEin && (lstrKomEin.indexOf("OK") >= 0))
      {       
        if ((lstrKomEin.indexOf(":1,1") >= 0))
        {
          // Datentransfer gestartet / abgeschlossen
          boZMSR = true;
        }
        else if ((lstrKomEin.indexOf(":1,") >= 0) && strZMKomAus.endsWith("FTPGET=1"))
        {
          // Fehler beim Öffnen der Datei
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
        else if ((lstrKomEin.indexOf("OK") >= 0) && (lstrKomEin.indexOf("ID") >= 0))
        {
          // Datenübertragung abgeschlossen - Dateiinhalt extrahieren
          if (lstrKomEin.indexOf("1\t") >= 0)
          {
            lstrFTP = lstrKomEin.substring(lstrKomEin.indexOf("1\t"), lstrKomEin.length() - 5);
            liEDID = 0;
            liIDID = 0;
            leModZMNeu = GZMFTND;          
          }
          else
          {
            // keine Nummern in der Datei
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
      }
      else if (boZMZt)
      {
        // Zeitüberwachung hat ausgelöst
        leModZMNeu = GZMFTWD;
      }        
      break;      

    // FTP-Verbindung: Neue Daten vom Server gelesen (Benutzerdaten)
    case GZMFTND:
      if ((liEDID == 0) && (liIDID == 0))
      {
#ifdef DEBUG_MAWU
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
          Serial.println("Eintrag entfernt: " + String(liIDID));
#endif
        }        
      }
      else
      {
        // externe Daten einfügen, Position des ersten Datensatzes extrahieren
        liEDID = lstrFTP.indexOf("\t", liEDID + 1) + 1;
        EEPROM_NeuEintrag(lubyTyp, lstrFTP.substring(liEDID, liEDID + 11));
        liIDID++;
        
#ifdef  DEBUG_MAWU        
        Serial.println(lstrFTP.substring(liEDID, liEDID + 11));
#endif        
        
        // Prüfen, ob alle Einträge bearbeitet wurden
        if (lstrFTP.indexOf("\t", liEDID) < 0)
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
        strZMKomAus = "AT+CIPSTART=\"TCP\",\"wp12069872.server-he.de\",\"3306\"";
      }
      else if ((lboKomEin) && (lstrKomEin.indexOf("CONNECT OK") >= 0))
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
        if ((!lboKomBin) && (lstrKomEin.indexOf("CLOSED") >= 0))
        {
          leModZMNeu = GZMSEVV;
        }

        // neue Daten auswerten

        // Datenlänge rücksetzen
        liKomEinL = 0;

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


  // Daten senden
  if (!lboZMKom && (strZMKomAus.length() > 0)){
    Serial3.println(strZMKomAus);
#ifdef DEBUG_ECHO_CMD
    Serial.println("-> " + strZMKomAus);
#endif    
    lboZMKom = true;
  }
   
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
        if (lstrKomEin.length() > 0)
        {        
          Serial.print("<- " + lstrKomEin);
        }
        // Protokoll ausgeben - binäre Daten als hexadezimale Zeichenkette
        else if (liKomEinL > 0)
        {
          Serial.print("<-");
          for (int i = 0; i < liKomEinL; i++)
          {
            Serial.print(" ");
            if (lfbyKomEin[i] < 16)
            {
              Serial.print("0");
            }
            Serial.print(String(lfbyKomEin[i], HEX));
          }
          Serial.println();
        }
#endif
      }
    }
  }  
}

// Dateinamen für die FTP Zugriffe setzen
// strDatei = Dateiname
// ACHTUNG: Es wird keine Überprüfung auf gültige Zeichen durchgeführt!
void GPRS_SetzeDateiname(String strDatei)
{
  lstrDatei = strDatei + ".txt";
  lboDatei = true;
}

// Neuen Eintrag zum Hochladen auf den Server setzen
// strEintrag = Hochzuladener Eintrag
// ACHTUNG: wenn der Speicher nicht transferiert wurde, werden die Daten nicht mehr übernommen!
void GPRS_Logeintrag(String strEintrag)
{
  int iH = (liESID + 1) % 20;

  // Prüfung, ob Einträge gespeichert werden dürfen
  if (iH != liETID)
  {
    lfstrEintrag[liESID] = strEintrag;
    liESID = iH;
  }
}

// APN umstellen
// iAPN = APN Einstellung (1 = HoT, X = T-Mobile)
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
