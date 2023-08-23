#include "carbus_analyzer.h"
#include "carbus_analyzer_cmd.h"
#include "carbus_analyzer_utils.h"

#include "../mcp251xfd_interconnect/can0_function.h"
#include "../mcp251xfd_interconnect/show_device.h"
#include "../mcp251xfd/MCP251XFD.h"

#define TAG "CarBusAnalyzer"
#define CARBUS_ANALYZER_TX_RX_BUF_SIZE 1024
#define CARBUS_ANALYZER_CMD_BUF_SIZE 128
#define CARBUS_ANALYZER_PAYLOAD_DATA_SIZE 8

typedef struct {
    uint8_t cmd;
    uint8_t cmd_counter;
    uint8_t cmd_extended;
    uint8_t data_len;
} CarBusAnalyzerCmd;

static CarBusAnalyzerCmd cmd_of_end = {CBA_CMD_OF_END, 0, 0, 0};

struct CarBusAnalyzer {
    FuriStreamBuffer* stream_rx;
    FuriStreamBuffer* stream_tx;
    bool exit;

    CarBusAnalyzerCmd cmd;

    uint8_t cmd_buf[CARBUS_ANALYZER_CMD_BUF_SIZE];
    size_t cmd_buf_len;
    bool can_open;
    bool can_timestamp;
    bool can_listen_only;
    uint32_t seq_counter;
    CarBusAnalyzerBitrate can0_bitrate;

    MCP251XFD_CANMessage rx_msg;
    uint8_t rx_msg_payload_data[CARBUS_ANALYZER_PAYLOAD_DATA_SIZE];
    uint32_t rx_msg_time_stamp;

    CarBusAnalyzerCallback callback;
    CarBusAnalyzerConnectCallback connect_callback;
    void* context;
};

CarBusAnalyzer* carbus_analyzer_alloc(void) {
    CarBusAnalyzer* instance = malloc(sizeof(CarBusAnalyzer));
    instance->stream_rx =
        furi_stream_buffer_alloc(sizeof(int8_t) * CARBUS_ANALYZER_TX_RX_BUF_SIZE, sizeof(int8_t));
    instance->stream_tx =
        furi_stream_buffer_alloc(sizeof(int8_t) * CARBUS_ANALYZER_TX_RX_BUF_SIZE, sizeof(int8_t));
    instance->exit = false;
    memset(instance->cmd_buf, 0, sizeof(uint8_t) * CARBUS_ANALYZER_CMD_BUF_SIZE);
    instance->cmd_buf_len = 0;
    instance->can_open = false;
    instance->can0_bitrate = CarBusAnalyzerBitrateOff;
    instance->can_timestamp = false;
    instance->can_listen_only = false;
    instance->rx_msg.PayloadData = &instance->rx_msg_payload_data[0];

    instance->seq_counter = 0;
    return instance;
}

void carbus_analyzer_free(CarBusAnalyzer* instance) {
    furi_assert(instance);
    if(instance->can_open) {
        can0_function_device_deinit();
        instance->can_open = false;
    }
    furi_stream_buffer_free(instance->stream_tx);
    furi_stream_buffer_free(instance->stream_rx);

    free(instance);
}

void carbus_analyzer_set_callback(
    CarBusAnalyzer* instance,
    CarBusAnalyzerCallback callback,
    CarBusAnalyzerConnectCallback connect_callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->connect_callback = connect_callback;
    instance->context = context;
}

bool carbus_analyzer_is_device_connected(CarBusAnalyzer* instance) {
    furi_assert(instance);
    bool ret = false;

    if(instance->can_open) {
        ret = true;
    } else {
        if(can0_function_device_init_can20(500000, MCP251XFD_LISTEN_ONLY_MODE) == ERR_OK) {
            ret = true;
            can0_function_device_deinit();
        }
    }
    return ret;
}

size_t carbus_analyzer_spaces_rx(CarBusAnalyzer* instance) {
    return furi_stream_buffer_spaces_available(instance->stream_rx);
}

bool carbus_analyzer_rx(CarBusAnalyzer* instance, uint8_t* data, size_t len) {
    furi_assert(instance);
    furi_assert(data);
    furi_assert(len != 0);
    size_t ret = furi_stream_buffer_send(instance->stream_rx, data, sizeof(uint8_t) * len, 0);
    return ret == sizeof(uint8_t) * len;
}

