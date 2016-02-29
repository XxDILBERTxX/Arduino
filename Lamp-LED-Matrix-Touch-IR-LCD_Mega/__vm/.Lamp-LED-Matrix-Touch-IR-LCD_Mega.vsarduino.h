/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino/Genuino Mega w/ ATmega2560 (Mega 2560), Platform=avr, Package=arduino
*/

#define __AVR_ATmega2560__
#define ARDUINO 167
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define F_CPU 16000000L
#define ARDUINO 167
#define ARDUINO_AVR_MEGA2560
#define ARDUINO_ARCH_AVR
extern "C" void __cxa_pure_virtual() {;}

void screenscale( accum88 a, byte N, byte& screen, byte& screenerr);
int16_t scale15by8_local( int16_t i, fract8 scale );
//
//
void lamp();
void touched(int pad);
void lcdmenu();
void fillnoise8();
void mapNoiseToLEDsUsingPalette();
void blur();
void FireWorks();
void Fire2012WithPalette();
void ChangePaletteAndSettings();
void SetupRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void SetupPurpleAndGreenPalette();

#include "C:\Users\XxDILBERTxX\AppData\Local\arduino15\packages\arduino\hardware\avr\1.6.9\variants\mega\pins_arduino.h" 
#include "C:\Users\XxDILBERTxX\AppData\Local\arduino15\packages\arduino\hardware\avr\1.6.9\cores\arduino\arduino.h"
#include <..\Lamp-LED-Matrix-Touch-IR-LCD_Mega\Lamp-LED-Matrix-Touch-IR-LCD_Mega.ino>
