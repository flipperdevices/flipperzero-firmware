#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <furi_hal.h>
#include "uhf_tag.h"
#include "uhf_buffer.h"
#include "uhf_tag.h"
#include <furi_hal.h>
#include "uhf_module_settings.h"

#define FRAME_START 0xBB
#define FRAME_END 0x7E
#define DEFAULT_BAUDRATE BAUD_RATES[BAUD_RATES_COUNT - 1]
#define DEFAULT_TRANSMITTING_POWER POWER_DBM[POWER_DBM_COUNT - 1]
#define DEFAULT_WORKING_REGION WR_US

typedef struct {
    char* hw_version;
    char* sw_version;
    char* manufacturer;
} M100ModuleInfo;

typedef enum {
    M100SuccessResponse,
    M100ValidationFail,
    M100NoTagResponse,
    M100MemoryOverrun,
    M100EmptyResponse,
    M100ChecksumFail
} M100ResponseType;

typedef struct {
    M100ModuleInfo* info;
    WorkingRegion region;
    uint32_t baudrate;
    uint16_t region_frequency;
    uint16_t transmitting_power;
    uint16_t max_transmitting_power;
    bool freq_hopping;
    Buffer* buf;
    FuriHalSerialHandle* serial_handle;
} M100Module;

M100ModuleInfo* m100_module_info_alloc();
void m100_module_info_free(M100ModuleInfo* module_info);

M100Module* m100_module_alloc();
void m100_module_free(M100Module* module);
uint16_t crc16_genibus(const uint8_t* data, size_t length);
uint8_t checksum(const uint8_t* data, size_t length);
uint8_t get_baudrate_count();

// Function prototypes
char* m100_get_hardware_version(M100Module* module);
char* m100_get_software_version(M100Module* module);
char* m100_get_manufacturers(M100Module* module);

void m100_set_baudrate(M100Module* module, uint32_t baudrate);
bool m100_set_working_region(M100Module* module, WorkingRegion region);
bool m100_set_transmitting_power(M100Module* module, uint16_t power);
bool m100_set_freq_hopping(M100Module* module, bool hopping);
bool m100_set_power(M100Module* module, uint8_t* power);

// gen2 cmds
M100ResponseType m100_single_poll(M100Module* module, UHFTag* uhf_tag);
M100ResponseType m100_set_select(M100Module* module, UHFTag* uhf_tag);
M100ResponseType m100_read_label_data_storage(
    M100Module* module,
    UHFTag* uhf_tag,
    BankType bank,
    uint32_t access_pwd,
    uint16_t word_count);

M100ResponseType m100_write_label_data_storage(
    M100Module* module,
    UHFTag* saved_tag,
    UHFTag* selected_tag,
    BankType bank,
    uint16_t source_address,
    uint32_t access_pwd);

uint32_t m100_get_baudrate(M100Module* module);
