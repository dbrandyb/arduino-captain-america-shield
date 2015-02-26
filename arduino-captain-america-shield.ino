#include <Wire.h>
#include <Adafruit_LSM303.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6

Adafruit_LSM303 lsm;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(51, PIN, NEO_GRB + NEO_KHZ800);

uint8_t mode = 0;
uint8_t last_mode = 0;
uint8_t d = 50;
boolean cw = true;
int8_t w = 0;
uint8_t wm = 0;

uint32_t red, white, bright_white, blue, yellow;

void setup() 
{
  Serial.begin(9600);
 
  strip.begin();
  strip.show();
   
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM303. Check your wiring!");
    while (1);
  }

  // Initialize colors
  red = strip.Color(50,0,0);
  white = strip.Color(50,50,50);
  bright_white = strip.Color(200,200,200);
  blue = strip.Color(0,50,70);
  yellow = strip.Color(64,35,0);
}


void loop() 
{
  lsm.read();
  //Serial.print("Accel X: "); Serial.print((int)lsm.accelData.x); Serial.print(" ");
  //Serial.print("Y: "); Serial.print((int)lsm.accelData.y);       Serial.print(" ");
  //Serial.print("Z: "); Serial.println((int)lsm.accelData.z);     Serial.print(" ");
  
  if( lsm.accelData.z < -800 ) { mode = 1; }                                // hold up
  else if( lsm.accelData.z > 800 ) { mode = 2; }                            // hold down
  else if ( lsm.accelData.x < 800 && lsm.accelData.y > 600 ) { mode = 3; }  // rotate right
  else if ( lsm.accelData.x < 800 && lsm.accelData.y < -600 ) { mode = 4; } // rotate left
  else { mode = 0; }

  if( mode != last_mode ) {    
    if( 1 == mode ) { colorFill(yellow); }
    else if ( 2 == mode ) { colorFill(red); }
    else if ( 3 == mode ) { d = 0; wm = 0; w = 50; }
    else if ( 4 == mode ) { d = 5 ; threeColors(red, white, blue); }
    else { d = 50 ; drawRWB(); }
    
    cw = ( random(0,100) > 50 )?true:false;
    last_mode = mode;
  }
 
  if( 3 == mode ) { fill(); }
  else { spin(cw, 1, 0); }
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


void spin(boolean cw, uint8_t steps, uint8_t wait) {
  for(uint16_t i=0; i<steps; i++) {
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
    
    if( 0 == wait ) {
      strip.show();
      delay(wait);
    }
  }
}

// Fill all pixels with color c; refresh pixels
void colorFill(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

// Divide pixes into three segments filling each with one of the specified colors; refresh pixels
void threeColors(uint32_t c1, uint32_t c2, uint32_t c3) {
  uint32_t c;
  
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if( i < (strip.numPixels() / 3) ) c = c1;
    else if(i < (strip.numPixels() / 3*2) ) c = c2;
    else c = c3;
   
    strip.setPixelColor(i, c);
  }
  strip.show();
}


