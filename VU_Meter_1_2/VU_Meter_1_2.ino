///////////////////////////////////////////////////////////
////  VU Meter Display for R2-D2 Data Port Panel v1.2  ////
////      Written By Ryan Sondgeroth (flthymcnsty)     ////
///////////////////////////////////////////////////////////

#include <Adafruit_NeoPixel.h>

#define LEFTPIN 9
#define RIGHTPIN 8
#define BRIGHT 40 // Brightness 0 (Dim) to 255 (bright)
#define SBSPEED 5 // Standby Speed in ms.  
#define STANDBYSENSITIVITY 500  //Timer for low signal before Standby mode is triggered. 
#define BASELINE 200 //  Baseline Signal - Adjust this higher if white noise on audio input is causing phantom VU triggers    

#define LEFT 0
#define RIGHT 1

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
Adafruit_NeoPixel stripLeft = Adafruit_NeoPixel(8, LEFTPIN);
Adafruit_NeoPixel stripRight = Adafruit_NeoPixel(8, RIGHTPIN);

// Assign Colors using RGB values for each LED in VU Meter Mode
int LED1[3] = {0, 0, 255};         // Blue
int LED2[3] = {0, 0, 255};         // Blue
int LED3[3] = {0, 255, 0};         // Green
int LED4[3] = {0, 255, 0};         // Green
int LED5[3] = {255, 255, 0};       // Yellow
int LED6[3] = {255, 255, 0};       // Yellow
int LED7[3] = {255, 0, 0};         // Red
int LED8[3] = {255, 0, 0};         // Red

// Assign Colors using RGB values for Standby Mode
int LEDPULSE[3] = {255, 255, 255}; // Color of Pulsing Pixel in Stanby Mode = White
int LEDBG[3] = {0, 0, 255};        // Color of Background in Stanby Mode = Blue

// Spectrum analyzer read values will be kept here.
int SpectrumLeft[7];
int SpectrumRight[7];

// Containers to Values 
int SumLeft = 0;
int SumRight = 0;
int leftAverage = 0;
int rightAverage = 0;
int standbyTimer = 0;

// Various Counters
int i = 0; 
int j = 8;
int k = 0; 
int n = 0;
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  stripLeft.begin();
  stripRight.begin();
  stripLeft.show(); // Initialize all pixels to 'off'
  stripRight.show(); // Initialize all pixels to 'off'
  
  //Setup pins to drive the spectrum analyzer. It needs RESET and STROBE pins.
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);

  //Init spectrum analyzer
  digitalWrite(4,LOW);  //pin 4 is strobe on shield
  digitalWrite(5,HIGH); //pin 5 is RESET on the shield
  digitalWrite(4,HIGH);
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
}

