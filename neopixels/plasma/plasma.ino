/*
 * Much of this code has been borrowed or inspired by the Adafruit NeoPixel library
 * Adafruit provides so much for DIY electronics.  Please consider buying from them.
 *  www.adafruit.com
 * Plasma/Color Switcher/Theater sketch
 * https://github.com/youngsoul/arduinosketches/tree/master/neopixels/plasma
 */
#include <Adafruit_NeoPixel.h>

// 0-5 volts for a 10bit converter the value is 0-1023
int analogVal = 0;
int micDCOffset = 1024/2;  // dc offset is vcc/2 so the 'zero' point is at 2.5 volts
// HIT_THRESHOLD is the amount above the micDCOffset that will trigger a hit
// Maximum value is 512
#define HIT_THRESHOLD  400

// HIT_TIME is the number of milliseconds before looking for another hit
#define HIT_TIME  325

// Processing State:
// 0 - plasma/fader state
// 1 - on solid for random duration, no fade, then new color
// 2 - mic trigger
// 3 - theater chase
// 4 - off
int state = 0;

long now = 0;
long hit_off_time = 0;

boolean button_was_pressed = false;

#define LOW_COLOR  0
#define HIGH_COLOR  255

int currentColorIndex = 0;
#define PLASMA_TIME 250
#define PIXEL_OFF_DELAY 50
#define PIXEL_STATE_DELAY 1000

// Mode Input
#define MODE_INPUT_PIN 1

// NeoPixel Data line
#define DATA_LINE_PIN 0

// Mic Input
// https://learn.adafruit.com/introducing-trinket/programming-with-arduino-ide
// You can read an analog voltage from digital #2 (called Analog 1), 
// digital #3 (called Analog 3) and digital #4 (called Analog 2)

// For example, to read an analog voltage on pin #2, you would call analogRead(1) 
// to read an analog voltage on pin #4 call analogRead(2)
#define MIC_INPUT_PIN 1

#define NUMPIXELS 30

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, DATA_LINE_PIN, NEO_GRB + NEO_KHZ800);



/**
 * turn off the entire pixel strip
 */
void pixelStripOff() {
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
  delay(PIXEL_OFF_DELAY);
}

void pixelStripRed() {
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  }
  pixels.show();
  delay(PIXEL_STATE_DELAY);
}
void pixelStripGreen() {
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));
  }
  pixels.show();
  delay(PIXEL_STATE_DELAY);
}
void pixelStripBlue() {
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));
  }
  pixels.show();
  delay(PIXEL_STATE_DELAY);
}
void pixelStripYellow() {
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 255));
  }
  pixels.show();
  delay(PIXEL_STATE_DELAY);
}

// From the Adafruit strandtest
//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
    boolean btn = false;

  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < pixels.numPixels(); i=i+3) {
          pixels.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        pixels.show();
       
        delay(wait);
       
        for (int i=0; i < pixels.numPixels(); i=i+3) {
          pixels.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
    checkButton();
    btn = doButtonPress();
    if( btn == true ) return;

  }
}

/**
 * turn on the pixel strip with a random color and initialize the hit_off_time
 */
void randomColorOn() {
  long newColor = random(LOW_COLOR,HIGH_COLOR);
  byte newColorByte = newColor & 255;
  
  for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, Wheel(newColorByte));
  }
  pixels.show();
  
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
  uint8_t brightness = 200;
  uint8_t brightnessOffset = 6;
  int8_t brightnessDirection = -1; // -1 bright to dim, +1 dim to bright
  boolean btn = false;
  
  //Adjust 60 and 90 to the starting and ending colors you want to fade between. 
  for(j=HIGH_COLOR; j>=LOW_COLOR; --j) {
    for(i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    brightness += (6*brightnessDirection);
    if( brightness < brightnessOffset || brightness > 249 ) brightnessDirection *= -1;
    pixels.setBrightness(brightness);
    delay(wait);
    if( brightness < brightnessOffset ) {
      checkButton();
      btn = doButtonPress();
      if( btn == true ) return;
    }
  }
  
// j<1170
  for(j=LOW_COLOR; j<HIGH_COLOR; j++) {
    for(i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    brightness += (6*brightnessDirection);
    if( brightness < brightnessOffset || brightness > 249 ) brightnessDirection *= -1;
    pixels.setBrightness(brightness);
    delay(wait);
    if( brightness < brightnessOffset ) {
      checkButton();
      btn = doButtonPress();
      if( btn == true ) return;
    }
  }
  
  for(i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();
  delay(wait);
}

void setup()
{
  
  // DO NOT RUN AS 16 MHZ with neopixels
 // if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  //pins driven by analogWrite do not need to be declared as outputs
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'

  randomSeed(millis());

  state = 0; // plasma state
  currentColorIndex = 0;
  now = 0;
  hit_off_time = 0;
  button_was_pressed = false;
  
  // initialize the MODE_INPUT_PIN pin as an INPUT
  pinMode(MODE_INPUT_PIN, INPUT);
  
  pixels.setBrightness(255);

}

void checkButton() {
  if ( digitalRead(MODE_INPUT_PIN) == 0) {
    button_was_pressed = true;
  }  
}

/**
 * @return 0 - button was not pressed, 1 - button was pressed
 */
boolean doButtonPress() {
  boolean rtn = button_was_pressed;
  if ( button_was_pressed == true ) {  // if the button is pressed
    button_was_pressed = false;
    pixels.setBrightness(250);
    state += 1;
    switch(state) {
     case 0: // plasma state
       pixelStripRed();
       pixelStripOff();
      break;
     
     case 1: // solid on
       pixelStripGreen();
       pixelStripOff();
       hit_off_time = 0;
      break;
     
     case 2: // sound activated
       pixelStripBlue();
       hit_off_time = 0;
       pixelStripOff();
      break;
     
     case 3: // theater chase
       pixelStripYellow();
       pixelStripOff();
       break;
       
     case 4: // turn it off
       pixelStripOff();
      break;
     
     default: // then reset back to original state
       state = 0;
       pixelStripRed();
       pixelStripOff();
      break;
      
    }    
    delay(1000); // sit here for a second to give user a chance to release the button 
  } 
  return rtn;
  
}

void loop()
{
  now = millis();
  
  checkButton();
  doButtonPress();
  
  switch(state) {
     case 0: // plasma state
        PlasmaPulse(PLASMA_TIME);
      break;
     
     case 1: // solid on
        if( now > hit_off_time) {
          randomColorOn();
          hit_off_time = now + (random(2,10)*1000);
        }
      break;
     
     case 2: // sound activated
        if( hit_off_time > 0 && now > hit_off_time ) {
          pixelStripOff();
          hit_off_time = 0;
        }
        analogVal = analogRead( MIC_INPUT_PIN );
        if( hit_off_time == 0 && analogVal > micDCOffset + HIT_THRESHOLD ) {
          randomColorOn();
          hit_off_time = now + HIT_TIME;
        }
      
      break;
     
     case 3: // theater chase
       theaterChaseRainbow(100);
       break;
       
     case 4: // no colors
       pixelStripOff();
      break;
     
     default: // then reset back to original state
      state = 0;
      break;
  
  }  
}
