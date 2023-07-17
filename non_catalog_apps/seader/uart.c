#include "seader_i.h"

#define TAG "SeaderUART"

void seader_uart_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    SeaderUartBridge* seader_uart = (SeaderUartBridge*)context;
    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(seader_uart->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(seader_uart->thread), WorkerEvtRxDone);
    }
}

void seader_uart_disable(SeaderUartBridge* seader_uart) {
    furi_assert(seader_uart);
    furi_thread_flags_set(furi_thread_get_id(seader_uart->thread), WorkerEvtStop);
    furi_thread_join(seader_uart->thread);
    furi_thread_free(seader_uart->thread);
    free(seader_uart);
}

void seader_uart_serial_init(SeaderUartBridge* seader_uart, uint8_t uart_ch) {
    UNUSED(seader_uart);
    furi_hal_uart_init(uart_ch, 115200);
    furi_hal_uart_set_irq_cb(uart_ch, seader_uart_on_irq_cb, seader_uart);
}

void seader_uart_serial_deinit(SeaderUartBridge* seader_uart, uint8_t uart_ch) {
    UNUSED(seader_uart);
    furi_hal_uart_set_irq_cb(uart_ch, NULL, NULL);
    furi_hal_uart_deinit(uart_ch);
}

void seader_uart_set_baudrate(SeaderUartBridge* seader_uart, uint32_t baudrate) {
    if(baudrate != 0) {
        furi_hal_uart_set_br(seader_uart->cfg.uart_ch, baudrate);
    } else {
        FURI_LOG_I(TAG, "No baudrate specified");
    }
}

size_t seader_uart_process_buffer(Seader* seader, uint8_t* cmd, size_t cmd_len) {
    SeaderWorker* seader_worker = seader->worker;
    SeaderUartBridge* seader_uart = seader->uart;
    if(cmd_len < 2) {
        return cmd_len;
    }

    size_t consumed = 0;
    do {
        consumed = processCCID(seader_worker, cmd, cmd_len);

        if(consumed > 0) {
            memset(cmd, 0, consumed);
            cmd_len -= consumed;
            if(cmd_len > 0) {
                memmove(cmd, cmd + consumed, cmd_len);
            }
            seader_uart->st.rx_cnt += consumed;

            /*
            memset(display, 0, SEADER_UART_RX_BUF_SIZE);
            for (uint8_t i = 0; i < cmd_len; i++) {
                snprintf(display+(i*2), sizeof(display), "%02x", cmd[i]);
            }
            FURI_LOG_I(TAG, "cmd is now %d bytes: %s", cmd_len, display);
            */
        }
    } while(consumed > 0 && cmd_len > 0);
    return cmd_len;
}

int32_t seader_uart_worker(void* context) {
    Seader* seader = (Seader*)context;
    SeaderUartBridge* seader_uart = seader->uart;

    memcpy(&seader_uart->cfg, &seader_uart->cfg_new, sizeof(SeaderUartConfig));

    seader_uart->rx_stream = furi_stream_buffer_alloc(SEADER_UART_RX_BUF_SIZE, 1);

    seader_uart->tx_sem = furi_semaphore_alloc(1, 1);

    seader_uart->tx_thread =
        furi_thread_alloc_ex("SeaderUartTxWorker", 2 * 1024, seader_uart_tx_thread, seader);

    seader_uart_serial_init(seader_uart, seader_uart->cfg.uart_ch);
    seader_uart_set_baudrate(seader_uart, seader_uart->cfg.baudrate);

    furi_thread_flags_set(furi_thread_get_id(seader_uart->tx_thread), WorkerEvtSamRx);

    furi_thread_start(seader_uart->tx_thread);

    uint8_t cmd[SEADER_UART_RX_BUF_SIZE];
    size_t cmd_len = 0;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check(!(events & FuriFlagError));
        if(events & WorkerEvtStop) {
            memset(cmd, 0, cmd_len);
            cmd_len = 0;
            break;
        }
        if(events & WorkerEvtRxDone) {
            size_t len = furi_stream_buffer_receive(
                seader_uart->rx_stream, seader_uart->rx_buf, SEADER_UART_RX_BUF_SIZE, 0);
            if(len > 0) {
                furi_delay_ms(5); //WTF

                /*
                char display[SEADER_UART_RX_BUF_SIZE * 2 + 1] = {0};
                for (uint8_t i = 0; i < len; i++) {
                    snprintf(display+(i*2), sizeof(display), "%02x", seader_uart->rx_buf[i]);
                }
                FURI_LOG_I(TAG, "RECV %d bytes: %s", len, display);
                */

                if(cmd_len + len > SEADER_UART_RX_BUF_SIZE) {
                    FURI_LOG_I(TAG, "OVERFLOW: %d + %d", cmd_len, len);
                    memset(cmd, 0, cmd_len);
                    cmd_len = 0;
                }

                memcpy(cmd + cmd_len, seader_uart->rx_buf, len);
                cmd_len += len;
                cmd_len = seader_uart_process_buffer(seader, cmd, cmd_len);
            }
        }
    }
    seader_uart_serial_deinit(seader_uart, seader_uart->cfg.uart_ch);

    furi_thread_flags_set(furi_thread_get_id(seader_uart->tx_thread), WorkerEvtTxStop);
    furi_thread_join(seader_uart->tx_thread);
    furi_thread_free(seader_uart->tx_thread);

    furi_stream_buffer_free(seader_uart->rx_stream);
    furi_semaphore_free(seader_uart->tx_sem);
    return 0;
}

