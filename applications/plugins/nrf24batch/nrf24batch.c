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
#define VERSION "1.1"

#define SCAN_APP_PATH_FOLDER "/ext/apps_data/nrf24batch"
#define LOG_FILENAME "log"
#define LOG_FILEEXT ".txt"
#define NRF_READ_TIMEOUT 300UL // ms
#define WORK_PERIOD 2 // ms, Timer period

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
const char SettingsFld_Read[] = "R:";
const char SettingsFld_Write[] = "W:";
const char SettingsFld_ReadBatch[] = "RBatch:";
const char SettingsFld_WriteBatch[] = "WBatch:";
const char AskQuestion_Save[] = "SAVE BATCH?";
#define Settings_i 'i'
#define Settings_n 'n'
#define VAR_EMPTY ((int32_t)0x80000000)

nRF24Batch* APP;
uint8_t what_doing = 0; // 0 - setup, 1 - cmd list, 2 - view send cmd
enum { rwt_read_batch = 0, rwt_read_cmd, rwt_write_batch };
uint8_t rw_type = rwt_read_batch; // What to do: rwt_*
enum { sst_none = 0, sst_sending, sst_receiving, sst_ok, sst_error, sst_timeout };
uint8_t send_status = sst_none; // sst_*
bool cmd_array = false;
uint8_t cmd_array_idx;
uint8_t cmd_array_cnt = 0;
uint8_t save_settings = 0;
uint16_t view_cmd[3] = {0, 0, 0}; // ReadBatch, Read, WriteBatch
uint8_t view_x = 0;
char Info[32] = "";
char screen_buf[64];
char file_name[32];
char ERR_STR[32];
uint8_t ERR = 0;
uint8_t NRF_rate; // 0 - 250Kbps, 1 - 1Mbps, 2 - 2Mbps
uint8_t NRF_channel; // 0..125
uint8_t NRF_DPL; // 1 - Dynamic Payload Length
uint8_t NRF_CRC; // 1 - No, 1 - CRC 1byte, 2 - CRC 2byte
uint8_t NRF_RETR = ((0b0011 << 4) | 0b1111); // Automatic Retransmission, ARD, ARC
uint8_t NRF_Payload; // Payload len in bytes, 0..32
bool NRF_ERROR = 0;
bool NRF_INITED = false;
uint8_t NRF_last_packet_send_st = 0;
uint8_t NRF_resend = 1; // number of transaction attempts
uint8_t NRF_repeat = 0; // count number of repeated requests (until < NRF_resend)
uint32_t NRF_time;

uint8_t addr[5]; // nRF24 address, MSB first
uint8_t addr_len; // 2..5
uint8_t payload[32];
uint8_t payload_receive[32];
uint8_t payload_struct[32]; // sizeof(1..4) in bytes of each field, example: 2,1,1
uint8_t payload_fields = 0;
uint8_t payload_size = 0; // bytes
uint16_t view_Batch = 0; // view pos
uint16_t view_WriteBatch = 0; // view pos
uint32_t delay_between_pkt = 10; // ms
uint8_t Edit = 0;
char* Edit_pos;
bool Edit_hex; // 0 - decimal, 1 - hex
bool Edited = false;

Stream* file_stream = NULL;
FuriString* ReadDefault = NULL;
FuriString* WriteDefault = NULL;
FuriString* WriteStart = NULL;
FuriString* Constants = NULL; // text of STR=x
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

enum { ask_write_batch = 1, ask_save_batch, ask_return, ask_exit };
uint8_t ask_question = 0; // 1 - Ask now - ask_*
uint8_t ask_question_answer = 0; // 0 - no, 1 - yes

static bool ask_fill_screen_buf(void) {
    if(ask_question == ask_write_batch)
        strcpy(screen_buf, "RUN WRITE BATCH?");
    else if(ask_question == ask_save_batch)
        strcpy(screen_buf, "SAVE AS WRITE BATCH?");
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
    return (uint8_t)hex - ((uint8_t)hex < 58 ? 48 : ((uint8_t)hex < 97 ? 55 : 87));
}

