#include "can_function.h"
#include "can_interface_sync.h"
#include "mcp251xfd_spi.h"
#include "mpc251xfd_user_driver_data.h"

#define TAG "CANEXTFunctions"

// //! Enum Ext2 counters
// typedef enum
// {
//   SID_0x000_to_0x1FF = 0,
//   SID_0x200_to_0x3FF,
//   SID_0x400_to_0x5FF,
//   SID_0x600_to_0x7FF,
//   EID_0x00000_to_0x07FFF,
//   EID_0x08000_to_0x0FFFF,
//   EID_0x10000_to_0x17FFF,
//   EID_0x18000_to_0x1FFFF,
//   EID_0x20000_to_0x27FFF,
//   EID_0x28000_to_0x2FFFF,
//   EID_0x30000_to_0x37FFF,
//   EID_0x38000_to_0x3FFFF,
//   EXT2_ALL,
//   EXT2_COUNTERS_COUNT     // Stay last
// } eExt2Counters;

// typedef union Ext2MessagesStats
// {
//   uint8_t Bytes[8];
//   struct
//   {
//     uint32_t MessagesPerSecond;
//     uint32_t ByteCount;
//   };
// } Ext2MessagesStats;

//bool Ext2ModulePresent = false;                   //! True if a MCP251XFD is present on EXT2 and configured
//Ext2MessagesStats Ext2Stats[EXT2_COUNTERS_COUNT]; //! Ext2 message counter for the MPS (Messages Per Second) and byte count
//uint32_t Ext2SequenceCounter = 0;                 //! Ext2 sequence counter

//=============================================================================
// SID/EID to counter index
//=============================================================================
// static eExt2Counters SIDEIDtoCounterIndex(uint32_t sideid)
// {
//   if (sideid <   0x200) return SID_0x000_to_0x1FF;
//   if (sideid <   0x400) return SID_0x200_to_0x3FF;
//   if (sideid <   0x600) return SID_0x400_to_0x5FF;
//   if (sideid <   0x800) return SID_0x600_to_0x7FF;
//   if (sideid < 0x08000) return EID_0x00000_to_0x07FFF;
//   if (sideid < 0x10000) return EID_0x08000_to_0x0FFFF;
//   if (sideid < 0x18000) return EID_0x10000_to_0x17FFF;
//   if (sideid < 0x20000) return EID_0x18000_to_0x1FFFF;
//   if (sideid < 0x28000) return EID_0x20000_to_0x27FFF;
//   if (sideid < 0x30000) return EID_0x28000_to_0x2FFFF;
//   if (sideid < 0x38000) return EID_0x30000_to_0x37FFF;
//   return EID_0x38000_to_0x3FFFF;
// }
//**********************************************************************************************************************************************************

const char OpModeStr[8][25 + 1 /* \0 */] = {
    "normal CAN FD mode",
    "sleep mode",
    "internal loopback mode",
    "listen only mode",
    "configuration mode",
    "external loopback mode",
    "normal CAN 2.0 mode",
    "restricted operation mode",
};

const char* AutoRTRmessage = "DEMO";
// //-----------------------------------------------------------------------------

//=============================================================================
// All configuration structure of the MCP251XFD on the Ext1
//=============================================================================
// MCP251XFD MCP251XFD_Ext1 =
// {
//   .UserDriverData = NULL,
//   //--- Driver configuration ---
//   .DriverConfig   = MCP251XFD_DRIVER_NORMAL_USE
//                   | MCP251XFD_DRIVER_SAFE_RESET
//                   | MCP251XFD_DRIVER_USE_READ_WRITE_CRC
//                   | MCP251XFD_DRIVER_INIT_SET_RAM_AT_0
//                   | MCP251XFD_DRIVER_CLEAR_BUFFER_BEFORE_READ,
//   //--- IO configuration ---
//   .GPIOsOutState   = MCP251XFD_GPIO0_LOW | MCP251XFD_GPIO1_HIGH,
//   //--- Interface driver call functions ---
//   .SPI_ChipSelect  = MCP252XFD_SPI_CS_EXT1,
//   .InterfaceDevice = &mcp251xfd_spi_bus_handle_external,
//   .fnSPI_Init      = MCP251XFD_InterfaceInit,
//   .fnSPI_Transfer  = MCP251XFD_InterfaceTransfer,
//   //--- Time call function ---
//   .fnGetCurrentms  = GetCurrentms,
//   //--- CRC16-USB call function ---
//   .fnComputeCRC16  = ComputeCRC16,
//   //--- Interface clocks ---
//   .SPIClockSpeed   = 2000000, // 20MHz
// };

//=============================================================================
// All configuration structure of the MCP251XFD on the Ext2
//=============================================================================
MCP251XFD_BitTimeStats can0_bt_stats;
uint32_t can0_sysclk;

