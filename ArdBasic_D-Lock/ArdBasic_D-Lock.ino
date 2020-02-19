#define PO_LED_ROT  32
#define PO_LED_GRN  30

/*

BWD-Steps
  - OPEN
  - Pre-Lock
  - Lock

*/

#define PILA_LOCK_SW          36
#define PILA_MOTOR_SW         37

#define POLA_MOTOR_FWD        35  // purple wire
#define POLA_MOTOR_BWD        34  // gray wire

#define LED_SET(led) {digitalWrite(led, HIGH);}    //
#define LED_RST(led) {digitalWrite(led, LOW);}      //

#define MOTOR_FWD() {\
                    digitalWrite(POLA_MOTOR_BWD, HIGH);\
                    digitalWrite(POLA_MOTOR_FWD, LOW);\
                    }

#define MOTOR_BWD() {\
                    digitalWrite(POLA_MOTOR_FWD, HIGH);\
                    digitalWrite(POLA_MOTOR_BWD, LOW);\
                    }
#define MOTOR_STP() {\
                    digitalWrite(POLA_MOTOR_FWD, HIGH);\
                    digitalWrite(POLA_MOTOR_BWD, HIGH);\
                    }

byte ubMotorSw, ubMotorSwOld;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Startup Serial Line 0");

  pinMode(PO_LED_ROT, OUTPUT);
  pinMode(PO_LED_GRN, OUTPUT);

  digitalWrite(POLA_MOTOR_FWD, HIGH);
  digitalWrite(POLA_MOTOR_BWD, HIGH);
  pinMode(POLA_MOTOR_FWD, OUTPUT);        // purple wire
  pinMode(POLA_MOTOR_BWD, OUTPUT);        // gray wire

 ubMotorSw = !digitalRead(PILA_MOTOR_SW);
 ubMotorSwOld = ubMotorSw;
 
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("running");

  if (ubMotorSw){
    Serial.print("MotorSw: On ");
  }
  else{
    Serial.print("MotorSw: Off");
  }
  MOTOR_BWD();
  do{
    ubMotorSwOld = ubMotorSw;
    ubMotorSw = !digitalRead(PILA_MOTOR_SW);
  }while(!(ubMotorSw && !ubMotorSwOld));
  MOTOR_FWD();
  delay(20);
  MOTOR_STP();
  if (ubMotorSw){
    Serial.println("MotorSw: On ");
  }
  else{
    Serial.println("MotorSw: Off");
  }
  delay (1000);
/*
  MOTOR_BWD();
  delay (1000);
  MOTOR_STP();
*/    
  LED_SET(PO_LED_ROT);
  LED_RST(PO_LED_GRN);
  delay(250);
  LED_RST(PO_LED_ROT);
  LED_SET(PO_LED_GRN);
  delay(5000);
}
