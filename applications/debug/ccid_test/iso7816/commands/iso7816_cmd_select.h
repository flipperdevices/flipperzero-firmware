/**
 * @file iso7816_cmd_select.h
 * @brief ISO7816-4 SELECT Command (INS = 0xA4)
 * 
 * Implements the SELECT command as defined in ISO/IEC 7816-4:2005 Section 7.1.1.
 * 
 * Supported selection modes:
 * - P1=0x00: Select MF, DF, or EF by file identifier
 * - P1=0x01: Select child DF
 * - P1=0x02: Select EF under current DF
 * - P1=0x03: Select parent DF
 * - P1=0x04: Select by DF name (AID) - direct selection
 * - P1=0x08: Select by path from MF
 * - P1=0x09: Select by path from current DF
 */

#pragma once

#include "../iso7816_t0_apdu.h"
#include "../iso7816_filesystem.h"

#ifdef __cplusplus
extern "C" {
#endif

/** SELECT instruction code */
#define ISO7816_INS_SELECT      0xA4

/** P1 values for SELECT command (Table 39) */
typedef enum {
    /** Select MF, DF, or EF (data = file identifier or absent) */
    ISO7816_SELECT_P1_BY_FID            = 0x00,
    /** Select child DF (data = DF identifier) */
    ISO7816_SELECT_P1_CHILD_DF          = 0x01,
    /** Select EF under current DF (data = EF identifier) */
    ISO7816_SELECT_P1_EF_UNDER_DF       = 0x02,
    /** Select parent DF of current DF (no data) */
    ISO7816_SELECT_P1_PARENT_DF         = 0x03,
    /** Select by DF name / AID (data = AID) */
    ISO7816_SELECT_P1_BY_DF_NAME        = 0x04,
    /** Select by path from MF (data = path) */
    ISO7816_SELECT_P1_PATH_FROM_MF      = 0x08,
    /** Select by path from current DF (data = path) */
    ISO7816_SELECT_P1_PATH_FROM_CURRENT = 0x09,
} Iso7816SelectP1;

/** P2 bits 1-0: File occurrence (Table 40) */
typedef enum {
    /** First or only occurrence */
    ISO7816_SELECT_P2_FIRST     = 0x00,
    /** Last occurrence */
    ISO7816_SELECT_P2_LAST      = 0x01,
    /** Next occurrence */
    ISO7816_SELECT_P2_NEXT      = 0x02,
    /** Previous occurrence */
    ISO7816_SELECT_P2_PREVIOUS  = 0x03,
} Iso7816SelectOccurrence;

/** P2 bits 3-2: File control information returned (Table 40) */
typedef enum {
    /** Return FCI template, optional use of FCI tag */
    ISO7816_SELECT_P2_FCI       = 0x00,
    /** Return FCP template, mandatory FCP tag */
    ISO7816_SELECT_P2_FCP       = 0x04,
    /** Return FMD template, mandatory FMD tag */
    ISO7816_SELECT_P2_FMD       = 0x08,
    /** No response data if Le absent, proprietary if present */
    ISO7816_SELECT_P2_NONE      = 0x0C,
} Iso7816SelectFCI;

/** Mask for P2 occurrence bits */
#define ISO7816_SELECT_P2_OCCURRENCE_MASK   0x03

/** Mask for P2 FCI bits */
#define ISO7816_SELECT_P2_FCI_MASK          0x0C

/**
 * @brief Process SELECT command
 * 
 * @param fs File system context
 * @param command Command APDU
 * @param response Response APDU to fill
 */
void iso7816_cmd_select_process(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response);

#ifdef __cplusplus
}
#endif
