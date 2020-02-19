/*
  Important note: ATmega328P (Old Bootloader)
  https://store.arduino.cc/arduino-nano

*/
#define DO_PWM1 3
#define DO_PWM2 5
#define DO_PWM3 6

#define DI_SW_ON_1  2
#define DI_SW_ON_2  4
#define DI_SW_ON_3  7

#define DI_SW_LO_1  8
#define DI_SW_LO_2  8 //9
#define DI_SW_LO_3  8 //10

#define DI_SW_HI_1  11
#define DI_SW_HI_2  11  //12
#define DI_SW_HI_3  11  //13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
//  pinMode(LED_BUILTIN, INPUT);

  analogWrite(DO_PWM1, 0);
  analogWrite(DO_PWM2, 0);
  analogWrite(DO_PWM3, 0);

  pinMode(DO_PWM1, OUTPUT);
  pinMode(DO_PWM2, OUTPUT);
  pinMode(DO_PWM3, OUTPUT);
  
  pinMode(DI_SW_ON_1, INPUT);
  pinMode(DI_SW_ON_2, INPUT);
  pinMode(DI_SW_ON_3, INPUT);

  Serial.println("Setup Martin Fritz InfraChair Demo Build 2020-02-16");
  Serial.println("(c) Maximilian Johannes Wurmitzer, Welando GmbH 2020");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
    
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  Serial.print("Status Area 1: "); Serial.print(digitalRead(DI_SW_ON_1)); Serial.print(" "); Serial.print(digitalRead(DI_SW_LO_1)); Serial.print(" ");  Serial.print(digitalRead(DI_SW_HI_1)); Serial.print(" "); Serial.println();
  Serial.print("Status Area 2: "); Serial.print(digitalRead(DI_SW_ON_2)); Serial.print(" "); Serial.print(digitalRead(DI_SW_LO_2)); Serial.print(" ");  Serial.print(digitalRead(DI_SW_HI_2)); Serial.print(" "); Serial.println();
  Serial.print("Status Area 3: "); Serial.print(digitalRead(DI_SW_ON_3)); Serial.print(" "); Serial.print(digitalRead(DI_SW_LO_3)); Serial.print(" ");  Serial.print(digitalRead(DI_SW_HI_3)); Serial.print(" "); Serial.println();
  Serial.println();

  if (digitalRead(DI_SW_ON_1)){
    // Area ON
    if (!digitalRead(DI_SW_LO_1) && digitalRead(DI_SW_HI_1)){
      // LOW: LO ON and HI OFF
      analogWrite(DO_PWM1, 84);
    }
    else if (digitalRead(DI_SW_LO_1) && digitalRead(DI_SW_HI_1)){
      // MID: LO OFF and HI OFF
      analogWrite(DO_PWM1, 168);
    }
    else if (digitalRead(DI_SW_LO_1) && !digitalRead(DI_SW_HI_1)){
      // HIG: LO OFF and HI ON
      analogWrite(DO_PWM1, 255);
    }
    else{
      analogWrite(DO_PWM1, 0);
    }
  }
  else{
    // Area OFF
    analogWrite(DO_PWM1, 0);
  }

  if (digitalRead(DI_SW_ON_2)){
    // Area ON
    if (!digitalRead(DI_SW_LO_2) && digitalRead(DI_SW_HI_2)){
      // LOW: LO ON and HI OFF
      analogWrite(DO_PWM2, 84);
    }
    else if (digitalRead(DI_SW_LO_2) && digitalRead(DI_SW_HI_2)){
      // MID: LO OFF and HI OFF
      analogWrite(DO_PWM2, 168);
    }
    else if (digitalRead(DI_SW_LO_2) && !digitalRead(DI_SW_HI_2)){
      // HIG: LO OFF and HI ON
      analogWrite(DO_PWM2, 255);
    }
    else{
      analogWrite(DO_PWM2, 0);
    }
  }
  else{
    // Area OFF
    analogWrite(DO_PWM2, 0);
  }

  if (digitalRead(DI_SW_ON_3)){
  // Area ON
  if (!digitalRead(DI_SW_LO_3) && digitalRead(DI_SW_HI_3)){
    // LOW: LO ON and HI OFF
    analogWrite(DO_PWM3, 84);
  }
  else if (digitalRead(DI_SW_LO_3) && digitalRead(DI_SW_HI_3)){
    // MID: LO OFF and HI OFF
    analogWrite(DO_PWM3, 168);
  }
  else if (digitalRead(DI_SW_LO_3) && !digitalRead(DI_SW_HI_3)){
    // HIG: LO OFF and HI ON
    analogWrite(DO_PWM3, 255);
  }
  else{
    analogWrite(DO_PWM3, 0);
  }
}
else{
  // Area OFF
  analogWrite(DO_PWM3, 0);
}

}
