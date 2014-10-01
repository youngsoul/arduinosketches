      /*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.

  To upload to your Gemma or Trinket:
  1) Select the proper board from the Tools->Board Menu
  2) Select USBtinyISP from the Tools->Programmer
  3) Plug in the Gemma/Trinket, make sure you see the green LED lit
  4) For windows, install the USBtiny drivers
  5) Press the button on the Gemma/Trinket - verify you see
     the red LED pulse. This means it is ready to receive data
  6) Click the upload button above within 10 seconds
*/

#include <avr/power.h>


int analogPin = 1; // which is digital #2
// 0-5 volts for a 10bit converter the value is 0-1023
int analogVal = 0;

int led = 1; // blink 'digital' pin 1 - AKA the built in red LED
int led3 = 3;
int led4 = 4;

unsigned long startTrigger, lastTrigger;
unsigned long diff;
unsigned long t1;
int delta = 50;  // millis seconds that the line is up/down after a trigger
  
// 0 - no input yet
// 1 - received start trigger, no last
// 2 - receivd start and have last
int state = 0;
  
unsigned long trigger_debouce_time = 10; // millis seconds
  
// retrigger_time is the time ( seconds) when we can look for a new 
// trigger input
unsigned long retrigger_time;
  
unsigned long now = 0;
  
  
void tempo() {
 if( diff == 0 || state==0 ) return;
 if( now >= (t1+diff) ) {
    digitalWrite(led,HIGH);
    t1 = now;
    //tone(led4, 300, 50);
    //delay(1);
  } else {
    if( now > (t1+delta) ) {
      digitalWrite(led,LOW);
    }
  }     
}
  
void init_app() {
  startTrigger = 0;
  lastTrigger = 0;
  state = 0;
  diff = 0;
  t1 = 999999999L;
  retrigger_time = 0;
  
  digitalWrite(led,LOW);
  digitalWrite(led3,LOW);
  digitalWrite(led4,LOW);
  }
  
// the setup routine runs once when you press reset:
void setup() {
  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);

  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(led3,OUTPUT);
  pinMode(led4,OUTPUT);
  init_app();  
  
 


}

void read_analog_input() {
  
  // if we are not ready to accept new input then return
  if( state > 0 && now < retrigger_time ) {
    digitalWrite(led3,HIGH);
    return;
  } else {
    digitalWrite(led3,LOW);
  }
  
  analogVal = analogRead(analogPin);
  if(analogVal > 100) {
    // setup the retrigger time
    retrigger_time = now + trigger_debouce_time;
    
    if( state == 0 ) {
      startTrigger = millis();
      state = 1;
      //digitalWrite(led3,LOW);
    }
    if( state == 1 ) {
      lastTrigger = millis();
      state = 2;
      diff = lastTrigger - startTrigger;
      t1 = lastTrigger;
    }
    if( state == 2 ) {
      startTrigger = lastTrigger;
      lastTrigger = millis();
      diff = lastTrigger - startTrigger;
      //digitalWrite(led3,HIGH);
      digitalWrite(led4,LOW);

    }
    
    // 200 BPM = 300ms or 300,000 micro seconds
    // if it is faster than 200BPM take that as a reset
    if( diff > 30 && diff < 200 ) {
      init_app();
      digitalWrite(led4,HIGH);
      delay(500);
      digitalWrite(led4,LOW);
      
    }
  }
  
  
}

// the loop routine runs over and over again forever:
void loop() {
  now = millis();

  read_analog_input();
  tempo();
    
    //digitalWrite(led,HIGH);
} 
//  else {
//    digitalWrite(led,LOW);
//  }
//  digitalWrite(led, HIGH); 
//  delay(1000);
//  digitalWrite(led, LOW);
//  delay(1000);
//}
    
