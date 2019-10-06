#define AI_POTI A0

#define DO_LED_R  5
#define DO_LED_G  6
#define DO_LED_B  7
#define AI_MAX  1023
#define PWM_MAX 255

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(DO_LED_R, OUTPUT);
  pinMode(DO_LED_G, OUTPUT);
  pinMode(DO_LED_B, OUTPUT);
}

void loop() {
  unsigned short lu16PotiValue;
  char lachStr[64];
  unsigned char u8red = 10;
  unsigned char u8blu = 255;
  unsigned char u8gre = 20;
  
  // put your main code here, to run repeatedly:
  Serial.print("Hallo");
  Serial.println();

  lu16PotiValue = analogRead(AI_POTI);
  Serial.print("Poti: ");
  Serial.print(lu16PotiValue);
  Serial.print(" 10-dig");
  sprintf(lachStr, "%4d,%03d V",(unsigned short)5*lu16PotiValue/1023, (unsigned long)5000*lu16PotiValue/1023 % 1000);
  Serial.print(" ");
  Serial.println(lachStr);
  Serial.println((unsigned long) lu16PotiValue * lu16PotiValue / AI_MAX);
  analogWrite(DO_LED_R, (unsigned long) u8red * lu16PotiValue * lu16PotiValue / AI_MAX / 4 / PWM_MAX);
  analogWrite(DO_LED_G, (unsigned long) u8gre * lu16PotiValue * lu16PotiValue / AI_MAX / 4 / PWM_MAX);
  analogWrite(DO_LED_B, (unsigned long) u8blu * lu16PotiValue * lu16PotiValue / AI_MAX / 4 / PWM_MAX);
  delay(50);
  
}
