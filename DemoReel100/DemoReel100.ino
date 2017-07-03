#include "FastLED.h"
#include <medianFilter.h>

medianFilter Filter;

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    7
#define CLK_PIN   13
#define LED_TYPE    WS2801
#define COLOR_ORDER GRB
#define NUM_LEDS    160
CRGB leds[NUM_LEDS];
#define BRIGHTNESS          128
#define FRAMES_PER_SECOND  120

#define POT1PIN A1
#define MODE_PIN 0 // pin2(Interrupt)
#define TYPE_PIN 8

int pot1val = 0;
int pot1lvl = 0;
int pot1out = 0;
int pot1filt = 0;

int modepinState = 0;
int typepinState = 0;

unsigned long button_time = 0;  // the last time the output pin was toggled
unsigned long last_button_time = 50;

void setup() {
  delay(3000); // 3 second delay for recovery
  Serial.begin(9600);
  Filter.begin();
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  
  //Buttons
  attachInterrupt(MODE_PIN, nextPattern, FALLING);
  pinMode(TYPE_PIN, INPUT);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, cylon, beam, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 
  
  
  pot1val = analogRead(POT1PIN);
  pot1filt = Filter.run(pot1val);
  pot1out = map(pot1filt, 0, 1023, 0, 255);
  pot1lvl = map(pot1filt, 0, 1023, 0, 100);
  
  // read the state of the pushbutton value:
  //modepinState = digitalRead(MODE_PIN);
  typepinState = digitalRead(TYPE_PIN);
  
  //if (typepinState == HIGH) {     
  //  nextPattern();  
  //} 
  
  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 60 ) { nextPattern(); } // change patterns periodically
  
  //Serial.println(pot1val);
  //Serial.println(pot1out);
  //Serial.println(pot1lvl);
  Serial.println(pot1filt);
  //Serial.println();
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  button_time = millis();
  if (button_time - last_button_time > 250)
  {
    // add one to the current pattern number, and wrap around at the end
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
    last_button_time = button_time;
  }
}

// Patterns

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void cylon()
{
  // a colored dot sweeping back and forth, with fading trails
  for(int i = 0; i < NUM_LEDS; i++) {
		// Set the i'th led to red 
		leds[i] = CHSV( gHue++, 255, 192);
		// Show the leds
		FastLED.show(); 
		// now that we've shown the leds, fade to black
                fadeToBlackBy( leds, NUM_LEDS, 20);
		// Wait a bit (Speed)
		delay(10);
  }
  // Now the otherway
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
		leds[i] = CHSV( gHue++, 255, 192);
		FastLED.show();
                fadeToBlackBy( leds, NUM_LEDS, 20);
		delay(10);
  }
}

void beam()
{
  // Beam
  for(int i = 0; i < NUM_LEDS; i++) {
		// Set the i'th led to red 
		leds[i] = CHSV( gHue++, 255, 192);
		// Show the leds
		FastLED.show(); 
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		//fadeall();
                fadeToBlackBy( leds, NUM_LEDS, 4);
		// Wait a bit (Speed)
		delay(10);
  }
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( pot1lvl, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

