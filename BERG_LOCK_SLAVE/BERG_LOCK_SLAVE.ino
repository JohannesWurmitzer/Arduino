#include <Wire.h>       // I2C Library

#define SERIAL_DEBUG_ENABLE

#define SERIAL_OUTPUT_ENABLE

#define RFID                    // define, if RFID is used
#define RS485_SEND                // define, if RS-485 should send something
//#define RS485_TEST                // define, if RS-485 should send something

#define POLA_BRIDGE_LOCK_A  2
#define POLA_BRIDGE_LOCK_B  3

#define PILA_LOCK_CLOSED  4     // Input using internal pullup resistor

#define POHA_RS485_DE     5     // RS-485 DE
#define POLA_RS485_RE     6     // RS-485 /RE


// Definition for SL030 I2C
#define SL030ADR 0xA0   // standard address for SL030
// StrongLink RFID Reader
boolean SL030readPassiveTargetID(uint8_t* puid, uint8_t* uidLength, uint8_t u8MaxLen);

static uint8_t RfIdArticel_uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
static uint8_t RfIdArticel_uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

static uint8_t u8LockStatus;  // 0 ... unkown, 1 ... closed, 2 ... open

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); 
  Serial.begin(115200);
  Serial.println("#I; Startup; BERG_LOCK_SLAVE");
  Serial.println("#I; Sys; LOSL; SW; 100");

  Serial.println("#I; I2C; Init NFC 1 OneWire I2C 400kHz");
  Wire.setClock(400000);
  Wire.begin();         // join i2c bus (address optional for master)

  Serial.println("#I; DIO; Init Bridge Lock");
  pinMode(POLA_BRIDGE_LOCK_A, OUTPUT);
  pinMode(POLA_BRIDGE_LOCK_B, OUTPUT);
  digitalWrite(POLA_BRIDGE_LOCK_A, HIGH); 
  digitalWrite(POLA_BRIDGE_LOCK_B, HIGH); 
//  digitalWrite(POLA_BRIDGE_LOCK_A, HIGH); 
//  digitalWrite(POLA_BRIDGE_LOCK_B, LOW);

  Serial.println("#I; DIO; Init Lock-Status Pullup");
  pinMode(PILA_LOCK_CLOSED, INPUT_PULLUP);


  Serial.println("#I; RS485; Init");
  pinMode(POHA_RS485_DE, OUTPUT);
  pinMode(POLA_RS485_RE, OUTPUT);
  digitalWrite(POLA_RS485_RE, HIGH);
  digitalWrite(POHA_RS485_DE, LOW);

}

void loop() {
  char chRS485;
  
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
#ifdef RFID
  if (SL030readPassiveTargetID(&RfIdArticel_uid[0], &RfIdArticel_uidLength, 50)){
    Serial.println("#I; RFID; Chip detected");
    Serial.print("#I; RFID; UidLen = ");
    Serial.print(RfIdArticel_uidLength);
    Serial.print("; Uid = ");
    Serial.print(RfIdArticel_uid[0], HEX); Serial.print(" ");
    Serial.print(RfIdArticel_uid[1], HEX); Serial.print(" ");
    Serial.print(RfIdArticel_uid[2], HEX); Serial.print(" ");
    Serial.print(RfIdArticel_uid[3], HEX);
    if (RfIdArticel_uidLength > 4){
      Serial.print(" ");
      Serial.print(RfIdArticel_uid[4], HEX); Serial.print(" ");
      Serial.print(RfIdArticel_uid[5], HEX); Serial.print(" ");
      Serial.print(RfIdArticel_uid[6], HEX);
    }
    Serial.println();
    // open LOCK
    digitalWrite(POLA_BRIDGE_LOCK_A, HIGH); 
    digitalWrite(POLA_BRIDGE_LOCK_B, LOW);
    // open Remote LOCK
#ifdef RS485_SEND
  digitalWrite(POHA_RS485_DE, HIGH);  // switch ON RS-485 data enable
  digitalWrite(POLA_RS485_RE, HIGH);  // no need for an echo
  Serial.println("#C; CMD = X");
  delay(250);
  Serial.println("#C; CMD = X");
  delay(250);
  Serial.println("#C; CMD = X");
  delay(250);
  Serial.println("#C; CMD = X");
  Serial.flush();     // transmit all outgoing data bytes
  digitalWrite(POHA_RS485_DE, LOW);   // switch OFF RS-485 data enable
  digitalWrite(POLA_RS485_RE, HIGH);
#endif 
  }
#endif
#ifdef RS485_TEST
  digitalWrite(POHA_RS485_DE, HIGH);
  digitalWrite(POLA_RS485_RE, LOW);
  Serial.println("Test RS-485");
//  Serial.println(Serial.availableForWrite()); // 63 bytes maximum
  Serial.flush();     // transmit all outgoing data bytes
//  Serial.println(Serial.availableForWrite());
//  while(Serial.availableForWrite()>0);  // wait till all is sent
  digitalWrite(POLA_RS485_RE, HIGH);
  digitalWrite(POHA_RS485_DE, LOW);
#endif
  while(Serial.available()>0){
    chRS485 = Serial.read();
    if (chRS485 == 'X'){
      // open LOCK
      digitalWrite(POLA_BRIDGE_LOCK_A, HIGH); 
      digitalWrite(POLA_BRIDGE_LOCK_B, LOW);
    }
    //Serial.print((char) chRS485);
  }

  Serial.flush();
  digitalWrite(POLA_RS485_RE, LOW);   // READ RS-485
  delay(500);
  digitalWrite(POLA_RS485_RE, HIGH);   // READ RS-485
  
  if (digitalRead(PILA_LOCK_CLOSED)){
    if (u8LockStatus != 2){
      u8LockStatus = 2;
      Serial.println("#I; LOCK; Open");
    }
  }
  else{
    if (u8LockStatus != 1){
      u8LockStatus = 1;
        Serial.println("#I; LOCK; Closed");
    }
  }
  Serial.print("#I; SysTick; ");
  Serial.println(millis());
  digitalWrite(POLA_BRIDGE_LOCK_A, HIGH); 
  digitalWrite(POLA_BRIDGE_LOCK_B, HIGH); 
}




//
//  RFID Reader StrongLink
//
//
#define PI_SL032_OUT 17
boolean SL030readPassiveTargetID(uint8_t* puid, uint8_t* uidLength, uint8_t u8MaxLen)
{
  unsigned char u8Len;
  unsigned char u8ProtNr;
  unsigned char u8Status;
  for (u8Len = 0; u8Len < 10; u8Len++){
    if (digitalRead(PI_SL032_OUT)){
      return(false);
      delay(1);
    }
  }
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
