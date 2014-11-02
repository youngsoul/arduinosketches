// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library
#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            0

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      30

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 50; // delay for half a second

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.show(); // Initialize all pixels to 'off'

}

int index = 0;

void loop() {
  if( index == -1 ) return;
  
  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  for(int i=0;i<NUMPIXELS;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    switch(index) {
      case 0:
        if(i%2==0) {
        pixels.setPixelColor(0, pixels.Color(0,150,0)); // Moderately bright green color.
        } else {
          pixels.setPixelColor(0, pixels.Color(0,0,0)); // Moderately bright green color.
        }          
        pixels.show(); // This sends the updated pixel color to the hardware.
       break;

      case 1:
        pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
        pixels.show(); // This sends the updated pixel color to the hardware.
       break;
      case 2:
        pixels.setPixelColor(i, pixels.Color(150,0,0)); // Moderately bright red color.
        pixels.show(); // This sends the updated pixel color to the hardware.
       break; 
       
      case 3:
        pixels.setPixelColor(i, pixels.Color(0,0,150)); // Moderately bright red color.
        pixels.show(); // This sends the updated pixel color to the hardware.
       break; 
       
       default:
        pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright red color.
        pixels.show(); // This sends the updated pixel color to the hardware.
        index = -2;
       break;
    }
    delay(delayval); // Delay for a period of time (in milliseconds).
  }
  index++;
  
}
