#include "wardriver_uart.h"

static void removeSpaces(char* str) {
    int i = 0;
    while(isspace((unsigned char)str[i])) {
        i++;
    }

    int j = 0;
    while(str[i] != '\0') {
        str[j++] = str[i++];
    }
    str[j] = '\0';

    int len = strlen(str);
    while(len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
}

static void sort_access_points(Context* ctx) {
    for(int i = 0; i < ctx->access_points_count; i++) {
        for(int j = i + 1; j < ctx->access_points_count; j++) {
            if(strcmp(ctx->access_points[i].ssid, ctx->access_points[j].ssid) > 0) {
                AccessPoint temp = ctx->access_points[i];
                ctx->access_points[i] = ctx->access_points[j];
                ctx->access_points[j] = temp;
            }
        }
    }
}

static void set_index_from_access_points(Context* ctx) {
    for(int i = 0; i < ctx->access_points_count; i++) {
        if(ctx->access_points[i].bssid == ctx->active_access_point.bssid) {
            ctx->access_points_index = i;
            break;
        }
    }
}

static void uart_parse_esp(void* context, char* line) {
    Context* ctx = context;

    AccessPoint ap = {.ssid = malloc(MAX_SSID_LENGTH + 1), .bssid = malloc(MAX_BSSID_LENGTH + 1)};

    Packet pkt = {.recievedMac = malloc(18 + 1), .sentMac = malloc(18 + 1)};

    char* token = strtok(line, ",");
    int i = 0;
    bool isAp = false;
    bool isValid = false;
    while(token != NULL) {
        switch(i) {
        case 0:
            if(strcmp(token, "AR") == 0) {
                isAp = true;
                isValid = true;
            } else if(strcmp(token, "PK") == 0) {
                isValid = true;
            }
            break;
        case 1:
            if(isAp && isValid) {
                removeSpaces(token);
                strcpy(ap.ssid, token);
            } else if(!isAp && isValid) {
                strncpy(pkt.recievedMac, token, 18);
                pkt.recievedMac[18] = '\0';
            }
            break;
        case 2:
            if(isAp && isValid) {
                strcpy(ap.bssid, token);
            } else if(!isAp && isValid) {
                strncpy(pkt.sentMac, token, 18);
                pkt.sentMac[18] = '\0';
            }
            break;
        case 3:
            if(isAp && isValid) {
                ap.rssi = atoi(token);
            }
            break;
        case 4:
            if(isAp && isValid) {
                ap.channel = atoi(token);
            }
            break;
        }

        token = strtok(NULL, ",");
        i++;
    }

    if(isAp && isValid) {
        // free the packet
        free(pkt.recievedMac);
        free(pkt.sentMac);

        if(ctx->view_state == NO_APS) {
            ctx->view_state = NORMAL;
        }

        // check if values are valid
        // bssid needs an ":"
        // rssi needs to be negative
        // channel needs to be between 1 and 14
        // ssid needs to be at least 1 character long
        if(ap.bssid[2] != ':' || ap.bssid[5] != ':' || ap.bssid[8] != ':' || ap.bssid[11] != ':' ||
           ap.bssid[14] != ':' || ap.rssi > 0 || ap.channel < 1 || ap.channel > 14 ||
           strlen(ap.ssid) < 1) {
            free(ap.ssid);
            free(ap.bssid);
            return;
        }

        furi_hal_light_set(LightBlue, 0);
        furi_hal_light_set(LightGreen, 255);

        furi_hal_rtc_get_datetime(&ap.datetime);

        if(isnan(ctx->gps_data.latitude) || isnan(ctx->gps_data.longitude)) {
            ap.latitude = 0;
            ap.longitude = 0;
        } else {
            ap.latitude = ctx->gps_data.latitude;
            ap.longitude = ctx->gps_data.longitude;
        }

        // check if ap is already in the list otherwise add it but update the rssi
        bool found = false;
        for(size_t i = 0; i < ctx->access_points_count; i++) {
            if(strcmp(ctx->access_points[i].bssid, ap.bssid) == 0) {
                found = true;
                //update rssi channel datetime
                ctx->access_points[i].rssi = ap.rssi;
                ctx->access_points[i].channel = ap.channel;
                ctx->access_points[i].datetime = ap.datetime;
                ctx->access_points[i].latitude = ap.latitude;
                ctx->access_points[i].longitude = ap.longitude;

                if(strcmp(ctx->active_access_point.bssid, ap.bssid) == 0) {
                    ctx->active_access_point.rssi = ap.rssi;
                    ctx->active_access_point.channel = ap.channel;
                    ctx->active_access_point.datetime = ap.datetime;
                    ctx->active_access_point.latitude = ap.latitude;
                    ctx->active_access_point.longitude = ap.longitude;
                }

                free(ap.ssid);
                free(ap.bssid);

                break;
            }
        }

        if(!found) {
            memcpy(&ctx->access_points[ctx->access_points_count], &ap, sizeof(AccessPoint));
            ctx->access_points_count++;
        }

        sort_access_points(ctx);
        set_index_from_access_points(ctx);
    } else {
        // it is a packet so screw the ap
        free(ap.ssid);
        free(ap.bssid);

        // check if values are valid
        // mac needs to be 6 characters long
        if(strlen(pkt.recievedMac) != 17 || strlen(pkt.sentMac) != 17 ||
           ctx->access_points_count == 0) {
            free(pkt.recievedMac);
            free(pkt.sentMac);
            return;
        }

        furi_hal_light_set(LightGreen, 0);
        furi_hal_light_set(LightBlue, 255);

        for(size_t i = 0; i < ctx->access_points_count; i++) {
            if(strcmp(ctx->access_points[i].bssid, pkt.recievedMac) == 0) {
                ctx->access_points[i].packetRxCount++;
                break;
            }
        }

        for(size_t i = 0; i < ctx->access_points_count; i++) {
            if(strcmp(ctx->access_points[i].bssid, pkt.sentMac) == 0) {
                ctx->access_points[i].packetTxCount++;
                break;
            }
        }

        free(pkt.recievedMac);
        free(pkt.sentMac);
    }
}

static void uart_cb_esp(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    Context* ctx = (Context*)context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(ctx->rx_stream_esp, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(ctx->thread_esp), WorkerEvtRxDone);
    }
}

