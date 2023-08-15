#include "lin_bus.h"
#include "lin_uart.h"

//https://microchipdeveloper.com/local--files/lin:specification/LIN-Spec_2.2_Rev_A.PDF

#define LIN_SYNC_BYTE 0x55u
#define LIN_BUS_BREAK_TIMEOUT_MAX 1000 //symbol time
#define LIN_BUS_RX_BUFFER_LENGTH 32

typedef enum {
    LinBusStateSyncBreak,
    LinBusStateSyncField,
    LinBusStateId,
    LinBusStateData,
    LinBusStateCrc,
    LinBusStateEnd,
} LinBusState;

struct LinBus {
    LinBusMode mode;
    uint32_t baudrate;
    //LinBusState state;

    LinBusFrame frame;
    LinBusState tx_state; /* What part of the LIN message we will be transmitting */
    bool is_tx;
    bool only_rx; /* Determines if we will transmit anything or will be only receiveing */
    bool is_rx; /* Determines if we are receiving */

    uint8_t rx_buf_index;
    uint8_t rx_buf[LIN_BUS_RX_BUFFER_LENGTH];
};

/* Response timeout counter variables */
uint16_t responseTimeoutValue;
uint16_t responseTimeoutMax;

/* Timeout of receiving break (in case of short of LIN to Vbat) */
uint8_t breakTmrRunning;
uint16_t breakTmrVal;

static void lin_bus_reset_satate_machihe(LinBus* instance) {
    instance->tx_state = LinBusStateSyncBreak;
    instance->is_tx = false;
    instance->only_rx = false;
    instance->is_rx = false;
    instance->rx_buf_index = 0;
}

LinBus* lin_bus_init(LinBusMode mode, uint32_t baudrate) {
    LinBus* instance = (LinBus*)malloc(sizeof(LinBus));
    instance->mode = mode;
    instance->baudrate = baudrate;

    instance->is_tx = false;

    lin_uart_init(baudrate, instance);

    lin_uart_break_irq_enable();
    lin_uart_rx_irq_enable();
    lin_uart_tx_irq_disable();
    lin_bus_reset_satate_machihe(instance);

    return instance;
}

void lin_bus_deinit(LinBus* instance) {
    lin_uart_break_irq_disable();
    lin_uart_rx_irq_disable();
    lin_uart_tx_irq_disable();

    lin_uart_deinit();
    free(instance);
}

static uint8_t lin_bus_get_parity_id(uint8_t id) {
    uint8_t new_id = id & 0x3Fu;
    new_id |= ((id >> 0u & 0x1u) ^ (id >> 1u & 0x1u) ^ (id >> 2u & 0x1u) ^ (id >> 4u & 0x1u))
              << 6u;
    new_id |=
        ((id >> 1u & 0x1u) ^ (id >> 3u & 0x1u) ^ (id >> 4u & 0x1u) ^ (id >> 5u & 0x1u) ^ 0x1u)
        << 7u;
    return new_id;
}

static uint8_t
    lin_bus_get_crc(uint8_t id, uint8_t* data, uint8_t size, LinBusChecksumType crc_type) {
    uint16_t sum = 0x0000;
    if(crc_type == LinBusChecksumTypeEnanced) {
        sum += lin_bus_get_parity_id(id);
    }

    for(uint8_t i = 0; i < size; i++) sum += data[i];

    sum = (sum & 0x00FFu) + ((sum & 0xFF00u) >> 8u);
    if(sum & 0xFF00u) /* Did adding the carry bits result in a carry? */
    {
        sum += 1; /* Add the last carry */
    }

    sum &= 0x00FFu;
    return (uint8_t)(~sum);
}

static void lin_bus_reset(void* context) {
    lin_uart_timeout_stop();
    lin_uart_reset();
    lin_bus_reset_satate_machihe(context);
}

bool lin_bus_tx_async(LinBus* instance, LinBusFrame* lin_frame) {
    bool ret = false;
    if(!instance->is_tx && !instance->is_rx) {
        /* We can begin sending the data */
        instance->frame.id = lin_bus_get_parity_id(lin_frame->id);
        if(lin_frame->frame_type == LinBusMasterResponse) {
            memcpy(instance->frame.data, lin_frame->data, lin_frame->length);
        } else {
            instance->frame.response_length = lin_frame->response_length;
        }

        instance->frame.length = lin_frame->length;
        instance->frame.frame_type = lin_frame->frame_type;
        instance->frame.crc_type = lin_frame->crc_type;
        instance->frame.crc = lin_bus_get_crc(
            lin_frame->id, lin_frame->data, lin_frame->length, lin_frame->crc_type);

        /* Reset the index for receiving the data */
        instance->rx_buf_index = 0;
        instance->tx_state = LinBusStateSyncBreak;
        instance->is_tx = true; /* Transmission in progress */
        instance->only_rx = false;
        lin_uart_tx_irq_enable();
        ret = true;
    }
    return ret;
}

// Callbacks
void lin_bus_error_callback(void* context) {
    LinBus* instance = (LinBus*)context;
    UNUSED(instance);
}

void lin_bus_break_callback(void* context) {
    LinBus* instance = (LinBus*)context;

    if(instance->tx_state == LinBusStateSyncField) {
        /* Enable the tx interrupt again */
        lin_uart_tx_irq_enable();
        instance->tx_state = LinBusStateId;
        lin_uart_put_char(LIN_SYNC_BYTE);
    } else {
        lin_bus_reset(context);
    }
}

