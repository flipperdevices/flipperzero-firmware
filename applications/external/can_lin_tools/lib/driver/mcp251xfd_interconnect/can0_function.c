#include "can0_function.h"
#include "can_interface_sync.h"
#include "mcp251xfd_spi.h"
#include "mpc251xfd_user_driver_data.h"

#define TAG "CAN0_Functions"

const char op_mode_str[8][25 + 1 /* \0 */] = {
    "normal CAN FD mode",
    "sleep mode",
    "internal loopback mode",
    "listen only mode",
    "configuration mode",
    "external loopback mode",
    "normal CAN 2.0 mode",
    "restricted operation mode",
};

//=============================================================================
// All configuration structure of the MCP251XFD on the CAN0
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
    //.ControlFlags = MCP251XFD_CAN_UNLIMITED_RETRANS_ATTEMPTS,
    .ControlFlags =
        MCP251XFD_CAN_RESTRICTED_RETRANS_ATTEMPTS, // | MCP251XFD_CAN_LISTEN_ONLY_MODE_ON_ERROR,// Restricted retransmission attempts, MCP251XFD_FIFO.Attempts (CiFIFOCONm.TXAT) is used
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

MCP251XFD_RAMInfos can0_tef_ram_infos;
MCP251XFD_RAMInfos can0_txq_ram_infos;
MCP251XFD_RAMInfos can_fifo_ram_infos[CAN0_FIFO_COUNT - 2];
MCP251XFD_FIFO can0_fifo_list[CAN0_FIFO_COUNT] = {
    { .Name = MCP251XFD_TEF,    .Size = MCP251XFD_FIFO_10_MESSAGE_DEEP,                                                                                                                                                                         .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_OBJ,    .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_EVENT_FIFO_NOT_EMPTY_INT,                .RAMInfos = &can0_tef_ram_infos,    },
    { .Name = MCP251XFD_TXQ,    .Size = MCP251XFD_FIFO_4_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_64BYTE,                                            .Attempts = MCP251XFD_THREE_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,      .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,         .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT, .RAMInfos = &can0_txq_ram_infos,    },
    { .Name = MCP251XFD_FIFO1,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_64BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[0], }, // SID: 0x000..0x1FF ; No EID
    { .Name = MCP251XFD_FIFO2,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_64BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[1], }, // SID: 0x200..0x3FF ; No EID
    { .Name = MCP251XFD_FIFO3,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_64BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[2], }, // SID: 0x400..0x5FF ; No EID
    { .Name = MCP251XFD_FIFO4,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_64BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[3], }, // SID: 0x600..0x7FF ; No EID
    
    { .Name = MCP251XFD_FIFO5,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_8BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[4], },   // EID: 0x00000..0x07FFF ; All SID
    { .Name = MCP251XFD_FIFO6,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_8BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[5], },   // EID: 0x08000..0x0FFFF ; All SID
    { .Name = MCP251XFD_FIFO7,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_8BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[6], },   // EID: 0x10000..0x17FFF ; All SID
    { .Name = MCP251XFD_FIFO8,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_8BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[7], },   // EID: 0x18000..0x1FFFF ; All SID
    { .Name = MCP251XFD_FIFO9,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_8BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[8], },   // EID: 0x20000..0x27FFF ; All SID
    { .Name = MCP251XFD_FIFO10,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_8BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[9], },  // EID: 0x28000..0x2FFFF ; All SID
    { .Name = MCP251XFD_FIFO11,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_8BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[10], }, // EID: 0x30000..0x37FFF ; All SID
    { .Name = MCP251XFD_FIFO12,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_8BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[11], }, // EID: 0x38000..0x3FFFF ; All SID
    { .Name = MCP251XFD_FIFO13,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_8BYTE,    .Direction = MCP251XFD_RECEIVE_FIFO,                                                                                            .ControlFlags = MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX,     .InterruptFlags = MCP251XFD_FIFO_OVERFLOW_INT + MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT,              .RAMInfos = &can_fifo_ram_infos[11], }, // EID: 0x38000..0x3FFFF ; All SID
    

   // { .Name = MCP251XFD_FIFO13,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_64BYTE,    .Direction = MCP251XFD_TRANSMIT_FIFO,   .Attempts = MCP251XFD_THREE_ATTEMPTS,   .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,    .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,         .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT, .RAMInfos = &can_fifo_ram_infos[12], },
    { .Name = MCP251XFD_FIFO14,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_64BYTE,    .Direction = MCP251XFD_TRANSMIT_FIFO,   .Attempts = MCP251XFD_THREE_ATTEMPTS,   .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,    .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,         .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT, .RAMInfos = &can_fifo_ram_infos[13], },
    { .Name = MCP251XFD_FIFO15,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_64BYTE,    .Direction = MCP251XFD_TRANSMIT_FIFO,   .Attempts = MCP251XFD_THREE_ATTEMPTS,   .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,    .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,         .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT, .RAMInfos = &can_fifo_ram_infos[14], },
    { .Name = MCP251XFD_FIFO16,  .Size = MCP251XFD_FIFO_2_MESSAGE_DEEP,  .Payload = MCP251XFD_PAYLOAD_64BYTE,    .Direction = MCP251XFD_TRANSMIT_FIFO,   .Attempts = MCP251XFD_THREE_ATTEMPTS,   .Priority = MCP251XFD_MESSAGE_TX_PRIORITY16,    .ControlFlags = MCP251XFD_FIFO_NO_RTR_RESPONSE,         .InterruptFlags = MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT + MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT, .RAMInfos = &can_fifo_ram_infos[15], },
};

