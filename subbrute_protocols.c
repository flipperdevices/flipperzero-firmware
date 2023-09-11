#include "subbrute_protocols.h"
#include "math.h"

#define TAG "SubBruteProtocols"

/**
 * CAME 12bit 303MHz
 */
const SubBruteProtocol subbrute_protocol_came_12bit_303 = {
    .frequency = 303875000,
    .bits = 12,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = CAMEFileProtocol};

/**
 * CAME 12bit 307MHz
 */
const SubBruteProtocol subbrute_protocol_came_12bit_307 = {
    .frequency = 307800000,
    .bits = 12,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = CAMEFileProtocol};

/**
 * CAME 12bit 315MHz
 */
const SubBruteProtocol subbrute_protocol_came_12bit_315 = {
    .frequency = 315000000,
    .bits = 12,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = CAMEFileProtocol};

/**
 * CAME 12bit 433MHz
 */
const SubBruteProtocol subbrute_protocol_came_12bit_433 = {
    .frequency = 433920000,
    .bits = 12,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = CAMEFileProtocol};

/**
 * CAME 12bit 868MHz
 */
const SubBruteProtocol subbrute_protocol_came_12bit_868 = {
    .frequency = 868350000,
    .bits = 12,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = CAMEFileProtocol};

/**
 * NICE 12bit 433MHz
 */
const SubBruteProtocol subbrute_protocol_nice_12bit_433 = {
    .frequency = 433920000,
    .bits = 12,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = NICEFileProtocol};

/**
 * NICE 12bit 868MHz
 */
const SubBruteProtocol subbrute_protocol_nice_12bit_868 = {
    .frequency = 868350000,
    .bits = 12,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = NICEFileProtocol};

/**
 * Ansonic 12bit 433.075MHz
 */
const SubBruteProtocol subbrute_protocol_ansonic_12bit_433075 = {
    .frequency = 433075000,
    .bits = 12,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPreset2FSKDev238Async,
    .file = AnsonicFileProtocol};

/**
 * Ansonic 12bit 433.92MHz
 */
const SubBruteProtocol subbrute_protocol_ansonic_12bit_433 = {
    .frequency = 433920000,
    .bits = 12,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPreset2FSKDev238Async,
    .file = AnsonicFileProtocol};

/**
 * Ansonic 12bit 434.075MHz
 */
const SubBruteProtocol subbrute_protocol_ansonic_12bit_434 = {
    .frequency = 434075000,
    .bits = 12,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPreset2FSKDev238Async,
    .file = AnsonicFileProtocol};

/**
 * Chamberlain 9bit 300MHz
 */
const SubBruteProtocol subbrute_protocol_chamberlain_9bit_300 = {
    .frequency = 300000000,
    .bits = 9,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = ChamberlainFileProtocol};

/**
 * Chamberlain 9bit 315MHz
 */
const SubBruteProtocol subbrute_protocol_chamberlain_9bit_315 = {
    .frequency = 315000000,
    .bits = 9,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = ChamberlainFileProtocol};

/**
 * Chamberlain 9bit 390MHz
 */
const SubBruteProtocol subbrute_protocol_chamberlain_9bit_390 = {
    .frequency = 390000000,
    .bits = 9,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = ChamberlainFileProtocol};

/**
 * Chamberlain 9bit 433MHz
 */
const SubBruteProtocol subbrute_protocol_chamberlain_9bit_433 = {
    .frequency = 433920000,
    .bits = 9,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = ChamberlainFileProtocol};

/**
 * Chamberlain 8bit 300MHz
 */
const SubBruteProtocol subbrute_protocol_chamberlain_8bit_300 = {
    .frequency = 300000000,
    .bits = 8,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = ChamberlainFileProtocol};

/**
 * Chamberlain 8bit 315MHz
 */
const SubBruteProtocol subbrute_protocol_chamberlain_8bit_315 = {
    .frequency = 315000000,
    .bits = 8,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = ChamberlainFileProtocol};

/**
 * Chamberlain 8bit 390MHz
 */
const SubBruteProtocol subbrute_protocol_chamberlain_8bit_390 = {
    .frequency = 390000000,
    .bits = 8,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = ChamberlainFileProtocol};

/**
 * Chamberlain 7bit 300MHz
 */
const SubBruteProtocol subbrute_protocol_chamberlain_7bit_300 = {
    .frequency = 300000000,
    .bits = 7,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = ChamberlainFileProtocol};

/**
 * Chamberlain 7bit 315MHz
 */
const SubBruteProtocol subbrute_protocol_chamberlain_7bit_315 = {
    .frequency = 315000000,
    .bits = 7,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = ChamberlainFileProtocol};

/**
 * Chamberlain 7bit 390MHz
 */
const SubBruteProtocol subbrute_protocol_chamberlain_7bit_390 = {
    .frequency = 390000000,
    .bits = 7,
    .te = 0,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = ChamberlainFileProtocol};

/**
 * Linear 10bit 300MHz
 */
const SubBruteProtocol subbrute_protocol_linear_10bit_300 = {
    .frequency = 300000000,
    .bits = 10,
    .te = 0,
    .repeat = 5,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = LinearFileProtocol};

/**
 * Linear 10bit 310MHz
 */
const SubBruteProtocol subbrute_protocol_linear_10bit_310 = {
    .frequency = 310000000,
    .bits = 10,
    .te = 0,
    .repeat = 5,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = LinearFileProtocol};

/**
 * Linear Delta 3 8bit 310MHz
 */
const SubBruteProtocol subbrute_protocol_linear_delta_8bit_310 = {
    .frequency = 310000000,
    .bits = 8,
    .te = 0,
    .repeat = 5,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = LinearDeltaFileProtocol};

/**
 * UNILARM 24bit 330MHz
 */
const SubBruteProtocol subbrute_protocol_unilarm_24bit_330 = {
    .frequency = 330000000,
    .bits = 25,
    .te = 209,
    .repeat = 4,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = UNILARMFileProtocol};

/**
 * UNILARM 24bit 433MHz
 */
const SubBruteProtocol subbrute_protocol_unilarm_24bit_433 = {
    .frequency = 433920000,
    .bits = 25,
    .te = 209,
    .repeat = 4,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = UNILARMFileProtocol};

/**
 * SMC5326 24bit 330MHz
 */
const SubBruteProtocol subbrute_protocol_smc5326_24bit_330 = {
    .frequency = 330000000,
    .bits = 25,
    .te = 320,
    .repeat = 4,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = SMC5326FileProtocol};

/**
 * SMC5326 24bit 433MHz
 */
const SubBruteProtocol subbrute_protocol_smc5326_24bit_433 = {
    .frequency = 433920000,
    .bits = 25,
    .te = 320,
    .repeat = 4,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = SMC5326FileProtocol};

/**
 * PT2260 (Princeton) 24bit 315MHz
 */
const SubBruteProtocol subbrute_protocol_pt2260_24bit_315 = {
    .frequency = 315000000,
    .bits = 24,
    .te = 286,
    .repeat = 4,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = PT2260FileProtocol};

/**
 * PT2260 (Princeton) 24bit 330MHz
 */
const SubBruteProtocol subbrute_protocol_pt2260_24bit_330 = {
    .frequency = 330000000,
    .bits = 24,
    .te = 286,
    .repeat = 4,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = PT2260FileProtocol};

/**
 * PT2260 (Princeton) 24bit 390MHz
 */
const SubBruteProtocol subbrute_protocol_pt2260_24bit_390 = {
    .frequency = 390000000,
    .bits = 24,
    .te = 286,
    .repeat = 4,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = PT2260FileProtocol};

/**
 * PT2260 (Princeton) 24bit 433MHz
 */
