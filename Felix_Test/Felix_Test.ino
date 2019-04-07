#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //Angabe der erforderlichen Pins

const String cstrVER = String("0.01");       // Softwareversion

int intCounter = 0;

// erstellen einiger Variablen
int Taster = 0;
int Analogwert = 0;
#define Tasterrechts 0
#define Tasteroben 1
#define Tasterunten 2
#define Tasterlinks 3
#define Tasterselect 4
#define KeinTaster 5

 
// Ab hier wird ein neuer Programmblock mit dem Namen "Tasterstatus" erstellt. Hier wird ausschließlich geprüft, welcher Taster gedrückt ist.
int Tasterstatus(){
  Analogwert = analogRead(A0); // Auslesen der Taster am Analogen Pin A0.
  Serial.print("Tastenanalogwert: ");
  Serial.println(Analogwert);
  if (Analogwert > 1000) return KeinTaster;
  if (Analogwert < 50) return Tasterrechts;
  if (Analogwert < 195) return Tasteroben;
  if (Analogwert < 380) return Tasterunten;
  if (Analogwert < 555) return Tasterlinks;
  if (Analogwert < 790) return Tasterselect;
   
  return KeinTaster; // Ausgabe wenn kein Taster gedrückt wurde.
}
// Hier wird der Programmblock "Tasterstatus" abeschlossen.



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // put your setup code here, to run once:
  Serial.begin(115200);   // (9600);
  Serial.println("Felix sagt");
  Serial.println("Hello World!");
  Serial.println("SW-Ver:");
  Serial.println(cstrVER);

  lcd.begin(16, 2); // Starten der Programmbibliothek.
  lcd.setCursor(0,0); // Angabe des Cursorstartpunktes oben links.
  lcd.print(""); // Ausgabe des Textes "Nachricht".


}

int incomingByte = 0;   // for incoming serial data
String strSerIn;
char chrSerIn;

void loop() {
  if (Serial.available() > 0) {
          // read the incoming byte:
//          incomingByte = Serial.read();
          chrSerIn = Serial.read();
          
          // say what you got:
          Serial.print("I received: ");
//          Serial.println(incomingByte, DEC);
//          strSerIn = "Hallo";
          Serial.println(chrSerIn);
          delay(1000);
  }

  if (Analogwert < 195) {
     digitalWrite(LED_BUILTIN, HIGH);      // built in LED switch on
     lcd.setCursor(0,0); // Angabe des Cursorstartpunktes oben links.
     lcd.print("Licht ist an "); // Ausgabe des Textes "Nachricht".

     } else {
     digitalWrite(LED_BUILTIN, LOW);       // LED switch off 
     lcd.setCursor(0,0); // Angabe des Cursorstartpunktes oben links.
     lcd.print("Licht ist aus"); // Ausgabe des Textes "Nachricht".

   }
  
  // put your main code here, to run repeatedly:
/*Serial.println(intCounter++);
  digitalWrite(LED_BUILTIN, HIGH);      // built in LED switch on
  delay (500);     // [ms] Delay time
  digitalWrite(LED_BUILTIN, LOW);       // LED switch off
  delay (250);
*/

  lcd.setCursor(12,1); // Bewegt den Cursor in Zeile 2 (Line0=Zeile1 und Line1=Zeile2) and die Stelle "12".
  lcd.print(millis()/1000); // Zeigt die Sekunden seit Start des Programms in Sekunden an.
  lcd.setCursor(0,1); // Bewegt den Cursor and die Stelle "0" in Zeile 2.

  Taster = Tasterstatus(); //Hier springt der Loop in den oben angegebenen Programmabschnitt "Tasterstatus" und liest dort den gedrückten Taster aus.
  Serial.print("Tasterstatus: ");
  Serial.println(Taster);
  
  switch (Taster) // Abhängig von der gedrückten Taste wird in dem folgenden switch-case Befehl entschieden, was auf dem LCD angezeigt wird.
  {
    case Tasterrechts: // Wenn die rechte Taste gedrückt wurde...
      {
      lcd.print("Rechts      "); //Erscheint diese Zeile. Die Leerzeichen hinter dem Text sorgen dafür, dass der vorherige Text in der Zeile gelöscht wird.
      break;
      }
    case Tasterlinks:  // Wenn die linke Taste gedrückt wurde...
      {
      lcd.print("Links       ");  //Erscheint diese Zeile... usw...
      break;
      }
    case Tasteroben:
      {
      lcd.print("Oben        ");
      break;
      }
    case Tasterunten:
      {
      lcd.print("Unten       ");
      break;
      }
    case Tasterselect:
      {
      lcd.print("SELECT      ");
      break;
      }
    case KeinTaster:
      {
      lcd.print("Keine Taste ");
      break;
      }
  } //switch-case Befehl beenden
 
}