size_t carbus_analyzer_tx(CarBusAnalyzer* instance, uint8_t* data, size_t max_len) {
    furi_assert(instance);
    return furi_stream_buffer_receive(instance->stream_tx, data, sizeof(int8_t) * max_len, 0);
}

static void carbus_analyzer_tx_add_cmd(CarBusAnalyzer* instance) {
    furi_assert(instance);
    furi_stream_buffer_send(
        instance->stream_tx, &instance->cmd, sizeof(CarBusAnalyzerCmd), FuriWaitForever);
}

static void carbus_analyzer_tx_add_data(CarBusAnalyzer* instance, uint8_t* data, size_t len) {
    furi_assert(instance);
    furi_stream_buffer_send(instance->stream_tx, data, sizeof(uint8_t) * len, FuriWaitForever);
}

// static void carbus_analyzer_tx_add_ch(CarBusAnalyzer* instance, uint8_t data) {
//     furi_assert(instance);
//     furi_stream_buffer_send(instance->stream_tx, &data, sizeof(uint8_t), FuriWaitForever);
// }

void carbus_analyzer_exit(CarBusAnalyzer* instance) {
    furi_assert(instance);
    instance->exit = true;
}

// static uint8_t carbus_analyzer_getch(CarBusAnalyzer* instance) {
//     furi_assert(instance);
//     uint8_t data[1] = {0};
//     while(furi_stream_buffer_receive(instance->stream_rx, &data, sizeof(int8_t), 30) == 0) {
//         if(instance->exit) break;
//     };
//     return data[0];
// }

void carbus_analyzer_get_cmd(CarBusAnalyzer* instance) {
    furi_assert(instance);

    // while(furi_stream_buffer_receive(instance->stream_rx, &data, sizeof(int8_t), 30) == 0) {
    //     if(instance->exit) break;
    // };

    //Todo: fix if translate work can interrupt
    // Check data length >= sizeof(CarBusAnalyzerCmd)
    if(furi_stream_buffer_bytes_available(instance->stream_rx) < sizeof(CarBusAnalyzerCmd)) {
        return;
    }

    if(furi_stream_buffer_receive(
           instance->stream_rx, &instance->cmd, sizeof(CarBusAnalyzerCmd), 0) == 0) {
        return;
    }

    if(instance->cmd.cmd != CBA_CMD_OF_END) {
        carbus_analyzer_process_cmd(instance);
    } else {
        //Todo forget all data
    }
}

static bool carbus_analyzer_get_data(CarBusAnalyzer* instance) {
    furi_assert(instance);
    furi_assert(instance->cmd.data_len <= CARBUS_ANALYZER_CMD_BUF_SIZE);
    size_t ret = furi_stream_buffer_receive(
        instance->stream_rx, &instance->cmd_buf, instance->cmd.data_len, 1000);

    if(ret == instance->cmd.data_len) {
        return true;
    }
    return false;
}

// static void carbus_analyzer_can_close(CarBusAnalyzer* instance) {
//     furi_assert(instance);
//     if(instance->can_open) {
//         can0_function_device_deinit();
//         if(instance->callback) {
//             instance->connect_callback(instance->context, CarBusAnalyzerDisconnected);
//         }
//         instance->can_open = false;
//         instance->can_listen_only = false;
//         carbus_analyzer_tx_add_ch(instance, CH2_OK);
//     } else {
//         carbus_analyzer_tx_add_ch(instance, CH2_BELL);
//     }
// }

