#define PO_LED_GRN  30
#define PO_LED_RED  32
#define PO_BUZZER   31

void bleeps();

void setup() {
  pinMode(PO_LED_GRN, OUTPUT);
  pinMode(PO_LED_RED, OUTPUT);
  pinMode(PO_BUZZER, OUTPUT);

  Serial.begin(115200);
  Serial.println("Start");
}

void loop() {
  digitalWrite(PO_LED_RED, LOW);
  bleeps();
  digitalWrite(PO_LED_RED, HIGH);
  tone(PO_BUZZER, 1000);
  delay(500);
  tone(PO_BUZZER, 3000);
  delay(500);
  noTone(PO_BUZZER);
  delay(2000);
  Serial.println("next");
}

void bleeps() {
  for(int i = 0; i < 3; i++) {
    digitalWrite(PO_LED_GRN, HIGH);
    digitalWrite(PO_BUZZER, HIGH);
    delay(20);
    digitalWrite(PO_BUZZER, LOW);
    digitalWrite(PO_LED_GRN, LOW);
    delay(50);
  }
}