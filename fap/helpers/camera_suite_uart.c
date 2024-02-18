#include "camera_suite_uart.h"
#include "../camera_suite.h"

// void camera_suite_uart_alloc(CameraSuite* instance, FuriThreadCallback* callback) {
//     // Allocate a stream buffer
//     instance->rx_stream = furi_stream_buffer_alloc(2048, 1);

//     // Allocate a thread for this camera to run on.
//     FuriThread* thread = furi_thread_alloc_ex("UsbUartWorker", 2048, callback, instance);
//     instance->worker_thread = thread;
//     furi_thread_start(instance->worker_thread);

//     // Set up UART thread.
//     instance->serial_handle = furi_hal_serial_control_acquire(UART_CH);
//     furi_check(instance->serial_handle);
//     furi_hal_serial_init(instance->serial_handle, 230400);

//     // Enable UART1 and set the IRQ callback.
//     furi_hal_serial_async_rx_start(instance->serial_handle, callback, instance, false);
// }

// void camera_suite_uart_free(CameraSuite* app_instance) {
//     furi_assert(app_instance);

//     // Free the worker thread.
//     furi_thread_flags_set(furi_thread_get_id(app_instance->worker_thread), WorkerEventStop);
//     furi_thread_join(app_instance->worker_thread);
//     furi_thread_free(app_instance->worker_thread);

//     // Free the stream buffer.
//     furi_stream_buffer_free(app_instance->rx_stream);

//     // Free the serial handle.
//     furi_hal_serial_deinit(app_instance->serial_handle);
//     furi_hal_serial_control_release(app_instance->serial_handle);
// }
