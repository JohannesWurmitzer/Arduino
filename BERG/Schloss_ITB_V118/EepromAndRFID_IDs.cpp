/*
  Copyright (c) 2017, E.C.E. Wurmitzer GmbH
  All rights reserved.
*/
/*  Function description
  //Memory map:
  adr.: 0 to 19 EEPROM Header
    adr.: 20 to 4019 EEPROM ID entries
                     20 to 3519 user
                     3520 to 4019 articles
    adr.: 4020 to 4095 reserved
*/
/*
  File Name:      EepromAndRFID_IDs.c
  Target Device:  Arduino Mega 2560 (tested); should work on all other Arduino boards !check EEPROM size!
  Created by:     Edmund Titz
  Created on:     2017-11-15
  derived from:   
*/

/*
 * ACHTUNG: Bei einem Überlauf der EEPROM-Adresse wird wieder ab 0 geschrieben!

  Versionsgeschichte:
  2020-08-16  V 105   JoWu
    - made static variables in functions to local variables to reduce memory usage
      maybe this was used because they saw a risk of returning the structures
  
  18.07.2018  V 104   Markus Emanuel Wurmitzer
    - "LiesEintrag" gibt jetzt für jeden Wert 2-stellige Zahlen aus
  
  18.05.2018  V 103   Markus Emanuel Wurmitzer
    - Seriennummer gibt bei "0xFF" ebenfalls 0 aus, somit hat ein neues Gerät die Seriennummer "0000000000"
    
  08.05.2018  V 102   Markus Emanuel Wurmitzer
    - Paramterbereich eingefügt (variable Überwachungszeiten für die Motorbewegung)
    - EEPROM-Zeile (= 16 Bytes ab Adresse X) auslesen
  
  03.05.2018  V 101   Markus Emanuel Wurmitzer
    - Neue Funktionen zur Statusabfrage eingefügt
    - Neue Funktion zum Status setzen eingefügt
    - Speicher des EEPROMs angepasst für die Statusabfrage
      
  23.04.2018  V 100   Markus Emanuel Wurmitzer
    - Neue Funktionen für die Kommunikation
    - Speicherbelegung angepasst (Seriennummer 4085 bis 4095)
             
  V1.00 2017-11-15  EdTi
    - creation of file
*/

/*  todo-list
  - do not know...
*/

/*
  Includes
*/

#include "EepromAndRFID_IDs.h"
#include "Log.h"
#include "MotorLockHbridge.h"

/*
  Macros / Defines
*/

/*
  Typedefs
*/

/*
  External Variables Protoypes
*/

/*
  Port pin definitions
*/

/*
  External Function Header Prototypes
*/

/*
  Public Variables
*/

/*
  Global Variables (global only in this module [C-File])
*/
 
                                                                      
/*
  Private Function Prototypes
*/
  
/*
  Public Function Prototypes
*/
  void checkIf_EEPROM_HeaderExistsElseWrite();
  void writeEEPROM_Header(TyDe_EEPROM_Header tydeEEPROM_header, unsigned char ubDef_whatToWrite);
  TyDe_EEPROM_Header readEEPROM_Header(unsigned char ubDef_whatToRead);
  void writeEEPROM_UserID(unsigned char ub_UserID_length, unsigned char* pub_UserID);
  void writeEEPROM_ArticleID(unsigned char ub_ArticleID_length, unsigned char* pub_ArticleID);
  unsigned char checkUserID(unsigned char ub_UserID_length, unsigned char* pub_UserID);
  unsigned char checkArticleID(unsigned char ub_ArticleID_length, unsigned char* pub_ArticleID);
  unsigned char checkMasterID(unsigned char ub_MasterID_length, unsigned char* pub_MasterID);
  void eraseEEPROM_AndReWriteHeader();  
/*
 implementation of functions
*/

//***********************************************************************************
void checkIf_EEPROM_HeaderExistsElseWrite(){
  TyDe_EEPROM_Header tyde_ObjHeaderInEEPROM;
  //-------------------------------------------------

  tyde_ObjHeaderInEEPROM = readEEPROM_Header(COMPLETE_HEADER);
  if(tyde_ObjHeaderInEEPROM.uwHeaderExistCheck1 == HEADER_EXIST_CHECKVAL1){
    if(tyde_ObjHeaderInEEPROM.uwHeaderExistCheck2 == HEADER_EXIST_CHECKVAL2){
      //valid header, do nothing  
    }
    else{
      //header is not valid, write header into EEPROM
      tyde_ObjHeaderInEEPROM.uwStartAddressUserMem = 20;//350 User, each user 10 bytes
      tyde_ObjHeaderInEEPROM.uwStartAddressArticleMem = 3520;//50 Articles, each article 10 bytes
      tyde_ObjHeaderInEEPROM.uwMaxNumOfUserIDs = 350;
      tyde_ObjHeaderInEEPROM.uwMaxNumOfArticleIDs = 50;
      tyde_ObjHeaderInEEPROM.tydeHeaderEntryPart.uwNumOfStoredUserIDs = 0;
      tyde_ObjHeaderInEEPROM.tydeHeaderEntryPart.uwNumOfStoredArticleIDs = 0;
      tyde_ObjHeaderInEEPROM.uwHeaderExistCheck1 = HEADER_EXIST_CHECKVAL1;
      tyde_ObjHeaderInEEPROM.uwHeaderExistCheck2 = HEADER_EXIST_CHECKVAL2;
      writeEEPROM_Header(tyde_ObjHeaderInEEPROM, COMPLETE_HEADER);
    }
  }
  else{
    //header is not valid, write header into EEPROM
    tyde_ObjHeaderInEEPROM.uwStartAddressUserMem = 20;//350 User, each user 10 bytes
    tyde_ObjHeaderInEEPROM.uwStartAddressArticleMem = 3520;//50 Articles, each article 10 bytes
    tyde_ObjHeaderInEEPROM.uwMaxNumOfUserIDs = 350;
    tyde_ObjHeaderInEEPROM.uwMaxNumOfArticleIDs = 50;
    tyde_ObjHeaderInEEPROM.tydeHeaderEntryPart.uwNumOfStoredUserIDs = 0;
    tyde_ObjHeaderInEEPROM.tydeHeaderEntryPart.uwNumOfStoredArticleIDs = 0;
    tyde_ObjHeaderInEEPROM.uwHeaderExistCheck1 = HEADER_EXIST_CHECKVAL1;
    tyde_ObjHeaderInEEPROM.uwHeaderExistCheck2 = HEADER_EXIST_CHECKVAL2;
    writeEEPROM_Header(tyde_ObjHeaderInEEPROM, COMPLETE_HEADER);  
  }
}
//***********************************************************************************
void writeEEPROM_Header(TyDe_EEPROM_Header tydeEEPROM_header, unsigned char ubDef_whatToWrite){
  unsigned short luw_ByteAddress;
  //-------------------------------------------------
  switch(ubDef_whatToWrite){
    case ONLY_ENTRY_PART:
      luw_ByteAddress = 8;//Offset in structure for tydeHeaderEntryPart
      EEPROM.put(luw_ByteAddress, tydeEEPROM_header.tydeHeaderEntryPart);
    break;

    case COMPLETE_HEADER:
      luw_ByteAddress = 0;
      EEPROM.put(luw_ByteAddress, tydeEEPROM_header);
    break;

    default:
    break;
  }
}

