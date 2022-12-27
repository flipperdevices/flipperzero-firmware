#pragma once

#include <furi.h>
#include <furi_hal_subghz.h>
#include <core/string.h>
#include <toolbox/stream/stream.h>

typedef enum {
    CAMEFileProtocol,
    NICEFileProtocol,
    ChamberlainFileProtocol,
    LinearFileProtocol,
    PrincetonFileProtocol,
    RAWFileProtocol,
    BETTFileProtocol,
    ClemsaFileProtocol,
    DoitrandFileProtocol,
    GateTXFileProtocol,
    MagellanFileProtocol,
    IntertechnoV3FileProtocol,
    AnsonicFileProtocol,
    SMC5326FileProtocol,
    UNILARMFileProtocol,
    PT2260FileProtocol,
    HoneywellFileProtocol,
    HoltekFileProtocol,
    UnknownFileProtocol,
    TotalFileProtocol,
} SubBruteFileProtocol;

typedef enum {
    SubBruteAttackCAME12bit303,
    SubBruteAttackCAME12bit307,
    SubBruteAttackCAME12bit315,
    SubBruteAttackCAME12bit433,
    SubBruteAttackCAME12bit868,
    SubBruteAttackNICE12bit433,
    SubBruteAttackNICE12bit868,
    SubBruteAttackAnsonic12bit433075,
    SubBruteAttackAnsonic12bit433,
    SubBruteAttackAnsonic12bit434,
    SubBruteAttackHoltek12bit433,
    SubBruteAttackChamberlain9bit300,
    SubBruteAttackChamberlain9bit315,
    SubBruteAttackChamberlain9bit390,
    SubBruteAttackChamberlain9bit433,
    SubBruteAttackChamberlain8bit300,
    SubBruteAttackChamberlain8bit315,
    SubBruteAttackChamberlain8bit390,
    SubBruteAttackChamberlain7bit300,
    SubBruteAttackChamberlain7bit315,
    SubBruteAttackChamberlain7bit390,
    SubBruteAttackLinear10bit300,
    SubBruteAttackLinear10bit310,
    SubBruteAttackUNILARM24bit330,
    SubBruteAttackUNILARM24bit433,
    SubBruteAttackSMC532624bit330,
    SubBruteAttackSMC532624bit433,
    SubBruteAttackPT226024bit315,
    SubBruteAttackPT226024bit330,
    SubBruteAttackPT226024bit390,
    SubBruteAttackPT226024bit433,
    SubBruteAttackLoadFile,
    SubBruteAttackTotalCount,
} SubBruteAttacks;

typedef struct {
    uint32_t frequency;
    uint8_t bits;
    uint32_t te;
    uint8_t repeat;
    FuriHalSubGhzPreset preset;
    SubBruteFileProtocol file;
} SubBruteProtocol;

const SubBruteProtocol* subbrute_protocol(SubBruteAttacks index);
const char* subbrute_protocol_preset(FuriHalSubGhzPreset preset);
const char* subbrute_protocol_file(SubBruteFileProtocol protocol);
FuriHalSubGhzPreset subbrute_protocol_convert_preset(FuriString* preset_name);
SubBruteFileProtocol subbrute_protocol_file_protocol_name(FuriString* name);
uint8_t subbrute_protocol_repeats_count(SubBruteAttacks index);
const char* subbrute_protocol_name(SubBruteAttacks index);

void subbrute_protocol_default_payload(
    Stream* stream,
    SubBruteFileProtocol file,
    uint64_t step,
    uint8_t bits,
    uint32_t te,
    uint8_t repeat);
void subbrute_protocol_file_payload(
    Stream* stream,
    uint64_t step,
    uint8_t bits,
    uint32_t te,
    uint8_t repeat,
    uint8_t bit_index,
    uint64_t file_key,
    bool two_bytes);
void subbrute_protocol_default_generate_file(
    Stream* stream,
    uint32_t frequency,
    FuriHalSubGhzPreset preset,
    SubBruteFileProtocol file,
    uint64_t step,
    uint8_t bits,
    uint32_t te,
    uint8_t repeat);
void subbrute_protocol_file_generate_file(
    Stream* stream,
    uint32_t frequency,
    FuriHalSubGhzPreset preset,
    SubBruteFileProtocol file,
    uint64_t step,
    uint8_t bits,
    uint32_t te,
    uint8_t repeat,
    uint8_t bit_index,
    uint64_t file_key,
    bool two_bytes);
uint64_t
    subbrute_protocol_calc_max_value(SubBruteAttacks attack_type, uint8_t bits, bool two_bytes);