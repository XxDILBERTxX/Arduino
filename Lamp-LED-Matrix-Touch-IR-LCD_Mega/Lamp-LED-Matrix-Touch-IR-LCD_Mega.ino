//LED
#include <FastLED.h> //Wire in setup// Wires = Yellow, White (D5 Connector)
#define ledMatrixWidth  5
#define ledMatrixHeight 32
#define MODEL_WIDTH  (ledMatrixWidth + 2)
#define MODEL_HEIGHT (ledMatrixHeight + 2)
#define NUM_LEDS (ledMatrixWidth * ledMatrixHeight)
#define MAX_DIMENSION ((ledMatrixWidth>ledMatrixHeight) ? ledMatrixWidth : ledMatrixHeight)
CRGB leds[NUM_LEDS];

//Touch
#include <Wire.h>
#include <GroveMultiTouch.h> //Wire SDA/SCL (I2C Connector)
GroveMultiTouch feelers(8); //Wire pin8(D4 Connector) Int
boolean padTouched[4]; //Number of Feelers

//LCD
#include <rgb_lcd.h> //Wire SDA/SCL (I2C Connector)
String COLPALNAME;
rgb_lcd lcd;

//Ir Remote
#include <IRremote.h>
IRrecv irrecv(4);
decode_results results;

// Global\Dynamic
boolean MENUMODE_DO = false;
boolean MENUMODE_OK = true;
boolean wastouched = true;
boolean remoteuse = false;
boolean light = false;
byte remote;
unsigned long current_time = 0;
unsigned long touched_time = 0;
unsigned long last_touched_time = 0;
int MENUMODE = 0;
int counter = 1;
CRGBPalette16 gPal(HeatColors_p);
byte colorR = 0;
byte colorG = 0;
byte colorB = 0;

//Menu-Mode (Defaults)
String SETID[] =
{
	"Mode",
	"FPS/Speed",
	"Palette",
	"Cooling",
	"Sparking",
	"Zoom",
	"Colors Loop ?",
	"Around Speed",
	"Up-n-Down Speed",
	"Launch Timer",
	"Brightness"
};
int SETTING[] = {2, 0, 4, 0, 0, 0, 0, 0, 0, 0, 255};
int SETMIN[] = {0, 1, 0, 0, 0, 0, 0, 1, 1, 5, 0};
int SETMAX[] = {3, 1000, 11, 100, 100, 1000, 1, 100, 100, 32766, 255};

String MODEID[] =
{
	"FireWorks",
	"Fire",
	"Noise Water",
	"Fly (WIP)"
};

#define MAXMENUMODE (sizeof(SETTING) / sizeof(SETTING[0]))
#define MAXMODE (sizeof(MODEID) / sizeof(MODEID[0]))
int MODESET[MAXMODE][MAXMENUMODE] = // Mode Settings List (Add Setting # to allow)
{
	0, 1, 0, 0, 0, 0, 0, 0, 0, 9, 10,
	0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 10,
	0, 1, 2, 0, 0, 5, 6, 0, 0, 0, 10,
	0, 0, 0, 0, 0, 0, 0, 7, 8, 0, 10
};

#define PIXEL_X_OFFSET ((MODEL_WIDTH  - ledMatrixWidth ) / 2)
#define PIXEL_Y_OFFSET ((MODEL_HEIGHT - ledMatrixHeight) / 2)
#define WINDOW_X_MIN (PIXEL_X_OFFSET)
#define WINDOW_X_MAX (WINDOW_X_MIN + ledMatrixWidth - 1)
#define WINDOW_Y_MIN (PIXEL_Y_OFFSET)
#define WINDOW_Y_MAX (WINDOW_Y_MIN + ledMatrixHeight - 1)
CRGB overrun;
CRGB& XYs(byte x, byte y)
{
	if (SETTING[0] == 0)
	{
		x -= PIXEL_X_OFFSET;
		y -= PIXEL_Y_OFFSET;
	}
	if (x < ledMatrixWidth && y < ledMatrixHeight)
	{
		uint16_t i;
		if ( x & 0x01)
		{
			// Even rows run forwards
			uint8_t reverseX = (ledMatrixHeight - 1) - y;
			i = (x * ledMatrixHeight) + reverseX;
		}
		else
		{
			// Even rows run forwards
			i = (x * ledMatrixHeight) + y;
		}
		return leds[i];
		//return i;
	}
	else
	{
		return overrun;
	}
}