//***********************************************************************************
TyDe_EEPROM_Header readEEPROM_Header(unsigned char ubDef_whatToRead){
  unsigned short luw_ByteAddress;
  /*static*/ TyDe_EEPROM_Header tyde_ObjHeaderRetVal;
  //-------------------------------------------------
  switch(ubDef_whatToRead){
    case ONLY_ENTRY_PART:
      luw_ByteAddress = 8;//Offset in structure for tydeHeaderEntryPart
      EEPROM.get(luw_ByteAddress, tyde_ObjHeaderRetVal.tydeHeaderEntryPart);
    break;

    case COMPLETE_HEADER:
      luw_ByteAddress = 0;
      EEPROM.get(luw_ByteAddress, tyde_ObjHeaderRetVal);
    break;

    default:
    break;
  }  
  return tyde_ObjHeaderRetVal;//returns the whole structure!
}

//***********************************************************************************
void writeEEPROM_UserID(unsigned char ub_UserID_length, unsigned char* pub_UserID){
  //ToDo: check if maximum number of articles is reached (return ok/nok)
  //      check if ID exists (do not write same id twice!); use "checkUserID"
  unsigned short luw_ByteAddress;
  unsigned short luw_numOfStoredUserIDs;
  unsigned char* lpub_UserID;
  unsigned char lub_IdIdx;
  /*static*/ TyDe_EEPROM_ID_Entry tyde_ObjEEPROM_UserIdEntry;static TyDe_EEPROM_ID_Entry tyde_ObjEEPROM_UserIdEntry2;
  /*static*/ TyDe_EEPROM_Header tyde_ObjHeader;
  //-------------------------------------------------
  //copy the parameter into structure...
  tyde_ObjEEPROM_UserIdEntry.ub_ID_length = ub_UserID_length;
  lpub_UserID = pub_UserID;//copy the start address
  for(lub_IdIdx = 0; lub_IdIdx < tyde_ObjEEPROM_UserIdEntry.ub_ID_length; lub_IdIdx++){//length 4 ==> write from 0 to 3
    tyde_ObjEEPROM_UserIdEntry.aub_ID[lub_IdIdx] = *lpub_UserID;
    lpub_UserID++;
  }
  tyde_ObjEEPROM_UserIdEntry.ub_UserOrArticle = INDICATE_USER;
  //...
  
  luw_ByteAddress = sizeof(tyde_ObjHeader);//20, first entry of user ID entry; header entry "uwStartAddressUserMem"
                                                                  //can also be used but needs COMPLETE_HEADER read
  tyde_ObjHeader = readEEPROM_Header(ONLY_ENTRY_PART);
  luw_numOfStoredUserIDs = tyde_ObjHeader.tydeHeaderEntryPart.uwNumOfStoredUserIDs;
  luw_ByteAddress = luw_ByteAddress + luw_numOfStoredUserIDs * sizeof(tyde_ObjEEPROM_UserIdEntry);
  EEPROM.put(luw_ByteAddress, tyde_ObjEEPROM_UserIdEntry);
  tyde_ObjHeader.tydeHeaderEntryPart.uwNumOfStoredUserIDs++;
  luw_ByteAddress = 8;//addr. of uwNumOfStoredUserIDs in header
  EEPROM.put(luw_ByteAddress, tyde_ObjHeader.tydeHeaderEntryPart.uwNumOfStoredUserIDs);
}

