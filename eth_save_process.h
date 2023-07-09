#pragma once

#include <stdint.h>

typedef struct EthernetSaveConfig {
    uint8_t mac[6];
    uint8_t ip[4];
    uint8_t mask[4];
    uint8_t gateway[4];
    uint8_t dns[4];
    uint8_t ping_ip[4];
} EthernetSaveConfig;

#define ETHERNET_SAVE_DEFAULT_MAC \
    { 0x10, 0x08, 0xDC, 0x47, 0x47, 0x54 }
#define ETHERNET_SAVE_DEFAULT_IP \
    { 192, 168, 0, 101 }
#define ETHERNET_SAVE_DEFAULT_MASK \
    { 255, 255, 255, 0 }
#define ETHERNET_SAVE_DEFAULT_GATEWAY \
    { 192, 168, 0, 1 }
#define ETHERNET_SAVE_DEFAULT_DNS \
    { 192, 168, 0, 1 }
#define ETHERNET_SAVE_DEFAULT_PING_IP \
    { 8, 8, 8, 8 }

void ethernet_save_process_write(const EthernetSaveConfig* config);
void ethernet_save_process_read(EthernetSaveConfig* config);

void ehternet_save_process_print(char* str);
