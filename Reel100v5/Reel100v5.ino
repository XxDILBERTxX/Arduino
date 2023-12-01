#include "FastLED.h"
#include "IRLibAll.h"  // Seeed Arduino IR

FASTLED_USING_NAMESPACE

#define NUM_LEDS 159
CRGB leds[NUM_LEDS];

// 0Brightness, 1Fade, 2BPM, 3FPS
int SETTING[] = { 50, 15, 40, 30 };
uint8_t cSetting = 2;
uint8_t brightness = 92;
uint8_t fadeby = 10;
uint8_t fps = 100;

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING 70

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 80


IRrecvPCI irrecv(2);
IRdecode results;

CRGBPalette16 palette;
TBlendType cBlending;
uint8_t paletteNumber = 0;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

static uint8_t startIndex = 0;
unsigned long button_time = 0;  // the last time the output pin was toggled
unsigned long last_button_time = 50;
unsigned long last_button_pressed = 0;
//bool bail_out = false;

void setup() {
  delay(3000);
  //Serial.begin(9600);
  //Serial.println(F("Ready"));
  FastLED.addLeds<WS2801, 10, 12, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(0);
  irrecv.enableIRIn();  // Start the receiver
  palette = RainbowColors_p;
  cBlending = LINEARBLEND;
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { firepal, palettecolors, paletteWithGlitter, bpm, confetti, beam, cylon, juggle };  //BouncingBalls
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t gCurrentPatternNumber = 1;  // Index number of which pattern is current
uint8_t gHue = 0;                   // rotating "base color" used by many of the patterns

void loop() {
  brightness = map((SETTING[0]), 0, 100, 1, 255);
  fadeby = map((SETTING[1]), 0, 100, 1, 100);
  fps = map((SETTING[3]), 0, 100, 1, 300);

  random16_add_entropy(random());
  startIndex = startIndex + 1; /* motion speed */

  if (irrecv.getResults()) {
    results.decode();  //Decode it
    remote();
    irrecv.enableIRIn();  //Restart receiver
  }

  gPatterns[gCurrentPatternNumber]();

  // do some periodic updates
  EVERY_N_MILLISECONDS(500) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
}

// New Remote code
void remote() {
  unsigned long button_pressed = results.value;
  int counter = 1;
  button_time = millis();
  if (button_time - last_button_time > 250) {
    if (button_pressed == 0xFFFFFFFF) {
      button_pressed = last_button_pressed;
      counter++;
    } else {
      counter = 1;
    }
    switch (button_pressed) {

      case 0xFD8877:  //Up
        SETTING[cSetting] += counter;
        if (SETTING[cSetting] > 100) {
          SETTING[cSetting] = 0;
        }
        flashLED(SETTING[cSetting]);
        break;
      case 0xFD9867:  //Down
        SETTING[cSetting] -= counter;
        if (SETTING[cSetting] < 0) {
          SETTING[cSetting] = 100;
        }
        flashLED(SETTING[cSetting]);
        break;
      case 0xFDA857:  //Ok

        break;
      case 0xFD28D7:  //Left
        cSetting = (cSetting + 1) % ARRAY_SIZE(SETTING);
        flashLED(cSetting);
        break;
      case 0xFD6897:  //Right
        cSetting--;
        if (cSetting == 255) {
          cSetting = (ARRAY_SIZE(SETTING) - 1);
        }
        flashLED(cSetting);
        break;

      case 0xFD30CF:  //*
        gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
        flashLED(gCurrentPatternNumber);
        break;
      case 0xFD708F:  //#
        ChangePalette();
        flashLED(paletteNumber);
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
  }
}

// Send to LED stip
void showLED() {
  FastLED.setBrightness(brightness);
  FastLED.show();
  FastLED.delay(1000 / fps);
}
void flashLED(int ledN) {
  FastLED.setBrightness(255);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  leds[ledN] = CRGB::White;
  FastLED.show();
  FastLED.delay(100);
  leds[ledN] = CRGB::Black;
  FastLED.show();
  FastLED.delay(100);
  leds[ledN] = CRGB::White;
  FastLED.show();
}
// Patterns

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, fadeby);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(128), 255, 255);
  showLED();
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, fadeby);
  int pos = beatsin16(SETTING[2], 0, NUM_LEDS);
  leds[pos] += CHSV(gHue, 255, 192);
  showLED();
}

void beam() {
  // Beam
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue++, 255, 192);
    showLED();
    fadeToBlackBy(leds, NUM_LEDS, fadeby);
    if (irrecv.getResults()) {
      results.decode(); 
      remote();
      irrecv.enableIRIn();
      return;
    }
  }
}

void cylon() {
  // a colored dot sweeping back and forth, with fading trails
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(gHue++, 255, 192);
    showLED();
    fadeToBlackBy(leds, NUM_LEDS, fadeby);
    if (irrecv.getResults()) {
      results.decode(); 
      remote();
      irrecv.enableIRIn();
      return;
    }
  }
  // Now the otherway
  for (int i = (NUM_LEDS)-1; i >= 0; i--) {
    leds[i] = CHSV(gHue++, 255, 192);
    showLED();
    fadeToBlackBy(leds, NUM_LEDS, fadeby);
    if (irrecv.getResults()) {
      results.decode(); 
      remote();
      irrecv.enableIRIn();
      return;
    }
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, fadeby);
  byte dothue = 0;
  for (int i = 0; i < 4; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue + gHue, 255, 255);
    dothue += 32;
  }
  showLED();
}

