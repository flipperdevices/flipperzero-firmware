#pragma once

#define CH2_SERIAL_RESPONSE "N0001\r\0"
#define CH2_SW_VERSION_RESPONSE "v0107\r\0"
#define CH2_VERSION_RESPONSE "V1010\r\0"

// #define CH2_SW_VERSION_RESPONSE "vCH-OBD.M2\r\0"
// #define CH2_VERSION_RESPONSE "VLIN_20191106\r\0"

#define CH2_CR 0x0D // "\r"
#define CH2_LF 0x0A // "\n"
#define CH2_END_OF_CMD 0x00 // "\0"

#define CH2_BELL 0x07 // "\a"
#define CH2_OK 0x0D // "\r"

#define CH2_SET_BITRATE 'S' // set CAN bit rate
#define CH2_SET_BTR 's' // set CAN bit rate via
#define CH2_OPEN_CAN_CHAN 'O' // open CAN channel
#define CH2_CLOSE_CAN_CHAN 'C' // close CAN channel
#define CH2_SEND_T11BIT_ID 't' // send CAN message with 11bit ID
#define CH2_SEND_T29BIT_ID 'T' // send CAN message with 29bit ID
#define CH2_SEND_R11BIT_ID 'r' // send CAN remote message with 11bit ID
#define CH2_SEND_R29BIT_ID 'R' // send CAN remote message with 29bit ID
#define CH2_READ_STATUS 'F' // read status flag byte
#define CH2_SET_ACR 'M' // set Acceptance Code Register
#define CH2_SET_AMR 'm' // set Acceptance Mask Register
#define CH2_GET_VERSION 'V' // get hardware and software version
#define CH2_GET_SW_VERSION 'v' // get software version only
#define CH2_GET_SERIAL 'N' // get device serial number
#define CH2_TIME_STAMP 'Z' // toggle time stamp setting
#define CH2_READ_ECR 'E' // read Error Capture Register
#define CH2_READ_ALCR 'A' // read Arbritation Lost Capture Register
#define CH2_READ_REG 'G' // read register conten from SJA1000
#define CH2_WRITE_REG 'W' // write register content to SJA1000
#define CH2_LISTEN_ONLY 'L' // switch to listen only mode

typedef enum {
    CanHacker2BitrateOff = 0,
    CanHacker2Bitrate10K = 10000,
    CanHacker2Bitrate20K = 20000,
    CanHacker2Bitrate50K = 50000,
    CanHacker2Bitrate100K = 100000,
    CanHacker2Bitrate125K = 125000,
    CanHacker2Bitrate250K = 250000,
    CanHacker2Bitrate500K = 500000,
    CanHacker2Bitrate800K = 800000,
    CanHacker2Bitrate1M = 1000000,
} CanHacker2Bitrate;
