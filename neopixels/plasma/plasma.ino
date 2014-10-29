/*
 * LedBrightness sketch
 * controls the brightness of LEDs on "analog" (PWM) output ports.
 */
#include <Adafruit_NeoPixel.h>

#define PIN 2
#define NUMPIXELS 30

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}


void PlasmaPulse(int wait) {
  uint16_t i, j;
  uint8_t brightness = 255;

  for(i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();
  delay(wait);
  //Adjust 60 and 90 to the starting and ending colors you want to fade between. 
  for(j=170; j>=135; --j) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    brightness -= 6;
    pixels.setBrightness(brightness);
    delay(wait);
  }

// j<1170
  for(j=135; j<1170; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    brightness += 6;
    pixels.setBrightness(brightness);
    delay(wait);
  }
  
  for(i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();
  delay(wait);

}

void setup()
{
  //pins driven by analogWrite do not need to be declared as outputs
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'

}

void loop()
{
  
  PlasmaPulse(750);
}
