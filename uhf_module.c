#include "uhf_module.h"
#include "uhf_module_cmd.h"

#define DELAY_MS 50

void rx_callback(UartIrqEvent event, uint8_t data, void* ctx) {
    UNUSED(event);
    Buffer* buf = ctx;
    buffer_append_single(buf, data);
    if(data == FRAME_END) buffer_close(buf);
}

M100ModuleInfo* m100_module_info_alloc() {
    M100ModuleInfo* module_info = (M100ModuleInfo*)malloc(sizeof(M100ModuleInfo));
    module_info->hw_version = NULL;
    module_info->sw_version = NULL;
    module_info->manufacturer = NULL;
    return module_info;
}

void m100_module_info_free(M100ModuleInfo* module_info) {
    free(module_info->hw_version);
    free(module_info->sw_version);
    free(module_info->manufacturer);
    free(module_info);
}
M100Module* m100_module_alloc() {
    M100Module* module = (M100Module*)malloc(sizeof(M100Module));
    module->info = m100_module_info_alloc();
    module->buf = buffer_alloc(128);
    furi_hal_uart_set_br(FuriHalUartIdUSART1, DEFAULT_BAUDRATE);
    module->baudrate = (uint16_t)(DEFAULT_BAUDRATE);
    return module;
}

void m100_module_free(M100Module* module) {
    m100_module_info_free(module->info);
    buffer_free(module->buf);
    free(module);
}

uint8_t checksum(const uint8_t* data, size_t length) {
    // CheckSum8 Modulo 256
    // Sum of Bytes % 256
    uint64_t sum_val = 0x00;
    for(size_t i = 0; i < length; i++) {
        sum_val += data[i];
    }
    return (uint8_t)(sum_val % 0x100);
}

uint16_t crc16_genibus(const uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF; // Initial value
    uint16_t polynomial = 0x1021; // CRC-16/GENIBUS polynomial

    for(size_t i = 0; i < length; i++) {
        crc ^= (data[i] << 8); // Move byte into MSB of 16bit CRC
        for(int j = 0; j < 8; j++) {
            if(crc & 0x8000) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc ^ 0xFFFF; // Post-inversion
}

char* m100_get_hardware_version(M100Module* module) {
    if(module->info->hw_version != NULL) return module->info->hw_version;
    buffer_reset(module->buf);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, rx_callback, module->buf);
    furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)&CMD_HW_VERSION.cmd[0], CMD_HW_VERSION.length);
    furi_delay_ms(DELAY_MS);
    if(!buffer_get_size(module->buf)) return NULL;
    uint8_t* data = buffer_get_data(module->buf);
    uint16_t payload_len = data[3];
    payload_len = (payload_len << 8) + data[4];
    FuriString* temp_str = furi_string_alloc();
    for(int i = 0; i < payload_len; i++) {
        furi_string_cat_printf(temp_str, "%c", data[6 + i]);
    }
    char* hw_version = (char*)malloc(sizeof(char) * payload_len);
    memcpy(hw_version, furi_string_get_cstr(temp_str), payload_len);
    module->info->hw_version = hw_version;
    furi_string_free(temp_str);
    return module->info->hw_version;
}
char* m100_get_software_version(M100Module* module) {
    if(module->info->sw_version != NULL) return module->info->sw_version;
    buffer_reset(module->buf);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, rx_callback, module->buf);
    furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)&CMD_SW_VERSION.cmd[0], CMD_SW_VERSION.length);
    furi_delay_ms(DELAY_MS);
    if(!buffer_get_size(module->buf)) return NULL;
    uint8_t* data = buffer_get_data(module->buf);
    uint16_t payload_len = data[3];
    payload_len = (payload_len << 8) + data[4];
    FuriString* temp_str = furi_string_alloc();
    for(int i = 0; i < payload_len; i++) {
        furi_string_cat_printf(temp_str, "%c", data[6 + i]);
    }
    char* sw_version = (char*)malloc(sizeof(char) * payload_len);
    memcpy(sw_version, furi_string_get_cstr(temp_str), payload_len);
    module->info->sw_version = sw_version;
    furi_string_free(temp_str);
    return module->info->sw_version;
}
char* m100_get_manufacturers(M100Module* module) {
    if(module->info->manufacturer != NULL) return module->info->manufacturer;
    buffer_reset(module->buf);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, rx_callback, module->buf);
    furi_hal_uart_tx(
        FuriHalUartIdUSART1, (uint8_t*)&CMD_MANUFACTURERS.cmd[0], CMD_MANUFACTURERS.length);
    furi_delay_ms(DELAY_MS);
    if(!buffer_get_size(module->buf)) return NULL;
    uint8_t* data = buffer_get_data(module->buf);
    uint16_t payload_len = data[3];
    payload_len = (payload_len << 8) + data[4];
    FuriString* temp_str = furi_string_alloc();
    for(int i = 0; i < payload_len; i++) {
        furi_string_cat_printf(temp_str, "%c", data[6 + i]);
    }
    char* manufacturer = (char*)malloc(sizeof(char) * payload_len);
    memcpy(manufacturer, furi_string_get_cstr(temp_str), payload_len);
    module->info->manufacturer = manufacturer;
    furi_string_free(temp_str);
    return module->info->manufacturer;
}