//***********************************************************************************
void writeEEPROM_ArticleID(unsigned char ub_ArticleID_length, unsigned char* pub_ArticleID){
  //ToDo: check if maximum number of articles is reached (return ok/nok)
  //      check if ID exists (do not write same id twice!); use "checkArticleID"
  unsigned short luw_ByteAddress;
  unsigned short luw_numOfStoredArticleIDs;
  unsigned char* lpub_ArticleID;
  unsigned char lub_IdIdx;
  /*static*/ TyDe_EEPROM_ID_Entry tyde_ObjEEPROM_ArticleIdEntry;
  /*static*/ TyDe_EEPROM_Header tyde_ObjHeader;
  //------------------------------------------------- 
  //copy the parameter into structure...
  tyde_ObjEEPROM_ArticleIdEntry.ub_ID_length = ub_ArticleID_length;
  lpub_ArticleID = pub_ArticleID;//copy the start address
  for(lub_IdIdx = 0; lub_IdIdx < tyde_ObjEEPROM_ArticleIdEntry.ub_ID_length; lub_IdIdx++){//length 4 ==> write from 0 to 3
    tyde_ObjEEPROM_ArticleIdEntry.aub_ID[lub_IdIdx] = *lpub_ArticleID;
    lpub_ArticleID++;
  }
  tyde_ObjEEPROM_ArticleIdEntry.ub_UserOrArticle = INDICATE_ARTICLE;
  //... 

  tyde_ObjHeader = readEEPROM_Header(COMPLETE_HEADER);
  luw_ByteAddress = tyde_ObjHeader.uwStartAddressArticleMem;//3520, first entry of article ID entry
  luw_numOfStoredArticleIDs = tyde_ObjHeader.tydeHeaderEntryPart.uwNumOfStoredArticleIDs;
  luw_ByteAddress = luw_ByteAddress + luw_numOfStoredArticleIDs * sizeof(tyde_ObjEEPROM_ArticleIdEntry);
  EEPROM.put(luw_ByteAddress, tyde_ObjEEPROM_ArticleIdEntry);
  tyde_ObjHeader.tydeHeaderEntryPart.uwNumOfStoredArticleIDs++;
  luw_ByteAddress = 10;//addr. of uwNumOfStoredArticleIDs in header
  EEPROM.put(luw_ByteAddress, tyde_ObjHeader.tydeHeaderEntryPart.uwNumOfStoredArticleIDs);
}

//***********************************************************************************
unsigned char checkUserID(unsigned char ub_UserID_length, unsigned char* pub_UserID){
  unsigned short luw_ByteAddress;
  unsigned short luw_numOfStoredUserIDs;
  unsigned char* lpub_UserID;
  unsigned char lub_UserEntryIdx;
  unsigned char lub_IdIdx;
  boolean lbo_AllIdBytesFit;
  boolean lboIdMatchToEEPROM_ID;
  /*static*/ TyDe_EEPROM_ID_Entry tyde_ObjEEPROM_UserIdEntry;
  /*static*/ TyDe_EEPROM_Header tyde_ObjHeader;
  //-------------------------------------------------
  lboIdMatchToEEPROM_ID = false;
  lbo_AllIdBytesFit = false;
  lpub_UserID = pub_UserID;//copy the start address
  luw_ByteAddress = sizeof(tyde_ObjHeader);//20, first entry of user ID entry; header entry "uwStartAddressUserMem"
                                                                  //can also be used but needs COMPLETE_HEADER read
  luw_ByteAddress = luw_ByteAddress + 1;//21, locate first byte of ID
  tyde_ObjHeader = readEEPROM_Header(ONLY_ENTRY_PART);
  luw_numOfStoredUserIDs = tyde_ObjHeader.tydeHeaderEntryPart.uwNumOfStoredUserIDs;
  for(lub_UserEntryIdx = 0; lub_UserEntryIdx < luw_numOfStoredUserIDs; lub_UserEntryIdx ++){ 
    for(lub_IdIdx = 0; lub_IdIdx < ub_UserID_length; lub_IdIdx++){
      EEPROM.get(luw_ByteAddress + lub_IdIdx, tyde_ObjEEPROM_UserIdEntry.aub_ID[lub_IdIdx]);
      if(tyde_ObjEEPROM_UserIdEntry.aub_ID[lub_IdIdx] != *(lpub_UserID + lub_IdIdx)){//check bytes of actual found entry
        break;//stop this for loop  
      }
      if(lub_IdIdx >= (ub_UserID_length - 1)){//if all bytes fit also break
        lbo_AllIdBytesFit = true;
        break;//stop this for loop
      }  
    }
    if(lbo_AllIdBytesFit == false){
      lub_IdIdx = 0;//if one byte do not fit next ID check must start at first byte!
    }
    else{//check if found entry is a 7 byte ID and check if length (4 or 7 bytes) fits to that (avoid 4byte request is the same as first four bytes of 7 byte address from EEPROM)
      EEPROM.get(luw_ByteAddress - 1, tyde_ObjEEPROM_UserIdEntry.ub_ID_length);// - 1 to reach length entry from EEPROM 
      if(tyde_ObjEEPROM_UserIdEntry.ub_ID_length == ub_UserID_length){ 
        lboIdMatchToEEPROM_ID = true;
        break;//ID is found in EEPROM ==> return found ID   
      }
    }
    luw_ByteAddress = luw_ByteAddress + sizeof(tyde_ObjEEPROM_UserIdEntry); //adr.: 21, 31, 41...
  }//if all entries are checked and non fits, "" stays false as set at the beginning of the function

  return lboIdMatchToEEPROM_ID;
/*
false:
false is defined as 0 (zero).
true:
true is often said to be defined as 1, which is correct, but any integer which is non-zero also deliver true!
*/   
}

