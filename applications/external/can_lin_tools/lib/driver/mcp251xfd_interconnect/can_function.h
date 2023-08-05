/*******************************************************************************
  File name:    CANEXTFunctions.h
  Author:       FMA
  Version:      1.0
  Date (d/m/y): 29/04/2020
  Description:  CAN-EXT (MCP251XFD) functions for the DEMO

  History :
*******************************************************************************/
#ifndef CANEXTFUNCTIONS_H_
#define CANEXTFUNCTIONS_H_
//=============================================================================

//-----------------------------------------------------------------------------
/// @cond 0
/**INDENT-OFF**/
#include "stdio.h"
#include <stdarg.h>
#include "../mcp251xfd/MCP251XFD.h"
#include "../mcp251xfd/ErrorsDef.h"

#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond
//-----------------------------------------------------------------------------

#define TIMESTAMP_TICK_us (25) // TimeStamp tick is 25µs
#define TIMESTAMP_TICK(sysclk) (((sysclk) / 1000000) * TIMESTAMP_TICK_us)

#define CAN0_CLK (40000000) // The MCP251XFD on interface is at 40MHz
extern MCP251XFD can0;
#define CAN0 &can0

// Structure of the MCP251XFD configuration used in the EXT2 in the demo
extern MCP251XFD_BitTimeStats can0_bt_stats;
extern uint32_t can0_sysclk;
extern MCP251XFD_Config can0_config;

#define CAN0_FIFO_COUNT 10
extern MCP251XFD_RAMInfos can0_tef_ram_infos;
extern MCP251XFD_RAMInfos can0_txq_ram_infos;
extern MCP251XFD_FIFO can0_fifo_list[CAN0_FIFO_COUNT];

#define CAN0_FILTER_COUNT 4
extern MCP251XFD_Filter can_filter_list[CAN0_FILTER_COUNT];


#define EXT2_GPIO0_Low MCP251XFD_SetGPIOPinsOutputLevel(CANEXT2, GPIO0_LOW, GPIO0_Mask)
#define EXT2_GPIO0_High MCP251XFD_SetGPIOPinsOutputLevel(CANEXT2, GPIO0_HIGH, GPIO0_Mask)

#define EXT2_GPIO1_Low MCP251XFD_SetGPIOPinsOutputLevel(CANEXT2, GPIO1_LOW, GPIO1_Mask)
#define EXT2_GPIO1_High MCP251XFD_SetGPIOPinsOutputLevel(CANEXT2, GPIO1_HIGH, GPIO1_Mask)

//-----------------------------------------------------------------------------

//=============================================================================
// Configure the MCP251XFD device on EXT2
//=============================================================================
eERRORRESULT ConfigureMCP251XFDDeviceOnEXT2(void);

//=============================================================================
// Transmit messages to MCP251XFD device on EXT2 no irq
//=============================================================================
eERRORRESULT TransmitMessageFromEXT2_No_IRQ(
    uint32_t messageID,
    uint32_t* messageSEQ,
    setMCP251XFD_MessageCtrlFlags controlFlags,
    eMCP251XFD_DataLength dlc,
    uint8_t* payloadData,
    bool flush);

//=============================================================================
// Transmit a message to MCP251XFD device on EXT2
//=============================================================================
eERRORRESULT TransmitMessageToEXT2(
    uint32_t messageID,
    uint32_t* messageSEQ,
    setMCP251XFD_MessageCtrlFlags controlFlags,
    eMCP251XFD_DataLength dlc,
    uint8_t* payloadData,
    eMCP251XFD_FIFO toFIFO,
    bool flush);

//=============================================================================
// Receive a message from MCP251XFD device on EXT2
//=============================================================================
eERRORRESULT ReceiveMessageFromEXT2(
    MCP251XFD* pComp,
    bool* receive_event,
    MCP251XFD_CANMessage* msg,
    eMCP251XFD_PayloadSize payload_size,
    uint32_t* msg_time_stamp);

//=============================================================================
// Check device interrupt (INT) on EXT2
//=============================================================================
void CheckDeviceINTOnEXT2(void);

//-----------------------------------------------------------------------------
/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
//-----------------------------------------------------------------------------
#endif /* CANEXTFUNCTIONS_H_ */