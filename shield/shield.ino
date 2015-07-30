#include <Wire.h>
#include <Adafruit_LSM303.h>    // https://github.com/adafruit/Adafruit_Sensor
#include <Adafruit_NeoPixel.h>  // https://github.com/adafruit/Adafruit_NeoPixel

// Remember to import Adafruit boards into the Arduino IDE
// https://github.com/arduino/Arduino/wiki/Unofficial-list-of-3rd-party-boards-support-urls#list-of-3rd-party-boards-support-urls

Adafruit_LSM303 lsm;

// configure NeoPixel with 51 pixels attached to pin 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(51, 6, NEO_GRB + NEO_KHZ800);

uint8_t mode = 0;       // mode of operation
uint8_t last_mode = 0;  // previous mode of operation
int8_t w = 0;
uint8_t wm = 0;

uint32_t red, white, bright_white, blue, yellow;

void setup() 
{
  // configure serial port
  Serial.begin(9600);
 
  // initialize colors
  red = strip.Color(50,0,0);
  white = strip.Color(50,50,50);
  bright_white = strip.Color(200,200,200);
  blue = strip.Color(0,50,70);
  yellow = strip.Color(64,35,0);
 
  // initialize LSM303 comminucations
  if (!lsm.begin()) Serial.println("unable to initialize the LSM303");
  
  strip.begin();
  strip.show();
}


void loop() 
{
  uint8_t d = 50;     // main loop delay
  boolean cw = true;  // clockwise spin
  
  lsm.read();
  //Serial.print("Accel (x,y,z): ");
  //Serial.print((int)lsm.accelData.x); Serial.print(",");
  //Serial.print((int)lsm.accelData.y); Serial.print(",");
  //Serial.println((int)lsm.accelData.z);
  
  if( lsm.accelData.z < -800 ) { mode = 1; }                                // hold up
  else if( lsm.accelData.z > 800 ) { mode = 2; }                            // hold down
  else if ( lsm.accelData.x < 800 && lsm.accelData.y > 600 ) { mode = 3; }  // rotate right
  else if ( lsm.accelData.x < 800 && lsm.accelData.y < -600 ) { mode = 4; } // rotate left
  else { mode = 0; }

  // if mode changed since last refresh...
  if( mode != last_mode ) {    
    if( 1 == mode ) { setAllPixelColor(yellow); }
    else if ( 2 == mode ) { setAllPixelColor(red); }
    else if ( 3 == mode ) { d = 0; wm = 0; w = 50; }
    else if ( 4 == mode ) { d = 5 ; setThreeColors(red, white, blue); }
    else { d = 50 ; drawRWB(); }
    
    // randomly choose spin direction
    cw = ( random(0,100) > 50 )?true:false;
    last_mode = mode;
  }
 
  if( 3 == mode ) { fill(); }
  else { spin(cw); }
  sparkle();
  delay(d);
}


void fill() {
  uint32_t wc;
  
  if( 0 == wm ) { wc = red; }
  else if (1 == wm ) { wc = white; }
  else { wc = blue; }
  
  strip.setPixelColor(w, wc );
  w--;

  if( w < 0 ) {w = 50 ; wm++; }
  if( 3 == wm ) wm = 0;

  strip.show();
}


// Sparkle two random pixels bright white for 5ms, return them to original color
void sparkle() {
    uint8_t r1 = random(0,50);
    uint8_t r2 = random(0,50);
    uint32_t sc1 = strip.getPixelColor(r1);
    uint32_t sc2 = strip.getPixelColor(r2);
    strip.setPixelColor(r1, bright_white );
    strip.setPixelColor(r2, bright_white );
    strip.show();
    delay(5);
    strip.setPixelColor(r1, sc1 );
    strip.setPixelColor(r2, sc2 );
    strip.show();
}


void drawRWB() {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i++, red );
      strip.setPixelColor(i++, red );
      strip.setPixelColor(i++, red );
      strip.setPixelColor(i++, white );
      strip.setPixelColor(i++, white );
      strip.setPixelColor(i++, white );
      strip.setPixelColor(i++, blue );
      strip.setPixelColor(i++, blue );
      strip.setPixelColor(i,   blue );
  }
  
  strip.show();
}


// Spin pixels one position in specified direction; refresh pixels
void spin(boolean cw) {
  uint32_t c = strip.getPixelColor( (true==cw)?strip.numPixels()-1 : 0); 
  
  if( true==cw ) {
    for(uint16_t j=strip.numPixels()-1; j>0 ; j--) {
      strip.setPixelColor(j, strip.getPixelColor(j-1) );
    }
  }
  else {
    for(uint16_t j=0 ; j<strip.numPixels()-1; j++) {
      strip.setPixelColor(j, strip.getPixelColor(j+1) );
    }
  }
    
  strip.setPixelColor( (true==cw)?0:strip.numPixels()-1, c);
  
  strip.show();
}


// Fill all pixels with color c; refresh pixels
void setAllPixelColor(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}


// Divide pixels into three segments filling each with one of the specified colors; refresh pixels
void setThreeColors(uint32_t c1, uint32_t c2, uint32_t c3) {
  uint32_t c;
  
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if( i < (strip.numPixels() / 3) ) c = c1;
    else if(i < (strip.numPixels() / 3*2) ) c = c2;
    else c = c3;
   
    strip.setPixelColor(i, c);
  }
  strip.show();
}