static bool is_digit(char* ptr, bool hex) {
    char c = *ptr;
    if(c >= '0' && c <= '9') return true;
    if(hex) {
        c &= ~0x20;
        if(c >= 'A' && c <= 'F') return true;
    }
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

void free_Log() {
    if(Log_Total) {
        for(uint16_t i = 0; i < Log_Total; i++) furi_string_free(Log[i]);
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
    free_Log();
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
               file_stream, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
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
    if(!NRF_INITED) {
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
        nrf24_write_reg(
            nrf24_HANDLE,
            REG_FEATURE,
            NRF24_EN_DYN_ACK |
                (NRF_DPL ?
                     4 :
                     0)); // Enables the W_TX_PAYLOAD_NOACK command, Disable Payload with ACK, set Dynamic Payload
        nrf24_write_reg(nrf24_HANDLE, REG_DYNPD, NRF_DPL ? 0x3F : 0); // Enable dynamic payload reg
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P0, payload_size);
        nrf24_set_maclen(nrf24_HANDLE, addr_len);
        nrf24_set_mac(REG_RX_ADDR_P0, addr, addr_len);
        uint8_t tmp[5] = {0};
        nrf24_read_reg(nrf24_HANDLE, REG_RX_ADDR_P0, tmp, addr_len);
        for(uint8_t i = 0; i < addr_len / 2; i++) {
            uint8_t tb = tmp[i];
            tmp[i] = tmp[addr_len - i - 1];
            tmp[addr_len - i - 1] = tb;
        }
        NRF_ERROR = memcmp(addr, tmp, addr_len) != 0;
        nrf24_set_mac(REG_TX_ADDR, addr, addr_len);
        nrf24_write_reg(nrf24_HANDLE, REG_EN_RXADDR, 1);
        //nrf24_set_idle(nrf24_HANDLE);
        NRF_INITED = true;
    }
    nrf24_flush_tx(nrf24_HANDLE);
    nrf24_flush_rx(nrf24_HANDLE);
    nrf24_write_reg(nrf24_HANDLE, REG_STATUS, MAX_RT | RX_DR | TX_DS);
    furi_hal_gpio_write(nrf24_CE_PIN, false);
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
    if(Log_Total) {
        FuriString* str = Log[Log_Total - 1];
        char* p = strstr(furi_string_get_cstr(str), ": ");
        if(p) furi_string_left(str, p - furi_string_get_cstr(str) + 2);
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
            int32_t var;
            if(size <= 1)
                var = *payload_receive;
            else if(size == 2)
                var = *(int16_t*)payload_receive;
            else if(size == 3)
                var = (*(uint32_t*)payload_receive) & 0xFFFFFF;
            else
                var = *(int32_t*)payload_receive;
            //FURI_LOG_D(TAG, "VAR(%d): %ld", size, var);
            if(size == 0)
                furi_string_cat_printf(str, "%c", (char)var);
            else {
                if(var >= 0 && var <= 9)
                    furi_string_cat_printf(str, "%ld", var);
                else {
                    char hex[9];
                    snprintf(hex, sizeof(hex), "%lX", var);
                    furi_string_cat_printf(
                        str, "%ld (%s)", var, hex + (var < 0 ? 8 - size * 2 : 0));
                }
            }
            if(cmd_array) {
                if(--cmd_array_cnt) {
                    furi_string_cat_str(str, ",");
                    payload[cmd_array_idx]++;
                    NRF_repeat = 0;
                    send_status = sst_sending;
                    furi_delay_ms(delay_between_pkt);
                    nrf24_send_packet();
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
            b = subs_constant(p, end ? (uint8_t)(end - p) : strlen(p));
            if(b == VAR_EMPTY) {
                ERR = 1;
                strcpy(ERR_STR, "No ");
                strcat(ERR_STR, p);
                FURI_LOG_D(TAG, "Constant not found: %s", p);
                return false;
            }
        } else if(end == p) {
            idx += payload_struct[fld];
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
        fs,
        (char*)furi_string_get_cstr(cmd),
        (*(p - 2) == '*' ? p - 2 : p) - (char*)furi_string_get_cstr(cmd)); // skip *n
    furi_string_cat_str(fs, ": ");
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
            if(cmd_array_cnt > 1) cmd_array = true; // array
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
        if(p == NULL) return false;
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
            Run_Read_cmd(fs);
            return true;
        }
    }
    ERR = 4;
    strcpy(ERR_STR, "Not found");
    FURI_LOG_D(TAG, "CMD %s: %s", ERR_STR, p == NULL ? "" : p);
    return false;
}

void Prepare_Write_cmd(FuriString* cmd) {
    if(cmd == NULL) return;
    char *end, *p = strchr((char*)furi_string_get_cstr(cmd), ':');
    if(p == NULL) return;
    p += 2;
    free_Log();
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

bool Run_WriteBatch_cmd() {
    if(Log_Total == 0) return false;
    if(WriteBatch_cmd_curr == 0) { // first
        prepare_nrf24();
        if(NRF_ERROR) return false;
        if(WriteStart) {
            if(!fill_payload((char*)furi_string_get_cstr(WriteStart), NULL, VAR_EMPTY))
                return false;
            send_status = sst_sending;
            if(!nrf24_send_packet()) return false;
        }
    }
    char* p = (char*)furi_string_get_cstr(Log[WriteBatch_cmd_curr]);
    uint16_t len = furi_string_size(Log[WriteBatch_cmd_curr]);
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
    FURI_LOG_D(TAG, "WriteBatch: =%d, (%d)%s", (int)new, len, p);
    char *w, *delim_col, i;
    FuriString* str = furi_string_alloc();
    stream_rewind(file_stream);
    while(stream_read_line(file_stream, str)) {
        w = (char*)furi_string_get_cstr(str);
        if(strncmp(w, SettingsFld_Write, sizeof(SettingsFld_Write) - 1) != 0) continue;
        w += sizeof(SettingsFld_Write);
        delim_col = strchr(w, '=');
        if(delim_col == NULL || len != delim_col - w) continue;
        if(strncmp(p, w, len) != 0) continue;
        delim_col++;
        str_rtrim(delim_col);
        cmd_array_cnt = 255;
        do {
            memset(payload, 0, sizeof(payload));
            if(WriteDefault &&
               !fill_payload((char*)furi_string_get_cstr(WriteDefault), NULL, new)) {
                view_Batch = WriteBatch_cmd_curr;
                return false;
            }
            if(!fill_payload(delim_col, &cmd_array_idx, VAR_EMPTY)) {
                view_Batch = WriteBatch_cmd_curr;
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
                    cmd_array_cnt = payload[cmd_array_idx] + 1;
                    continue;
                } else
                    send_status = sst_ok;
            }
            break;
        } while(1);
        if(send_status != sst_ok) {
            send_status = sst_error;
            view_Batch = WriteBatch_cmd_curr;
            return false;
        }
        return true;
    }
    ERR = 7;
    strcpy(ERR_STR, "NOT FOUND!");
    send_status = sst_error;
    view_Batch = WriteBatch_cmd_curr;
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
        if(*p <= ' ') continue;
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
            }
        }
    }
    furi_string_free(str);
    return err;
}