void screenscale( accum88 a, byte N, byte& screen, byte& screenerr)
{
	byte ia = a >> 8;
	screen = scale8( ia, N);
	byte m = screen * (256 / N);
	screenerr = (ia - m) * scale8(255, N);
	return;
}
int16_t scale15by8_local( int16_t i, fract8 scale )
{
	int16_t result;
	result = (int32_t)((int32_t)i * scale) / 256;
	return result;
}

//
static uint16_t x = random16();
static uint16_t y = random16();
static uint16_t z = random16(); // Time/Speed
uint8_t ledmap[ledMatrixWidth][ledMatrixHeight];
uint8_t moisemap[MAX_DIMENSION][MAX_DIMENSION];

//FireWorks
#define NUM_DOTS 3
#define NUM_SPARKS 30
saccum78 gGravity = 10;
fract8 gBounce = 200;
fract8 gDrag = 255;
bool gSkyburst = 0;
//
accum88 gBurstx;
accum88 gBursty;
saccum78 gBurstxv;
saccum78 gBurstyv;
CRGB gBurstcolor;
#define NONE 0
#define SHELL 1
#define SPARK 2
class Dot
{
	public:
	byte    show;
	byte    theType;
	accum88 x;
	accum88 y;
	saccum78 xv;
	saccum78 yv;
	accum88 r;
	CRGB    color;

	Dot()
	{
		show = 0;
		theType = 0;
		x =  0;
		y =  0;
		xv = 0;
		yv = 0;
		r  = 0;
		color.setRGB( 0, 0, 0);
	}

	void Draw()
	{
		if ( !show) return;
		byte ix, xe, xc;
		byte iy, ye, yc;
		screenscale( x, MODEL_WIDTH, ix, xe);
		screenscale( y, MODEL_HEIGHT, iy, ye);
		yc = 255 - ye;
		xc = 255 - xe;

		CRGB c00 = CRGB( dim8_video( scale8( scale8( color.r, yc), xc)),
		dim8_video( scale8( scale8( color.g, yc), xc)),
		dim8_video( scale8( scale8( color.b, yc), xc))
		);
		CRGB c01 = CRGB( dim8_video( scale8( scale8( color.r, ye), xc)),
		dim8_video( scale8( scale8( color.g, ye), xc)),
		dim8_video( scale8( scale8( color.b, ye), xc))
		);

		CRGB c10 = CRGB( dim8_video( scale8( scale8( color.r, yc), xe)),
		dim8_video( scale8( scale8( color.g, yc), xe)),
		dim8_video( scale8( scale8( color.b, yc), xe))
		);
		CRGB c11 = CRGB( dim8_video( scale8( scale8( color.r, ye), xe)),
		dim8_video( scale8( scale8( color.g, ye), xe)),
		dim8_video( scale8( scale8( color.b, ye), xe))
		);

		XYs(ix, iy) += c00;
		XYs(ix, iy + 1) += c01;
		XYs(ix + 1, iy) += c10;
		XYs(ix + 1, iy + 1) += c11;
	}

	void Move()
	{
		if ( !show) return;
		yv -= gGravity;
		xv = scale15by8_local( xv, gDrag);
		yv = scale15by8_local( yv, gDrag);

		if ( theType == SPARK)
		{
			xv = scale15by8_local( xv, gDrag);
			yv = scale15by8_local( yv, gDrag);
			color.nscale8( 255);
			if ( !color)
			{
				show = 0;
			}
		}

		if ( yv < 0 && (y < (-yv)) ) // if we'd hit the ground, bounce
		{
			if ( theType == SPARK )
			{
				show = 0;
			}
			else
			{

				yv = -yv;
				yv = scale15by8_local(yv, gBounce);
				if ( yv < 500 )
				{
					show = 0;
				}
			}
		}

		if (yv < -300) // pinnacle
		{
			if ( theType == SHELL )
			{
				if ( (y > (32768)) && (random8() < 64))  // boom
				{
					lcd.setRGB(254, 254, 254);
					LEDS.showColor( CRGB::White);
					LEDS.showColor( CRGB::Black);
					lcd.setRGB(colorR, colorG, colorB);
				}
				show = 0;
				gSkyburst = 1;
				gBurstx = x;
				gBursty = y;
				gBurstxv = xv;
				gBurstyv = yv;
				gBurstcolor = color;
			}
		}

		if ( theType == SPARK)
		{
			if ( ((xv >  0) && (x > xv)) ||
			((xv < 0 ) && (x < (65535 + xv))) ) //65535 with 0xFFFF
			{
				x += xv;
			}
			else
			{
				show = 0;
			}
		}

		else
		{
			x += xv;
		}
		y += yv;

	}

