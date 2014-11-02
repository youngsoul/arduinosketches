/*
 * LedBrightness sketch
 * controls the brightness of LEDs on "analog" (PWM) output ports.
 */
// https://github.com/1ChicagoDave/Random_NeoPixel_Effects
#include <Adafruit_NeoPixel.h>

#define DEBOUNCE_TIME  500
long button_bounce_time = 0L;

// 0-5 volts for a 10bit converter the value is 0-1023
int analogVal = 0;
int micDCOffset = 1024/2;  // dc offset is vcc/2 so the 'zero' point is at 2.5 volts

// Processing State:
// 0 - plasma state
// 1 - mic trigger
// 2 - off
int state = 0;

#define LOW_COLOR  0
#define HIGH_COLOR  255

int currentColorIndex = 0;
#define PLASMA_TIME 250


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


void pixelStripOff() {
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}


void showHit() {
  long newColor = random(LOW_COLOR,HIGH_COLOR);
  byte newColorByte = newColor & 255;
  
  for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, Wheel(newColorByte));
  }
  pixels.show();
  
  delay(1000);
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
  uint8_t brightness = 200;
  uint8_t brightnessOffset = 6;
  int8_t brightnessDirection = -1; // -1 bright to dim, +1 dim to bright

//  for(i=0; i<NUMPIXELS; i++) {
//    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
//  }
//  pixels.show();
//  delay(wait);
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

  state = 1; // plasma state
  currentColorIndex = 0;
  button_bounce_time = 0L;
  
  // initialize the MODE_INPUT_PIN pin as an INPUT
  pinMode(MODE_INPUT_PIN, OUTPUT);
  
  // ...with a pullup
  digitalWrite(MODE_INPUT_PIN, HIGH);
 
  
  pixels.setBrightness(255);

}


void loop() {
//    PlasmaPulse(PLASMA_TIME);
//  digitalWrite(MODE_INPUT_PIN,HIGH);
//  showHit();
//  delay(1000);
//  digitalWrite(MODE_INPUT_PIN,LOW);
//  delay(1000);
  
  
  _loop();
}

void _loop()
{
  long now = millis();
  
  if ( digitalRead(MODE_INPUT_PIN) == 0 && now > button_bounce_time) {  // if the button is pressed
    button_bounce_time = ( now + DEBOUNCE_TIME );
    
    state += 1;
    if( state >= 3 ) state = 0;
    pixelStripOff();
    if( state == 1 ) {
        pixels.setBrightness(250);
    }

  } 
  
  if( state == 0 ) {
    PlasmaPulse(PLASMA_TIME);
  } else if( state == 1 ) {
    analogVal = analogRead( MIC_INPUT_PIN );
    if( analogVal > micDCOffset + 1 ) {
      showHit();
    }
    
  }
}
