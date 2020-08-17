#pragma once

#include "cmsis_os.h"

typedef void(*FlipperApplication)(void*);

// Furiac application handler
typedef struct {
	TaskHandle_t handler;
} FuriApp;

/*
simply starts application.
It call app entrypoint with param passed as argument.
Useful for daemon applications and pop-up.
*/

FuriApp furiac_start(FlipperApplication app, const char* name, void* param);
