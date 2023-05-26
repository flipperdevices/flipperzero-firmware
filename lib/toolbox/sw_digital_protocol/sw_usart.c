#include "sw_usart.h"
#include <furi_hal_resources.h>
#include <furi_hal_cortex.h>
#include <furi_hal_sw_digital_pin.h>

#define TAG "SwUsart"

#define SW_USART_RX_BUFFER_SIZE_DEFAULT 1024

#define CPU_CLOCK_TIM 64000000

typedef struct {
    SwUsartMode mode;
    uint32_t baudrate;
    SwUsartWordLength data_bit;
    SwUsartParity parity;
    SwUsartStopBits stop_bit;
    bool inverted;
    bool sync;
    const GpioPin* rx_pin;
    const GpioPin* tx_pin;
    const GpioPin* sync_pin;
} SwUsartConfig;

struct SwUsart {
    SwUsartConfig config;

    uint8_t* tx_buffer;
    size_t tx_buffer_len;
    size_t tx_buffer_pos_byte;
    size_t tx_buffer_pos_bit;

    uint32_t* tx_upload_char;
    size_t tx_upload_char_len;
    uint8_t tx_upload_char_index;

    uint32_t tx_pin_set;
    uint32_t tx_pin_reset;

    FuriStreamBuffer* rx_stream;
    uint8_t rx_buffer_parse_byte[24];
    uint8_t rx_buffer_parse_byte_pos;

    SwUsartTxCallbackEnd tx_callback_end;
    void* tx_context_end;
};

static void sw_usart_tx_char_encoder(SwUsart* sw_usart) {
    uint8_t parity = 0;
    uint8_t ind = 0;

    if(sw_usart->tx_buffer_pos_byte >= sw_usart->tx_buffer_len) {
        memset(sw_usart->tx_upload_char, 0, sizeof(uint32_t) * sw_usart->tx_upload_char_len);
        return;
    }

    // Start bit
    sw_usart->tx_upload_char[ind] = sw_usart->tx_pin_reset;

    // Data
    do {
        if((sw_usart->tx_buffer[sw_usart->tx_buffer_pos_byte] >> ind) & 0x1) {
            sw_usart->tx_upload_char[++ind] = sw_usart->tx_pin_set;
            parity++;
        } else {
            sw_usart->tx_upload_char[++ind] = sw_usart->tx_pin_reset;
        }
    } while(ind < sw_usart->config.data_bit);
    ind++;
    sw_usart->tx_buffer_pos_byte++;

    // Parity
    if(sw_usart->config.parity != SwUsartParityNone) {
        if(sw_usart->config.parity == SwUsartParityEven) {
            if(parity & 0x1) {
                sw_usart->tx_upload_char[ind++] = sw_usart->tx_pin_set;
            } else {
                sw_usart->tx_upload_char[ind++] = sw_usart->tx_pin_reset;
            }
        } else {
            if(parity & 0x1) {
                sw_usart->tx_upload_char[ind++] = sw_usart->tx_pin_reset;
            } else {
                sw_usart->tx_upload_char[ind++] = sw_usart->tx_pin_set;
            }
        }
    }

    // Stop bits
    if(sw_usart->config.stop_bit == SwUsartStopBit1) {
        sw_usart->tx_upload_char[ind] = sw_usart->tx_pin_set;
    } else {
        sw_usart->tx_upload_char[ind++] = sw_usart->tx_pin_set;
        sw_usart->tx_upload_char[ind] = sw_usart->tx_pin_set;
    }
}

static uint32_t sw_usart_tx_encoder_yield(void* context) {
    //furi_assert(context);
    SwUsart* sw_usart = context;
    if(sw_usart->tx_upload_char_index >= sw_usart->tx_upload_char_len) {
        sw_usart_tx_char_encoder(sw_usart);
        sw_usart->tx_upload_char_index = 0;
    }
    return sw_usart->tx_upload_char[sw_usart->tx_upload_char_index++];
}

static void sw_usart_tx_end(void* context) {
    SwUsart* sw_usart = context;
    furi_assert(sw_usart);

    if(sw_usart->config.mode == SwUsartModeRxTxHalfDuplex) {
        furi_hal_sw_digital_pin_switch_tx_to_rx();
    }

    if(sw_usart->tx_callback_end) {
        sw_usart->tx_callback_end(sw_usart->tx_context_end);
    }
}

