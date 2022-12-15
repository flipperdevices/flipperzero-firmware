//
// Written by vad7, 20.11.2022.
//
#include "nrf24scan.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <dialogs/dialogs.h>
#include <input/input.h>
#include <stdlib.h>
#include <dolphin/dolphin.h>
#include <nrf24.h>
#include <u8g2.h>

#define TAG "nrf24scan"
#define VERSION "1.7"
#define MAX_CHANNEL 125
#define MAX_ADDR 6

#define SCAN_APP_PATH_FOLDER "/ext/nrf24scan"
#define SETTINGS_FILENAME "addresses.txt" // Settings file format (1 parameter per line):
// SNIFF - if present then sniff mode
// Rate: 0/1/2 - rate in Mbps (=0.25/1/2)
// Ch: 0..125 - default channel
// ESB: 0/1 (1 - Enhanced ShockBurst)
// DPL: 0/1 (1 - Dynamic Payload Length)
// CRC: 0/1/2 (CRC length)
// Payload: 1..32 (bytes)
// P0: address P0 in hex (5 byte, LSB last)
// P1: address P1 in hex (5 byte, LSB last)
// P2: address P2, LSB in hex (1 byte)
// P3: address P3, LSB in hex (1 byte)
// P4: address P4, LSB in hex (1 byte)
// P5: address P5, LSB in hex (1 byte)
// captured data:
//		first byte = { RAW packet flag (0x80/0x00) } + { channel number }
//		second byte = { Payload len 5 bits, 0 = 32 } + {{ RAW packet: ESB flag 0x04/0x00 + address size-2 if RAW packet } or { pipe #(0..5) }},
// ... up to MAX_LOG_RECORDS-1
#define SNIFF_FILENAME "sniff.txt" // settings for sniff mode
#define LOG_FILENAME "log"
#define LOG_FILEEXT ".txt"
#define MAX_LOG_RECORDS 100
#define LOG_REC_SIZE 34 // max packet size
#define VIEW_LOG_MAX_X 22
#define VIEW_LOG_WIDTH_B 10 // bytes

const char SettingsFld_Rate[] = "Rate:";
const char SettingsFld_Ch[] = "Ch:";
const char SettingsFld_ESB[] = "ESB:";
const char SettingsFld_DPL[] = "DPL:";
const char SettingsFld_CRC[] = "CRC:";
const char SettingsFld_Payload[] = "Payload:";
const char SettingsFld_Sniff[] = "SNIFF";
char SettingsFld_Addr = 'P';

Nrf24Scan* APP;
uint8_t what_doing = 0; // 0 - setup, 1 - view log, 2 - view addresses
uint8_t what_to_do = 1; // 0 - view, 1 - view & sniff, 2 - view & read
uint32_t key_press_seq_ok = 0;
uint8_t save_settings = 0;
char screen_buf[64];
char addr_file_name[32];
uint8_t NRF_rate = 1; // 0 - 250Kbps, 1 - 1Mbps, 2 - 2Mbps
uint8_t NRF_channel = 0; // 0..125
uint8_t NRF_ESB = 1; // 0 - ShockBurst, 1 - Enhanced ShockBurst
uint8_t NRF_DPL = 0; // 1 - Dynamic Payload Length
uint8_t NRF_CRC = 2; // 1 - No, 1 - CRC 1byte, 2 - CRC 2byte
uint8_t NRF_Payload = 32; // len in bytes, max 32
uint8_t NRF_Sniff_payload_max = 28;
uint8_t NRF_AA_OFF = 0; // Disable Auto Acknowledgement
bool NRF_ERROR = 0;

struct {
    uint8_t addr_P0[5]; // MSB first
    uint8_t addr_P1[5]; // MSB first
    uint8_t addr_P2; // LSB only, MSB bytes equal addr_P1
    uint8_t addr_P3; // LSB only, MSB bytes equal addr_P1
    uint8_t addr_P4; // LSB only, MSB bytes equal addr_P1
    uint8_t addr_P5; // LSB only, MSB bytes equal addr_P1
    uint8_t addr_len; // 2..5
    uint8_t addr_count;
} addrs;

int8_t log_to_file = 0; // 0 - no, 1 - yes(new), 2 - append, -1 - only clear
uint16_t log_arr_idx;
uint16_t view_log_arr_idx = 0;
uint16_t view_log_arr_x = 0;
bool save_to_new_log = true;
uint16_t last_packet_send = -1;
uint8_t last_packet_send_st = 0;
int16_t find_channel_period = 0; // sec
uint8_t menu_selected = 0;
uint32_t start_time;
uint8_t view_log_decode_PCF =
    0; // view log: 1 - decode packet control field (9b) when ESB off. After pipe # (hex): <Payload len 6b><PID_2b+NO_ACK_1b>
uint8_t view_log_decode_CRC = 0; // CRC bytes - 1/2, 0 - none

#define menu_selected_max 5
enum {
    Menu_open_file = 0,
    Menu_enter_channel,
    Menu_enter_rate,
    Menu_enter_scan_period,
    Menu_log,
    Menu_ok
};

//#define MIN(a, b)  ((a<b)?a:b)

static uint8_t GetHexVal(char hex) {
    return (uint8_t)hex - ((uint8_t)hex < 58 ? 48 : ((uint8_t)hex < 97 ? 55 : 87));
}

// Return num bytes in array
static uint8_t ConvertHexToArray(char* hex, uint8_t* array, uint8_t maxlen) {
    uint8_t len = 0;
    while(maxlen) {
        uint8_t ch = *hex++;
        if(ch == 0) break;
        if(ch < '0') continue;
        *array++ = (GetHexVal(ch) << 4) + GetHexVal(*hex++);
        len++;
        maxlen--;
    }
    return len;
}

static void add_to_str_hex_bytes(char* out, char* arr, int bytes) {
    if(bytes <= 0) return;
    out += strlen(out);
    do {
        snprintf(out, 3, "%02X", *arr++);
        out += 2;
    } while(--bytes);
}

static void add_to_furi_str_hex_bytes(FuriString* str, char* arr, int bytes) {
    if(!bytes) return;
    do {
        furi_string_cat_printf(str, "%02X", *arr++);
    } while(--bytes);
}

static void add_to_str_hex_bytes_shift_9b(char* out, char* arr, int bytes) {
    if(bytes <= 0) return;
    out += strlen(out);
    arr++; // +8b
    do {
        snprintf(out, 4, "%02X", ((uint8_t)(*arr << 1)) | (*(arr + 1) >> 7));
        arr++;
        out += 2;
    } while(--bytes);
}

void clear_log() {
    log_arr_idx = 0;
    view_log_arr_idx = 0;
    last_packet_send = -1;
}

void allocate_log_array() {
    APP->log_arr = malloc(LOG_REC_SIZE * MAX_LOG_RECORDS);
    if(APP->log_arr == NULL) {
        FURI_LOG_E(TAG, "Not enouch memory: %d", LOG_REC_SIZE * MAX_LOG_RECORDS);
        strcpy(addr_file_name, "MEMORY LOW!");
    }
    clear_log();
}

