//
// Written by vad7, 10.01.2023. vad7@yahoo.com
//
#include "nrf24batch.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <dialogs/dialogs.h>
#include <input/input.h>
#include <stdlib.h>
#include <dolphin/dolphin.h>
#include <nrf24.h>
#include <u8g2.h>

#define TAG "nrf24batch"
#define VERSION "1.9"

#define SCAN_APP_PATH_FOLDER STORAGE_APP_DATA_PATH_PREFIX
#define LOG_FILEEXT ".txt"
#define NRF_READ_TIMEOUT 300UL // ms
#define WORK_PERIOD 2 // ms, Timer period
#define MAX_CHANNEL 125
#define FONT_5x7_SCREEN_WIDTH 25
#define NRF_EN_DYN_ACK 0 // does not work on some nrf24l01+ chips, (0/1)

const char SettingsFld_Info[] = "Info:";
const char SettingsFld_Ch[] = "Ch:";
const char SettingsFld_Rate[] = "Rate:";
const char SettingsFld_DPL[] = "DPL:";
const char SettingsFld_CRC[] = "CRC:";
const char SettingsFld_RETR[] = "RETR:";
const char SettingsFld_Address[] = "Address:";
const char SettingsFld_Resend[] = "Resend:";
const char SettingsFld_Delay[] = "Delay_ms:";
const char SettingsFld_WriteStart[] = "Write start:";
const char SettingsFld_Payload[] = "Payload struct:";
const char SettingsFld_ReadDefault[] = "R default:";
const char SettingsFld_WriteDefault[] = "W default:";
const char SettingsFld_Read[] = "R:"; // Read cmd
const char SettingsFld_Write[] = "W:"; // Write cmd
const char SettingsFld_Set[] = "S:"; // Set cmd (like Write but without "Write start" packet)
const char SettingsFld_ReadBatch[] = "RBatch:";
const char SettingsFld_WriteBatch[] = "WBatch:";
const char SettingsFld_SetBatch[] = "SBatch:";
const char SettingsFld_Listen[] = "Listen:";
const char SettingsFld_ReadCmdRepeatPeriod[] = "ReadCmd repeat:";
const char AskQuestion_Save[] = "SAVE BATCH?";
#define Settings_i 'i'
#define Settings_n 'n'
#define VAR_EMPTY ((int32_t)0x80000000)

nRF24Batch* APP;
uint8_t what_doing = 0; // 0 - setup, 1 - cmd list, 2 - read/write/listen cmd
enum {
    rwt_set_batch = 0, // fast send packet without question
    rwt_read_batch, // Send read cmd and wait for answer in batch
    rwt_read_cmd, // Send read cmd and wait for answer
    rwt_write_batch, // Send write cmd (with Write start pkt if available) with a question before it
    rwt_listen, // Listen mode (wait incoming pkts)
    rwt_max
};
uint8_t rw_type = rwt_read_batch; // What to do: rwt_*
enum { sst_none = 0, sst_sending, sst_receiving, sst_ok, sst_error, sst_timeout };
uint8_t send_status = sst_none; // sst_*
bool cmd_array = false;
uint8_t cmd_array_idx;
uint8_t cmd_array_cnt = 0;
bool cmd_array_hex;
uint8_t save_settings = 0;
uint16_t view_cmd[rwt_max - 1] = {0}; // SetBatch, ReadBatch, Read, WriteBatch
uint8_t view_x = 0;
char screen_buf[64];
char Info[35] = "";
char file_name[FONT_5x7_SCREEN_WIDTH];
char ERR_STR[FONT_5x7_SCREEN_WIDTH];
uint8_t ERR = 0;
uint8_t NRF_rate; // 0 - 250Kbps, 1 - 1Mbps, 2 - 2Mbps
uint8_t NRF_channel; // 0..125
uint8_t NRF_DPL; // 1 - Dynamic Payload Length
uint8_t NRF_CRC; // 1 - No, 1 - CRC 1byte, 2 - CRC 2byte
uint8_t NRF_RETR = ((0b0011 << 4) | 0b1111); // Automatic Retransmission, ARD, ARC
uint8_t NRF_Payload; // Payload len in bytes, 0..32
bool NRF_ERROR = 0;
uint8_t NRF_INITED = 0; // 0 - not, 1 - rw, rwt_listen - listen
bool NRF_BOARD_POWER_5V = false;
uint8_t NRF_last_packet_send_st = 0;
uint8_t NRF_resend = 1; // number of transaction attempts
int8_t NRF_repeat = 0; // count number of repeated requests (until < NRF_resend)
uint32_t NRF_time;
uint16_t ReadCmdRepeatPeriod = 10; // s
bool ReadRepeat = false;
uint32_t delay_between_pkt = 10; // ms

uint8_t addr[5]; // nRF24 address, MSB first
uint8_t addr_len = 0; // 2..5
uint8_t payload[32];
uint8_t payload_receive[32];
uint8_t payload_struct[32]; // sizeof(1..4) in bytes of each field, example: 2,1,1
uint8_t payload_fields = 0;
uint8_t payload_size = 0; // bytes
uint16_t view_Batch = 0; // view pos in Batch or inside WriteBatch (Log[view_Batch])
uint16_t view_WriteBatch = 0; // view pos of WriteBatch list
uint8_t setup_cursor = 0; // cursor position on Setup scr
uint8_t Edit = 0;
char* Edit_pos;
char* Edit_start;
bool Edit_hex; // 0 - decimal, 1 - hex
bool Edited = false; // list of cmds edited

Stream* file_stream = NULL;
FuriString* ReadDefault = NULL;
FuriString* WriteDefault = NULL;
FuriString* WriteStart = NULL;
FuriString* Constants = NULL; // text of STR=x
uint8_t listen_addr[5];
uint8_t listen_addr_len = 0;
char* ListenFields = NULL; // ptr to string: field1,field2,... max 5 field now
bool ListenNew;
FuriHalRtcDateTime ListenLastTime = {0};
uint32_t ListenPrev = 0;
uint32_t ListenLast = 0;
FuriString** Read_cmd = NULL; // Names of read cmd
uint16_t Read_cmd_Total = 0;
FuriString** Log = NULL; // Strings: var=n
uint16_t Log_Total = 0;
FuriString** ReadBatch_cmd = NULL; // Names of read batch cmd
uint16_t ReadBatch_cmd_Total = 0;
char* ReadBatch_cmd_curr = NULL; // =0xFFFFFFFF - finish
FuriString** WriteBatch_cmd = NULL; // Names of write batch cmd
uint16_t WriteBatch_cmd_Total = 0;
uint16_t WriteBatch_cmd_curr = 0; // == _Total - finish
FuriString** SetBatch_cmd = NULL; // Names of set batch cmd
uint16_t SetBatch_cmd_Total = 0;
uint16_t SetBatch_cmd_curr = 0; // == _Total - finish

#define POWER_READ_PERIOD 501 // ms
uint16_t pwr_read_timer = 0;
int Current = 0;
int CurrentStart = 0;

enum { ask_write_batch = 1, ask_save_batch, ask_skip_cmd, ask_return, ask_exit };
uint8_t ask_question = 0; // 1 - Ask now - ask_*
uint8_t ask_question_answer = 0; // 0 - no, 1 - yes

static bool ask_fill_screen_buf(void) {
    if(ask_question == ask_write_batch)
        strcpy(screen_buf, "RUN WRITE BATCH?");
    else if(ask_question == ask_save_batch)
        strcpy(screen_buf, "SAVE AS WRITE BATCH?");
    else if(ask_question == ask_skip_cmd)
        strcpy(screen_buf, "SKIP CMD?");
    else if(ask_question == ask_return)
        strcpy(screen_buf, "RETURN?");
    else if(ask_question == ask_exit)
        strcpy(screen_buf, "EXIT?");
    else
        return false;
    strcat(screen_buf, ask_question_answer ? " YES" : " NO");
    return true;
}

//#define MIN(a, b)  ((a<b)?a:b)

static uint8_t GetHexVal(char hex) {
    return hex == 0 ? 0 :
                      ((uint8_t)hex - ((uint8_t)hex < 58 ? 48 : ((uint8_t)hex < 97 ? 55 : 87)));
}

static bool is_digit(char* ptr, bool hex) {
    char c = *ptr;
    if(c >= '0' && c <= '9') return true;
    if(hex) {
        c &= ~0x20;
        if(c >= 'A' && c <= 'F') return true;
    } else if(c == '-')
        return true;
    return false;
}

// Return num bytes in array
static uint8_t ConvertHexToArray(char* hex, uint8_t* array, uint8_t maxlen) {
    uint8_t len = 0;
    while(maxlen) {
        uint8_t ch = *hex++;
        if(ch < ' ') break;
        if(ch < '0') continue;
        *array++ = (GetHexVal(ch) << 4) + GetHexVal(*hex++);
        len++;
        maxlen--;
    }
    return len;
}

int32_t str_to_int(char* p) {
    if(*(p + 1) == 'x') { // hex
        return strtol(p + 2, NULL, 16);
    } else
        return strtol(p, NULL, 10);
}

void str_rtrim(char* p) {
    char* delim_col = strchr(p, '\r');
    if(delim_col)
        *delim_col = '\0';
    else {
        delim_col = strchr(p, '\n');
        if(delim_col) *delim_col = '\0';
    }
}

static void add_to_str_hex_bytes(char* out, uint8_t* arr, int bytes) {
    if(bytes <= 0) return;
    out += strlen(out);
    do {
        snprintf(out, 3, "%02X", *arr++);
        out += 2;
    } while(--bytes);
}

// skip leading zeros
static void add_to_str_hex_variable(char* out, uint8_t* arr, int size) {
    if(size <= 0) return;
    out += strlen(out);
    arr += size - 1;
    while(*arr == 0 && size > 1) {
        arr--;
        size--;
    }
    do {
        snprintf(out, 3, "%02X", *arr--);
        out += 2;
    } while(--size);
}