static inline void sw_usart_parse_byte(SwUsart* sw_usart, uint8_t base) {
    furi_assert(sw_usart);
    uint8_t parity = 0;
    uint8_t byte = 0;
    uint8_t* ptr_buf = sw_usart->rx_buffer_parse_byte + base;

    //Data
    uint8_t i = 0;
    do {
        ptr_buf += 2;
        if(*(ptr_buf)) {
            byte |= 1 << i;
            parity++;
        }
    } while(++i < sw_usart->config.data_bit);

    do {
        // Check parity
        if(sw_usart->config.parity != SwUsartParityNone) {
            if(sw_usart->config.parity == SwUsartParityEven) {
                ptr_buf += 2;
                if((parity & 0x1) != *ptr_buf) {
                    break;
                }
            } else {
                ptr_buf += 2;
                if((parity & 0x1) == *ptr_buf) {
                    break;
                }
            }
        }

        // Check stop bits
        ptr_buf += 2;
        if(!(*ptr_buf)) {
            break;
        }
        if(sw_usart->config.stop_bit == SwUsartStopBit2) {
            ptr_buf += 2;
            if(!(*ptr_buf)) {
                break;
            }
        }

        //Todo check overrun?
        furi_stream_buffer_send(sw_usart->rx_stream, &byte, 1, 0);
    } while(false);
}

static void sw_usart_rx(void* context, SwDigitalPinRx data) {
    SwUsart* sw_usart = context;
    furi_assert(sw_usart);
    uint16_t ind = 0;
    uint32_t mask = sw_usart->config.rx_pin->pin;

    do {
        if(sw_usart->rx_buffer_parse_byte_pos == 0) {
            //Found start bit
            if((data.rx_buff[ind++] & mask)) {
                continue;
            } else {
                //sw_usart->rx_buffer_parse_byte[sw_usart->rx_buffer_parse_byte_pos++] = 0;
                sw_usart->rx_buffer_parse_byte_pos++;
            }
        }

        if(ind < data.rx_buff_size) {
            sw_usart->rx_buffer_parse_byte[sw_usart->rx_buffer_parse_byte_pos++] =
                (data.rx_buff[ind++] & mask ? 1 : 0);
            if(sw_usart->rx_buffer_parse_byte_pos == sw_usart->tx_upload_char_len * 2) {
                //if there is the required number of bits, parse the data
                uint8_t i = 0;
                do {
                    if(sw_usart->rx_buffer_parse_byte[i] == 1) {
                        i &= 0x01;
                        if(i)
                            sw_usart->rx_buffer_parse_byte[sw_usart->rx_buffer_parse_byte_pos++] =
                                (data.rx_buff[ind++] & mask ? 1 : 0);
                        break;
                    }
                    i++;
                } while(ind < sw_usart->rx_buffer_parse_byte_pos);
                sw_usart->rx_buffer_parse_byte_pos = 0;
                sw_usart_parse_byte(sw_usart, i);
            }
        }
    } while(ind < data.rx_buff_size);
}

static void sw_usart_rx_inverted(void* context, SwDigitalPinRx data) {
    SwUsart* sw_usart = context;
    furi_assert(sw_usart);
    uint16_t ind = 0;
    uint32_t mask = sw_usart->config.rx_pin->pin;

    do {
        if(sw_usart->rx_buffer_parse_byte_pos == 0) {
            //Found start bit
            if(!(data.rx_buff[ind++] & mask)) {
                continue;
            } else {
                //sw_usart->rx_buffer_parse_byte[sw_usart->rx_buffer_parse_byte_pos++] = 0;
                sw_usart->rx_buffer_parse_byte_pos++;
            }
        }

        if(ind < data.rx_buff_size) {
            sw_usart->rx_buffer_parse_byte[sw_usart->rx_buffer_parse_byte_pos++] =
                (data.rx_buff[ind++] & mask ? 0 : 1);
            if(sw_usart->rx_buffer_parse_byte_pos == sw_usart->tx_upload_char_len * 2) {
                //if there is the required number of bits, parse the data
                uint8_t i = 0;
                do {
                    if(sw_usart->rx_buffer_parse_byte[i] == 1) {
                        i &= 0x01;
                        if(i)
                            sw_usart->rx_buffer_parse_byte[sw_usart->rx_buffer_parse_byte_pos++] =
                                (data.rx_buff[ind++] & mask ? 0 : 1);
                        break;
                    }
                    i++;
                } while(ind < sw_usart->rx_buffer_parse_byte_pos);
                sw_usart->rx_buffer_parse_byte_pos = 0;
                sw_usart_parse_byte(sw_usart, i);
            }
        }
    } while(ind < data.rx_buff_size);
}