// Palettes
void palettecolors() {
  uint8_t colorIndex = startIndex;
  showLED();
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(palette, colorIndex, SETTING[0], cBlending);
    colorIndex += 2;
  }
}
void paletteWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  palettecolors();
  addGlitter(80);
}
void addGlitter(fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}
void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = map((SETTING[2]), 0, 100, 1, 60);
  uint8_t beat = beatsin8(BeatsPerMinute, 0, 255);
  for (int i = 0; i < NUM_LEDS; i++) {  //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10), cBlending);
  }
  showLED();
}
void firepal() {
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (random8() < SPARKING) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < NUM_LEDS; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8(heat[j], 240);
    leds[j] = ColorFromPalette(palette, colorindex);
  }
  showLED();
}

///////////////////////////////////////

byte colors[3][3] = {
  { 0xff, 0x00, 0x00 },
  { 0xff, 0xff, 0xff },
  { 0x00, 0x00, 0xff }
};
int BallCount = 3;
boolean continuous = true;

void BouncingBalls() {
  float Gravity = -9.81;
  int StartHeight = 1;

  float Height[BallCount];
  float ImpactVelocityStart = sqrt(-2 * Gravity * StartHeight);
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int Position[BallCount];
  long ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
  boolean ballBouncing[BallCount];
  boolean ballsStillBouncing = true;

  for (int i = 0; i < BallCount; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
    ballBouncing[i] = true;
  }

  while (ballsStillBouncing) {
    for (int i = 0; i < BallCount; i++) {
      TimeSinceLastBounce[i] = millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce[i] / 1000, 2.0) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

      if (Height[i] < 0) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if (ImpactVelocity[i] < 0.01) {
          if (continuous) {
            ImpactVelocity[i] = ImpactVelocityStart;
          } else {
            ballBouncing[i] = false;
          }
        }
      }
      Position[i] = round(Height[i] * (NUM_LEDS - 1) / StartHeight);
    }
    ballsStillBouncing = false;  // assume no balls bouncing
    for (int i = 0; i < BallCount; i++) {
      leds[Position[i]] = CHSV(colors[i][0], colors[i][1], colors[i][2]);
      //leds[Position[i]] = CHSV(colors[i][0], colors[i][1], colors[i][2]);
      if (ballBouncing[i]) {
        ballsStillBouncing = true;
      }
    }
    showLED();
    fadeToBlackBy(leds, NUM_LEDS, 100);
  }
}

////////////////////////////////////////

void ChangePalette() {
  paletteNumber += 1;
  if (paletteNumber >= 23) {
    paletteNumber = 0;
  }
  if (paletteNumber == 0) {
    palette = RainbowColors_p;
    cBlending = NOBLEND;
  }
  if (paletteNumber == 1) {
    palette = RainbowColors_p;
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 2) {
    palette = RainbowStripeColors_p;
    cBlending = NOBLEND;
  }
  if (paletteNumber == 3) {
    palette = RainbowStripeColors_p;
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 4) {
    palette = PartyColors_p;
    cBlending = NOBLEND;
  }
  if (paletteNumber == 5) {
    palette = PartyColors_p;
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 6) {
    palette = HeatColors_p;
    cBlending = NOBLEND;
  }
  if (paletteNumber == 7) {
    palette = HeatColors_p;
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 8) {
    palette = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::Gray);
    cBlending = NOBLEND;
  }
  if (paletteNumber == 9) {
    palette = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::Gray);
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 10) {
    palette = LavaColors_p;
    cBlending = NOBLEND;
  }
  if (paletteNumber == 11) {
    palette = LavaColors_p;
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 12) {
    palette = CloudColors_p;
    cBlending = NOBLEND;
  }
  if (paletteNumber == 13) {
    palette = CloudColors_p;
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 14) {
    palette = OceanColors_p;
    cBlending = NOBLEND;
  }
  if (paletteNumber == 15) {
    palette = OceanColors_p;
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 16) {
    palette = ForestColors_p;
    cBlending = NOBLEND;
  }
  if (paletteNumber == 17) {
    palette = ForestColors_p;
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 18) {
    palette = myRedWhiteBluePalette_p;
    cBlending = NOBLEND;
  }
  if (paletteNumber == 19) {
    palette = myRedWhiteBluePalette_p;
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 20) {
    SetupTotallyRandomPalette();
    cBlending = LINEARBLEND;
  }
  if (paletteNumber == 21) {
    SetupBlackAndWhiteStripedPalette();
    cBlending = NOBLEND;
  }
  if (paletteNumber == 22) {
    SetupBlackAndWhiteStripedPalette();
    cBlending = LINEARBLEND;
  }
}
void SetupTotallyRandomPalette() {
  for (int i = 0; i < 16; i++) {
    palette[i] = CHSV(random8(), 255, random8());
  }
}
void SetupBlackAndWhiteStripedPalette() {
  // 'black out' all 16 palette entries...
  fill_solid(palette, 16, CRGB::Black);
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
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
  CRGB::Red,
  CRGB::Gray,  // 'white' is too bright compared to red and blue
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