void write_to_log_file(Storage* storage, bool f_settings) {
    if(log_arr_idx == 0 && !f_settings) return;
    Stream* file_stream = file_stream_alloc(storage);
    FuriString* str = furi_string_alloc();
    furi_string_set(str, SCAN_APP_PATH_FOLDER);
    furi_string_cat(str, "/");
    bool fl;
    if(f_settings) {
        furi_string_cat(str, SETTINGS_FILENAME);
        fl = file_stream_open(
            file_stream, furi_string_get_cstr(str), FSAM_READ_WRITE, FSOM_CREATE_ALWAYS);
        if(!fl) file_stream_close(file_stream);
    } else {
        furi_string_cat(str, LOG_FILENAME);
        furi_string_cat(str, LOG_FILEEXT);
        if(save_to_new_log) {
            int cnt = 1;
            do {
                fl = file_stream_open(
                    file_stream, furi_string_get_cstr(str), FSAM_READ_WRITE, FSOM_CREATE_NEW);
                if(fl) break;
                file_stream_close(file_stream);
                furi_string_set(str, SCAN_APP_PATH_FOLDER);
                furi_string_cat(str, "/");
                furi_string_cat(str, LOG_FILENAME);
                furi_string_cat_printf(str, "-%02d", cnt);
                furi_string_cat(str, LOG_FILEEXT);
            } while(++cnt < 100);
            if(!fl) {
                FURI_LOG_E(TAG, "Failed to create new log file");
                notification_message(APP->notification, &sequence_blink_red_100);
            }
        } else {
            fl = file_stream_open(
                file_stream, furi_string_get_cstr(str), FSAM_READ_WRITE, FSOM_OPEN_APPEND);
            if(fl) {
                if(stream_size(file_stream) == 0) save_to_new_log = true;
            } else
                file_stream_close(file_stream);
        }
    }
    if(fl) {
        FURI_LOG_D(TAG, "Save to %s", furi_string_get_cstr(str));
        if(save_to_new_log || f_settings) {
            furi_string_set(str, what_to_do == 1 ? SettingsFld_Sniff : "");
            furi_string_cat_printf(
                str,
                "%s %d\n%s %d\n%s %d\n",
                SettingsFld_Rate,
                NRF_rate,
                SettingsFld_Ch,
                NRF_channel,
                SettingsFld_ESB,
                NRF_ESB);
            furi_string_cat_printf(
                str,
                "%s %d\n%s %d\n%s %d\n",
                SettingsFld_DPL,
                NRF_DPL,
                SettingsFld_CRC,
                NRF_CRC,
                SettingsFld_Payload,
                NRF_Payload);
            furi_string_cat_printf(str, "P0: ");
            add_to_furi_str_hex_bytes(str, (char*)addrs.addr_P0, addrs.addr_len);
            furi_string_cat(str, "\n");
            if(addrs.addr_count > 1) {
                furi_string_cat_printf(str, "P1: ");
                add_to_furi_str_hex_bytes(str, (char*)addrs.addr_P1, addrs.addr_len);
                furi_string_cat(str, "\n");
            }
            if(addrs.addr_count > 2) {
                furi_string_cat_printf(str, "P2: ");
                furi_string_cat_printf(str, "%02X\n", addrs.addr_P2);
            }
            if(addrs.addr_count > 3) {
                furi_string_cat_printf(str, "P3: ");
                furi_string_cat_printf(str, "%02X\n", addrs.addr_P3);
            }
            if(addrs.addr_count > 4) {
                furi_string_cat_printf(str, "P4: ");
                furi_string_cat_printf(str, "%02X\n", addrs.addr_P4);
            }
            if(addrs.addr_count > 5) {
                furi_string_cat_printf(str, "P5: ");
                furi_string_cat_printf(str, "%02X\n", addrs.addr_P5);
            }
            if(!(fl = stream_write_string(file_stream, str) == furi_string_size(str))) {
                FURI_LOG_E(TAG, "Failed to write header to file!");
                notification_message(APP->notification, &sequence_blink_red_100);
            }
        }
        if(fl) {
            if(f_settings) {
                save_settings = 0;
                if(strcmp(addr_file_name, "NONE") == 0) strcpy(addr_file_name, SETTINGS_FILENAME);
            } else {
                int i = 0;
                for(; i < log_arr_idx; i++) {
                    furi_string_reset(str);
                    add_to_furi_str_hex_bytes(
                        str, (char*)APP->log_arr + i * LOG_REC_SIZE, LOG_REC_SIZE);
                    furi_string_cat(str, "\n");
                    if(stream_write_string(file_stream, str) != furi_string_size(str)) {
                        FURI_LOG_E(TAG, "Failed to write to file!");
                        break;
                    }
                }
                if(i == log_arr_idx) {
                    notification_message(APP->notification, &sequence_blink_yellow_100);
                    FURI_LOG_D(TAG, "File saved");
                }
                save_to_new_log = false;
            }
        }
        file_stream_close(file_stream);
    } else {
        FURI_LOG_E(TAG, "Failed to open file %s", furi_string_get_cstr(str));
        notification_message(APP->notification, &sequence_blink_red_100);
    }
    stream_free(file_stream);
    furi_string_free(str);
}

static bool select_settings_file(Stream* stream) {
    DialogsApp* dialogs = furi_record_open("dialogs");
    bool result = false;
    FuriString* path;
    path = furi_string_alloc();
    furi_string_set(path, SCAN_APP_PATH_FOLDER);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, ".txt", NULL);
    browser_options.hide_ext = false;

    bool ret = dialog_file_browser_show(dialogs, path, path, &browser_options);

    furi_record_close("dialogs");
    if(ret) {
        if(!file_stream_open(stream, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_D(TAG, "Cannot open file \"%s\"", furi_string_get_cstr(path));
            file_stream_close(stream);
        } else {
            FURI_LOG_D(TAG, "Open file \"%s\"", furi_string_get_cstr(path));
            strncpy(
                addr_file_name,
                furi_string_get_cstr(path) + sizeof(SCAN_APP_PATH_FOLDER),
                sizeof(addr_file_name));
            result = true;
        }
    }
    furi_string_free(path);
    return result;
}

