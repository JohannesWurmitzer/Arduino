/*
  Copyright (c) 2017, E.C.E. Wurmitzer GmbH
  All rights reserved.
*/

/*
  File Name:      EepromAndRFID_IDs.h
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check EEPROM size!
  Created by:     Edmund Titz
  Created on:     2017-11-15
  derived from:   

  ACHTUNG: Bei einem Überlauf der EEPROM-Adresse wird wieder ab 0 geschrieben!

  Versionsgeschichte:
  18.07.2018  V 103   Markus Emanuel Wurmitzer
    - Parameter "APN" eingefügt
    - Parameterbeschreibung ergänzt (Rückfahrzeit hat gefehlt)
    
  08.05.2018  V 102   Markus Emanuel Wurmitzer
    - Paramterbereich eingefügt (variable Überwachungszeiten für die Motorbewegung)
    - EEPROM-Zeile (= 16 Bytes ab Adresse X) auslesen
    
  02.05.2018  V 101   Markus Emanuel Wurmitzer
    - Neue Funktionen zur Statusabfrage eingefügt
    - Neue Funktion zum Status setzen eingefügt
    - Speicher des EEPROMs angepasst für die Statusabfrage
  
  23.04.2018  V 100   Markus Emanuel Wurmitzer
    - Neue Funktionen für die Kommunikation
    - Speicherbelegung angepasst (Seriennummer 4085 bis 4095)
*/

/*  todo-list
  - do not know...
*/

/*
  Includes
*/
#include "Arduino.h"
#include <EEPROM.h>
//Memory map:
/*adr.: 0 to 19 EEPROM Header
  adr.: 20 to 4019 EEPROM ID entries
                   20 to 3519 user
                   3520 to 4019 articles

  Adr.: 4020 = letzter Status des Schlosses
  Adr.: 4030 = Länge Benutzer letzter Zugriff
  Adr.: 4031 bis 4037 = Benutzer
  Adr.: 4040 = Länge Artikel letzter Zugriff
  Adr.: 4041 bis 4047 = Artikel
  Adr.: 4050 bis 4079 = Parameter
          4050 = Überwachungszeit Blockade beim Schließen (16 Bit ohne Vorzeichen)
          4052 = Überwachungszeit Blockade beim Öffnen (16 Bit ohne Vorzeichen)
          4054 = Rückfahrzeit Schloss (8 Bit ohne Vorzeichen)
          4055 = mobiles Internet APN Einstellung (8 Bit ohne Vorzeichen)
  Adr.: 4086 bis 4095 Seriennummer (wird nicht gelöscht beim Rücksetzen!)
 */
 
/*
  Macros / Defines
*/
#ifndef EEPROMANDRFID_IDS_H_
#define EEPROMANDRFID_IDS_H_

