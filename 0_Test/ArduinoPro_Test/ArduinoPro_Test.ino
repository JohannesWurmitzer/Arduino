void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, true);
  delay(250);
  digitalWrite(LED_BUILTIN, false);
  delay(500);
  Serial.print("Tick: "); Serial.println(millis());
}