MCP251XFD_Filter can_filter_list[CAN0_FILTER_COUNT] = {
    { .Filter = MCP251XFD_FILTER0,  .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_SID,    .AcceptanceID = 0x000,    .AcceptanceMask = 0x600,    .PointTo = MCP251XFD_FIFO1, }, // 0x000..0x1FF
    { .Filter = MCP251XFD_FILTER1,  .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_SID,    .AcceptanceID = 0x200,    .AcceptanceMask = 0x600,    .PointTo = MCP251XFD_FIFO2, }, // 0x200..0x3FF
    { .Filter = MCP251XFD_FILTER2,  .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_SID,    .AcceptanceID = 0x400,    .AcceptanceMask = 0x600,    .PointTo = MCP251XFD_FIFO3, }, // 0x400..0x5FF
    { .Filter = MCP251XFD_FILTER3,  .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_SID,    .AcceptanceID = 0x600,    .AcceptanceMask = 0x600,    .PointTo = MCP251XFD_FIFO4, }, // 0x600..0x7FF

    { .Filter = MCP251XFD_FILTER4 , .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_EID,    .AcceptanceID = 0x00000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO5,  },  // 0x00000000..0x1C000000
    { .Filter = MCP251XFD_FILTER5 , .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_EID,    .AcceptanceID = 0x04000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO6,  },  // 0x04000000..0x1C000000
    { .Filter = MCP251XFD_FILTER6 , .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_EID,    .AcceptanceID = 0x08000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO7,  },  // 0x08000000..0x1C000000
    { .Filter = MCP251XFD_FILTER7 , .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_EID,    .AcceptanceID = 0x0C000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO8,  },  // 0x0C000000..0x1C000000
    { .Filter = MCP251XFD_FILTER8 , .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_EID,    .AcceptanceID = 0x10000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO9,  },  // 0x10000000..0x1C000000
    { .Filter = MCP251XFD_FILTER9,  .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_EID,    .AcceptanceID = 0x14000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO10, },  // 0x14000000..0x1C000000
    { .Filter = MCP251XFD_FILTER10, .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_EID,    .AcceptanceID = 0x18000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO11, },  // 0x18000000..0x1C000000
    { .Filter = MCP251XFD_FILTER11, .EnableFilter = true,   .Match = MCP251XFD_MATCH_ONLY_EID,    .AcceptanceID = 0x1C000000, .AcceptanceMask = 0x1C000000, .PointTo = MCP251XFD_FIFO12, },  // 0x1C000000..0x1C000000
    { .Filter = MCP251XFD_FILTER13,  .EnableFilter = true,   .Match = MCP251XFD_MATCH_SID_EID,     .AcceptanceID = MCP251XFD_ACCEPT_ALL_MESSAGES, .AcceptanceMask = MCP251XFD_ACCEPT_ALL_MESSAGES, .PointTo = MCP251XFD_FIFO13, },
};

