//Choose UART GPIO pins for Serial1
#define SERIAL0_RX_GPIO 16    // Serial 0 Rx Pin, 3 default
#define SERIAL0_TX_GPIO 17    // Serial 0 Tx Pin, 1 default
#define SERIAL1_RX_GPIO 4     // Serial 1 Rx Pin, 9 default in conflict with internal SPI Flash, so better change it
#define SERIAL1_TX_GPIO 2     // Serial 1 Tx Pin, 10 default in conflict with internal SPI Flash, so better change it
#define SERIAL2_RX_GPIO 16    // Serial 2 Rx Pin, 16 default
#define SERIAL2_TX_GPIO 17    // Serial 2 Tx Pin, 17 default

//HardwareSerial Serial1(1);    // wrong, or not needed
//HardwareSerial Serial2(2);    // wrong, or not needed


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("ESP_UART Scatch running");
//  pinMode(2, OUTPUT);
  Serial1.begin(19200,SERIAL_8N1,SERIAL1_RX_GPIO,SERIAL1_TX_GPIO,false,20000UL); 
  Serial2.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
//  digitalWrite(2, HIGH);
  delay(1000);
  Serial.print("Hello again - ");
  Serial.println("Aca aka ninja!");
//  digitalWrite(2, LOW);

  Serial1.println("Serial 1 sending");
  Serial2.println("Serial 2 sending");
  delay(500);
  
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
