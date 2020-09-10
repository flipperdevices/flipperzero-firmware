#include "unique_id.h"

uint32_t get_flipper_unique_id(){
  return DBGMCU->IDCODE;
}