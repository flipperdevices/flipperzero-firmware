#pragma once

#include <stdlib.h> // malloc
#include <stdint.h> // uint32_t
#include <stdarg.h> // __VA_ARGS__
#include <string.h>
#include <stdio.h>

#include "sub.h"
#include "seader_worker_i.h"

#define SYNC (0x03)
#define CTRL (0x06)
#define NAK (0x15)

#define BMICCSTATUS_MASK 0x03
#define CARD_OUT 0x02
#define CARD_IN_1 0x03
#define CARD_IN_2 0x06
#define CARD_IN_BOTH 0x07

/*
 *  * BULK_OUT messages from PC to Reader
 *   * Defined in CCID Rev 1.1 6.1 (page 26)
 *    */
#define CCID_MESSAGE_TYPE_PC_to_RDR_IccPowerOn 0x62
#define CCID_MESSAGE_TYPE_PC_to_RDR_IccPowerOff 0x63
#define CCID_MESSAGE_TYPE_PC_to_RDR_GetSlotStatus 0x65
#define CCID_MESSAGE_TYPE_PC_to_RDR_XfrBlock 0x6f
#define CCID_MESSAGE_TYPE_PC_to_RDR_GetParameters 0x6c
#define CCID_MESSAGE_TYPE_PC_to_RDR_ResetParameters 0x6d
#define CCID_MESSAGE_TYPE_PC_to_RDR_SetParameters 0x61
#define CCID_MESSAGE_TYPE_PC_to_RDR_Escape 0x6b
#define CCID_MESSAGE_TYPE_PC_to_RDR_IccClock 0x6e
#define CCID_MESSAGE_TYPE_PC_to_RDR_T0APDU 0x6a
#define CCID_MESSAGE_TYPE_PC_to_RDR_Secure 0x69
#define CCID_MESSAGE_TYPE_PC_to_RDR_Mechanical 0x71
#define CCID_MESSAGE_TYPE_PC_to_RDR_Abort 0x72
#define CCID_MESSAGE_TYPE_PC_to_RDR_SetDataRateAndClockFrequency 0x73
/*
 *  * BULK_IN messages from Reader to PC
 *   * Defined in CCID Rev 1.1 6.2 (page 48)
 *    */
#define CCID_MESSAGE_TYPE_RDR_to_PC_DataBlock 0x80
#define CCID_MESSAGE_TYPE_RDR_to_PC_SlotStatus 0x81
#define CCID_MESSAGE_TYPE_RDR_to_PC_Parameters 0x82
#define CCID_MESSAGE_TYPE_RDR_to_PC_Escape 0x83
#define CCID_MESSAGE_TYPE_RDR_to_PC_DataRateAndClockFrequency 0x84
/*
 *  * INTERRUPT_IN messages from Reader to PC
 *   * Defined in CCID Rev 1.1 6.3 (page 56)
 *    */
#define CCID_MESSAGE_TYPE_RDR_to_PC_NotifySlotChange 0x50
#define CCID_MESSAGE_TYPE_RDR_to_PC_HardwareError 0x51

/* Status codes that go in bStatus (see 6.2.6) */
enum { ICC_STATUS_PRESENT_ACTIVE = 0, ICC_STATUS_PRESENT_INACTIVE, ICC_STATUS_NOT_PRESENT };
enum {
    COMMAND_STATUS_NO_ERROR = 0,
    COMMAND_STATUS_FAILED,
    COMMAND_STATUS_TIME_EXTENSION_REQUIRED
};
/* Error codes that go in bError (see 6.2.6) */
enum {
    ERROR_CMD_NOT_SUPPORTED = 0,
    ERROR_CMD_ABORTED = -1,
    ERROR_ICC_MUTE = -2,
    ERROR_XFR_PARITY_ERROR = -3,
    ERROR_XFR_OVERRUN = -4,
    ERROR_HW_ERROR = -5,
};

struct CCID_Message {
    uint8_t bMessageType;
    uint32_t dwLength;
    uint8_t bSlot;
    uint8_t bSeq;
    uint8_t bStatus;
    uint8_t bError;

    uint8_t* payload;
    size_t consumed;
};

void PC_to_RDR_IccPowerOn(SeaderUartBridge* seader_uart);
void PC_to_RDR_GetSlotStatus(SeaderUartBridge* seader_uart);
void PC_to_RDR_SetParameters(SeaderUartBridge* seader_uart);
void PC_to_RDR_GetParameters(SeaderUartBridge* seader_uart);
void PC_to_RDR_XfrBlock(SeaderUartBridge* seader_uart, uint8_t* data, size_t len);
size_t processCCID(SeaderWorker* seader_worker, uint8_t* cmd, size_t cmd_len);
