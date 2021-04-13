/*
  SD card test

  This example shows how use the utility libraries on which the'
  SD library is based in order to get info about your SD card.
  Very useful for testing a card when you're not sure whether its working or not.

  The circuit:
    SD card attached to SPI bus as follows:
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
#include <SPI.h>
#include <SD.h>


#define USE_TEST_CARD      //
//#define USE_TEST_CARD_SD      //
//#define USE_PRINT_DIRECTORY   //
//#define SD_ERROR_READ_IMPLEMENTED //

#define PIN_MISO  50
#define PIN_MOSI  51
#define PIN_SSPK  52
#define PIN_SSEL  53

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
//const int chipSelect = 4;
const int chipSelect = 53;

#ifdef USE_PRINT_DIRECTORY
File root;
#endif

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

//  pinMode(10, OUTPUT); // change this to 53 on a mega(don't follow this!!)
//  digitalWrite(10, HIGH); // Add this line

//  pinMode(chipSelect, OUTPUT); // change this to 53 on a mega(don't follow this!!)
//  digitalWrite(chipSelect, HIGH); // Add this line
//  digitalWrite(chipSelect, LOW); // Add this line

  pinMode(PIN_MISO, INPUT);
  pinMode(PIN_MISO, INPUT_PULLUP);
  pinMode(PIN_MOSI, OUTPUT); digitalWrite(51, LOW);
//  pinMode(PIN_SSPK, OUTPUT); digitalWrite(52, LOW);
  
//  pinMode(53, OUTPUT);
//  digitalWrite(53, HIGH);
  delay(50);
//  digitalWrite(53, LOW);
  delay(50);
//  digitalWrite(53, HIGH);
  delay(50);
//  SPI.begin();
 while(1){
#ifdef USE_TEST_CARD
  testCard();
#endif
#ifdef USE_TEST_CARD_SD
  testCard_SD();
#endif
#ifdef USE_PRINT_DIRECTORY
  testCard_SD();
#endif
  SPI.end();
  digitalWrite(PIN_MISO, LOW);
  pinMode(PIN_MISO, OUTPUT);
  delay(1000);
 }
}


void testCard_SD(void){
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
//  SD.end();
//  if (!SD.begin(chipSelect)) {
  if (!SD.begin(SPI_HALF_SPEED, chipSelect)) {
#ifdef SD_ERROR_READ_IMPLEMENTED
    Serial.print("Card failed, or not present"); Serial.print("Err-#: "); Serial.print(SD.error()); Serial.println();
#else
    Serial.print("Card failed, or not present"); /*Serial.print("Err-#: "); Serial.print(SD.error()); */Serial.println();
#endif
    // don't do anything more:
//    digitalWrite(chipSelect, LOW); // Add this line
//    delay(50);
//    digitalWrite(chipSelect, HIGH); // Add this line
//    delay(50);

//    while (1);
  }
  else{
    Serial.println("card initialized.");

#ifdef USE_PRINT_DIRECTORY

    root = SD.open("/");
  
    printDirectory(root, 0);
  
    Serial.println("done!");
#endif    
  }
}

#ifdef USE_TEST_CARD

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;


void testCard(){

  Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
//  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.println();
  Serial.print("Card type:         ");
  switch (card.type()) {
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
    while (1);
  }

  Serial.print("Clusters:          ");
  Serial.println(volume.clusterCount());
  Serial.print("Blocks x Cluster:  ");
  Serial.println(volume.blocksPerCluster());

  Serial.print("Total Blocks:      ");
  Serial.println(volume.blocksPerCluster() * volume.clusterCount());
  Serial.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:    FAT");
  Serial.println(volume.fatType(), DEC);

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  Serial.print("Volume size (Kb):  ");
  Serial.println(volumesize);
  Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  root.close();

  Serial.println("--- ENDE ---");
  // https://www.ulrichradig.de/forum/viewtopic.php?f=47&t=1981
  
  //https://forum.arduino.cc/t/sd-verzeichnis-als-liste-lesen/473526/3
  //Verzeichnis SD lesen
  root.openRoot(volume);
  dir_t p;
  
  root.rewind();
  
  while (root.readDir(p) > 0) {
  
    if (p.name[0] == DIR_NAME_FREE) break;
  
    if (p.name[0] == DIR_NAME_DELETED || p.name[0] == '.') continue;
  
    if (!DIR_IS_FILE_OR_SUBDIR(&p)) continue;
  
    for (uint8_t i = 0; i < 11; i++) 
    {
  
      if (p.name[i] == ' ') continue;
  
      if (i == 8) 
      {
        Serial.print('.');
      }  
  
      Serial.print((char)p.name[i]);     
    }
  
    Serial.write('\n');// Terminator
  }
}
#endif

#ifdef USE_PRINT_DIRECTORY
void printDirectory(File dir, int numTabs) {

  while (true) {

    File entry =  dir.openNextFile();

    if (! entry) {

      // no more files

      break;

    }

    for (uint8_t i = 0; i < numTabs; i++) {

      Serial.print('\t');

    }

    Serial.print(entry.name());

    if (entry.isDirectory()) {

      Serial.println("/");

      printDirectory(entry, numTabs + 1);

    } else {

      // files have sizes, directories do not

      Serial.print("\t\t");

      Serial.println(entry.size(), DEC);

    }

    entry.close();

  }
}
#endif

void loop(void) {
}