void loop() {
  readSpectrum();
  SumLeft = 0;
  SumRight = 0;
  leftAverage = 0;
  rightAverage = 0;
  for(i=0; i<7; i++) {
     SumLeft += SpectrumLeft[i];
     SumRight += SpectrumRight[i];
  }
  leftAverage = SumLeft/7;
  rightAverage = SumRight/7;
  if (leftAverage < BASELINE || rightAverage < BASELINE) {
    standbyTimer++;
    if(standbyTimer > STANDBYSENSITIVITY) {
       standby();
    }
    else {
       for(i=0;i<8;i++) {
        stripLeft.setPixelColor(i, 0, 0, 0);
        stripRight.setPixelColor(i, 0, 0, 0);
       }
    }

  }
  else {
    standbyTimer = 0;
    // Left VU Display 
    if (leftAverage > BASELINE) { stripLeft.setPixelColor(0, LED1[0], LED1[1], LED1[2]); }  else { stripLeft.setPixelColor(0, 0, 0, 0); }
    if (leftAverage > (BASELINE+100)) { stripLeft.setPixelColor(1, LED2[0], LED2[1], LED2[2]); } else { stripLeft.setPixelColor(1, 0, 0, 0); }
    if (leftAverage > (BASELINE+200)) { stripLeft.setPixelColor(2, LED3[0], LED3[1], LED3[2]); } else { stripLeft.setPixelColor(2, 0, 0, 0); }
    if (leftAverage > (BASELINE+300)) { stripLeft.setPixelColor(3, LED4[0], LED4[1], LED4[2]); } else { stripLeft.setPixelColor(3, 0, 0, 0); }
    if (leftAverage > (BASELINE+400)) { stripLeft.setPixelColor(4, LED5[0], LED5[1], LED5[2]); } else { stripLeft.setPixelColor(4, 0, 0, 0); }
    if (leftAverage > (BASELINE+500)) { stripLeft.setPixelColor(5, LED6[0], LED6[1], LED6[2]); } else { stripLeft.setPixelColor(5, 0, 0, 0); }
    if (leftAverage > (BASELINE+600)) { stripLeft.setPixelColor(6, LED7[0], LED7[1], LED7[2]); } else { stripLeft.setPixelColor(6, 0, 0, 0); }
    if (leftAverage > (BASELINE+700)) { stripLeft.setPixelColor(7, LED8[0], LED8[1], LED8[2]); } else { stripLeft.setPixelColor(7, 0, 0, 0); }
    // Right VU Display 
    if (rightAverage > BASELINE) { stripRight.setPixelColor(0, LED1[0], LED1[1], LED1[2]); }  else { stripRight.setPixelColor(0, 0, 0, 0); }
    if (rightAverage > (BASELINE+100)) { stripRight.setPixelColor(1, LED2[0], LED2[1], LED2[2]); } else { stripRight.setPixelColor(1, 0, 0, 0); }
    if (rightAverage > (BASELINE+200)) { stripRight.setPixelColor(2, LED3[0], LED3[1], LED3[2]); } else { stripRight.setPixelColor(2, 0, 0, 0); }
    if (rightAverage > (BASELINE+300)) { stripRight.setPixelColor(3, LED4[0], LED4[1], LED4[2]); } else { stripRight.setPixelColor(3, 0, 0, 0); }
    if (rightAverage > (BASELINE+400)) { stripRight.setPixelColor(4, LED5[0], LED5[1], LED5[2]); } else { stripRight.setPixelColor(4, 0, 0, 0); }
    if (rightAverage > (BASELINE+500)) { stripRight.setPixelColor(5, LED6[0], LED6[1], LED6[2]); } else { stripRight.setPixelColor(5, 0, 0, 0); }
    if (rightAverage > (BASELINE+600)) { stripRight.setPixelColor(6, LED7[0], LED7[1], LED7[2]); } else { stripRight.setPixelColor(6, 0, 0, 0); }
    if (rightAverage > (BASELINE+700)) { stripRight.setPixelColor(7, LED8[0], LED8[1], LED8[2]); } else { stripRight.setPixelColor(7, 0, 0, 0); }
  }
  
  stripLeft.setBrightness(BRIGHT);
  stripRight.setBrightness(BRIGHT);
  stripLeft.show();
  stripRight.show(); 
}

void readSpectrum()
{
  // Band 0 = Lowest Frequencies.
  byte Band;
  for(Band=0;Band <7; Band++)
  {
    SpectrumLeft[Band] = analogRead(0); //left
    SpectrumRight[Band] = analogRead(1); //right
    digitalWrite(4,HIGH);  //Strobe pin on the shield
    digitalWrite(4,LOW);     
  }
}

void standby() {
  k=0;
  for(k=0;k<8;k++) {
    standbyFrame(k);
  }
  j=7;
  for(j=7;j >= 1; --j) {
    standbyFrame(j);
  }
}

void standbyFrame(int n) {
  i = 0;
  for(i=0;i<8;i++) {
    if(n==i) {
       stripLeft.setPixelColor(i, LEDPULSE[0], LEDPULSE[1], LEDPULSE[2]);
       stripRight.setPixelColor(i, LEDPULSE[0], LEDPULSE[1], LEDPULSE[2]);
     }
     else {
       stripLeft.setPixelColor(i,  LEDBG[0], LEDBG[1], LEDBG[2]);
       stripRight.setPixelColor(i, LEDBG[0], LEDBG[1], LEDBG[2]);
     }

    stripLeft.setBrightness(BRIGHT);
    stripRight.setBrightness(BRIGHT);
    stripRight.show();
    stripLeft.show();  
    delay(SBSPEED); 
   }   
}