const SubBruteProtocol subbrute_protocol_pt2260_24bit_433 = {
    .frequency = 433920000,
    .bits = 24,
    .te = 286,
    .repeat = 4,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = PT2260FileProtocol};

/**
 * Holtek FM 12bit 433MHz
 */
const SubBruteProtocol subbrute_protocol_holtek_12bit_433 = {
    .frequency = 433920000,
    .bits = 12,
    .te = 204,
    .repeat = 4,
    .preset = FuriHalSubGhzPreset2FSKDev476Async,
    .file = HoltekFileProtocol};

/**
 * Holtek AM 12bit 433MHz
 */
const SubBruteProtocol subbrute_protocol_holtek_12bit_am_433 = {
    .frequency = 433920000,
    .bits = 12,
    .te = 433,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = HoltekFileProtocol};

/**
 * Holtek AM 12bit 315MHz
 */
const SubBruteProtocol subbrute_protocol_holtek_12bit_am_315 = {
    .frequency = 315000000,
    .bits = 12,
    .te = 433,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = HoltekFileProtocol};

/**
 * Holtek AM 12bit 868MHz
 */
const SubBruteProtocol subbrute_protocol_holtek_12bit_am_868 = {
    .frequency = 868350000,
    .bits = 12,
    .te = 433,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = HoltekFileProtocol};

/**
 * Holtek AM 12bit 915MHz
 */
const SubBruteProtocol subbrute_protocol_holtek_12bit_am_915 = {
    .frequency = 915000000,
    .bits = 12,
    .te = 433,
    .repeat = 3,
    .preset = FuriHalSubGhzPresetOok650Async,
    .file = HoltekFileProtocol};

/**
 * BF existing dump
 */
const SubBruteProtocol subbrute_protocol_load_file =
    {0, 0, 0, 3, FuriHalSubGhzPresetOok650Async, UnknownFileProtocol};

static const char* subbrute_protocol_names[] = {
    [SubBruteAttackCAME12bit303] = "CAME 12bit 303MHz",
    [SubBruteAttackCAME12bit307] = "CAME 12bit 307MHz",
    [SubBruteAttackCAME12bit315] = "CAME 12bit 315MHz",
    [SubBruteAttackCAME12bit433] = "CAME 12bit 433MHz",
    [SubBruteAttackCAME12bit868] = "CAME 12bit 868MHz",
    [SubBruteAttackNICE12bit433] = "NICE 12bit 433MHz",
    [SubBruteAttackNICE12bit868] = "NICE 12bit 868MHz",
    [SubBruteAttackAnsonic12bit433075] = "Ansonic 12bit 433.07MHz",
    [SubBruteAttackAnsonic12bit433] = "Ansonic 12bit 433.92MHz",
    [SubBruteAttackAnsonic12bit434] = "Ansonic 12bit 434.07MHz",
    [SubBruteAttackHoltek12bitFM433] = "Holtek FM 12bit 433MHz",
    [SubBruteAttackHoltek12bitAM433] = "Holtek AM 12bit 433MHz",
    [SubBruteAttackHoltek12bitAM315] = "Holtek AM 12bit 315MHz",
    [SubBruteAttackHoltek12bitAM868] = "Holtek AM 12bit 868MHz",
    [SubBruteAttackHoltek12bitAM915] = "Holtek AM 12bit 915MHz",
    [SubBruteAttackChamberlain9bit300] = "Chamberlain 9bit 300MHz",
    [SubBruteAttackChamberlain9bit315] = "Chamberlain 9bit 315MHz",
    [SubBruteAttackChamberlain9bit390] = "Chamberlain 9bit 390MHz",
    [SubBruteAttackChamberlain9bit433] = "Chamberlain 9bit 433MHz",
    [SubBruteAttackChamberlain8bit300] = "Chamberlain 8bit 300MHz",
    [SubBruteAttackChamberlain8bit315] = "Chamberlain 8bit 315MHz",
    [SubBruteAttackChamberlain8bit390] = "Chamberlain 8bit 390MHz",
    [SubBruteAttackChamberlain7bit300] = "Chamberlain 7bit 300MHz",
    [SubBruteAttackChamberlain7bit315] = "Chamberlain 7bit 315MHz",
    [SubBruteAttackChamberlain7bit390] = "Chamberlain 7bit 390MHz",
    [SubBruteAttackLinear10bit300] = "Linear 10bit 300MHz",
    [SubBruteAttackLinear10bit310] = "Linear 10bit 310MHz",
    [SubBruteAttackLinearDelta8bit310] = "LinearDelta3 8bit 310MHz",
    [SubBruteAttackUNILARM24bit330] = "UNILARM 25bit 330MHz",
    [SubBruteAttackUNILARM24bit433] = "UNILARM 25bit 433MHz",
    [SubBruteAttackSMC532624bit330] = "SMC5326 25bit 330MHz",
    [SubBruteAttackSMC532624bit433] = "SMC5326 25bit 433MHz",
    [SubBruteAttackPT226024bit315] = "PT2260 24bit 315MHz",
    [SubBruteAttackPT226024bit330] = "PT2260 24bit 330MHz",
    [SubBruteAttackPT226024bit390] = "PT2260 24bit 390MHz",
    [SubBruteAttackPT226024bit433] = "PT2260 24bit 433MHz",
    [SubBruteAttackLoadFile] = "BF existing dump",
    [SubBruteAttackTotalCount] = "Total Count",
};

