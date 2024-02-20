#include <furi_hal.h>
#include <cfw/cfw.h>
#include <expansion/expansion.h>

#define UART_CH (cfw_settings.uart_nmea_channel)

#define RX_BUF_SIZE 1024

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

typedef struct {
    Expansion* expansion;
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
} SubGhzGPS;

/**
 * Initialize SubGhzGPS object
 * 
 * @return SubGhzGPS* SubGhzGPS object
*/
SubGhzGPS* subghz_gps_init();

/**
 * Deinitialize SubGhzGPS object
 * 
 * @param subghz_gps SubGhzGPS object
 * @return void
*/
void subghz_gps_deinit(SubGhzGPS* subghz_gps);

/**
 * Start GPS thread
 * 
 * @param subghz_gps SubGhzGPS object
 * @return void
*/
void subghz_gps_start(SubGhzGPS* subghz_gps);

/**
 * Stop GPS thread
 * 
 * @param subghz_gps SubGhzGPS object
 * @return void
*/
void subghz_gps_stop(SubGhzGPS* subghz_gps);

/**
 * Set baudrate for GPS
 * 
 * @param baudrate Baudrate
 * @return void
*/
void subghz_gps_set_baudrate(SubGhzGPS* subghz_gps, uint32_t baudrate);

/**
 * Convert degree to radian
 * 
 * @param deg Degree
 * @return double Radian
 */
double subghz_gps_deg2rad(double deg);

/**
 * Calculate distance between two coordinates
 * 
 * @param lat1d Latitude 1
 * @param lon1d Longitude 1
 * @param lat2d Latitude 2
 * @param lon2d Longitude 2
 * @return double Distance in km
*/
double subghz_gps_calc_distance(double lat1d, double lon1d, double lat2d, double lon2d);

/**
 * Calculate angle between two coordinates
 * 
 * @param lat1 Latitude 1
 * @param lon1 Longitude 1
 * @param lat2 Latitude 2
 * @param lon2 Longitude 2
 * @return double Angle in degree
*/
double subghz_gps_calc_angle(double lat1, double lon1, double lat2, double lon2);