void Edit_insert_digit(char new) {
    if(*Edit_pos == '-') return;
    if(what_doing <= 1) {
        if(strlen(Edit_start) < (what_doing == 0 && setup_cursor == 2 ? 3 : 5 * 2)) {
            memmove(Edit_pos + 1, Edit_pos, strlen(Edit_pos) + 1);
            *Edit_pos = new;
        }
    } else {
        FuriString* fs = Log[view_Batch];
        FuriString* ns = furi_string_alloc();
        if(ns) {
            uint16_t len = Edit_pos - (char*)furi_string_get_cstr(fs);
            furi_string_set_n(ns, fs, 0, len);
            furi_string_cat_printf(ns, "%c", new);
            furi_string_cat_str(ns, Edit_pos);
            Log[view_Batch] = ns;
            Edit_pos = (char*)furi_string_get_cstr(ns);
            Edit_start = Edit_pos + (Edit_start - (char*)furi_string_get_cstr(fs));
            Edit_pos += len;
            furi_string_free(fs);
        }
    }
}

int32_t get_payload_receive_field(uint8_t* var, uint8_t size) {
    if(size <= 1)
        return *var;
    else if(size == 2)
        return *(int16_t*)var;
    else if(size == 3)
        return (*(uint32_t*)var) & 0xFFFFFF;
    else
        return *(int32_t*)var;
}

void free_Log() {
    if(Log_Total) {
        for(uint16_t i = 0; i < Log_Total; i++)
            if(Log[i]) furi_string_free(Log[i]);
        Log_Total = 0;
    }
    if(Log) {
        free(Log);
        Log = NULL;
    }
}

void free_store(void) {
    if(Constants) {
        furi_string_free(Constants);
        Constants = NULL;
    }
    if(ReadDefault) {
        furi_string_free(ReadDefault);
        ReadDefault = NULL;
    }
    if(WriteDefault) {
        furi_string_free(WriteDefault);
        WriteDefault = NULL;
    }
    if(WriteStart) {
        furi_string_free(WriteStart);
        WriteDefault = NULL;
    }
    if(ListenFields) {
        free(ListenFields);
        ListenFields = NULL;
    }
    if(Read_cmd_Total) {
        for(uint16_t i = 0; i < Read_cmd_Total; i++) furi_string_free(Read_cmd[i]);
        Read_cmd_Total = 0;
    }
    if(Read_cmd) {
        free(Read_cmd);
        Read_cmd = NULL;
    }
    if(ReadBatch_cmd_Total) {
        for(uint16_t i = 0; i < ReadBatch_cmd_Total; i++) furi_string_free(ReadBatch_cmd[i]);
        ReadBatch_cmd_Total = 0;
    }
    if(ReadBatch_cmd) {
        free(ReadBatch_cmd);
        ReadBatch_cmd = NULL;
    }
    if(WriteBatch_cmd_Total) {
        for(uint16_t i = 0; i < WriteBatch_cmd_Total; i++) furi_string_free(WriteBatch_cmd[i]);
        WriteBatch_cmd_Total = 0;
    }
    if(WriteBatch_cmd) {
        free(WriteBatch_cmd);
        WriteBatch_cmd = NULL;
    }
    if(SetBatch_cmd_Total) {
        for(uint16_t i = 0; i < SetBatch_cmd_Total; i++) furi_string_free(SetBatch_cmd[i]);
        SetBatch_cmd_Total = 0;
    }
    if(SetBatch_cmd) {
        free(SetBatch_cmd);
        SetBatch_cmd = NULL;
    }
    free_Log();
}

void update_power(void) {
    Current = furi_hal_power_get_battery_current(FuriHalPowerICFuelGauge) * 1000;
}

void check_en_power_5V(void) {
    if(!furi_hal_power_is_otg_enabled() && !furi_hal_power_is_charging()) {
        FURI_LOG_D("PWR", "NO 5V, TURN ON");
        notification_message(APP->notification, &sequence_blink_yellow_100);
        furi_delay_ms(10);
        update_power();
        CurrentStart = Current;
        furi_hal_power_enable_otg();
        NRF_BOARD_POWER_5V = true;
        furi_delay_ms(100);
        NRF_INITED = 0;
    }
}

static bool select_settings_file() {
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
        if(!file_stream_open(
               file_stream, furi_string_get_cstr(path), FSAM_READ_WRITE, FSOM_OPEN_EXISTING)) {
            FURI_LOG_D(TAG, "Cannot open file \"%s\"", furi_string_get_cstr(path));
            file_stream_close(file_stream);
        } else {
            FURI_LOG_D(TAG, "Open file \"%s\"", furi_string_get_cstr(path));
            strncpy(
                file_name,
                furi_string_get_cstr(path) + sizeof(SCAN_APP_PATH_FOLDER),
                sizeof(file_name));
            result = true;
        }
    }
    furi_string_free(path);
    return result;
}

static void prepare_nrf24(void) {
    check_en_power_5V();
    if(!NRF_INITED || rw_type == rwt_listen || NRF_INITED == rwt_listen) {
        FURI_LOG_D("NRF", "Prepare");
        uint8_t adrlen, *adr;
        if(rw_type == rwt_listen) {
            adrlen = listen_addr_len;
            adr = listen_addr;
            NRF_INITED = rwt_listen;
        } else {
            adrlen = addr_len;
            adr = addr;
            NRF_INITED = 1;
        }
        furi_hal_gpio_write(nrf24_CE_PIN, false);
        nrf24_set_mac(REG_RX_ADDR_P0, adr, adrlen);
        uint8_t tmp[5] = {0};
        nrf24_read_reg(nrf24_HANDLE, REG_RX_ADDR_P0, tmp, adrlen);
        for(uint8_t i = 0; i < adrlen / 2; i++) {
            uint8_t tb = tmp[i];
            tmp[i] = tmp[adrlen - i - 1];
            tmp[adrlen - i - 1] = tb;
        }
        NRF_ERROR = memcmp(adr, tmp, adrlen) != 0;
        if(NRF_ERROR) {
            NRF_INITED = 0;
            return;
        }
        // EN_DYN_ACK(0x01) option for W_TX_PAYLOAD_NOACK cmd broke AA on some fake nRF24l01+, i.e. set it to 0
        nrf24_write_reg(
            nrf24_HANDLE,
            REG_FEATURE,
            NRF_EN_DYN_ACK +
                (NRF_DPL ? 4 :
                           0)); // Dynamic Payload, Payload with ACK, W_TX_PAYLOAD_NOACK command
        nrf24_write_reg(nrf24_HANDLE, REG_RF_CH, NRF_channel);
        nrf24_write_reg(
            nrf24_HANDLE,
            REG_RF_SETUP,
            (NRF_rate == 0 ? 0b00100000 :
             NRF_rate == 1 ? 0 :
                             0b00001000) |
                0b111); // +TX high power
        nrf24_write_reg(
            nrf24_HANDLE,
            REG_CONFIG,
            0x70 | ((NRF_CRC == 1 ? 0b1000 :
                     NRF_CRC == 2 ? 0b1100 :
                                    0))); // Mask all interrupts
        nrf24_write_reg(
            nrf24_HANDLE, REG_SETUP_RETR, NRF_RETR); // Automatic Retransmission, ARD<<4 + ARC
        nrf24_write_reg(nrf24_HANDLE, REG_EN_AA, 0x01); // Auto acknowledgement
        nrf24_write_reg(nrf24_HANDLE, REG_DYNPD, NRF_DPL ? 0x3F : 0); // Enable dynamic payload reg
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P0, payload_size);
        nrf24_set_maclen(nrf24_HANDLE, adrlen);
        nrf24_set_mac(REG_TX_ADDR, adr, adrlen);
        nrf24_write_reg(nrf24_HANDLE, REG_EN_RXADDR, 1);
        //nrf24_set_idle(nrf24_HANDLE);
    }
    nrf24_flush_tx(nrf24_HANDLE);
    nrf24_flush_rx(nrf24_HANDLE);
    nrf24_write_reg(nrf24_HANDLE, REG_STATUS, MAX_RT | RX_DR | TX_DS);
}

// true - ok
uint8_t nrf24_send_packet() {
    if(furi_log_get_level() == FuriLogLevelDebug) {
        char buf[65];
        buf[0] = 0;
        add_to_str_hex_bytes(buf, payload, payload_size);
        FURI_LOG_D(TAG, "SEND: %s", buf);
    }
    //nrf24_flush_tx(nrf24_HANDLE);
    //nrf24_write_reg(nrf24_HANDLE, REG_STATUS, RX_DR | TX_DS | MAX_RT);
    NRF_last_packet_send_st = nrf24_txpacket(nrf24_HANDLE, payload, payload_size, true); // ACK
    if(NRF_last_packet_send_st) {
        if((rw_type == rwt_read_cmd || rw_type == rwt_read_batch) &&
           send_status == sst_sending) { // Read
            nrf24_set_rx_mode(nrf24_HANDLE);
            send_status = sst_receiving; // receiving
        }
    } else
        notification_message(APP->notification, &sequence_blink_red_100);
    NRF_time = furi_get_tick();
    FURI_LOG_D(
        TAG,
        "Send packet: %d%s",
        NRF_last_packet_send_st,
        send_status == sst_receiving ? ", Receiving" : "");
    return NRF_last_packet_send_st;
}

uint8_t nrf24_resend_read_packet() {
    if(Log_Total && !cmd_array) {
        FuriString* str = Log[Log_Total - 1];
        char* p = strstr(furi_string_get_cstr(str), ": ");
        if(p) {
            if(strncmp(p + 2, "0x", 2) == 0) p += 2;
            furi_string_left(str, p - furi_string_get_cstr(str) + 2);
        }
    }
    return nrf24_send_packet();
}