	void GroundLaunch()
	{
		yv = 700 + random16(500);  // 700 + ~500
		xv = (int16_t)random16(600) - (int16_t)300;
		y = 0;
		//x = 0x8000; //=32768 / 16384 / 8192
		x = ((0x8000) + ((int16_t)random16(16384) - (int16_t)8192));
		hsv2rgb_rainbow( CHSV( random8(), 240, 200), color);
		theType = SHELL;
		show = 1;
	}

	void Skyburst( accum88 basex, accum88 basey, saccum78 basedv, CRGB& basecolor)
	{
		yv = (int16_t)0 + (int16_t)random16(1500) - (int16_t)500;
		xv = basedv + (int16_t)random16(2000) - (int16_t)1000;
		y = basey;
		x = basex;
		color = basecolor;
		color *= 4;
		theType = SPARK;
		show = 1;
	}
};
Dot gDot[NUM_DOTS];
Dot gSparks[NUM_DOTS][NUM_SPARKS];

void setup()
{
	delay(2000);
	LEDS.setBrightness(0);
	LEDS.addLeds<WS2801, 5, 6>(leds, NUM_LEDS); // Wires = Yellow, White  (D5 Connector)

	lcd.begin(16, 2);
	lcd.setRGB(colorR, colorG, colorB); // Red, Green, Blue

	Wire.begin(); // needed by the GroveMultiTouch lib
	feelers.initialize(); // initialize the feelers
	for (int i = 0; i <= 3; i++)
	{
		padTouched[i] = false;
	}

	irrecv.enableIRIn(); // Start the receiver
	ChangePaletteAndSettings(); //Load Default Mode Defaults
	//Serial.begin(9600);
}

void loop()
{
	random16_add_entropy( random()); // Add entropy to random number generator; we use a lot of it.
	current_time = millis();

	if (current_time - last_touched_time > 5000UL)
	{
		if (wastouched)
		{
			colorR = 0;
			colorG = 0;
			colorB = 245;
			lcd.setRGB(colorR, colorG, colorB);
			//lcd.clear();
			//lcd.setCursor(0, 0);
			//lcd.print("Run Time");
			//lcd.setCursor(0, 1);
			//lcd.print(current_time / 1000); // millis to seconds
			wastouched = false;
		}
		if (current_time - last_touched_time > 20000UL && (colorR > 0 || colorG > 0 || colorB > 0))
		{
			//int Maxc  = (max (colorR, colorG), colorB);
			colorR = 0;
			colorG = 0;
			colorB = 0;
			lcd.setRGB(colorR, colorG, colorB);
			lcd.noDisplay();
		}
	}

	if (irrecv.decode(&results)) //this checks to see if a code has been received
	{
		if (results.value == 0x490) //if the button V Up
		{
			remoteuse = true;
			remote = 0;
		}
		if (results.value == 0xC90) //if the button V Down
		{
			remoteuse = true;
			remote = 1;
		}
		if (results.value == 0x90) //if the button C Up
		{
			remoteuse = true;
			remote = 2;
		}
		if (results.value == 0x890) //if the button C Down
		{
			remoteuse = true;
			remote = 3;
		}
		if (results.value == 0x290 && current_time - last_touched_time > 200) //if the button Mute
		{
			last_touched_time = current_time;
			light = !light;
			lamp();
		}
		if (results.value == 0x54ED && current_time - last_touched_time > 200) //if the button Sat
		{
			//
		}
		if (results.value == 0xA90 && current_time - last_touched_time > 200) //if the button TV
		{
			//
		}
		irrecv.resume(); //receive the next value
	}

	feelers.readTouchInputs(); // test read the touch sensors
	for (int i = 0; i <= 3; i++) // Number of feelers (-1)
	{
		if (feelers.getTouchState(i) || remoteuse)
		{
			if (remoteuse)
			{
				i = remote;
				remoteuse = false;
			}
			if (!padTouched[i])
			{
				//Serial.print(i);
				//Serial.println(" Touched");
				touched_time = current_time;
			}
			if (padTouched[i])
			{
				//Serial.print(i);
				//Serial.println(" Held");
				counter = (current_time / 1000) - (touched_time / 1000);
			}
			padTouched[i] = true;
			touched(i);
		}
		else
		{
			if (padTouched[i])
			{
				//Serial.print(i);
				//Serial.println(" Released");
				counter = 1;
			}
			if (current_time - last_touched_time > 400)
			{
				padTouched[i] = false;
			}
		}
	}

	if (!light)
	{
		switch (SETTING[0])
		{
			case 0:
			FireWorks();
			break;

			case 1:
			Fire2012WithPalette();
			break;

			case 2:
			// generate noise data
			fillnoise8();
			// convert the noise data to colors in the LED array
			// using the current palette
			mapNoiseToLEDsUsingPalette();
			break;

			case 3:
			blur();
			break;
		}
	}
	LEDS.setBrightness(SETTING[10]);
}

