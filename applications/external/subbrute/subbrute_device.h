#pragma once

#include "subbrute_protocols.h"
#include <lib/subghz/protocols/base.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/environment.h>
#include "helpers/radio_device_loader.h"

#define SUBBRUTE_TEXT_STORE_SIZE 256

#define SUBBRUTE_MAX_LEN_NAME 64
#define SUBBRUTE_PATH EXT_PATH("subghz")
#define SUBBRUTE_FILE_EXT ".sub"

#define SUBBRUTE_PAYLOAD_SIZE 16

typedef enum {
    SubBruteFileResultUnknown,
    SubBruteFileResultOk,
    SubBruteFileResultErrorOpenFile,
    SubBruteFileResultMissingOrIncorrectHeader,
    SubBruteFileResultFrequencyNotAllowed,
    SubBruteFileResultMissingOrIncorrectFrequency,
    SubBruteFileResultPresetInvalid,
    SubBruteFileResultMissingProtocol,
    SubBruteFileResultProtocolNotSupported,
    SubBruteFileResultDynamicProtocolNotValid,
    SubBruteFileResultProtocolNotFound,
    SubBruteFileResultMissingOrIncorrectBit,
    SubBruteFileResultMissingOrIncorrectKey,
    SubBruteFileResultMissingOrIncorrectTe,
} SubBruteFileResult;

typedef struct {
    const SubBruteProtocol* protocol_info;
    SubBruteProtocol* file_protocol_info;

    // Current step
    uint64_t current_step;

    // SubGhz
    SubGhzReceiver* receiver;
    SubGhzProtocolDecoderBase* decoder_result;
    SubGhzEnvironment* environment;
    const SubGhzDevice* radio_device;

    // Attack state
    SubBruteAttacks attack;
    uint64_t max_value;
    uint8_t extra_repeats;

    // Loaded info for attack type
    uint64_t key_from_file;
    uint64_t current_key_from_file;
    bool two_bytes;
    // Index of group to bruteforce in loaded file
    uint8_t bit_index;
} SubBruteDevice;

SubBruteDevice* subbrute_device_alloc(const SubGhzDevice* radio_device;);
void subbrute_device_free(SubBruteDevice* instance);

bool subbrute_device_save_file(SubBruteDevice* instance, const char* key_name);
const char* subbrute_device_error_get_desc(SubBruteFileResult error_id);
SubBruteFileResult subbrute_device_attack_set(
    SubBruteDevice* context,
    SubBruteAttacks type,
    uint8_t extra_repeats);
uint8_t subbrute_device_load_from_file(SubBruteDevice* context, const char* file_path);

uint64_t subbrute_device_add_step(SubBruteDevice* instance, int8_t step);

void subbrute_device_free_protocol_info(SubBruteDevice* instance);
void subbrute_device_attack_set_default_values(
    SubBruteDevice* context,
    SubBruteAttacks default_attack);