static const char* subbrute_protocol_presets[] = {
    [FuriHalSubGhzPresetIDLE] = "FuriHalSubGhzPresetIDLE",
    [FuriHalSubGhzPresetOok270Async] = "FuriHalSubGhzPresetOok270Async",
    [FuriHalSubGhzPresetOok650Async] = "FuriHalSubGhzPresetOok650Async",
    [FuriHalSubGhzPreset2FSKDev238Async] = "FuriHalSubGhzPreset2FSKDev238Async",
    [FuriHalSubGhzPreset2FSKDev476Async] = "FuriHalSubGhzPreset2FSKDev476Async",
    [FuriHalSubGhzPresetMSK99_97KbAsync] = "FuriHalSubGhzPresetMSK99_97KbAsync",
    [FuriHalSubGhzPresetGFSK9_99KbAsync] = "FuriHalSubGhzPresetGFSK9_99KbAsync",
};

const SubBruteProtocol* subbrute_protocol_registry[] = {
    [SubBruteAttackCAME12bit303] = &subbrute_protocol_came_12bit_303,
    [SubBruteAttackCAME12bit307] = &subbrute_protocol_came_12bit_307,
    [SubBruteAttackCAME12bit315] = &subbrute_protocol_came_12bit_315,
    [SubBruteAttackCAME12bit433] = &subbrute_protocol_came_12bit_433,
    [SubBruteAttackCAME12bit868] = &subbrute_protocol_came_12bit_868,
    [SubBruteAttackNICE12bit433] = &subbrute_protocol_nice_12bit_433,
    [SubBruteAttackNICE12bit868] = &subbrute_protocol_nice_12bit_868,
    [SubBruteAttackAnsonic12bit433075] = &subbrute_protocol_ansonic_12bit_433075,
    [SubBruteAttackAnsonic12bit433] = &subbrute_protocol_ansonic_12bit_433,
    [SubBruteAttackAnsonic12bit434] = &subbrute_protocol_ansonic_12bit_434,
    [SubBruteAttackHoltek12bitFM433] = &subbrute_protocol_holtek_12bit_433,
    [SubBruteAttackHoltek12bitAM433] = &subbrute_protocol_holtek_12bit_am_433,
    [SubBruteAttackHoltek12bitAM315] = &subbrute_protocol_holtek_12bit_am_315,
    [SubBruteAttackHoltek12bitAM868] = &subbrute_protocol_holtek_12bit_am_868,
    [SubBruteAttackHoltek12bitAM915] = &subbrute_protocol_holtek_12bit_am_915,
    [SubBruteAttackChamberlain9bit300] = &subbrute_protocol_chamberlain_9bit_300,
    [SubBruteAttackChamberlain9bit315] = &subbrute_protocol_chamberlain_9bit_315,
    [SubBruteAttackChamberlain9bit390] = &subbrute_protocol_chamberlain_9bit_390,
    [SubBruteAttackChamberlain9bit433] = &subbrute_protocol_chamberlain_9bit_433,
    [SubBruteAttackChamberlain8bit300] = &subbrute_protocol_chamberlain_8bit_300,
    [SubBruteAttackChamberlain8bit315] = &subbrute_protocol_chamberlain_8bit_315,
    [SubBruteAttackChamberlain8bit390] = &subbrute_protocol_chamberlain_8bit_390,
    [SubBruteAttackChamberlain7bit300] = &subbrute_protocol_chamberlain_7bit_300,
    [SubBruteAttackChamberlain7bit315] = &subbrute_protocol_chamberlain_7bit_315,
    [SubBruteAttackChamberlain7bit390] = &subbrute_protocol_chamberlain_7bit_390,
    [SubBruteAttackLinear10bit300] = &subbrute_protocol_linear_10bit_300,
    [SubBruteAttackLinear10bit310] = &subbrute_protocol_linear_10bit_310,
    [SubBruteAttackLinearDelta8bit310] = &subbrute_protocol_linear_delta_8bit_310,
    [SubBruteAttackUNILARM24bit330] = &subbrute_protocol_unilarm_24bit_330,
    [SubBruteAttackUNILARM24bit433] = &subbrute_protocol_unilarm_24bit_433,
    [SubBruteAttackSMC532624bit330] = &subbrute_protocol_smc5326_24bit_330,
    [SubBruteAttackSMC532624bit433] = &subbrute_protocol_smc5326_24bit_433,
    [SubBruteAttackPT226024bit315] = &subbrute_protocol_pt2260_24bit_315,
    [SubBruteAttackPT226024bit330] = &subbrute_protocol_pt2260_24bit_330,
    [SubBruteAttackPT226024bit390] = &subbrute_protocol_pt2260_24bit_390,
    [SubBruteAttackPT226024bit433] = &subbrute_protocol_pt2260_24bit_433,
    [SubBruteAttackLoadFile] = &subbrute_protocol_load_file};

