#include "wardriver.h"

#define UART_CH_ESP (xtreme_settings.uart_esp_channel)

#define UART_CH_GPS (xtreme_settings.uart_nmea_channel)

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

void wardriver_uart_init(Context* ctx);
void wardriver_uart_deinit(Context* ctx);