void lamp()
{
	int lighthow = random8(0, 6);

	if (lighthow == 6)
	{
		if (light)
		{
			for (int x = 0; x < 255; x++)
			{
				for ( int i = 0; i < NUM_LEDS; i++)
				{
					leds[i] = CHSV(0, 0, x);
				}
				FastLED.delay(1000 / 60); //Speed
			}
		}
		else if (!light)
		{
			for (int x = 255; x > 0; x--)
			{
				for ( int i = 0; i < NUM_LEDS; i++)
				{
					leds[i] = CHSV(0, 0, x);
				}
				FastLED.delay(1000 / 60); //Speed
			}
		}
	}

	if (lighthow == 5)
	{
		for (int x = ledMatrixWidth; x > 0; x--)
		{
			for (int y = ledMatrixHeight; y > 0; y--)
			{
				if (light)
				{
					XYs(x, y) = CRGB::White;
				}
				else
				{
					XYs(x, y) = CRGB::Black;
				}
				FastLED.delay(1000 / 30); //Speed
			}
		}
	}

	if (lighthow == 4)
	{
		for (int x = 0; x < ledMatrixWidth; x++)
		{
			for (int y = 0; y < ledMatrixHeight; y++)
			{
				if (light)
				{
					XYs(x, y) = CRGB::White;
				}
				else
				{
					XYs(x, y) = CRGB::Black;
				}
				FastLED.delay(1000 / 30); //Speed
			}
		}
	}

	if (lighthow == 3)
	{
		for (int y = ledMatrixHeight; y > 0; y--)
		{
			for (int x = ledMatrixWidth; x > 0; x--)
			{
				if (light)
				{
					XYs(x, y) = CRGB::White;
				}
				else
				{
					XYs(x, y) = CRGB::Black;
				}
				FastLED.delay(1000 / 30); //Speed
			}
		}
	}

	if (lighthow == 2)
	{
		for (int y = 0; y < ledMatrixHeight; y++)
		{
			for (int x = 0; x < ledMatrixWidth; x++)
			{
				if (light)
				{
					XYs(x, y) = CRGB::White;
				}
				else
				{
					XYs(x, y) = CRGB::Black;
				}
				FastLED.delay(1000 / 30); //Speed
			}
		}
	}

	if (lighthow == 1)
	{
		for ( int i = NUM_LEDS; i > 0; i--)
		{
			if (light)
			{
				leds[i] = CRGB::White;
			}
			else
			{
				leds[i] = CRGB::Black;
			}
			FastLED.delay(1000 / 30); //Speed
		}
	}

	if (lighthow == 0)
	{
		for ( int i = 0; i < NUM_LEDS; i++)
		{
			if (light)
			{
				leds[i] = CRGB::White;
			}
			else
			{
				leds[i] = CRGB::Black;
			}
			FastLED.delay(1000 / 30); //Speed
		}
	}
}

