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
#define SERIAL2_RX_GPIO 15    // 16    // Serial 2 Rx Pin, 16 default
#define SERIAL2_TX_GPIO 14    // 17    // Serial 2 Tx Pin, 17 default
//#define GPIO_UART2_EXP_ADDR0  14  // was planned, but now used for Serial 2 Tx because of using the WROVER module, which uses the dedicated UART2 Tx pin internally
//#define GPIO_UART2_EXP_ADDR1  15  // was planned, but now used for Serial 2 Rx because of using the WROVER module, which uses the dedicated UART2 Rx pin internally

// GPIO ports for SD card
#define GPIO_SD_CARD_SPI_SS      5    // SD card slave select
#define GPIO_SD_CARD_SPI_CLK    18    // SD card SPI MISO
#define GPIO_SD_CARD_SPI_MISO   19    // SD card SPI MISO
#define GPIO_SD_CARD_SPI_MOSI   23    // SD card SPI MOSI

#define GPIO_BUZZER       12  // Buzzer output


//
//  xGPIO shift register definitions
//
#define GPIO_XIOSR_CLK    13  // Shift register Clk
#define GPIO_XIOSR_MOSI   27  // Shift register MOSI
#define GPIO_XIOSR_MISO   34  // Shift register MOSI
#define GPIO_XIOSR_LE     32  // Output Latch Output Enable Clk L->H latch outputs
#define GPIO_XIOSR_LEI    33  // Output Latch Input Enable Clk H->L latch inputs

//
// xGPIO definitions
//
// xGPI 3v3
#define XGPI_00     0     // Input 0 3v3
#define XGPI_01     1     // Input 1 3v3
#define XGPI_02     2     // Input 2 3v3
#define XGPI_03     3     // Input 3 3v3
#define XGPI_04     4     // Input 4 3v3
#define XGPI_05     5     // Input 5 3v3
#define XGPI_06     6     // Input 6 3v3
#define XGPI_07     7     // Input 7 3v3

// xGPO 3v3
#define XGPO_00     0     // Output 0 3v3
#define XGPO_01     1     // Output 1 3v3
#define XGPO_02     2     // Output 2 3v3
#define XGPO_03     3     // Output 3 3v3
#define XGPO_04     4     // Output 4 3v3
#define XGPO_05     5     // Output 5 3v3
#define XGPO_06     6     // Output 6 3v3
#define XGPO_07     7     // Output 7 3v3

// xGPO 5v0
#define XGPO_08     8     // Output 8 5v0
#define XGPO_09     9     // Output 9 5v0
#define XGPO_10     10    // Output 10 5v0
#define XGPO_11     11    // Output 11 5v0
#define XGPO_12     12    // Output 12 5v0
#define XGPO_13     13    // Output 13 5v0
#define XGPO_14     14    // Output 14 5v0
#define XGPO_15     15    // Output 15 5v0

#define XGPO_16     16    // Output 16 5v0
#define XGPO_17     17    // Output 17 5v0
#define XGPO_18     18    // Output 18 5v0
#define XGPO_19     19    // Output 19 5v0
#define XGPO_20     20    // Output 20 5v0
#define XGPO_21     21    // Output 21 5v0
#define XGPO_22     22    // Output 22 5v0
#define XGPO_23     23    // Output 23 5v0

//
// Application I/O defines
//

#define GPIO_XI_SL032_OUT         ((uint32_t)(1 << XGPI_0))     // SL032 OUT - article rfid within range
#define GPIO_XI_SL030_OUT         ((uint32_t)(1 << XGPI_1))     // SL030 OUT - user rfid within range

#define GPIO_XI_KEYLOCK_CLOSED    ((uint32_t)(1 << XGPI_4))     // Keylock status closed

#define GPIO_XI_SD_CARD_INSERT    ((uint32_t)(1 << XGPI_7))     // SD CARD_INSERT

#define GPIO_XO_RS485_TX_ON       ((uint32_t)(1 << XGPO_0))     // RS485 Tx On
#define GPIO_XO_RS485_RX_ON       ((uint32_t)(1 << XGPO_1))     // RS485 Rx On
#define GPIO_XO_SL032_IN          ((uint32_t)(1 << XGPI_2))     // SL032 IN - activate reader
#define GPIO_XO_SL030_IN          ((uint32_t)(1 << XGPI_3))     // SL030 IN - activate reader