MCP251XFD_Config can0_config = {
    //--- Controller clocks ---
    .XtalFreq = CAN0_CLK, // CLKIN is an crystal
    .OscFreq = 0, // CLKIN is not a oscillator
    .SysclkConfig = MCP251XFD_SYSCLK_IS_CLKIN,
    .ClkoPinConfig = MCP251XFD_CLKO_DivBy10,
    .SYSCLK_Result = &can0_sysclk,
    //--- CAN configuration ---
    .NominalBitrate = 500000, // Nominal Bitrate to 0.5Mbps
    .DataBitrate = MCP251XFD_NO_CANFD, // No CAN-FD
    .BitTimeStats = &can0_bt_stats,
    .Bandwidth = MCP251XFD_DELAY_4BIT_TIMES,
    .ControlFlags = MCP251XFD_CAN_UNLIMITED_RETRANS_ATTEMPTS,
    //.ControlFlags   = MCP251XFD_CAN_RESTRICTED_RETRANS_ATTEMPTS, // | MCP251XFD_CAN_LISTEN_ONLY_MODE_ON_ERROR,// Restricted retransmission attempts, MCP251XFD_FIFO.Attempts (CiFIFOCONm.TXAT) is used
    //--- GPIOs and Interrupts pins ---
    .GPIO0PinMode =
        MCP251XFD_PIN_AS_GPIO0_IN, // Cannot be PIN_AS_INT0_TX, PIN_AS_GPIO0_OUT, or PIN_AS_XSTBY in this demo else there is no CONSOLE
    .GPIO1PinMode =
        MCP251XFD_PIN_AS_GPIO1_IN, // Cannot be PIN_AS_INT1_TX, PIN_AS_GPIO1_OUT, or PIN_AS_XSTBY in this demo else there is no CONSOLE
    .INTsOutMode = MCP251XFD_PINS_PUSHPULL_OUT,
    .TXCANOutMode = MCP251XFD_PINS_PUSHPULL_OUT,
    //--- Interrupts ---
    .SysInterruptFlags = MCP251XFD_INT_ENABLE_ALL_EVENTS,
};

const MCP251XFD_UserDriverData can0_user_driver_data = {
    .config = &can0_config,
    .can_irq = CAN0_PIN_IRQ,
    .can_gpio0 = NULL,
    .can_gpio1 = NULL,
};

MCP251XFD can0 = {
    .UserDriverData = (void*)&can0_user_driver_data,
    //--- Driver configuration ---
    .DriverConfig = MCP251XFD_DRIVER_NORMAL_USE | MCP251XFD_DRIVER_SAFE_RESET,
    //--- IO configuration ---
    .GPIOsOutState = MCP251XFD_GPIO0_LOW | MCP251XFD_GPIO1_LOW,
    //--- Interface driver call functions ---
    .SPI_ChipSelect = CAN0_SPI_CS,
    .InterfaceDevice = &mcp251xfd_spi_bus_handle_external,
    .fnSPI_Init = can_interface_sync_init,
    .fnSPI_Transfer = can_interface_sync_transfer,
    //--- Time call function ---
    .fnGetCurrentms = can_interface_sync_get_currentms,
    //--- CRC16-USB call function ---
    .fnComputeCRC16 = can_interface_sync_compute_crc16,
    //--- Interface clocks ---
    .SPIClockSpeed = 4000000, // 10MHz
};


// MCP251XFD_RAMInfos Ext2_TEF_RAMInfos;
// MCP251XFD_RAMInfos Ext2_TXQ_RAMInfos;
// MCP251XFD_RAMInfos Ext2_FIFOs_RAMInfos[MCP2517FD_EXT2_FIFO_COUNT - 2];

// MCP251XFD_FIFO MCP2517FD_Ext2_FIFOlist[MCP2517FD_EXT2_FIFO_COUNT] =
// {
//   { .Name = MCP251XFD_TEF   , .Size = MCP251XFD_FIFO_20_MESSAGE_DEEP,                                                                                                                                                              .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_OBJ, .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_EVENT_FIFO_NOT_EMPTY_INT               , .RAMInfos = &Ext2_TEF_RAMInfos      , },
//   { .Name = MCP251XFD_TXQ   , .Size = MCP251XFD_FIFO_12_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_8BYTE,                                       .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_TXQ_RAMInfos      , }, // Buttons + Events menu

//   { .Name = MCP251XFD_FIFO1 , .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[ 0], }, // SID: 0x000..0x1FF ; No EID
//   { .Name = MCP251XFD_FIFO2 , .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[ 1], }, // SID: 0x200..0x3FF ; No EID
//   { .Name = MCP251XFD_FIFO3 , .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[ 2], }, // SID: 0x400..0x5FF ; No EID
//   { .Name = MCP251XFD_FIFO4 , .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[ 3], }, // SID: 0x600..0x7FF ; No EID
//   { .Name = MCP251XFD_FIFO5 , .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[ 4], }, // FPS + Byte Count per second FIFO1
//   { .Name = MCP251XFD_FIFO6 , .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[ 5], }, // FPS + Byte Count per second FIFO2
//   { .Name = MCP251XFD_FIFO7 , .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[ 6], }, // FPS + Byte Count per second FIFO3
//   { .Name = MCP251XFD_FIFO8 , .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[ 7], }, // FPS + Byte Count per second FIFO4

