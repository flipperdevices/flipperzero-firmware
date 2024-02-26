#include "eth_save_process.h"

#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <locale/locale.h>

#define TAG "EthSave"

#define STORAGE_FILE_BUF_LEN 50

// fuction spizzhena from archive_favorites.c
static bool storage_read_line(File* file, FuriString* str_result) {
    furi_string_reset(str_result);
    uint8_t buffer[STORAGE_FILE_BUF_LEN];
    bool result = false;

    do {
        uint16_t read_count = storage_file_read(file, buffer, STORAGE_FILE_BUF_LEN);
        if(storage_file_get_error(file) != FSE_OK) {
            return false;
        }

        for(uint16_t i = 0; i < read_count; i++) {
            if(buffer[i] == '\n') {
                uint32_t position = storage_file_tell(file);
                if(storage_file_get_error(file) != FSE_OK) {
                    return false;
                }

                position = position - read_count + i + 1;

                storage_file_seek(file, position, true);
                if(storage_file_get_error(file) != FSE_OK) {
                    return false;
                }

                result = true;
                break;
            } else {
                furi_string_push_back(str_result, buffer[i]);
            }
        }

        if(result || read_count == 0) {
            break;
        }
    } while(true);

    return result;
}

static bool storage_printf(File* file, const char* format, ...) {
    va_list args;
    va_start(args, format);
    FuriString* fstring = furi_string_alloc_vprintf(format, args);
    va_end(args);
    if(storage_file_write(file, furi_string_get_cstr(fstring), furi_string_size(fstring)) &&
       storage_file_write(file, "\n", 1)) {
        furi_string_free(fstring);
        return true;
    }
    furi_string_free(fstring);
    return false;
}

static bool storage_write_config(File* file, const EthernetSaveConfig* cfg) {
    storage_file_seek(file, 0, true);
    storage_file_truncate(file);

    bool result = true;
    result = result ? storage_printf(
                          file,
                          "mac: %02X-%02X-%02X-%02X-%02X-%02X",
                          cfg->mac[0],
                          cfg->mac[1],
                          cfg->mac[2],
                          cfg->mac[3],
                          cfg->mac[4],
                          cfg->mac[5]) :
                      result;
    result = result ?
                 storage_printf(
                     file, "ip: %d.%d.%d.%d", cfg->ip[0], cfg->ip[1], cfg->ip[2], cfg->ip[3]) :
                 result;
    result =
        result ?
            storage_printf(
                file, "mask: %d.%d.%d.%d", cfg->mask[0], cfg->mask[1], cfg->mask[2], cfg->mask[3]) :
            result;
    result = result ? storage_printf(
                          file,
                          "gateway: %d.%d.%d.%d",
                          cfg->gateway[0],
                          cfg->gateway[1],
                          cfg->gateway[2],
                          cfg->gateway[3]) :
                      result;
    result =
        result ?
            storage_printf(
                file, "dns: %d.%d.%d.%d", cfg->dns[0], cfg->dns[1], cfg->dns[2], cfg->dns[3]) :
            result;
    result = result ? storage_printf(
                          file,
                          "ping_ip: %d.%d.%d.%d",
                          cfg->ping_ip[0],
                          cfg->ping_ip[1],
                          cfg->ping_ip[2],
                          cfg->ping_ip[3]) :
                      result;
    return result;
}

static void read_02X(const char* str, uint8_t* byte) {
    uint8_t b[2] = {str[0], str[1]};

    for(int i = 0; i < 2; ++i) {
        if('0' <= b[i] && b[i] <= '9') {
            b[i] -= '0';
        } else if('A' <= b[i] && b[i] <= 'F') {
            b[i] -= 'A' - 0x0A;
        } else {
            b[i] = 0;
        }
    }

    *byte = b[1] + (b[0] << 4);
}

static void read_ip(const char* str, uint8_t* ip) {
    uint8_t ip_i = 0;
    uint8_t i = 0;
    uint16_t tmp = 0;
    for(;;) {
        if('0' <= str[i] && str[i] <= '9') {
            tmp = tmp * 10 + str[i] - '0';
        } else if(str[i] == '.' || str[i] != '\n' || str[i] != '\0') {
            if(tmp <= 0xFF && ip_i < 4) {
                ip[ip_i] = tmp;
                ip_i += 1;
                tmp = 0;
            } else {
                break;
            }
            if(str[i] != '\n' && ip_i == 4) {
                return;
            }
        } else {
            break;
        }
        i += 1;
    }
    FURI_LOG_E(TAG, "cannot parse as ip string [%s]", str);
    ip[0] = ip[1] = ip[2] = ip[3] = 0;
    return;
}