#define GPIO_XO_UART_MUX_ADDR0    ((uint32_t)(1 << XGPO_6))     // UART MUX ADDR0
#define GPIO_XO_UART_MUX_ADDR1    ((uint32_t)(1 << XGPO_7))     // UART MUX ADDR1
#define GPIO_XO_GSM_POWER_ON      ((uint32_t)(1 << XGPO_8))     // GSM Module Power On
#define GPIO_XO_GSM_POWER_RST     ((uint32_t)(1 << XGPO_9))     // GSM Module Reset
#define GPIO_XO_GSM_POWER_BTN     ((uint32_t)(1 << XGPO_10))    // GSM Module Power Button (Pin 9)

#define GPIO_XO_LED_G             ((uint32_t)(1 << XGPO_12))    // LED green
#define GPIO_XO_LED_R             ((uint32_t)(1 << XGPO_13))    // LED red

#define GPIO_XO_KEYLOCK_CMD_OPEN  ((uint32_t)(1 << XGPO_16))    // Keylock Cmd Open
#define GPIO_XO_KEYLOCK_LED_GREEN ((uint32_t)(1 << XGPO_17))    // Keylock LED Green
#define GPIO_XO_KEYLOCK_LED_RED   ((uint32_t)(1 << XGPO_18))    // (reserved) Keylock LED Red

#define GPIO_XO_SET(x)            (xGpioOutputMirror|=x)        //
#define GPIO_XO_RESET(x)          (xGpioOutputMirror&=~x)       //

//HardwareSerial Serial1(1);    // wrong, or not needed
//HardwareSerial Serial2(2);    // wrong, or not needed

uint8_t   xGpioInputMirror;
uint32_t  xGpioOutputMirror;

void xGpioWrite(uint32_t u32OutputValues);

#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22

#if 0
int i2c_master_port = 0;
i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,         // select GPIO specific to your project
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = I2C_MASTER_SCL_IO,         // select GPIO specific to your project
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,  // select frequency specific to your project
    // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
};
#endif

void setup() {
  // setup serial ports
  // void begin(unsigned long baud, uint32_t config=SERIAL_8N1, int8_t rxPin=-1, int8_t txPin=-1, bool invert=false, unsigned long timeout_ms = 20000UL, uint8_t rxfifo_full_thrhd = 112);

  // UART0 - PC application communication
  Serial.begin(115200, SERIAL_8N1, SERIAL0_RX_GPIO, SERIAL0_TX_GPIO, false, 20000L);
  Serial.println("#INF ESP32_ITB-IoT-ESP32-WROOM-32 Scatch running");

  // UART1 - GSM module communication
  Serial1.begin(19200, SERIAL_8N1, SERIAL1_RX_GPIO, SERIAL1_TX_GPIO, false, 20000UL); 

  // UART2 - MUX UART
  Serial2.begin(115200, SERIAL_8N1, SERIAL2_RX_GPIO, SERIAL2_TX_GPIO, false, 20000UL);
  //pinMode(GPIO_UART2_EXP_ADDR0, OUTPUT);
  //digitalWrite(GPIO_UART2_EXP_ADDR0, LOW);
  //pinMode(GPIO_UART2_EXP_ADDR1, OUTPUT);
  //digitalWrite(GPIO_UART2_EXP_ADDR1, LOW);

//  pinMode(I2C_MASTER_SDA_IO, INPUT_PULLUP);
//  pinMode(I2C_MASTER_SCL_IO, INPUT_PULLUP);
  setup_TCA9544();


  // Buzzer output 5V
  pinMode(GPIO_BUZZER, OUTPUT);

//#if 0
  // XIO shift register
  //PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_XIOSR_CLK], PIN_FUNC_GPIO);
  pinMode(GPIO_XIOSR_CLK, OUTPUT);
  digitalWrite(GPIO_XIOSR_CLK, LOW);
  pinMode(GPIO_XIOSR_MOSI, OUTPUT);
  digitalWrite(GPIO_XIOSR_MOSI, LOW);
  pinMode(GPIO_XIOSR_LE, OUTPUT);
  digitalWrite(GPIO_XIOSR_LE, LOW);
  
  pinMode(GPIO_XIOSR_MISO, INPUT);
  pinMode(GPIO_XIOSR_LEI, OUTPUT);
  digitalWrite(GPIO_XIOSR_LEI, HIGH);

  // set all xGPIO outputs low, which is standard
  Serial.println("set all xGPIO to low");
  xGpioOutputMirror = 0x00000000;
  xGpioWrite(xGpioOutputMirror);
  delay(5000);
  Serial.println("set all xGPIO to startup state");
  GPIO_XO_SET(GPIO_XO_LED_G);
  xGpioWrite(xGpioOutputMirror);
  Serial.println("init finished");
  delay(1000);
//#endif  
  while(1){
//#if 0
    GPIO_XO_SET(GPIO_XO_LED_R);
    xGpioWrite(xGpioOutputMirror);
    delay(500);
    GPIO_XO_RESET(GPIO_XO_LED_R);
    xGpioWrite(xGpioOutputMirror);

    Serial.print("xInput: "); Serial.println(xGpioRead(),HEX);

    Serial.println("setup loop");
    if (Serial1.available()){
      Serial.print("Data Serial-1: ");
      while (Serial1.available()){
        Serial.write(Serial1.read());
      }
    }
    delay(2000);
//#endif
    Serial.println("Test I2C-Mux");
//  loop_i2cScanner();
    loop_TCA9544();
    delay(1000);
  }
  
  //  pinMode(2, OUTPUT);

//  setup_i2cScanner();
  setup_TCA9544();

}

