#pragma once

#include "stdio.h"
#include <stdarg.h>
#include "../mcp251xfd/MCP251XFD.h"
#include "../mcp251xfd/ErrorsDef.h"

#define TIMESTAMP_TICK_us (25) // TimeStamp tick is 25µs
#define TIMESTAMP_TICK(sysclk) (((sysclk) / 1000000) * TIMESTAMP_TICK_us)

#define CAN0_CLK (40000000) // The MCP251XFD on interface is at 40MHz
extern MCP251XFD can0;
#define CAN0 &can0

// Structure of the MCP251XFD configuration used in the CAN0 in the demo
extern MCP251XFD_BitTimeStats can0_bt_stats;
extern uint32_t can0_sysclk;
extern MCP251XFD_Config can0_config;

#define CAN0_FIFO_COUNT 18
extern MCP251XFD_RAMInfos can0_tef_ram_infos;
extern MCP251XFD_RAMInfos can0_txq_ram_infos;
extern MCP251XFD_FIFO can0_fifo_list[CAN0_FIFO_COUNT];

#define CAN0_FILTER_COUNT 13
extern MCP251XFD_Filter can_filter_list[CAN0_FILTER_COUNT];

#define CAN0_GPIO0_Low MCP251XFD_SetGPIOPinsOutputLevel(CAN0, GPIO0_LOW, GPIO0_Mask)
#define CAN0_GPIO0_High MCP251XFD_SetGPIOPinsOutputLevel(CAN0, GPIO0_HIGH, GPIO0_Mask)

#define CAN0_GPIO1_Low MCP251XFD_SetGPIOPinsOutputLevel(CAN0, GPIO1_LOW, GPIO1_Mask)
#define CAN0_GPIO1_High MCP251XFD_SetGPIOPinsOutputLevel(CAN0, GPIO1_HIGH, GPIO1_Mask)

//-----------------------------------------------------------------------------

//=============================================================================
// Configure the MCP251XFD device on CAN0
//=============================================================================
eERRORRESULT can0_function_device_init_can20(uint32_t nominal_bitrate, eMCP251XFD_OperationMode mode);

//=============================================================================
// Deinit the MCP251XFD device on CAN0
//=============================================================================
eERRORRESULT can0_function_device_deinit(void);

//=============================================================================
// Transmit messages to MCP251XFD device on CAN0 no irq
//=============================================================================
eERRORRESULT can0_function_transmit_msg_no_irq(
    uint32_t message_id,
    uint32_t* message_seq,
    setMCP251XFD_MessageCtrlFlags control_flags,
    eMCP251XFD_DataLength dlc,
    uint8_t* payload_data,
    bool flush);

//=============================================================================
// Transmit a message to MCP251XFD device on CAN0
//=============================================================================
eERRORRESULT can0_function_transmit_msg1(
    uint32_t message_id,
    uint32_t* message_seq,
    setMCP251XFD_MessageCtrlFlags control_flags,
    eMCP251XFD_DataLength dlc,
    uint8_t* payload_data,
    eMCP251XFD_FIFO to_fifo,
    bool flush);

eERRORRESULT can0_function_transmit_msg(MCP251XFD_CANMessage* can_msg, uint32_t* message_seq, eMCP251XFD_FIFO to_fifo, bool flush);

//=============================================================================
// Receive a message from MCP251XFD device on CAN0
//=============================================================================
eERRORRESULT can0_function_receive_msg(
    MCP251XFD* pComp,
    bool* receive_event,
    MCP251XFD_CANMessage* msg,
    eMCP251XFD_PayloadSize payload_size,
    uint32_t* msg_time_stamp);

//=============================================================================
// Check device interrupt (INT) on CAN0
//=============================================================================
void can0_function_device_check_irq(void);
