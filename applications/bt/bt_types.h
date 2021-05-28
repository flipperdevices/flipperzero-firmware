#pragma once

#include <stdint.h>

typedef enum {
    BtMessageTypeStartTestCarrier,
    BtMessageTypeHoppingTx,
    BtMessageTypeStopTestCarrier,
    BtMessageTypeSetupTestPacketTx,
    BtMessageTypeStartTestPacketTx,
    BtMessageTypeStopTestPacket,
    BtMessageTypeSetupTestPacketRx,
    BtMessageTypeStartTestPacketRx,
    BtMessageTypeStopTestPacketRx,
    BtMessageTypeStartApp,
    BtMessageTypeUpdateStatusbar,
} BtMessageType;

typedef enum {
    BtStatusReady,
    BtStatusCarrierTx,
    BtStatusHoppingTx,
    BtStatusCarrierRx,
    BtStatusPacketSetup,
    BtStatusPacketRun,
    BtStatusStartedApp,
} BtStateType;

typedef enum {
    BtChannel2402 = 0,
    BtChannel2440 = 19,
    BtChannel2480 = 39,
} BtTestChannel;

typedef enum {
    BtPower0dB = 0x19,
    BtPower2dB = 0x1B,
    BtPower4dB = 0x1D,
    BtPower6dB = 0x1F,
} BtTestPower;

typedef enum {
    BtDateRate1M = 1,
    BtDateRate2M = 2,
} BtTestDataRate;

typedef struct {
    BtTestChannel channel;
    BtTestPower power;
    BtTestDataRate datarate;
    float rssi;
    uint16_t packets;
} BtTestParam;

typedef struct {
    BtMessageType type;
    BtTestParam param;
} BtMessage;

typedef struct {
    BtStateType type;
    BtTestParam param;
} BtState;