static bool transmit_event = false;
static bool receive_event = false;

//**********************************************************************************************************************************************************
//=============================================================================
// Configure the MCP251XFD device on CAN0
//=============================================================================
eERRORRESULT
    can0_function_device_init_can20(uint32_t nominal_bitrate, eMCP251XFD_OperationMode mode) {
    can0_config.NominalBitrate = nominal_bitrate;

    switch(mode) {
    case MCP251XFD_LISTEN_ONLY_MODE:
        can0_config.ControlFlags = 0;
        can0_config.ControlFlags = MCP251XFD_CAN_RESTRICTED_RETRANS_ATTEMPTS |
                                   MCP251XFD_CAN_LISTEN_ONLY_MODE_ON_ERROR;
        break;
    case MCP251XFD_NORMAL_CAN20_MODE:
        can0_config.ControlFlags = 0;
        can0_config.ControlFlags = MCP251XFD_CAN_RESTRICTED_RETRANS_ATTEMPTS;
        break;
    case MCP251XFD_NORMAL_CANFD_MODE:
        // ToDo you need to add some flags to the config to work in CANFD mode
        // can0_config.NominalBitrate = ....;
        // can0_config.ControlFlags = 0;
        // can0_config.ControlFlags = .....;
        break;
    default:
        break;
    }

    //--- Initialize Int pins or GPIOs ---
    can_interface_sync_pin_init(CAN0);
    can_interface_sync_int0_gpio0_pin_init(CAN0);
    can_interface_sync_int1_gpio1_pin_init(CAN0);

    //--- Configure module on CAN0 ---
    eERRORRESULT error_can = ERR__NO_DEVICE_DETECTED;
    error_can = Init_MCP251XFD(CAN0, &can0_config);
    if(error_can == ERR_OK) {
        error_can = MCP251XFD_ConfigureTimeStamp(
            CAN0, true, MCP251XFD_TS_CAN20_SOF_CANFD_SOF, TIMESTAMP_TICK(can0_sysclk), true);
        if(error_can == ERR_OK)
            error_can = MCP251XFD_ConfigureFIFOList(CAN0, &can0_fifo_list[0], CAN0_FIFO_COUNT);
        if(error_can == ERR_OK)
            error_can = MCP251XFD_ConfigureFilterList(
                CAN0, MCP251XFD_D_NET_FILTER_DISABLE, &can_filter_list[0], CAN0_FILTER_COUNT);
        if(error_can == ERR_OK)
            //error_can = MCP251XFD_StartCAN20(CAN0);
            //error_can = MCP251XFD_RequestOperationMode(CANEXT2, MCP251XFD_NORMAL_CAN20_MODE, false);
            //error_can = MCP251XFD_RequestOperationMode(CANEXT2, MCP251XFD_INTERNAL_LOOPBACK_MODE, false);
            //error_can = MCP251XFD_RequestOperationMode(CANEXT2, MCP251XFD_EXTERNAL_LOOPBACK_MODE, false);

            //Todo change the mode, the device must be in Configuration mode
            //you need to add some flags to the config to work in CANFD mode
            error_can = MCP251XFD_RequestOperationMode(CAN0, mode, false);
    }
    return error_can;
}

//=============================================================================
// Deinit the MCP251XFD device on CAN0
//=============================================================================
eERRORRESULT can0_function_device_deinit(void) {
    return can_interface_sync_deinit(CAN0);
}