// 0 - success, otherwise an error
static uint8_t load_settings_file(Stream* file_stream) {
    size_t file_size = 0;
    char* file_buf;
    uint8_t err = 5;
    file_size = stream_size(file_stream);
    if(file_size == (size_t)0) {
        FURI_LOG_D(TAG, "load failed. file_size: %d", file_size);
        return 1;
    }
    file_size = MIN(file_size, (size_t)LOG_REC_SIZE * MAX_LOG_RECORDS * 2 + 100);
    file_buf = malloc(file_size + 1);
    if(file_buf == NULL) {
        FURI_LOG_D(TAG, "Memory low, need: %d", file_size);
        return 2;
    }
    memset(file_buf, 0, file_size + 1);
    if(stream_read(file_stream, (uint8_t*)file_buf, file_size) == file_size) {
        FURI_LOG_D(TAG, "Loading settings file");
        char* line_ptr = file_buf;
        int16_t line_num = 0;
        memset((uint8_t*)&addrs, 0, sizeof(addrs));
        what_to_do = 2;
        bool log_loaded = false;
        while(line_ptr && (size_t)(line_ptr - file_buf) < file_size) {
            char* end_ptr = strstr((char*)line_ptr, "\n");
            if(end_ptr == NULL)
                end_ptr = file_buf + file_size;
            else
                *end_ptr = '\0';
            int line_len = end_ptr - line_ptr;
            if(*line_ptr == '\r' || line_len == 0) {
                line_ptr = end_ptr + 1;
                continue;
            }
            if(*(end_ptr - 1) < '0') {
                *(end_ptr - 1) = '\0';
                line_len--;
            }
            //FURI_LOG_D(TAG, " L#%d: [%d]%s", line_num, line_len, line_ptr);
            if(strncmp(line_ptr, SettingsFld_Rate, sizeof(SettingsFld_Rate) - 1) == 0) {
                NRF_rate = atoi(line_ptr + sizeof(SettingsFld_Rate));
            } else if(strncmp(line_ptr, SettingsFld_Ch, sizeof(SettingsFld_Ch) - 1) == 0) {
                NRF_channel = atoi(line_ptr + sizeof(SettingsFld_Ch));
            } else if(strncmp(line_ptr, SettingsFld_ESB, sizeof(SettingsFld_ESB) - 1) == 0) {
                NRF_ESB = atoi(line_ptr + sizeof(SettingsFld_ESB));
            } else if(strncmp(line_ptr, SettingsFld_DPL, sizeof(SettingsFld_DPL) - 1) == 0) {
                NRF_DPL = atoi(line_ptr + sizeof(SettingsFld_DPL));
            } else if(strncmp(line_ptr, SettingsFld_CRC, sizeof(SettingsFld_CRC) - 1) == 0) {
                NRF_CRC = atoi(line_ptr + sizeof(SettingsFld_CRC));
                if(what_to_do == 1) view_log_decode_CRC = NRF_CRC;
            } else if(strncmp(line_ptr, SettingsFld_Payload, sizeof(SettingsFld_Payload) - 1) == 0) {
                NRF_Payload = atoi(line_ptr + sizeof(SettingsFld_Payload));
                if(NRF_Payload == 0 || NRF_Payload > 32) NRF_Payload = 32;
            } else if(strncmp(line_ptr, SettingsFld_Sniff, sizeof(SettingsFld_Sniff) - 1) == 0) {
                what_to_do = 1;
            } else if(*line_ptr == SettingsFld_Addr) {
                char a = *(++line_ptr);
                line_ptr += 3;
                switch(a) {
                case '0':
                    addrs.addr_len =
                        ConvertHexToArray(line_ptr, addrs.addr_P0, what_to_do == 1 ? 3 : 5);
                    //FURI_LOG_D(TAG, " +Addr(%d): %02X%02X%02X...", addrs.addr_len, addrs.addr_P0[0], addrs.addr_P0[1], addrs.addr_P0[2]);
                    if(addrs.addr_len >= 2) err = 0;
                    break;
                case '1':
                    ConvertHexToArray(line_ptr, addrs.addr_P1, what_to_do == 1 ? 3 : 5);
                    //FURI_LOG_D(TAG, " +Addr: %02X%02X%02X...", addrs.addr_P0[1], addrs.addr_P1[1], addrs.addr_P1[2]);
                    break;
                case '2':
                    ConvertHexToArray(line_ptr, &addrs.addr_P2, 1);
                    break;
                case '3':
                    ConvertHexToArray(line_ptr, &addrs.addr_P3, 1);
                    break;
                case '4':
                    ConvertHexToArray(line_ptr, &addrs.addr_P4, 1);
                    break;
                case '5':
                    ConvertHexToArray(line_ptr, &addrs.addr_P5, 1);
                    break;
                default:
                    a = 0;
                    break;
                }
                if(err == 0 && a) addrs.addr_count = a - '0' + 1;
            } else if(line_len >= (NRF_Payload + 1) * 2) { // data
                if(!log_loaded) {
                    clear_log();
                    what_to_do = 0;
                    log_loaded = true;
                }
                if(log_arr_idx < MAX_LOG_RECORDS - 1) {
                    ConvertHexToArray(
                        line_ptr, APP->log_arr + log_arr_idx * LOG_REC_SIZE, LOG_REC_SIZE);
                    log_arr_idx++;
                }
            }
            line_ptr = end_ptr + 1;
            line_num++;
        }
    } else {
        FURI_LOG_D(TAG, "load failed. file size: %d", file_size);
        err = 4;
    }
    free(file_buf);
    return err;
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void prepare_nrf24(bool fsend_packet) {
    uint8_t erx_addr = (1 << 0); // Enable RX_P0
    if(!fsend_packet) {
        if(addrs.addr_count == 0) return;
        uint8_t payload = NRF_Payload;
        if(what_to_do == 1) { // SNIFF
            payload += 5 + NRF_CRC; // + addr_max + CRC
            if(NRF_ESB) payload += 2;
            if(payload > 32) payload = 32;
            nrf24_write_reg(nrf24_HANDLE, REG_CONFIG, 0x70); // Mask all interrupts
            nrf24_write_reg(nrf24_HANDLE, REG_STATUS, 0x70); // clear interrupts
            nrf24_write_reg(nrf24_HANDLE, REG_RF_SETUP, NRF_rate);
            nrf24_write_reg(nrf24_HANDLE, REG_SETUP_RETR, 0); // Automatic Retransmission
            nrf24_write_reg(nrf24_HANDLE, REG_EN_AA, 0); // Auto acknowledgement
            nrf24_write_reg(
                nrf24_HANDLE,
                REG_FEATURE,
                0); // Enables the W_TX_PAYLOAD_NOACK command, Disable Payload with ACK, set Dynamic Payload
        } else {
            nrf24_write_reg(
                nrf24_HANDLE,
                REG_CONFIG,
                0x70 | ((NRF_CRC == 1 ? 0b1000 :
                         NRF_CRC == 2 ? 0b1100 :
                                        0))); // Mask all interrupts
            nrf24_write_reg(nrf24_HANDLE, REG_STATUS, 0x70); // clear interrupts
            nrf24_write_reg(nrf24_HANDLE, REG_RF_SETUP, NRF_rate);
            nrf24_write_reg(
                nrf24_HANDLE, REG_SETUP_RETR, NRF_ESB ? 0x11 : 0); // Automatic Retransmission
            nrf24_write_reg(
                nrf24_HANDLE,
                REG_EN_AA,
                NRF_AA_OFF || !NRF_ESB ? 0 : 0x3F); // Auto acknowledgement
            nrf24_write_reg(
                nrf24_HANDLE,
                REG_FEATURE,
                NRF_DPL ?
                    4 + 1 :
                    1); // Enables the W_TX_PAYLOAD_NOACK command, Disable Payload with ACK, set Dynamic Payload
        }
        nrf24_set_maclen(nrf24_HANDLE, addrs.addr_len);
        nrf24_set_mac(REG_RX_ADDR_P0, addrs.addr_P0, addrs.addr_len);
        uint8_t tmp[5] = {0};
        nrf24_read_reg(nrf24_HANDLE, REG_RX_ADDR_P0, tmp, addrs.addr_len);
        for(uint8_t i = 0; i < addrs.addr_len / 2; i++) {
            uint8_t tb = tmp[i];
            tmp[i] = tmp[addrs.addr_len - i - 1];
            tmp[addrs.addr_len - i - 1] = tb;
        }
        NRF_ERROR = memcmp(addrs.addr_P0, tmp, addrs.addr_len) != 0;
        uint8_t dyn = 0;
        if(addrs.addr_count > 0 && NRF_DPL) dyn |= (1 << 0);
        FURI_LOG_D(TAG, "Payload: %d", payload);
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P0, payload);
        if(addrs.addr_count > 1) {
            nrf24_set_mac(REG_RX_ADDR_P1, addrs.addr_P1, addrs.addr_len);
            nrf24_write_reg(nrf24_HANDLE, RX_PW_P1, payload);
            if(NRF_DPL) dyn |= (1 << 1);
            erx_addr |= (1 << 1); // Enable RX_P1
        } else
            nrf24_write_reg(nrf24_HANDLE, RX_PW_P1, 0);
        if(addrs.addr_count > 2) {
            nrf24_write_buf_reg(nrf24_HANDLE, REG_RX_ADDR_P2, &addrs.addr_P2, 1);
            nrf24_write_reg(nrf24_HANDLE, RX_PW_P2, payload);
            if(NRF_DPL) dyn |= (1 << 2);
            erx_addr |= (1 << 2); // Enable RX_P2
        } else
            nrf24_write_reg(nrf24_HANDLE, RX_PW_P2, 0);
        if(addrs.addr_count > 3) {
            nrf24_write_buf_reg(nrf24_HANDLE, REG_RX_ADDR_P3, &addrs.addr_P3, 1);
            nrf24_write_reg(nrf24_HANDLE, RX_PW_P3, payload);
            if(NRF_DPL) dyn |= (1 << 3);
            erx_addr |= (1 << 3); // Enable RX_P3
        } else
            nrf24_write_reg(nrf24_HANDLE, RX_PW_P3, 0);
        if(addrs.addr_count > 4) {
            nrf24_write_buf_reg(nrf24_HANDLE, REG_RX_ADDR_P4, &addrs.addr_P4, 1);
            nrf24_write_reg(nrf24_HANDLE, RX_PW_P4, payload);
            if(NRF_DPL) dyn |= (1 << 4);
            erx_addr |= (1 << 4); // Enable RX_P4
        } else
            nrf24_write_reg(nrf24_HANDLE, RX_PW_P4, 0);
        if(addrs.addr_count > 5) {
            nrf24_write_buf_reg(nrf24_HANDLE, REG_RX_ADDR_P5, &addrs.addr_P5, 1);
            nrf24_write_reg(nrf24_HANDLE, RX_PW_P5, payload);
            if(NRF_DPL) dyn |= (1 << 5);
            erx_addr |= (1 << 5); // Enable RX_P5
        } else
            nrf24_write_reg(nrf24_HANDLE, RX_PW_P5, 0);
        nrf24_write_reg(nrf24_HANDLE, REG_DYNPD, dyn); // Enable dynamic payload reg
        nrf24_write_reg(nrf24_HANDLE, REG_EN_RXADDR, erx_addr);
        nrf24_write_reg(nrf24_HANDLE, REG_RF_CH, NRF_channel);
    }
    nrf24_flush_rx(nrf24_HANDLE);
    nrf24_flush_tx(nrf24_HANDLE);
    nrf24_set_idle(nrf24_HANDLE);
}

