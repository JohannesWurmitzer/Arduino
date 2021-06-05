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

#define ITB                         // define, if ITB ist used to test

#ifdef ITB
#define PO_HA_GSM900_PWRKEY   23    // PWRKEY on ITB1
#else
#define PO_HA_GSM900_PWRKEY   9       // PWRKEY if Shield mounted direclty to Arduino UNO or Mega
#endif

void WelAccessSim900Init(void);
void WelAccessSim900GetTime(void);
void WelAccessSetupInternet(void);
void WelAccessHttpGet(void);
void WelAccessNtp(void);


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
  delay(500);
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

  WelAccessSim900Init();
  WelAccessSim900GetTime();
  WelAccessSetupInternet();
//  WelAccessHttpGet();
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
void ShowSerialData(unsigned short usDelay){
  unsigned long ulMillisEntry = millis();
  do{
    while (Serial3.available() != 0)
      Serial.write(Serial3.read())
    ;
/*
  while(Serial3.available()){
    bySerRx = Serial3.read();
    Serial.write(bySerRx);
  }
*/
  }while ((millis()-ulMillisEntry) < usDelay);
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

static String strZMKomAus;        // Zustandsmaschine Ausgangsdaten

void WelAccessSim900Init(void){
  Serial.println("WelAccessSim900Init");
  strZMKomAus = F("AT+IPR?");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

}

void WelAccessSim900GetTime(void){
  Serial.println("WelAccessGsmGetTime");
  strZMKomAus = F("AT+CCLK?");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);
  
  strZMKomAus = F("AT+CLTS=?");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  strZMKomAus = F("AT+CLTS=1");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  strZMKomAus = F("AT&W");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);
  
  strZMKomAus = F("AT+CLTS?");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  strZMKomAus = F("AT+CCLK?");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

}

void WelAccessSetupInternet(void){
  Serial.println("WelAccessSetupInternet");
  strZMKomAus = F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");    // \" inserts " into the string
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  // APN
  
  // HoT
  strZMKomAus = F("AT+SAPBR=3,1,\"APN\",\"webaut\"");
  // Standardzugang T-Mobile
  strZMKomAus = F("AT+SAPBR=3,1,\"APN\",\"gprsinternet\"");
  // Standardzugang T-Mobile M2M - APN
  strZMKomAus = F("AT+SAPBR=3,1,\"APN\",\"m2m.public.at\"");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  // USER
  
  // HoT
  strZMKomAus = F("AT+SAPBR=3,1,\"USER\",\"\"");  
  // Standardzugang T-Mobile
  strZMKomAus = F("AT+SAPBR=3,1,\"USER\",\"t-mobile\"");
  // Standardzugang T-Mobile M2M - Username
  strZMKomAus = F("AT+SAPBR=3,1,\"USER\",\"\"");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  // PW

  // HoT
  strZMKomAus = F("AT+SAPBR=3,1,\"PWD\",\"\"");
  // Standardzugang T-Mobile
  strZMKomAus = F("AT+SAPBR=3,1,\"PWD\",\"tm\"");
  // Standardzugang T-Mobile M2M - Passwort
  strZMKomAus = F("AT+SAPBR=3,1,\"PWD\",\"\"");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  // FTP/HTTP Verbindung prüfen (Bearer open)
  strZMKomAus = F("AT+SAPBR=2, 1");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  // GPRS FTP / HTTP: Verbindung aufbauen, Wichtig: ERROR auch, wenn die Verbindung schon steht!
  strZMKomAus = F("AT+SAPBR=1,1");
  // Überwachungszeiten vergrößern
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(5000);

  // FTP/HTTP Verbindung prüfen (Bearer open)
  strZMKomAus = F("AT+SAPBR=2, 1");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  WelAccessNtp();
//  WelAccessHttpGet();
  

  // GPRS FTP / HTTP: Verbindung trennen
  strZMKomAus = F("AT+SAPBR=0,1");
  // Überwachungszeiten vergrößern
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(5000);

}

void WelAccessHttpGet(void){
  Serial.println("WelAccessHttpGet");

  strZMKomAus = F("AT+HTTPINIT");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(5000);

  strZMKomAus = F("AT+HTTPPARA=\"CID\",1");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  // GET
  strZMKomAus = F("AT+HTTPPARA=\"URL\",\"http://welando.wurmitzer.net:8080/wel-access-server/iot-message/list\"");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  strZMKomAus = F("AT+HTTPACTION=0");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(5000);

  strZMKomAus = F("AT+HTTPREAD=0,4096");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(3000);


  // POST
  strZMKomAus = F("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(2000);

  strZMKomAus = F("AT+HTTPPARA=\"URL\",\"http://welando.wurmitzer.net:8080/wel-access-server/iot-message/create\"");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);

  String strJason = F("{    \"iotClientUid\": \"861508032676678\",    \"timestampIotClient\": \"2020-11-24T11:28:12\",    \"version\": \"1.03\",    \"type\": \"info\",    \"domain\": \"system\",    \"message\": \"powerup firmware\"    }  ");
  strZMKomAus = String(F("AT+HTTPDATA=")) + String(strJason.length()) + String(F(",100000"));
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(6000);

  strZMKomAus = strJason;
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(6000);
  
  strZMKomAus = F("AT+HTTPACTION=1");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(5000);

  strZMKomAus = F("AT+HTTPREAD=0,4096");
  strZMKomAus = F("AT+HTTPREAD");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(3000);

  // Terminate HTTP connection
  strZMKomAus = F("AT+HTTPTERM");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(5000);
  
}

void WelAccessNtp(void){
  strZMKomAus = F("AT+CNTP?");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);
  
  strZMKomAus = F("AT+CNTP=\"pool.ntp.org\",8,1,2");
  strZMKomAus = F("AT+CNTP=\"202.120.2.101\",8");
  strZMKomAus = F("AT+CNTP=\"europe.pool.ntp.org\",8");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(1000);
  
  strZMKomAus = F("AT+CNTP");
  Serial.println(strZMKomAus);
  Serial3.println(strZMKomAus);
  ShowSerialData(2000);
  
}