//***********************************************************************************
unsigned char checkArticleID(unsigned char ub_ArticleID_length, unsigned char* pub_ArticleID){
  unsigned short luw_ByteAddress;
  unsigned short luw_numOfStoredArticleIDs;
  unsigned char* lpub_ArticleID;
  unsigned char lub_ArticleEntryIdx;
  unsigned char lub_IdIdx;
  boolean lbo_AllIdBytesFit;
  boolean lboIdMatchToEEPROM_ID;
  /*static*/ TyDe_EEPROM_ID_Entry tyde_ObjEEPROM_ArticleIdEntry;
  /*static*/ TyDe_EEPROM_Header tyde_ObjHeader;
  //-------------------------------------------------
  lboIdMatchToEEPROM_ID = false;
  lbo_AllIdBytesFit = false;
  lpub_ArticleID = pub_ArticleID;//copy the start address
  tyde_ObjHeader = readEEPROM_Header(COMPLETE_HEADER);
  luw_ByteAddress = tyde_ObjHeader.uwStartAddressArticleMem;//3520, first entry of article ID entry
  luw_ByteAddress = luw_ByteAddress + 1;//3521, locate first byte of ID
  luw_numOfStoredArticleIDs = tyde_ObjHeader.tydeHeaderEntryPart.uwNumOfStoredArticleIDs;
  for(lub_ArticleEntryIdx = 0; lub_ArticleEntryIdx < luw_numOfStoredArticleIDs; lub_ArticleEntryIdx ++){ 
    for(lub_IdIdx = 0; lub_IdIdx < ub_ArticleID_length; lub_IdIdx++){
      EEPROM.get(luw_ByteAddress + lub_IdIdx, tyde_ObjEEPROM_ArticleIdEntry.aub_ID[lub_IdIdx]);
      if(tyde_ObjEEPROM_ArticleIdEntry.aub_ID[lub_IdIdx] != *(lpub_ArticleID + lub_IdIdx)){//check bytes of actual found entry
        break;//stop this for loop  
      }
      if(lub_IdIdx >= (ub_ArticleID_length - 1)){//if all bytes fit also break
        lbo_AllIdBytesFit = true;
        break;//stop this for loop
      }    
    }
    if(lbo_AllIdBytesFit == false){
      lub_IdIdx = 0;//if one byte do not fit next ID check must start at first byte!
    }
    else{//check if found entry is a 7 byte ID and check if length (4 or 7 bytes) fits to that (avoid 4byte request is the same as first four bytes of 7 byte address from EEPROM)
      EEPROM.get(luw_ByteAddress - 1, tyde_ObjEEPROM_ArticleIdEntry.ub_ID_length);// - 1 to reach length entry from EEPROM 
      if(tyde_ObjEEPROM_ArticleIdEntry.ub_ID_length == ub_ArticleID_length){
        lboIdMatchToEEPROM_ID = true;
        break;//ID is found in EEPROM ==> return found ID   
      }
    }
    luw_ByteAddress = luw_ByteAddress + sizeof(tyde_ObjEEPROM_ArticleIdEntry); //adr.: 3521, 3531, 3541...
  }//if all entries are checked and non fits, "lboIdMatchToEEPROM_ID" stays false as set at the beginning of the function

  return lboIdMatchToEEPROM_ID; 
/*
false:
false is defined as 0 (zero).
true:
true is often said to be defined as 1, which is correct, but any integer which is non-zero also deliver true!
*/
}

