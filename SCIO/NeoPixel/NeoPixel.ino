// https://www.arduino.cc/reference/en/


#include <Adafruit_NeoPixel.h>

// https://funduino.de/nr-17-ws2812-neopixel

#define PIN 9 // Hier wird angegeben, an welchem digitalen Pin die WS2812 LEDs bzw. NeoPixel angeschlossen sind
#define NUMPIXELS 256 // Hier wird die Anzahl der angeschlossenen WS2812 LEDs bzw. NeoPixel angegeben
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int pause=100; // 100 Millisekunden Pause bis zur Ansteuerung der nächsten LED.

void setup() {
  // put your setup code here, to run once:
  pixels.begin(); // Initialisierung der NeoPixel
  Serial.begin(115200); // Initialisierung der seriellen Schnittstelle
  Serial1.begin(115200); // Initialisierung der seriellen Schnittstelle
}

byte color;
byte idx;
//byte bild[] = {0,0,0, 0xff, 0x34, ,}

extern int __heap_start, *__brkval;
int iHeapStart;

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.write("Hello World 0\n");
  Serial1.write("Hello World 1\n");
  Serial.write("FreeRam: ");
  Serial.println(freeRam());
  Serial.write("HeapStart: ");
  iHeapStart = &__heap_start;
  Serial.println(iHeapStart);
  
  for (idx = 0; idx < 12; idx++){
    pixels.setPixelColor(idx, pixels.Color((color+idx) % 25, (color+idx+8) % 25, (color+idx+16) % 25)); // Pixel1 leuchtet in der Farbe Grün
  }
  color++;
  if (color >= 250) color = 0;
  delay(50);
/*
  pixels.setPixelColor(0, pixels.Color(25, 0,0)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(1, pixels.Color(25, 0,0)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(2, pixels.Color(25, 0,0)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(3, pixels.Color(25, 0,0)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(4, pixels.Color(0, 25,0)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(5, pixels.Color(0, 25,0)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(6, pixels.Color(0, 25,0)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(7, pixels.Color(0, 25,0)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(8, pixels.Color(0, 0,25)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(9, pixels.Color(0, 0,25)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(10, pixels.Color(0, 0,25)); // Pixel1 leuchtet in der Farbe Grün
  pixels.setPixelColor(11, pixels.Color(0, 0,25)); // Pixel1 leuchtet in der Farbe Grün
*/  
/*  
pixels.setPixelColor(1, pixels.Color(0,255,0)); // Pixel1 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.
pixels.setPixelColor(2, pixels.Color(0,150,0)); // Pixel2 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.
pixels.setPixelColor(3, pixels.Color(0,50,0)); // Pixel3 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.
pixels.setPixelColor(4, pixels.Color(0,10,0)); // Pixel4 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.
pixels.setPixelColor(5, pixels.Color(0,1,0)); // Pixel5 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.

pixels.setPixelColor(6, pixels.Color(255,0,0)); // Pixel1 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.
pixels.setPixelColor(7, pixels.Color(150,0,0)); // Pixel2 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.
pixels.setPixelColor(8, pixels.Color(50,0,0)); // Pixel3 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.
pixels.setPixelColor(9, pixels.Color(10,0,0)); // Pixel4 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.
pixels.setPixelColor(10, pixels.Color(1,0,0)); // Pixel5 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.

pixels.setPixelColor(11, pixels.Color(0,0,255)); // Pixel1 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.
pixels.setPixelColor(0, pixels.Color(0,0,150)); // Pixel2 leuchtet in der Farbe Grün
pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.


// Zurücksetzen aller Pixelfarben auf Stufe "0" (aus)
pixels.setPixelColor(0, pixels.Color(0,0,0)); 
pixels.setPixelColor(1, pixels.Color(0,0,0)); 
pixels.setPixelColor(2, pixels.Color(0,0,0)); 
pixels.setPixelColor(3, pixels.Color(0,0,0)); 
pixels.setPixelColor(4, pixels.Color(0,0,0)); 
pixels.setPixelColor(5, pixels.Color(0,0,0)); 
pixels.setPixelColor(6, pixels.Color(0,0,0)); 
pixels.setPixelColor(7, pixels.Color(0,0,0)); 
pixels.setPixelColor(8, pixels.Color(0,0,0)); 
pixels.setPixelColor(9, pixels.Color(0,0,0)); 
pixels.setPixelColor(10, pixels.Color(0,0,0)); 
pixels.setPixelColor(11, pixels.Color(0,0,0)); */
pixels.show(); // Durchführen der Pixel-Ansteuerung
/*
delay (pause); // Pause, die LEDs bleiben in dieser Zeit aus
*/
}
