#pragma once
/*
  this file must be touched every compile
  for example, cmake: file(TOUCH_NOCREATE "Src/app/libs/version.h")
*/

#define VERSION_DATE __DATE__
#define VERSION_TIME __TIME__