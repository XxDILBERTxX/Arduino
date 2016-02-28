/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino/Genuino Uno, Platform=avr, Package=arduino
*/

#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
#define ARDUINO 167
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define F_CPU 16000000L
#define ARDUINO 167
#define ARDUINO_AVR_UNO
#define ARDUINO_ARCH_AVR
extern "C" void __cxa_pure_virtual() {;}

void getFuseBytes ();
void writeBootloader ();
void getSignature ();
//
//
byte readFlash (unsigned long addr);
void writeFlash (unsigned long addr, const byte data);
byte readFuse (const byte which);
void readSignature (byte sig [3]);
void pollUntilReady ();
void commitPage (unsigned long addr, bool showMessage);
void eraseMemory ();
void writeFuse (const byte newValue, const byte whichFuse);
bool startProgramming ();
void stopProgramming ();
void initPins ();
byte HVbit (const byte sii, const byte sdi);
byte HVtransfer (byte instruction, byte data);
byte readFlash (unsigned long addr);
void writeFlash (unsigned long addr, const byte data);
byte readFuse (const byte which);
void readSignature (byte sig [3]);
void pollUntilReady ();
void commitPage (unsigned long addr, bool showMessage);
void eraseMemory ();
void writeFuse (const byte newValue, const byte whichFuse);
bool startProgramming ();
void stopProgramming ();
void initPins ();
byte program (const byte b1, const byte b2, const byte b3, const byte b4);
byte readFlash (unsigned long addr);
void writeFlash (unsigned long addr, const byte data);
byte readFuse (const byte which);
void readSignature (byte sig [3]);
void pollUntilReady ();
void commitPage (unsigned long addr, bool showMessage);
void eraseMemory ();
void writeFuse (const byte newValue, const byte whichFuse);
bool startProgramming ();
void stopProgramming ();
void initPins ();
void showProgress ();
void clearPage ();
void writeData (const unsigned long addr, const byte * pData, const int length);
void showHex (const byte b, const boolean newline, const boolean show0x);
void showYesNo (const boolean b, const boolean newline);

#include "C:\Users\XxDILBERTxX\AppData\Local\arduino15\packages\arduino\hardware\avr\1.6.9\variants\standard\pins_arduino.h" 
#include "C:\Users\XxDILBERTxX\AppData\Local\arduino15\packages\arduino\hardware\avr\1.6.9\cores\arduino\arduino.h"
#include <..\Atmega_Board_Programmer\Atmega_Board_Programmer.ino>
#include <..\Atmega_Board_Programmer\General_Stuff.h>
#include <..\Atmega_Board_Programmer\HV_Parallel_Utils.ino>
#include <..\Atmega_Board_Programmer\HV_Pins.h>
#include <..\Atmega_Board_Programmer\HV_Serial_Utils.ino>
#include <..\Atmega_Board_Programmer\ICSP_Utils.ino>
#include <..\Atmega_Board_Programmer\Programming_Utils.ino>
#include <..\Atmega_Board_Programmer\Signatures.h>
#include <..\Atmega_Board_Programmer\bootloader_atmega1280.h>
#include <..\Atmega_Board_Programmer\bootloader_atmega1284.h>
#include <..\Atmega_Board_Programmer\bootloader_atmega168.h>
#include <..\Atmega_Board_Programmer\bootloader_atmega16u2.h>
#include <..\Atmega_Board_Programmer\bootloader_atmega2560_v2.h>
#include <..\Atmega_Board_Programmer\bootloader_atmega256rfr2_v1.0a.h>
#include <..\Atmega_Board_Programmer\bootloader_atmega328.h>
#include <..\Atmega_Board_Programmer\bootloader_atmega32u4.h>
#include <..\Atmega_Board_Programmer\bootloader_atmega8.h>
#include <..\Atmega_Board_Programmer\bootloader_lilypad328.h>