//***********************************************************************************
unsigned char checkMasterID(unsigned char ub_MasterID_length, unsigned char* pub_MasterID){
  unsigned short luw_numOfMasterIDs = 3;
  unsigned char aalub_MasterIdTeachUser[luw_numOfMasterIDs][sizeof(TyDe_MasterIdStruct)] = MASTER_ID_TEACH_USER;
  unsigned char aalub_MasterIdTeachArticle[luw_numOfMasterIDs][sizeof(TyDe_MasterIdStruct)] = MASTER_ID_TEACH_ARTICLE;
  unsigned char aalub_MasterIdEraseIDs[luw_numOfMasterIDs][sizeof(TyDe_MasterIdStruct)] = MASTER_ID_ERASE_IDS;
  unsigned char* lpub_MasterID;
  unsigned char lub_MasterIdx;
  unsigned char lub_IdIdx;
  unsigned char lub_IdIdxOffset;
  boolean lbo_AllIdBytesFit;
  unsigned char lub_IdMatchToMasterID_type;
  /*static*/ TyDe_MasterIdStruct tyde_ObjMasterIdStruct;
  //-------------------------------------------------
  lub_IdMatchToMasterID_type = NO_MASTER;   //lboIdMatchToEEPROM_ID = false;
  lpub_MasterID = pub_MasterID;   //copy the start address//lpub_ArticleID = pub_ArticleID;//copy the start address
  lub_IdIdxOffset = 1;//locate first byte of Master ID (4 or 7 bytes are possible); (0 = length, first entry of structure)   //luw_ByteAddress = tyde_ObjHeader.uwStartAddressArticleMem;//3520, first entry of article ID entry

  //.....................................................................................................................add user:
  lbo_AllIdBytesFit = false;
  for(lub_MasterIdx = 0; lub_MasterIdx < luw_numOfMasterIDs; lub_MasterIdx ++){ 
    for(lub_IdIdx = 0; lub_IdIdx < ub_MasterID_length; lub_IdIdx++){
      tyde_ObjMasterIdStruct.aub_ID[lub_IdIdx] = aalub_MasterIdTeachUser[lub_MasterIdx][lub_IdIdxOffset + lub_IdIdx];
      if(tyde_ObjMasterIdStruct.aub_ID[lub_IdIdx] != *(lpub_MasterID + lub_IdIdx)){//check bytes of actual MasterID
        break;//stop this for loop  
      }
      if(lub_IdIdx >= (ub_MasterID_length - 1)){//if all bytes fit also break
        lbo_AllIdBytesFit = true;
        break;//stop this for loop
      }    
    }
    if(lbo_AllIdBytesFit == false){
      lub_IdIdx = 0;//if one byte do not fit next ID check must start at first byte!
    }
    else{//check if found entry is a 7 byte ID and check if length (4 or 7 bytes) fits to that (avoid 4byte request is the same as first four bytes of 7 byte address from EEPROM)
      tyde_ObjMasterIdStruct.ub_ID_length = aalub_MasterIdTeachUser[lub_MasterIdx][0];//(0 = length, first entry of structure)
      if(tyde_ObjMasterIdStruct.ub_ID_length == ub_MasterID_length){
        lub_IdMatchToMasterID_type = TEACH_USER_ID_MASTER;
        break;//ID is found in the hardcoded ID list of TEACH_USER_ID_MASTER ==> return found TEACH_USER_ID_MASTER   
      }
    }
  }//if all entries are checked and non fits, "lub_IdMatchToMasterID_type" stays NO_MASTER as set at the beginning of the function
  //.....................................................................................................................add article:
  lbo_AllIdBytesFit = false;
  for(lub_MasterIdx = 0; lub_MasterIdx < luw_numOfMasterIDs; lub_MasterIdx ++){ 
    for(lub_IdIdx = 0; lub_IdIdx < ub_MasterID_length; lub_IdIdx++){
      tyde_ObjMasterIdStruct.aub_ID[lub_IdIdx] = aalub_MasterIdTeachArticle[lub_MasterIdx][lub_IdIdxOffset + lub_IdIdx];
      if(tyde_ObjMasterIdStruct.aub_ID[lub_IdIdx] != *(lpub_MasterID + lub_IdIdx)){//check bytes of actual MasterID
        break;//stop this for loop  
      }
      if(lub_IdIdx >= (ub_MasterID_length - 1)){//if all bytes fit also break
        lbo_AllIdBytesFit = true;
        break;//stop this for loop
      }    
    }
    if(lbo_AllIdBytesFit == false){
      lub_IdIdx = 0;//if one byte do not fit next ID check must start at first byte!
    }
    else{//check if found entry is a 7 byte ID and check if length (4 or 7 bytes) fits to that (avoid 4byte request is the same as first four bytes of 7 byte address from EEPROM)
      tyde_ObjMasterIdStruct.ub_ID_length = aalub_MasterIdTeachArticle[lub_MasterIdx][0];//(0 = length, first entry of structure)
      if(tyde_ObjMasterIdStruct.ub_ID_length == ub_MasterID_length){
        lub_IdMatchToMasterID_type = TEACH_ARTICLE_ID_MASTER;
        break;//ID is found in the hardcoded ID list of TEACH_USER_ID_MASTER ==> return found TEACH_USER_ID_MASTER   
      }
    }
  }//if all entries are checked and non fits, "lub_IdMatchToMasterID_type" stays NO_MASTER as set at the beginning of the function
  //.....................................................................................................................erase IDs:
  lbo_AllIdBytesFit = false;
  for(lub_MasterIdx = 0; lub_MasterIdx < luw_numOfMasterIDs; lub_MasterIdx ++){ 
    for(lub_IdIdx = 0; lub_IdIdx < ub_MasterID_length; lub_IdIdx++){
      tyde_ObjMasterIdStruct.aub_ID[lub_IdIdx] = aalub_MasterIdEraseIDs[lub_MasterIdx][lub_IdIdxOffset + lub_IdIdx];
      if(tyde_ObjMasterIdStruct.aub_ID[lub_IdIdx] != *(lpub_MasterID + lub_IdIdx)){//check bytes of actual MasterID
        break;//stop this for loop  
      }
      if(lub_IdIdx >= (ub_MasterID_length - 1)){//if all bytes fit also break
        lbo_AllIdBytesFit = true;
        break;//stop this for loop
      }    
    }
    if(lbo_AllIdBytesFit == false){
      lub_IdIdx = 0;//if one byte do not fit next ID check must start at first byte!
    }
    else{//check if found entry is a 7 byte ID and check if length (4 or 7 bytes) fits to that (avoid 4byte request is the same as first four bytes of 7 byte address from EEPROM)
      tyde_ObjMasterIdStruct.ub_ID_length = aalub_MasterIdEraseIDs[lub_MasterIdx][0];//(0 = length, first entry of structure)
      if(tyde_ObjMasterIdStruct.ub_ID_length == ub_MasterID_length){
        lub_IdMatchToMasterID_type = ERASE_IDS_MASTER;
        break;//ID is found in the hardcoded ID list of TEACH_USER_ID_MASTER ==> return found TEACH_USER_ID_MASTER   
      }
    }
  }//if all entries are checked and non fits, "lub_IdMatchToMasterID_type" stays NO_MASTER as set at the beginning of the function
  //.....................................................................................................................
  return lub_IdMatchToMasterID_type; 
/*
false:
false is defined as 0 (zero).
true:
true is often said to be defined as 1, which is correct, but any integer which is non-zero also deliver true!
*/
}

//***********************************************************************************
void eraseEEPROM_AndReWriteHeader(){
  unsigned short luw_ByteAddress;
  /*static*/ TyDe_EEPROM_Header tyde_ObjHeader;
  //-------------------------------------------------
  for (luw_ByteAddress = 0 ; luw_ByteAddress < 4020/*EEPROM.length()*/ ; luw_ByteAddress++) {
    EEPROM.write(luw_ByteAddress, 0);
  }
 
  tyde_ObjHeader.uwStartAddressUserMem = 20;//350 User, each user 10 bytes
  tyde_ObjHeader.uwStartAddressArticleMem = 3520;//50 Articles, each article 10 bytes
  tyde_ObjHeader.uwMaxNumOfUserIDs = 350;
  tyde_ObjHeader.uwMaxNumOfArticleIDs = 50;
  tyde_ObjHeader.tydeHeaderEntryPart.uwNumOfStoredUserIDs = 0;
  tyde_ObjHeader.tydeHeaderEntryPart.uwNumOfStoredArticleIDs = 0;
  tyde_ObjHeader.uwHeaderExistCheck1 = HEADER_EXIST_CHECKVAL1;
  tyde_ObjHeader.uwHeaderExistCheck2 = HEADER_EXIST_CHECKVAL2;

  writeEEPROM_Header(tyde_ObjHeader, COMPLETE_HEADER);
}

