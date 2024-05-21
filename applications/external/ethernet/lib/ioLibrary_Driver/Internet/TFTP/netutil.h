
#ifndef __NETUTIL_H__
#define __NETUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SYSTEM_LITTLE_ENDIAN

int8_t* inet_ntoa(uint32_t addr);
int8_t* inet_ntoa_pad(uint32_t addr);
uint32_t inet_addr(uint8_t* addr);
uint16_t swaps(uint16_t i);
uint32_t swapl(uint32_t l);
uint16_t htons(uint16_t hostshort);
uint32_t htonl(uint32_t hostlong);
uint32_t ntohs(uint16_t netshort);
uint32_t ntohl(uint32_t netlong);

#ifdef __cplusplus
}
#endif

#endif