static int32_t uart_worker_esp(void* context) {
    Context* ctx = (Context*)context;

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
                // receive serial bytes into rx_buf, starting at rx_offset from the start of the buffer
                // the maximum we can receive is RX_BUF_SIZE - 1 - rx_offset
                len = furi_stream_buffer_receive(
                    ctx->rx_stream_esp,
                    ctx->rx_buf_esp + rx_offset,
                    RX_BUF_SIZE - 1 - rx_offset,
                    0);

                if(len > 0) {
                    // increase rx_offset by the number of bytes received, and null-terminate rx_buf
                    rx_offset += len;
                    ctx->rx_buf_esp[rx_offset] = '\0';

                    // look for strings ending in newlines, starting at the start of rx_buf
                    char* line_current = (char*)ctx->rx_buf_esp;
                    while(1) {
                        // skip null characters
                        while(*line_current == '\0' &&
                              line_current < (char*)ctx->rx_buf_esp + rx_offset - 1) {
                            line_current++;
                        }

                        // find the next newline
                        char* newline = strchr(line_current, '\n');
                        if(newline) // newline found
                        {
                            // put a null terminator in place of the newline, to delimit the line string
                            *newline = '\0';

                            uart_parse_esp(ctx, line_current);

                            // move the cursor to the character after the newline
                            line_current = newline + 1;
                        } else // no more newlines found
                        {
                            if(line_current >
                               (char*)ctx->rx_buf_esp) // at least one line was found
                            {
                                // clear parsed lines, and move any leftover bytes to the start of rx_buf
                                rx_offset = 0;
                                while(
                                    *line_current) // stop when the original rx_offset terminator is reached
                                {
                                    ctx->rx_buf_esp[rx_offset++] = *(line_current++);
                                }
                            }
                            break; // go back to receiving bytes from the serial stream
                        }
                    }
                }
            } while(len > 0);
        }
    }

    return 0;
}

static void uart_parse_gps(Context* ctx, char* line) {
    switch(minmea_sentence_id(line, false)) {
    case MINMEA_SENTENCE_RMC: {
        struct minmea_sentence_rmc frame;
        if(minmea_parse_rmc(&frame, line)) {
            ctx->gps_data.latitude = minmea_tocoord(&frame.latitude);
            ctx->gps_data.longitude = minmea_tocoord(&frame.longitude);
            ctx->gps_data.hour = frame.time.hours;
            ctx->gps_data.minute = frame.time.minutes;
            ctx->gps_data.second = frame.time.seconds;
        }
    } break;
    case MINMEA_SENTENCE_GGA: {
        struct minmea_sentence_gga frame;
        if(minmea_parse_gga(&frame, line)) {
            ctx->gps_data.latitude = minmea_tocoord(&frame.latitude);
            ctx->gps_data.longitude = minmea_tocoord(&frame.longitude);
            ctx->gps_data.hour = frame.time.hours;
            ctx->gps_data.minute = frame.time.minutes;
            ctx->gps_data.second = frame.time.seconds;
            ctx->gps_data.satelites = frame.satellites_tracked;
        }
    } break;

    case MINMEA_SENTENCE_GLL: {
        struct minmea_sentence_gll frame;
        if(minmea_parse_gll(&frame, line)) {
            ctx->gps_data.latitude = minmea_tocoord(&frame.latitude);
            ctx->gps_data.longitude = minmea_tocoord(&frame.longitude);
            ctx->gps_data.hour = frame.time.hours;
            ctx->gps_data.minute = frame.time.minutes;
            ctx->gps_data.second = frame.time.seconds;
        }
    } break;

    default:
        break;
    }
}

