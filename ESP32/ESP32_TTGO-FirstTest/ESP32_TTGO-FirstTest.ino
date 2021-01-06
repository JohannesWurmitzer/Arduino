/*
Factory output:
ets Jun  8 2016 00:22:57

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0018,len:4
load:0x3fff001c,len:1324
load:0x40078000,len:7788
ho 0 tail 12 room 4
load:0x40080400,len:6448
entry 0x400806e8
IP5306 KeepOn PASS
Setup Complete!
Found 9 networks
Begin SIM800 PASS
SIM800 Begin PASS
Test motor ...
Init success
AT+CHFA=1
AT+CRSL=100
AT+CLVL=100
AT+CLIP=1

==========
SIM OK
==========

==========
SIM OK
==========

==========
SIM OK
==========

==========
SIM OK
==========

Board Manager
  selected: "ESP32 Wrover Module"

*/
//#include <SoftwareSerial.h>
#define RXD2 16
#define TXD2 17
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22
//SoftwareSerial mySerial(MODEM_RX, MODEM_TX); // RX, TX

#define SerialAT Serial1

//Choose UART GPIO pins for Serial1
#define SERIAL0_RX_GPIO 16    // Serial 0 Rx Pin, 3 default
#define SERIAL0_TX_GPIO 17    // Serial 0 Tx Pin, 1 default
#define SERIAL1_RX_GPIO 26    // Serial 1 Rx Pin, 9 default in conflict with internal SPI Flash, so better change it
#define SERIAL1_TX_GPIO 27    // Serial 1 Tx Pin, 10 default in conflict with internal SPI Flash, so better change it
#define SERIAL2_RX_GPIO 16    // Serial 2 Rx Pin, 16 default
#define SERIAL2_TX_GPIO 17    // Serial 2 Tx Pin, 17 default


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello World");
  
  Serial1.begin(115200,SERIAL_8N1,SERIAL1_RX_GPIO,SERIAL1_TX_GPIO,false,20000UL); 
  
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  delay(1000);
  SerialAT.println("AT");
}

void loop() {
  // put your main code here, to run repeatedly:
//  delay(1000);
//  Serial.println("Hello again");

  if (SerialAT.available())
    Serial.write(SerialAT.read())
  ;
  if (Serial.available())
    SerialAT.write(Serial.read());
  ;
}
