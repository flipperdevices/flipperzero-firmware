#include <inttypes.h>
#include "time.h"

void TOTP(uint8_t* hmacKey, uint8_t keyLength, uint32_t timeStep);
void setTimezone(uint8_t timezone);
uint32_t getCodeFromTimestamp(uint32_t timeStamp);
uint32_t getCodeFromTimeStruct(struct tm time);
uint32_t getCodeFromSteps(uint32_t steps);