#ifdef __cplusplus
extern "C"{
#endif
	
//general
#define NONE                    0
#define NOT_APPLICABLE          0

#define HEADER_EXIST_CHECKVAL1  0xAA55
#define HEADER_EXIST_CHECKVAL2  0xCC33
#define ONLY_ENTRY_PART         0
#define COMPLETE_HEADER         1
#define INDICATE_USER           0x55//ascii "U" (= dec. 85)
#define INDICATE_ARTICLE        0x41//ascii "A" (= dec. 65)

//hardcoded master IDs (needed to trigger some EEPROM functions like adding a new user)
#define MASTER_ID_TEACH_USER    {{7,0x04,0x2D,0x7E,0xF2,0x20,0x1D,0x80},{7,0x04,0x32,0x7E,0xF2,0x20,0x1D,0x80},{7,0x04,0x4B,0x72,0xBA,0x1F,0x1D,0x80}}
#define MASTER_ID_TEACH_ARTICLE {{7,0x04,0x1E,0x7E,0xF2,0x20,0x1D,0x80},{7,0x04,0x1F,0x7E,0xF2,0x20,0x1D,0x80},{7,0x04,0x36,0x72,0xBA,0x1F,0x1D,0x80}}
#define MASTER_ID_ERASE_IDS     {{7,0x04,0x3A,0x7E,0xF2,0x20,0x1D,0x80},{7,0x04,0x33,0x7E,0xF2,0x20,0x1D,0x80},{7,0x04,0x39,0x72,0xBA,0x1F,0x1D,0x80}}

//master ID return values
#define NO_MASTER               0
#define TEACH_USER_ID_MASTER    1
#define TEACH_ARTICLE_ID_MASTER 2
#define ERASE_IDS_MASTER        3

/*
  Typedefs
*/
struct TyDe_EEPROM_HeaderEntryPart{//EEPROM address 8 to 11
  unsigned short uwNumOfStoredUserIDs;
  unsigned short uwNumOfStoredArticleIDs; 
};
struct TyDe_EEPROM_Header{//EEPROM address 0 to 19
  unsigned short uwStartAddressUserMem;//350 User
  unsigned short uwStartAddressArticleMem;//50 Articles
  unsigned short uwMaxNumOfUserIDs;
  unsigned short uwMaxNumOfArticleIDs;
  TyDe_EEPROM_HeaderEntryPart tydeHeaderEntryPart;
  unsigned short uwHeaderExistCheck1;
  unsigned short uwHeaderExistCheck2;
  unsigned short uwReservedShortVar1;
  unsigned short uwReservedShortVar2;
};

struct TyDe_EEPROM_ID_Entry{//EEPROM address 20 to 4019
  unsigned char ub_ID_length;
  unsigned char aub_ID[7];
  unsigned char ub_UserOrArticle;//ascii "U" = 0x55, ascii "A" = 0x41
  unsigned char ubReservedByteVar1; 
};

struct TyDe_MasterIdStruct{//hardcoded master IDs
  unsigned char ub_ID_length;
  unsigned char aub_ID[7]; 
};
/*
  Port pin definitions
*/

/*
  Public Variables
*/

/*
  Public Function Prototypes
*/
  extern void checkIf_EEPROM_HeaderExistsElseWrite(void);
	extern void writeEEPROM_Header(TyDe_EEPROM_Header tydeEEPROM_header, unsigned char ubDef_whatToWrite);//should not be needed extern (private function)
  extern TyDe_EEPROM_Header readEEPROM_Header(unsigned char ubDef_whatToRead);//should not be needed extern (private function)
  extern void writeEEPROM_UserID(unsigned char ub_UserID_length, unsigned char* pub_UserID);
  extern void writeEEPROM_ArticleID(unsigned char ub_ArticleID_length, unsigned char* pub_ArticleID);
  extern unsigned char checkUserID(unsigned char ub_UserID_length, unsigned char* pub_UserID);
  extern unsigned char checkArticleID(unsigned char ub_ArticleID_length, unsigned char* pub_ArticleID);
  extern unsigned char checkMasterID(unsigned char ub_MasterID_length, unsigned char* pub_MasterID);
  extern void eraseEEPROM_AndReWriteHeader(void);

  extern short EEPROM_AnzEintraege(unsigned char ubyType);
  extern String EEPROM_LiesEintrag(unsigned char ubyType, int iID);
  extern String EEPROM_NeuEintrag(unsigned char ubyType, String strNummer);
  extern String EEPROM_EntfEintrag(unsigned char ubyType, int iID);
  extern String EEPROM_EntfNummern(unsigned char ubyType);
  extern String EEPROM_SNrLesen(void);
  extern String EEPROM_SNrSchreiben(String strNummer);
  extern String EEPROM_StatusLesen(void);
  extern String EEPROM_ParSchreiben(String strTyp, int iAdr, int iWert);
  extern String EEPROM_ParLesen(String strTyp, int iAdr);
  extern String EEPROM_ZeileLesen(int iAdr);

  extern void EEPROM_Schlossstatus(unsigned char ubyStat);
  extern void EEPROM_LetzterZugriff(unsigned char ubyType, unsigned char ubyIDL, unsigned char* pubyID);
    
#ifdef __cplusplus
} // extern "C"
#endif 

#endif /* EEPROMANDRFID_IDS_H_ */