SwUsart* sw_usart_alloc(void) {
    SwUsart* sw_usart = malloc(sizeof(SwUsart));

    sw_usart->config.mode = SwUsartModeRxTx;
    sw_usart->config.baudrate = 115200;
    sw_usart->config.data_bit = SwUsartDataBit8;
    sw_usart->config.parity = SwUsartParityNone;
    sw_usart->config.stop_bit = SwUsartStopBit1;
    sw_usart->config.inverted = false;
    sw_usart->config.sync = false;
    sw_usart->config.rx_pin = NULL;
    sw_usart->config.tx_pin = NULL;
    sw_usart->config.sync_pin = NULL;

    sw_usart->tx_upload_char_len = 1 + sw_usart->config.data_bit + sw_usart->config.stop_bit;
    if(sw_usart->config.parity != SwUsartParityNone) {
        sw_usart->tx_upload_char_len += 1;
    }
    sw_usart->tx_upload_char = malloc(sizeof(uint32_t) * sw_usart->tx_upload_char_len);

    sw_usart->rx_stream = furi_stream_buffer_alloc(
        sizeof(uint8_t) * SW_USART_RX_BUFFER_SIZE_DEFAULT, sizeof(uint8_t));

    return sw_usart;
}

void sw_usart_free(SwUsart* sw_usart) {
    furi_assert(sw_usart);

    if(sw_usart->config.sync) {
        furi_hal_sw_digital_pin_sync_deinit();
    }

    switch(sw_usart->config.mode) {
    case SwUsartModeOnlyTx:
        furi_hal_sw_digital_pin_tx_deinit();
        break;
    case SwUsartModeOnlyRx:
        furi_hal_sw_digital_pin_rx_deinit();
        break;

    case SwUsartModeRxTx:
    case SwUsartModeRxTxHalfDuplex:
        furi_hal_sw_digital_pin_tx_deinit();
        furi_hal_sw_digital_pin_rx_deinit();
        break;
    default:
        break;
    }

    furi_stream_buffer_free(sw_usart->rx_stream);
    free(sw_usart->tx_upload_char);
    free(sw_usart);
}

void sw_uasart_set_tx_callback(SwUsart* sw_usart, SwUsartTxCallbackEnd callback, void* context) {
    furi_assert(sw_usart);
    sw_usart->tx_callback_end = callback;
    sw_usart->tx_context_end = context;
}

void sw_usart_set_rx_buffer_size(SwUsart* sw_usart, size_t size) {
    furi_assert(sw_usart);
    furi_assert(size);
    furi_stream_buffer_free(sw_usart->rx_stream);
    sw_usart->rx_stream = furi_stream_buffer_alloc(sizeof(uint8_t) * size, sizeof(uint8_t));
}

void sw_usart_set_config(
    SwUsart* sw_usart,
    SwUsartMode mode,
    SwUsartWordLength data_bit,
    SwUsartParity parity,
    SwUsartStopBits stop_bit) {
    furi_assert(sw_usart);
    sw_usart->config.mode = mode;
    sw_usart->config.data_bit = data_bit;
    sw_usart->config.parity = parity;
    sw_usart->config.stop_bit = stop_bit;
}

void sw_usart_set_sync_config(SwUsart* sw_usart, const GpioPin* gpio, bool inverse) {
    furi_assert(sw_usart);
    sw_usart->config.sync = true;
    sw_usart->config.sync_pin = gpio;
    furi_hal_sw_digital_pin_sync_init(gpio, inverse);
}

void sw_usart_sync_start(SwUsart* sw_usart) {
    furi_assert(sw_usart);
    furi_assert(sw_usart->config.sync);
    furi_hal_sw_digital_pin_sync_start();
}

void sw_usart_sync_stop(SwUsart* sw_usart) {
    furi_assert(sw_usart);
    furi_assert(sw_usart->config.sync);
    furi_hal_sw_digital_pin_sync_stop();
}

static uint16_t sw_usart_get_arr(uint32_t speed) {
    return (CPU_CLOCK_TIM / speed) - 1;
}

static void sw_usart_tx_init(SwUsart* sw_usart) {
    furi_assert(sw_usart);
    furi_check(sw_usart->config.tx_pin);

    furi_hal_gpio_write(sw_usart->config.tx_pin, (sw_usart->config.inverted ? false : true));
    furi_hal_sw_digital_pin_tx_init(
        sw_usart_tx_encoder_yield,
        sw_usart_tx_end,
        sw_usart,
        0,
        sw_usart_get_arr(sw_usart->config.baudrate),
        sw_usart->tx_upload_char_len * 8,
        sw_usart->config.tx_pin);
}

static void sw_usart_rx_init(SwUsart* sw_usart) {
    furi_assert(sw_usart);
    furi_check(sw_usart->config.rx_pin);

    if(!sw_usart->config.inverted) {
        furi_hal_sw_digital_pin_rx_init(
            sw_usart_rx,
            sw_usart,
            0,
            sw_usart_get_arr(sw_usart->config.baudrate) / 2, // oversampling x2
            512,
            sw_usart->config.rx_pin);
    } else {
        furi_hal_sw_digital_pin_rx_init(
            sw_usart_rx_inverted,
            sw_usart,
            0,
            sw_usart_get_arr(sw_usart->config.baudrate) / 2, // oversampling x2
            512,
            sw_usart->config.rx_pin);
    }
    furi_hal_sw_digital_pin_rx_start();
}