void loop() {
  static uint8_t u8Uart2MuxSet = 0;
  uint16_t i;
  uint8_t u8BuzzerLoopCnt;
  uint8_t u8XioSrClkCnt;
  
  Serial.println("Test SD-Card");
  testSdCard();

  Serial.println("Test I2C-Mux");
//  loop_i2cScanner();
  loop_TCA9544();
  
  Serial.println("Test Buzzer");
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

  Serial.println("Test xGPIO");
//  while(1){
    Serial.print("xInput: "); Serial.println(xGpioRead());
    delay(1000);
//  }
  xGpioWrite(0xFFFFFF);
  delay(1000);  
  for (i=0; i < 65535; i++){
    xGpioWrite(i+1);
//    delay(1);
  }
  i = 0;
  xGpioWrite(i);
  Serial.println("Test xGPIO end");
  delay(100);
#if 0
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
#endif

//  digitalWrite(2, HIGH);

  delay(10);
//#if 0  
  delay(1000);
  Serial.println("Test UART-Mux");
  Serial.print("Serial 0 sending: Hello again - ");
  Serial.println("Aca aka ninja!");
//  digitalWrite(2, LOW);

  delay(100);
  Serial1.println("Serial 1 sending");

//  delay(500);
//#endif  
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
  for (u8Uart2MuxSet = 0; u8Uart2MuxSet < 4; u8Uart2MuxSet++){
    u8Uart2MuxSet = u8Uart2MuxSet % 4;
    //if (u8Uart2MuxSet & 0x01){digitalWrite(GPIO_UART2_EXP_ADDR0, HIGH);} else{digitalWrite(GPIO_UART2_EXP_ADDR0, LOW);};
    //if (u8Uart2MuxSet & 0x02){digitalWrite(GPIO_UART2_EXP_ADDR1, HIGH);} else{digitalWrite(GPIO_UART2_EXP_ADDR1, LOW);};
    if (u8Uart2MuxSet & 0x01){i=0x40;} else{i=0;};
    if (u8Uart2MuxSet & 0x02){i+=0x80;} else{i=i+0;};
    xGpioWrite(i);
    delay(10);
    while (Serial2.available()) Serial2.read();
    Serial.print("MUX-Serial 2: "); Serial.println(u8Uart2MuxSet); Serial2.println(" Serial 2 sending");
    delay(10);
    if (Serial2.available()){
      Serial.print("Data Serial-2: ");
      while (Serial2.available()){
  //      Serial.print(Serial2.read());
        Serial.write(Serial2.read());
      }
    }  
  }
  delay(5000);
}

#define PCA9544A  0   // default
#define PCA9545A  1   // test device

#define I2C_MUX_TYPE  PCA9544A  //  default
//#define I2C_MUX_TYPE  PCA9545A  // test device

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
//  Wire.setClock(10000);
  Serial.println(F("I2C-Scanner fuer TCA9544A"));
  Serial.println();
}