static void start_scanning() {
    FURI_LOG_D(TAG, "Start proc-%d: Ch=%d Rate=%d", what_to_do, NRF_channel, NRF_rate);
    if(what_to_do == 1) {
        NRF_Sniff_payload_max = 32 - 3 - (NRF_ESB ? 2 : 0) - NRF_CRC + (addrs.addr_len - 2);
        if(NRF_Payload > NRF_Sniff_payload_max) NRF_Payload = NRF_Sniff_payload_max;
        view_log_decode_CRC = NRF_CRC;
    }
    prepare_nrf24(false);
    if(NRF_ERROR) {
        FURI_LOG_E(TAG, "NRF R/W ERROR!");
        return;
    }
    nrf24_set_rx_mode(nrf24_HANDLE);
    start_time = furi_get_tick();
}

// start bitnum = 7
uint16_t calc_crc(uint32_t crc, uint8_t* ptr, uint8_t bitnum, uint16_t bits) {
    //uint8_t bitnum = 7;
    uint32_t crc_high, polynom;
    if(view_log_decode_CRC == 2) {
        crc_high = (1 << 16);
        polynom = 0x11021; // X^16+X^12+X^5+1
    } else {
        crc_high = (1 << 8);
        polynom = 0x107; // x^8+x^2+x^1+1
    }
    while(bits--) {
        crc <<= 1;
        if(((crc & crc_high) != 0) ^ ((*ptr >> bitnum) & 1)) crc ^= polynom;
        if(bitnum == 0) {
            ptr++;
            bitnum = 7;
        } else
            bitnum--;
    }
    return crc & (view_log_decode_CRC == 2 ? 0xFFFF : 0xFF);
}

// shifted 1 bit right
uint16_t get_shifted_crc(uint8_t* pcrc) {
    uint16_t crc = ((uint8_t)(*pcrc << 1)) | (*(pcrc + 1) >> 7);
    if(view_log_decode_CRC == 2) {
        crc = (crc << 8) | (((uint8_t)(*(pcrc + 1) << 1))) | (*(pcrc + 2) >> 7);
    }
    return crc;
}

bool check_packet(uint8_t* pkt, uint8_t size) {
    for(uint8_t addr_size = 3; addr_size <= 5; addr_size++) {
        if(NRF_ESB) {
            uint8_t b = *(pkt + addr_size) >> 2;
            if((b > NRF_Sniff_payload_max && b != 0x33)) continue;
            if(furi_log_get_level() == FuriLogLevelDebug && addr_size == 3) {
                char dbuf[70];
                dbuf[0] = 0;
                add_to_str_hex_bytes(dbuf, (char*)pkt, size);
                FURI_LOG_D(TAG, "PKT%d: %s (%d)", *(pkt - 1), dbuf, size);
            }
            if(b != 0x33) { // DPL
                uint16_t crc = view_log_decode_CRC == 2 ? 0xFFFF : 0xFF;
                crc = calc_crc(crc, pkt, 7, 9 + b * 8);
                FURI_LOG_D(TAG, "DCRC: %X - %X", crc, get_shifted_crc(pkt + b + 1));
                if(crc == get_shifted_crc(pkt + b + 1)) {
                    *(pkt - 1) = ((b & 0x1F) << 3) + 0b100 + (addr_size - 2);
                    FURI_LOG_D(TAG, "VALID: pl: %d, addr: %d", b, addr_size);
                    return true;
                }
            } else {
                for(uint8_t i = 0; i < size - view_log_decode_CRC; i++) {
                    uint16_t crc = view_log_decode_CRC == 2 ? 0xFFFF : 0xFF;
                    crc = calc_crc(crc, pkt, 7, 9 + i * 8);
                    FURI_LOG_D(TAG, "CRC: %X - %X", crc, get_shifted_crc(pkt + i + 1));
                    if(crc == get_shifted_crc(pkt + i + 1)) {
                        *(pkt - 1) = ((i & 0x1F) << 3) + 0b100 + (addr_size - 2);
                        FURI_LOG_D(TAG, "VALID: pl: %d, addr: %d", i, addr_size);
                        return true;
                    }
                }
            }
        } else {
            for(uint8_t i = 0; i < size - view_log_decode_CRC; i++) {
                uint16_t crc = view_log_decode_CRC == 2 ? 0xFFFF : 0xFF;
                crc = calc_crc(crc, pkt, 7, i * 8);
                if((view_log_decode_CRC == 1 && crc == *(pkt + i + 1)) ||
                   (view_log_decode_CRC == 2 && crc == ((*(pkt + i + 1) << 8) | *(pkt + i + 2)))) {
                    *(pkt - 1) = ((i & 0x1F) << 3) + 0b000 + (addr_size - 2);
                    return true;
                }
            }
        }
    }
    return false;
}

