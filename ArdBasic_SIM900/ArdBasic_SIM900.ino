// https://www.diafaan.com/sms-tutorials/gsm-modem-tutorial/at-cmgl-text-mode/

// Serial - Buffer-Size
// https://internetofhomethings.com/homethings/?p=927
/*
 * 
 * AppData\Local\Arduino15\packages\arduino\hardware\avr\1.6.11\cores\arduino\HardwareSerial.h
 * C:\Users\Johannes\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.2\cores\arduino\HardwareSerial.h
Software Serial Buffer Expansion
The change for software serial ports require a simple modification of the file:

<base Arduino folder>\hardware\arduino\avr\libraries\SoftwareSerial\SoftwareSerial.h

Change:

__#define _SS_MAX_RX_BUFF 64 // RX buffer size

To:

__#define _SS_MAX_RX_BUFF 256 // RX buffer size


Hardware Serial Buffer Expansion
The change for hardware serial ports require a simple modification of the file:

<base Arduino folder>\hardware\arduino\avr\cores\arduino\HardwareSerial.h

Change:

__#define SERIAL_TX_BUFFER_SIZE 64
__#define SERIAL_RX_BUFFER_SIZE 64

To:

__#define SERIAL_TX_BUFFER_SIZE 256
__#define SERIAL_RX_BUFFER_SIZE 256

*/
// AT+CREG?
//
// AT+CMGL="ALL"
//
// 
// +CMTI: "SM",2<\r><\n>
//
// +CMTI: "SM",3<\r><\n>


#define PO_HA_GSM900_PWRKEY   9       // PWRKEY if Shield mounted direclty to Arduino UNO or Mega
//#define PO_HA_GSM900_PWRKEY   23    // PWRKEY on ITB1

void setup() {
   byte bySerRx;

  pinMode(PO_HA_GSM900_PWRKEY, OUTPUT);
  digitalWrite(PO_HA_GSM900_PWRKEY, LOW);
  
  // put your setup code here, to run once:
  Serial.begin(115200);   // Interface to PC
  Serial.println("PC Interface ready");
  
  Serial3.begin(19200);  // Interface to SIM900
  Serial.println("SIM900 Interface ready");
  Serial.print("Serial Buffer Size = ");
  Serial.println(Serial3.available());

  Serial.println("GSM900; Check");
  Serial3.setTimeout(500);
  Serial3.println("AT");
  while(!Serial3.find("OK")){
    Serial.println("GSM900; Not OK");
    digitalWrite(PO_HA_GSM900_PWRKEY, HIGH);
    delay(1000);
    digitalWrite(PO_HA_GSM900_PWRKEY, LOW);
    delay(1000);

    while(Serial3.available()){
      bySerRx = Serial3.read();
      Serial.write(bySerRx);
    }
    
    Serial3.println("AT");
  }
  while(Serial3.available()){
    bySerRx = Serial3.read();
    Serial.write(bySerRx);
  }
  Serial.println();
  Serial.println("GSM900; OK");

  delay(500); 
}

void loop() {
  byte bySerRx;
 
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    delay(100);
    Serial.print("Serial bytes available = ");
    Serial.println(Serial.available());
    
    while(Serial.available()){
      bySerRx = Serial.read();
      if (bySerRx == 'X'){
        SendMessage();
        delay(5000);
        Serial.println("Hopefully sent...");
      }
      else{
        Serial3.write(bySerRx);
      }
    }  
  }

  while(Serial3.available()){
    bySerRx = Serial3.read();
    Serial.write(bySerRx);
  }
//  delay(1500);
//  Serial3.println("AT");
}


void SendMessage()
{
  Serial3.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second

  Serial3.println("AT+CMGS=\"+4367684135313\"\r"); // Replace x with mobile number
//  Serial3.println("AT+CMGS=\"+436764158789\"\r"); // Replace x with mobile number
  delay(1000);

  Serial3.println("UrbanSharing;ITB;LifeCheck");// The SMS text you want to send
  delay(100);
   Serial3.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