//=============================================================================
// Transmit messages to MCP251XFD device on CAN0 no irq
//=============================================================================
eERRORRESULT can0_function_transmit_msg_no_irq(
    uint32_t message_id,
    uint32_t* message_seq,
    setMCP251XFD_MessageCtrlFlags control_flags,
    eMCP251XFD_DataLength dlc,
    uint8_t* payload_data,
    bool flush) {
    eERRORRESULT error_can = ERR_OK;
    eMCP251XFD_FIFOstatus fifo_status = 0;
    setMCP251XFD_InterruptOnFIFO interrupt_on_fifo = 0;
    error_can = MCP251XFD_GetTransmitPendingInterruptStatusOfAllFIFO(
        CAN0, &interrupt_on_fifo); // Get all FIFO status
    if(error_can != ERR_OK) return error_can;
    for(eMCP251XFD_FIFO z_fifo = 0; z_fifo < MCP251XFD_TX_FIFO_MAX;
        z_fifo++) // For each transmit FIFO, TXQ but not TEF
        if((interrupt_on_fifo & (1 << z_fifo)) > 0) // If an Interrupt is flagged
        {
            error_can = MCP251XFD_GetFIFOStatus(
                CAN0, z_fifo, &fifo_status); // Get the status of the flagged FIFO
            if(error_can != ERR_OK) return error_can;
            if((fifo_status & MCP251XFD_TX_FIFO_NOT_FULL) > 0) // Check FIFO not empty
            {
                eMCP251XFD_Devices dev = MCP2517FD;
                MCP251XFD_GetDeviceID(CAN0, &dev, NULL, NULL);
                uint32_t max_sequence = MCP2518FD_SEQUENCE_MAX;
                if(dev == MCP2517FD) max_sequence = MCP2517FD_SEQUENCE_MAX;

                MCP251XFD_CANMessage can_msg;
                //***** Fill the message as you want *****
                can_msg.MessageID = message_id;
                can_msg.MessageSEQ = *message_seq;
                (*message_seq)++;
                if(*message_seq > max_sequence)
                    *message_seq =
                        0; // Roll over the sequence value (maximum differ if it's a MCP2517FD or a MCP2518FD)
                can_msg.ControlFlags = control_flags;
                can_msg.DLC = dlc;
                can_msg.PayloadData = payload_data;
                error_can = MCP251XFD_TransmitMessageToFIFO(
                    CAN0, &can_msg, z_fifo, flush); // Send message and flush
            }
        }
    return error_can;
}

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
    bool flush) {
    // if (Ext2ModulePresent == false) return ERR_OK;
    transmit_event = false;
    MCP251XFD_CANMessage can_msg;

    eMCP251XFD_Devices dev = MCP2517FD;
    MCP251XFD_GetDeviceID(CAN0, &dev, NULL, NULL);
    uint32_t max_sequence = MCP2518FD_SEQUENCE_MAX;
    if(dev == MCP2517FD) max_sequence = MCP2517FD_SEQUENCE_MAX;

    //--- Create message ---
    can_msg.MessageID = message_id;
    can_msg.MessageSEQ = *message_seq;
    (*message_seq)++;
    if(*message_seq > max_sequence)
        *message_seq =
            0; // Roll over the sequence value (maximum differ if it's a MCP2517FD or a MCP2518FD)
    can_msg.ControlFlags = control_flags;
    can_msg.DLC = dlc;
    can_msg.PayloadData = payload_data;
    return MCP251XFD_TransmitMessageToFIFO(CAN0, &can_msg, to_fifo, flush);
}

//=============================================================================
// Transmit a message to MCP251XFD device on CAN0
//=============================================================================
eERRORRESULT can0_function_transmit_msg(
    MCP251XFD_CANMessage* can_msg,
    uint32_t* message_seq,
    eMCP251XFD_FIFO to_fifo,
    bool flush) {
    eMCP251XFD_Devices dev = MCP2517FD;
    MCP251XFD_GetDeviceID(CAN0, &dev, NULL, NULL);
    uint32_t max_sequence = MCP2518FD_SEQUENCE_MAX;
    if(dev == MCP2517FD) max_sequence = MCP2517FD_SEQUENCE_MAX;

    can_msg->MessageSEQ = *message_seq;
    (*message_seq)++;
    if(*message_seq > max_sequence)
        *message_seq =
            0; // Roll over the sequence value (maximum differ if it's a MCP2517FD or a MCP2518FD)
    return MCP251XFD_TransmitMessageToFIFO(CAN0, can_msg, to_fifo, flush);
}

