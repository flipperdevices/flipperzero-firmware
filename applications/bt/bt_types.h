#pragma once

typedef enum {
    BtMessageTypeSrartTestToneTx,
    BtMessageTypeHoppingTx,
    BtMessageTypeStopTestToneTx,
    BtMessageTypeSetupTestPacketTx,
    BtMessageTypeStartTestPacketTx,
    BtMessageTypeStopTestPacketTx,
    BtMessageTypeStartTestRx,
    BtMessageTypeStopTestRx,
    BtMessageTypeStartApp,
    BtMessageTypeUpdateStatusbar,
} BtMessageType;

typedef enum {
    BtStatusReady,
    BtStatusToneTx,
    BtStatusHoppingTx,
    BtStatusToneRx,
    BtStatusPacketSetup,
    BtStatusPacketTx,
    BtStatusStartedApp,
} BtStateType;

typedef enum {
    BtTestChannel2402 = 0,
    BtTestChannel2440 = 19,
    BtTestChannel2480 = 39,
} BtTestChannel;

typedef enum {
    BtTestPower0dB = 25,
    BtTestPower2dB = 27,
    BtTestPower4dB = 29,
    BtTestPower6dB = 31,
} BtTestPower;

typedef enum {
    BtTestDateRate1M = 1,
    BtTestDateRate2M = 2,
} BtTestDateRate;

typedef struct {
    BtTestChannel channel;
    BtTestPower power;
    BtTestDateRate daterate;
} BtTestParam;

typedef struct {
    BtMessageType type;
    BtTestParam param;
} BtMessage;

typedef struct {
    BtStateType type;
    BtTestParam param;
} BtState;
