#include <RTClib.h>

//
// Include for SL030 I2C
//
#include <Wire.h>       // I2C Library

//#define SERIAL_DEBUG_ENABLE
//
// Definition for SL030 I2C
//
#define SL030ADR 0xA0   // standard address for SL030


//
// Definition for SL032
//
#define SL032_WAKEUP_DO 23  // Portpin für Wakeup

#define PO_SPEAKER  31      // Portpin for speaker output

/*
  SD card test 
   
This example shows how use the utility libraries on which the'
SD library is based in order to get info about your SD card.
Very useful for testing a card when you're not sure whether its working or not.
    
The circuit:
  * SD card attached to SPI bus as follows:
** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
** CS - depends on your SD card shield or module. 
        Pin 4 used here for consistency with other Arduino examples


created  28 Mar 2011
by Limor Fried 
modified 9 Apr 2012
by Tom Igoe
*/
// include the SD library:
#include <SD.h>
#include <SPI.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// Mega/Due: pin 53


//void SL032_SendCom(unsigned char *g_cCommand);                                           

//Command List, preamble + length + command code + data frame
//unsigned char const SelectCard[3] =      {0xBA,0x02,0x01 };       
unsigned char const LoginSector0[11] =   {0xBA,0x0A,0x02,0x00,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  
unsigned char const LoginSector1[11] =   {0xBA,0x0A,0x02,0x01,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned char const ReadBlock1[4]   =    {0xBA,0x03,0x03,0x01};                                     
unsigned char const WriteBlock1[20] =    {0xBA,0x13,0x04,0x01,0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};                                
unsigned char const ReadValue[4]  =      {0xBA,0x03,0x05,0x04};  
unsigned char const WriteValue[8]  =     {0xBA,0x07,0x06,0x04,0x00,0x00,0x00,0x01};                                
unsigned char const IncrementValue[8] =  {0xBA,0x07,0x08,0x04,0x00,0x00,0x00,0x20};                                
unsigned char const DecrementValue[8] =  {0xBA,0x07,0x09,0x04,0x03,0x00,0x00,0x00};                                          
unsigned char const CopyValue[5] =       {0xBA,0x04,0x0A,0x04,0x05};
unsigned char const ReadULPage5[4] =     {0xBA,0x03,0x10,0x05};                                          
unsigned char const WriteULPage5[8] =    {0xBA,0x07,0x11,0x05,0x11,0x22,0x33,0x44}; 
unsigned char const RATSDesFire[3] =     {0xBA,0x02,0x20};
unsigned char const DesFireGetVersion[4]={0xBA,0x03,0x21,0x60}; 
unsigned char const GetAdditionalData[4]={0xBA,0x03,0x21,0xAF}; 
unsigned char const LightOn[4] =         {0xBA,0x03,0x40,0x01}; 
unsigned char const Halt[4] =            {0xBA,0x03,0x50,0x00};             

unsigned char g_ucReceBuf[70];                                      
unsigned char g_ucReceNum;                                          
unsigned char g_ucWatchTime; 
unsigned char g_ucCardType;
char g_cStatus;                                       
char g_cErr;

// SL032 Boolean Variables
bool  g_bReceOver;                                                 
bool  g_bReceReady;
bool  g_bTimeOut;                                                  

static uint8_t uid[7] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
static uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

unsigned int uiCounter;

boolean SL030readPassiveTargetID(uint8_t* puid, uint8_t* uidLength, uint8_t u8MaxLen);
boolean SL032readPassiveTargetID(uint8_t uid[], uint8_t *uidLength, uint8_t u8MaxLen);
//void SL032_ReadUid(uint8_t uid[], uint8_t &uidLength);

void setup() {
  // put your setup code here, to run once:
// Open serial communications and wait for port to open:
  Serial.begin(115200);
//   while (!Serial) {
//    ; // wait for serial port to connect. Needed for Leonardo only
  
  Serial.println("Startup System");
  uiCounter = 0;




  Serial.print("\nInitializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(53, OUTPUT);
  SPI.begin(53); //habe ich eingeführt, da es in einigen Foren erwähnt wird, funktioniert jedoch mit und ohne nicht. //I made this, becaus it's mentioned in severals forums, but it didn't workt wiht it and whitout it   [/b]  // change this to 53 on a mega 


  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, 53)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
   Serial.println("Wiring is correct and a card is present."); 
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();
  
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  
  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);

  // Speaker output init
  pinMode(PO_SPEAKER, OUTPUT);

  //
  //  setup() SL032 UART
  //
  // SL032 Pin Configuration
  Serial.println("Startup SL032");

  pinMode(SL032_WAKEUP_DO, OUTPUT);
  // SL032 Wakeup
  delay(1);   
  digitalWrite(SL032_WAKEUP_DO, HIGH);
  delay(1);   
  digitalWrite(SL032_WAKEUP_DO, LOW);
  
  Serial2.begin(115200);    // SL032 Serial-ComPort

  //
  // setup() for SL030 I2C
  //
  Wire.begin();         // join i2c bus (address optional for master)
  
  g_ucReceNum = 0;
  g_bReceOver = false;
  g_bReceReady = false;
}

void loop() {
  unsigned char i;
  // put your main code here, to run repeatedly:
  Serial.print("Cycle Counter:");
  Serial.println(uiCounter++, DEC);
//  char c = 65;
//  Serial.print((char)65);

  // SL032 Wakeup
 /*  delay(10);   
  digitalWrite(SL032_WAKEUP_DO, LOW);
  delay(10);   
  digitalWrite(SL032_WAKEUP_DO, HIGH);
  delay(100);   
*/
  //
  // loop() for SL032 UART
  //
  if (SL032readPassiveTargetID(&uid[0],&uidLength, 7))
  {
    Serial.print("RFID#1: ");
    for (i=0; i<uidLength; i++)
    {
      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    for (i = 0; i < 100; i++){
      delay(1);   
      digitalWrite(PO_SPEAKER, HIGH);
      delay(1);   
      digitalWrite(PO_SPEAKER, LOW);
    }
  }
  else
  {
    Serial.println("NO RFID#1");
  }

  //
  // loop() for SL030 I2C
  //
  if (SL030readPassiveTargetID(&uid[0],&uidLength, 7))
  {
    Serial.print("RFID#2: ");
    for (i=0; i<uidLength; i++)
    {
      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    for (i = 0; i < 100; i++){
      delay(2);   
      digitalWrite(PO_SPEAKER, HIGH);
      delay(2);   
      digitalWrite(PO_SPEAKER, LOW);
    }
  }
  else
  {
    Serial.println("NO RFID#2");
  }

  Serial.println();
  Serial.print("Pause");
  Serial.println();
  delay(900+100/*500+360+1000*/);
}



// Stronglink I2C Reader
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
void SL032_SendCom(const unsigned char *g_cCommand);                                           

unsigned char const SelectCard[3] =      {0xBA,0x02,0x01 };       
void SL032_SendCom(const unsigned char *g_cCommand)
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
  while(Serial2.available() == 0);
  
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