//   { .Name = MCP251XFD_FIFO9 , .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[ 8], }, // EID: 0x00000..0x07FFF ; All SID
//   { .Name = MCP251XFD_FIFO10, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[ 9], }, // EID: 0x08000..0x0FFFF ; All SID
//   { .Name = MCP251XFD_FIFO11, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[10], }, // EID: 0x10000..0x17FFF ; All SID
//   { .Name = MCP251XFD_FIFO12, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[11], }, // EID: 0x18000..0x1FFFF ; All SID
//   { .Name = MCP251XFD_FIFO13, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[12], }, // EID: 0x20000..0x27FFF ; All SID
//   { .Name = MCP251XFD_FIFO14, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[13], }, // EID: 0x28000..0x2FFFF ; All SID
//   { .Name = MCP251XFD_FIFO15, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[14], }, // EID: 0x30000..0x37FFF ; All SID
//   { .Name = MCP251XFD_FIFO16, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO ,                                                                                    .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX , .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT             , .RAMInfos = &Ext2_FIFOs_RAMInfos[15], }, // EID: 0x38000..0x3FFFF ; All SID
//   { .Name = MCP251XFD_FIFO17, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[16], }, // FPS + Byte Count per second FIFO9
//   { .Name = MCP251XFD_FIFO18, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[17], }, // FPS + Byte Count per second FIFO10
//   { .Name = MCP251XFD_FIFO19, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[18], }, // FPS + Byte Count per second FIFO11
//   { .Name = MCP251XFD_FIFO20, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[19], }, // FPS + Byte Count per second FIFO12
//   { .Name = MCP251XFD_FIFO21, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[20], }, // FPS + Byte Count per second FIFO13
//   { .Name = MCP251XFD_FIFO22, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[21], }, // FPS + Byte Count per second FIFO14
//   { .Name = MCP251XFD_FIFO23, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[22], }, // FPS + Byte Count per second FIFO15
//   { .Name = MCP251XFD_FIFO24, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[23], }, // FPS + Byte Count per second FIFO16

//   { .Name = MCP251XFD_FIFO25, .Size = MCP251XFD_FIFO_1_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY30, .ControlFlags = MCP251XFD_FIFO_AUTO_RTR_RESPONSE   , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT, .RAMInfos = &Ext2_FIFOs_RAMInfos[24], }, // Response for RTR
//   { .Name = MCP251XFD_FIFO26, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[25], }, // SOF-CLKO Ext1 - Counts per second
//   { .Name = MCP251XFD_FIFO27, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[26], }, // SOF-CLKO Ext2 - Counts per second
//   { .Name = MCP251XFD_FIFO28, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[27], }, // Ext1: FPS + Data count per second
//   { .Name = MCP251XFD_FIFO29, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY20, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[28], }, // Ext1: Error Message
//   { .Name = MCP251XFD_FIFO30, .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[29], }, // Ext2: FPS + Data count per second
//   { .Name = MCP251XFD_FIFO31, .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP , .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_TRANSMIT_FIFO, .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY20, .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE     , .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT                                            , .RAMInfos = &Ext2_FIFOs_RAMInfos[30], }, // Ext2: Error Message
// };

// MCP251XFD_Filter MCP2517FD_Ext2_FilterList[MCP2517FD_EXT2_FILTER_COUNT] =
// {
//   { .Filter = MCP251XFD_FILTER0 , .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_SID, .AcceptanceID = 0x000, .AcceptanceMask = 0x7FF, .PointTo = MCP251XFD_FIFO25, }, // 0x000 for RTR response

//   { .Filter = MCP251XFD_FILTER1 , .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_SID, .AcceptanceID = 0x100, .AcceptanceMask = 0x700, .PointTo = MCP251XFD_FIFO1, }, // 0x100..0x1FF
//   { .Filter = MCP251XFD_FILTER2 , .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_SID, .AcceptanceID = 0x200, .AcceptanceMask = 0x600, .PointTo = MCP251XFD_FIFO2, }, // 0x200..0x3FF
//   { .Filter = MCP251XFD_FILTER3 , .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_SID, .AcceptanceID = 0x400, .AcceptanceMask = 0x600, .PointTo = MCP251XFD_FIFO3, }, // 0x400..0x5FF
//   { .Filter = MCP251XFD_FILTER4 , .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_SID, .AcceptanceID = 0x600, .AcceptanceMask = 0x600, .PointTo = MCP251XFD_FIFO4, }, // 0x600..0x7FF

//   { .Filter = MCP251XFD_FILTER5 , .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_EID, .AcceptanceID = 0x00000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO9 , }, // 0x00000000..0x1C000000
//   { .Filter = MCP251XFD_FILTER6 , .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_EID, .AcceptanceID = 0x04000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO10, }, // 0x04000000..0x1C000000
//   { .Filter = MCP251XFD_FILTER7 , .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_EID, .AcceptanceID = 0x08000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO11, }, // 0x08000000..0x1C000000
//   { .Filter = MCP251XFD_FILTER8 , .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_EID, .AcceptanceID = 0x0C000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO12, }, // 0x0C000000..0x1C000000
//   { .Filter = MCP251XFD_FILTER9 , .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_EID, .AcceptanceID = 0x10000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO13, }, // 0x10000000..0x1C000000
//   { .Filter = MCP251XFD_FILTER10, .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_EID, .AcceptanceID = 0x14000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO14, }, // 0x14000000..0x1C000000
//   { .Filter = MCP251XFD_FILTER11, .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_EID, .AcceptanceID = 0x18000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO15, }, // 0x18000000..0x1C000000
//   { .Filter = MCP251XFD_FILTER12, .EnableFilter = true, .Match = MCP251XFD_MATCH_ONLY_EID, .AcceptanceID = 0x1C000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO16, }, // 0x1C000000..0x1C000000
// };