static void save_batch(void) {
    // to do...
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
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
            if(*(end - 1) == '*')
                end--; // skip *
            else if(*(end - 2) == '*')
                end -= 2; // skip *?
            len = MIN((end - p), 30);
            strncpy(screen_buf, p, len);
            screen_buf[len] = '\0';
            canvas_draw_str(canvas, 5, y, screen_buf);
        }
        if((view_pos & 7) == i) {
            canvas_draw_str(canvas, 0, y, ">");
            canvas_draw_str(canvas, -1, y, ">");
        }
    }
}

#define FONT_5x7_SCREEN_WIDTH 25
static void render_callback(Canvas* const canvas, void* ctx) {
    const PluginState* plugin_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(plugin_state == NULL) return;
    //canvas_draw_frame(canvas, 0, 0, 128, 64); // border around the edge of the screen
    if(what_doing == 0) {
        canvas_set_font(canvas, FontSecondary); // 8x10 font, 6 lines
        snprintf(screen_buf, sizeof(screen_buf), "Open: %s", file_name);
        canvas_draw_str(canvas, 10, 10, screen_buf);
        if(addr_len) {
            canvas_draw_str(canvas, 10, 22, Info);
            strcpy(screen_buf, "Address: ");
            add_to_str_hex_bytes(screen_buf, addr, addr_len);
            canvas_draw_str(canvas, 10, 32, screen_buf);
            snprintf(screen_buf, sizeof(screen_buf), "Ch: %d, Rate: %d", NRF_channel, NRF_rate);
            canvas_draw_str(canvas, 10, 42, screen_buf);
            snprintf(
                screen_buf,
                sizeof(screen_buf),
                "RB: %d, R: %d, WB: %d",
                ReadBatch_cmd_Total,
                Read_cmd_Total,
                WriteBatch_cmd_Total);
            canvas_draw_str(canvas, 10, 52, screen_buf);
            //canvas_draw_str(canvas, 10, 60, screen_buf);
        } else {
            snprintf(screen_buf, sizeof(screen_buf), "Ver. %s,  vad7", VERSION);
            canvas_draw_str(canvas, 10, 60, screen_buf);
        }
        canvas_draw_str(canvas, 0, 10, ">");
    } else if(what_doing == 1) {
        canvas_set_font(canvas, FontBatteryPercent); // 5x7 font, 9 lines, 25 cols
        if(rw_type == rwt_read_batch) {
            canvas_draw_str(canvas, 0, 7, "Read Batch:");
            render_display_list(
                canvas, &ReadBatch_cmd, ':', view_cmd[rw_type], ReadBatch_cmd_Total);
        } else if(rw_type == rwt_read_cmd) {
            canvas_draw_str(canvas, 0, 7, "Read Command:");
            render_display_list(canvas, &Read_cmd, '=', view_cmd[rw_type], Read_cmd_Total);
        } else { // rwt_write_batch
            if(!ask_fill_screen_buf()) strcpy(screen_buf, "Write Batch:");
            canvas_draw_str(canvas, 0, 7, screen_buf);
            render_display_list(
                canvas, &WriteBatch_cmd, ':', view_cmd[rw_type], WriteBatch_cmd_Total);
        }
        if(NRF_ERROR) canvas_draw_str(canvas, 70, 7, "nRF24 ERROR!");
    } else { // what_doing == 2
        if(rw_type == rwt_read_cmd) { // Read command
            canvas_set_font(canvas, FontSecondary); // 8x10 font, 6 lines
            if(!ask_fill_screen_buf()) strcpy(screen_buf, "Read cmd:");
            if(NRF_ERROR)
                strcat(screen_buf, "nRF24 ERROR!");
            else if(ERR) {
                snprintf(screen_buf + strlen(screen_buf), 16, " Error %d", ERR);
                canvas_draw_str(canvas, 0, 60, ERR_STR);
            } else if(send_status == sst_sending)
                strcat(screen_buf, " sending");
            else if(send_status == sst_receiving)
                strcat(screen_buf, " receiving");
            else if(send_status == sst_error)
                strcat(screen_buf, " NO ACK!");
            else if(send_status == sst_timeout)
                strcat(screen_buf, " TIMEOUT!");
            else if(send_status == sst_ok)
                strcat(screen_buf, " Ok");
            canvas_draw_str(canvas, 0, 10, screen_buf);
            if(Log_Total) {
                char* p = (char*)furi_string_get_cstr(Log[Log_Total - 1]);
                strncpy(screen_buf, p + MIN(view_x, strlen(p)), 30);
                canvas_draw_str(canvas, 0, 15 + 10, screen_buf);
            }

        } else { // if(rw_type == rwt_read_batch || rw_type == rwt_write_batch)
            canvas_set_font(canvas, FontBatteryPercent); // 5x7 font, 9 lines, 25 cols
            if(!ask_fill_screen_buf()) {
                strcpy(
                    screen_buf,
                    rw_type == rwt_read_batch ? "Read Batch: " :
                    what_doing == 1           ? "Write Batch: " :
                                                "Write: ");
                if(rw_type == rwt_read_batch || send_status != sst_none) {
                    if(NRF_ERROR)
                        strcat(screen_buf, "nRF24 ERROR!");
                    else if(ERR)
                        snprintf(screen_buf + strlen(screen_buf), 16, "Error %d", ERR);
                    else if(send_status == sst_error)
                        strcat(screen_buf, "NO ACK!");
                    else if(send_status == sst_timeout)
                        strcat(screen_buf, "TIMEOUT!");
                    else if(
                        send_status == sst_ok &&
                        ((rw_type == rwt_read_batch &&
                          (uint32_t)ReadBatch_cmd_curr == 0xFFFFFFFF) ||
                         (rw_type == rwt_write_batch && WriteBatch_cmd_curr == Log_Total)))
                        strcat(screen_buf, "Ok");
                    else
                        strcat(screen_buf, "working");
                } else if(rw_type == rwt_write_batch) {
                    char* p =
                        (char*)furi_string_get_cstr(WriteBatch_cmd[view_cmd[rwt_write_batch]]);
                    char* end = strchr(p, ':');
                    if(end) {
                        uint8_t len = MIN(end - p, 25);
                        end = screen_buf + strlen(screen_buf);
                        memcpy(end, p, len);
                        end[len] = '\0';
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
                            int n = Edit_pos - p - vx - (FONT_5x7_SCREEN_WIDTH - 3);
                            if(n > 0) vx += n; // fix out of screen
                            int x = 6 + (Edit_pos - p - vx) * 5;
                            canvas_draw_line(canvas, x - 1, y, x - 1, y - 1);
                            canvas_draw_line(canvas, x - 1, y, n = x + 1 * 5, y);
                            canvas_draw_line(canvas, n, y, n, y - 1);
                        }
                    }
                    strncpy(screen_buf, p + vx, FONT_5x7_SCREEN_WIDTH);
                    screen_buf[FONT_5x7_SCREEN_WIDTH] = '\0';
                    //if(ERR && page + i == Log_Total - 1) strcat(screen_buf, ERR_STR);
                    canvas_draw_str(canvas, 6, y, screen_buf);
                }
            }
        }
    }
    release_mutex((ValueMutex*)ctx, plugin_state);
}