void touched(int pad)
{
	if (current_time - last_touched_time > 200 || !MENUMODE_OK)
	{
		last_touched_time = current_time;
		if (colorR == 0 && colorG == 0 && colorB == 0 && !wastouched)
		{
			colorR = 0;
			colorG = 0;
			colorB = 128;
			lcd.setRGB(colorR, colorG, colorB); // Red, Green, Blue
			lcd.display();
			return;
		}
		wastouched = true;
		if (pad == 0)
		{
			MENUMODE ++;
			if (MENUMODE > MAXMENUMODE)
			{
				MENUMODE = 0;
			}
			MENUMODE_DO = true;
		}
		if (pad == 1)
		{
			MENUMODE --;
			if (MENUMODE < 0)
			{
				MENUMODE = MAXMENUMODE;
			}
			MENUMODE_DO = false;
		}

		if (pad == 2)
		{
			SETTING[MENUMODE] += counter;
			if (SETTING[MENUMODE] > SETMAX[MENUMODE])
			{
				SETTING[MENUMODE] = SETMIN[MENUMODE];
			}

			if (MENUMODE == 0)
			{
				ChangePaletteAndSettings();
			}
		}

		if (pad == 3)
		{
			SETTING[MENUMODE] -= counter;
			if (SETTING[MENUMODE] < SETMIN[MENUMODE])
			{
				SETTING[MENUMODE] = SETMAX[MENUMODE];

			}
			if (MENUMODE == 0)
			{
				ChangePaletteAndSettings();
			}
		}
		//Timed
		lcdmenu();
	}
	//Always
}

void lcdmenu()
{
	MENUMODE_OK = false;
	lcd.clear();
	for (int i = 0; i < MAXMENUMODE; i++)
	{
		if (MODESET[SETTING[0]][i] == MENUMODE) //Setting 0 is the mode
		{
			MENUMODE_OK = true;
			colorR = 0;
			colorG = 245;
			colorB = 0;
			lcd.setRGB(colorR, colorG, colorB);
		}
	}
	if (MENUMODE_OK)
	{
		lcd.setCursor(0, 0);
		lcd.print(SETID[MENUMODE]);

		lcd.setCursor(0, 1);
		if (MENUMODE == 0)
		{
			lcd.print(MODEID[SETTING[0]]);
		}

		else if (MENUMODE == 2)
		{
			switch (SETTING[2])
			{
				case 0:
				COLPALNAME = "Heat Colors PAL";
				gPal = HeatColors_p;
				break;

				case 1:
				COLPALNAME = "Lava Colors PAL";
				gPal = LavaColors_p;
				break;

				case 2:
				COLPALNAME = "Random";
				SetupRandomPalette();
				break;

				case 3:
				COLPALNAME = "Purple-N-Green";
				SetupPurpleAndGreenPalette();
				break;

				case 4:
				COLPALNAME = "Black -n- White";
				SetupBlackAndWhiteStripedPalette();
				break;

				case 5:
				COLPALNAME = "PartyColors PAL";
				gPal = PartyColors_p;
				break;

				case 6:
				COLPALNAME = "RainbowColor PAL";
				gPal = RainbowColors_p;
				break;

				case 7:
				COLPALNAME = "RainbowStrip PAL";
				gPal = RainbowStripeColors_p;
				break;

				case 8:
				COLPALNAME = "CloudColors PAL";
				gPal = CloudColors_p;
				break;

				case 9:
				COLPALNAME = "OceanColors PAL";
				gPal = OceanColors_p;
				break;

				case 10:
				COLPALNAME = "ForestColors PAL";
				gPal = ForestColors_p;
				break;
				/*
				case 1:
				COLPALNAME = "Multi Random    ";
				// @ 'if (COLPAL==1)' in void Fire2012WithPalette
				hue0 += random8(2);
				hue1 += random8(2);
				hue2 += random8(2);
				CRGB darkcolor  = CHSV(hue2, 255, 191); // (Hue, Sat, Bri)
				CRGB midcolor   = CHSV(hue1, 250, 223);
				CRGB lightcolor = CHSV(hue0, 245, 255);
				gPal = CRGBPalette16( CRGB::Black, darkcolor, midcolor, lightcolor);
				break;
				*/
			}
			lcd.print(COLPALNAME);
		}
		else // Just show the value for the current menu mode
		{
			lcd.print(SETTING[MENUMODE]);
		}
	}
	else // Menu Being skipped
	{
		colorR = 245;
		colorG = 0;
		colorB = 0;
		lcd.setRGB(colorR, colorG, colorB); //Setting does not aplly to this mode
		if (MENUMODE_DO == true) //Going up or Down
		{
			touched(0);
		}
		else
		{
			touched(1);
		}
	}
}