// static void carbus_analyzer_set_bitrate(CarBusAnalyzer* instance) {
//     furi_assert(instance);
//     furi_assert(instance->cmd_buf[0] == CH2_SET_BITRATE);
//     if(instance->can_open) {
//         carbus_analyzer_tx_add_ch(instance, CH2_BELL);
//     } else {
//         switch(instance->cmd_buf[1]) {
//         case '0':
//             FURI_LOG_D(TAG, "Set Bitrate: 10K");
//             instance->can0_bitrate = CarBusAnalyzerBitrate10K;
//             break;
//         case '1':
//             FURI_LOG_D(TAG, "Set Bitrate: 20K");
//             instance->can0_bitrate = CarBusAnalyzerBitrate20K;
//             break;
//         case '2':
//             FURI_LOG_D(TAG, "Set Bitrate: 50K");
//             instance->can0_bitrate = CarBusAnalyzerBitrate50K;
//             break;
//         case '3':
//             FURI_LOG_D(TAG, "Set Bitrate: 100K");
//             instance->can0_bitrate = CarBusAnalyzerBitrate100K;
//             break;
//         case '4':
//             FURI_LOG_D(TAG, "Set Bitrate: 125K");
//             instance->can0_bitrate = CarBusAnalyzerBitrate125K;
//             break;
//         case '5':
//             FURI_LOG_D(TAG, "Set Bitrate: 250K");
//             instance->can0_bitrate = CarBusAnalyzerBitrate250K;
//             break;
//         case '6':
//             FURI_LOG_D(TAG, "Set Bitrate: 500K");
//             instance->can0_bitrate = CarBusAnalyzerBitrate500K;
//             break;
//         case '7':
//             FURI_LOG_D(TAG, "Set Bitrate: 800K");
//             instance->can0_bitrate = CarBusAnalyzerBitrate800K;
//             break;
//         case '8':
//             FURI_LOG_D(TAG, "Set Bitrate: 1M");
//             instance->can0_bitrate = CarBusAnalyzerBitrate1M;
//             break;
//         default:
//             FURI_LOG_D(TAG, "Set Bitrate: Off");
//             instance->can0_bitrate = CarBusAnalyzerBitrateOff;
//             break;
//         }
//         carbus_analyzer_tx_add_ch(instance, CH2_OK);
//     }
// }
// static void carbus_analyzer_set_timestamp(CarBusAnalyzer* instance) {
//     furi_assert(instance);
//     furi_assert(instance->cmd_buf[0] == CH2_TIME_STAMP);

//     switch(instance->cmd_buf[1]) {
//     case '0':
//         FURI_LOG_D(TAG, "Set Timestamp: Off");
//         instance->can_timestamp = false;
//         carbus_analyzer_tx_add_ch(instance, CH2_OK);
//         break;
//     case '1':
//         FURI_LOG_D(TAG, "Set Timestamp: On");
//         instance->can_timestamp = true;
//         carbus_analyzer_tx_add_ch(instance, CH2_OK);
//         break;
//     default:
//         FURI_LOG_D(TAG, "Set Timestamp: Unknown");
//         carbus_analyzer_tx_add_ch(instance, CH2_BELL);
//         break;
//     }
// }

// static void can_hacher2_can_open(CarBusAnalyzer* instance) {
//     furi_assert(instance);

//     if(instance->can_open) {
//         carbus_analyzer_tx_add_ch(instance, CH2_BELL);

//     } else {
//         if(instance->can0_bitrate == CarBusAnalyzerBitrateOff) {
//             //Todo: check supported speeds
//             carbus_analyzer_tx_add_ch(instance, CH2_BELL);
//         } else {
//             //Todo: add loopback mode
//             eERRORRESULT ret = ERR__UNKNOWN;
//             if(instance->can_listen_only) {
//                 ret = can0_function_device_init_can20(
//                     instance->can0_bitrate, MCP251XFD_LISTEN_ONLY_MODE);
//             } else {
//                 ret = can0_function_device_init_can20(
//                     instance->can0_bitrate, MCP251XFD_NORMAL_CAN20_MODE);
//             }

//             if(ret == ERR_OK) {
//                 if(instance->callback) {
//                     instance->connect_callback(instance->context, CarBusAnalyzerConnected);
//                 }
//                 show_device_detected(CAN0, can0_sysclk);
//                 show_device_configuration(&can0_bt_stats);
//                 show_device_fifo_configuration(&can0_fifo_list[0], CAN0_FIFO_COUNT);
//                 show_device_filter_configuration(&can_filter_list[0], CAN0_FILTER_COUNT, false);
//                 instance->can_open = true;
//                 carbus_analyzer_tx_add_ch(instance, CH2_OK);
//             } else {
//                 show_device_fifo_configuration(&can0_fifo_list[0], CAN0_FIFO_COUNT);
//                 show_device_filter_configuration(&can_filter_list[0], CAN0_FILTER_COUNT, false);
//                 carbus_analyzer_tx_add_ch(instance, CH2_BELL);
//             }
//         }
//     }
// }

// static void carbus_analyzer_can_transmit(CarBusAnalyzer* instance) {
//     furi_assert(instance);
//     furi_assert(
//         (instance->cmd_buf[0] == CH2_SEND_T11BIT_ID) ||
//         (instance->cmd_buf[0] == CH2_SEND_T29BIT_ID) ||
//         (instance->cmd_buf[0] == CH2_SEND_R11BIT_ID) ||
//         (instance->cmd_buf[0] == CH2_SEND_R29BIT_ID));

