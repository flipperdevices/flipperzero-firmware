#pragma once
#include "main.h"
/*
  this file must be touched every compile
  for example, cmake: file(TOUCH_NOCREATE "Src/app/libs/version.h")
*/

uint32_t get_flipper_unique_id(){
  return DBGMCU->IDCODE;
}