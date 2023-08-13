#include "uhf_module.h"
#include "uhf_module_cmd.h"
#include "uhf_buffer.h"
#include "uhf_tag.h"
#include <furi_hal.h>

static void rx_callback(UartIrqEvent event, uint8_t data, void* ctx) {
    UNUSED(event);
    Buffer* buf = ctx;
    if(data == FRAME_END) {
        buffer_append_single(buf, data);
        buffer_close(buf);
    }
    buffer_append_single(buf, data);
}

M100ModuleInfo* m100_module_info_alloc() {
    M100ModuleInfo* module_info = (M100ModuleInfo*)malloc(sizeof(M100ModuleInfo));
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
}

void m100_module_free(M100Module* module) {
    m100_module_info_free(module->info);
    buffer_free(module->buf);
    free(module);
}

uint8_t checksum(uint8_t* data, size_t length) {
    // CheckSum8 Modulo 256
    // Sum of Bytes % 256
    uint8_t sum_val = 0x00;
    for(size_t i = 1; i < length; i++) {
        sum_val += data[i];
    }
    return sum_val % 256;
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
    return module->info->hw_version;
}
char* m100_get_software_version(M100Module* module) {
    return module->info->sw_version;
}
char* m100_get_manufacturers(M100Module* module) {
    return module->info->manufacturer;
}

UHFTag* m100_read_single(M100Module* module) {
    buffer_reset(module->buf);
    furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, rx_callback, module->buf);
    furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_SINGLE_POLLING.cmd, CMD_SINGLE_POLLING.length);
    uint8_t* data = buffer_get_data(module->buf);
    size_t length = buffer_get_size(module->buf);
    if(length == 7 && data[2] == 0xFF) return NULL;
    uint16_t pc = data[6];
    uint16_t crc = 0;
    size_t epc_len = pc;
    epc_len <<= 1;
    epc_len += (data[7] & 0x80) > 0;
    epc_len *= 2;
    pc <<= 8;
    pc += data[7];
    crc = data[8 + epc_len + 1];
    crc <<= 8;
    crc += data[8 + epc_len + 2];
    if(checksum(data + 1, length - 3) != data[length - 2]) return NULL;
    if(crc16_genibus(data + 6, epc_len + 2) != crc) return NULL;
    UHFTag* uhf_tag = uhf_tag_alloc();
    uhf_tag_set_epc_pc(uhf_tag, pc);
    uhf_tag_set_epc_crc(uhf_tag, crc);
    uhf_tag_set_epc(uhf_tag, data + 8, epc_len);
    return uhf_tag;
}

void m100_set_baudrate(M100Module* module, uint16_t baudrate) {
    size_t length = CMD_SET_COMMUNICATION_BAUD_RATE.length;
    uint8_t cmd[length];
    memcpy(cmd, CMD_SET_COMMUNICATION_BAUD_RATE.cmd, length);
    cmd[6] = 0xFF & baudrate; // pow LSB
    cmd[5] = 0xFF & (baudrate >> 4); // pow MSB
    // furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, NULL, NULL);
    furi_hal_uart_tx(FuriHalUartIdUSART1, cmd, length);
    furi_hal_uart_set_br(FuriHalUartIdUSART1, baudrate * 100);
    module->baudrate = baudrate;
}
bool m100_set_working_area(M100Module* module, WorkingArea area) {
    size_t length = CMD_SET_WORK_AREA.length;
    uint8_t cmd[length];
    memcpy(cmd, CMD_SET_WORK_AREA.cmd, length);
    cmd[5] = area;
    Buffer* buf = buffer_alloc(9);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, rx_callback, buf);
    furi_hal_uart_tx(FuriHalUartIdUSART1, cmd, length);
    buffer_free(buf);
    return true;
}
bool m100_set_working_channel(M100Module* module, WorkingChannel channel) {
    return true;
}
bool m100_set_transmitting_power(M100Module* module, uint16_t power) {
    return true;
}
bool m100_set_freq_hopping(M100Module* module, bool hopping) {
    return true;
}