// Anzahl der gespeicherten Einträge auslesen
// Parameter:
// ubyType = 'A' für Artikel, 'B' für Benutzer
// Rückgabewert:
// Anzahl der gespeicherten Einträge
short EEPROM_AnzEintraege(unsigned char ubyType)
{
  unsigned short uiAnz = 0;
  TyDe_EEPROM_Header sKopf = readEEPROM_Header(COMPLETE_HEADER);

  switch(ubyType)
  {
    case 'A':
      uiAnz = sKopf.tydeHeaderEntryPart.uwNumOfStoredArticleIDs;
      break;
    case 'B':
      uiAnz = sKopf.tydeHeaderEntryPart.uwNumOfStoredUserIDs;
      break;
  }
  
  return uiAnz;    
}

// Anzahl der gespeicherten Einträge auslesen
// Parameter:
// ubyType = 'A' für Artikel, 'B' für Benutzer
// iID = Identifikationsnummer des zu lesenden Eintrags (1 - X)
// Rückgabewert:
// der gefundene Eintrag (4 bzw. 7 Zeichen in hexadezimaler Schreibweise)
String EEPROM_LiesEintrag(unsigned char ubyType, int iID)
{
  String strID = "";
  unsigned short uiAdr = 0;
  TyDe_EEPROM_Header sKopf = readEEPROM_Header(COMPLETE_HEADER);
  TyDe_EEPROM_ID_Entry sEintrag;

  if (iID > 0)
  {
    switch(ubyType)
    {
      case 'A':
        // Startadresse auslesen
        if (iID <= sKopf.uwMaxNumOfArticleIDs)
        {
          uiAdr = sKopf.uwStartAddressArticleMem;          
        }
        break;
      case 'B':
        // Startadresse auslesen
        if (iID <= sKopf.uwMaxNumOfUserIDs)
        {
          uiAdr = sKopf.uwStartAddressUserMem;          
        }
        break;
    }
  }


  // Schlüssel auslesen
  if (uiAdr > 0)
  {
    // Startadresse
    uiAdr += (iID - 1) * sizeof(sEintrag);
    
    // Datenstruktur auslesen
    EEPROM.get(uiAdr, sEintrag);

    // Variable umwandeln in eine Zeichenkette
    for(char i = 0; i < sEintrag.ub_ID_length; i++)
    {
      if (sEintrag.aub_ID[i] < 16)
      {
        strID += " 0" + String(sEintrag.aub_ID[i],HEX);             
      }
      else
      {
        strID += " " + String(sEintrag.aub_ID[i],HEX);      
      }
    }    
  }
  else
  {
    // ungültiger Schlüssel abgefragt
    strID += " Fehler " + String(sKopf.uwStartAddressArticleMem) + "/" + String(sKopf.uwMaxNumOfArticleIDs) + " " + String(sKopf.uwStartAddressUserMem) + "/" + String(sKopf.uwMaxNumOfUserIDs);
  }
  return strID; 
}

// Neuen Eintrag am Ende der Liste hinzufügen
// Parameter:
// ubyType = 'A' für Artikel, 'B' für Benutzer
// strNummer = neue Nummer
// Rückgabewert:
// "OK" oder "Fehler: X"
String EEPROM_NeuEintrag(unsigned char ubyType, String strNummer)
{
  String strAntwort = "+ ";
  unsigned short uiAdr = 0;
  unsigned short uiAnz = 0;
  
  TyDe_EEPROM_Header sKopf = readEEPROM_Header(COMPLETE_HEADER);
  TyDe_EEPROM_ID_Entry sEintrag;

  // Länge der Nummer in Bytes ermitteln  
  sEintrag.ub_ID_length = 7;
  if (strNummer.length() < 20)
  {
    sEintrag.ub_ID_length = 4;
  }

  // Nummer umwandeln
  for (int i = 0; i < sEintrag.ub_ID_length; i++)
  {
    sEintrag.aub_ID[i] = strtoul(strNummer.substring(i * 3, i * 3 + 2).c_str(), NULL, 16);
  }

  switch(ubyType)
  {
    case 'A':
      // Startadresse auslesen
      uiAdr = sKopf.uwStartAddressArticleMem;
      uiAnz = sKopf.tydeHeaderEntryPart.uwNumOfStoredArticleIDs;
      if (uiAnz >= sKopf.uwMaxNumOfArticleIDs)
      {
        return "Fehler: Speicher voll";
      }
      if (checkArticleID(sEintrag.ub_ID_length, sEintrag.aub_ID))
      {
        return "Fehler: existiert bereits";
      }
      //Anzahl im Kopf um eins erhöhen
      EEPROM.put(10, uiAnz + 1);
      break;
    case 'B':
      // Startadresse und Anzahl auslesen
      uiAdr = sKopf.uwStartAddressUserMem;
      uiAnz = sKopf.tydeHeaderEntryPart.uwNumOfStoredUserIDs;
      if (uiAnz >= sKopf.uwMaxNumOfUserIDs)
      {
        return "Fehler: Speicher voll";
      }
      if (checkUserID(sEintrag.ub_ID_length, sEintrag.aub_ID))
      {
        return "Fehler: existiert bereits";
      }
      //Anzahl im Kopf um eins erhöhen
      EEPROM.put(8, uiAnz + 1);
      break;
  } 

  // Adresse ermitteln
  uiAdr += uiAnz * sizeof(sEintrag);

  // Eintrag schreiben
  EEPROM.put(uiAdr, sEintrag);
  
  return strAntwort + String(++uiAnz) + " " + strNummer; 
}

