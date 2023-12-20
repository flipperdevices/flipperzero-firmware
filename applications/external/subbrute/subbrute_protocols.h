#pragma once

#include <furi.h>
#include <furi_hal_subghz.h>
#include <core/string.h>
#include <toolbox/stream/stream.h>

#define SUBBRUTE_PROTOCOL_MAX_REPEATS 9

/**
 * @enum SubBruteFileProtocol
 * @brief Enum defining various file protocols used in a system
 *
 * This enum defines different file protocols that can be used in the system.
 * Each file protocol is identified by its name.
 *
 * The possible values for this enum are:
 * - CAMEFileProtocol: CAME file protocol
 * - NICEFileProtocol: NICE file protocol
 * - ChamberlainFileProtocol: Chamberlain file protocol
 * - LinearFileProtocol: Linear file protocol
 * - LinearDeltaFileProtocol: Linear Delta file protocol
 * - PrincetonFileProtocol: Princeton file protocol
 * - RAWFileProtocol: RAW file protocol
 * - BETTFileProtocol: BETT file protocol
 * - ClemsaFileProtocol: Clemsa file protocol
 * - DoitrandFileProtocol: Doitrand file protocol
 * - GateTXFileProtocol: GateTX file protocol
 * - MagellanFileProtocol: Magellan file protocol
 * - IntertechnoV3FileProtocol: Intertechno V3 file protocol
 * - AnsonicFileProtocol: Ansonic file protocol
 * - SMC5326FileProtocol: SMC5326 file protocol
 * - UNILARMFileProtocol: UNILARM file protocol
 * - PT2260FileProtocol: PT2260 file protocol
 * - HoneywellFileProtocol: Honeywell file protocol
 * - HoltekFileProtocol: Holtek file protocol
 * - UnknownFileProtocol: Unknown file protocol
 * - TotalFileProtocol: Total file protocol
 */
