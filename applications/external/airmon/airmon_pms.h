#pragma once

#include <furi_hal.h>
#include <notification/notification_messages.h>

#define RX_BUF_SIZE 64

typedef struct {
    uint16_t pm1_0cf; // PM1.0 ug/m3 (ultrafine particles)
    uint16_t pm2_5cf; // PM2.5 ug/m3 (combustion particles, organic compounds, metals)
    uint16_t pm10cf; // PM10 ug/m3  (dust, pollen, mould spores)
    uint16_t pm1_0at; // PM1.0 ug/m3 (atmos env)
    uint16_t pm2_5at; // PM2.5 ug/m3 (atmos env)
    uint16_t pm10at; // PM10 ug/m3 (atmos env)
    uint16_t ct0_3; // count of >0.3um in 0.1L air
    uint16_t ct0_5; // count of >0.5um in 0.1L air
    uint16_t ct1_0; // count of >1.0um in 0.1L air
    uint16_t ct2_5; // count of >2.5um in 0.1L air
    uint16_t ct5_0; // count of >5.0um in 0.1L air
    uint16_t ct10; // count of >10um in 0.1L air
} AirmonPmsData;

typedef struct {
    FuriMutex* mutex;
    FuriThread* thread;
    FuriStreamBuffer* rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE];

    NotificationApp* notifications;

    AirmonPmsData pms_data;
} AirmonPmsContext;

AirmonPmsContext* airmon_pms_context_alloc();
void airmon_pms_context_free(AirmonPmsContext* pms_context);

void airmon_pms_init(AirmonPmsContext* pms_context);
void airmon_pms_deinit(AirmonPmsContext* pms_context);
