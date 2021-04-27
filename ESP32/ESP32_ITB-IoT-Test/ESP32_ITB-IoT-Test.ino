#include <Wire.h>
#include <SD.h>
#include <SPI.h>

//
// Port pin definition
//

// Choose UART GPIO pins for Serial
#define SERIAL0_RX_GPIO 3     // Serial 0 Rx Pin, 3 default
#define SERIAL0_TX_GPIO 1     // Serial 0 Tx Pin, 1 default

// Choose UART GPIO pins for Serial1
#define SERIAL1_RX_GPIO 4     // Serial 1 Rx Pin, 9 default in conflict with internal SPI Flash, so better change it
#define SERIAL1_TX_GPIO 2     // Serial 1 Tx Pin, 10 default in conflict with internal SPI Flash, so better change it

// Choose UART GPIO pins for Serial2
#define SERIAL2_RX_GPIO 16    // Serial 2 Rx Pin, 16 default
#define SERIAL2_TX_GPIO 17    // Serial 2 Tx Pin, 17 default

#define GPIO_BUZZER       12  // Buzzer output

#define GPIO_XIOSR_CLK    13  // Shift register Clk
#define GPIO_XIOSR_MOSI   27  // Shift register MOSI
#define GPIO_XIOSR_LE     32  // Output Latch Enable Clk


//HardwareSerial Serial1(1);    // wrong, or not needed
//HardwareSerial Serial2(2);    // wrong, or not needed


void setup() {
  // setup serial ports
  // void begin(unsigned long baud, uint32_t config=SERIAL_8N1, int8_t rxPin=-1, int8_t txPin=-1, bool invert=false, unsigned long timeout_ms = 20000UL, uint8_t rxfifo_full_thrhd = 112);

  // UART0 - PC application communication
  Serial.begin(115200, SERIAL_8N1, SERIAL0_RX_GPIO, SERIAL0_TX_GPIO, false, 20000L);
  Serial.println("#INF ESP32_ITB-IoT-ESP32-WROOM-32 Scatch running");

  // UART1 - GSM module communication
  Serial1.begin(19200,SERIAL_8N1,SERIAL1_RX_GPIO,SERIAL1_TX_GPIO,false,20000UL); 
  Serial2.begin(115200);

  // Buzzer output 5V
  pinMode(GPIO_BUZZER, OUTPUT);

  // XIO shift register
  //PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_XIOSR_CLK], PIN_FUNC_GPIO);
  pinMode(GPIO_XIOSR_CLK, OUTPUT);
  digitalWrite(GPIO_XIOSR_CLK, LOW);
  pinMode(GPIO_XIOSR_MOSI, OUTPUT);
  digitalWrite(GPIO_XIOSR_MOSI, LOW);
  pinMode(GPIO_XIOSR_LE, OUTPUT);
  digitalWrite(GPIO_XIOSR_LE, LOW);
  
  //  pinMode(2, OUTPUT);

//  setup_i2cScanner();
  setup_TCA9544();

}