void fillnoise8()
{
	// If we're running at a low "speed", some 8-bit artifacts become visible
	// from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
	// The amount of data smoothing we're doing depends on "speed".
	uint8_t dataSmoothing = 0;
	if ( SETTING[1] < 50)
	{
		dataSmoothing = 200 - (SETTING[1] * 4);
	}

	for (int i = 0; i < MAX_DIMENSION; i++)
	{
		int ioffset = SETTING[5] * i;
		for (int j = 0; j < MAX_DIMENSION; j++)
		{
			int joffset = SETTING[5] * j;
			uint8_t data = inoise8(x + ioffset, y + joffset, z);

			// The range of the inoise8 function is roughly 16-238.
			// These two operations expand those values out to roughly 0..255
			// You can comment them out if you want the raw noise data.
			data = qsub8(data, 16);
			data = qadd8(data, scale8(data, 39));

			if ( dataSmoothing )
			{
				uint8_t olddata = moisemap[i][j];
				uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
				data = newdata;
			}
			moisemap[i][j] = data;
		}
	}

	z += SETTING[1];

	// apply slow drift to X and Y, just for visual variation.
	x += SETTING[1] / 2;
	y -= SETTING[1] / 4;
}

void mapNoiseToLEDsUsingPalette()
{
	static uint8_t ihue = 0;

	for (int i = 0; i < ledMatrixWidth; i++)
	{
		for (int j = 0; j < ledMatrixHeight; j++)
		{
			// We use the value at the (i,j) coordinate in the noise
			// array for our brightness, and the flipped value from (j,i)
			// for our pixel's index into the color palette.
			uint8_t bri = moisemap[i][j];
			uint8_t index = moisemap[j][i];

			// if this palette is a 'loop', add a slowly-changing base value
			if ( SETTING[6])
			{
				index += ihue;
			}

			// brighten up, as the color palette itself often contains the
			// light/dark dynamic range desired
			if ( bri > 255 )
			{
				bri = 255;
			}
			else
			{
				bri = dim8_raw( bri * 2);
			}
			XYs(i, j) = ColorFromPalette( gPal, index, bri);
		}
	}
	ihue += 1;
	LEDS.show();
}

void blur() // Need to randomize movement and work on wrap
{
	fadeToBlackBy(leds, NUM_LEDS, 1);
	//blur2d(leds, ledMatrixWidth, ledMatrixHeight, 172); // Fade

	uint8_t  j = beatsin8(SETTING[7], 0, ledMatrixWidth - 1); //Speed, Min, Max
	uint8_t  i = beatsin8(SETTING[8], 0, ledMatrixHeight - 1);

	XYs(j, i) += CHSV(current_time / 700, 255, 255);
	FastLED.delay(1000 / SETTING[1]); //Speed
}

void FireWorks()
{
	fadeToBlackBy(leds, NUM_LEDS, 64);
	/*
	CRGB star(1, 0, 2);
	#if 1
	for( byte u = 0; u < 1; u++)
	{
	leds[random8(NUM_LEDS)] = star;
	}
	#endif
	*/
	for (byte d = 0; d < NUM_DOTS; d++)
	{
		gDot[d].Move();
		gDot[d].Draw();
		for (byte b = 0; b < NUM_SPARKS; b++)
		{
			gSparks[d][b].Move();
			gSparks[d][b].Draw();
		}
		if ( gSkyburst)
		{
			byte nsparks = random8( NUM_SPARKS / 2, NUM_SPARKS + 1);
			for ( byte b = 0; b < nsparks; b++)
			{
				gSparks[d][b].Skyburst( gBurstx, gBursty, gBurstyv, gBurstcolor);
				gSkyburst = 0;
			}
		}
	}

	static uint16_t launchcountdown = 0;
	static uint16_t burstcountdown = 0;
	if ( gDot[0].show == 0 )
	{
		if ( launchcountdown == 0)
		{
			if ( random8() < 64 )
			{

				for (byte i = 0; i < NUM_DOTS; i++)
				{
					if ( gDot[i].show == 0 )
					{
						if ( burstcountdown == 0)
						{
							gDot[i].GroundLaunch();
							burstcountdown = random16(100, 1000);
						}
						else
						{
							burstcountdown --;

						}
					}
				}
			}
			else
			{
				gDot[0].GroundLaunch();
			}
			launchcountdown = random16(50, SETTING[9]); // Countdown
		}
		else
		{
			launchcountdown --;
		}
	}
	FastLED.delay(1000 / SETTING[1]); //Speed
}

