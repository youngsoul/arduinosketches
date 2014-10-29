/*
 * LedBrightness sketch
 * controls the brightness of LEDs on "analog" (PWM) output ports.
 */
#include <avr/power.h>

#include <Adafruit_NeoPixel.h>

#define DEBOUNCE_TIME  500
long button_bounce_time = 0L;

// 0-5 volts for a 10bit converter the value is 0-1023
int analogVal = 0;

// Processing State:
// 0 - plasma state
// 1 - mic trigger
// 2 - off
int state = 0;

// Mode Input
#define MODE_INPUT 1

// NeoPixel Data line
#define PIN 2

// Mic Input
#define MIC_INPUT 3

#define NUMPIXELS 30

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


void pixelStripOff() {
  for(int i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}

int lowColor = 134;
int highColor = 180;
int currentColorIndex = 0;

void showHit() {
  currentColorIndex = 1;
  int newColor = lowColor+currentColorIndex; 
  if( newColor >= highColor ) currentColorIndex = 0;
  
  for(int i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((newColor) & 255));
  }
  pixels.show();
  
  delay(175);
  pixelStripOff();
 
}

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
  for(j=135; j<170; j++) {
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
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  //pins driven by analogWrite do not need to be declared as outputs
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'

  state = 0; // plasma state
  currentColorIndex = 0;
  button_bounce_time = 0L;
  
  // initialize the MODE_INPUT pin as an INPUT
  pinMode(MODE_INPUT, INPUT);
  
  // ...with a pullup
  digitalWrite(MODE_INPUT, HIGH);
  
}


void loop()
{
  long now = millis();
  
  if (! digitalRead(MODE_INPUT) && now > button_bounce_time) {  // if the button is pressed
    button_bounce_time = ( now + DEBOUNCE_TIME );
    
    state += 1;
    if( state >= 3 ) state = 0;
    pixelStripOff();
    if( state == 1 ) {
        pixels.setBrightness(255);
    }

  } 
  
  if( state == 0 ) {
    PlasmaPulse(750);
  } else if( state == 1 ) {
    analogVal = analogRead( MIC_INPUT );
    if( analogVal > 400 ) {
      showHit();
    }
    
  }
}