static void uart_cb_gps(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    Context* ctx = (Context*)context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(ctx->rx_stream_gps, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(ctx->thread_gps), WorkerEvtRxDone);
    }
}

static int32_t uart_worker_gps(void* context) {
    Context* ctx = (Context*)context;

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
                // receive serial bytes into rx_buf, starting at rx_offset from the start of the buffer
                // the maximum we can receive is RX_BUF_SIZE - 1 - rx_offset
                len = furi_stream_buffer_receive(
                    ctx->rx_stream_gps,
                    ctx->rx_buf_gps + rx_offset,
                    RX_BUF_SIZE - 1 - rx_offset,
                    0);

                if(len > 0) {
                    // increase rx_offset by the number of bytes received, and null-terminate rx_buf
                    rx_offset += len;
                    ctx->rx_buf_gps[rx_offset] = '\0';

                    // look for strings ending in newlines, starting at the start of rx_buf
                    char* line_current = (char*)ctx->rx_buf_gps;
                    while(1) {
                        // skip null characters
                        while(*line_current == '\0' &&
                              line_current < (char*)ctx->rx_buf_gps + rx_offset - 1) {
                            line_current++;
                        }

                        // find the next newline
                        char* newline = strchr(line_current, '\n');
                        if(newline) // newline found
                        {
                            // put a null terminator in place of the newline, to delimit the line string
                            *newline = '\0';

                            uart_parse_gps(ctx, line_current);

                            // move the cursor to the character after the newline
                            line_current = newline + 1;
                        } else // no more newlines found
                        {
                            if(line_current >
                               (char*)ctx->rx_buf_gps) // at least one line was found
                            {
                                // clear parsed lines, and move any leftover bytes to the start of rx_buf
                                rx_offset = 0;
                                while(
                                    *line_current) // stop when the original rx_offset terminator is reached
                                {
                                    ctx->rx_buf_gps[rx_offset++] = *(line_current++);
                                }
                            }
                            break; // go back to receiving bytes from the serial stream
                        }
                    }
                }
            } while(len > 0);
        }
    }

    return 0;
}

void wardriver_uart_init(Context* ctx) {
    ctx->rx_stream_esp = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);

    ctx->thread_esp = furi_thread_alloc();
    furi_thread_set_name(ctx->thread_esp, "LLwardriverUartWorkerESP");
    furi_thread_set_stack_size(ctx->thread_esp, 2048);
    furi_thread_set_context(ctx->thread_esp, ctx);
    furi_thread_set_callback(ctx->thread_esp, uart_worker_esp);

    furi_thread_start(ctx->thread_esp);

    ctx->serial_handle_esp = furi_hal_serial_control_acquire(UART_CH_ESP);
    furi_check(ctx->serial_handle_esp);
    furi_hal_serial_init(ctx->serial_handle_esp, 115200);
    furi_hal_serial_async_rx_start(ctx->serial_handle_esp, uart_cb_esp, ctx, false);

    if(UART_CH_ESP != UART_CH_GPS) {
        ctx->rx_stream_gps = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);

        ctx->thread_gps = furi_thread_alloc();
        furi_thread_set_name(ctx->thread_gps, "LLwardriverUartWorkerGPS");
        furi_thread_set_stack_size(ctx->thread_gps, 2048);
        furi_thread_set_context(ctx->thread_gps, ctx);
        furi_thread_set_callback(ctx->thread_gps, uart_worker_gps);

        furi_thread_start(ctx->thread_gps);

        ctx->serial_handle_gps = furi_hal_serial_control_acquire(UART_CH_GPS);
        furi_check(ctx->serial_handle_gps);
        furi_hal_serial_init(ctx->serial_handle_gps, 9600);
        furi_hal_serial_async_rx_start(ctx->serial_handle_gps, uart_cb_gps, ctx, false);
    }

    return;
}

void wardriver_uart_deinit(Context* ctx) {
    furi_thread_flags_set(furi_thread_get_id(ctx->thread_esp), WorkerEvtStop);
    furi_thread_join(ctx->thread_esp);
    furi_thread_free(ctx->thread_esp);
    furi_stream_buffer_free(ctx->rx_stream_esp);

    if(UART_CH_ESP != UART_CH_GPS) {
        furi_thread_flags_set(furi_thread_get_id(ctx->thread_gps), WorkerEvtStop);
        furi_thread_join(ctx->thread_gps);
        furi_thread_free(ctx->thread_gps);
        furi_stream_buffer_free(ctx->rx_stream_gps);
    }

    furi_hal_serial_deinit(ctx->serial_handle_esp);
    furi_hal_serial_control_release(ctx->serial_handle_esp);

    if(UART_CH_ESP != UART_CH_GPS) {
        furi_hal_serial_deinit(ctx->serial_handle_gps);
        furi_hal_serial_control_release(ctx->serial_handle_gps);
    }

    return;
}