//     if(instance->can_open) {
//         uint8_t is_exended = 0;
//         uint8_t is_rtr = 0;

//         switch(instance->cmd_buf[0]) {
//         case CH2_SEND_T11BIT_ID:
//             break;
//         case CH2_SEND_T29BIT_ID:
//             is_exended = 1;
//             break;
//         case CH2_SEND_R11BIT_ID:
//             is_rtr = 1;
//             break;
//         case CH2_SEND_R29BIT_ID:
//             is_exended = 1;
//             is_rtr = 1;
//             break;
//         default:
//             carbus_analyzer_tx_add_ch(instance, CH2_BELL);
//             return;
//         }

//         uint8_t offset = 1;
//         MCP251XFD_CANMessage tx_msg;
//         uint8_t payload_data[8] = {0};
//         tx_msg.PayloadData = &payload_data[0];
//         tx_msg.MessageID = 0;
//         tx_msg.ControlFlags = 0;
//         uint8_t id_chars = is_exended ? 8 : 3;
//         for(int i = 0; i < id_chars; i++) {
//             tx_msg.MessageID <<= 4;
//             tx_msg.MessageID += hexCharToByte(instance->cmd_buf[offset++]);
//         }
//         if(is_rtr) {
//             tx_msg.ControlFlags |= MCP251XFD_REMOTE_TRANSMISSION_REQUEST;
//         }

//         if(is_exended) {
//             tx_msg.ControlFlags |= MCP251XFD_CAN20_FRAME | MCP251XFD_EXTENDED_MESSAGE_ID;
//         } else {
//             tx_msg.ControlFlags |= MCP251XFD_CAN20_FRAME | MCP251XFD_STANDARD_MESSAGE_ID;
//         }

//         tx_msg.DLC = hexCharToByte(instance->cmd_buf[offset++]);
//         if(tx_msg.DLC > 8 || (tx_msg.DLC == 0 && !is_rtr)) {
//             FURI_LOG_E(TAG, "DLC > 8 or DLC == 0");
//             carbus_analyzer_tx_add_ch(instance, CH2_BELL);
//             return;
//         }

//         if(!is_rtr) {
//             for(uint8_t i = 0; i < tx_msg.DLC; i++) {
//                 char hiHex = instance->cmd_buf[offset++];
//                 char loHex = instance->cmd_buf[offset++];
//                 payload_data[i] = hexCharToByte(loHex) + (hexCharToByte(hiHex) << 4);
//             }
//         }
//         if(can0_function_transmit_msg(&tx_msg, &instance->seq_counter, MCP251XFD_TXQ, true) ==
//            ERR_OK) {
//             carbus_analyzer_tx_add_ch(instance, CH2_OK);
//         } else {
//             carbus_analyzer_tx_add_ch(instance, CH2_BELL);
//         }
//     } else {
//         carbus_analyzer_tx_add_ch(instance, CH2_BELL);
//     }
// }

// static void carbus_analyzer_process_set_fifo(CarBusAnalyzer* instance) {
//     furi_assert(instance);
//     furi_assert(instance->cmd_buf[0] == CH2_SET_ACR);
//     //Todo: add fifo support
//     carbus_analyzer_tx_add_ch(instance, CH2_BELL);
// }

// static void carbus_analyzer_process_set_filter_list(CarBusAnalyzer* instance) {
//     furi_assert(instance);
//     furi_assert(instance->cmd_buf[0] == CH2_SET_AMR);

//     //Todo: add filter support
//     carbus_analyzer_tx_add_ch(instance, CH2_BELL);
// }