void loop() {

  testSdCard();

//  loop_i2cScanner();
  loop_TCA9544();
  uint16_t i;
  uint8_t u8BuzzerLoopCnt;
  uint8_t u8XioSrClkCnt;
  
  // put your main code here, to run repeatedly:
  for (u8BuzzerLoopCnt = 0; u8BuzzerLoopCnt < 100; u8BuzzerLoopCnt++){
    digitalWrite(GPIO_BUZZER, HIGH);
//      digitalWrite(GPIO_XIOSR_MOSI, HIGH);
//      digitalWrite(GPIO_XIOSR_CLK, HIGH);
//      digitalWrite(GPIO_XIOSR_LE, HIGH);
    delay(1);
    digitalWrite(GPIO_BUZZER, LOW);
//      digitalWrite(GPIO_XIOSR_MOSI, LOW);
//      digitalWrite(GPIO_XIOSR_CLK, LOW);
//      digitalWrite(GPIO_XIOSR_LE, LOW);
    delay(1);
  }
  
  for (i = 0; i<10; i++){
    for (u8XioSrClkCnt = 0; u8XioSrClkCnt < 24; u8XioSrClkCnt++){
      digitalWrite(GPIO_XIOSR_MOSI, LOW);
      digitalWrite(GPIO_XIOSR_CLK, LOW);
//      delay(1);
      digitalWrite(GPIO_XIOSR_CLK, HIGH);
//      delay(1);
      digitalWrite(GPIO_XIOSR_CLK, LOW);
    }
    digitalWrite(GPIO_XIOSR_LE, LOW);
    digitalWrite(GPIO_XIOSR_LE, HIGH);
    digitalWrite(GPIO_XIOSR_LE, LOW);
    delay(1);
    for (u8XioSrClkCnt = 0; u8XioSrClkCnt < 24; u8XioSrClkCnt++){
      digitalWrite(GPIO_XIOSR_MOSI, HIGH);
      digitalWrite(GPIO_XIOSR_CLK, LOW);
//      delay(1);
      digitalWrite(GPIO_XIOSR_CLK, HIGH);
//      delay(1);
      digitalWrite(GPIO_XIOSR_CLK, LOW);
    }
    digitalWrite(GPIO_XIOSR_LE, LOW);
    digitalWrite(GPIO_XIOSR_LE, HIGH);
    digitalWrite(GPIO_XIOSR_LE, LOW);
    delay(2);
  }
//  digitalWrite(2, HIGH);

  delay(10);
#if 0  
  delay(1000);
  Serial.print("Hello again - ");
  Serial.println("Aca aka ninja!");
//  digitalWrite(2, LOW);

  Serial1.println("Serial 1 sending");
  Serial2.println("Serial 2 sending");
  delay(500);
#endif  
  if (Serial.available()){
    Serial.print("Data Serial-0: ");
    while (Serial.available()){
//      Serial.print(Serial.read());
      Serial.write(Serial.read());
    }
  }
  
  if (Serial1.available()){
    Serial.print("Data Serial-1: ");
    while (Serial1.available()){
//      Serial.print(Serial1.read());
      Serial.write(Serial1.read());
    }
  }
  
  if (Serial2.available()){
    Serial.print("Data Serial-2: ");
    while (Serial2.available()){
//      Serial.print(Serial2.read());
      Serial.write(Serial2.read());
    }
  }  
}

// https://randomnerdtutorials.com/esp32-i2c-communication-arduino-ide/

void setup_i2cScanner(){
  Wire.begin();
//  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
}


void loop_i2cScanner() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
//  delay(5000);          
}

// https://arduino-projekte.webnode.at/meine-libraries/i2c-multiplexer-tca9548a/

void setup_TCA9544() {
//  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
//  Wire.setClock(100000);
  Serial.println(F("I2C-Scanner fuer TCA9544A"));
  Serial.println();
}

void loop_TCA9544() {
  bool tcaGefunden = false;
  uint8_t u8I2cError;
  Serial.print(F("Scanne TCA9544A-Adressen:"));
  for (byte tcaI2cAdd = 0x70; tcaI2cAdd < 0x70+4; tcaI2cAdd++)
  {
    Wire.beginTransmission(tcaI2cAdd);
    u8I2cError = Wire.endTransmission();
    if (u8I2cError == 0){
      tcaGefunden = true;
      Serial.print(F(" 0x"));
      Serial.print(tcaI2cAdd, HEX);
      Serial.println(F(" gefunden"));
      Serial.println(F("Scanne Kanaele:"));
      scanneTCA(tcaI2cAdd);
    }
    else{
      Serial.print("Fehler: "); Serial.println(u8I2cError);
    }
  }
//  scanneTCA(0x70);
  if (!tcaGefunden) Serial.println(F("TCA9544A nicht gefunden"));
  Serial.println();
//  delay(1000);
}

void scanneTCA(byte tcaAdd)
{ 
  bool adresseGefunden = false;
  for (byte channel = 0; channel < 4; channel++)
  {
    Wire.beginTransmission(tcaAdd);
    Wire.write(1 << channel);
    Wire.endTransmission();
    Serial.print("Channel: "); Serial.println(channel);
    for (byte add = 0; add < 128; add++)
    {
      if (add != tcaAdd)
      {
        Wire.beginTransmission(add);
        if (Wire.endTransmission() == 0)
        {
          adresseGefunden = true;
          Serial.print(F("Kanal "));
          Serial.print(channel);
          Serial.print(F(": Adresse 0x"));
          Serial.print(add, HEX);
          Serial.println(F(" gefunden"));
        }
      }
    }
   }
  if (!adresseGefunden) Serial.println(F("Keine Adresse gefunden"));
}

#define SD_CS   5         // card CS

void testSdCard(void){
  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  } 
}