void work_timer_callback(void* ctx) {
    UNUSED(ctx);
    if(what_doing == 2) {
        if(rw_type == rwt_write_batch) {
            if(send_status == sst_ok) {
                if(ERR == 0 && WriteBatch_cmd_curr < Log_Total &&
                   furi_get_tick() - NRF_time >= delay_between_pkt) {
                    if(++WriteBatch_cmd_curr < Log_Total) Run_WriteBatch_cmd();
                }
            } else if(send_status == sst_sending) {
                if(NRF_repeat++ < NRF_resend)
                    nrf24_send_packet();
                else {
                    view_Batch = WriteBatch_cmd_curr;
                    send_status = sst_error; // error NO_ACK
                }
            }
        } else if(send_status == sst_sending) { // sending
            if(!NRF_last_packet_send_st) { // No ACK on last attempt
                if(furi_get_tick() - NRF_time > delay_between_pkt) {
                    if(NRF_repeat++ < NRF_resend)
                        nrf24_resend_read_packet();
                    else
                        send_status = sst_error; // error NO_ACK
                }
            }
        } else if(send_status == sst_receiving) { // receiving
            for(uint8_t i = 0; i < 3; i++) {
                bool new = nrf24_read_newpacket();
                if(new) {
                    if(send_status != sst_receiving) break;
                } else if(furi_get_tick() - NRF_time > NRF_READ_TIMEOUT) {
                    if(NRF_repeat++ < NRF_resend) {
                        send_status = sst_sending;
                        nrf24_resend_read_packet();
                    } else {
                        FURI_LOG_D(TAG, "TIMEOUT: %lu", furi_get_tick() - NRF_time);
                        send_status = sst_timeout;
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
    }
}

int32_t nrf24batch_app(void* p) {
    UNUSED(p);
    APP = malloc(sizeof(nRF24Batch));
    APP->event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    APP->plugin_state = malloc(sizeof(PluginState));
    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, APP->plugin_state, sizeof(PluginState))) {
        furi_message_queue_free(APP->event_queue);
        FURI_LOG_E(TAG, "cannot create mutex");
        free(APP->plugin_state);
        return 255;
    }
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
    file_stream = file_stream_alloc(APP->storage);
    FuriTimer* work_timer = furi_timer_alloc(work_timer_callback, FuriTimerTypePeriodic, NULL);
    furi_timer_start(work_timer, WORK_PERIOD);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(APP->event_queue, &event, 200);
        PluginState* plugin_state = (PluginState*)acquire_mutex_block(&state_mutex);

        static FuriLogLevel FuriLogLevel = FuriLogLevelDefault;
        if(furi_log_get_level() != FuriLogLevel) {
            FuriLogLevel = furi_log_get_level();
            if(FuriLogLevel == FuriLogLevelDebug)
                furi_hal_uart_set_br(FuriHalUartIdUSART1, 1843200);
        }

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                switch(event.input.key) {
                case InputKeyUp:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(!ask_question) {
                            if(what_doing == 0) {
                            } else if(what_doing == 1) {
                                if(view_cmd[rw_type]) view_cmd[rw_type]--;
                            } else if(what_doing == 2) {
                                if(Edit) {
                                    if(*Edit_pos < '9')
                                        (*Edit_pos)++;
                                    else if(Edit_hex) {
                                        if(*Edit_pos == '9')
                                            *Edit_pos = 'A';
                                        else if((*Edit_pos & ~0x20) < 'F')
                                            (*Edit_pos)++;
                                    }
                                } else if(view_Batch)
                                    view_Batch--;
                            }
                        }
                    }
                    break;
                case InputKeyDown:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(!ask_question) {
                            if(what_doing == 0) {
                                what_doing = 1;
                            } else if(what_doing == 1) {
                                if(view_cmd[rw_type] + 1 <
                                   (rw_type == rwt_read_batch ? ReadBatch_cmd_Total :
                                    rw_type == rwt_read_cmd   ? Read_cmd_Total :
                                                                WriteBatch_cmd_Total))
                                    view_cmd[rw_type]++;
                            } else if(what_doing == 2) {
                                if(Edit) {
                                    if(Edit_hex && (*Edit_pos & ~0x20) == 'A')
                                        (*Edit_pos) = '9';
                                    else if(*Edit_pos > '0')
                                        (*Edit_pos)--;
                                } else if(view_Batch < Log_Total - 1)
                                    view_Batch++;
                            }
                        }
                    }
                    break;
                case InputKeyLeft:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(ask_question) {
                            ask_question_answer ^= 1;
                        } else if(what_doing == 0) {
                        } else if(what_doing == 1) {
                            if(--rw_type > rwt_write_batch) rw_type = rwt_write_batch;
                        } else if(what_doing == 2) {
                            if(Edit) {
                                if(is_digit(Edit_pos - 1, Edit_hex))
                                    Edit_pos--;
                                else if(*(Edit_pos - 1) == ',' && is_digit(Edit_pos - 2, true)) {
                                    Edit_pos -= 2;
                                    // char *p = Edit_pos;
                                    // while(is_digit(p, true)) p--;
                                    // Edit_hex = *p == 'x';
                                }
                            } else if(view_x)
                                view_x--;
                        }
                    }
                    break;
                case InputKeyRight:
                    if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                        if(ask_question) {
                            ask_question_answer ^= 1;
                        } else if(what_doing == 0) {
                            what_doing = 1;
                        } else if(what_doing == 1) {
                            if(++rw_type > rwt_write_batch) rw_type = rwt_read_batch;
                        } else if(what_doing == 2) {
                            if(Edit) {
                                if(is_digit(Edit_pos + 1, Edit_hex))
                                    Edit_pos++;
                                else if(*(Edit_pos + 1) == ',' && is_digit(Edit_pos + 2, Edit_hex)) {
                                    if((/*Edit_hex =*/*(Edit_pos + 3) == 'x'))
                                        Edit_pos += 3;
                                    else
                                        Edit_pos += 2;
                                }
                            } else
                                view_x++;
                        }
                    }
                    break;
                case InputKeyOk:
                    if(event.input.type == InputTypeShort) {
                        if(ask_question) {
                            if(ask_question_answer) {
                                if(ask_question == ask_save_batch) {
                                    save_batch();
                                } else if(ask_question == ask_write_batch) {
                                    ERR = 0;
                                    WriteBatch_cmd_curr = 0;
                                    Run_WriteBatch_cmd();
                                    what_doing = 2;
                                } else if(ask_question == ask_exit) {
                                    processing = false;
                                } else if(ask_question == ask_return) {
                                    if(what_doing == 2) {
                                        ERR = 0;
                                        send_status = sst_none;
                                        what_doing--;
                                    }
                                }
                            }
                            ask_question = 0;
                        } else if(what_doing == 0) {
                            file_stream_close(file_stream);
                            if(select_settings_file()) {
                                uint8_t err = load_settings_file();
                                if(err)
                                    snprintf(file_name, sizeof(file_name), "LOAD ERROR #%d", err);
                            }
                        } else if(what_doing == 1) {
                            if(rw_type == rwt_read_batch) {
                                if(ReadBatch_cmd_Total) {
                                    ERR = 0;
                                    Run_ReadBatch_cmd(ReadBatch_cmd[view_cmd[rwt_read_batch]]);
                                    view_Batch = 0;
                                    what_doing = 2;
                                }
                            } else if(rw_type == rwt_read_cmd) {
                                if(Read_cmd_Total) {
                                    ERR = 0;
                                    free_Log();
                                    Run_Read_cmd(Read_cmd[view_cmd[rwt_read_cmd]]);
                                    what_doing = 2;
                                }
                            } else if(rw_type == rwt_write_batch) {
                                if(WriteBatch_cmd_Total) {
                                    Prepare_Write_cmd(WriteBatch_cmd[view_cmd[rwt_write_batch]]);
                                    Edited = false;
                                    send_status = sst_none;
                                    view_Batch = 0;
                                    what_doing = 2;
                                }
                            }
                        } else if(what_doing == 2) {
                            if(Log_Total) {
                                if(rw_type == rwt_read_batch) {
                                    ask_question = ask_save_batch;
                                    ask_question_answer = 0;
                                } else if(rw_type == rwt_write_batch) {
                                    if(Edit) { // insert digit
                                        FuriString* fs = Log[view_Batch];
                                        FuriString* ns = furi_string_alloc();
                                        if(ns) {
                                            uint16_t len =
                                                Edit_pos - (char*)furi_string_get_cstr(fs);
                                            furi_string_set_n(ns, fs, 0, len);
                                            furi_string_cat_str(ns, "0");
                                            furi_string_cat_str(ns, Edit_pos);
                                            furi_string_free(fs);
                                            Log[view_Batch] = ns;
                                            Edit_pos = (char*)furi_string_get_cstr(ns) + len;
                                        }
                                    } else {
                                        Edit = 1;
                                        Edited = true;
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
                                            Edit_pos = p;
                                        } else
                                            Edit = 0;
                                    }
                                }
                            }
                        }
                    } else if(event.input.type == InputTypeLong) {
                        if(rw_type == rwt_write_batch && what_doing == 2 && WriteBatch_cmd_Total) {
                            if(Edit) { // delete
                                FuriString* fs = Log[view_Batch];
                                if(is_digit(Edit_pos + 1, Edit_hex) ||
                                   is_digit(Edit_pos - 1, Edit_hex)) {
                                    memmove(Edit_pos, Edit_pos + 1, strlen(Edit_pos));
                                    furi_string_left(fs, furi_string_size(fs) - 1);
                                }
                            } else {
                                ask_question = ask_write_batch;
                                ask_question_answer = 0;
                            }
                        }
                    }
                    break;
                case InputKeyBack:
                    if(event.input.type == InputTypeLong) {
                        if(what_doing <= 1)
                            processing = false;
                        else if(what_doing == 2 && (Edited || rw_type == rwt_read_batch)) {
                            if(!ask_question) ask_question_answer = 1;
                            ask_question = ask_exit;
                        }
                    } else if(event.input.type == InputTypeShort) {
                        if(Edit)
                            Edit = 0;
                        else if(ask_question)
                            ask_question = 0;
                        else {
                            if(what_doing == 2 && Edited) {
                                ask_question = ask_return;
                                ask_question_answer = 1;
                            } else if(what_doing == 0) {
                                processing = false;
                            } else {
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

        view_port_update(APP->view_port);
        release_mutex(&state_mutex, plugin_state);
    }
    nrf24_set_idle(nrf24_HANDLE);
    nrf24_deinit();

    view_port_enabled_set(APP->view_port, false);
    gui_remove_view_port(APP->gui, APP->view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_STORAGE);
    if(file_stream) {
        file_stream_close(file_stream);
        stream_free(file_stream);
    }
    view_port_free(APP->view_port);
    furi_message_queue_free(APP->event_queue);
    free_store();
    furi_timer_stop(work_timer);
    furi_timer_free(work_timer);
    free(APP->plugin_state);
    free(APP);
    return 0;
}