void carbus_analyzer_process_cmd(CarBusAnalyzer* instance) {
    furi_assert(instance);
    switch(instance->cmd.cmd) {
    case CBA_CMD_GET_PROTOCOL_TYPE:
        FURI_LOG_D(TAG, "CBA_CMD_GET_PROTOCOL_TYPE");
        instance->cmd.cmd = CBA_CMD_GET_PROTOCOL_TYPE_RESPONSE;
        //instance->cmd.cmd_counter = 0;
        instance->cmd.cmd_extended = CBA_CMD_GET_PROTOCOL_TYPE_RESPONSE;
        instance->cmd.data_len = 0;
        carbus_analyzer_tx_add_cmd(instance);
        break;
    case CBA_CMD_GET_DEVICE_TYPE:
        FURI_LOG_D(TAG, "CBA_CMD_GET_DEVICE_TYPE");
        instance->cmd.cmd = CBA_CMD_GET_DEVICE_TYPE;
        //instance->cmd.cmd_counter = 0;
        instance->cmd.cmd_extended = 0;
        instance->cmd.data_len = 1;

        instance->cmd_buf[0] = CBA_CMD_GET_DEVICE_TYPE_RESPONSE;

        carbus_analyzer_tx_add_cmd(instance);
        carbus_analyzer_tx_add_data(instance, instance->cmd_buf, instance->cmd.data_len);
        break;
    case CBA_CMD_GET_DEVICE_NAME:
        FURI_LOG_D(TAG, "CBA_CMD_GET_DEVICE_NAME");
        instance->cmd.cmd = CBA_CMD_GET_DEVICE_NAME;
        instance->cmd.cmd_extended = 0;
        instance->cmd.data_len = strlen(CBA_CMD_GET_DEVICE_NAME_RESPONSE);
        furi_assert(instance->cmd.data_len <= CARBUS_ANALYZER_CMD_BUF_SIZE);
        memcpy(instance->cmd_buf, CBA_CMD_GET_DEVICE_NAME_RESPONSE, instance->cmd.data_len);

        carbus_analyzer_tx_add_cmd(instance);
        carbus_analyzer_tx_add_data(instance, instance->cmd_buf, instance->cmd.data_len);
        break;
    case CBA_CMD_GET_DEVICE_VERSION:
        FURI_LOG_D(TAG, "CBA_CMD_GET_DEVICE_VERSION");
        instance->cmd.cmd = CBA_CMD_GET_DEVICE_VERSION;
        instance->cmd.cmd_extended = 0;
        instance->cmd.data_len = strlen(CBA_CMD_GET_DEVICE_VERSION_RESPONSE);
        furi_assert(instance->cmd.data_len <= CARBUS_ANALYZER_CMD_BUF_SIZE);
        memcpy(instance->cmd_buf, CBA_CMD_GET_DEVICE_VERSION_RESPONSE, instance->cmd.data_len);

        carbus_analyzer_tx_add_cmd(instance);
        carbus_analyzer_tx_add_data(instance, instance->cmd_buf, instance->cmd.data_len);
        break;
    case CBA_CMD_RESET_PERIPHERAL:
        FURI_LOG_D(TAG, "CBA_CMD_RESET_PERIPHERAL");
        instance->cmd.cmd = CBA_CMD_RESET_PERIPHERAL | CBA_CMD_FLAG_OK;
        instance->cmd.cmd_extended = 0;
        instance->cmd.data_len = 0;

        carbus_analyzer_tx_add_cmd(instance);
        break;
    case CBA_CMD_GET_DEVICE_SERIAL:
        FURI_LOG_D(TAG, "CBA_CMD_GET_DEVICE_SERIAL");
        instance->cmd.cmd = CBA_CMD_GET_DEVICE_SERIAL;
        instance->cmd.cmd_extended = 0;
        instance->cmd.data_len = 8;
        furi_assert(instance->cmd.data_len <= CARBUS_ANALYZER_CMD_BUF_SIZE);
        uint8_t serial[8] = {CBA_CMD_GET_DEVICE_SERIAL_RESPONSE};
        memcpy(instance->cmd_buf, serial, instance->cmd.data_len);

        carbus_analyzer_tx_add_cmd(instance);
        carbus_analyzer_tx_add_data(instance, instance->cmd_buf, instance->cmd.data_len);
        break;

    case CBA_CMD_DICONNECT_PERIPHERAL:
        FURI_LOG_D(TAG, "CBA_CMD_DICONNECT_PERIPHERAL");
        instance->cmd.cmd = CBA_CMD_NO_CORRECT_RESPONSE | CBA_CMD_FLAG_OK;
        instance->cmd.cmd_extended = 0;
        instance->cmd.data_len = 0;

        carbus_analyzer_tx_add_cmd(instance);
        break;
    case CBA_CMD_SET_CONFIG_START:
        FURI_LOG_D(TAG, "CBA_CMD_SET_CONFIG_START");
        instance->cmd.cmd = CBA_CMD_SET_CONFIG_START | CBA_CMD_FLAG_OK;
        instance->cmd.cmd_extended = 0;
        instance->cmd.data_len = 0;

        carbus_analyzer_tx_add_cmd(instance);
        break;
    case CBA_CMD_SET_CONFIG_CHANNEL:
        //Todo load onfig by channels
        FURI_LOG_D(TAG, "CBA_CMD_SET_CONFIG_CHANNEL");
        instance->cmd.cmd = CBA_CMD_SET_CONFIG_CHANNEL | CBA_CMD_FLAG_OK;
        instance->cmd.cmd_extended = 0;
        instance->cmd.data_len = 0;

        carbus_analyzer_tx_add_cmd(instance);
        break;
    case CBA_CMD_SET_CONFIG_DATA:
        //Todo load onfig by channels
        FURI_LOG_D(TAG, "CBA_CMD_SET_CONFIG_DATA");
        
        carbus_analyzer_get_data(instance);

        instance->cmd.cmd = CBA_CMD_SET_CONFIG_DATA | CBA_CMD_FLAG_OK;
        instance->cmd.cmd_extended = 0;
        instance->cmd.data_len = 0;

        carbus_analyzer_tx_add_cmd(instance);
        break;
    case CBA_CMD_GET_LICENCE:
        FURI_LOG_D(TAG, "CBA_CMD_GET_PROTOCOL_TYPE");
        carbus_analyzer_get_data(instance);
        instance->cmd.cmd = CBA_CMD_GET_LICENCE;
        instance->cmd.cmd_extended = 0;
        instance->cmd.data_len = 0;
        carbus_analyzer_tx_add_cmd(instance);
        break;

    default:
        FURI_LOG_E(TAG, "Unknown command: 0x%02X", instance->cmd.cmd);
        carbus_analyzer_get_data(instance);
        carbus_analyzer_tx_add_data(instance, (uint8_t*)&cmd_of_end, sizeof(CarBusAnalyzerCmd));
        break;
    }
    if(instance->callback) {
        instance->callback(instance->context);
    }
}