// true - new packet
bool nrf24_read_newpacket() {
    bool found = false;
    uint8_t packetsize;
    uint8_t st =
        nrf24_rxpacket(nrf24_HANDLE, payload_receive, &packetsize, NRF_DPL ? 0 : payload_size);
    if(st & RX_DR) {
        NRF_time = furi_get_tick();
        if(furi_log_get_level() == FuriLogLevelDebug) {
            char buf[65];
            buf[0] = 0;
            add_to_str_hex_bytes(buf, payload_receive, packetsize);
            FURI_LOG_D(TAG, "READ(%X): %s", st, buf);
        }
        if(Log_Total) {
            FuriString* str = Log[Log_Total - 1];
            uint8_t size = 1;
            char* p = strchr((char*)furi_string_get_cstr(str), '*');
            if(p) {
                p++;
                if(*p == '=')
                    size = 0; // string
                else {
                    size = *p - '0';
                    if(size > 4) size = 0;
                }
            }
            int32_t var = get_payload_receive_field(payload_receive, size);
            //FURI_LOG_D(TAG, "VAR(%d): %ld", size, var);
            if(size == 0)
                furi_string_cat_printf(str, "%c", (char)var);
            else {
                char hex[9];
                hex[0] = '\0';
                add_to_str_hex_variable(hex, (uint8_t*)&var, size);
                if((cmd_array && cmd_array_hex) || furi_string_end_with_str(str, "0x"))
                    furi_string_cat_str(str, hex);
                else {
                    if(var >= 0 && var <= 9)
                        furi_string_cat_printf(str, "%ld", var);
                    else
                        furi_string_cat_printf(str, "%ld (%s)", var, hex);
                }
            }
            if(cmd_array) {
                if(--cmd_array_cnt) {
                    furi_string_cat_str(str, ",");
                    if(cmd_array_hex) furi_string_cat_str(str, "0x");
                    payload[cmd_array_idx] += size; // next array element
                    NRF_repeat = -1;
                    send_status = sst_sending; // Will be send after delay_between_pkt
                } else
                    send_status = sst_ok;
            } else {
                if(size == 0) { // string, until '\0'
                    if(var == 0) send_status = sst_ok;
                } else
                    send_status = sst_ok;
            }
        }
        //notification_message(APP->notification, &sequence_blink_white_100);
        found = true;
    } else if(st & 0x80) { // NRF24 hardware error
        NRF_ERROR = 1;
        NRF_INITED = 0;
    }
    return found;
}

// Search in constatnt pull (Const1=n; Const2=n;...)
// VAR_EMPTY - not found
int32_t subs_constant(char* p, uint8_t len) {
    char* c = (char*)furi_string_get_cstr(Constants);
    while((c = strchr(c, *p))) {
        if(strncmp(c, p, len) != 0) {
            c++;
            continue;
        }
        if(c == (char*)furi_string_get_cstr(Constants) || *(c - 1) == ';' || *(c - 1) <= ' ') {
            c += len;
            if(*c == '=') {
                c++;
                return str_to_int(c);
            }
        } else
            c += len;
    }
    return VAR_EMPTY;
}

// fill payload with default = p
// if var_n = VAR_EMPTY - skip filling var_*
bool fill_payload(char* p, uint8_t* idx_i, int32_t var_n) {
    if(idx_i) *idx_i = 255;
    uint8_t fld = 0; // field #
    uint8_t idx = 0; // byte index
    do {
        int32_t b = 0;
        char* end = strchr(p, ',');
        if(*p >= '0' && *p <= '9') { // Number found
            b = str_to_int(p);
        } else if(*p == 'i' && *(p + 1) == ':') { // 'i:' array index
            b = str_to_int(p + 2);
            if(idx_i) *idx_i = idx;
        } else if(*p == 'n' && *(p + 1) < '0') { // var_n
            if(var_n != VAR_EMPTY) b = var_n;
        } else if(*p >= 'A') { // constant found
            uint8_t len;
            if(end)
                len = end - p;
            else {
                len = strlen(p);
                if(*(p + len - 1) == '#') len--;
            }
            b = subs_constant(p, len);
            if(b == VAR_EMPTY) {
                ERR = 1;
                memset(ERR_STR, 0, sizeof(ERR_STR));
                strcpy(ERR_STR, "No ");
                strncpy(ERR_STR + strlen(ERR_STR), p, sizeof(ERR_STR) - 4);
                FURI_LOG_D(TAG, "Constant not found: %s", p);
                return false;
            }
        } else if(end == p) {
            idx += payload_struct[fld];
        } else if(*p == '#') { // value in Hexadecimal, end string
            break;
        } else {
            ERR = 2;
            strcpy(ERR_STR, "char: ");
            uint8_t l = strlen(ERR_STR);
            ERR_STR[l] = *p;
            ERR_STR[l + 1] = '\0';
            FURI_LOG_D(TAG, "Wrong format char(%c)", *p);
            return false;
        }
        if(end != p) {
            payload[idx++] = b;
            if(payload_struct[fld] > 1) payload[idx++] = b >> 8;
            if(payload_struct[fld] > 2) payload[idx++] = b >> 16;
            if(payload_struct[fld] > 3) payload[idx++] = b >> 24;
        }
        if(++fld == payload_fields || idx >= sizeof(payload) || end == NULL) break;
        p = end + 1;
    } while(1);
    return true;
}

// Cmd: "name=payload"
bool Run_Read_cmd(FuriString* cmd) {
    char* p = (char*)furi_string_get_cstr(cmd);
    p = strchr(p, '=');
    if(p == NULL) return false;
    if(Log == NULL)
        Log = malloc(sizeof(Log));
    else
        Log = realloc(Log, sizeof(Log) * (Log_Total + 1));
    if(Log == NULL) {
        ERR = 3;
        strcpy(ERR_STR, "Memory low");
        FURI_LOG_D(TAG, ERR_STR);
        return false;
    }
    FuriString* fs = furi_string_alloc();
    furi_string_set_strn(
        fs, (char*)furi_string_get_cstr(cmd), p - (char*)furi_string_get_cstr(cmd));
    furi_string_cat_str(fs, ": ");
    bool hexval;
    if((hexval = *(p + strlen(p) - 1) == '#'))
        furi_string_cat_str(fs, "0x"); // value in Hex format
    Log[Log_Total++] = fs;
    p++;
    memset(payload, 0, sizeof(payload));
    if(ReadDefault && !fill_payload((char*)furi_string_get_cstr(ReadDefault), NULL, VAR_EMPTY))
        return false;
    if(!fill_payload(p, &cmd_array_idx, VAR_EMPTY)) return false;
    memset(payload_receive, 0, sizeof(payload_receive));
    cmd_array = false;
    if(*(p - 2) == ']' && cmd_array_idx != 255) { // array
        p = strchr(furi_string_get_cstr(cmd), '[');
        if(p) {
            cmd_array_cnt = str_to_int(p + 1);
            if(cmd_array_cnt > 1) {
                cmd_array_hex = hexval;
                cmd_array = true; // array
            }
        }
    }
    prepare_nrf24();
    if(NRF_ERROR) return false;
    what_doing = 2;
    NRF_repeat = 0;
    send_status = sst_sending; // Read - sending
    nrf24_send_packet();
    return true;
}

// run commands one by one, true - command running
bool Run_ReadBatch_cmd(FuriString* cmd) {
    char* p;
    if(cmd) {
        p = strchr((char*)furi_string_get_cstr(cmd), ':');
        if(p == NULL) {
            ERR = 5;
            strcpy(ERR_STR, "WRONG FORMAT");
            return false;
        }
        p += 2;
        ReadBatch_cmd_curr = NULL;
        free_Log();
    } else {
        if(ReadBatch_cmd_curr)
            p = ReadBatch_cmd_curr;
        else
            return false;
    }
    char* end = strchr(p, ';');
    uint8_t len;
    if(end)
        len = end - p;
    else {
        str_rtrim(p);
        len = strlen(p);
    }
    for(uint16_t i = 0; i < Read_cmd_Total; i++) {
        FuriString* fs = Read_cmd[i];
        if(strncmp((char*)furi_string_get_cstr(fs), p, len) == 0) {
            char c = *((char*)furi_string_get_cstr(fs) + len);
            if(c != '=' && c != '*' && c != '[') continue;
            if(end)
                ReadBatch_cmd_curr = end + 1;
            else
                ReadBatch_cmd_curr = (char*)0xFFFFFFFF;
            if(!Run_Read_cmd(fs)) break;
            return true;
        }
    }
    if(NRF_ERROR) return false;
    if(ERR == 0) {
        ERR = 4;
        strcpy(ERR_STR, "NOT FOUND");
        FuriString* fs = furi_string_alloc();
        furi_string_set_strn(fs, p, len);
        Log[Log_Total++] = fs;
        FURI_LOG_D(TAG, "CMD %s: %s", ERR_STR, p);
    }
    view_Batch = Log_Total ? Log_Total - 1 : 0;
    return false;
}

void Prepare_Write_cmd(FuriString* cmd) {
    free_Log();
    if(cmd == NULL) return;
    char *end, *p = strchr((char*)furi_string_get_cstr(cmd), ':');
    if(p == NULL) {
        ERR = 8;
        strcpy(ERR_STR, "Wrong batch");
        FURI_LOG_D(TAG, ERR_STR);
        return;
    }
    p += 2;
    Log = malloc(sizeof(Log));
    do {
        end = strchr(p, ';');
        uint8_t len;
        if(end) {
            len = end - p;
            end++;
        } else {
            str_rtrim(p);
            len = strlen(p);
        }
        FuriString* fs = furi_string_alloc();
        if(Log_Total) Log = realloc(Log, sizeof(Log) * (Log_Total + 1));
        if(Log == NULL) {
            ERR = 3;
            strcpy(ERR_STR, "Memory low");
            FURI_LOG_D(TAG, ERR_STR);
            return;
        }
        furi_string_set_strn(fs, p, len);
        Log[Log_Total++] = fs;
    } while((p = end));
}