//=============================================================================
// Receive a message from MCP251XFD device on CAN0
//=============================================================================
eERRORRESULT can0_function_receive_msg(
    MCP251XFD* pComp,
    bool* receive_event,
    MCP251XFD_CANMessage* msg,
    eMCP251XFD_PayloadSize payload_size,
    uint32_t* msg_time_stamp) {
    //if (Ext2ModulePresent == false) return ERR_OK;
    eERRORRESULT error_can = ERR_OK;

#ifdef APP_USE_EXT2_INT1_PIN
    eMCP251XFD_FIFO fifo_name;
    eMCP251XFD_FIFOstatus fifo_status = 0;
    if((ioport_get_pin_level(EXT2_PIN_14) == 0) ||
       (receive_event ==
        true)) // Check INT1 pin status of the MCP251XFD (Active low state), this pin is configured for all receive FIFO not empty
    {
        error_can =
            MCP251XFD_GetCurrentReceiveFIFONameAndStatusInterrupt(pComp, &fifo_name, &fifo_status);
        if(error_can != ERR_OK)
            return error_can; // First get which FIFO set interrupt and its status
        if(((fifo_status & MCP251XFD_RX_FIFO_NOT_EMPTY) > 0) &&
           (fifo_name != MCP251XFD_NO_FIFO)) // Second check FIFO not empty
#else
    eMCP251XFD_FIFOstatus fifo_status = 0;
    setMCP251XFD_InterruptOnFIFO interrupt_on_fifo = 0;
    error_can = MCP251XFD_GetReceivePendingInterruptStatusOfAllFIFO(
        pComp, &interrupt_on_fifo); // Get all FIFO status
    if(error_can != ERR_OK) return error_can;
    for(eMCP251XFD_FIFO fifo_name = 1; fifo_name < MCP251XFD_FIFO_MAX;
        fifo_name++) // For each receive FIFO but not TEF, TXQ
        if((interrupt_on_fifo & (1 << fifo_name)) > 0) // If an Interrupt is flagged
        {
            error_can = MCP251XFD_GetFIFOStatus(
                pComp, fifo_name, &fifo_status); // Get the status of the flagged FIFO
            if(error_can != ERR_OK) return error_can;
            if((fifo_status & MCP251XFD_RX_FIFO_NOT_EMPTY) >
               0) // Check FIFO not empty and no error while retrieve FIFO status
#endif

        {
            //Todo add getting more than one fifo
            error_can = MCP251XFD_ReceiveMessageFromFIFO(
                pComp, msg, payload_size, msg_time_stamp, fifo_name);
            *receive_event = true;
        }
    }
    return error_can;
}

//=============================================================================
// Check device interrupt (INT) on CAN0
//=============================================================================
void can0_function_device_check_irq(void) {
    //if (Ext2ModulePresent == false) return;
    eERRORRESULT error_can;
    setMCP251XFD_CRCEvents crc_event;
    eMCP251XFD_OperationMode op_mode;
    setMCP251XFD_InterruptEvents events;
    setMCP251XFD_ECCEvents ecc_event;

#ifdef APP_USE_EXT2_INT_PIN
    if(ioport_get_pin_level(EXT2_PIN_IRQ) != 0)
        return; // Check INT pin status of the MCP251XFD (Active low state)
#endif
    bool transmit_error = true;
    setMCP251XFD_InterruptOnFIFO interrupt_on_fifo = 0;
    eMCP251XFD_InterruptFlagCode interrupt_code = 0;
    error_can = MCP251XFD_GetCurrentInterruptEvent(
        CAN0, &interrupt_code); // Get the current Interrupt event
    if(error_can == ERR_OK) // If no errors, process
    {
        switch(interrupt_code) {
        case MCP251XFD_ERROR_INTERRUPT: // CAN Bus Error Interrupt
            FURI_LOG_E(TAG, "CAN0: CAN Bus Error");
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_BUS_ERROR_EVENT);
            break;

        case MCP251XFD_WAKEUP_INTERRUPT: // Wake-up interrupt
            // No sleep defined for this one
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_BUS_WAKEUP_EVENT);
            break;

        case MCP251XFD_RECEIVE_FIFO_OVF: // Receive FIFO Overflow Interrupt
            MCP251XFD_GetReceiveOverflowInterruptStatusOfAllFIFO(CAN0, &interrupt_on_fifo);
            for(eMCP251XFD_FIFO z_fifo = 1; z_fifo < MCP251XFD_RX_FIFO_MAX;
                z_fifo++) // For each receive FIFO but not the TEF
                if((interrupt_on_fifo & (1 << z_fifo)) > 0) // If the Overflow Interrupt is flagged
                {
                    FURI_LOG_E(
                        TAG,
                        "CAN0: Receive FIFO%u Overflow",
                        (unsigned int)z_fifo); // Display an error for this FIFO
                    MCP251XFD_ClearFIFOOverflowEvent(CAN0, z_fifo); // Clear the flag of this FIFO
                }
            break;

        case MCP251XFD_TRANSMIT_ATTEMPT: // Transmit Attempt Interrupt
            MCP251XFD_GetTransmitAttemptInterruptStatusOfAllFIFO(CAN0, &interrupt_on_fifo);
            for(eMCP251XFD_FIFO z_fifo = 0; z_fifo < MCP251XFD_TX_FIFO_MAX;
                z_fifo++) // For each transmit FIFO
                if((interrupt_on_fifo & (1 << z_fifo)) > 0) // If the Attempt Interrupt is flagged
                {
                    FURI_LOG_E(
                        TAG,
                        "CAN0: Transmit FIFO%u Attempts Exhaust",
                        (unsigned int)z_fifo); // Display an error for this FIFO
                    MCP251XFD_ClearFIFOAttemptsEvent(CAN0, z_fifo); // Clear the flag of this FIFO
                }
            break;

        case MCP251XFD_ADDRESS_ERROR_INTERRUPT: // Address Error Interrupt (illegal FIFO address presented to system)
            FURI_LOG_E(TAG, "CAN0: Illegal FIFO address presented to system");
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_SYSTEM_ERROR_EVENT);
            break;

        case MCP251XFD_RXTX_MAB_OVF_UVF: // RX MAB Overflow (RX: message received before previous message was saved to memory)
            FURI_LOG_E(
                TAG,
                "CAN0: RX MAB Overflow (RX: message received before previous message was saved to memory)");
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_SYSTEM_ERROR_EVENT);
            break;

        case MCP251XFD_TBC_OVF_INTERRUPT: // TBC Overflow

            // The time base for the timestamp overflow (32-bits), update the associated time

            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_TIME_BASE_COUNTER_EVENT);
            break;

        case MCP251XFD_OPMODE_CHANGE_OCCURED: // Operation Mode Change Occurred
            MCP251XFD_GetActualOperationMode(CAN0, &op_mode);
            FURI_LOG_I(TAG, "CAN0: Operation mode change to %s", op_mode_str[(size_t)op_mode]);
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_OPERATION_MODE_CHANGE_EVENT);
            break;

        case MCP251XFD_INVALID_MESSAGE_OCCURED: // Invalid Message Occurred
            FURI_LOG_W(TAG, "CAN0: Invalid Message Occurred");
            MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_RX_INVALID_MESSAGE_EVENT);
            break;

        case MCP251XFD_TRANSMIT_EVENT_FIFO: // Transmit Event FIFO Interrupt

            //*** Do something with the TEF object ***

            MCP251XFD_UpdateTEF(CAN0);
            transmit_error = false;
            return;

            //case MCP251XFD_FIFO25_INTERRUPT:
            // FURI_LOG_I(TAG,"MCP251XFD_FIFO25_INTERRUPT");
            // //MCP251XFD_ClearInterruptEvents(CANEXT2, MCP251XFD_FIFO25_INTERRUPT);
            // //TransmitMessageToEXT2(SID_DEMO_RTR_RESPONSE, &Ext2SequenceCounter, MCP251XFD_STANDARD_MESSAGE_ID, MCP251XFD_DLC_4BYTE, (uint8_t*)AutoRTRmessage, MCP251XFD_FIFO25, false); // Prepare a message with "DEMO" in case of 0x000 message incoming
            // return;

        case MCP251XFD_FIFO1_INTERRUPT:
        case MCP251XFD_FIFO2_INTERRUPT:
        case MCP251XFD_FIFO3_INTERRUPT:
        case MCP251XFD_FIFO4_INTERRUPT:
        case MCP251XFD_FIFO5_INTERRUPT:
        case MCP251XFD_FIFO6_INTERRUPT:
        case MCP251XFD_FIFO7_INTERRUPT:
        case MCP251XFD_FIFO8_INTERRUPT:
        case MCP251XFD_FIFO9_INTERRUPT:
        case MCP251XFD_FIFO10_INTERRUPT:
        case MCP251XFD_FIFO11_INTERRUPT:
        case MCP251XFD_FIFO12_INTERRUPT:
        case MCP251XFD_FIFO13_INTERRUPT:
            //FURI_LOG_I(TAG, "receve 1-16");
            receive_event = true;
            return;

        //case MCP251XFD_FIFO13_INTERRUPT:
        case MCP251XFD_FIFO14_INTERRUPT:
        case MCP251XFD_FIFO15_INTERRUPT:
        case MCP251XFD_FIFO16_INTERRUPT:
            // case MCP251XFD_FIFO17_INTERRUPT:
            // case MCP251XFD_FIFO18_INTERRUPT:
            // case MCP251XFD_FIFO19_INTERRUPT:
            // case MCP251XFD_FIFO20_INTERRUPT:
            // case MCP251XFD_FIFO21_INTERRUPT:
            // case MCP251XFD_FIFO22_INTERRUPT:
            // case MCP251XFD_FIFO23_INTERRUPT:
            // case MCP251XFD_FIFO24_INTERRUPT:
            // case MCP251XFD_FIFO26_INTERRUPT:
            // case MCP251XFD_FIFO27_INTERRUPT:
            // case MCP251XFD_FIFO28_INTERRUPT:
            // case MCP251XFD_FIFO29_INTERRUPT:
            // case MCP251XFD_FIFO30_INTERRUPT:
            // case MCP251XFD_FIFO31_INTERRUPT:
            //FURI_LOG_I(TAG, "Transm 15-31");
            transmit_event = true;
            return;

        case MCP251XFD_NO_INTERRUPT: // No interrupt
            return;

        default:
            //--- Check others interrupts ---
            MCP251XFD_GetInterruptEvents(CAN0, &events);

            if((events & MCP251XFD_INT_SPI_CRC_EVENT) > 0) // SPI CRC event
            {
                MCP251XFD_GetCRCEvents(CAN0, &crc_event, NULL);
                if(crc_event == MCP251XFD_CRC_CRCERR_EVENT)
                    FURI_LOG_E(TAG, "CAN0: CRC mismatch occurred on device");
                else
                    FURI_LOG_E(
                        TAG,
                        "CAN0: Number of Bytes mismatch during 'SPI with CRC' command occurred");
                MCP251XFD_ClearCRCEvents(CAN0);
            }

            if((events & MCP251XFD_INT_RAM_ECC_EVENT) > 0) // ECC event
            {
                uint16_t AddrError = 0;
                MCP251XFD_GetECCEvents(CAN0, &ecc_event, &AddrError);
                if(ecc_event == MCP251XFD_ECC_SEC_EVENT)
                    FURI_LOG_E(TAG, "CAN0: ECC Single Error was corrected");
                else
                    FURI_LOG_E(TAG, "CAN0: ECC Double Error was detected");
                MCP251XFD_ClearECCEvents(CAN0);
            }

            if((events & MCP251XFD_INT_BUS_ERROR_EVENT) > 0) // Bus error event
            {
                FURI_LOG_E(TAG, "CAN0: Bus error");
                //MCP251XFD_GetBusDiagnostic();
                //MCP251XFD_ClearBusDiagnostic();
                MCP251XFD_ClearInterruptEvents(CAN0, MCP251XFD_INT_BUS_ERROR_EVENT);
            }
            break;
        }
    }
    if(transmit_error) FURI_LOG_W(TAG, "transmit_error = %d", interrupt_code);
    //TransmitMessageToEXT2(SID_EXT2_ERROR_EVENT, &Ext2SequenceCounter, MCP251XFD_STANDARD_MESSAGE_ID, MCP251XFD_DLC_1BYTE, &interrupt_code, MCP251XFD_FIFO31, true); // Send last error on CAN0 module through CAN0
}
