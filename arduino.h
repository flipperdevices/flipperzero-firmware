/*
Abstraction layer to convert arduino functions into
their flipper zero alternatives. We need this because
the dmcomm code was written for arduino and flipper
doesn't define these methods.

Kinda janky but works well enough for this...
*/

#ifndef HAL_HEADERS
#define HAL_HEADERS

#include "flipper.h"
#include "app_state.h"

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#define HIGH true
#define LOW false
#define INPUT 1
#define OUTPUT 0

typedef uint8_t byte;
typedef bool boolean;

typedef void (*DmcommCallback)(void*);

EXTERNC const char *F(const char* i);
EXTERNC void delay(int ms);
EXTERNC void delayMicroseconds(int us);
EXTERNC uint32_t micros();
EXTERNC uint32_t millis();

#undef EXTERNC
#endif