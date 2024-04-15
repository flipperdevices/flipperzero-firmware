#pragma once

#include <furi_hal.h>
#include <flipper_application/flipper_application.h>

#define RX_BUF_SIZE 1024

typedef struct SubGhzGPS SubGhzGPS;

struct SubGhzGPS {
    FlipperApplication* plugin_app;
    FuriThread* thread;
    FuriStreamBuffer* rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE];
    FuriHalSerialHandle* serial_handle;
    FuriTimer* timer;

    float latitude;
    float longitude;
    int satellites;
    uint8_t fix_second;
    uint8_t fix_minute;
    uint8_t fix_hour;

    /**
     * Deinitialize SubGhzGPS object
     * To be used by plugin handler
     * 
     * @param subghz_gps SubGhzGPS object
     * @return void
    */
    void (*deinit)(SubGhzGPS* subghz_gps);

    /**
     * Concatenate realtime GPS info to string
     * 
     * @param subghz_gps SubGhzGPS object
     * @param descr Output string
     * @param latitude Latitude
     * @param longitude Longitude
     * @return void
    */
    void (*cat_realtime)(SubGhzGPS* subghz_gps, FuriString* descr, float latitude, float longitude);
};

/**
 * Initialize SubGhzGPS plugin
 * Fails and returns NULL if Expansion is connected
 *
 * @return SubGhzGPS* SubGhzGPS object
*/
SubGhzGPS* subghz_gps_plugin_init(uint32_t baudrate);

/**
 * Deinitialize SubGhzGPS plugin
 * 
 * @param subghz_gps SubGhzGPS object
 * @return void
*/
void subghz_gps_plugin_deinit(SubGhzGPS* subghz_gps);