static void set_default_config(EthernetSaveConfig* cfg) {
    const uint8_t def_mac[6] = ETHERNET_SAVE_DEFAULT_MAC;
    const uint8_t def_ip[4] = ETHERNET_SAVE_DEFAULT_IP;
    const uint8_t def_mask[4] = ETHERNET_SAVE_DEFAULT_MASK;
    const uint8_t def_gateway[4] = ETHERNET_SAVE_DEFAULT_GATEWAY;
    const uint8_t def_dns[4] = ETHERNET_SAVE_DEFAULT_DNS;
    const uint8_t def_ping_ip[4] = ETHERNET_SAVE_DEFAULT_PING_IP;

    memcpy(cfg->mac, def_mac, 6);
    memcpy(cfg->ip, def_ip, 4);
    memcpy(cfg->mask, def_mask, 4);
    memcpy(cfg->gateway, def_gateway, 4);
    memcpy(cfg->dns, def_dns, 4);
    memcpy(cfg->ping_ip, def_ping_ip, 4);
}

bool storage_read_config(File* file, EthernetSaveConfig* cfg) {
    FuriString* fstring = furi_string_alloc();

    while(storage_read_line(file, fstring)) {
        const char* str = furi_string_get_cstr(fstring);
        if(!strncmp(str, "mac: ", 5)) {
            read_02X(str + strlen("mac: "), &cfg->mac[0]);
            read_02X(str + strlen("mac: XX-"), &cfg->mac[1]);
            read_02X(str + strlen("mac: XX-XX-"), &cfg->mac[2]);
            read_02X(str + strlen("mac: XX-XX-XX-"), &cfg->mac[3]);
            read_02X(str + strlen("mac: XX-XX-XX-XX-"), &cfg->mac[4]);
            read_02X(str + strlen("mac: XX-XX-XX-XX-XX-"), &cfg->mac[5]);
        } else if(!strncmp(str, "ip: ", 4)) {
            read_ip(str + strlen("ip: "), cfg->ip);
        } else if(!strncmp(str, "mask: ", 6)) {
            read_ip(str + strlen("mask: "), cfg->mask);
        } else if(!strncmp(str, "gateway: ", 9)) {
            read_ip(str + strlen("gateway: "), cfg->gateway);
        } else if(!strncmp(str, "dns: ", 5)) {
            read_ip(str + strlen("dns: "), cfg->dns);
        } else if(!strncmp(str, "ping_ip: ", 9)) {
            read_ip(str + strlen("ping_ip: "), cfg->ping_ip);
        }
    }

    furi_string_free(fstring);
    return true;
}

void ethernet_save_process_write(const EthernetSaveConfig* config) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    File* file = storage_file_alloc(storage);

    if(!storage_file_open(file, APP_DATA_PATH("config.txt"), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(TAG, "Failed to open file");
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return;
    }

    if(!storage_write_config(file, config)) {
        FURI_LOG_E(TAG, "Failed to write cpnfig to file");
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return;
    }
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

void ethernet_save_process_read(EthernetSaveConfig* config) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    File* file = storage_file_alloc(storage);

    if(!storage_file_open(file, APP_DATA_PATH("config.txt"), FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Failed to open file or file not exists");
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return;
    }
    if(!storage_read_config(file, config)) {
        FURI_LOG_E(TAG, "Failed to read config from file");
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return;
    }
    storage_file_close(file);

    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);
}

EthernetSaveConfig* ethernet_save_process_malloc() {
    EthernetSaveConfig* config = malloc(sizeof(EthernetSaveConfig));

    set_default_config(config);

    ethernet_save_process_read(config);

    Storage* storage = furi_record_open(RECORD_STORAGE);

    FURI_LOG_E(TAG, "ethernet_save_process_malloc");

    File* file = storage_file_alloc(storage);

    if(!storage_file_open(file, APP_DATA_PATH("log.txt"), FSAM_WRITE, FSOM_OPEN_APPEND)) {
        FURI_LOG_E(TAG, "Failed to open file or file not exists");
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return NULL;
    }

    config->log_file = file;

    return config;
}

void ethernet_save_process_print(EthernetSaveConfig* config, const char* str) {
    furi_assert(config);
    DateTime datetime = {0};
    furi_hal_rtc_get_datetime(&datetime);
    storage_printf(
        config->log_file,
        "%4d.%02d.%02d-%02d:%2d:%02d || %s",
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second,
        str);
}

void ethernet_save_process_free(EthernetSaveConfig* config) {
    FURI_LOG_E(TAG, "ethernet_save_process_free");
    ethernet_save_process_write(config);
    storage_file_close(config->log_file);
    storage_file_free(config->log_file);
    furi_record_close(RECORD_STORAGE);
    free(config);
}