// Write / Set batch
bool Run_WriteBatch_cmd() {
    if(Log_Total == 0) return false;
    uint16_t cmd_curr = rw_type == rwt_write_batch ? WriteBatch_cmd_curr : SetBatch_cmd_curr;
    if(cmd_curr == 0) { // first
        prepare_nrf24();
        if(NRF_ERROR) return false;
        if(rw_type == rwt_write_batch && WriteStart) {
            if(!fill_payload((char*)furi_string_get_cstr(WriteStart), NULL, VAR_EMPTY))
                return false;
            send_status = sst_sending;
            if(!nrf24_send_packet()) return false;
        }
    }
    char* p = (char*)furi_string_get_cstr(Log[cmd_curr]);
    uint16_t len = furi_string_size(Log[cmd_curr]);
    char* arr = NULL;
    cmd_array = false;
    int32_t new = 0;
    for(uint16_t i = 0; i < len; i++) {
        if(p[i] == '=') {
            len = i;
            char* p2 = p + i + 1;
            if(*p2 == '{') {
                arr = ++p2; // array
                cmd_array = true;
            }
            new = str_to_int(p2);
            break;
        }
    }
    FURI_LOG_D(
        TAG, "%cBatch: =%d, (%d)%s", rw_type == rwt_write_batch ? 'W' : 'S', (int)new, len, p);
    char *w, *delim_col, i, size;
    FuriString* str = furi_string_alloc();
    stream_rewind(file_stream);
    while(stream_read_line(file_stream, str)) {
        w = (char*)furi_string_get_cstr(str);
        if(strncmp(w, SettingsFld_Write, sizeof(SettingsFld_Write) - 1) == 0)
            w += sizeof(SettingsFld_Write);
        else if(strncmp(w, SettingsFld_Set, sizeof(SettingsFld_Set) - 1) == 0)
            w += sizeof(SettingsFld_Set);
        else
            continue;
        delim_col = strchr(w, '=');
        if(delim_col == NULL) continue;
        size = 1;
        if(*(delim_col - 2) == '*') {
            if(len != delim_col - w - 2) continue;
            size = *(delim_col - 1) - '0';
            new = new&(size == 1 ? 0xFF : size == 2 ? 0xFFFF : size == 3 ? 0xFFFFFF : 0xFFFFFFFF);
        } else if(len != delim_col - w)
            continue;
        if(strncmp(p, w, len) != 0) continue;
        delim_col++;
        str_rtrim(delim_col);
        cmd_array_cnt = 255;
        do {
            memset(payload, 0, sizeof(payload));
            if(WriteDefault &&
               !fill_payload((char*)furi_string_get_cstr(WriteDefault), NULL, new)) {
                view_Batch = cmd_curr;
                return false;
            }
            if(!fill_payload(delim_col, &cmd_array_idx, VAR_EMPTY)) {
                view_Batch = cmd_curr;
                return false;
            }
            if(cmd_array && cmd_array_idx != 255) {
                if(cmd_array_cnt != 255) payload[cmd_array_idx] = cmd_array_cnt;
            } else
                cmd_array = false;
            send_status = sst_sending;
            NRF_repeat = 0;
            i = 0;
            do {
                if(nrf24_send_packet()) break;
                furi_delay_ms(delay_between_pkt);
            } while(i++ < NRF_resend);
            if(i < NRF_resend || i == 0) { // ok
                if(cmd_array) { // array
                    for(; arr != NULL;) {
                        if(*arr == ',') break;
                        if(*arr == '}' || *arr < ' ')
                            arr = NULL;
                        else
                            arr++;
                    }
                    if(arr == NULL) {
                        send_status = sst_ok;
                        break;
                    }
                    arr++;
                    new = str_to_int(arr);
                    cmd_array_cnt = payload[cmd_array_idx] + size;
                    //furi_delay_ms(delay_between_pkt); // do it fast
                    continue; // send next array element
                } else
                    send_status = sst_ok;
            }
            break;
        } while(1);
        if(send_status != sst_ok) {
            send_status = sst_error;
            view_Batch = cmd_curr;
            return false;
        }
        return true;
    }
    ERR = 7;
    strcpy(ERR_STR, "NOT FOUND!");
    send_status = sst_error;
    view_Batch = cmd_curr;
    return false;
}

// Return 0 - success, otherwise an error
static uint8_t load_settings_file() {
    uint8_t err = 0;
    FURI_LOG_D(TAG, "Loading settings file");
    FuriString* str = furi_string_alloc();
    free_store();
    Info[0] = '\0';
    NRF_INITED = false;
    while(stream_read_line(file_stream, str)) {
        char* p = (char*)furi_string_get_cstr(str);
        if(*p <= '!' || *p == ';') continue;
        //char* delim_eq = strchr(p, '=');
        char* delim_col = strchr(p, ':');
        if(delim_col == NULL) { // Constant found - no ':'
            if(Constants == NULL) {
                Constants = furi_string_alloc_set(str);
            } else
                furi_string_cat(Constants, str);
        } else {
            str_rtrim(p);
            if(strncmp(p, SettingsFld_Rate, sizeof(SettingsFld_Rate) - 1) == 0) {
                NRF_rate = str_to_int(p + sizeof(SettingsFld_Rate));
            } else if(strncmp(p, SettingsFld_Info, sizeof(SettingsFld_Info) - 1) == 0) {
                strncpy(Info, p + sizeof(SettingsFld_Info), sizeof(Info) - 1);
            } else if(strncmp(p, SettingsFld_Ch, sizeof(SettingsFld_Ch) - 1) == 0) {
                NRF_channel = str_to_int(p + sizeof(SettingsFld_Ch));
            } else if(strncmp(p, SettingsFld_Address, sizeof(SettingsFld_Address) - 1) == 0) {
                p += sizeof(SettingsFld_Address);
                addr_len = ConvertHexToArray(p, addr, 5);
            } else if(strncmp(p, SettingsFld_CRC, sizeof(SettingsFld_CRC) - 1) == 0) {
                NRF_CRC = str_to_int(p + sizeof(SettingsFld_CRC));
            } else if(strncmp(p, SettingsFld_DPL, sizeof(SettingsFld_DPL) - 1) == 0) {
                NRF_DPL = str_to_int(p + sizeof(SettingsFld_DPL));
            } else if(strncmp(p, SettingsFld_RETR, sizeof(SettingsFld_RETR) - 1) == 0) {
                NRF_RETR = str_to_int(p + sizeof(SettingsFld_RETR));
            } else if(strncmp(p, SettingsFld_Resend, sizeof(SettingsFld_Resend) - 1) == 0) {
                NRF_resend = str_to_int(p + sizeof(SettingsFld_Resend));
            } else if(strncmp(p, SettingsFld_Delay, sizeof(SettingsFld_Delay) - 1) == 0) {
                delay_between_pkt = str_to_int(p + sizeof(SettingsFld_Delay));
            } else if(
                strncmp(
                    p,
                    SettingsFld_ReadCmdRepeatPeriod,
                    sizeof(SettingsFld_ReadCmdRepeatPeriod) - 1) == 0) {
                ReadCmdRepeatPeriod = str_to_int(p + sizeof(SettingsFld_ReadCmdRepeatPeriod));
            } else if(strncmp(p, SettingsFld_Payload, sizeof(SettingsFld_Payload) - 1) == 0) {
                p += sizeof(SettingsFld_Payload);
                payload_fields = 0;
                payload_size = 0;
                do {
                    uint8_t b = str_to_int(p);
                    if(b < 1 || b > 4) {
                        FURI_LOG_D(TAG, "Wrong payload format (%d)", b);
                        err = 3;
                        break;
                    }
                    payload_struct[payload_fields++] = b;
                    payload_size += b;
                    if(payload_fields == sizeof(payload_struct) - 1) break;
                    if((p = strchr(p, ',')) == NULL) break;
                    p++;
                } while(1);
                FURI_LOG_D(
                    TAG,
                    "Payload fields %d: %d,%d,%d",
                    payload_fields,
                    payload_struct[0],
                    payload_struct[1],
                    payload_struct[2]);
            } else if(strncmp(p, SettingsFld_ReadDefault, sizeof(SettingsFld_ReadDefault) - 1) == 0) {
                ReadDefault = furi_string_alloc_set_str(p + sizeof(SettingsFld_ReadDefault));
            } else if(strncmp(p, SettingsFld_WriteStart, sizeof(SettingsFld_WriteStart) - 1) == 0) {
                WriteStart = furi_string_alloc_set_str(p + sizeof(SettingsFld_WriteStart));
            } else if(strncmp(p, SettingsFld_WriteDefault, sizeof(SettingsFld_WriteDefault) - 1) == 0) {
                WriteDefault = furi_string_alloc_set_str(p + sizeof(SettingsFld_WriteDefault));
            } else if(strncmp(p, SettingsFld_Read, sizeof(SettingsFld_Read) - 1) == 0) {
                p += sizeof(SettingsFld_Read);
                if(Read_cmd == NULL)
                    Read_cmd = malloc(sizeof(Read_cmd));
                else {
                    Read_cmd = realloc(Read_cmd, sizeof(Read_cmd) * (Read_cmd_Total + 1));
                }
                if(Read_cmd == NULL) {
                    FURI_LOG_D(TAG, "Memory low, err 4");
                    err = 4;
                    break;
                }
                Read_cmd[Read_cmd_Total++] = furi_string_alloc_set_str(p);
            } else if(strncmp(p, SettingsFld_ReadBatch, sizeof(SettingsFld_ReadBatch) - 1) == 0) {
                p += sizeof(SettingsFld_ReadBatch);
                if(ReadBatch_cmd == NULL)
                    ReadBatch_cmd = malloc(sizeof(ReadBatch_cmd));
                else {
                    ReadBatch_cmd =
                        realloc(ReadBatch_cmd, sizeof(ReadBatch_cmd) * (ReadBatch_cmd_Total + 1));
                }
                if(ReadBatch_cmd == NULL) {
                    FURI_LOG_D(TAG, "Memory low, err 5");
                    err = 5;
                    break;
                }
                ReadBatch_cmd[ReadBatch_cmd_Total++] = furi_string_alloc_set_str(p);
            } else if(strncmp(p, SettingsFld_WriteBatch, sizeof(SettingsFld_WriteBatch) - 1) == 0) {
                p += sizeof(SettingsFld_WriteBatch);
                if(WriteBatch_cmd == NULL)
                    WriteBatch_cmd = malloc(sizeof(WriteBatch_cmd));
                else {
                    WriteBatch_cmd = realloc(
                        WriteBatch_cmd, sizeof(WriteBatch_cmd) * (WriteBatch_cmd_Total + 1));
                }
                if(WriteBatch_cmd == NULL) {
                    FURI_LOG_D(TAG, "Memory low, err 6");
                    err = 6;
                    break;
                }
                WriteBatch_cmd[WriteBatch_cmd_Total++] = furi_string_alloc_set_str(p);
            } else if(strncmp(p, SettingsFld_SetBatch, sizeof(SettingsFld_SetBatch) - 1) == 0) {
                p += sizeof(SettingsFld_SetBatch);
                if(SetBatch_cmd == NULL)
                    SetBatch_cmd = malloc(sizeof(SetBatch_cmd));
                else {
                    SetBatch_cmd =
                        realloc(SetBatch_cmd, sizeof(SetBatch_cmd) * (SetBatch_cmd_Total + 1));
                }
                if(SetBatch_cmd == NULL) {
                    FURI_LOG_D(TAG, "Memory low, err 7");
                    err = 7;
                    break;
                }
                SetBatch_cmd[SetBatch_cmd_Total++] = furi_string_alloc_set_str(p);
            } else if(strncmp(p, SettingsFld_Listen, sizeof(SettingsFld_Listen) - 1) == 0) {
                p += sizeof(SettingsFld_Listen);
                char* p2 = strchr(p, '=');
                if(p2) {
                    listen_addr_len = ConvertHexToArray(p, listen_addr, (p2 - p) / 2);
                    p2++;
                    uint8_t len = strlen(p2);
                    ListenFields = malloc(len + 1);
                    if(ListenFields) memcpy(ListenFields, p2, len);
                }
            }
        }
    }
    furi_string_free(str);
    return err;
}