static const char* subbrute_protocol_file_types[] = {
    [CAMEFileProtocol] = "CAME",
    [NICEFileProtocol] = "Nice FLO",
    [ChamberlainFileProtocol] = "Cham_Code",
    [LinearFileProtocol] = "Linear",
    [LinearDeltaFileProtocol] = "LinearDelta3",
    [PrincetonFileProtocol] = "Princeton",
    [RAWFileProtocol] = "RAW",
    [BETTFileProtocol] = "BETT",
    [ClemsaFileProtocol] = "Clemsa",
    [DoitrandFileProtocol] = "Doitrand",
    [GateTXFileProtocol] = "GateTX",
    [MagellanFileProtocol] = "Magellan",
    [IntertechnoV3FileProtocol] = "Intertechno_V3",
    [AnsonicFileProtocol] = "Ansonic",
    [SMC5326FileProtocol] = "SMC5326",
    [UNILARMFileProtocol] = "SMC5326",
    [PT2260FileProtocol] = "Princeton",
    [HoneywellFileProtocol] = "Honeywell",
    [HoltekFileProtocol] = "Holtek_HT12X",
    [UnknownFileProtocol] = "Unknown"};

/**
 * Values to not use less memory for packet parse operations
 */
static const char* subbrute_key_file_start_no_tail =
    "Filetype: Flipper SubGhz Key File\nVersion: 1\nFrequency: %u\nPreset: %s\nProtocol: %s\nBit: %d\nKey: %s\n";