bool nrf24_read_newpacket() {
    if(APP->log_arr == NULL) return false;
    bool found = false;
    uint8_t packetsize;
    uint8_t* ptr = APP->log_arr + log_arr_idx * LOG_REC_SIZE;
    uint8_t st = nrf24_rxpacket(
        nrf24_HANDLE, ptr + 2 + (what_to_do == 1 ? addrs.addr_len - 2 : 0), &packetsize, !NRF_DPL);
    if(st & RX_DR) {
        st = (st >> 1) & 7; // pipe #
        if(what_to_do == 1) {
            *ptr++ = NRF_channel | 0x80;
            *ptr++ = st; // pipe #
            if(addrs.addr_len > 2) {
                *ptr = st == 0 ? addrs.addr_P0[2] :
                       st == 1 ? addrs.addr_P1[2] :
                       st == 2 ? addrs.addr_P2 :
                       st == 3 ? addrs.addr_P3 :
                       st == 4 ? addrs.addr_P4 :
                                 addrs.addr_P5;
            }
            if(!check_packet(ptr, packetsize)) {
                if(addrs.addr_len > 2) return false; // skip if mac MSB added to preamble
                if(addrs.addr_count == 1 &&
                   addrs.addr_P0[1] == 0xAA) { // Shift packet right by one bit if preamble = 0xAA
                    for(uint8_t i = packetsize - 1; i > 0; i--) {
                        ptr[i] = ptr[i - 1] << 7 | ptr[i] >> 1;
                    }
                    *ptr >>= 1;
                    //if((st == 0 && (addrs.addr_P0[1] & 1)) || (st == 1 && (addrs.addr_P1[1] & 1))) *ptr |= 0x80;
                    if(!check_packet(ptr, packetsize)) return false;
                } else
                    return false;
            }
        } else {
            *ptr++ = NRF_channel;
            *ptr++ = ((packetsize & 0x1F) << 3) | st; // payload size + pipe #
        }
        if(packetsize < 32) memset(ptr + packetsize, 0, 32 - packetsize);
        if(log_arr_idx < MAX_LOG_RECORDS - 1) {
            log_arr_idx++;
        } else {
            if(log_to_file == 1 || log_to_file == 2) {
                write_to_log_file(APP->storage, false);
                clear_log();
            } else {
                memmove(APP->log_arr, APP->log_arr + LOG_REC_SIZE, log_arr_idx * LOG_REC_SIZE);
            }
        }
        FURI_LOG_D(TAG, "Found packet #%d pipe %d", log_arr_idx, st);
        notification_message(APP->notification, &sequence_blink_white_100);
        found = true;
    }
    return found;
}

bool nrf24_send_packet() {
    if(log_arr_idx == 0) return false;
    prepare_nrf24(!what_to_do);
    uint8_t* ptr = APP->log_arr + view_log_arr_idx * LOG_REC_SIZE;
    nrf24_write_reg(nrf24_HANDLE, REG_RF_CH, *ptr & 0x7F);
    if(*ptr & 0x80) { // RAW packet
        //uint8_t pktinfo = *(ptr + 1);
        //nrf24_set_maclen(nrf24_HANDLE, (pktinfo & 0b11) + 2);
        nrf24_set_maclen(nrf24_HANDLE, 2);
        //if(pktinfo & 0b100) { // ESB
        nrf24_write_reg(nrf24_HANDLE, REG_SETUP_RETR, 0); // No Automatic Retransmission
        nrf24_write_reg(nrf24_HANDLE, REG_EN_AA, 0); // No Auto acknowledgement
        //}
        //uint8_t alen = (*(ptr + 2) & 0b11) + 2;
        uint8_t adr[2] = {0x55, 0x55}; // NOT TESTED!
        if(*(ptr + 2) & 0x80) adr[0] = adr[1] = 0xAA;
        nrf24_set_mac(REG_RX_ADDR_P0, adr, 2);
        nrf24_set_mac(REG_TX_ADDR, adr, 2);
        last_packet_send_st = nrf24_txpacket(nrf24_HANDLE, ptr + 2, 32, false);
    } else {
        nrf24_write_reg(
            nrf24_HANDLE, REG_SETUP_RETR, NRF_ESB ? 0x11 : 0); // Automatic Retransmission
        nrf24_write_reg(
            nrf24_HANDLE, REG_EN_AA, NRF_AA_OFF || !NRF_ESB ? 0 : 0x3F); // Auto acknowledgement
        uint8_t* adr;
        uint8_t a = *(ptr + 1) & 0b111;
        if(a < 2) {
            if(a == 0)
                adr = addrs.addr_P0;
            else
                adr = addrs.addr_P1;
            nrf24_set_mac(REG_RX_ADDR_P0, adr, addrs.addr_len);
            nrf24_set_mac(REG_TX_ADDR, adr, addrs.addr_len);
        } else {
            uint8_t buf[5];
            memcpy(buf, addrs.addr_P1, addrs.addr_len - 1);
            buf[addrs.addr_len - 1] = a == 2 ? addrs.addr_P2 :
                                      a == 3 ? addrs.addr_P3 :
                                      a == 4 ? addrs.addr_P4 :
                                               addrs.addr_P5;
            nrf24_set_mac(REG_RX_ADDR_P0, buf, addrs.addr_len);
            nrf24_set_mac(REG_TX_ADDR, buf, addrs.addr_len);
        }
        a = *(ptr + 1) >> 3;
        if(a == 0) a = 32;
        last_packet_send_st = nrf24_txpacket(nrf24_HANDLE, ptr + 2, a, false);
    }
    last_packet_send = view_log_arr_idx;
    notification_message(
        APP->notification,
        last_packet_send_st ? &sequence_blink_blue_100 : &sequence_blink_red_100);
    if(what_to_do) start_scanning();
    return last_packet_send_st;
}

