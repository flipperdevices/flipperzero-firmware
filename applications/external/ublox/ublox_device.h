// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <furi.h>

#define UBLOX_I2C_ADDRESS 0x42
#define I2C_TIMEOUT_MS 20

#define UBX_NAV_CLASS 0x01
#define UBX_RXM_CLASS 0x02
#define UBX_INF_CLASS 0x04
#define UBX_ACK_CLASS 0x05
#define UBX_CFG_CLASS 0x06
#define UBX_UPD_CLASS 0x09
#define UBX_MON_CLASS 0x0A
#define UBX_AID_CLASS 0x0B
#define UBX_TIM_CLASS 0x0D
#define UBX_ESF_CLASS 0x10
#define UBX_MGA_CLASS 0x13
#define UBX_LOG_CLASS 0x21
#define UBX_SEC_CLASS 0x27
#define UBX_HNR_CLASS 0x28

// The following are respective to a class.

// ACK_CLASS
#define UBX_ACK_ACK_MESSAGE 0x01
// ACK and NAK have the same length
#define UBX_ACK_ACK_MESSAGE_LENGTH (8 + 2)

// NAV_CLASS
#define UBX_NAV_PVT_MESSAGE 0x07
#define UBX_NAV_PVT_MESSAGE_LENGTH (8 + 92)
#define UBX_NAV_SAT_MESSAGE 0x35
#define UBX_NAV_ODO_MESSAGE 0x09
#define UBX_NAV_ODO_MESSAGE_LENGTH (8 + 20)
#define UBX_NAV_RESETODO_MESSAGE 0x10

// CFG_CLASS
#define UBX_CFG_PMS_MESSAGE 0x86
#define UBX_CFG_PMS_MESSAGE_LENGTH (8 + 8)
#define UBX_CFG_ODO_MESSAGE 0x1e
#define UBX_CFG_ODO_MESSAGE_LENGTH (8 + 20)
#define UBX_CFG_NAV5_MESSAGE 0x24
#define UBX_CFG_NAV5_MESSAGE_LENGTH (8 + 36)
/** A frame is a message sent to the GPS. This app supports u-blox 8 devices. */

typedef struct UbloxFrame {
    uint8_t sync1; // always 0xb5, or ISO-8859.1 for 'µ'
    uint8_t sync2; // always 0x62, or ASCII for 'b'
    // class and id together indicate what kind of message is being sent.
    uint8_t class; // message class
    uint8_t id; // message id

    uint16_t len; // length of the payload only, 2 bytes, little-endian
    uint8_t* payload; // any number of bytes

    // 2 bytes of checksum
    uint8_t ck_a;
    uint8_t ck_b;

    // metadata
    bool valid;
} UbloxFrame;

typedef struct UbloxMessage {
    uint8_t* message;
    uint8_t length;
} UbloxMessage;

// Field names taken directly from u-blox protocol manual.
typedef struct Ublox_NAV_PVT_Message {
    uint32_t iTOW;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t valid;
    uint32_t tAcc;
    int32_t nano;
    uint8_t fixType;
    uint8_t flags;
    uint8_t flags2;
    uint8_t numSV;
    int32_t lon;
    int32_t lat;
    int32_t height;
    int32_t hMSL;
    uint32_t hAcc;
    uint32_t vAcc;
    int32_t velN;
    int32_t velE;
    int32_t velD;
    int32_t gSpeed;
    int32_t headMot;
    uint32_t sAcc;
    uint32_t headAcc;
    uint16_t pDOP;
    uint16_t flags3;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
    int32_t headVeh;
    int16_t magDec;
    uint16_t magAcc;
} Ublox_NAV_PVT_Message;

typedef struct Ublox_NAV_ODO_Message {
    uint8_t version;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint32_t iTOW;
    uint32_t distance;
    uint32_t totalDistance;
    uint32_t distanceStd;
} Ublox_NAV_ODO_Message;

/** For a given UbloxFrame, populate the sync bytes, calculate the
 * checksum bytes (storing them in `frame`), allocate a uint8_t array,
 * and fill it with the contents of the frame in an order ready to
 * send to the GPS. */
UbloxMessage* ublox_frame_to_bytes(UbloxFrame* frame);

/** For an array of uint8_ts, convert them to a frame. Returns NULL if
    something goes wrong, either with invalid data or malloc errors.*/

UbloxFrame* ublox_bytes_to_frame(UbloxMessage* message);

void ublox_message_free(UbloxMessage* message);
void ublox_frame_free(UbloxFrame* frame);
