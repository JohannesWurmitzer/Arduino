#define IN_NOT_MOTOR_SWITCH         37

void setup() {
  // put your setup code here, to run once:
  pinMode(IN_NOT_MOTOR_SWITCH, INPUT_PULLUP);
  
  Serial.begin(115200);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(digitalRead(IN_NOT_MOTOR_SWITCH));
  delay(500);
}