// Eintrag aus der Liste entfernen
// Parameter:
// ubyType = 'A' für Artikel, 'B' für Benutzer
// iID = Identifikationsnummer des zu lesenden Eintrags (1 - X)
// Rückgabewert:
// "OK" oder "Fehler: X"
String EEPROM_EntfEintrag(unsigned char ubyType, int iID)
{
  String strAntwort = "- ";
  unsigned short uiAdr = 0;
  unsigned short uiAnz = 0;
  
  TyDe_EEPROM_Header sKopf = readEEPROM_Header(COMPLETE_HEADER);
  TyDe_EEPROM_ID_Entry sEintrag;

  switch(ubyType)
  {
    case 'A':
      // Startadresse auslesen
      uiAdr = sKopf.uwStartAddressArticleMem;
      uiAnz = sKopf.tydeHeaderEntryPart.uwNumOfStoredArticleIDs;
      if (iID < 1 || iID > uiAnz)
      {
        return "Fehler: nicht vorhanden";
      }
      //Anzahl im Kopf um eins senken
      EEPROM.put(10, --uiAnz);
      break;
    case 'B':
      // Startadresse und Anzahl auslesen
      uiAdr = sKopf.uwStartAddressUserMem;
      uiAnz = sKopf.tydeHeaderEntryPart.uwNumOfStoredUserIDs;
      if (iID < 1 || iID > uiAnz)
      {
        return "Fehler: nicht vorhanden";
      }
      //Anzahl im Kopf um eins senken
      EEPROM.put(8, --uiAnz);
      break;
  } 

  // Adresse des Eintrags berechnen
  uiAdr += (iID - 1) * sizeof(sEintrag);

  // Lücke füllen
  for (int i = iID - 1; i < uiAnz; i++)
  {       
    // nächsten Eintrag lesen
    EEPROM.get(uiAdr + sizeof(sEintrag), sEintrag);
    
    // Eintrag schreiben
    EEPROM.put(uiAdr, sEintrag);

    // Adresse erhöhen
    uiAdr += sizeof(sEintrag);
  }
  
  return strAntwort + String(iID); 
}

// Nummern löschen
// Parameter:
// ubyType = 'A' für Artikel, 'B' für Benutzer
// Rückgabewert:
// "0" = ok, "Fehler"
String EEPROM_EntfNummern(unsigned char ubyType)
{ 
  String strAntwort = "Fehler";
  unsigned short uiAdr = 0;
  unsigned short uiAnz = 0;
  
  switch(ubyType)
  {
    case 'A':
      //Adresse übernehmen
      uiAdr = 10;
      break;
    case 'B':
      //Adresse übernehmen
      uiAdr = 8;
      break;
  } 

  // Anzahl löschen
  if (uiAdr > 0)
  {
      EEPROM.put(uiAdr, uiAnz);
      strAntwort = "0";
  } 
  
  return strAntwort;
}

// Seriennummer lesen
// Rückgabewert:
// gelesene Seriennummer
String EEPROM_SNrLesen()
{
  String strAntwort;  

  // Nummer auslesen
  for (int i = 0; i < 10; i++)
  {
    byte byWert = EEPROM.read(4086 + i);
    if (byWert == 255)
    {
      strAntwort += "0";
    }
    else
    {
      strAntwort += String(char(byWert));      
    }
  }

  // Antwort ausgeben
  return strAntwort;
}

// Seriennummer schreiben
// strNummer = neue Seriennummer
// Rückgabewert:
// geschriebene Seriennummer
String EEPROM_SNrSchreiben(String strNummer)
{
  // alte Nummer lesen
  String strNrAlt;  

  // Nummer auslesen
  for (int i = 0; i < 10; i++)
  {
    byte byWert = EEPROM.read(4086 + i);
    strNrAlt += String(char(byWert));
  }

  // Nummer umkopieren
  char fbyNr[11];
  strNummer.toCharArray(fbyNr, 11);

  // Nummer schreiben - ACHTUNG: Überlauf ist möglich, dann wird wieder ab Adresse 0 geschrieben!
  for (int i = 0; i < 10; i++)
  {
    EEPROM.write(4086 + i, fbyNr[i]);
  }

  // Logeintrag schreiben
  LOG_Eintrag("Seriennummer neu " + strNummer + " (" + strNrAlt + ")");

  // Antwort ausgeben
  return strNummer + " " + strNrAlt;
}

// aktuellen Status ausgeben
// Rückgabewert:
// Schlosszustand, letzter Benutzer, letzter Artikel
String EEPROM_StatusLesen(void)
{
  String strAntwort;
  byte byL = 0;
  byte byWert = 0;

  // Schlosszustand ermitteln
  strAntwort = MOT_ZustandZK(uint8_t(EEPROM.read(4020)));

  // Benutzer- und Artikelnummer ermitteln und anhängen
  for (int i = 0; i < 2; i++)
  { 
    strAntwort += " ";
    byL = EEPROM.read(4030 + i * 10);
    if ((byL == 4) || (byL == 7))
    {
      for (int j = 0; j <byL ; j++)
      {
        byWert = EEPROM.read(4031 + i * 10 + j);
        if (byWert < 16)
        {
          strAntwort += "0";
        }
        strAntwort += String(byWert, HEX);
      }
    }
    else
    {
      strAntwort += "unbekannt";
    }
  }

  return strAntwort;
}