MCP251XFD_RAMInfos can0_tef_ram_infos;
MCP251XFD_RAMInfos can0_txq_ram_infos;
MCP251XFD_RAMInfos can_fifo_ram_infos[CAN0_FIFO_COUNT - 2];
MCP251XFD_FIFO can0_fifo_list[CAN0_FIFO_COUNT] = {
    {
        .Name = MCP251XFD_TEF,
        .Size = MCP251XFD_FIFO_10_MESSAGE_DEEP,
        .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_OBJ,
        .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_EVENT_FIFO_NOT_EMPTY_INT,
        .RAMInfos = &can0_tef_ram_infos,
    },
    {
        .Name = MCP251XFD_TXQ,
        .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP,
        .Payload = MCP251XFD_PAYLOAD_64BYTE,
        .Attempts = MCP251XFD_THREE_ATTEMPTS,
        .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,
        .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,
        .InterruptFlags =
            MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT,
        .RAMInfos = &can0_txq_ram_infos,
    },
    {
        .Name = MCP251XFD_FIFO1,
        .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP,
        .Payload = MCP251XFD_PAYLOAD_64BYTE,
        .Direction = MCP251XFD_RECEIVE_FIFO,
        .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,
        .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,
        .RAMInfos = &can_fifo_ram_infos[0],
    }, // SID: 0x000..0x1FF ; No EID
    {
        .Name = MCP251XFD_FIFO2,
        .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,
        .Payload = MCP251XFD_PAYLOAD_64BYTE,
        .Direction = MCP251XFD_RECEIVE_FIFO,
        .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,
        .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,
        .RAMInfos = &can_fifo_ram_infos[1],
    }, // SID: 0x200..0x3FF ; No EID
    {
        .Name = MCP251XFD_FIFO3,
        .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP,
        .Payload = MCP251XFD_PAYLOAD_64BYTE,
        .Direction = MCP251XFD_RECEIVE_FIFO,
        .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,
        .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,
        .RAMInfos = &can_fifo_ram_infos[2],
    }, // SID: 0x400..0x5FF ; No EID
    {
        .Name = MCP251XFD_FIFO4,
        .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,
        .Payload = MCP251XFD_PAYLOAD_64BYTE,
        .Direction = MCP251XFD_RECEIVE_FIFO,
        .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,
        .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,
        .RAMInfos = &can_fifo_ram_infos[3],
    }, // SID: 0x600..0x7FF ; No EID
    {
        .Name = MCP251XFD_FIFO5,
        .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP,
        .Payload = MCP251XFD_PAYLOAD_64BYTE,
        .Direction = MCP251XFD_TRANSMIT_FIFO,
        .Attempts = MCP251XFD_THREE_ATTEMPTS,
        .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,
        .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,
        .InterruptFlags =
            MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT,
        .RAMInfos = &can_fifo_ram_infos[4],
    },
    {
        .Name = MCP251XFD_FIFO6,
        .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,
        .Payload = MCP251XFD_PAYLOAD_64BYTE,
        .Direction = MCP251XFD_TRANSMIT_FIFO,
        .Attempts = MCP251XFD_THREE_ATTEMPTS,
        .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,
        .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,
        .InterruptFlags =
            MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT,
        .RAMInfos = &can_fifo_ram_infos[5],
    },
    {
        .Name = MCP251XFD_FIFO7,
        .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,
        .Payload = MCP251XFD_PAYLOAD_64BYTE,
        .Direction = MCP251XFD_TRANSMIT_FIFO,
        .Attempts = MCP251XFD_THREE_ATTEMPTS,
        .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,
        .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,
        .InterruptFlags =
            MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT,
        .RAMInfos = &can_fifo_ram_infos[6],
    },
    {
        .Name = MCP251XFD_FIFO8,
        .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,
        .Payload = MCP251XFD_PAYLOAD_64BYTE,
        .Direction = MCP251XFD_TRANSMIT_FIFO,
        .Attempts = MCP251XFD_THREE_ATTEMPTS,
        .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,
        .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,
        .InterruptFlags =
            MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT,
        .RAMInfos = &can_fifo_ram_infos[7],
    },
};

MCP251XFD_Filter can_filter_list[CAN0_FILTER_COUNT] = {
    {
        .Filter = MCP251XFD_FILTER0,
        .EnableFilter = true,
        .Match = MCP251XFD_MATCH_ONLY_SID,
        .AcceptanceID = 0x000,
        .AcceptanceMask = 0x600,
        .PointTo = MCP251XFD_FIFO1,
    }, // 0x000..0x1FF
    {
        .Filter = MCP251XFD_FILTER1,
        .EnableFilter = true,
        .Match = MCP251XFD_MATCH_ONLY_SID,
        .AcceptanceID = 0x200,
        .AcceptanceMask = 0x600,
        .PointTo = MCP251XFD_FIFO2,
    }, // 0x200..0x3FF
    {
        .Filter = MCP251XFD_FILTER2,
        .EnableFilter = true,
        .Match = MCP251XFD_MATCH_ONLY_SID,
        .AcceptanceID = 0x400,
        .AcceptanceMask = 0x600,
        .PointTo = MCP251XFD_FIFO3,
    }, // 0x400..0x5FF
    {
        .Filter = MCP251XFD_FILTER3,
        .EnableFilter = true,
        .Match = MCP251XFD_MATCH_ONLY_SID,
        .AcceptanceID = 0x600,
        .AcceptanceMask = 0x600,
        .PointTo = MCP251XFD_FIFO4,
    }, // 0x600..0x7FF
};