UHFTag* m100_send_single_poll(M100Module* module) {
    buffer_reset(module->buf);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, rx_callback, module->buf);
    furi_hal_uart_tx(
        FuriHalUartIdUSART1, (uint8_t*)&CMD_SINGLE_POLLING.cmd[0], CMD_SINGLE_POLLING.length);
    furi_delay_ms(DELAY_MS);
    uint8_t* data = buffer_get_data(module->buf);
    size_t length = buffer_get_size(module->buf);
    if(length <= 8 && data[2] == 0xFF) return NULL;
    uint16_t pc = data[6];
    uint16_t crc = 0;
    // mask out epc length from protocol control
    size_t epc_len = pc;
    epc_len >>= 3;
    epc_len *= 2;
    // get protocol control
    pc <<= 8;
    pc += data[7];
    // get cyclic redundency check
    crc = data[8 + epc_len];
    crc <<= 8;
    crc += data[8 + epc_len + 1];
    // validate checksum
    uint8_t cs = checksum(data + 1, length - 3);
    for(size_t i = 0; i < length; i++){
        FURI_LOG_E("m100", "data[%d]=%02X", i, data[i]);
    }
    if(cs != data[length - 2]) return NULL;
    FURI_LOG_E("m100", "checksum pass");
    // validate crc
    uint16_t ccrc = crc16_genibus(data + 6, epc_len + 2);
    FURI_LOG_E("m100", "crc found = %04X, calculated crc = %04X", crc, ccrc);
    if(ccrc != crc) return NULL;
    FURI_LOG_E("m100", "crc pass");
    UHFTag* uhf_tag = uhf_tag_alloc();
    uhf_tag_set_epc_pc(uhf_tag, pc);
    uhf_tag_set_epc_crc(uhf_tag, crc);
    uhf_tag_set_epc(uhf_tag, data + 8, epc_len);
    FURI_LOG_E("m100", "returning tag");
    return uhf_tag;
}