// Parameter aus dem EEPROM lesen
// Parameter:
// strTyp = Parametertyp ("08+", "08-", "16+, "16-" ...) entspricht Bitanzahl, bzw. "+" = kein Vorzeichen oder "-" = Vorzeichen
// iAdr = Adresse
// Rückgabewert:
// gelesener Parameter
String EEPROM_ParLesen(String strTyp, int iAdr)
{
  unsigned short uiAdr = 4050;  
  iAdr--;
  
  if (strTyp == "08+")
  {
    // Byte ohne Vorzeichen
    byte byWert;
    uiAdr += iAdr;
    EEPROM.get(uiAdr, byWert);
    return String(byWert);    
  }
  else if (strTyp == "08-")
  {
    // Byte mit Vorzeichen
    char ubyWert;
    uiAdr += iAdr;
    EEPROM.get(uiAdr, ubyWert);
    return String(byte(ubyWert));  
  }
  else if (strTyp == "16+")
  {
    // Wort ohne Vorzeichen
    word uiWert;
    uiAdr += iAdr * 2;
    EEPROM.get(uiAdr, uiWert);
    return String(uiWert);    
  }  
  else if (strTyp == "16-")
  {
    // Wort mit Vorzeichen
    short iWert;
    uiAdr += iAdr * 2;
    EEPROM.get(uiAdr, iWert);
    return String(iWert);  
  }    
  else
  {
    return "Typfehler";
  }
}


// Parameter in den EEPROM schreiben
// Parameter:
// strTyp = Parametertyp ("08+", "08-", "16+, "16-" ...) entspricht Bitanzahl, bzw. "+" = kein Vorzeichen oder "-" = Vorzeichen
// iAdr = Adresse
// iWert = neuer Wert
// Rückgabewert:
// Bestätigung des Schreibvorgangs
String EEPROM_ParSchreiben(String strTyp, int iAdr, int iWert)
{
  unsigned short uiAdr = 4050;
  //die interne Adressierung beginnt mit 0
  iAdr--;
  
  if (strTyp == "08+")
  {
    // Byte ohne Vorzeichen
    if ((iWert < 0) ||(iWert > 255))
    {
      return "Wertfehler";
    }
    byte byWert = byte(iWert);
    uiAdr += iAdr;
    EEPROM.put(uiAdr, byWert);
  }
  else if (strTyp == "08-")
  {
    // Byte mit Vorzeichen
    if ((iWert < -128) ||(iWert > 127))
    {
      return "Wertfehler";
    }
    char ubyWert = char(iWert);
    uiAdr += iAdr;
    EEPROM.put(uiAdr, ubyWert);
  }
  else if (strTyp == "16+")
  {
    // Wort ohne Vorzeichen
    if ((iWert < 0) ||(iWert > 65535))
    {
      return "Wertfehler";
    }    
    word uiWert = word(iWert);
    uiAdr += iAdr * 2;
    EEPROM.put(uiAdr, uiWert); 
  }  
  else if (strTyp == "16-")
  {
    // Wort mit Vorzeichen
    if ((iWert < -32768) ||(iWert > 32767))
    {
      return "Wertfehler";
    }    
    short iWert = short(iWert);
    uiAdr += iAdr * 2;
    EEPROM.put(uiAdr, iWert);
  }    
  else
  {
    return "Typfehler";
  }

  // damit die angezeigte Adresse wieder korrekt ist
  iAdr++;
  
  // Logeintrag schreiben
  LOG_Eintrag("Parameter EEPROM(" + String(uiAdr) + "): " + strTyp + " " + String(iAdr) + " = " + String(iWert));
  return strTyp + " " + String(iAdr) + " " + String(iWert);
}

// EEPROM Zeile (16 Byte) auslesen
// Parameter:
// iAdr = Startzelle
// Rückgabewert:
// Adresse Byte1 Byte2 ... hexadezimal formatiert
String EEPROM_ZeileLesen(int iAdr)
{
  String strAntwort;

  // Adresse anhängen
  if(iAdr < 16)
  {
    strAntwort = "000";
  }
  else if(iAdr < 256)
  {
    strAntwort = "00";
  }
  else if(iAdr < 4096)
  {
    strAntwort = "0";
  }
  strAntwort += String(iAdr, HEX);

  // Bytes auslesen
  if ((iAdr >= 0) && (iAdr <= 4096 - 16))
  {
    for (int i = 0; i < 16; i++)
    {
      byte byWert = EEPROM.read(iAdr + i);
      strAntwort += " ";
      if (byWert < 16)
      {
        strAntwort += "0";
      }
      strAntwort += String(byWert, HEX);
    }
  }
  else
  {
    // ungültige Adresse
    strAntwort += " Adressfehler";
  }
  
  return strAntwort;
}

// Schlosszustand im EEPROM sichern
// Parameter:
// ubyStat = aktueller Schlosszustand
void EEPROM_Schlossstatus(unsigned char ubyStat)
{
  EEPROM.write(4020, byte(ubyStat));
}

// letzten Zugriff im EEPROM sichern
// Parameter:
// ubyType = 'A' für Artikel, 'B' für Benutzer
// ubyIDL = Länge der Identifikationsnummer (4 oder 7)
// pubyID = Zeiger auf das Feld mit der Identifikationsnummer
void EEPROM_LetzterZugriff(unsigned char ubyType, unsigned char ubyIDL, unsigned char* pubyID)
{
  unsigned short uiAdr = 0;
  
  switch(ubyType)
  {
    case 'A':
      uiAdr = 4040;
      break;
    case 'B':
      uiAdr = 4030;
      break;
  }

  if (uiAdr > 0)
  {
    // gültige ID abfragen
    if ((ubyIDL == 4) || (ubyIDL == 7))
    {
      // Länge sichern
      EEPROM.write(uiAdr++, byte(ubyIDL));
      // neue Nummer sichern
      for (int i = 0; i < 7; i++)
      {
        // bei Länge 4 werden die letzten 3 Stellen auf 0 gesetzt
        if (i < ubyIDL)
        {
          EEPROM.write(uiAdr++, pubyID[i]);          
        }
        else
        {
          EEPROM.write(uiAdr++, 0);
        }
      }
    }
    else if(ubyIDL == 0)
    {
      // löschen der Nummer und Länge
      for (int i = 0; i < 8; i++)
      {
        EEPROM.write(uiAdr++, 0);
      }      
    }
  }
}
