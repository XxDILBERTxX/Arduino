#include "FastLED.h"
#include "IRremote.h"

FASTLED_USING_NAMESPACE

#define NUM_LEDS 159
CRGB leds[NUM_LEDS];

// 0Brightness, 1Fade, 2BPM, 3FPS
int SETTING[] = {50, 15, 40, 30};
uint8_t cSetting = 2;
uint8_t brightness = 128;
uint8_t fadeby = 10;
uint8_t fps = 100;



IRrecv irrecv(11);
decode_results results;

CRGBPalette16 palette;
TBlendType cBlending;
uint8_t paletteNumber = 0;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
static uint8_t startIndex = 0;

unsigned long button_time = 0;  // the last time the output pin was toggled
unsigned long last_button_time = 50;
unsigned long last_button_pressed = 0;

void setup() {
  delay(3000);
  //Serial.begin(9600);
  FastLED.addLeds<WS2801, 10, 12, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(0);
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
  brightness = map((SETTING[0]), 0, 100, 1, 255);
  fadeby = map((SETTING[1]), 0, 100, 1, 100);
  fps = map((SETTING[3]), 0, 100, 1, 300);


  startIndex = startIndex + 1; /* motion speed */

  gPatterns[gCurrentPatternNumber]();

  if (irrecv.decode(&results)) {
    remote();
    irrecv.resume(); // Receive the next value
  }

  // do some periodic updates
  EVERY_N_MILLISECONDS( 30 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
}

// New Remote code
void remote()
{
  unsigned long button_pressed = results.value;
  int counter = 1;
  button_time = millis();
  if (button_time - last_button_time > 250) {
    if (button_pressed == 0xFFFFFFFF) {
      button_pressed = last_button_pressed;
      counter ++;
    }
    else {
      counter = 1;
    }
    switch (button_pressed) {
      case 0xFD8877:  //Up
        SETTING[cSetting] += counter;
        if (SETTING[cSetting] > 100) {
          SETTING[cSetting] = 0;
        }
        break;
      case 0xFD9867:  //Down
        SETTING[cSetting] -= counter;
        if (SETTING[cSetting] < 0) {
          SETTING[cSetting] = 100;
        }
        break;
      case 0xFDA857:  //Ok

        break;
      case 0xFD28D7:  //Left
        cSetting = (cSetting + 1) % ARRAY_SIZE(SETTING);
        leds[cSetting] = CRGB::White;
        FastLED.show();
        break;
      case 0xFD6897:  //Right
        cSetting --;
        if (cSetting == 255) {
          cSetting = (ARRAY_SIZE(SETTING) - 1);
        }
        leds[cSetting] = CRGB::White;
        FastLED.show();
        break;

      case 0xFD30CF:  //*
        gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
        break;
      case 0xFD708F:  //#
        ChangePalette();
        break;

      case 0xFD00FF:  //1
        break;
      case 0xFD807F:  //2
        break;
      case 0xFD40BF:  //3
        break;
      case 0xFD20DF:  //4
        break;
      case 0xFDA05F:  //5
        break;
      case 0xFD609F:  //6
        break;
      case 0xFD10EF:  //7
        break;
      case 0xFD906F:  //8
        break;
      case 0xFD50AF:  //9
        break;
      case 0xFDB04F:  //0
        break;
    }
    last_button_time = button_time;
    last_button_pressed = button_pressed;
    //Debuging Stuff
    //Serial.print(button_pressed, HEX);
    //Serial.write(' ');
    //Serial.print(cSetting);
    //Serial.write(' ');
    //Serial.println(SETTING[cSetting]);
  }
}


void showLED()
{
  FastLED.setBrightness(brightness);
  FastLED.show();
  FastLED.delay(1000 / fps);
}

// Patterns

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, fadeby);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
  showLED();
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, fadeby);
  int pos = beatsin16(SETTING[2], 0, NUM_LEDS);
  leds[pos] += CHSV(gHue, 255, 192);
  showLED();
}

