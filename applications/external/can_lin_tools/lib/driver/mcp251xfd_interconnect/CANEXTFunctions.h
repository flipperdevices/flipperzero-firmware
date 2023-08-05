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



#define TIMESTAMP_TICK_us       ( 25 )                                       // TimeStamp tick is 25µs
#define TIMESTAMP_TICK(sysclk)  ( ((sysclk) / 1000000) * TIMESTAMP_TICK_us )


//**********************************************************************************************************************************************************
// Structure of the MCP251XFD used in the EXT2 in the demo
//#define MCP251XFD_EXT2_OSCCLK  ( 40000000 ) // The MCP251XFD on the EXT2 interface is at 40MHz
#define MCP251XFD_EXT2_CLK  ( 40000000 ) // The MCP251XFD on the EXT2 interface is at 40MHz
extern MCP251XFD MCP251XFD_Ext2;
#define CANEXT2  &MCP251XFD_Ext2

// Structure of the MCP251XFD configuration used in the EXT2 in the demo
extern MCP251XFD_BitTimeStats MCP2517FD_Ext2_BTStats;
extern uint32_t SYSCLK_Ext2;
extern MCP251XFD_Config MCP2517FD_Ext2_Config;

// // Structure of the MCP251XFD FIFO configuration used in the EXT2 in the demo
// #define MCP2517FD_EXT2_FIFO_COUNT    MCP251XFD_FIFO_CONF_MAX // 33
// extern MCP251XFD_RAMInfos Ext2_TEF_RAMInfos;
// extern MCP251XFD_RAMInfos Ext2_TXQ_RAMInfos;
// extern MCP251XFD_FIFO MCP2517FD_Ext2_FIFOlist[MCP2517FD_EXT2_FIFO_COUNT];

// // Structure of the MCP251XFD filters configuration used in the EXT2 in the demo
// #define MCP2517FD_EXT2_FILTER_COUNT  13
// extern MCP251XFD_Filter MCP2517FD_Ext2_FilterList[MCP2517FD_EXT2_FILTER_COUNT];

#define MCP2517FD_EXT2_FIFO_COUNT 10
extern MCP251XFD_RAMInfos Ext2_TEF_RAMInfos;
extern MCP251XFD_RAMInfos Ext2_TXQ_RAMInfos;
extern MCP251XFD_FIFO MCP2517FD_Ext2_FIFOlist[MCP2517FD_EXT2_FIFO_COUNT];

#define MCP2517FD_EXT2_FILTER_COUNT 4
extern MCP251XFD_Filter MCP2517FD_Ext2_FilterList[MCP2517FD_EXT2_FILTER_COUNT];



// All SID/EID for transmit messages to EXT2
#define SID_DEMO_RTR_RESPONSE  0x000
#define SID_BUTTON1_EVENT      0x001
#define SID_MENU_EVENT         0x002
#define SID_EXT1_ERROR_EVENT   0x003
#define SID_EXT2_ERROR_EVENT   0x004

#define SID_EXT1_CLKO_EVENT    0x101
#define SID_EXT2_CLKO_EVENT    0x102

#define SID_0x000_to_0x1FF_MPS_EVENT  0x1FF
#define SID_0x200_to_0x3FF_MPS_EVENT  0x3FF
#define SID_0x400_to_0x5FF_MPS_EVENT  0x5FF
#define SID_0x600_to_0x7FF_MPS_EVENT  0x7FF
#define EID_0x00000_to_0x07FFF_MPS_EVENT  0x07FFF
#define EID_0x08000_to_0x0FFFF_MPS_EVENT  0x0FFFF
#define EID_0x10000_to_0x17FFF_MPS_EVENT  0x17FFF
#define EID_0x18000_to_0x1FFFF_MPS_EVENT  0x1FFFF
#define EID_0x20000_to_0x27FFF_MPS_EVENT  0x27FFF
#define EID_0x28000_to_0x2FFFF_MPS_EVENT  0x2FFFF
#define EID_0x30000_to_0x37FFF_MPS_EVENT  0x37FFF
#define EID_0x38000_to_0x3FFFF_MPS_EVENT  0x3FFFF
#define EID_ALL_EXT2_MESSAGES_MPS_EVENT   0x1FFFFFFF

//-----------------------------------------------------------------------------



#define EXT2_GPIO0_Low   MCP251XFD_SetGPIOPinsOutputLevel(CANEXT2, GPIO0_LOW , GPIO0_Mask)
#define EXT2_GPIO0_High  MCP251XFD_SetGPIOPinsOutputLevel(CANEXT2, GPIO0_HIGH, GPIO0_Mask)

#define EXT2_GPIO1_Low   MCP251XFD_SetGPIOPinsOutputLevel(CANEXT2, GPIO1_LOW , GPIO1_Mask)
#define EXT2_GPIO1_High  MCP251XFD_SetGPIOPinsOutputLevel(CANEXT2, GPIO1_HIGH, GPIO1_Mask)

//-----------------------------------------------------------------------------



// //=============================================================================
// // MCP251XFD_Ext2 INT interrupt handler
// //=============================================================================
// void MCP251XFD_Ext2_CLKO_Handler(uint32_t id, uint32_t mask);



//=============================================================================
// Configure the MCP251XFD device on EXT2
//=============================================================================
eERRORRESULT ConfigureMCP251XFDDeviceOnEXT2(void);

//=============================================================================
// Transmit messages to MCP251XFD device on EXT2 no irq
//=============================================================================
eERRORRESULT TransmitMessageFromEXT2_No_IRQ(uint32_t messageID, uint32_t* messageSEQ, setMCP251XFD_MessageCtrlFlags controlFlags,  eMCP251XFD_DataLength dlc, uint8_t* payloadData, bool flush);

//=============================================================================
// Transmit a message to MCP251XFD device on EXT2
//=============================================================================
eERRORRESULT TransmitMessageToEXT2(uint32_t messageID, uint32_t* messageSEQ, setMCP251XFD_MessageCtrlFlags controlFlags, eMCP251XFD_DataLength dlc, uint8_t* payloadData, eMCP251XFD_FIFO toFIFO, bool flush);



//=============================================================================
// Receive a message from MCP251XFD device on EXT2
//=============================================================================
eERRORRESULT ReceiveMessageFromEXT2(MCP251XFD *pComp, bool* receive_event, MCP251XFD_CANMessage *msg, eMCP251XFD_PayloadSize payload_size, uint32_t *msg_time_stamp);


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