typedef enum {
    CAMEFileProtocol,
    NICEFileProtocol,
    ChamberlainFileProtocol,
    LinearFileProtocol,
    LinearDeltaFileProtocol,
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

/**
 * @enum SubBruteAttacks
 *
 * Enumeration of all supported sub-brute attacks.
 *
 * The `SubBruteAttacks` represents the different types of sub-brute attacks
 * that can be performed. Each attack has a unique identifier, which can be used
 * to specify the attack when calling functions that handle sub-brute attacks.
 *
 * The possible values of `SubBruteAttacks` are as follows:
 *
 * - `SubBruteAttackCAME12bit303`: CAME 12-bit 303 MHz sub-brute attack.
 * - `SubBruteAttackCAME12bit307`: CAME 12-bit 307 MHz sub-brute attack.
 * - `SubBruteAttackCAME12bit315`: CAME 12-bit 315 MHz sub-brute attack.
 * - `SubBruteAttackCAME12bit433`: CAME 12-bit 433 MHz sub-brute attack.
 * - `SubBruteAttackCAME12bit868`: CAME 12-bit 868 MHz sub-brute attack.
 * - `SubBruteAttackNICE12bit433`: NICE 12-bit 433 MHz sub-brute attack.
 * - `SubBruteAttackNICE12bit868`: NICE 12-bit 868 MHz sub-brute attack.
 * - `SubBruteAttackAnsonic12bit433075`: Ansonic 12-bit 433.075 MHz sub-brute attack.
 * - `SubBruteAttackAnsonic12bit433`: Ansonic 12-bit 433 MHz sub-brute attack.
 * - `SubBruteAttackAnsonic12bit434`: Ansonic 12-bit 434 MHz sub-brute attack.
 * - `SubBruteAttackHoltek12bitFM433`: Holtek 12-bit FM 433 MHz sub-brute attack.
 * - `SubBruteAttackHoltek12bitAM433`: Holtek 12-bit AM 433 MHz sub-brute attack.
 * - `SubBruteAttackHoltek12bitAM315`: Holtek 12-bit AM 315 MHz sub-brute attack.
 * - `SubBruteAttackHoltek12bitAM868`: Holtek 12-bit AM 868 MHz sub-brute attack.
 * - `SubBruteAttackHoltek12bitAM915`: Holtek 12-bit AM 915 MHz sub-brute attack.
 * - `SubBruteAttackChamberlain9bit300`: Chamberlain 9-bit 300 MHz sub-brute attack.
 * - `SubBruteAttackChamberlain9bit315`: Chamberlain 9-bit 315 MHz sub-brute attack.
 * - `SubBruteAttackChamberlain9bit390`: Chamberlain 9-bit 390 MHz sub-brute attack.
 * - `SubBruteAttackChamberlain9bit433`: Chamberlain 9-bit 433 MHz sub-brute attack.
 * - `SubBruteAttackChamberlain8bit300`: Chamberlain 8-bit 300 MHz sub-brute attack.
 * - `SubBruteAttackChamberlain8bit315`: Chamberlain 8-bit 315 MHz sub-brute attack.
 * - `SubBruteAttackChamberlain8bit390`: Chamberlain 8-bit 390 MHz sub-brute attack.
 * - `SubBruteAttackChamberlain7bit300`: Chamberlain 7-bit 300 MHz sub-brute attack.
 * - `SubBruteAttackChamberlain7bit315`: Chamberlain 7-bit 315 MHz sub-brute attack.
 * - `SubBruteAttackChamberlain7bit390`: Chamberlain 7-bit 390 MHz sub-brute attack.
 * - `SubBruteAttackLinear10bit300`: Linear 10-bit 300 MHz sub-brute attack.
 * - `SubBruteAttackLinear10bit310`: Linear 10-bit 310 MHz sub-brute attack.
 * - `SubBruteAttackLinearDelta8bit310`: Linear Delta 8-bit 310 MHz sub-brute attack.
 * - `SubBruteAttackUNILARM24bit330`: UNILARM 24-bit 330 MHz sub-brute attack.
 * - `SubBruteAttackUNILARM24bit433`: UNILARM 24-bit 433 MHz sub-brute attack.
 * - `SubBruteAttackSMC532624bit330`: SMC5326 24-bit 330 MHz sub-brute attack.
 * - `SubBruteAttackSMC532624bit433`: SMC5326 6-bit 433 MHz sub-brute attack.
 * - `SubBruteAttackPT226024bit315`: PT2260 24-bit 315 MHz sub-brute attack.
 * - `SubBruteAttackPT226024bit330`: PT2260 24-bit 330 MHz sub-brute attack.
 * - `SubBruteAttackPT226024bit390`: PT2260 24-bit 390 MHz sub-brute attack.
 * - `SubBruteAttackPT226024bit433`: PT2260 24-bit 433 MHz sub-brute attack.
 * - `SubBruteAttackLoadFile`: Sub-brute attack using values loaded from a file.
 * - `SubBruteAttackTotalCount`: Total number of sub-brute attacks.
 */
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
    SubBruteAttackHoltek12bitFM433,
    SubBruteAttackHoltek12bitAM433,
    SubBruteAttackHoltek12bitAM315,
    SubBruteAttackHoltek12bitAM868,
    SubBruteAttackHoltek12bitAM915,
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
    SubBruteAttackLinearDelta8bit310,
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

/**
 * @struct SubBruteProtocol
 * @brief Structure representing the SubBrute protocol.
 *
 * The SubBruteProtocol struct contains information about the protocol parameters such as frequency,
 * bits, transmission time (te), repeat count, preset type, and file details using SubBruteFileProtocol.
 */
typedef struct {
    uint32_t frequency;
    uint8_t bits;
    uint32_t te;
    uint8_t repeat;
    FuriHalSubGhzPreset preset;
    SubBruteFileProtocol file;
} SubBruteProtocol;

/**
 * @brief Get the SubBruteProtocol object based on the given index
 *
 * This function returns the SubBruteProtocol object based on the provided index.
 *
 * @param index The index of the SubBruteProtocol
 * @return const SubBruteProtocol* The pointer to the SubBruteProtocol object
 */
const SubBruteProtocol* subbrute_protocol(SubBruteAttacks index);

/**
 * @brief Retrieves a sub-GHz protocol preset for brute force attack.
 *
 * Given a FuriHalSubGhzPreset value, this function retrieves a sub-GHz protocol
 * preset suitable for performing a brute force attack. The protocol preset is
 * returned as a null-terminated string.
 *
 * @param preset The FuriHalSubGhzPreset value representing the desired sub-GHz
 *               protocol preset.
 * @return A null-terminated string representing the protocol preset.
 */
const char* subbrute_protocol_preset(FuriHalSubGhzPreset preset);

/**
 * @brief Determines the file protocol used by SubBrute
 *
 * This function takes a SubBruteFileProtocol and returns the corresponding file protocol used by SubBrute.
 *
 * @param protocol The SubBruteFileProtocol to retrieve the file protocol for
 *
 * @returns The file protocol as a C string
 */
const char* subbrute_protocol_file(SubBruteFileProtocol protocol);

/**
 * @brief Convert a preset name to a FuriHalSubGhzPreset.
 *
 * This function takes a preset name as a FuriString pointer and converts it to a corresponding FuriHalSubGhzPreset value.
 * The converted preset is returned as a FuriHalSubGhzPreset value. If the preset name is not recognized, the behavior is undefined.
 *
 * @param preset_name The preset name to be converted.
 *
 * @return The converted FuriHalSubGhzPreset value.
 */
FuriHalSubGhzPreset subbrute_protocol_convert_preset(FuriString* preset_name);

/**
 * @brief Creates a SubBruteFileProtocol with the given name.
 *
 * @param name The name of the protocol.
 * @return The created SubBruteFileProtocol.
 */
SubBruteFileProtocol subbrute_protocol_file_protocol_name(FuriString* name);

/**
 * @brief Get the number of protocol repeats for a specific SubBrute attack.
 *
 * This function returns the number of protocol repeats for a specific SubBrute attack, identified by its index.
 *
 * @param index The index of the SubBrute attack.
 * @return The number of protocol repeats for the specified SubBrute attack.
 */
uint8_t subbrute_protocol_repeats_count(SubBruteAttacks index);

/**
 * @brief Retrieves the protocol name for a given SubBrute attack.
 *
 * This function returns the protocol name associated with the specified SubBrute attack.
 *
 * @param index The index of the SubBrute attack.
 * @return The protocol name as a constant character pointer.
 */
const char* subbrute_protocol_name(SubBruteAttacks index);

/**
 * @brief Executes a sub-brute force attack with default payload.
 *
 * This function performs a sub-brute force attack using the default payload on the specified stream and file protocol.
 * It continues the attack in steps, with each step increasing the payload by a specified number of bits.
 *
 * @param stream The stream to perform the attack on.
 * @param file The file protocol to use for the attack.
 * @param step The number of bits to increase the payload with in each step.
 * @param bits The initial number of bits in the payload.
 * @param te The timeout value in milliseconds.
 * @param repeat The number of times to repeat the attack.
 *
 * @note The function does not return any value.
 */
void subbrute_protocol_default_payload(
    Stream* stream,
    SubBruteFileProtocol file,
    uint64_t step,
    uint8_t bits,
    uint32_t te,
    uint8_t repeat);

/**
 * @brief Performs a sub-brute force protocol operation with file payload.
 *
 * This function takes in a stream, step, bits, te, repeat, bit_index,
 * file_key, and two_bytes as input parameters and performs a sub-brute
 * force protocol operation with a file payload.
 *
 * @param stream The Stream object to operate on.
 * @param step The step value to use during the operation.
 * @param bits The number of bits to use.
 * @param te The te value to use during the operation.
 * @param repeat The number of times to repeat the operation.
 * @param bit_index The bit index to use during the operation.
 * @param file_key The file key to use during the operation.
 * @param two_bytes A boolean indicating whether to use two bytes in the operation.
 *
 * @return None
 */
void subbrute_protocol_file_payload(
    Stream* stream,
    uint64_t step,
    uint8_t bits,
    uint32_t te,
    uint8_t repeat,
    uint8_t bit_index,
    uint64_t file_key,
    bool two_bytes);

/**
 * @brief Generates a file using the SubBrute protocol with default settings.
 *
 * This function generates a file using the SubBrute protocol with default settings.
 *
 * @param stream The stream used for writing the file.
 * @param frequency The frequency of the SubBrute protocol.
 * @param preset The SubGhzPreset used for transmission.
 * @param file The SubBruteFileProtocol used for generating the file.
 * @param step The step size used for generating the file.
 * @param bits The number of bits to generate for each interval.
 * @param te The duration of each transmission interval in milliseconds.
 */
void subbrute_protocol_default_generate_file(
    Stream* stream,
    uint32_t frequency,
    FuriHalSubGhzPreset preset,
    SubBruteFileProtocol file,
    uint64_t step,
    uint8_t bits,
    uint32_t te);

/**
 * @brief Generates a file for the SubBrute protocol with the given parameters.
 *
 * This function generates a file for the SubBrute protocol using the provided parameters.
 * The generated file can be used for various purposes, such as testing or data analysis.
 *
 * @param stream The Stream to output the generated file to.
 * @param frequency The frequency to use for the SubBrute protocol.
 * @param preset The SubGhzPreset to use for the SubBrute protocol.
 * @param file The SubBruteFileProtocol to generate.
 * @param step The step value to use for the SubBrute protocol.
 * @param bits The number of bits to use for each transmission in the SubBrute protocol.
 * @param te The TE value to use for the SubBrute protocol.
 * @param bit_index The starting bit index for the SubBrute protocol.
 * @param file_key The file key to use for generating the SubBrute file.
 * @param two_bytes Indicates whether two bytes should be used for each transmission (true) or not (false).
 */
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
    bool two_bytes);

/**
 * @brief Calculates the maximum value based on the attack type, number of bits, and whether two bytes are used.
 *
 * This function calculates the maximum value that can be generated based on the specified attack type,
 * number of bits, and whether two bytes are used. The result is returned as a 64-bit unsigned integer.
 *
 * @param attack_type   The type of attack to be performed (SubBruteAttacks).
 * @param bits          The number of bits used for the attack (uint8_t).
 * @param two_bytes     Whether two bytes are used for the attack (bool).
 *
 * @return The maximum value that can be generated based on the attack parameters (uint64_t).
 */
uint64_t
    subbrute_protocol_calc_max_value(SubBruteAttacks attack_type, uint8_t bits, bool two_bytes);