static void save_batch(void) {
    FURI_LOG_D(TAG, "Save Batch");
    char *p, *p2;
    stream_seek(file_stream, 0, StreamOffsetFromEnd);
    FuriHalRtcDateTime dt;
    furi_hal_rtc_get_datetime(&dt);
    stream_write_format(file_stream, "\n%s ", SettingsFld_WriteBatch);
    p = (char*)furi_string_get_cstr(ReadBatch_cmd[view_cmd[rwt_read_batch]]);
    p2 = strchr(p, ':');
    if(p2 == NULL) p2 = p + strlen(p);
    stream_write(file_stream, (uint8_t*)p, p2 - p);
    stream_write_format(
        file_stream,
        " %02d.%02d.%02d %02d.%02d: ",
        dt.day,
        dt.month,
        dt.year % 100,
        dt.hour,
        dt.minute);
    for(uint16_t i = 0; i < Log_Total; i++) {
        p = (char*)furi_string_get_cstr(Log[i]);
        p2 = strchr(p, ':');
        if(p2 && *(p2 - 1) != '*') { // skip string
            if(*(p2 - 1) == ']') { // array
                char* p3 = strchr(p, '[');
                if(p3) {
                    stream_write(file_stream, (uint8_t*)p, p3 - p - (*(p3 - 2) == '*' ? 2 : 0));
                    stream_write_cstring(file_stream, "={");
                    p = (p2 += 2);
                    do {
                        while(is_digit(p2, true) || *p2 == 'x') p2++;
                        stream_write(file_stream, (uint8_t*)p, p2 - p);
                        char c = *p2;
                        if(c == '\0') break;
                        if(c != ',') {
                            p2 = strchr(p2, ',');
                            if(p2 == NULL) break;
                        }
                        stream_write_char(file_stream, ',');
                        p = ++p2;
                    } while(1);
                    stream_write_char(file_stream, '}');
                }
            } else {
                stream_write(file_stream, (uint8_t*)p, p2 - p - (*(p2 - 2) == '*' ? 2 : 0));
                stream_write_char(file_stream, '=');
                p2 += 2;
                p = strchr(p2, ' ');
                if(p == NULL) p = p2 + strlen(p2);
                stream_write(file_stream, (uint8_t*)p2, p - p2);
            }
            if(i < Log_Total - 1) stream_write_char(file_stream, ';');
        }
    }
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

void display_remove_asterisk(char* fsp, uint8_t vx) {
    char* p2 = strchr(fsp, '*');
    if(p2) { // remove '*' or '*n'
        int pos = p2 - fsp;
        if((pos -= vx) < 0) pos = 0;
        char c = *(p2 + 1);
        if(*(screen_buf + pos))
            memmove(
                screen_buf + pos,
                screen_buf + pos + (c == ':' || c == '=' ? 1 : 2),
                FONT_5x7_SCREEN_WIDTH + 1 + 2 - pos);
    }
}

void render_display_list(
    Canvas* const canvas,
    FuriString*** fsa,
    char delim,
    uint16_t view_pos,
    uint16_t max_i) {
    uint16_t page = view_pos & ~7;
    char *p, *end;
    uint16_t y, len;
    for(uint8_t i = 0; i < 8 && page + i < max_i; i++) {
        y = 14 + i * 7;
        p = (char*)furi_string_get_cstr((*fsa)[page + i]);
        end = strchr(p, delim);
        if(end) {
            len = MIN(end - p, view_x);
            len = MIN(end - p - len, FONT_5x7_SCREEN_WIDTH);
            strncpy(screen_buf, p + view_x, len);
            screen_buf[len] = '\0';
            display_remove_asterisk(p, MIN(end - p, view_x));
            canvas_draw_str(canvas, 5, y, screen_buf);
        }
        if((view_pos & 7) == i) {
            canvas_draw_str(canvas, 0, y, ">");
            canvas_draw_str(canvas, -1, y, ">");
        }
    }
}

void display_edit_ttf_font(Canvas* const canvas, uint8_t start_x, uint8_t start_y) {
    screen_buf[0] = *Edit_pos;
    screen_buf[1] = '\0';
    int n = canvas_string_width(canvas, screen_buf);
    int len = Edit_pos - Edit_start;
    memcpy(screen_buf, Edit_start, len);
    screen_buf[len] = '\0';
    int x = start_x + canvas_string_width(canvas, screen_buf);
    int len2 = strlen(Edit_pos);
    memcpy(screen_buf + len, Edit_pos, len2);
    screen_buf[len + len2] = '\0';
    canvas_draw_str(canvas, start_x, start_y, screen_buf);
    start_y += 1;
    canvas_draw_line(
        canvas, x + (len ? 1 : 0), start_y, x + n + (*Edit_pos == '1' && len ? 1 : 0), start_y);
}

void display_add_status(void) {
    if(NRF_ERROR)
        strcat(screen_buf, "nRF24 ERROR!");
    else if(ERR)
        snprintf(screen_buf, sizeof(screen_buf), "ERROR %s", ERR_STR);
    else if(send_status == sst_error)
        strcat(screen_buf, "NO ACK!");
    else if(send_status == sst_timeout)
        strcat(screen_buf, "TIMEOUT!");
    else if(send_status == sst_none)
        ;
    else if(send_status == sst_sending)
        strcat(screen_buf, "sending");
    else if(send_status == sst_receiving)
        strcat(screen_buf, "receiving");
    else if(
        send_status == sst_ok &&
        (rw_type == rwt_read_cmd ||
         (rw_type == rwt_read_batch && (uint32_t)ReadBatch_cmd_curr == 0xFFFFFFFF) ||
         (rw_type == rwt_set_batch && SetBatch_cmd_curr == Log_Total) ||
         (rw_type == rwt_write_batch && WriteBatch_cmd_curr == Log_Total)))
        strcat(screen_buf, "OK");
    else
        strcat(screen_buf, "working");
}

static void render_callback(Canvas* const canvas, void* ctx) {
    if(ctx == NULL) return;
    const PluginState* plugin_state = ctx;
    if(furi_mutex_acquire(plugin_state->mutex, 5) != FuriStatusOk) return;

    //canvas_draw_frame(canvas, 0, 0, 128, 64); // border around the edge of the screen
    if(what_doing == 0) {
        canvas_set_font(canvas, FontSecondary); // 8x10 font, 6 lines
        snprintf(screen_buf, sizeof(screen_buf), "%s: %s", addr_len ? "File" : "Open", file_name);
        canvas_draw_str(canvas, 8, 10, screen_buf);
        if(addr_len) {
            if(Edit) {
                if(setup_cursor == 1)
                    display_edit_ttf_font(canvas, 45, 20);
                else if(setup_cursor == 2)
                    display_edit_ttf_font(canvas, 55, 30);
            }
            if(!Edit || setup_cursor != 1) {
                screen_buf[0] = '\0';
                add_to_str_hex_bytes(screen_buf, addr, addr_len);
                canvas_draw_str(canvas, 45, 20, screen_buf);
            }
            if(!Edit || setup_cursor != 2) {
                snprintf(screen_buf, sizeof(screen_buf), "%d", NRF_channel);
                canvas_draw_str(canvas, 55, 30, screen_buf);
            }
            canvas_draw_str(canvas, 8, 20, "Address:");
            snprintf(screen_buf, sizeof(screen_buf), "Rate: %d, Ch:", NRF_rate);
            canvas_draw_str(canvas, 8, 30, screen_buf);
            snprintf(
                screen_buf,
                sizeof(screen_buf),
                "RB: %d, R: %d, WB: %d",
                ReadBatch_cmd_Total,
                Read_cmd_Total,
                WriteBatch_cmd_Total);
            canvas_draw_str(canvas, 8, 40, screen_buf);
            canvas_draw_str(canvas, 0, 64, Info);
            canvas_draw_str(canvas, 0, 10 + setup_cursor * 10, ">");
        } else {
            snprintf(screen_buf, sizeof(screen_buf), "Ver. %s,  vad7", VERSION);
            canvas_draw_str(canvas, 10, 60, screen_buf);
            canvas_draw_str(canvas, 0, 10, ">");
        }
    } else if(what_doing == 1) {
        if(rw_type == rwt_listen) {
            canvas_set_font(canvas, FontSecondary); // 8x10 font, 6 lines
            canvas_draw_str(canvas, 0, 10, "Listen mode");
            canvas_draw_str(canvas, 0, 25, "Address:");
            if(Edit)
                display_edit_ttf_font(canvas, 40, 25);
            else if(listen_addr_len) {
                screen_buf[0] = '\0';
                add_to_str_hex_bytes(screen_buf, listen_addr, listen_addr_len);
                canvas_draw_str(canvas, 40, 25, screen_buf);
            }
            snprintf(
                screen_buf,
                sizeof(screen_buf),
                "I: %d +(%d) mA",
                Current - CurrentStart,
                CurrentStart);
            canvas_draw_str(canvas, 0, 60, screen_buf);
        } else {
            canvas_set_font(canvas, FontBatteryPercent); // 5x7 font, 9 lines, 25 cols
            if(rw_type == rwt_read_batch) {
                canvas_draw_str(canvas, 0, 7, "Read Batch:");
                render_display_list(
                    canvas, &ReadBatch_cmd, ':', view_cmd[rw_type], ReadBatch_cmd_Total);
            } else if(rw_type == rwt_read_cmd) {
                canvas_draw_str(canvas, 0, 7, "Read Command:");
                render_display_list(canvas, &Read_cmd, '=', view_cmd[rw_type], Read_cmd_Total);
            } else if(rw_type == rwt_write_batch) {
                if(!ask_fill_screen_buf()) strcpy(screen_buf, "Write Batch:");
                canvas_draw_str(canvas, 0, 7, screen_buf);
                render_display_list(
                    canvas, &WriteBatch_cmd, ':', view_cmd[rw_type], WriteBatch_cmd_Total);
            } else if(rw_type == rwt_set_batch) {
                strcpy(screen_buf, "Set: ");
                display_add_status();
                canvas_draw_str(canvas, 0, 7, screen_buf);
                render_display_list(
                    canvas, &SetBatch_cmd, ':', view_cmd[rw_type], SetBatch_cmd_Total);
            }
        }
    } else { // what_doing == 2
        if(rw_type == rwt_listen) {
            canvas_set_font(canvas, FontSecondary); // 8x10 font, 6 lines
            strcpy(screen_buf, "Listen: ");
            if(NRF_ERROR)
                strcat(screen_buf, "nRF24 ERROR!");
            else if(ListenNew) {
                snprintf(
                    screen_buf + strlen(screen_buf),
                    16,
                    "%02d:%02d:%02d",
                    ListenLastTime.hour,
                    ListenLastTime.minute,
                    ListenLastTime.second);
                if(ListenPrev)
                    snprintf(
                        screen_buf + strlen(screen_buf), 16, " (%lu)", ListenLast - ListenPrev);
            } else
                strcat(screen_buf, "receiving");
            snprintf(screen_buf + strlen(screen_buf), 16, " %dmA", Current - CurrentStart);
            canvas_draw_str(canvas, 0, 10, screen_buf);
            if(ListenFields) {
                char *p2, *p = ListenFields;
                uint8_t hex, len, *pld = payload_receive;
                for(uint8_t i = 0; i < 5 && *p; i++) {
                    hex = false;
                    p2 = strchr(p, ',');
                    if(p2 == NULL) p2 = p + strlen(p);
                    if(*(p2 - 1) == '#') hex = true;
                    memcpy(screen_buf, p, len = p2 - p);
                    strcpy(screen_buf + len, ": ");
                    if(ListenNew) {
                        len = payload_struct[i];
                        int32_t n = get_payload_receive_field(pld, len);
                        if(hex) {
                            strcat(screen_buf, "0x");
                            add_to_str_hex_variable(screen_buf, pld, len);
                        } else {
                            snprintf(screen_buf + strlen(screen_buf), 20, "%ld", n);
                            if(n > 9) {
                                strcat(screen_buf, " (");
                                add_to_str_hex_variable(screen_buf, pld, len);
                                strcat(screen_buf, ")");
                            }
                        }
                        pld += len;
                    }
                    canvas_draw_str(canvas, 0, 20 + i * 10, screen_buf);
                    if(*p2 == '\0') break;
                    p = p2 + 1;
                }
            }
        } else if(rw_type == rwt_read_cmd) { // Read command
            canvas_set_font(canvas, FontSecondary); // 8x10 font, 6 lines
            if(!ask_fill_screen_buf()) {
                strcpy(screen_buf, "Read ");
                strcat(screen_buf, ReadRepeat ? "rep: " : "cmd: ");
            }
            display_add_status();
            canvas_draw_str(canvas, 0, 10, screen_buf);
            if(Log_Total) {
                char* p = (char*)furi_string_get_cstr(Log[Log_Total - 1]);
                uint8_t vx = MIN(view_x, strlen(p));
                strncpy(screen_buf, p + vx, 30);
                display_remove_asterisk(p, vx);
                canvas_draw_str(canvas, 0, 15 + 10, screen_buf);
            }
        } else if(rw_type == rwt_set_batch) {
            canvas_set_font(canvas, FontBatteryPercent); // 5x7 font, 9 lines, 25 cols
            strcpy(screen_buf, "Set: ");
            display_add_status();
            canvas_draw_str(canvas, 0, 7, screen_buf);
            render_display_list(canvas, &SetBatch_cmd, ':', view_cmd[rw_type], SetBatch_cmd_Total);
        } else { // rwt_read_batch, rwt_write_batch
            canvas_set_font(canvas, FontBatteryPercent); // 5x7 font, 9 lines, 25 cols
            if(!ask_fill_screen_buf()) {
                strcpy(screen_buf, rw_type == rwt_read_batch ? "Read Batch: " : "Write: ");
                if(rw_type == rwt_read_batch || send_status != sst_none) {
                    display_add_status();
                } else if(rw_type == rwt_write_batch) {
                    char* p =
                        (char*)furi_string_get_cstr(WriteBatch_cmd[view_cmd[rwt_write_batch]]);
                    char* end = strchr(p, ':');
                    if(end) {
                        uint8_t len = end - p;
                        uint8_t lenb = strlen(screen_buf);
                        end = screen_buf + lenb;
                        lenb = FONT_5x7_SCREEN_WIDTH - lenb;
                        if(len > lenb) {
                            if(view_x < len) {
                                strncpy(end, p + view_x, len = MIN(lenb, len - view_x));
                                end[len] = '\0';
                            }
                        } else {
                            strncpy(end, p, len);
                            end[len] = '\0';
                        }
                    }
                }
            }
            canvas_draw_str(canvas, 0, 7, screen_buf);
            if(Log_Total) {
                char* p;
                uint16_t y, page = view_Batch & ~7;
                uint8_t vx;
                for(uint8_t i = 0; i < 8 && page + i < Log_Total; i++) {
                    p = (char*)furi_string_get_cstr(Log[page + i]);
                    y = 14 + i * 7;
                    vx = MIN(view_x, strlen(p));
                    if((view_Batch & 7) == i) {
                        canvas_draw_str(canvas, 0, y, ">");
                        canvas_draw_str(canvas, -1, y, ">");
                        if(Edit) {
                            int n = Edit_pos - p - vx - (FONT_5x7_SCREEN_WIDTH - 4);
                            if(n > 0) vx += n; // fix out of screen
                            int x = 6 + (Edit_pos - p - vx) * 5;
                            canvas_draw_line(canvas, x - 1, y, x - 1, y - 1);
                            canvas_draw_line(canvas, x - 1, y, n = x + 1 * 5, y);
                            canvas_draw_line(canvas, n, y, n, y - 1);
                        }
                    }
                    strncpy(screen_buf, p + vx, FONT_5x7_SCREEN_WIDTH + 2);
                    screen_buf[FONT_5x7_SCREEN_WIDTH + 2] = '\0';
                    display_remove_asterisk(p, vx);
                    canvas_draw_str(canvas, 6, y, screen_buf);
                }
            }
        }
    }
    furi_mutex_release(plugin_state->mutex);
}

void work_timer_callback(void* ctx) {
    if(ctx == NULL) return;
    if(rw_type == rwt_listen && (pwr_read_timer += WORK_PERIOD) >= POWER_READ_PERIOD) {
        pwr_read_timer = 0;
        update_power();
    }
    if(what_doing == 2) {
        const PluginState* plugin_state = ctx;
        if(furi_mutex_acquire(plugin_state->mutex, 0) != FuriStatusOk) return;
        if(rw_type == rwt_write_batch || rw_type == rwt_set_batch) {
            uint16_t* cmd_curr = rw_type == rwt_set_batch ? &SetBatch_cmd_curr :
                                                            &WriteBatch_cmd_curr;
            if(ERR == 0 && furi_get_tick() - NRF_time >= delay_between_pkt) {
                if(send_status == sst_ok) {
                    if(ERR == 0 && *cmd_curr < Log_Total) {
                        if(++(*cmd_curr) < Log_Total)
                            Run_WriteBatch_cmd();
                        else { // finished ok
                            if(rw_type == rwt_set_batch) what_doing = 1;
                            Edited = false;
                        }
                    }
                } else if(send_status == sst_sending) {
                    if(NRF_repeat++ < NRF_resend)
                        nrf24_send_packet();
                    else {
                        view_Batch = *cmd_curr;
                        send_status = sst_error; // error NO_ACK
                    }
                }
            }
            if((ERR || send_status == sst_error) && rw_type == rwt_set_batch) {
                what_doing = 1;
            }
            // ReadBatch or ReadCmd
        } else if(send_status == sst_sending) { // sending
            if(furi_get_tick() - NRF_time > delay_between_pkt) {
                if(NRF_repeat++ < NRF_resend) {
                    nrf24_resend_read_packet();
                } else
                    send_status = sst_error; // error NO_ACK
            }
        } else if(send_status == sst_receiving) { // receiving
            for(uint8_t i = 0; i < 3; i++) {
                if(nrf24_read_newpacket()) {
                    if(rw_type == rwt_listen) {
                        ListenPrev = ListenLast;
                        furi_hal_rtc_get_datetime(&ListenLastTime);
                        ListenLast = furi_hal_rtc_datetime_to_timestamp(&ListenLastTime);
                        ListenNew = true;
                    } else if(send_status != sst_receiving)
                        break;
                } else {
                    if(rw_type != rwt_listen && furi_get_tick() - NRF_time > NRF_READ_TIMEOUT) {
                        if(NRF_repeat++ < NRF_resend) {
                            send_status = sst_sending;
                            nrf24_resend_read_packet();
                        } else {
                            FURI_LOG_D(TAG, "TIMEOUT: %lu", furi_get_tick() - NRF_time);
                            send_status = sst_timeout;
                        }
                    }
                    break;
                }
            }
        } else if(send_status == sst_ok) {
            if(rw_type == rwt_read_batch) {
                if((uint32_t)ReadBatch_cmd_curr != 0xFFFFFFFF && ERR == 0 &&
                   furi_get_tick() - NRF_time >= delay_between_pkt) {
                    Run_ReadBatch_cmd(NULL);
                }
            }
        }
        furi_mutex_release(plugin_state->mutex);
    }
}

void next_rw_type(int8_t add) {
    do {
        rw_type += add;
        if(rw_type >= rwt_max) {
            if(add > 0)
                rw_type = 0;
            else
                rw_type = rwt_max - 1;
        }
        if(rw_type == rwt_set_batch && SetBatch_cmd_Total) break;
        if(rw_type == rwt_read_batch && ReadBatch_cmd_Total) break;
        if(rw_type == rwt_read_cmd && Read_cmd_Total) break;
        if(rw_type == rwt_write_batch && WriteBatch_cmd_Total) break;
    } while(rw_type != rwt_listen);
    send_status = sst_none;
}

void next_view_cmd(int8_t add) {
    if(rw_type == rwt_listen) return;
    uint16_t max =
        (rw_type == rwt_read_batch ? ReadBatch_cmd_Total :
         rw_type == rwt_read_cmd   ? Read_cmd_Total :
         rw_type == rwt_set_batch  ? SetBatch_cmd_Total :
                                     WriteBatch_cmd_Total);
    if((view_cmd[rw_type] += add) >= max) view_cmd[rw_type] = add > 0 ? 0 : max - 1;
}

int32_t nrf24batch_app(void* p) {
    UNUSED(p);
    APP = malloc(sizeof(nRF24Batch));
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    APP->plugin_state = malloc(sizeof(PluginState));
    APP->plugin_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!APP->plugin_state->mutex) {
        furi_message_queue_free(event_queue);
        FURI_LOG_E(TAG, "cannot create mutex");
        free(APP->plugin_state);
        return 255;
    }
    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, APP->plugin_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Open GUI and register view_port
    APP->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(APP->gui, view_port, GuiLayerFullscreen);
    APP->notification = furi_record_open(RECORD_NOTIFICATION);
    APP->storage = furi_record_open(RECORD_STORAGE);
    storage_common_migrate(APP->storage, EXT_PATH("nrf24batch"), SCAN_APP_PATH_FOLDER);
    storage_common_mkdir(APP->storage, SCAN_APP_PATH_FOLDER);
    file_stream = file_stream_alloc(APP->storage);
    FuriTimer* work_timer =
        furi_timer_alloc(work_timer_callback, FuriTimerTypePeriodic, APP->plugin_state);
    furi_timer_start(work_timer, WORK_PERIOD);
    nrf24_init();
    check_en_power_5V();

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        furi_mutex_acquire(APP->plugin_state->mutex, FuriWaitForever);

        // static FuriLogLevel FuriLogLevel = FuriLogLevelDefault;
        // if(furi_log_get_level() != FuriLogLevel) {
        //     FuriLogLevel = furi_log_get_level();
        //     if(FuriLogLevel == FuriLogLevelDebug)
        //         furi_hal_uart_set_br(FuriHalUartIdUSART1, 1843200);
        // }
        if(what_doing == 2 && rw_type == rwt_read_cmd && ReadRepeat &&
           furi_get_tick() - NRF_time > (uint32_t)(ReadCmdRepeatPeriod * 1000)) {
            ERR = 0;
            free_Log();
            Run_Read_cmd(Read_cmd[view_cmd[rwt_read_cmd]]);
            notification_message(APP->notification, &sequence_blink_blue_100);
        }

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                switch(event.input.key) {
                case InputKeyUp:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(!ask_question) {
                            if(Edit) {
                                char c = *Edit_pos;
                                if(Edit_hex) {
                                    if(c == '9')
                                        *Edit_pos = 'A';
                                    else if(c < 'F' || c < '9')
                                        *Edit_pos = c + 1;
                                } else {
                                    if(c == '-')
                                        *Edit_pos = '0';
                                    else if(c < '9')
                                        *Edit_pos = c + 1;
                                    else {
                                        c = *(Edit_pos - 1);
                                        if(Edit_pos > Edit_start && c < '9' && c >= '0') {
                                            *Edit_pos = '0';
                                            (*(Edit_pos - 1)) = c + 1;
                                        }
                                    }
                                }
                            } else if(what_doing == 0) {
                                if(addr_len) {
                                    if(setup_cursor > 0)
                                        setup_cursor--;
                                    else
                                        setup_cursor = 2;
                                }
                            } else if(what_doing == 1)
                                next_view_cmd(-1);
                            else if(what_doing == 2)
                                if(view_Batch) view_Batch--;
                        }
                    }
                    break;
                case InputKeyDown:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(!ask_question) {
                            if(Edit) {
                                if(*Edit_pos != '-') {
                                    if(Edit_hex && *Edit_pos == 'A')
                                        (*Edit_pos) = '9';
                                    else if(*Edit_pos > '0')
                                        (*Edit_pos)--;
                                    else if(!Edit_hex) {
                                        if(Edit_pos > Edit_start) {
                                            if(*(Edit_pos - 1) > '0' && *(Edit_pos - 1) <= '9') {
                                                *Edit_pos = '9';
                                                (*(Edit_pos - 1))--;
                                            }
                                        } else
                                            Edit_insert_digit('-'); // negative
                                    }
                                }
                            } else if(what_doing == 0) {
                                if(addr_len) {
                                    if(setup_cursor < 2)
                                        setup_cursor++;
                                    else
                                        setup_cursor = 0;
                                }
                            } else if(what_doing == 1)
                                next_view_cmd(+1);
                            else if(what_doing == 2)
                                if(view_Batch < Log_Total - 1) view_Batch++;
                        }
                    }
                    break;
                case InputKeyLeft:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(ask_question) {
                            if(event.input.type == InputTypeShort) ask_question_answer ^= 1;
                        } else if(Edit) {
                            if(Edit_pos > Edit_start) {
                                if(is_digit(Edit_pos - 1, Edit_hex))
                                    Edit_pos--;
                                else if(*(Edit_pos - 1) == 'x' && *(Edit_pos - 3) == ',')
                                    Edit_pos -= 4;
                                else if(*(Edit_pos - 1) == ',')
                                    Edit_pos -= 2;
                            }
                        } else if(what_doing == 0) {
                            if(addr_len) {
                                rw_type = rwt_listen;
                                what_doing = 1;
                            }
                        } else if(what_doing == 1) {
                            if(event.input.type == InputTypeShort)
                                next_rw_type(-1);
                            else if(view_x)
                                view_x--;
                        } else if(what_doing == 2)
                            if(view_x) view_x--;
                    } else if(event.input.type == InputTypeLong) {
                        if(!ask_question && view_x == 0 && what_doing == 2 &&
                           (rw_type == rwt_write_batch || rw_type == rwt_read_batch) &&
                           Log_Total && Log[view_Batch] != NULL) {
                            ask_question = ask_skip_cmd;
                            ask_question_answer = 1;
                        }
                    }
                    break;
                case InputKeyRight:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(ask_question) {
                            ask_question_answer ^= 1;
                        } else if(Edit) {
                            if(is_digit(Edit_pos + 1, Edit_hex))
                                Edit_pos++;
                            else if(*(Edit_pos + 1) == ',') {
                                Edit_pos += 2;
                                if(*(Edit_pos + 1) == 'x') Edit_pos += 2;
                            }
                        } else if(what_doing == 0) {
                            if(addr_len) {
                                rw_type = rwt_set_batch;
                                what_doing = 1;
                            }
                        } else if(what_doing == 1) {
                            if(event.input.type == InputTypeShort)
                                next_rw_type(+1);
                            else
                                view_x++;
                        } else if(what_doing == 2)
                            view_x++;
                    }
                    break;
                case InputKeyOk:
                    if(event.input.type == InputTypeShort) {
                        if(ask_question) {
                            if(ask_question_answer) {
                                if(ask_question == ask_save_batch) {
                                    save_batch();
                                } else if(ask_question == ask_write_batch) {
                                    if(WriteBatch_cmd_Total) {
                                        if(what_doing == 1) {
                                            Prepare_Write_cmd(
                                                WriteBatch_cmd[view_cmd[rwt_write_batch]]);
                                            send_status = sst_none;
                                            Edited = false;
                                            view_x = 0;
                                            view_Batch = 0;
                                        }
                                        if(Log_Total) {
                                            ERR = 0;
                                            WriteBatch_cmd_curr = 0;
                                            Run_WriteBatch_cmd();
                                            what_doing = 2;
                                        }
                                    }
                                } else if(ask_question == ask_skip_cmd) {
                                    if(rw_type == rwt_write_batch || rw_type == rwt_read_batch) {
                                        furi_string_free(Log[view_Batch]);
                                        if(view_Batch < Log_Total - 1)
                                            memmove(
                                                &Log[view_Batch],
                                                &Log[view_Batch + 1],
                                                sizeof(Log) * (Log_Total - view_Batch - 1));
                                        else
                                            view_Batch--;
                                        Log_Total--;
                                    }
                                } else if(ask_question == ask_exit) {
                                    processing = false;
                                } else if(ask_question == ask_return) {
                                    if(what_doing == 2) {
                                        ERR = 0;
                                        send_status = sst_none;
                                        Edited = false;
                                        what_doing--;
                                    }
                                }
                            }
                            ask_question = 0;
                        } else if(Edit) { // insert digit
                            Edit_insert_digit('0');
                        } else if(what_doing == 0) {
                            if(setup_cursor == 0) { // open file
                                file_stream_close(file_stream);
                                if(select_settings_file()) {
                                    uint8_t err = load_settings_file();
                                    if(err)
                                        snprintf(
                                            file_name, sizeof(file_name), "LOAD ERROR #%d", err);
                                }
                            } else if(setup_cursor == 1) { // change address
                                char* ebuf = (char*)payload;
                                ebuf[0] = '\0';
                                add_to_str_hex_bytes(ebuf, addr, addr_len);
                                Edit_hex = true;
                                Edit_pos = ebuf + strlen(ebuf) - 1;
                                Edit_start = ebuf;
                                Edit = 1;
                                NRF_INITED = 0;
                            } else if(setup_cursor == 2) { // change channel
                                char* ebuf = (char*)payload;
                                snprintf(ebuf, sizeof(payload), "%d", NRF_channel);
                                Edit_hex = false;
                                Edit_pos = ebuf + strlen(ebuf) - 1;
                                Edit_start = ebuf;
                                Edit = 1;
                                NRF_INITED = 0;
                            }
                        } else if(what_doing == 1) {
                            if(rw_type == rwt_set_batch) {
                                if(SetBatch_cmd_Total) {
                                    ERR = 0;
                                    send_status = sst_none;
                                    Prepare_Write_cmd(SetBatch_cmd[view_cmd[rwt_set_batch]]);
                                    if(!ERR) {
                                        SetBatch_cmd_curr = 0;
                                        Run_WriteBatch_cmd();
                                        what_doing = 2;
                                    }
                                    Edited = false;
                                }
                            } else if(rw_type == rwt_read_batch) {
                                if(ReadBatch_cmd_Total) {
                                    ERR = 0;
                                    Run_ReadBatch_cmd(ReadBatch_cmd[view_cmd[rwt_read_batch]]);
                                    view_x = 0;
                                    view_Batch = 0;
                                    what_doing = 2;
                                }
                            } else if(rw_type == rwt_read_cmd) {
                                if(Read_cmd_Total) {
                                    ERR = 0;
                                    free_Log();
                                    Run_Read_cmd(Read_cmd[view_cmd[rwt_read_cmd]]);
                                    view_x = 0;
                                    what_doing = 2;
                                }
                            } else if(rw_type == rwt_write_batch) {
                                if(WriteBatch_cmd_Total) {
                                    ERR = 0;
                                    Prepare_Write_cmd(WriteBatch_cmd[view_cmd[rwt_write_batch]]);
                                    send_status = sst_none;
                                    Edited = false;
                                    view_x = 0;
                                    view_Batch = 0;
                                    what_doing = 2;
                                }
                            } else if(rw_type == rwt_listen) {
                                if(listen_addr_len) {
                                    free_Log();
                                    prepare_nrf24();
                                    if(!NRF_ERROR) {
                                        nrf24_set_rx_mode(nrf24_HANDLE);
                                        ListenNew = false;
                                        send_status = sst_receiving; // receiving
                                    }
                                    what_doing = 2;
                                }
                            }
                        } else if(what_doing == 2) {
                            if(rw_type == rwt_read_cmd) {
                                ERR = 0;
                                free_Log();
                                Run_Read_cmd(Read_cmd[view_cmd[rwt_read_cmd]]);
                            } else if(Log_Total) {
                                if(rw_type == rwt_read_batch) {
                                    ask_question = ask_save_batch;
                                    ask_question_answer = 0;
                                } else if(rw_type == rwt_write_batch) {
                                    ask_question = ask_write_batch;
                                    ask_question_answer = 0;
                                }
                            }
                        }
                    } else if(event.input.type == InputTypeLong) {
                        if(Edit) { // delete
                            if(what_doing <= 1) {
                                if(strlen(Edit_start) > 1) {
                                    memmove(Edit_pos, Edit_pos + 1, strlen(Edit_pos));
                                    if(*Edit_pos == '\0') Edit_pos--;
                                }
                            } else {
                                FuriString* fs = Log[view_Batch];
                                if(is_digit(Edit_pos + 1, Edit_hex) ||
                                   (Edit_pos > Edit_start && is_digit(Edit_pos - 1, Edit_hex))) {
                                    memmove(Edit_pos, Edit_pos + 1, strlen(Edit_pos));
                                    if(*Edit_pos == '\0') Edit_pos--;
                                    furi_string_left(fs, furi_string_size(fs) - 1);
                                }
                            }
                        } else if(what_doing == 1) {
                            if(rw_type == rwt_listen) {
                                if(listen_addr_len) {
                                    char* ebuf = (char*)payload;
                                    ebuf[0] = '\0';
                                    add_to_str_hex_bytes(ebuf, listen_addr, listen_addr_len);
                                    Edit_hex = true;
                                    Edit_pos = ebuf + strlen(ebuf) - 1;
                                    Edit_start = ebuf;
                                    Edit = 1;
                                    NRF_INITED = 0;
                                }
                            } else if(rw_type == rwt_read_batch) {
                                if(ReadBatch_cmd_Total) {
                                    ERR = 0;
                                    Run_ReadBatch_cmd(ReadBatch_cmd[view_cmd[rwt_read_batch]]);
                                    view_x = 0;
                                    view_Batch = 0;
                                    what_doing = 2;
                                }
                            } else if(rw_type == rwt_write_batch) {
                                ask_question = ask_write_batch;
                                ask_question_answer = 0;
                            }
                        } else if(what_doing == 2) {
                            if(rw_type == rwt_read_cmd) {
                                ReadRepeat = !ReadRepeat;
                            } else if(Log_Total) {
                                if(rw_type == rwt_write_batch) {
                                    if(!Edit) {
                                        Edit = 0;
                                        Edit_hex = 0;
                                        char* s = (char*)furi_string_get_cstr(Log[view_Batch]);
                                        char* p = strchr(s, '=');
                                        if(p) {
                                            p++;
                                            if(*p == '{') p++; // array
                                            if(*(p + 1) == 'x') {
                                                p += 2;
                                                Edit_hex = 1; // hex
                                            }
                                            if(is_digit(p, Edit_hex)) {
                                                Edit_start = p;
                                                while(is_digit(p, Edit_hex)) p++;
                                                Edit_pos = p - 1;
                                                Edited = true;
                                                Edit = 1;
                                            }
                                        }
                                    }
                                } else if(rw_type == rwt_read_batch) {
                                    ask_question = ask_save_batch;
                                    ask_question_answer = 0;
                                }
                            }
                        }
                    }
                    break;
                case InputKeyBack:
                    if(event.input.type == InputTypeLong) {
                        if(what_doing == 2 && Edited) {
                            if(!ask_question) ask_question_answer = 1;
                            ask_question = ask_exit;
                        } else
                            processing = false;
                    } else if(event.input.type == InputTypeShort) {
                        if(ask_question)
                            ask_question = 0;
                        else if(Edit) {
                            if(what_doing == 0) {
                                if(setup_cursor == 1) {
                                    addr_len = ConvertHexToArray((char*)payload, addr, 5);
                                } else if(setup_cursor == 2) {
                                    NRF_channel = str_to_int((char*)payload);
                                    if(NRF_channel > MAX_CHANNEL) NRF_channel = MAX_CHANNEL;
                                }
                            } else if(what_doing == 1 && rw_type == rwt_listen) {
                                listen_addr_len =
                                    ConvertHexToArray((char*)payload, listen_addr, 5);
                            }
                            Edit = 0;
                        } else {
                            if(what_doing == 2 && Edited) {
                                ask_question = ask_return;
                                ask_question_answer = 1;
                            } else if(what_doing != 0) {
                                if(what_doing) what_doing--;
                                if(what_doing == 0) rw_type = rwt_read_batch;
                                if(what_doing <= 1) view_x = 0;
                                ERR = 0;
                                send_status = sst_none;
                            }
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }

        view_port_update(view_port);
        furi_mutex_release(APP->plugin_state->mutex);
    }
    nrf24_set_idle(nrf24_HANDLE);
    nrf24_deinit();
    if(NRF_BOARD_POWER_5V) furi_hal_power_disable_otg();

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(APP->gui, view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_STORAGE);
    if(file_stream) {
        file_stream_close(file_stream);
        stream_free(file_stream);
    }
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    free_store();
    furi_timer_stop(work_timer);
    furi_timer_free(work_timer);
    furi_mutex_free(APP->plugin_state->mutex);
    free(APP->plugin_state);
    free(APP);
    return 0;
}
