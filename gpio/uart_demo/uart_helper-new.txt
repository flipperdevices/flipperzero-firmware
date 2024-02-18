/**
 * UartHelper is a utility class that helps with reading lines of data from a UART.
 * It uses a stream buffer to receive data from the UART ISR, and a worker thread
 * to dequeue data from the stream buffer and process it.  The worker thread uses
 * a ring buffer to hold data until a delimiter is found, at which point the line
 * is extracted and the process_line callback is invoked.
 * 
 * @author CodeAllNight
*/

#include <furi_hal.h>
#include "ring_buffer.h"

/**
 * Callback invoked when a line is read from the UART.
*/
typedef void (*ProcessLine)(FuriString* line, void* context);

/**
 * UartHelper is a utility class that helps with reading lines of data from a UART.
*/
typedef struct {
    // UART bus & channel to use
    FuriHalBus uart_bus;
    FuriHalSerialHandle* serial_handle;
    bool uart_init_by_app;

    // Stream buffer to hold incoming data (worker will dequeue and process)
    FuriStreamBuffer* rx_stream;

    // Worker thread that dequeues data from the stream buffer and processes it
    FuriThread* worker_thread;

    // Buffer to hold data until a delimiter is found
    RingBuffer* ring_buffer;

    // Callback to invoke when a line is read
    ProcessLine process_line;
    void* context;
} UartHelper;

/**
 * WorkerEventFlags are used to signal the worker thread to exit or to process data.
 * Each flag is a bit in a 32-bit integer, so we can use the FuriThreadFlags API to
 * wait for either flag to be set.
*/
typedef enum {
    WorkerEventDataWaiting = 1 << 0, // bit flag 0 - data is waiting to be processed
    WorkerEventExiting = 1 << 1, // bit flag 1 - worker thread is exiting
} WorkerEventFlags;

/** 
 * Invoked when a byte of data is received on the UART bus.  This function
 * adds the byte to the stream buffer and sets the WorkerEventDataWaiting flag.
 * 
 * @param handle   Serial handle 
 * @param event    FuriHalSerialRxEvent
 * @param context  UartHelper instance
*/
static void uart_helper_received_byte_callback(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context) {
    UartHelper* helper = context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(helper->rx_stream, (void*)&data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(helper->worker_thread), WorkerEventDataWaiting);
    }
}

/** 
 * Worker thread that dequeues data from the stream buffer and processes it.  When
 * a delimiter is found in the data, the line is extracted and the process_line callback
 * is invoked. This thread will exit when the WorkerEventExiting flag is set.
 * 
 * @param context  UartHelper instance
 * @return         0
*/
static int32_t uart_helper_worker(void* context) {
    UartHelper* helper = context;
    FuriString* line = furi_string_alloc();
    uint32_t events;

    do {
        events = furi_thread_flags_wait(
            WorkerEventDataWaiting | WorkerEventExiting, FuriFlagWaitAny, FuriWaitForever);

        if(events & WorkerEventDataWaiting) {
            size_t length_read = 0;
            do {
                // We will read out of the stream into this temporary read_buffer in chunks
                // of up to 32 bytes.  A tricker implementation would be to read directly
                // into our ring_buffer, avoiding the extra copy.
                uint8_t read_buffer[32];

                size_t available = ring_buffer_available(helper->ring_buffer);
                if(available == 0) {
                    // In our case, the buffer is large enough to hold the entire response,
                    // so we should never hit this case & data loss is fine in the
                    // exceptional case.

                    // If loosing data is unacceptable, you could transfer the buffer into
                    // a string and prepend that to the response, or you could increase the
                    // size of the buffer.

                    // Buffer is full, so data loss will happen at beginning of string!
                    // We read one byte (hopefully the delimiter).
                    available = 1;
                }

                // We will read up to 32 bytes, but no more than the available space in the
                // ring buffer.
                size_t request_bytes = available < COUNT_OF(read_buffer) ? available :
                                                                           COUNT_OF(read_buffer);

                // hasDelim will be true if at least one delimiter was found in the data.
                bool hasDelim = false;

                // Read up to 32 bytes from the stream buffer into our temporary read_buffer.
                length_read =
                    furi_stream_buffer_receive(helper->rx_stream, read_buffer, request_bytes, 0);

                // Add the data to the ring buffer, check for delimiters, and process lines.
                if(length_read > 0) {
                    // Add the data to the ring buffer.  If at least one delimiter is found,
                    // hasDelim will be true.
                    hasDelim = ring_buffer_add(helper->ring_buffer, read_buffer, length_read);

                    if(hasDelim) {
                        size_t index;
                        do {
                            // Find the next delimiter in the ring buffer.
                            index = ring_buffer_find_delim(helper->ring_buffer);

                            // If a delimiter was found, extract the line and process it.
                            if(index != FURI_STRING_FAILURE) {
                                // Extract the line from the ring buffer, advancing the read
                                // pointer to the next byte after the delimiter.
                                ring_buffer_extract_line(helper->ring_buffer, index, line);

                                // Invoke the callback to process the line.
                                if(helper->process_line) {
                                    helper->process_line(line, helper->context);
                                }
                            }
                        } while(index != FURI_STRING_FAILURE);
                    }
                }
                // Keep reading until we have read all of the data from the stream buffer.
            } while(length_read > 0);
        }
        // Keep processing data until the WorkerEventExiting flag is set.
    } while((events & WorkerEventExiting) != WorkerEventExiting);

    furi_string_free(line);

    return 0;
}