void lin_uart_rx_callback(uint8_t data, void* context) {
    LinBus* instance = (LinBus*)context;

    instance->rx_buf[instance->rx_buf_index++] = data;
    instance->rx_buf_index %= LIN_BUS_RX_BUFFER_LENGTH;

    if(instance->frame.frame_type == LinBusMasterRequest && instance->is_rx) {
        /* Last byte received */
        if(instance->rx_buf_index >= instance->frame.response_length + 1 + 3) {
            for(uint8_t i = 0;
                i < instance->frame.response_length + 1 && i < LIN_BUS_RX_BUFFER_LENGTH - 3;
                i++)
                instance->rx_buf[i] = instance->rx_buf[i + 3];

            instance->is_rx = 0;
            lin_uart_timeout_stop();
            uint8_t chkTmp;
            if(instance->rx_buf[instance->frame.response_length] ==
               (chkTmp = lin_bus_get_crc(
                    instance->frame.id,
                    instance->frame.data,
                    instance->frame.length,
                    instance->frame.crc_type))) {
                // LinBusFrame* pFr = FindRxFrame(txFrame.ID & 0x3f);
                // /* Copy the data to the scheduler frame buffer */
                // if(pFr != NULL) memcpy(pFr->Data, instance->rx_buf, txFrame.ResponseLength);
            } else {
                /* Bad checksum */
                lin_bus_error_callback(context);
            }
        }
    }
}

static uint16_t lin_bus_response_timeout(uint8_t data_size) {
    /* MaxResponseTimeoutValue is in Tbits. */
    return (data_size + 1) * 14;
}

void lin_bus_timeout_start(void) {
    responseTimeoutValue = 0;

    /* Reset the counter value */
    lin_uart_timeout_start();
}

void lin_uart_tx_callback(void* context) {
    LinBus* instance = (LinBus*)context;

    if(instance->only_rx) /* Stop transmitting (done or waiting for the response) */
    {
        if(instance->frame.frame_type == LinBusMasterRequest) {
            instance->is_rx = true;
            instance->is_tx = false;
            responseTimeoutMax = lin_bus_response_timeout(instance->frame.response_length);
            lin_uart_timeout_start();
            /* Disable the tx interrupt */
            lin_uart_tx_irq_disable();
        } else {
            instance->is_tx = false;
            instance->is_rx = false;
            /* Disable the tx interrupt */
            lin_uart_tx_irq_disable();
        }
    } else /* Continue with the transmission */
    {
        switch(instance->tx_state) {
        case LinBusStateSyncBreak:
            lin_uart_put_break();
            instance->tx_state = LinBusStateSyncField;
            /* Trigger timer waiting for break (for the situation when LIN is shorted to Vcc) */
            breakTmrRunning = 1;
            breakTmrVal = 0;
            lin_bus_timeout_start();

            /* Disable the tx interrupt until the break appears */
            lin_uart_tx_irq_disable();
            break;

        case LinBusStateId:
            if(instance->frame.frame_type == LinBusMasterResponse) {
                if(instance->frame.length != 0) {
                    instance->tx_state = LinBusStateData;
                    instance->rx_buf_index = 0;
                } else
                    instance->tx_state = LinBusStateCrc;
            } else if(instance->frame.frame_type == LinBusMasterRequest) {
                instance->tx_state = LinBusStateSyncBreak;
                /* Do not transmit anything else, wait for response */
                instance->only_rx = true;
            } else
                lin_bus_reset(context);

            /* Continue only if reset did not happen */
            if(instance->frame.frame_type == LinBusMasterRequest ||
               instance->frame.frame_type == LinBusMasterResponse)
                lin_uart_put_char(instance->frame.id);
            break;

        case LinBusStateData:
            if(instance->rx_buf_index + 1 >= instance->frame.length) {
                /* Last data byte will be transmitted, then the checksum */
                instance->tx_state = LinBusStateCrc;
            }
            lin_uart_put_char(instance->frame.data[instance->rx_buf_index++]);
            break;

        case LinBusStateCrc:
            instance->tx_state = LinBusStateSyncBreak;
            instance->only_rx = true; /* Last byte will be sent */
            lin_uart_put_char(instance->frame.crc);
            break;

        default: /* Bad state */
            lin_bus_reset(context);
            break;
        }
    }
}

void lin_bus_timeout_callback(void* context) {
    LinBus* instance = (LinBus*)context;

    if(breakTmrRunning) {
        /* Waiting for break - in case when LIN is shorted to Vbat */
        breakTmrVal++;
        if(breakTmrVal >= LIN_BUS_BREAK_TIMEOUT_MAX) {
            lin_uart_timeout_stop();
            breakTmrVal = breakTmrRunning = 0;
            lin_bus_reset(context);
        }
    } else if(instance->is_rx) /* Timeout for response running */
    {
        if(responseTimeoutValue < responseTimeoutMax)
            responseTimeoutValue++;
        else {
            instance->is_rx = false;
            lin_bus_error_callback(context);
            lin_uart_timeout_stop();
            lin_bus_reset_satate_machihe(context); /* So as not to hang on instance->is_rx == 1 */
        }
    }
}