#ifndef _token_h
#define _token_h

#include <inttypes.h>
#include <sys/time.h>

void totp_setup(uint32_t timeStep);
void totp_set_timezone(float timezone);
uint32_t totp_get_code_from_timestamp(uint8_t* hmacKey, uint8_t keyLength, uint32_t timeStamp);
uint32_t totp_get_code_from_time_struct(uint8_t* hmacKey, uint8_t keyLength, struct tm time);
#endif