static const char* subbrute_key_file_start_with_tail =
    "Filetype: Flipper SubGhz Key File\nVersion: 1\nFrequency: %u\nPreset: %s\nProtocol: %s\nBit: %d\nKey: %s\nTE: %d\n";
static const char* subbrute_key_small_no_tail = "Bit: %d\nKey: %s\nRepeat: %d\n";
//static const char* subbrute_key_small_raw =
//    "Filetype: Flipper SubGhz Key File\nVersion: 1\nFrequency: %u\nPreset: %s\nProtocol: %s\nBit: %d\n";
static const char* subbrute_key_small_with_tail = "Bit: %d\nKey: %s\nTE: %d\nRepeat: %d\n";

const char* subbrute_protocol_name(SubBruteAttacks index) {
    return subbrute_protocol_names[index];
}

const SubBruteProtocol* subbrute_protocol(SubBruteAttacks index) {
    return subbrute_protocol_registry[index];
}

uint8_t subbrute_protocol_repeats_count(SubBruteAttacks index) {
    return subbrute_protocol_registry[index]->repeat;
}

const char* subbrute_protocol_preset(FuriHalSubGhzPreset preset) {
    return subbrute_protocol_presets[preset];
}

const char* subbrute_protocol_file(SubBruteFileProtocol protocol) {
    return subbrute_protocol_file_types[protocol];
}

FuriHalSubGhzPreset subbrute_protocol_convert_preset(FuriString* preset_name) {
    for(size_t i = FuriHalSubGhzPresetIDLE; i < FuriHalSubGhzPresetCustom; i++) {
        if(furi_string_cmp_str(preset_name, subbrute_protocol_presets[i]) == 0) {
            return i;
        }
    }

    return FuriHalSubGhzPresetIDLE;
}

SubBruteFileProtocol subbrute_protocol_file_protocol_name(FuriString* name) {
    for(size_t i = CAMEFileProtocol; i < TotalFileProtocol - 1; i++) {
        if(furi_string_cmp_str(name, subbrute_protocol_file_types[i]) == 0) {
            return i;
        }
    }

    return UnknownFileProtocol;
}

void subbrute_protocol_create_candidate_for_existing_file(
    FuriString* candidate,
    uint64_t step,
    size_t bit_index,
    uint64_t file_key,
    bool two_bytes) {
    uint8_t p[8];
    for(int i = 0; i < 8; i++) {
        p[i] = (uint8_t)(file_key >> 8 * (7 - i)) & 0xFF;
    }
    uint8_t low_byte = step & (0xff);
    uint8_t high_byte = (step >> 8) & 0xff;

    size_t size = sizeof(uint64_t);
    for(size_t i = 0; i < size; i++) {
        if(i == bit_index - 1 && two_bytes) {
            furi_string_cat_printf(candidate, "%02X %02X", high_byte, low_byte);
            i++;
        } else if(i == bit_index) {
            furi_string_cat_printf(candidate, "%02X", low_byte);
        } else if(p[i] != 0) {
            furi_string_cat_printf(candidate, "%02X", p[i]);
        } else {
            furi_string_cat_printf(candidate, "%s", "00");
        }

        if(i < size - 1) {
            furi_string_push_back(candidate, ' ');
        }
    }

#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "file candidate: %s, step: %lld", furi_string_get_cstr(candidate), step);
#endif
}