void Fire2012WithPalette()
{
	//static float hue0 = 0, hue1 = 1, hue2 = 2;
	for (int x = 0; x < ledMatrixWidth; x++)
	{
		// Step 1.  Cool down every cell a little
		for ( int i = 0; i < ledMatrixHeight; i++)
		{
			ledmap[x][i] = qsub8( ledmap[x][i],  random8(0, ((SETTING[3] * 10) / ledMatrixHeight) + 2)); //Cooling
		}
		// Step 2.  Heat from each cell drifts 'up' and diffuses a little
		for ( int k = ledMatrixHeight - 1; k >= 2; k--)
		{
			ledmap[x][k] = (ledmap[x][k - 1] + ledmap[x][k - 2] + ledmap[x][k - 2] ) / 3;
		}
		// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
		if ( random8() < SETTING[4] ) //Sparking
		{

			int j = random8(1);
			ledmap[x][j] = qadd8( ledmap[x][j], random8(32, 255) );
		}
		// Step 4.  Map from heat cells to LED colors
		for ( int y = 0; y < ledMatrixHeight; y++)
		{
			// Scale the heat value from 0-255 down to 0-240
			// for best results with color palettes.
			byte colorindex = scale8( ledmap[x][y], 240);
			XYs(x, y) = ColorFromPalette( gPal, colorindex);
		}
	}
	FastLED.delay(1000 / SETTING[1]); //Speed
}

void ChangePaletteAndSettings() //Mode Defaults
{
	switch (SETTING[0])
	{
		case 0:
		SETTING[1] = 1000; //Speed
		SETMIN[1] = 100;
		//FLASH_CHANCE = 64; // not imp
		//BURST_CHANCE = 64; // not imp
		SETTING[9] = 1000; // Random Launch time max
		break;

		case 1:
		SETTING[2] = 0;
		gPal = HeatColors_p;
		SETTING[1] = 60;
		SETTING[3] = 60; //Cooling
		SETTING[4] = 30; //Sparking
		break;

		case 2:
		SETTING[2] = 4;
		SetupBlackAndWhiteStripedPalette();
		SETTING[5] = 50; //Zoom
		SETTING[6] = 0;  //Loop
		SETTING[1] = 1;
		SETMIN[1] = 1;
		break;

		case 3:
		SETTING[1] = 1000;
		SETMIN[1] = 1;
		SETTING[7] = 7; //Around Speed
		SETTING[8] = 3; //Up-Down Speed
		break;
	}
}

void SetupRandomPalette()
{
	// This function generates a random palette that's a gradient
	// between four different colors.(Hue, Sat, Bri),
	gPal = CRGBPalette16(
	CHSV( random8(), 255, 64),
	CHSV( random8(), 255, 96),
	CHSV( random8(), 255, 192),
	CHSV( random8(), 255, 255));
}

void SetupBlackAndWhiteStripedPalette()
{
	// This function sets up a palette of black and white stripes,
	// using code. The various fill_* functions can be used.

	// 'black out' all 16 palette entries...
	fill_solid( gPal, 16, CRGB::Black);
	// and set # to white.
	gPal[0] = CRGB::White;
	//gPal[1] = CRGB::White;
	//gPal[2] = CRGB::White;
	//gPal[3] = CRGB::White;
	gPal[4] = CRGB::White;
	//gPal[5] = CRGB::White;
	//gPal[6] = CRGB::White;
	gPal[7] = CRGB::White;
	gPal[8] = CRGB::White;
	//gPal[9] = CRGB::White;
	//gPal[10] = CRGB::White;
	gPal[11] = CRGB::White;
	//gPal[12] = CRGB::White;
	//gPal[13] = CRGB::White;
	//gPal[14] = CRGB::White;
	gPal[15] = CRGB::White;
}

void SetupPurpleAndGreenPalette()
{
	// This function sets up a palette of purple and green stripes.
	CRGB purple = CHSV( HUE_PURPLE, 255, 255);
	CRGB green  = CHSV( HUE_GREEN, 255, 255);
	CRGB black  = CRGB::Black;

	gPal = CRGBPalette16(
	black,  green,  black,  black,
	black, purple, black,  black,
	black,  green,  black,  black,
	black, purple, black,  black );
}
