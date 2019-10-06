
#define DO_SPEAKER 10  // Pin for Speaker-Output

void setup() {
  // put your setup code here, to run once:
  pinMode(DO_SPEAKER, OUTPUT);
}

unsigned char u8Volume;
void loop() {
  int i;
  int o;
  
  // put your main code here, to run repeatedly:
  for (o = 0; o < 100; o++) {
    for (i = 0; i < o * 5; i++){
      digitalWrite(DO_SPEAKER, HIGH);
      delayMicroseconds(i);
      digitalWrite(DO_SPEAKER, LOW);
      delayMicroseconds(i);
      

    }
  }
   
   for (o = 100; o > 0; o--) {
    for (i = 99; i > o * 5; i--){
      digitalWrite(DO_SPEAKER, HIGH);
      delayMicroseconds(i);
      digitalWrite(DO_SPEAKER, LOW);
      delayMicroseconds(i);
      

    }
  }
}

    //  analogWrite(DO_SPEAKER, u8Volume++);
  //    if (u8Volume > 50){
  //      u8Volume = 0;
  //    }
    //  delay(100);
    //  digitalWrite(DO_SPEAKER, LOW);
  
