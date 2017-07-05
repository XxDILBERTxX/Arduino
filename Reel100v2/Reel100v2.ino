#include "FastLED.h"
#include "IRremote.h"

FASTLED_USING_NAMESPACE

#define NUM_LEDS 159
CRGB leds[NUM_LEDS];
#define FRAMES_PER_SECOND 120

uint8_t brightness = 128;
uint8_t fadeBy = 15;
uint8_t BeatsPerMinute = 40;

int SETTING[] = {128, 15, 40};
uint8_t cSetting = 3;

IRrecv irrecv(11);
decode_results results;

CRGBPalette16 palette;
TBlendType cBlending;
uint8_t paletteNumber = 0;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

unsigned long button_time = 0;  // the last time the output pin was toggled
unsigned long last_button_time = 50;
static uint8_t startIndex = 0;

void setup() {
  delay(3000);
  //Serial.begin(9600);
  FastLED.addLeds<WS2801,10,12,RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(SETTING[0]);
  irrecv.enableIRIn(); // Start the receiver
  palette = RainbowColors_p;
  cBlending = LINEARBLEND;
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { palettecolors, paletteWithGlitter, bpm, confetti, cylon, beam, juggle };
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
  startIndex = startIndex + 1; /* motion speed */
  
  gPatterns[gCurrentPatternNumber]();

  if (irrecv.decode(&results)) {
    button_time = millis();
    if (button_time - last_button_time > 250) {
      if (results.value == 0xFD8877) //Up
      {
        SETTING[cSetting] = (SETTING[cSetting] + 1) % 100;
      }
      if (results.value == 0xFD9867) //Down
      {
        SETTING[cSetting] = (SETTING[cSetting] - 1) % 100;
      }
      if (results.value == 0xFDA857) //Ok
      {
        //Do stuff
      }
      if (results.value == 0xFD28D7) //Left
      {
        cSetting = (cSetting - 1) % ARRAY_SIZE(SETTING);
      }
      if (results.value == 0xFD6897) //Right
      {
        cSetting = (cSetting + 1) % ARRAY_SIZE(SETTING);
      }
      if (results.value == 0xFD30CF) //*
      {
        gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
      }
      if (results.value == 0xFD708F) //#
      {
        ChangePalette();
      }
      last_button_time = button_time;
      //Serial.println(SETTING[cSetting]);
    }
    irrecv.resume(); // Receive the next value
  }
  
  // do some periodic updates
  EVERY_N_MILLISECONDS( 30 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

void showLED()
{ 
  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

// Patterns

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, SETTING[1]);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
  showLED();
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, SETTING[1]);
  int pos = beatsin16(SETTING[2],0,NUM_LEDS);
  leds[pos] += CHSV(gHue, 255, 192);
  showLED(); 
}

void cylon()
{
  // a colored dot sweeping back and forth, with fading trails
  for(int i = 0; i < NUM_LEDS; i++) {
		leds[i] = CHSV(gHue++, 255, 192);
		showLED();
    fadeToBlackBy(leds, NUM_LEDS, SETTING[1]);
  }
  // Now the otherway
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
		leds[i] = CHSV(gHue++, 255, 192);
		showLED(); 
    fadeToBlackBy(leds, NUM_LEDS, SETTING[1]);
  }
}

void beam()
{
  // Beam
  for(int i = 0; i < NUM_LEDS; i++) {
		leds[i] = CHSV(gHue++, 255, 192);
		showLED();
    fadeToBlackBy(leds, NUM_LEDS, SETTING[1]);
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, SETTING[1]);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  showLED(); 
}

// Palettes
void palettecolors()
{
  uint8_t colorIndex = startIndex;
  showLED();
  for( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = ColorFromPalette(palette, colorIndex, SETTING[0], cBlending);
      colorIndex += 3;
  } 
}
void paletteWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  palettecolors();
  addGlitter(80);
}
void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  //uint8_t BeatsPerMinute = map(pot1filt, 10, 990, 0, 100);
  uint8_t beat = beatsin8(SETTING[2], 0, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10), cBlending);
  }
  showLED(); 
}
void ChangePalette()
{
  paletteNumber += 1;
  if( paletteNumber >= 10)
  {
    paletteNumber = 0;
  }
  if( paletteNumber == 0)  { palette = RainbowColors_p;           cBlending = LINEARBLEND; }
  if( paletteNumber == 1)  { palette = RainbowStripeColors_p;     cBlending = NOBLEND;     }
  if( paletteNumber == 2)  { palette = RainbowStripeColors_p;     cBlending = LINEARBLEND; }
  if( paletteNumber == 3)  { SetupTotallyRandomPalette();         cBlending = LINEARBLEND; }
  if( paletteNumber == 4)  { SetupBlackAndWhiteStripedPalette();  cBlending = NOBLEND;     }
  if( paletteNumber == 5)  { SetupBlackAndWhiteStripedPalette();  cBlending = LINEARBLEND; }
  if( paletteNumber == 6)  { palette = CloudColors_p;             cBlending = LINEARBLEND; }
  if( paletteNumber == 7)  { palette = PartyColors_p;             cBlending = LINEARBLEND; }
  if( paletteNumber == 8)  { palette = myRedWhiteBluePalette_p;   cBlending = NOBLEND;     }
  if( paletteNumber == 9)  { palette = myRedWhiteBluePalette_p;   cBlending = LINEARBLEND; }
}
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        palette[i] = CHSV( random8(), 255, random8());
    }
}
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( palette, 16, CRGB::Black);
    // and set every fourth one to white.
    palette[0] = CRGB::White;
    palette[4] = CRGB::White;
    palette[8] = CRGB::White;
    palette[12] = CRGB::White;
    
}
// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};