UartHelper* uart_helper_alloc() {
    // rx_buffer_size should be large enough to hold the entire response from the device.
    const size_t rx_buffer_size = 2048;

    // worker_stack_size should be large enough stack for the worker thread (including functions it calls).
    const size_t worker_stack_size = 1024;

    // uart_baud is the default baud rate for the UART.
    const uint32_t uart_baud = 115200;

    // The uart_helper uses USART1.
    UartHelper* helper = malloc(sizeof(UartHelper));
    helper->uart_bus = FuriHalBusUSART1;
    helper->serial_handle = furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    // Typically the UART is already enabled and will assert if you try to enable again.
    helper->uart_init_by_app = !furi_hal_bus_is_enabled(helper->uart_bus);
    if(helper->uart_init_by_app) {
        furi_hal_serial_init(helper->serial_handle, uart_baud);
    } else {
        // If UART is already initialized, disable the console so it doesn't write to the UART.
        // furi_hal_console_disable();
    }

    // process_line callback gets invoked when a line is read.  By default the callback is not set.
    helper->process_line = NULL;

    // Set the baud rate for the UART
    furi_hal_serial_set_br(helper->serial_handle, uart_baud);

    // When a byte of data is received, it will be appended to the rx_stream.  A worker thread
    // will dequeue the data from the rx_stream.
    helper->rx_stream = furi_stream_buffer_alloc(rx_buffer_size, 1);

    // The worker thread will remove the data from the rx_stream and copy it into the ring_buffer.
    // The worker thread will then process the data in the ring_buffer, invoking the process_line
    // callback whenever a delimiter is found in the data.
    helper->ring_buffer = ring_buffer_alloc();

    // worker_thread is the routine that will process data from the rx_stream.
    helper->worker_thread =
        furi_thread_alloc_ex("UartHelperWorker", worker_stack_size, uart_helper_worker, helper);
    furi_thread_start(helper->worker_thread);

    // Set the callback to invoke when data is received.
    furi_hal_serial_async_rx_start(
        helper->serial_handle, uart_helper_received_byte_callback, helper, false);

    return helper;
}

void uart_helper_set_delimiter(UartHelper* helper, char delimiter, bool include_delimiter) {
    // Update the delimiter character and flag to determine if delimiter should be part
    // of the response to the process_line callback.
    ring_buffer_set_delimiter(helper->ring_buffer, delimiter, include_delimiter);
}

void uart_helper_set_callback(UartHelper* helper, ProcessLine process_line, void* context) {
    // Set the process_line callback and context.
    helper->process_line = process_line;
    helper->context = context;
}

void uart_helper_set_baud_rate(UartHelper* helper, uint32_t baud_rate) {
    // Update the baud rate for the UART.
    furi_hal_serial_set_br(helper->serial_handle, baud_rate);
    ring_buffer_clear(helper->ring_buffer);
}

void uart_helper_send(UartHelper* helper, const char* data, size_t length) {
    // Transmit data via UART TX.
    furi_hal_serial_tx(helper->serial_handle, (uint8_t*)data, length);
}

void uart_helper_send_string(UartHelper* helper, FuriString* string) {
    const char* str = furi_string_get_cstr(string);

    // UTF-8 strings can have character counts different then lengths!
    // Count the bytes until a null is encountered.
    size_t length = 0;
    while(str[length] != 0) {
        length++;
    }

    // Transmit data
    uart_helper_send(helper, str, length);
}

void uart_helper_free(UartHelper* helper) {
    // Signal that we want the worker to exit.  It may be doing other work.
    furi_thread_flags_set(furi_thread_get_id(helper->worker_thread), WorkerEventExiting);

    // Wait for the worker_thread to complete it's work and release its resources.
    furi_thread_join(helper->worker_thread);

    // Free the worker_thread.
    furi_thread_free(helper->worker_thread);

    furi_hal_serial_control_release(helper->serial_handle);
    // If the UART was initialized by the application, deinitialize it, otherwise re-enable the console.
    if(helper->uart_init_by_app) {
        furi_hal_serial_deinit(helper->serial_handle);
    } else {
        // furi_hal_console_enable();
    }

    // Free the rx_stream and ring_buffer.
    furi_stream_buffer_free(helper->rx_stream);
    ring_buffer_free(helper->ring_buffer);

    free(helper);
}