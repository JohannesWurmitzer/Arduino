#include "optiboot.h"

uint16_t u16Idx;
uint16_t u16Page;
uint8_t storage_array[256];

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Reset V6");
  Serial.println("Write page 32");
  
  memset(storage_array, 0x01, sizeof(storage_array));
  //optiboot_page_erase((uint32_t)32*256);               // Page-Write calls page_erase
  optiboot_writePage((uint32_t)32*256, storage_array, 1);  

  Serial.println("Read whole programm memory");
//  Serial.write("PROGMEM_PAGE_SIZE: "); Serial.write(PROGMEM_PAGE_SIZE); Serial.println();
  memset(storage_array, 0x00, sizeof(storage_array));
  for (u16Page = 0; u16Page < 1024; u16Page++){
    Serial.print("Page: "); Serial.println(u16Page);
    optiboot_readPage((uint32_t) u16Page * 256, storage_array, 1);    // funny, page numbers begin with 1, not 0
    for (u16Idx = 0; u16Idx < 256; u16Idx++){
      if (storage_array[u16Idx] < 0x10){
        Serial.print("0");
      }
      Serial.print(storage_array[u16Idx], HEX);
      //Serial.write(" ");
    }
    Serial.println();
  }
}

void loop() {
  static unsigned long ulMillisOld;
  unsigned long lulMillisTemp;
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, true);
  delay(250);
  digitalWrite(LED_BUILTIN, false);
  delay(250);
  digitalWrite(LED_BUILTIN, true);
  delay(250);
  digitalWrite(LED_BUILTIN, false);
  delay(750);
  Serial.print("Loop-Time: ");
  lulMillisTemp = millis();
  Serial.println(lulMillisTemp - ulMillisOld);
  ulMillisOld = lulMillisTemp;
}
