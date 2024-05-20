#ifndef PING_H
#define PING_H

#include "wizchip_conf.h"

#define BUF_LEN 32
#define PING_REQUEST 8
#define PING_REPLY 0
#define CODE_ZERO 0

#define SOCKET_ERROR 1
#define TIMEOUT_ERROR 2
#define PING_SUCCESS 3
#define REPLY_ERROR 4
#define FUNCTION_ERROR 5
#define PING_DEBUG

typedef struct pingmsg {
    uint8_t Type; // 0 - Ping Reply, 8 - Ping Request
    uint8_t Code; // Always 0
    int16_t CheckSum; // Check sum
    int16_t ID; // Identification
    int16_t SeqNum; // Sequence Number
    int8_t Data[BUF_LEN]; // Ping Data  : 1452 = IP RAW MTU - sizeof(Type+Code+CheckSum+ID+SeqNum)
} PINGMSGR;

uint8_t ping_auto(uint8_t s, uint8_t* addr);
uint8_t ping_count(uint8_t s, uint16_t pCount, uint8_t* addr);
uint8_t ping_request(uint8_t s, uint8_t* addr);
uint8_t ping_reply(uint8_t s, uint8_t* addr, uint16_t rlen);
uint16_t checksum(uint8_t* data_buf, uint16_t len);
uint16_t htons(
    uint16_t
        hostshort); /* htons function converts a unsigned short from host to TCP/IP network byte order (which is big-endian).*/

void eth_printf(const char* format, ...);

#endif /* PING_H */