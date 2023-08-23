#pragma once

#include <furi.h>

typedef struct CarBusAnalyzer CarBusAnalyzer;

typedef enum {
    CarBusAnalyzerDisconnected,
    CarBusAnalyzerConnected,
} CarBusAnalyzerStatus;

typedef void (*CarBusAnalyzerCallback)(void* context);
typedef void (*CarBusAnalyzerConnectCallback)(void* context, CarBusAnalyzerStatus status);

CarBusAnalyzer* carbus_analyzer_alloc(void);
void carbus_analyzer_free(CarBusAnalyzer* instance);
bool carbus_analyzer_is_device_connected(CarBusAnalyzer* instance);
void carbus_analyzer_set_callback(
    CarBusAnalyzer* instance,
    CarBusAnalyzerCallback callback,
    CarBusAnalyzerConnectCallback connect_callback,
    void* context);
size_t carbus_analyzer_spaces_rx(CarBusAnalyzer* instance);
bool carbus_analyzer_rx(CarBusAnalyzer* instance, uint8_t* data, size_t len);
size_t carbus_analyzer_tx(CarBusAnalyzer* instance, uint8_t* data, size_t max_len);
void carbus_analyzer_exit(CarBusAnalyzer* instance);
void carbus_analyzer_process_cmd(CarBusAnalyzer* instance);
void carbus_analyzer_get_cmd(CarBusAnalyzer* instance);
void carbus_analyzer_process(CarBusAnalyzer* instance);
