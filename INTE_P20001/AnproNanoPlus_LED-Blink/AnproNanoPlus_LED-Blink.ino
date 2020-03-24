#define PDO_BUZZ  2

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  digitalWrite(PDO_BUZZ, LOW);
  pinMode(PDO_BUZZ, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(64);
  digitalWrite(LED_BUILTIN, LOW);
  delay(936);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(125);
  digitalWrite(LED_BUILTIN, LOW);
  delay(875);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(PDO_BUZZ, HIGH);
  delay(250);
  digitalWrite(PDO_BUZZ, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  delay(750);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