static bool TransmitEvent = false;
static bool ReceiveEvent = false;

//**********************************************************************************************************************************************************
//=============================================================================
// Configure the MCP251XFD device on EXT2
//=============================================================================
eERRORRESULT ConfigureMCP251XFDDeviceOnEXT2(void) {

    //--- Initialize Int pins or GPIOs ---
    can_interface_sync_pin_init(CAN0);
    can_interface_sync_int0_gpio0_pin_init(CAN0);
    can_interface_sync_int1_gpio1_pin_init(CAN0);

    // //--- Configure CLKO pin from EXT2 as input ---
    // ioport_set_pin_dir(EXT2_PIN_7, IOPORT_DIR_INPUT);
    // ioport_set_pin_mode(EXT2_PIN_7, IOPORT_MODE_PULLUP);
    // ioport_set_pin_sense_mode(EXT2_PIN_7, IOPORT_SENSE_FALLING);
    // // Enable interrupt
    // if (pio_handler_set_pin(EXT2_PIN_7, PIO_IT_FALL_EDGE, &MCP251XFD_CLKO_Handler) != 0) return ERR__CONFIGURATION;
    // pio_enable_pin_interrupt(EXT2_PIN_7);

    //--- Configure module on Ext2 ---
    eERRORRESULT ErrorExt2 = ERR__NO_DEVICE_DETECTED;
    ErrorExt2 = Init_MCP251XFD(CAN0, &can0_config);
    if(ErrorExt2 == ERR_OK) {
        ErrorExt2 = MCP251XFD_ConfigureTimeStamp(
            CAN0, true, MCP251XFD_TS_CAN20_SOF_CANFD_SOF, TIMESTAMP_TICK(can0_sysclk), true);
        if(ErrorExt2 == ERR_OK)
            ErrorExt2 = MCP251XFD_ConfigureFIFOList(CAN0, &can0_fifo_list[0], CAN0_FIFO_COUNT);
        if(ErrorExt2 == ERR_OK)
            ErrorExt2 = MCP251XFD_ConfigureFilterList(
                CAN0, MCP251XFD_D_NET_FILTER_DISABLE, &can_filter_list[0], CAN0_FILTER_COUNT);
        if(ErrorExt2 == ERR_OK) ErrorExt2 = MCP251XFD_StartCAN20(CAN0);
        //ErrorExt2 = MCP251XFD_RequestOperationMode(CANEXT2, MCP251XFD_NORMAL_CAN20_MODE, false);
        //ErrorExt2 = MCP251XFD_RequestOperationMode(CANEXT2, MCP251XFD_INTERNAL_LOOPBACK_MODE, false);
        //ErrorExt2 = MCP251XFD_RequestOperationMode(CANEXT2, MCP251XFD_EXTERNAL_LOOPBACK_MODE, false);
        //ErrorExt2 = MCP251XFD_RequestOperationMode(CANEXT2, MCP251XFD_LISTEN_ONLY_MODE, false);
    }
    return ErrorExt2;
}

//=============================================================================
// Transmit messages to MCP251XFD device on EXT2 no irq
//=============================================================================
eERRORRESULT TransmitMessageFromEXT2_No_IRQ(
    uint32_t messageID,
    uint32_t* messageSEQ,
    setMCP251XFD_MessageCtrlFlags controlFlags,
    eMCP251XFD_DataLength dlc,
    uint8_t* payloadData,
    bool flush) {
    eERRORRESULT ErrorExt2 = ERR_OK;
    eMCP251XFD_FIFOstatus FIFOstatus = 0;
    setMCP251XFD_InterruptOnFIFO InterruptOnFIFO = 0;
    ErrorExt2 = MCP251XFD_GetTransmitPendingInterruptStatusOfAllFIFO(
        CAN0, &InterruptOnFIFO); // Get all FIFO status
    if(ErrorExt2 != ERR_OK) return ErrorExt2;
    for(eMCP251XFD_FIFO zFIFO = 0; zFIFO < MCP251XFD_TX_FIFO_MAX;
        zFIFO++) // For each transmit FIFO, TXQ but not TEF
        if((InterruptOnFIFO & (1 << zFIFO)) > 0) // If an Interrupt is flagged
        {
            ErrorExt2 = MCP251XFD_GetFIFOStatus(
                CAN0, zFIFO, &FIFOstatus); // Get the status of the flagged FIFO
            if(ErrorExt2 != ERR_OK) return ErrorExt2;
            if((FIFOstatus & MCP251XFD_TX_FIFO_NOT_FULL) > 0) // Check FIFO not empty
            {
                eMCP251XFD_Devices Dev = MCP2517FD;
                MCP251XFD_GetDeviceID(CAN0, &Dev, NULL, NULL);
                uint32_t MaxSequence = MCP2518FD_SEQUENCE_MAX;
                if(Dev == MCP2517FD) MaxSequence = MCP2517FD_SEQUENCE_MAX;

                MCP251XFD_CANMessage TansmitMessage;
                //***** Fill the message as you want *****
                TansmitMessage.MessageID = messageID;
                TansmitMessage.MessageSEQ = *messageSEQ;
                (*messageSEQ)++;
                if(*messageSEQ > MaxSequence)
                    *messageSEQ =
                        0; // Roll over the sequence value (maximum differ if it's a MCP2517FD or a MCP2518FD)
                TansmitMessage.ControlFlags = controlFlags;
                TansmitMessage.DLC = dlc;
                TansmitMessage.PayloadData = payloadData;
                ErrorExt2 = MCP251XFD_TransmitMessageToFIFO(
                    CAN0, &TansmitMessage, zFIFO, flush); // Send message and flush
            }
        }
    return ErrorExt2;
}

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
    bool flush) {
    // if (Ext2ModulePresent == false) return ERR_OK;
    TransmitEvent = false;
    MCP251XFD_CANMessage CanMessage;

    eMCP251XFD_Devices Dev = MCP2517FD;
    MCP251XFD_GetDeviceID(CAN0, &Dev, NULL, NULL);
    uint32_t MaxSequence = MCP2518FD_SEQUENCE_MAX;
    if(Dev == MCP2517FD) MaxSequence = MCP2517FD_SEQUENCE_MAX;

    //--- Create message ---
    CanMessage.MessageID = messageID;
    CanMessage.MessageSEQ = *messageSEQ;
    (*messageSEQ)++;
    if(*messageSEQ > MaxSequence)
        *messageSEQ =
            0; // Roll over the sequence value (maximum differ if it's a MCP2517FD or a MCP2518FD)
    CanMessage.ControlFlags = controlFlags;
    CanMessage.DLC = dlc;
    CanMessage.PayloadData = payloadData;
    return MCP251XFD_TransmitMessageToFIFO(CAN0, &CanMessage, toFIFO, flush);
}