void subbrute_protocol_create_candidate_for_default(
    FuriString* candidate,
    SubBruteFileProtocol file,
    uint64_t step) {
    uint8_t p[8];
    if(file == SMC5326FileProtocol) {
        const uint8_t lut[] = {0x00, 0x02, 0x03}; // 00, 10, 11
        const uint64_t gate1 = 0x01D5; // 111010101
        //const uint8_t gate2 = 0x0175; // 101110101

        uint64_t total = 0;
        for(size_t j = 0; j < 8; j++) {
            total |= lut[step % 3] << (2 * j);
            double sub_step = (double)step / 3;
            step = (uint64_t)floor(sub_step);
        }
        total <<= 9;
        total |= gate1;

        for(int i = 0; i < 8; i++) {
            p[i] = (uint8_t)(total >> 8 * (7 - i)) & 0xFF;
        }
    } else if(file == UNILARMFileProtocol) {
        const uint8_t lut[] = {0x00, 0x02, 0x03}; // 00, 10, 11
        const uint64_t gate1 = 3 << 7;
        //const uint8_t gate2 = 3 << 5;

        uint64_t total = 0;
        for(size_t j = 0; j < 8; j++) {
            total |= lut[step % 3] << (2 * j);
            double sub_step = (double)step / 3;
            step = (uint64_t)floor(sub_step);
        }
        total <<= 9;
        total |= gate1;

        for(int i = 0; i < 8; i++) {
            p[i] = (uint8_t)(total >> 8 * (7 - i)) & 0xFF;
        }
    } else if(file == PT2260FileProtocol) {
        const uint8_t lut[] = {0x00, 0x01, 0x03}; // 00, 01, 11
        const uint64_t button_open = 0x03; // 11
        //const uint8_t button_lock = 0x0C; // 1100
        //const uint8_t button_stop = 0x30; // 110000
        //const uint8_t button_close = 0xC0; // 11000000

        uint64_t total = 0;
        for(size_t j = 0; j < 8; j++) {
            total |= lut[step % 3] << (2 * j);
            double sub_step = (double)step / 3;
            step = (uint64_t)floor(sub_step);
        }
        total <<= 8;
        total |= button_open;

        for(int i = 0; i < 8; i++) {
            p[i] = (uint8_t)(total >> 8 * (7 - i)) & 0xFF;
        }
    } else {
        for(int i = 0; i < 8; i++) {
            p[i] = (uint8_t)(step >> 8 * (7 - i)) & 0xFF;
        }
    }

    size_t size = sizeof(uint64_t);
    for(size_t i = 0; i < size; i++) {
        if(p[i] != 0) {
            furi_string_cat_printf(candidate, "%02X", p[i]);
        } else {
            furi_string_cat_printf(candidate, "%s", "00");
        }

        if(i < size - 1) {
            furi_string_push_back(candidate, ' ');
        }
    }

#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "candidate: %s, step: %lld", furi_string_get_cstr(candidate), step);
#endif
}

void subbrute_protocol_default_payload(
    Stream* stream,
    SubBruteFileProtocol file,
    uint64_t step,
    uint8_t bits,
    uint32_t te,
    uint8_t repeat) {
    FuriString* candidate = furi_string_alloc();
    subbrute_protocol_create_candidate_for_default(candidate, file, step);

#ifdef FURI_DEBUG
    FURI_LOG_D(
        TAG,
        "candidate: %s, step: %lld, repeat: %d, te: %s",
        furi_string_get_cstr(candidate),
        step,
        repeat,
        te ? "true" : "false");
#endif
    stream_clean(stream);
    if(te) {
        stream_write_format(
            stream,
            subbrute_key_small_with_tail,
            bits,
            furi_string_get_cstr(candidate),
            te,
            repeat);
    } else {
        stream_write_format(
            stream, subbrute_key_small_no_tail, bits, furi_string_get_cstr(candidate), repeat);
    }

    furi_string_free(candidate);
}

void subbrute_protocol_file_payload(
    Stream* stream,
    uint64_t step,
    uint8_t bits,
    uint32_t te,
    uint8_t repeat,
    uint8_t bit_index,
    uint64_t file_key,
    bool two_bytes) {
    FuriString* candidate = furi_string_alloc();
    subbrute_protocol_create_candidate_for_existing_file(
        candidate, step, bit_index, file_key, two_bytes);

#ifdef FURI_DEBUG
    FURI_LOG_D(
        TAG,
        "candidate: %s, step: %lld, repeat: %d, te: %s",
        furi_string_get_cstr(candidate),
        step,
        repeat,
        te ? "true" : "false");
#endif
    stream_clean(stream);

    if(te) {
        stream_write_format(
            stream,
            subbrute_key_small_with_tail,
            bits,
            furi_string_get_cstr(candidate),
            te,
            repeat);
    } else {
        stream_write_format(
            stream, subbrute_key_small_no_tail, bits, furi_string_get_cstr(candidate), repeat);
    }

    furi_string_free(candidate);
}

