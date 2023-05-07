#include "sw_usart.h"
#include <furi_hal_sw_digital_pin.h>

#define TAG "SwUsart"

#define bit_read(value, bit) (((value) >> (bit)) & 0x01)

struct SwUsart {
    SwUsartConfig* config;
    uint8_t* tx_buffer;
    size_t tx_buffer_len;
    size_t tx_buffer_pos_byte;
    size_t tx_buffer_pos_bit;

    uint32_t* tx_upload_char;
    size_t tx_upload_char_len;
    uint8_t tx_upload_char_index;
};

SwUsart* sw_usart_alloc(SwUsartConfig* config) {
    SwUsart* sw_usart = malloc(sizeof(SwUsart));
    sw_usart->config = malloc(sizeof(SwUsartConfig));
    memcpy(sw_usart->config, config, sizeof(SwUsartConfig));

    sw_usart->tx_upload_char_len = 1 + sw_usart->config->data_bit + sw_usart->config->stop_bit;

    if(sw_usart->config->parity != SwUsartParityNone) {
        sw_usart->tx_upload_char_len += 1;
    }

    sw_usart->tx_upload_char = malloc(sizeof(uint32_t) * sw_usart->tx_upload_char_len);

    switch(sw_usart->config->mode) {
    case SwUsartModeOnlyAsyncTx:
        furi_assert(sw_usart->config->tx_pin);
        furi_hal_sw_digital_pin_init();
        break;

    default:
        break;
    }
    return sw_usart;
}

void sw_usart_free(SwUsart* sw_usart) {
    furi_assert(sw_usart);

    free(sw_usart->tx_upload_char);
    free(sw_usart->config);
    free(sw_usart);
}
static bool sw_usart_get_bit_array(uint8_t* data_array, size_t read_index_bit) {
    return bit_read(data_array[read_index_bit >> 3], 7 - (read_index_bit & 0x7));
}

static void sw_usart_tx_char_encoder(SwUsart* sw_usart) {
    furi_assert(sw_usart);

    uint8_t parity = 0;
    uint8_t ind = 0;

    if(sw_usart->tx_buffer_pos_byte > sw_usart->tx_buffer_len) {
        memset(sw_usart->tx_upload_char, 0, sizeof(uint32_t) * sw_usart->tx_upload_char_len);
        return;
    }

    // Start bit
    sw_usart->tx_upload_char[ind++] = sw_usart->config->tx_pin->pin << GPIO_NUMBER;

    sw_usart->tx_buffer_pos_byte++;
    sw_usart->tx_buffer_pos_bit = sw_usart->tx_buffer_pos_byte * 8;
    for(uint8_t i = 0; i < sw_usart->config->data_bit; i++) {
        if(sw_usart_get_bit_array(sw_usart->tx_buffer, sw_usart->tx_buffer_pos_bit - i - 1)) {
            sw_usart->tx_upload_char[ind++] = sw_usart->config->tx_pin->pin;
            parity++;
        } else {
            sw_usart->tx_upload_char[ind++] = sw_usart->config->tx_pin->pin << GPIO_NUMBER;
        }
    }

    // Parity
    if(sw_usart->config->parity == SwUsartParityEven) {
        if(parity & 0x1) {
            sw_usart->tx_upload_char[ind++] = sw_usart->config->tx_pin->pin;
        } else {
            sw_usart->tx_upload_char[ind++] = sw_usart->config->tx_pin->pin << GPIO_NUMBER;
        }
    } else if(sw_usart->config->parity == SwUsartParityOdd) {
        if(parity & 0x1) {
            sw_usart->tx_upload_char[ind++] = sw_usart->config->tx_pin->pin << GPIO_NUMBER;
        } else {
            sw_usart->tx_upload_char[ind++] = sw_usart->config->tx_pin->pin;
        }
    }

    // Stop bits
    for(uint8_t i = 0; i < sw_usart->config->stop_bit; i++) {
        sw_usart->tx_upload_char[ind++] = sw_usart->config->tx_pin->pin;
    }
}

uint32_t sw_usart_tx_encoder_yield(void* context) {
    SwUsart* sw_usart = context;
    furi_assert(sw_usart);

    uint32_t ret = sw_usart->tx_upload_char[sw_usart->tx_upload_char_index++];
    if(sw_usart->tx_upload_char_index >= sw_usart->tx_upload_char_len) {
        sw_usart_tx_char_encoder(sw_usart);
        sw_usart->tx_upload_char_index = 0;
    }
    FURI_LOG_RAW_I("%d ", ret == (uint32_t)(sw_usart->config->tx_pin->pin << GPIO_NUMBER) ? 0 : 1);
    return ret;
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
    // set default value, pin tx
    furi_hal_gpio_write(sw_usart->config->tx_pin, true);

    sw_usart_tx_char_encoder(sw_usart);

    furi_hal_sw_digital_pin_tx_start(
        sw_usart_tx_encoder_yield, sw_usart, 24, sw_usart->config->tx_pin);
}

bool sw_usart_is_end_tx(SwUsart* sw_usart) {
    furi_assert(sw_usart);
    UNUSED(sw_usart);
    return furi_hal_sw_digital_pin_is_tx_complete();
}