bool m100_set_select(M100Module* module, UHFTag* uhf_tag) {
    buffer_reset(module->buf);
    // Set select
    uint8_t cmd[MAX_BUFFER_SIZE];
    size_t cmd_length = CMD_SET_SELECT_PARAMETER.length;
    size_t mask_length_bytes = uhf_tag->epc->size;
    size_t mask_length_bits = mask_length_bytes * 8;
    // payload len = sel param len + ptr len + mask len + epc len
    size_t payload_len = 7 + mask_length_bytes;
    memcpy(cmd, CMD_SET_SELECT_PARAMETER.cmd, cmd_length);
    // set payload length
    cmd[3] = (payload_len >> 8) & 0xFF;
    cmd[4] = payload_len & 0xFF;
    // set select param
    cmd[5] = 0x01; // 0x00=rfu, 0x01=epc, 0x10=tid, 0x11=user
    // set ptr
    cmd[9] = 0x20; // epc data begins after 0x20
    // set mask length
    cmd[10] = mask_length_bits;
    // truncate
    cmd[11] = false;
    // set mask
    memcpy((void*)&cmd[12], uhf_tag->epc->data, mask_length_bytes);
    // set checksum
    cmd[12 + mask_length_bytes + 1] = checksum(cmd + 1, 11 + mask_length_bytes);
    // end frame
    cmd[12 + mask_length_bytes + 2] = FRAME_END;
    furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, rx_callback, module->buf);
    furi_hal_uart_tx(FuriHalUartIdUSART1, cmd, 12 + mask_length_bytes + 3);
    furi_delay_ms(DELAY_MS);

    uint8_t* data = buffer_get_data(module->buf);
    if(checksum(data + 1, 5) != data[6]) return false; // error in rx
    if(data[5] != 0x00) return false; // error if not 0

    return true;
}

UHFTag* m100_get_select_param(M100Module module) {
    UNUSED(module);
    return NULL;
}

bool m100_read_label_data_storage(
    M100Module* module,
    UHFTag* uhf_tag,
    BankType bank,
    uint32_t access_pwd) {
    UNUSED(uhf_tag);
    buffer_reset(module->buf);
    uint8_t cmd[MAX_BUFFER_SIZE];
    size_t length = CMD_READ_LABEL_DATA_STORAGE_AREA.length;
    memcpy(cmd, CMD_READ_LABEL_DATA_STORAGE_AREA.cmd, length);
    // set access password
    cmd[5] = (access_pwd >> 24) & 0xFF;
    cmd[6] = (access_pwd >> 16) & 0xFF;
    cmd[7] = (access_pwd >> 8) & 0xFF;
    cmd[8] = access_pwd & 0xFF;
    // set mem bank
    cmd[9] = (uint8_t)bank;
    // recalc checksum
    cmd[length - 2] = checksum(cmd + 1, length - 3);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, rx_callback, module->buf);
    furi_hal_uart_tx(FuriHalUartIdUSART1, cmd, length);
    furi_delay_ms(DELAY_MS);
    return true;
}

void m100_set_baudrate(M100Module* module, uint16_t baudrate) {
    size_t length = CMD_SET_COMMUNICATION_BAUD_RATE.length;
    uint8_t cmd[length];
    memcpy(cmd, CMD_SET_COMMUNICATION_BAUD_RATE.cmd, length);
    uint16_t br_mod = baudrate / 100; // module format
    cmd[6] = 0xFF & br_mod; // pow LSB
    cmd[5] = 0xFF & (br_mod >> 4); // pow MSB
    // furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, NULL, NULL);
    furi_hal_uart_tx(FuriHalUartIdUSART1, cmd, length);
    furi_hal_uart_set_br(FuriHalUartIdUSART1, baudrate);
    module->baudrate = baudrate;
}
bool m100_set_working_area(M100Module* module, WorkingArea area) {
    size_t length = CMD_SET_WORK_AREA.length;
    uint8_t cmd[length];
    memcpy(cmd, CMD_SET_WORK_AREA.cmd, length);
    cmd[5] = area;
    Buffer* buf = buffer_alloc(12);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, rx_callback, buf);
    furi_hal_uart_tx(FuriHalUartIdUSART1, cmd, length);
    buffer_free(buf);
    module->area = area;
    return true;
}
bool m100_set_working_channel(M100Module* module, WorkingChannel channel) {
    UNUSED(module);
    UNUSED(channel);
    return true;
}
bool m100_set_transmitting_power(M100Module* module, uint16_t power) {
    UNUSED(module);
    UNUSED(power);
    return true;
}
bool m100_set_freq_hopping(M100Module* module, bool hopping) {
    UNUSED(module);
    UNUSED(hopping);
    return true;
}
