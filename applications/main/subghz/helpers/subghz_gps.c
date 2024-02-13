#include "subghz_gps.h"

static void subghz_gps_uart_parse_nmea(SubGhzGPS* subghz_gps, char* line) {
    switch(minmea_sentence_id(line, false)) {
    case MINMEA_SENTENCE_RMC: {
        struct minmea_sentence_rmc frame;
        if(minmea_parse_rmc(&frame, line)) {
            subghz_gps->latitude = minmea_tocoord(&frame.latitude);
            subghz_gps->longitude = minmea_tocoord(&frame.longitude);
            subghz_gps->fix_second = frame.time.seconds;
            subghz_gps->fix_minute = frame.time.minutes;
            subghz_gps->fix_hour = frame.time.hours;
        }
    } break;

    case MINMEA_SENTENCE_GGA: {
        struct minmea_sentence_gga frame;
        if(minmea_parse_gga(&frame, line)) {
            subghz_gps->latitude = minmea_tocoord(&frame.latitude);
            subghz_gps->longitude = minmea_tocoord(&frame.longitude);
            subghz_gps->satellites = frame.satellites_tracked;
            subghz_gps->fix_second = frame.time.seconds;
            subghz_gps->fix_minute = frame.time.minutes;
            subghz_gps->fix_hour = frame.time.hours;
        }
    } break;

    case MINMEA_SENTENCE_GLL: {
        struct minmea_sentence_gll frame;
        if(minmea_parse_gll(&frame, line)) {
            subghz_gps->latitude = minmea_tocoord(&frame.latitude);
            subghz_gps->longitude = minmea_tocoord(&frame.longitude);
            subghz_gps->fix_second = frame.time.seconds;
            subghz_gps->fix_minute = frame.time.minutes;
            subghz_gps->fix_hour = frame.time.hours;
        }
    } break;

    default:
        break;
    }
}

static void subghz_gps_uart_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    SubGhzGPS* subghz_gps = (SubGhzGPS*)context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(subghz_gps->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(subghz_gps->thread), WorkerEvtRxDone);
    }
}

static int32_t subghz_gps_uart_worker(void* context) {
    SubGhzGPS* subghz_gps = (SubGhzGPS*)context;

    size_t rx_offset = 0;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEvtStop) {
            break;
        }

        if(events & WorkerEvtRxDone) {
            size_t len = 0;
            do {
                len = furi_stream_buffer_receive(
                    subghz_gps->rx_stream,
                    subghz_gps->rx_buf + rx_offset,
                    RX_BUF_SIZE - rx_offset,
                    0);

                if(len > 0) {
                    rx_offset += len;
                    subghz_gps->rx_buf[rx_offset] = '\0';

                    char* current_line = (char*)subghz_gps->rx_buf;
                    while(true) {
                        while(*current_line == '\0' &&
                              current_line < (char*)subghz_gps->rx_buf + rx_offset) {
                            current_line++;
                        }

                        char* next_line = strchr(current_line, '\n');
                        if(next_line) {
                            *next_line = '\0';
                            subghz_gps_uart_parse_nmea(subghz_gps, current_line);
                            current_line = next_line + 1;
                        } else {
                            if(current_line > (char*)subghz_gps->rx_buf) {
                                rx_offset = 0;
                                while(*current_line) {
                                    subghz_gps->rx_buf[rx_offset++] = *(current_line++);
                                }
                            }
                            break;
                        }
                    }
                }
            } while(len > 0);
        }
    }

    return 0;
}

SubGhzGPS* subghz_gps_init() {
    SubGhzGPS* subghz_gps = malloc(sizeof(SubGhzGPS));

    subghz_gps->latitude = NAN;
    subghz_gps->longitude = NAN;
    subghz_gps->satellites = 0;
    subghz_gps->fix_hour = 0;
    subghz_gps->fix_minute = 0;
    subghz_gps->fix_second = 0;

    subghz_gps->rx_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);

    subghz_gps->thread = furi_thread_alloc();
    furi_thread_set_name(subghz_gps->thread, "SubGhzGPSWorker");
    furi_thread_set_stack_size(subghz_gps->thread, 1024);
    furi_thread_set_context(subghz_gps->thread, subghz_gps);
    furi_thread_set_callback(subghz_gps->thread, subghz_gps_uart_worker);

    if(UART_CH == FuriHalUartIdUSART1) {
        furi_hal_console_disable();
    } else if(UART_CH == FuriHalUartIdLPUART1) {
        furi_hal_uart_init(UART_CH, 9600);
    }

    furi_hal_uart_set_irq_cb(UART_CH, subghz_gps_uart_on_irq_cb, subghz_gps);
    furi_hal_uart_set_br(UART_CH, 9600);

    return subghz_gps;
}

void subghz_gps_deinit(SubGhzGPS* subghz_gps) {
    furi_assert(subghz_gps);
    furi_thread_free(subghz_gps->thread);

    free(subghz_gps);

    furi_hal_uart_set_irq_cb(UART_CH, NULL, NULL);
    if(UART_CH == FuriHalUartIdLPUART1) {
        furi_hal_uart_deinit(UART_CH);
    } else {
        furi_hal_console_enable();
    }
}

void subghz_gps_start(SubGhzGPS* subghz_gps) {
    furi_thread_start(subghz_gps->thread);
}

void subghz_gps_stop(SubGhzGPS* subghz_gps) {
    furi_thread_flags_set(furi_thread_get_id(subghz_gps->thread), WorkerEvtStop);
    furi_thread_join(subghz_gps->thread);
}

double subghz_gps_deg2rad(double deg) {
    return (deg * (double)M_PI / 180);
}

double subghz_gps_calc_distance(double lat1d, double lon1d, double lat2d, double lon2d) {
    double lat1r, lon1r, lat2r, lon2r, u, v;
    lat1r = subghz_gps_deg2rad(lat1d);
    lon1r = subghz_gps_deg2rad(lon1d);
    lat2r = subghz_gps_deg2rad(lat2d);
    lon2r = subghz_gps_deg2rad(lon2d);
    u = sin((lat2r - lat1r) / 2);
    v = sin((lon2r - lon1r) / 2);
    return 2 * 6371 * asin(sqrt(u * u + cos((double)lat1r) * cos((double)lat2r) * v * v));
}

double subghz_gps_calc_angle(double lat1, double lon1, double lat2, double lon2) {
    return atan2(lat1 - lat2, lon1 - lon2) * 180 / (double)M_PI;
}