void cylon()
{
  // a colored dot sweeping back and forth, with fading trails
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue++, 255, 192);
    showLED();
    fadeToBlackBy(leds, NUM_LEDS, fadeby);
  }
  // Now the otherway
  for (int i = (NUM_LEDS) - 1; i >= 0; i--) {
    leds[i] = CHSV(gHue++, 255, 192);
    showLED();
    fadeToBlackBy(leds, NUM_LEDS, fadeby);
  }
}

void beam()
{
  // Beam
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue++, 255, 192);
    showLED();
    fadeToBlackBy(leds, NUM_LEDS, fadeby);
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, fadeby);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  showLED();
}

// Palettes
void palettecolors()
{
  uint8_t colorIndex = startIndex;
  showLED();
  for ( int i = 0; i < NUM_LEDS; i++) {
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
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = map((SETTING[2]), 0, 100, 1, 60);
  uint8_t beat = beatsin8(BeatsPerMinute, 0, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10), cBlending);
  }
  showLED();
}
void ChangePalette()
{
  paletteNumber += 1;
  if ( paletteNumber >= 21)
  {
    paletteNumber = 0;
  }
  if ( paletteNumber == 0)  {
    palette = RainbowColors_p;
    cBlending = NOBLEND;
  }
  if ( paletteNumber == 1)  {
    palette = RainbowColors_p;
    cBlending = LINEARBLEND;
  }
  if ( paletteNumber == 2)  {
    palette = RainbowStripeColors_p;
    cBlending = NOBLEND;
  }
  if ( paletteNumber == 3)  {
    palette = RainbowStripeColors_p;
    cBlending = LINEARBLEND;
  }
  if ( paletteNumber == 4)  {
    palette = PartyColors_p;
    cBlending = NOBLEND;
  }
  if ( paletteNumber == 5)  {
    palette = PartyColors_p;
    cBlending = LINEARBLEND;
  }
  if ( paletteNumber == 6)  {
    palette = HeatColors_p;
    cBlending = NOBLEND;
  }
  if ( paletteNumber == 7)  {
    palette = HeatColors_p;
    cBlending = LINEARBLEND;
  }
  if ( paletteNumber == 8)  {
    palette = LavaColors_p;
    cBlending = NOBLEND;
  }
  if ( paletteNumber == 9)  {
    palette = LavaColors_p;
    cBlending = LINEARBLEND;
  }
  if ( paletteNumber == 10)  {
    palette = CloudColors_p;
    cBlending = NOBLEND;
  }
  if ( paletteNumber == 11)  {
    palette = CloudColors_p;
    cBlending = LINEARBLEND;
  }
  if ( paletteNumber == 12)  {
    palette = OceanColors_p;
    cBlending = NOBLEND;
  }
  if ( paletteNumber == 13)  {
    palette = OceanColors_p;
    cBlending = LINEARBLEND;
  }
  if ( paletteNumber == 14)  {
    palette = ForestColors_p;
    cBlending = NOBLEND;
  }
  if ( paletteNumber == 15)  {
    palette = ForestColors_p;
    cBlending = LINEARBLEND;
  }
  if ( paletteNumber == 16)  {
    palette = myRedWhiteBluePalette_p;
    cBlending = NOBLEND;
  }
  if ( paletteNumber == 17)  {
    palette = myRedWhiteBluePalette_p;
    cBlending = LINEARBLEND;
  }
  if ( paletteNumber == 18)  {
    SetupTotallyRandomPalette();
    cBlending = LINEARBLEND;
  }
  if ( paletteNumber == 19)  {
    SetupBlackAndWhiteStripedPalette();
    cBlending = NOBLEND;
  }
  if ( paletteNumber == 20)  {
    SetupBlackAndWhiteStripedPalette();
    cBlending = LINEARBLEND;
  }
}
void SetupTotallyRandomPalette()
{
  for ( int i = 0; i < 16; i++) {
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