void sw_usart_start(
    SwUsart* sw_usart,
    uint32_t baudrate,
    const GpioPin* tx_pin,
    const GpioPin* rx_pin,
    bool inverted) {
    furi_assert(sw_usart);
    sw_usart->config.baudrate = baudrate;
    sw_usart->config.tx_pin = tx_pin;
    sw_usart->config.rx_pin = rx_pin;
    sw_usart->config.inverted = inverted;

    if(!sw_usart->config.inverted) {
        sw_usart->tx_pin_set = sw_usart->config.tx_pin->pin;
        sw_usart->tx_pin_reset = sw_usart->config.tx_pin->pin << GPIO_NUMBER;
    } else {
        sw_usart->tx_pin_set = sw_usart->config.tx_pin->pin << GPIO_NUMBER;
        sw_usart->tx_pin_reset = sw_usart->config.tx_pin->pin;
    }

    switch(sw_usart->config.mode) {
    case SwUsartModeOnlyTx:
        sw_usart_tx_init(sw_usart);
        break;
    case SwUsartModeOnlyRx:
        sw_usart_rx_init(sw_usart);
        break;

    case SwUsartModeRxTx:
    case SwUsartModeRxTxHalfDuplex:
        sw_usart_tx_init(sw_usart);
        sw_usart_rx_init(sw_usart);
        break;
    default:
        break;
    }
}

void sw_usart_dma_tx(SwUsart* sw_usart, uint8_t* data, uint8_t len) {
    furi_assert(sw_usart);
    furi_assert(data);
    furi_assert(len);

    sw_usart->tx_buffer = data;
    sw_usart->tx_buffer_len = len;
    sw_usart->tx_buffer_pos_byte = 0;
    sw_usart->tx_buffer_pos_bit = 0;
    sw_usart->tx_upload_char_index = 0;

    sw_usart_tx_char_encoder(sw_usart);

    //furi_hal_sw_digital_pin_rx_start();
    if(sw_usart->config.mode == SwUsartModeRxTxHalfDuplex) {
        furi_hal_sw_digital_pin_switch_rx_to_tx();
    } else {
        furi_hal_sw_digital_pin_tx_start();
    }
}

void sw_usart_tx(SwUsart* sw_usart, uint8_t* data, uint8_t len, uint32_t timeout_ms) {
    furi_assert(sw_usart);
    furi_assert(data);
    furi_assert(len);

    FuriHalCortexTimer timer = furi_hal_cortex_timer_get(timeout_ms * 1000);
    sw_usart_dma_tx(sw_usart, data, len);
    while(!sw_usart_is_end_tx(sw_usart)) {
        if(furi_hal_cortex_timer_is_expired(timer)) {
            //timeout
            break;
        }
    }
}

bool sw_usart_is_end_tx(SwUsart* sw_usart) {
    furi_assert(sw_usart);
    UNUSED(sw_usart);
    return furi_hal_sw_digital_pin_is_tx_complete();
}

void sw_usart_print_debug_data(SwUsart* sw_usart) {
    furi_assert(sw_usart);
    if(!furi_stream_buffer_bytes_available(sw_usart->rx_stream)) return;
    uint8_t data = 0;
    while(furi_stream_buffer_bytes_available(sw_usart->rx_stream)) {
        furi_stream_buffer_receive(sw_usart->rx_stream, &data, sizeof(data), 10);
        printf("%c", data);
    }
}

size_t sw_usart_available(SwUsart* sw_usart) {
    furi_assert(sw_usart);
    return furi_stream_buffer_bytes_available(sw_usart->rx_stream);
}

bool sw_usart_read(SwUsart* sw_usart, uint8_t* data, uint32_t timeout_ms) {
    furi_assert(sw_usart);
    return furi_stream_buffer_receive(sw_usart->rx_stream, &data, 1, timeout_ms);
}

void sw_usart_read_line(SwUsart* sw_usart, FuriString* data, uint32_t timeout_ms) {
    furi_assert(sw_usart);
    furi_assert(data);

    uint8_t data_tmp = 0;
    furi_string_reset(data);
    do {
        if(!sw_usart_read(sw_usart, &data_tmp, timeout_ms)) {
            //timeout
            break;
        }
        if(data_tmp != '\r') {
            furi_string_push_back(data, data_tmp);
        }
    } while(data_tmp != '\n');
}