void subbrute_protocol_default_generate_file(
    Stream* stream,
    uint32_t frequency,
    FuriHalSubGhzPreset preset,
    SubBruteFileProtocol file,
    uint64_t step,
    uint8_t bits,
    uint32_t te) {
    FuriString* candidate = furi_string_alloc();
    subbrute_protocol_create_candidate_for_default(candidate, file, step);

#ifdef FURI_DEBUG
    FURI_LOG_D(TAG, "candidate: %s, step: %lld", furi_string_get_cstr(candidate), step);
#endif
    stream_clean(stream);

    if(te) {
        stream_write_format(
            stream,
            subbrute_key_file_start_with_tail,
            frequency,
            subbrute_protocol_preset(preset),
            subbrute_protocol_file(file),
            bits,
            furi_string_get_cstr(candidate),
            te);
    } else {
        stream_write_format(
            stream,
            subbrute_key_file_start_no_tail,
            frequency,
            subbrute_protocol_preset(preset),
            subbrute_protocol_file(file),
            bits,
            furi_string_get_cstr(candidate));
    }

    furi_string_free(candidate);
}

void subbrute_protocol_file_generate_file(
    Stream* stream,
    uint32_t frequency,
    FuriHalSubGhzPreset preset,
    SubBruteFileProtocol file,
    uint64_t step,
    uint8_t bits,
    uint32_t te,
    uint8_t bit_index,
    uint64_t file_key,
    bool two_bytes) {
    FuriString* candidate = furi_string_alloc();
    // char subbrute_payload_byte[8];
    //furi_string_set_str(candidate, file_key);
    subbrute_protocol_create_candidate_for_existing_file(
        candidate, step, bit_index, file_key, two_bytes);

    stream_clean(stream);

    if(te) {
        stream_write_format(
            stream,
            subbrute_key_file_start_with_tail,
            frequency,
            subbrute_protocol_preset(preset),
            subbrute_protocol_file(file),
            bits,
            furi_string_get_cstr(candidate),
            te);
    } else {
        stream_write_format(
            stream,
            subbrute_key_file_start_no_tail,
            frequency,
            subbrute_protocol_preset(preset),
            subbrute_protocol_file(file),
            bits,
            furi_string_get_cstr(candidate));
    }

    furi_string_free(candidate);
}

uint64_t
    subbrute_protocol_calc_max_value(SubBruteAttacks attack_type, uint8_t bits, bool two_bytes) {
    uint64_t max_value;
    if(attack_type == SubBruteAttackLoadFile) {
        max_value = two_bytes ? 0xFFFF : 0xFF;
    } else if(
        attack_type == SubBruteAttackSMC532624bit330 ||
        attack_type == SubBruteAttackSMC532624bit433 ||
        attack_type == SubBruteAttackUNILARM24bit330 ||
        attack_type == SubBruteAttackUNILARM24bit433 ||
        attack_type == SubBruteAttackPT226024bit315 ||
        attack_type == SubBruteAttackPT226024bit330 ||
        attack_type == SubBruteAttackPT226024bit390 ||
        attack_type == SubBruteAttackPT226024bit433) {
        max_value = 6561;
    } else {
        FuriString* max_value_s;
        max_value_s = furi_string_alloc();
        for(uint8_t i = 0; i < bits; i++) {
            furi_string_cat_printf(max_value_s, "1");
        }
        max_value = (uint64_t)strtol(furi_string_get_cstr(max_value_s), NULL, 2);
        furi_string_free(max_value_s);
    }

    return max_value;
}
