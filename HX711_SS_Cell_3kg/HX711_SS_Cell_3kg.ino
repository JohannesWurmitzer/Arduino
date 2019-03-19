/* 
 *  (c) Maximilian Johannes Wurmitzer
 *  
 *  thanks to circuits4you.com for publishing the source
 *  
 *  HX711 Load Cell Amplifier Interface with Arduino
 *  https://www.hackster.io/MOHAN_CHANDALURU/hx711-load-cell-amplifier-interface-with-arduino-fa47f3
 *  
 *  Weight Sensor Load Cell 0-3 kg
 *  https://www.seeedstudio.com/Weight-Sensor-Load-Cell-0-3kg-p-2086.html
 *  
 *  
 *  History
 *  
 *  2018-11-05  V1  JoWu  Creation
 *  * 
 * 
*/

#include "HX711.h"  //You must have this library in your arduino library folder

#include <SoftwareSerial.h>
/*Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69*/
SoftwareSerial mySerial(50,51);   // RX, TX
String str;
#define RS485_TX_ENABLE  49         // RS485 TX Enable
#define HX711_DOUT  23               // 3 HX711 Data Out
#define HX711_CLK   22               // 2 HX711 Clock

const int AIA = 44;  // (pwm) pin 44(4) connected to pin A-IA
byte spd = 128;  // change this (0-255) to control the speed of the motors

HX711 scale(HX711_DOUT, HX711_CLK, 128);

//Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
float calibration_factor = -1600000;//-96650; //-106600 worked for my 40Kg max scale setup 
 
void setup() {
  // put your setup code here, to run once:

  // digital output for Pumps and Mixer
  pinMode(AIA, OUTPUT); // set pins to output
  analogWrite(AIA, 255-spd);
  delay(1000);
  analogWrite(AIA, 255-0);
  delay(1000);  
  analogWrite(AIA, 255-spd);
  delay(1000);

  // Serial Interface to SMT-100
  mySerial.begin (9600);
  pinMode(RS485_TX_ENABLE, OUTPUT);

  // Serial Interface to PC
  Serial.begin(115200);
  Serial.println("HX711 Calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press a,s,d,f to increase calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press z,x,c,v to decrease calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press t for tare");

  // Init Scale
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
 
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);


}

void loop() {
  // put your main code here, to run repeatedly:
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
 
  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 3);
  Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

/*  digitalWrite(RS485_TX_ENABLE, HIGH);
  mySerial.println("HELLO WORLD!");
  digitalWrite(RS485_TX_ENABLE, LOW);
*/  
  analogWrite(AIA, 255-spd);
  
  str = "GetFirmwareVersion!";
  digitalWrite(RS485_TX_ENABLE, HIGH);
  mySerial.println (str);
  digitalWrite(RS485_TX_ENABLE, LOW);
  delay(500);
  if (mySerial.available() > 0){
    str = mySerial.readStringUntil('\n');
    Serial.println(str);
  }
  else {
     Serial.println("no response from sensor");
  }         

  analogWrite(AIA, 255-0);
  delay(500);

   
  if(Serial.available()) {
    // Handle SMT-100
    str = Serial.readStringUntil('\n');
//    str = "GetFirmwareVersion!\r\n";
    mySerial.println (str);
    delay(500);
    if (mySerial.available() > 0){
      str = mySerial.readStringUntil('\n');
      Serial.println(str);
    } else {
       Serial.println("no response from sensor");
    }         

    // Handle Scale
    Serial.print("Input: ");
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
    else if(temp == 's')
      calibration_factor += 100;  
    else if(temp == 'x')
      calibration_factor -= 100;  
    else if(temp == 'd')
      calibration_factor += 1000;  
    else if(temp == 'c')
      calibration_factor -= 1000;
    else if(temp == 'f')
      calibration_factor += 10000;  
    else if(temp == 'v')
      calibration_factor -= 10000;  
    else if(temp == 't')
      scale.tare();  //Reset the scale to zero
    Serial.print(temp);
    Serial.println();
  }
}

