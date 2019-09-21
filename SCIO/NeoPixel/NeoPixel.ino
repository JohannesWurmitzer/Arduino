// https://www.arduino.cc/reference/en/


#include <Adafruit_NeoPixel.h>

// https://funduino.de/nr-17-ws2812-neopixel

#define PIN  9   // Hier wird angegeben, an welchem digitalen Pin die WS2812 LEDs bzw. NeoPixel angeschlossen sind
#define RGBMATRIX_DOUT 8  //
#define NUMPIXELS 12 // Hier wird die Anzahl der angeschlossenen WS2812 LEDs bzw. NeoPixel angegeben
#define RGBMATRIX_NUMPIXELS 256 // Hier wird die Anzahl der angeschlossenen WS2812 LEDs bzw. NeoPixel angegeben

#define RGB_DIV 2048

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel NeoPixel = Adafruit_NeoPixel(RGBMATRIX_NUMPIXELS, RGBMATRIX_DOUT, NEO_GRB + NEO_KHZ800);

int pause=100; // 100 Millisekunden Pause bis zur Ansteuerung der nächsten LED.
byte bySerialDummy;
byte bySerialR;
byte bySerialG;
byte bySerialB;

void setup() {
  // put your setup code here, to run once:
  pixels.begin(); // Initialisierung der NeoPixel
  NeoPixel.begin(); // Initialisierung der NeoPixel
  Serial.begin(115200); // Initialisierung der seriellen Schnittstelle
  Serial1.begin(115200); // Initialisierung der seriellen Schnittstelle
}

struct BITMAPFILEHEADER {
    byte    bfType;        // Typ bei *.bmp in Hex = 424D = BM
    word    bfSize;        // Dateigröße in Byte
    word    bfReserved1;
    word    bfReserved2;
    unsigned long   bfOffBits;     // Offset zum ersten Datenbyte des Bildes
};

struct BITMAPINFOHEADER {
    unsigned long   biSize;      // Länge des Info-Headers(dieser Record) = 40 Byte in Hex 28
    long    biWidth;      // Breite des Bilds 
    long    biHeight;      // Höhe des Bilds 
    word    biPlanes;       // Anzahl der Farbebenen in der Regel 1 u. 0
    word    biBitCount;      // Anzahl der Bits pro Pixel( Anzahl der Farben) 
           // 1= Schwarz-Weis, 4 = 16 Farben = 64 Byte, 
           // 8 = 256 Farben = 1024 Byte, 24 = 16 Milionen Farben   
    unsigned long   biCompression;    // Komprimierungstyp, 0 = Unkomprimiert, 1 = 8-Bit 
            // Run-Length-Encording-Verfahren, 
                              // 2 = 4-Bit Run-Encording-Verfahren
    unsigned long   biSizeImage;       // Bildgröße in Bytes.
    long    biXPelsPerMeter;   // Horizontale Auflösung
    long    biYPelsPerMeter;   // Vertikale Auflösung
    unsigned long   biClrUsed;         // Die Zahl der im Bild vorkommenden Farben
    unsigned long   biClrImportant;    // die Anzahl der wichtigen Farben
} ;

#define BMP_HEADER_SIZE 54
byte color;
long idx;
int iBmpIdxCnt;
int iTimeOut;   // [ms]
byte byMsCnt;
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
  if (byMsCnt == 0){
    for (idx = 0; idx < 256; idx++){
      NeoPixel.setPixelColor(idx, NeoPixel.Color((idx*idx+RGB_DIV-1)/RGB_DIV,(idx*idx+RGB_DIV-1)/RGB_DIV,(idx*idx+RGB_DIV-1)/RGB_DIV)); // Pixel1 leuchtet in der Farbe Grün
    }
    NeoPixel.show(); // Durchführen der Pixel-Ansteuerung
  }
/*
  Serial.write("Hello World 0\n");
  Serial1.write("Hello World 1\n");
  Serial.write("FreeRam: ");
  Serial.println(freeRam());
  Serial.write("HeapStart: ");
  iHeapStart = &__heap_start;
  Serial.println(iHeapStart);
*/ 
  if (byMsCnt >= 50){
    for (idx = 0; idx < 12; idx++){
      pixels.setPixelColor(idx, pixels.Color((color+idx) % 25, (color+idx+8) % 25, (color+idx+16) % 25)); // Pixel1 leuchtet in der Farbe Grün
    }
    color++;
    if (color >= 250) color = 0;
    if (iBmpIdxCnt == 0){
      for (idx = 0; idx < 256; idx++){
        NeoPixel.setPixelColor(idx, NeoPixel.Color((color+idx) % 25, (color+idx+8) % 25, (color+idx+16) % 25)); // Pixel1 leuchtet in der Farbe Grün
      }
    }
  }
  delay(1);
  byMsCnt++;
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
NeoPixel.show(); // Durchführen der Pixel-Ansteuerung
/*
delay (pause); // Pause, die LEDs bleiben in dieser Zeit aus
*/
  while(Serial.available() == 0);
  iBmpIdxCnt = 0;
  while(Serial.available()>0){
    if (iBmpIdxCnt >= BMP_HEADER_SIZE){
      if ((iBmpIdxCnt-BMP_HEADER_SIZE) % 3 == 0){ bySerialB = Serial.read();}
      else if ((iBmpIdxCnt-BMP_HEADER_SIZE) % 3 == 1){ bySerialG = Serial.read();}
      else if ((iBmpIdxCnt-BMP_HEADER_SIZE) % 3 == 2 /*&& iBmpIdxCnt-BMP_HEADER_SIZE < 32*3*/) {
        bySerialR = Serial.read();
        if (((iBmpIdxCnt-BMP_HEADER_SIZE)/3/16) & 1){
          // ungerade Zeile
          NeoPixel.setPixelColor((iBmpIdxCnt-BMP_HEADER_SIZE)/3 + 15 - (((iBmpIdxCnt-BMP_HEADER_SIZE)/3) % 16)- (((iBmpIdxCnt-BMP_HEADER_SIZE)/3) % 16), NeoPixel.Color(((long)bySerialR*bySerialR+RGB_DIV-1)/RGB_DIV, ((long)bySerialG*bySerialG+RGB_DIV-1)/RGB_DIV, ((long)bySerialB*bySerialB+RGB_DIV-1)/RGB_DIV)); // Pixel1 leuchtet in der Farbe Grün
        }
        else{
          // gerade Zeile
          NeoPixel.setPixelColor((iBmpIdxCnt-BMP_HEADER_SIZE)/3, NeoPixel.Color(((long)bySerialR*bySerialR+RGB_DIV-1)/RGB_DIV, ((long)bySerialG*bySerialG+RGB_DIV-1)/RGB_DIV, ((long)bySerialB*bySerialB+RGB_DIV-1)/RGB_DIV)); // Pixel1 leuchtet in der Farbe Grün
        }
//        Serial.println(iBmpIdxCnt-BMP_HEADER_SIZE);
      }
    }
    else{
      bySerialDummy = Serial.read();
    }
    iBmpIdxCnt++;
    if (Serial.available()<5){
      delay(3);
    }
  }
  Serial.println(iBmpIdxCnt);
/*  if (iTimeOut > 10000){
    iBmpIdxCnt = 0;
    iTimeOut = 0;
  }
  else{
    if (iBmpIdxCnt > 0){
      iTimeOut += 50;
    }
    else{
      iTimeOut = 0;
    }
  }
*/
}