void loop_TCA9544() {
  bool tcaGefunden = false;
  uint8_t u8I2cError;
  for (byte tcaI2cAdd = 0x70; tcaI2cAdd < 0x70 + 4; tcaI2cAdd++){
  Serial.print(F("Scanne TCA9544A-Adresse:"));
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
      Serial.print(F(" 0x"));
      Serial.print(tcaI2cAdd, HEX);
      Serial.print(" Fehler: "); Serial.println(u8I2cError);
    }
  }
  if (!tcaGefunden) Serial.println(F("TCA9544A nicht gefunden"));
  Serial.println();
//  delay(1000);
}

void scanneTCA(byte tcaAdd){ 
  bool adresseGefunden = false;
  for (byte channel = 0; channel < 4; channel++){
    Wire.beginTransmission(tcaAdd);
#if I2C_MUX_TYPE == PCA9544A
    Wire.write(channel + 0x04);
#elif I2C_MUX_TYPE == PCA9545A
    Wire.write(1 << channel);
#else
  
#endif
    Wire.endTransmission();
    Serial.print("Kanal: "); Serial.println(channel);
    for (byte add = 0; add < 128; add++)
    {
      if (add != tcaAdd)
      {
        Wire.beginTransmission(add);
        if (Wire.endTransmission() == 0)
        {
          adresseGefunden = true;
          Serial.print(F("Kanal: "));
          Serial.print(channel);
          Serial.print(F(": Adresse: 0x"));
          Serial.print(add, HEX);
          Serial.println(F(" gefunden"));
        }
      }
    }
  }
  if (!adresseGefunden) Serial.println(F("Keine Adresse gefunden"));
}

#define SD_CS   GPIO_SD_CARD_SPI_SS         // card CS

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


// XGPIO

#define XIO_PORT_CNT        3
#define XIO_PORT_PINS_CNT   8

void xGpioInit(void){
  pinMode(GPIO_XIOSR_CLK, OUTPUT);
  digitalWrite(GPIO_XIOSR_CLK, LOW);
  pinMode(GPIO_XIOSR_MOSI, OUTPUT);
  digitalWrite(GPIO_XIOSR_MOSI, LOW);
  pinMode(GPIO_XIOSR_MISO, INPUT);
  pinMode(GPIO_XIOSR_LE, OUTPUT);
  digitalWrite(GPIO_XIOSR_LE, LOW);
}

void xGpioWrite(uint32_t u32OutputValues){
  uint8_t u8XioSrClkCnt;
  uint8_t toggle;
//  u32OutputValues = 0x55555555;
  for (u8XioSrClkCnt = 0; u8XioSrClkCnt < 24; u8XioSrClkCnt++){
    if (u32OutputValues & 0x0800000){
      digitalWrite(GPIO_XIOSR_MOSI, HIGH);
      toggle = 0;
    }
    else{
      digitalWrite(GPIO_XIOSR_MOSI, LOW);
      toggle = 1;
    }
    u32OutputValues = u32OutputValues << 1;
    digitalWrite(GPIO_XIOSR_CLK, LOW);
//  delay(1);
    digitalWrite(GPIO_XIOSR_CLK, HIGH);
//  delay(1);
//    digitalWrite(GPIO_XIOSR_CLK, LOW);
  }
  // output the latched values
  digitalWrite(GPIO_XIOSR_LE, LOW);
  digitalWrite(GPIO_XIOSR_LE, HIGH);      // latch outputs at rising edge?
//  digitalWrite(GPIO_XIOSR_LE, LOW);
}

unsigned char xGpioRead(void){
  uint8_t u8XioSrClkCnt;
  uint8_t lu8Input;

  // latched input values
  digitalWrite(GPIO_XIOSR_LEI, LOW);    // load input (not edge triggered, level triggered)
  digitalWrite(GPIO_XIOSR_LEI, HIGH);   // release latch inputs again to enable shift

  digitalWrite(GPIO_XIOSR_CLK, LOW);
  lu8Input = 0;
  for (u8XioSrClkCnt = 0; u8XioSrClkCnt < 8; u8XioSrClkCnt++){
    digitalWrite(GPIO_XIOSR_CLK, LOW);
    lu8Input = lu8Input << 1;
    if (digitalRead(GPIO_XIOSR_MISO)){
      lu8Input = lu8Input | 0x01;
    }
    digitalWrite(GPIO_XIOSR_CLK, HIGH); // clock next
  }
  return lu8Input;
}