static void render_callback(Canvas* const canvas, void* ctx) {
    const PluginState* plugin_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(plugin_state == NULL) return;
    //canvas_draw_frame(canvas, 0, 0, 128, 64); // border around the edge of the screen
    if(what_doing == 0) {
        canvas_set_font(canvas, FontSecondary); // 8x10 font
        if(save_settings)
            snprintf(
                screen_buf, sizeof(screen_buf), "Save: %s", SETTINGS_FILENAME); // menu_selected = 0
        else
            snprintf(screen_buf, sizeof(screen_buf), "Load: %s", addr_file_name);
        canvas_draw_str(canvas, 10, 10, screen_buf);
        snprintf(screen_buf, sizeof(screen_buf), "Ch: %d", NRF_channel); // menu_selected = 1
        canvas_draw_str(canvas, 10, 20, screen_buf);
        if(NRF_ESB) {
            strcpy(screen_buf, "ESB");
            if(NRF_DPL) strcat(screen_buf, " DPL");
            canvas_draw_str(canvas, 80, 20, screen_buf);
        }
        if(NRF_AA_OFF) {
            canvas_draw_str(canvas, 61, 20, "AA");
            canvas_draw_line(canvas, 60, 21, 72, 12);
        }
        snprintf(
            screen_buf,
            sizeof(screen_buf),
            "Rate: %sbps",
            NRF_rate == 2 ? "2M" :
            NRF_rate == 1 ? "1M" :
                            "250K"); // menu_selected = 2
        canvas_draw_str(canvas, 10, 30, screen_buf);
        snprintf(screen_buf, sizeof(screen_buf), "Payload: %d", NRF_Payload);
        canvas_draw_str(canvas, 80, 30, screen_buf);
        strcpy(screen_buf, "Next Ch time: "); // menu_selected = 3
        if(find_channel_period == 0)
            strcat(screen_buf, "off");
        else
            snprintf(
                screen_buf + strlen(screen_buf), sizeof(screen_buf), "%d s", find_channel_period);
        canvas_draw_str(canvas, 10, 40, screen_buf);
        if(NRF_CRC == 1)
            canvas_draw_str(canvas, 99, 40, "CRC1");
        else if(NRF_CRC == 2)
            canvas_draw_str(canvas, 99, 40, "CRC2");
        snprintf(
            screen_buf,
            sizeof(screen_buf),
            "Log: %s",
            log_to_file == 0 ? "No" :
            log_to_file == 1 ? "Yes" :
            log_to_file == 2 ? "Append" :
                               "Clear"); // menu_selected = 4
        canvas_draw_str(canvas, 10, 50, screen_buf);
        if(what_to_do) { // menu_selected = 5
            if(NRF_ERROR)
                snprintf(screen_buf, sizeof(screen_buf), "nRF24L01+ R/W ERROR!");
            else {
                if(what_to_do == 1)
                    snprintf(screen_buf, sizeof(screen_buf), "Start sniff");
                else
                    snprintf(
                        screen_buf, sizeof(screen_buf), "Start scan (pipes: %d)", addrs.addr_count);
            }
        } else
            snprintf(screen_buf, sizeof(screen_buf), "View log (pipes: %d)", addrs.addr_count);
        canvas_draw_str(canvas, 10, 60, screen_buf);
        canvas_draw_str(canvas, 0, menu_selected * 10 + 10, ">");
    } else if(what_doing == 1) {
        canvas_set_font(canvas, FontBatteryPercent); // 5x7 font, 9 lines
        bool ch2 = false;
        screen_buf[0] = '\0';
        if(view_log_arr_x == 0) {
            strcat(screen_buf, "  ");
            ch2 = true;
        } else {
            snprintf(screen_buf, sizeof(screen_buf), "<%d", view_log_arr_x);
            if(view_log_arr_x < VIEW_LOG_MAX_X) ch2 = true;
        }
        snprintf(
            screen_buf + strlen(screen_buf),
            sizeof(screen_buf),
            " %s ch: %d - %d.",
            what_to_do == 1 ? "Sniff" :
            what_to_do == 2 ? "Read" :
                              "View",
            NRF_channel,
            log_arr_idx);
        canvas_draw_str(canvas, 0, 7, screen_buf);
        if(ch2) canvas_draw_str(canvas, 121, 7, ">");
        if(log_arr_idx) {
            if(view_log_arr_idx >= log_arr_idx) view_log_arr_idx = log_arr_idx - 1;
            uint16_t page = view_log_arr_idx & ~7;
            for(uint8_t i = 0; i < 8 && page + i < log_arr_idx; i++) {
                screen_buf[0] = (view_log_arr_idx & 7) != i          ? ' ' :
                                last_packet_send != view_log_arr_idx ? '>' :
                                last_packet_send_st                  ? '*' :
                                                                       '!';
                screen_buf[1] = '\0';
                uint8_t* ptr = APP->log_arr + (page + i) * LOG_REC_SIZE;
                uint8_t channel = *ptr++;
                uint8_t* crcptr = NULL;
                uint8_t pre = 0;
                int count = 0;
                if(channel & 0x80) { // RAW packet: nn:>{.address..}-xxxxxxxx
                    uint8_t pktinfo = *ptr++;
                    bool _PCF = pktinfo & 0b100;
                    uint8_t count = (pktinfo >> 3);
                    uint8_t adrsize = (pktinfo & 0b11) + 2;
                    uint8_t plen = adrsize + count;
                    count -= view_log_arr_x;
                    uint8_t max_width = VIEW_LOG_WIDTH_B;
                    if(view_log_arr_x == 0) max_width -= 5;
                    if(count > max_width) count = max_width;
                    if(count > 0) {
                        uint8_t* pcrc = ptr;
                        uint16_t crc;
                        crc = view_log_decode_CRC == 2 ? 0xFFFF : 0xFF;
                        crc = calc_crc(crc, pcrc, 7, (_PCF ? 9 : 0) + plen * 8);
                        pcrc += plen;
                        if(_PCF) { //ESB
                            pcrc++;
                            if(crc == get_shifted_crc(pcrc)) crcptr = pcrc;
                        } else {
                            if((view_log_decode_CRC == 1 && crc == *pcrc) ||
                               (view_log_decode_CRC == 2 && crc == ((*pcrc << 8) | *(pcrc + 1)))) {
                                crcptr = pcrc;
                            }
                        }
                        if(view_log_arr_x == 0) {
                            add_to_str_hex_bytes(screen_buf, (char*)ptr, adrsize);
                            for(int8_t j = 5 - adrsize; j > 0; j--) strcat(screen_buf, "  ");
                            strcat(screen_buf, "-");
                            pre += 5 * 2 + 1;
                        } else {
                            ptr += view_log_arr_x - 1;
                        }
                        ptr += adrsize;
                        if(_PCF)
                            add_to_str_hex_bytes_shift_9b(screen_buf, (char*)ptr, count);
                        else
                            add_to_str_hex_bytes(screen_buf, (char*)ptr, count);
                    }
                } else {
                    uint8_t dpl = *ptr++;
                    uint8_t pipe = dpl & 0b111;
                    dpl >>= 3;
                    if(dpl == 0) dpl = 32;
                    count = dpl - view_log_arr_x;
                    if(view_log_decode_PCF) count--;
                    uint8_t max_width = VIEW_LOG_WIDTH_B;
                    if(view_log_arr_x == 0) {
                        if(addrs.addr_count > 1) max_width--;
                        if(view_log_decode_PCF) max_width -= 2;
                    }
                    if(count > max_width) count = max_width;
                    if(count > 0) {
                        if(view_log_decode_CRC) {
                            static uint16_t prev_addr_CRC;
                            static int8_t prev_pipe = -1;
                            uint8_t* pcrc = APP->log_arr + (page + i) * LOG_REC_SIZE + 2;
                            uint16_t crc;
                            if(prev_pipe == pipe) {
                                crc = prev_addr_CRC;
                            } else {
                                crc = view_log_decode_CRC == 2 ? 0xFFFF : 0xFF;
                                if(pipe <= 1) {
                                    crc = calc_crc(
                                        crc,
                                        pipe == 0 ? addrs.addr_P0 : addrs.addr_P1,
                                        7,
                                        addrs.addr_len * 8);
                                } else {
                                    crc =
                                        calc_crc(crc, addrs.addr_P1, 7, (addrs.addr_len - 1) * 8);
                                    crc = calc_crc(
                                        crc,
                                        pipe == 2 ? &addrs.addr_P2 :
                                        pipe == 3 ? &addrs.addr_P3 :
                                        pipe == 4 ? &addrs.addr_P4 :
                                                    &addrs.addr_P5,
                                        7,
                                        8);
                                }
                                prev_addr_CRC = crc;
                                prev_pipe = pipe;
                            }
                            if(view_log_decode_PCF) {
                                crc = calc_crc(crc, pcrc++, 7, 9);
                                if(crc == get_shifted_crc(pcrc)) crcptr = pcrc;
                                if(crcptr == NULL) {
                                    for(int8_t j = 0; j < (int8_t)dpl - view_log_decode_CRC - 1;
                                        j++) {
                                        crc = calc_crc(crc, pcrc++, 6, 8);
                                        if(crc == get_shifted_crc(pcrc)) {
                                            crcptr = pcrc;
                                            break;
                                        }
                                    }
                                }
                            } else {
                                for(int8_t j = 0; j < (int8_t)dpl - view_log_decode_CRC; j++) {
                                    crc = calc_crc(crc, pcrc++, 7, 8);
                                    if((view_log_decode_CRC == 1 && crc == *pcrc) ||
                                       (view_log_decode_CRC == 2 &&
                                        crc == ((*pcrc << 8) | *(pcrc + 1)))) {
                                        crcptr = pcrc;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    ptr += view_log_arr_x;
                    if(max_width < VIEW_LOG_WIDTH_B) {
                        pre += snprintf(screen_buf + 1, 10, "%X-", pipe);
                        if(view_log_decode_PCF) {
                            pre += snprintf(
                                screen_buf + strlen(screen_buf),
                                10,
                                "%02X%01X-",
                                *ptr >> 2,
                                ((*ptr & 3) << 1) | (*(ptr + 1) >> 7));
                        }
                    }
                    if(view_log_decode_PCF)
                        add_to_str_hex_bytes_shift_9b(screen_buf, (char*)ptr, count);
                    else
                        add_to_str_hex_bytes(screen_buf, (char*)ptr, count);
                }
                uint16_t y = 14 + i * 7;
                canvas_draw_str(canvas, 3 * 5, y, screen_buf);
                uint16_t x = snprintf(screen_buf, sizeof(screen_buf), "%d", page + i + 1);
                canvas_draw_str(canvas, 0, y, screen_buf);
                if(crcptr) { // 5x7 font, 9 lines
                    canvas_draw_str(canvas, x * 5, y, "=");
                    int n = crcptr - (uint8_t*)ptr - 1;
                    if(n > -view_log_decode_CRC && n < count) {
                        int len;
                        x = (4 + pre) * 5;
                        if(n < 0) {
                            len = view_log_decode_CRC + n;
                            n = 0;
                        } else {
                            len = MIN(view_log_decode_CRC, count - n);
                            x += n * 2 * 5;
                            canvas_draw_line(canvas, x - 1, y, x - 1, y - 1);
                        }
                        canvas_draw_line(canvas, x - 1, y, n = x + len * 2 * 5 - 1, y);
                        canvas_draw_line(canvas, n, y, n, y - 1);
                    }
                } else
                    canvas_draw_str(canvas, x * 5, y, ":");
            }
        }
    } else {
        canvas_set_font(canvas, FontBatteryPercent); // 5x7 font, 9 lines
        if(addrs.addr_count > 0) {
            snprintf(screen_buf, sizeof(screen_buf), "P0: ");
            add_to_str_hex_bytes(screen_buf, (char*)addrs.addr_P0, addrs.addr_len);
            canvas_draw_str(canvas, 0, 1 * 7, screen_buf);
        }
        if(addrs.addr_count > 1) {
            snprintf(screen_buf, sizeof(screen_buf), "P1: ");
            add_to_str_hex_bytes(screen_buf, (char*)addrs.addr_P1, addrs.addr_len);
            canvas_draw_str(canvas, 0, 2 * 7, screen_buf);
        }
        if(addrs.addr_count > 2) {
            canvas_draw_str(canvas, 0, 3 * 7, "P2: ");
            snprintf(screen_buf, sizeof(screen_buf), "%02X", addrs.addr_P2);
            canvas_draw_str(canvas, (4 + (addrs.addr_len - 1) * 2) * 5, 3 * 7, screen_buf);
        }
        if(addrs.addr_count > 3) {
            canvas_draw_str(canvas, 0, 4 * 7, "P3: ");
            snprintf(screen_buf, sizeof(screen_buf), "%02X", addrs.addr_P3);
            canvas_draw_str(canvas, (4 + (addrs.addr_len - 1) * 2) * 5, 4 * 7, screen_buf);
        }
        if(addrs.addr_count > 4) {
            canvas_draw_str(canvas, 0, 5 * 7, "P4: ");
            snprintf(screen_buf, sizeof(screen_buf), "%02X", addrs.addr_P4);
            canvas_draw_str(canvas, (4 + (addrs.addr_len - 1) * 2) * 5, 5 * 7, screen_buf);
        }
        if(addrs.addr_count > 5) {
            canvas_draw_str(canvas, 0, 6 * 7, "P5: ");
            snprintf(screen_buf, sizeof(screen_buf), "%02X", addrs.addr_P5);
            canvas_draw_str(canvas, (4 + (addrs.addr_len - 1) * 2) * 5, 6 * 7, screen_buf);
        }
        if(log_arr_idx) {
            uint8_t* ptr = APP->log_arr + view_log_arr_idx * LOG_REC_SIZE;
            snprintf(
                screen_buf,
                sizeof(screen_buf),
                "Current Ch: %d, size: %d",
                *ptr & 0x7F,
                *(ptr + 1) >> 3);
            canvas_draw_str(canvas, 0, 7 * 7, screen_buf);
        }
        screen_buf[0] = 'v';
        strcpy(screen_buf + 1, VERSION);
        canvas_draw_str(canvas, 108, 7, screen_buf);
        if(view_log_decode_PCF || view_log_decode_CRC) {
            strcpy(screen_buf, "Decode: ");
            if(view_log_decode_PCF) strcat(screen_buf, "PCF ");
            if(view_log_decode_CRC == 1)
                strcat(screen_buf, "CRC1");
            else if(view_log_decode_CRC == 2)
                strcat(screen_buf, "CRC2");
            canvas_draw_str(canvas, 0, 64, screen_buf);
        }
    }
    release_mutex((ValueMutex*)ctx, plugin_state);
}

int32_t nrf24scan_app(void* p) {
    UNUSED(p);
    APP = malloc(sizeof(Nrf24Scan));
    APP->event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    APP->plugin_state = malloc(sizeof(PluginState));
    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, APP->plugin_state, sizeof(PluginState))) {
        furi_message_queue_free(APP->event_queue);
        FURI_LOG_E(TAG, "cannot create mutex");
        free(APP->plugin_state);
        return 255;
    }
    memset((uint8_t*)&addrs, 0, sizeof(addrs));
    nrf24_init();

    // Set system callbacks
    APP->view_port = view_port_alloc();
    view_port_draw_callback_set(APP->view_port, render_callback, &state_mutex);
    view_port_input_callback_set(APP->view_port, input_callback, APP->event_queue);

    // Open GUI and register view_port
    APP->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(APP->gui, APP->view_port, GuiLayerFullscreen);
    APP->notification = furi_record_open(RECORD_NOTIFICATION);
    APP->storage = furi_record_open(RECORD_STORAGE);
    storage_common_mkdir(APP->storage, SCAN_APP_PATH_FOLDER);
    Stream* file_stream = file_stream_alloc(APP->storage);
    FuriString* path = furi_string_alloc();
    furi_string_set(path, SCAN_APP_PATH_FOLDER);
    furi_string_cat(path, "/");
    furi_string_cat(path, SNIFF_FILENAME);
    if(file_stream_open(file_stream, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint8_t err = load_settings_file(file_stream);
        if(!err)
            strncpy(
                addr_file_name,
                furi_string_get_cstr(path) + sizeof(SCAN_APP_PATH_FOLDER),
                sizeof(addr_file_name));
        else
            snprintf(addr_file_name, sizeof(addr_file_name), "LOAD ERROR#%d", err);
    } else {
        strcpy(addr_file_name, "NONE");
        if(what_to_do == 1) {
            addrs.addr_P0[0] = 0;
            addrs.addr_P0[1] = 0xAA;
            addrs.addr_len = 2;
            addrs.addr_count = 1;
            view_log_decode_CRC = NRF_CRC = 2;
        }
    }
    file_stream_close(file_stream);
    stream_free(file_stream);
    furi_string_free(path);
    allocate_log_array();

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(APP->event_queue, &event, 100);
        PluginState* plugin_state = (PluginState*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                //FURI_LOG_D(TAG, "Key: %d Type: %d Sec: %u", event.input.key, event.input.type, event.input.sequence);
                switch(event.input.key) {
                case InputKeyUp:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(what_doing == 0) {
                            if(menu_selected > 0)
                                menu_selected--;
                            else
                                menu_selected = menu_selected_max;
                        } else if(what_doing == 1) {
                            view_log_arr_idx -= event.input.type == InputTypeRepeat ? 10 : 1;
                            if(view_log_arr_idx >= log_arr_idx) view_log_arr_idx = 0;
                        }
                    }
                    break;
                case InputKeyDown:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(what_doing == 0) {
                            if(menu_selected < menu_selected_max)
                                menu_selected++;
                            else
                                menu_selected = 0;
                        } else if(what_doing == 1) {
                            view_log_arr_idx += event.input.type == InputTypeRepeat ? 10 : 1;
                            if(view_log_arr_idx >= log_arr_idx) view_log_arr_idx = log_arr_idx - 1;
                        }
                    }
                    break;
                case InputKeyLeft:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(what_doing == 0) {
                            switch(menu_selected) {
                            case Menu_enter_channel:
                                NRF_channel -= event.input.type == InputTypeRepeat ? 10 : 1;
                                if(NRF_channel > MAX_CHANNEL) NRF_channel = MAX_CHANNEL;
                                break;
                            case Menu_enter_rate:
                                NRF_Payload -= event.input.type == InputTypeRepeat ? 10 : 1;
                                if(NRF_Payload == 0 || NRF_Payload > 32) NRF_Payload = 1;
                                break;
                            case Menu_enter_scan_period:
                                find_channel_period -= event.input.type == InputTypeRepeat ? 10 :
                                                                                             1;
                                if(find_channel_period < 0) find_channel_period = 0;
                                break;
                            case Menu_log:
                                if(--log_to_file < -1) log_to_file = 2;
                                break;
                            case Menu_ok:
                                if(++what_to_do > 2) what_to_do = 0;
                                break;
                            }
                        } else if(what_doing == 1) {
                            if(view_log_arr_x > 0) view_log_arr_x--;
                        } else if(what_doing == 2) {
                            //if(NRF_ESB == 0)
                            view_log_decode_PCF ^= 1;
                        }
                    }
                    break;
                case InputKeyRight:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(what_doing == 0) {
                            switch(menu_selected) {
                            case Menu_open_file:
                                save_settings ^= 1;
                                break;
                            case Menu_enter_channel:
                                NRF_channel += event.input.type == InputTypeRepeat ? 10 : 1;
                                if(NRF_channel > MAX_CHANNEL) NRF_channel = 0;
                                break;
                            case Menu_enter_rate:
                                NRF_Payload += event.input.type == InputTypeRepeat ? 10 : 1;
                                if(NRF_Payload == 0 || NRF_Payload > 32) NRF_Payload = 32;
                                break;
                            case Menu_enter_scan_period:
                                find_channel_period += event.input.type == InputTypeRepeat ? 10 :
                                                                                             1;
                                break;
                            case Menu_log:
                                if(++log_to_file > 2) log_to_file = -1;
                                break;
                            case Menu_ok:
                                if(++what_to_do > 2) what_to_do = 0;
                                break;
                            }
                        } else if(what_doing == 1) {
                            if(view_log_arr_x < VIEW_LOG_MAX_X) view_log_arr_x++;
                        } else if(what_doing == 2) {
                            if(++view_log_decode_CRC > 2) view_log_decode_CRC = 0;
                        }
                    }
                    break;
                case InputKeyOk:
                    if(event.input.type == InputTypeShort) {
                        if(what_doing == 0) {
                            switch(menu_selected) {
                            case Menu_open_file:
                                if(save_settings) {
                                    write_to_log_file(APP->storage, true);
                                } else {
                                    file_stream = file_stream_alloc(APP->storage);
                                    if(select_settings_file(file_stream)) {
                                        uint8_t err = load_settings_file(file_stream);
                                        if(!err)
                                            save_to_new_log = true;
                                        else
                                            snprintf(
                                                addr_file_name,
                                                sizeof(addr_file_name),
                                                "LOAD ERROR#%d",
                                                err);
                                        file_stream_close(file_stream);
                                    }
                                    stream_free(file_stream);
                                }
                                break;
                            case Menu_enter_channel:
                                if(NRF_ESB) {
                                    if(NRF_DPL)
                                        NRF_DPL = NRF_ESB = 0;
                                    else
                                        NRF_DPL = 1;
                                } else
                                    NRF_ESB = 1;
                                //if(NRF_ESB) view_log_decode_PCF = 0;
                                break;
                            case Menu_enter_rate:
                                NRF_rate++;
                                if(NRF_rate > 2) NRF_rate = 0;
                                break;
                            case Menu_enter_scan_period:
                                if(++NRF_CRC > 2) NRF_CRC = what_to_do == 1 ? 1 : 0;
                                break;
                            case Menu_ok:
                                if(what_to_do) {
                                    if(addrs.addr_count || what_to_do == 1) {
                                        if(log_to_file == -1) {
                                            log_to_file = 0;
                                            clear_log();
                                            save_to_new_log = true;
                                        } else if(log_to_file == 1)
                                            save_to_new_log = true;
                                        start_scanning();
                                        if(!NRF_ERROR) what_doing = 1;
                                    }
                                } else
                                    what_doing = 1;
                                key_press_seq_ok = event.input.sequence;
                                break;
                            }
                        } else if(what_doing == 1) {
                            nrf24_send_packet();
                        }
                    } else if(event.input.type == InputTypeLong) {
                        if(what_doing == 0) {
                            if(menu_selected == Menu_enter_channel) {
                                NRF_AA_OFF ^= 1;
                                key_press_seq_ok = event.input.sequence;
                            } else if(menu_selected == Menu_log) { // Log
                                if(log_arr_idx && (log_to_file == 1 || log_to_file == 2)) {
                                    write_to_log_file(APP->storage, false);
                                    clear_log();
                                }
                            }
                        } else if(what_doing == 1) {
                            what_doing = 2;
                        }
                    }
                    break;
                case InputKeyBack:
                    if(event.input.type == InputTypeLong)
                        processing = false;
                    else if(event.input.type == InputTypeShort) {
                        if(what_doing) what_doing--;
                        if(what_doing == 0) nrf24_set_idle(nrf24_HANDLE);
                        ;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        if(what_doing && what_to_do) {
            nrf24_read_newpacket();
            if(find_channel_period &&
               furi_get_tick() - start_time >= (uint32_t)find_channel_period * 1000UL) {
                if(++NRF_channel > MAX_CHANNEL) NRF_channel = 0;
                start_scanning();
            }
        }

        view_port_update(APP->view_port);
        release_mutex(&state_mutex, plugin_state);
    }
    nrf24_set_idle(nrf24_HANDLE);
    if(log_arr_idx && (log_to_file == 1 || log_to_file == 2)) {
        write_to_log_file(APP->storage, false);
    }
    nrf24_deinit();

    view_port_enabled_set(APP->view_port, false);
    gui_remove_view_port(APP->gui, APP->view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_STORAGE);
    view_port_free(APP->view_port);
    furi_message_queue_free(APP->event_queue);
    free(APP->plugin_state);
    if(APP->log_arr) free(APP->log_arr);
    free(APP);
    return 0;
}
