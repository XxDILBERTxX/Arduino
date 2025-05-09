// ATmega series 0
// excludes Arduino Nano Every which optimised with VPORTs
// include boards eg. Arduino Uno Wifi Rev 2 or Rev 3

#pragma once

#include <avr/io.h>
#include "fl/namespace.h"
#include "avr_pin.h"

#warning "Untested platform. Please report any issues to the https://github.com/fastled/fastled/issues"

FASTLED_NAMESPACE_BEGIN

#if defined(FASTLED_FORCE_SOFTWARE_PINS)
#warning "Software pin support forced, pin access will be slightly slower."
#define NO_HARDWARE_PIN_SUPPORT
#undef HAS_HARDWARE_PIN_SUPPORT

#else

#define AVR_PIN_CYCLES(_PIN) ((((int)FastPin<_PIN>::port()) - 0x20 < 64) ? 1 : 2)

typedef volatile uint8_t& reg8_t;

//todo: replace with _CONCAT3 like avr_nano_every.h
//but why VPORT##L.DIR not VPORT##L##DIR - is it due to _DIR ?
#define _R(T) struct __gen_struct_##T
#define _RD8(T) struct __gen_struct_##T { static inline reg8_t& r() { return T; } };
//#define _CONCAT3(a, b, c) a##b##c

#define _FL_IO(L,C)             \
    _RD8(PORT ## L ## _DIR);    \
    _RD8(PORT ## L ## _OUT);    \
    _RD8(PORT ## L ## _IN);     \
    _FL_DEFINE_PORT3(L, C, _R(PORT ## L ## _OUT));

#define _FL_DEFPIN(_PIN, BIT, L)                        \
    template<> class FastPin<_PIN> : public _AVRPIN<    \
        _PIN,                                           \
        1<<BIT,                                         \
        _R(PORT ## L ## _OUT),                          \
        _R(PORT ## L ## _DIR),                          \
        _R(PORT ## L ## _IN)                            \
    > {};


// Pre-do all the port definitions
#ifdef VPORTA
_FL_IO(A, 0)
#endif
#ifdef VPORTB
_FL_IO(B, 1)
#endif
#ifdef VPORTC
_FL_IO(C, 2)
#endif
#ifdef VPORTD
_FL_IO(D, 3)
#endif
#ifdef VPORTE
_FL_IO(E, 4)
#endif
#ifdef VPORTF
_FL_IO(F, 5)
#endif
#ifdef VPORTG
_FL_IO(G, 6)
#endif
#ifdef VPORTH
_FL_IO(H, 7)
#endif
#ifdef VPORTI
_FL_IO(I, 8)
#endif
#ifdef VPORTJ
_FL_IO(J, 9)
#endif
#ifdef VPORTK
_FL_IO(K, 10)
#endif
#ifdef VPORTL
_FL_IO(L, 11)
#endif
#ifdef VPORTM
_FL_IO(M, 12)
#endif
#ifdef VPORTN
_FL_IO(N, 13)
#endif

#define MAX_PIN 21
_FL_DEFPIN(0, 4, C); _FL_DEFPIN(1, 5, C); _FL_DEFPIN(2, 0, A); _FL_DEFPIN(3, 5, F);
_FL_DEFPIN(4, 6, C); _FL_DEFPIN(5, 2, B); _FL_DEFPIN(6, 4, F); _FL_DEFPIN(7, 1, A);
_FL_DEFPIN(8, 3, E); _FL_DEFPIN(9, 0, B); _FL_DEFPIN(10, 1, B); _FL_DEFPIN(11, 0, E);
_FL_DEFPIN(12, 1, E); _FL_DEFPIN(13, 2, E); _FL_DEFPIN(14, 3, D); _FL_DEFPIN(15, 2, D);
_FL_DEFPIN(16, 1, D); _FL_DEFPIN(17, 0, D); _FL_DEFPIN(18, 2, A); _FL_DEFPIN(19, 3, A);
_FL_DEFPIN(20, 4, D); _FL_DEFPIN(21, 5, D);

#define SPI_DATA 11
#define SPI_CLOCK 13
#define SPI_SELECT 8
#define AVR_HARDWARE_SPI 1
#define HAS_HARDWARE_PIN_SUPPORT 1

//todo: pins has not been confirmed thus not enabled
//#define SPI_UART0_DATA 1
//#define SPI_UART0_CLOCK 4



#endif // FASTLED_FORCE_SOFTWARE_PINS

FASTLED_NAMESPACE_END
