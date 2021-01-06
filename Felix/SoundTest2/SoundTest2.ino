#include "TimerOne.h"

#define PO_BUZZER 31
 
void setup(){
  pinMode(PO_BUZZER, OUTPUT);  

  digitalWrite(PO_BUZZER, HIGH);
  delay(1000);
  digitalWrite(PO_BUZZER, LOW);
  delay(1000);
  tone(PO_BUZZER, 440);
  delay(1000);
  noTone(PO_BUZZER);
  
  playAllSounds();
  
  // https://www.pjrc.com/teensy/td_libs_TimerOne.html
  Timer1.initialize(1000);//1000µs (1kHz); 16bit timer! parameter is long variable in micro seconds
        //Timer1.setPeriod(3000);//3000µs (333.3Hz)
  Timer1.attachInterrupt(Tmr1_ISR);
  Timer1.stop();

}
 
void loop(){
  
}

void playAllSounds() {
  playSound_ok();
  delay(750);
  playSound_error();
  delay(750);
  playSound_detectTag();
  delay(750);
  playSound_unknownID();
  delay(750);
  playSound_teachUser();
  delay(750);
  playSound_teachUserEnd();
  delay(750);
  playSound_teachArticle();
  delay(750);
  playSound_teachArticleEnd();
  delay(750);
  playSound_eraseIDs();
  delay(750);
  playSound_eraseIDsEnd();
}

void playSound_ok() {
    playNote(400, 20);
    playNote(800, 50);
    delay(50);
    playNote(1200, 50);
}

void playSound_error() {
    playNote(100, 500);
}

void playSound_detectTag() {
    playNote(200, 30);
    playNote(400, 30);
    playNote(600, 30);
    playNote(800, 30);
    delay(30);
    playNote(1000, 30);
    delay(30);
    playNote(1000, 30);
}

void playSound_unknownID() {
    for(int i = 0; i < 3; i++){
      playNote(120, 30);
      delay(30);
    }  
}

void playSound_teachUser() {
    playNote(500, 20);
    delay(20);
    playNote(1000, 20);
    delay(20);
    playNote(1000, 50);
}

void playSound_teachUserEnd() {
    playNote(500, 20);
    delay(20);
    playNote(1000, 50);
    playNote(1200, 100);
}

void playSound_teachArticle() {
    playNote(1000, 20);
    delay(30);
    playNote(1000, 60);
}

void playSound_teachArticleEnd() {
    playNote(1000, 100);
    delay(50);
    playNote(1200, 100);
}

void playSound_eraseIDs() {
    playNote(200, 100);
    delay(50);
    playNote(200, 100);
}

void playSound_eraseIDsEnd() {
    playNote(100, 20);
    playNote(200, 50);
    delay(50);
    playNote(400, 50);
}

void playNote(int frequency_Hz, int duration_ms) {
  for(int i = 0; i < duration_ms * 0.001 * frequency_Hz * 0.5; i++) {
    delayMicroseconds(1000000 / frequency_Hz);
    digitalWrite(PO_BUZZER, HIGH);
    delayMicroseconds(1000000 / frequency_Hz);
    digitalWrite(PO_BUZZER, LOW);
  }
}
 
