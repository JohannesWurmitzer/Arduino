void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);   // Interface to PC
  Serial1.begin(19200);  // Interface to SIM900

  Serial.println("PC Interface ready");
}

void loop() {
  byte bySerRx;
  
  // put your main code here, to run repeatedly:
  while(Serial.available()){
    bySerRx = Serial.read();
    if (bySerRx == 'X'){
      SendMessage();
      delay(5000);
      Serial.println("Hopefully sent...");
    }
    else{
      Serial1.write(bySerRx);
    }
  }  
  while(Serial1.available()){
    bySerRx = Serial1.read();
    Serial.write(bySerRx);
  }  
}


void SendMessage()
{
  Serial1.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second

  Serial1.println("AT+CMGS=\"+4367684135313\"\r"); // Replace x with mobile number
  delay(1000);

  Serial1.println("LOW CGPA DUDE....");// The SMS text you want to send
  delay(100);
   Serial1.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