//=============================================================================
// Receive a message from MCP251XFD device on EXT2
//=============================================================================
eERRORRESULT ReceiveMessageFromEXT2(
    MCP251XFD* pComp,
    bool* receive_event,
    MCP251XFD_CANMessage* msg,
    eMCP251XFD_PayloadSize payload_size,
    uint32_t* msg_time_stamp) {
    //if (Ext2ModulePresent == false) return ERR_OK;
    eERRORRESULT ErrorExt2 = ERR_OK;

#ifdef APP_USE_EXT2_INT1_PIN
    eMCP251XFD_FIFO FIFOname;
    eMCP251XFD_FIFOstatus FIFOstatus = 0;
    if((ioport_get_pin_level(EXT2_PIN_14) == 0) ||
       (ReceiveEvent ==
        true)) // Check INT1 pin status of the MCP251XFD (Active low state), this pin is configured for all receive FIFO not empty
    {
        ErrorExt2 =
            MCP251XFD_GetCurrentReceiveFIFONameAndStatusInterrupt(pComp, &FIFOname, &FIFOstatus);
        if(ErrorExt2 != ERR_OK)
            return ErrorExt2; // First get which FIFO set interrupt and its status
        if(((FIFOstatus & MCP251XFD_RX_FIFO_NOT_EMPTY) > 0) &&
           (FIFOname != MCP251XFD_NO_FIFO)) // Second check FIFO not empty
#else
    eMCP251XFD_FIFOstatus FIFOstatus = 0;
    setMCP251XFD_InterruptOnFIFO InterruptOnFIFO = 0;
    ErrorExt2 = MCP251XFD_GetReceivePendingInterruptStatusOfAllFIFO(
        pComp, &InterruptOnFIFO); // Get all FIFO status
    if(ErrorExt2 != ERR_OK) return ErrorExt2;
    for(eMCP251XFD_FIFO FIFOname = 1; FIFOname < MCP251XFD_FIFO_MAX;
        FIFOname++) // For each receive FIFO but not TEF, TXQ
        if((InterruptOnFIFO & (1 << FIFOname)) > 0) // If an Interrupt is flagged
        {
            ErrorExt2 = MCP251XFD_GetFIFOStatus(
                pComp, FIFOname, &FIFOstatus); // Get the status of the flagged FIFO
            if(ErrorExt2 != ERR_OK) return ErrorExt2;
            if((FIFOstatus & MCP251XFD_RX_FIFO_NOT_EMPTY) >
               0) // Check FIFO not empty and no error while retrieve FIFO status
#endif

        {
            // uint32_t MessageTimeStamp = 0;
            // uint8_t RxPayloadData[8];                                                            // In the default demo configuration, all the FIFO have 8 bytes of payload
            // MCP251XFD_CANMessage ReceivedMessage;
            // ReceivedMessage.PayloadData = &RxPayloadData[0];                                     // Add receive payload data pointer to the message structure that will be received
            //ErrorExt2 = MCP251XFD_ReceiveMessageFromFIFO(CANEXT2, &ReceivedMessage, MCP251XFD_PAYLOAD_8BYTE, &MessageTimeStamp, FIFOname);
            // if (ErrorExt2 == ERR_OK)
            // {
            //   //***** Do what you want with the message *****

            //   //*** Processing for the demo ***
            //   eExt2Counters Counter = SIDEIDtoCounterIndex(ReceivedMessage.MessageID);
            //   Ext2Stats[Counter].MessagesPerSecond++;
            //   Ext2Stats[Counter].ByteCount += MCP251XFD_DLCToByte(ReceivedMessage.DLC, false);

            //   Ext2Stats[EXT2_ALL].MessagesPerSecond++;
            //   Ext2Stats[EXT2_ALL].ByteCount += MCP251XFD_DLCToByte(ReceivedMessage.DLC, false);
            // }
            ErrorExt2 = MCP251XFD_ReceiveMessageFromFIFO(
                pComp, msg, payload_size, msg_time_stamp, FIFOname);
            *receive_event = true;
        }
    }
    //*receive_event = false;
    return ErrorExt2;
}

//=============================================================================
// Check device interrupt (INT) on EXT2
//=============================================================================
void CheckDeviceINTOnEXT2(void) {
    //if (Ext2ModulePresent == false) return;
    eERRORRESULT ErrorExt2;
    setMCP251XFD_CRCEvents CRCEvent;
    eMCP251XFD_OperationMode OpMode;
    setMCP251XFD_InterruptEvents Events;
    setMCP251XFD_ECCEvents ECCEvent;

#ifdef APP_USE_EXT2_INT_PIN
    if(ioport_get_pin_level(EXT2_PIN_IRQ) != 0)
        return; // Check INT pin status of the MCP251XFD (Active low state)
#endif
    bool TransmitError = true;
    setMCP251XFD_InterruptOnFIFO InterruptOnFIFO = 0;
    eMCP251XFD_InterruptFlagCode InterruptCode = 0;
    ErrorExt2 = MCP251XFD_GetCurrentInterruptEvent(
        CAN0, &InterruptCode); // Get the current Interrupt event
    if(ErrorExt2 == ERR_OK) // If no errors, process
    {
        switch(InterruptCode) {
        case MCP251XFD_ERROR_INTERRUPT: // CAN Bus Error Interrupt
            FURI_LOG_E(TAG, "Ext2: CAN Bus Error");
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_BUS_ERROR_EVENT);
            break;

        case MCP251XFD_WAKEUP_INTERRUPT: // Wake-up interrupt
            // No sleep defined for this one
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_BUS_WAKEUP_EVENT);
            break;

        case MCP251XFD_RECEIVE_FIFO_OVF: // Receive FIFO Overflow Interrupt
            MCP251XFD_GetReceiveOverflowInterruptStatusOfAllFIFO(CAN0, &InterruptOnFIFO);
            for(eMCP251XFD_FIFO zFIFO = 1; zFIFO < MCP251XFD_RX_FIFO_MAX;
                zFIFO++) // For each receive FIFO but not the TEF
                if((InterruptOnFIFO & (1 << zFIFO)) > 0) // If the Overflow Interrupt is flagged
                {
                    FURI_LOG_E(
                        TAG,
                        "Ext2: Receive FIFO%u Overflow",
                        (unsigned int)zFIFO); // Display an error for this FIFO
                    MCP251XFD_ClearFIFOOverflowEvent(CAN0, zFIFO); // Clear the flag of this FIFO
                }
            break;

        case MCP251XFD_TRANSMIT_ATTEMPT: // Transmit Attempt Interrupt
            MCP251XFD_GetTransmitAttemptInterruptStatusOfAllFIFO(CAN0, &InterruptOnFIFO);
            for(eMCP251XFD_FIFO zFIFO = 0; zFIFO < MCP251XFD_TX_FIFO_MAX;
                zFIFO++) // For each transmit FIFO
                if((InterruptOnFIFO & (1 << zFIFO)) > 0) // If the Attempt Interrupt is flagged
                {
                    FURI_LOG_E(
                        TAG,
                        "Ext2: Transmit FIFO%u Attempts Exhaust",
                        (unsigned int)zFIFO); // Display an error for this FIFO
                    MCP251XFD_ClearFIFOAttemptsEvent(CAN0, zFIFO); // Clear the flag of this FIFO
                }
            break;

        case MCP251XFD_ADDRESS_ERROR_INTERRUPT: // Address Error Interrupt (illegal FIFO address presented to system)
            FURI_LOG_E(TAG, "Ext2: Illegal FIFO address presented to system");
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_SYSTEM_ERROR_EVENT);
            break;

        case MCP251XFD_RXTX_MAB_OVF_UVF: // RX MAB Overflow (RX: message received before previous message was saved to memory)
            FURI_LOG_E(
                TAG,
                "Ext2: RX MAB Overflow (RX: message received before previous message was saved to memory)");
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_SYSTEM_ERROR_EVENT);
            break;

        case MCP251XFD_TBC_OVF_INTERRUPT: // TBC Overflow

            // The time base for the timestamp overflow (32-bits), update the associated time

            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_TIME_BASE_COUNTER_EVENT);
            break;

        case MCP251XFD_OPMODE_CHANGE_OCCURED: // Operation Mode Change Occurred
            MCP251XFD_GetActualOperationMode(CAN0, &OpMode);
            FURI_LOG_I(TAG, "Ext2: Operation mode change to %s", OpModeStr[(size_t)OpMode]);
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_OPERATION_MODE_CHANGE_EVENT);
            break;

        case MCP251XFD_INVALID_MESSAGE_OCCURED: // Invalid Message Occurred
            FURI_LOG_W(TAG, "Ext2: Invalid Message Occurred");
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_RX_INVALID_MESSAGE_EVENT);
            break;

        case MCP251XFD_TRANSMIT_EVENT_FIFO: // Transmit Event FIFO Interrupt

            //*** Do something with the TEF object ***

            MCP251XFD_UpdateTEF(CAN0);
            TransmitError = false;
            return;

        case MCP251XFD_FIFO25_INTERRUPT:
            // FURI_LOG_I(TAG,"MCP251XFD_FIFO25_INTERRUPT");
            // //MCP251XFD_ClearInterruptEvents(CANEXT2, MCP251XFD_FIFO25_INTERRUPT);
            // //TransmitMessageToEXT2(SID_DEMO_RTR_RESPONSE, &Ext2SequenceCounter, MCP251XFD_STANDARD_MESSAGE_ID, MCP251XFD_DLC_4BYTE, (uint8_t*)AutoRTRmessage, MCP251XFD_FIFO25, false); // Prepare a message with "DEMO" in case of 0x000 message incoming
            // return;

        case MCP251XFD_FIFO1_INTERRUPT:
        case MCP251XFD_FIFO2_INTERRUPT:
        case MCP251XFD_FIFO3_INTERRUPT:
        case MCP251XFD_FIFO4_INTERRUPT:
        case MCP251XFD_FIFO9_INTERRUPT:
        case MCP251XFD_FIFO10_INTERRUPT:
        case MCP251XFD_FIFO11_INTERRUPT:
        case MCP251XFD_FIFO12_INTERRUPT:
        case MCP251XFD_FIFO13_INTERRUPT:
        case MCP251XFD_FIFO14_INTERRUPT:
        case MCP251XFD_FIFO15_INTERRUPT:
        case MCP251XFD_FIFO16_INTERRUPT:
            FURI_LOG_I(TAG, "receve 1-16");
            ReceiveEvent = true;
            return;

        case MCP251XFD_FIFO5_INTERRUPT:
        case MCP251XFD_FIFO6_INTERRUPT:
        case MCP251XFD_FIFO7_INTERRUPT:
        case MCP251XFD_FIFO8_INTERRUPT:
        case MCP251XFD_FIFO17_INTERRUPT:
        case MCP251XFD_FIFO18_INTERRUPT:
        case MCP251XFD_FIFO19_INTERRUPT:
        case MCP251XFD_FIFO20_INTERRUPT:
        case MCP251XFD_FIFO21_INTERRUPT:
        case MCP251XFD_FIFO22_INTERRUPT:
        case MCP251XFD_FIFO23_INTERRUPT:
        case MCP251XFD_FIFO24_INTERRUPT:
        case MCP251XFD_FIFO26_INTERRUPT:
        case MCP251XFD_FIFO27_INTERRUPT:
        case MCP251XFD_FIFO28_INTERRUPT:
        case MCP251XFD_FIFO29_INTERRUPT:
        case MCP251XFD_FIFO30_INTERRUPT:
        case MCP251XFD_FIFO31_INTERRUPT:
            FURI_LOG_I(TAG, "Transm 15-31");
            TransmitEvent = true;
            return;

        case MCP251XFD_NO_INTERRUPT: // No interrupt
            return;

        default:
            //--- Check others interrupts ---
            MCP251XFD_GetInterruptEvents(CAN0, &Events);

            if((Events & MCP251XFD_INT_SPI_CRC_EVENT) > 0) // SPI CRC event
            {
                MCP251XFD_GetCRCEvents(CAN0, &CRCEvent, NULL);
                if(CRCEvent == MCP251XFD_CRC_CRCERR_EVENT)
                    FURI_LOG_E(TAG, "Ext2: CRC mismatch occurred on device");
                else
                    FURI_LOG_E(
                        TAG,
                        "Ext2: Number of Bytes mismatch during 'SPI with CRC' command occurred");
                MCP251XFD_ClearCRCEvents(CAN0);
            }

            if((Events & MCP251XFD_INT_RAM_ECC_EVENT) > 0) // ECC event
            {
                uint16_t AddrError = 0;
                MCP251XFD_GetECCEvents(CAN0, &ECCEvent, &AddrError);
                if(ECCEvent == MCP251XFD_ECC_SEC_EVENT)
                    FURI_LOG_E(TAG, "Ext2: ECC Single Error was corrected");
                else
                    FURI_LOG_E(TAG, "Ext2: ECC Double Error was detected");
                MCP251XFD_ClearECCEvents(CAN0);
            }

            if((Events & MCP251XFD_INT_BUS_ERROR_EVENT) > 0) // Bus error event
            {
                FURI_LOG_E(TAG, "Ext2: Bus error");
                //MCP251XFD_GetBusDiagnostic();
                //MCP251XFD_ClearBusDiagnostic();
                MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_BUS_ERROR_EVENT);
            }
            break;
        }
    }
    if(TransmitError) FURI_LOG_W(TAG, "TransmitError = %d", InterruptCode);
    //TransmitMessageToEXT2(SID_EXT2_ERROR_EVENT, &Ext2SequenceCounter, MCP251XFD_STANDARD_MESSAGE_ID, MCP251XFD_DLC_1BYTE, &InterruptCode, MCP251XFD_FIFO31, true); // Send last error on Ext2 module through Ext2
}