// static void carbus_analyzer_can_receive(CarBusAnalyzer* instance) {
//     furi_assert(instance);

//     if(instance->rx_msg.ControlFlags & MCP251XFD_TRANSMIT_ERROR_PASSIVE) {
//         FURI_LOG_E(TAG, "Transmit error passive");
//         return;
//     }

//     uint8_t data[32] = {0};
//     uint8_t offset = 0;

//     uint8_t is_rtr = (instance->rx_msg.ControlFlags & MCP251XFD_REMOTE_TRANSMISSION_REQUEST) ? 1 :
//                                                                                                0;
//     uint8_t is_exended = (instance->rx_msg.ControlFlags & MCP251XFD_EXTENDED_MESSAGE_ID) ? 1 : 0;

//     if(is_exended) {
//         data[offset++] = is_rtr ? CH2_SEND_R29BIT_ID : CH2_SEND_T29BIT_ID;
//         put_eff_id(data + offset, instance->rx_msg.MessageID);
//         offset += 8;
//     } else {
//         data[offset++] = is_rtr ? CH2_SEND_R11BIT_ID : CH2_SEND_T11BIT_ID;
//         put_sff_id(data + offset, instance->rx_msg.MessageID);
//         offset += 3;
//     }

//     data[offset++] = hex_asc_upper_lo(instance->rx_msg.DLC);

//     if(!is_rtr) {
//         for(uint8_t i = 0; i < instance->rx_msg.DLC; i++) {
//             put_hex_byte(data + offset, instance->rx_msg.PayloadData[i]);
//             offset += 2;
//         }
//     }

//     if(instance->can_timestamp) {
//         //FURI_LOG_I(TAG, "Timestamp: %ld", instance->rx_msg_time_stamp);
//         uint16_t ts = (uint16_t)(instance->rx_msg_time_stamp / 40);
//         put_hex_byte(data + offset, ts >> 8);
//         offset += 2;
//         put_hex_byte(data + offset, ts);
//         offset += 2;
//     }

//     data[offset++] = CH2_OK;
//     data[offset] = '\0';
//     carbus_analyzer_tx_add_data(instance, data, offset);
//     if(instance->callback) {
//         instance->callback(instance->context);
//     }
// }

void carbus_analyzer_process(CarBusAnalyzer* instance) {
    furi_assert(instance);
    carbus_analyzer_get_cmd(instance);

    if(instance->can_open) {
        can0_function_device_check_irq(); //check irq
        bool receive_event = false;
        do {
            receive_event = false;
            can0_function_receive_msg(
                CAN0,
                &receive_event,
                &instance->rx_msg,
                MCP251XFD_PAYLOAD_8BYTE,
                &instance->rx_msg_time_stamp);
            if(receive_event) {
                //carbus_analyzer_can_receive(instance);
            }
        } while(receive_event);
    }
}