SeaderUartBridge* seader_uart_enable(SeaderUartConfig* cfg, Seader* seader) {
    SeaderUartBridge* seader_uart = malloc(sizeof(SeaderUartBridge));

    memcpy(&(seader_uart->cfg_new), cfg, sizeof(SeaderUartConfig));

    seader_uart->thread =
        furi_thread_alloc_ex("SeaderUartWorker", 5 * 1024, seader_uart_worker, seader);

    furi_thread_start(seader_uart->thread);
    return seader_uart;
}

int32_t seader_uart_tx_thread(void* context) {
    Seader* seader = (Seader*)context;
    SeaderUartBridge* seader_uart = seader->uart;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_TX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check(!(events & FuriFlagError));
        if(events & WorkerEvtTxStop) break;
        if(events & WorkerEvtSamRx) {
            if(seader_uart->tx_len > 0) {
                /*
                char display[SEADER_UART_RX_BUF_SIZE * 2 + 1] = {0};
                for(uint8_t i = 0; i < seader_uart->tx_len; i++) {
                    snprintf(display + (i * 2), sizeof(display), "%02x", seader_uart->tx_buf[i]);
                }
                FURI_LOG_I(TAG, "SEND %d bytes: %s", seader_uart->tx_len, display);
                */
                seader_uart->st.tx_cnt += seader_uart->tx_len;
                furi_hal_uart_tx(
                    seader_uart->cfg.uart_ch, seader_uart->tx_buf, seader_uart->tx_len);
            }
        }
    }
    return 0;
}

void seader_uart_get_config(SeaderUartBridge* seader_uart, SeaderUartConfig* cfg) {
    furi_assert(seader_uart);
    furi_assert(cfg);
    memcpy(cfg, &(seader_uart->cfg_new), sizeof(SeaderUartConfig));
}

void seader_uart_get_state(SeaderUartBridge* seader_uart, SeaderUartState* st) {
    furi_assert(seader_uart);
    furi_assert(st);
    memcpy(st, &(seader_uart->st), sizeof(SeaderUartState));
}

SeaderUartBridge* seader_uart_alloc(Seader* seader) {
    SeaderUartConfig cfg = {.uart_ch = FuriHalUartIdLPUART1, .baudrate = 115200};
    SeaderUartState uart_state;
    SeaderUartBridge* seader_uart;

    FURI_LOG_I(TAG, "Enable UART");
    seader_uart = seader_uart_enable(&cfg, seader);

    seader_uart_get_config(seader_uart, &cfg);
    seader_uart_get_state(seader_uart, &uart_state);
    return seader_uart;
}

void seader_uart_free(SeaderUartBridge* seader_uart) {
    seader_uart_disable(seader_uart);
}
