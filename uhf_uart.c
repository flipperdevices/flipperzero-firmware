#include "uhf_uart.h"


int32_t uhf_uart_worker_callback(void *ctx){
    // UHFUart* uart = (UHFUart*)ctx;
    UNUSED(ctx);
    // while(true){
    //     if(uart->tick > 0){
    //         furi_thread_sleep(uart->tick);
    //         uart->tick--;
    //     }
    // }
    return 0;
}
void uhf_uart_default_rx_callback(FuriHalSerialHandle *handle, FuriHalSerialRxEvent event, void* ctx) {
    UHFUart* uart = (UHFUart*)ctx;
    Buffer* buffer = (Buffer*)uart->buffer;
    if(buffer->closed) return; // buffer closed
    if(event != FuriHalSerialRxEventData) return; // not byte received
    uint8_t data = furi_hal_serial_async_rx(handle); // read data
    buffer_append_single(buffer, data); // append data
    if(data == UHF_UART_FRAME_END) buffer_close(buffer); // end of frame
    uart->tick = UHF_UART_WAIT_TICK; // reset tick
}

UHFUart* uhf_uart_alloc(){
    UHFUart *uart = (UHFUart*)malloc(sizeof(UHFUart));
    uart->bus = FuriHalBusUSART1;
    uart->handle = furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    uart->init_by_app = !furi_hal_bus_is_enabled(uart->bus);
    uart->tick = UHF_UART_WAIT_TICK;
    if(uart->init_by_app){
        furi_hal_serial_init(uart->handle, uart->baudrate);
    }
    uhf_uart_set_baudrate(uart, UHF_UART_DEFAULT_BAUDRATE);
    uart->buffer = buffer_alloc(UHF_UART_RX_BUFFER_SIZE);
    uart->thread = furi_thread_alloc_ex("UHFUartWorker", UHF_UART_WORKER_STACK_SIZE, uhf_uart_worker_callback, uart);
    furi_thread_start(uart->thread);
    uhf_uart_set_receive_byte_callback(uart, uhf_uart_default_rx_callback, uart, false);
    return uart;
}   

void uhf_uart_free(UHFUart* uart){
    // furi_thread_flags_set(furi_thread_get_id(uart->thread), UHFUartWorkerExitingFlag);
    furi_thread_join(uart->thread);
    furi_thread_free(uart->thread);
    buffer_free(uart->buffer);
    if(uart->init_by_app){
        furi_hal_serial_deinit(uart->handle);
    }
    furi_hal_serial_control_release(uart->handle);
    free(uart);
}

void uhf_uart_set_receive_byte_callback(UHFUart* uart, FuriHalSerialAsyncRxCallback callback, void *ctx, bool report_errors){
    furi_hal_serial_async_rx_start(uart->handle, callback, ctx, report_errors);
}

void uhf_uart_send(UHFUart* uart, uint8_t* data, size_t size){
    furi_hal_serial_tx(uart->handle, data, size);
}

void uhf_uart_send_wait(UHFUart* uart, uint8_t* data, size_t size){
    uhf_uart_send(uart, data, size);
    furi_hal_serial_tx_wait_complete(uart->handle);
}

void uhf_uart_set_baudrate(UHFUart* uart, uint32_t baudrate){
    furi_hal_serial_set_br(uart->handle, baudrate);
    uart->baudrate = baudrate;
}