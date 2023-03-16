#include "avr_isp.h"
#include "../lib/driver/avr_isp_prog_cmd.h"
#include "../lib/driver/avr_isp_spi_sw.h"

#include <furi.h>

#define AVR_ISP_PROG_TX_RX_BUF_SIZE 320
#define TAG "AvrIsp"

struct AvrIsp {
    AvrIspSpiSw* spi;

    uint16_t error;
    uint16_t addr;
    bool pmode;
    //uint8_t buff[AVR_ISP_PROG_TX_RX_BUF_SIZE];

    uint16_t pagesize;
    uint16_t eepromsize;
    uint32_t flashsize;

    AvrIspCallback callback;
    void* context;
};

AvrIsp* avr_isp_alloc(void) {
    AvrIsp* instance = malloc(sizeof(AvrIsp));
    return instance;
}

void avr_isp_free(AvrIsp* instance) {
    furi_assert(instance);
    if(instance->spi) avr_isp_end_pmode(instance);
    free(instance);
}

void avr_isp_set_tx_callback(AvrIsp* instance, AvrIspCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(context);
    instance->callback = callback;
    instance->context = context;
}

uint8_t avr_isp_spi_transaction(
    AvrIsp* instance,
    uint8_t cmd,
    uint8_t addr_hi,
    uint8_t addr_lo,
    uint8_t data) {
    furi_assert(instance);

    avr_isp_spi_sw_txrx(instance->spi, cmd);
    avr_isp_spi_sw_txrx(instance->spi, addr_hi);
    avr_isp_spi_sw_txrx(instance->spi, addr_lo);
    return avr_isp_spi_sw_txrx(instance->spi, data);
}

static bool avr_isp_set_pmode(AvrIsp* instance, uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    furi_assert(instance);
    uint8_t res = 0;
    avr_isp_spi_sw_txrx(instance->spi, a);
    avr_isp_spi_sw_txrx(instance->spi, b);
    res = avr_isp_spi_sw_txrx(instance->spi, c);
    avr_isp_spi_sw_txrx(instance->spi, d);
    return res == 0x53;
}

void avr_isp_end_pmode(AvrIsp* instance) {
    furi_assert(instance);
    if(instance->pmode) {
        avr_isp_spi_sw_res_set(instance->spi, true);
        // We're about to take the target out of reset
        // so configure SPI pins as input
        if(instance->spi) avr_isp_spi_sw_free(instance->spi);
        instance->spi = NULL;
    }

    instance->pmode = false;
}

static bool avr_isp_start_pmode(AvrIsp* instance, AvrIspSpiSwSpeed spi_speed) {
    furi_assert(instance);
    // Reset target before driving PIN_SCK or PIN_MOSI

    // SPI.begin() will configure SS as output,
    // so SPI master mode is selected.
    // We have defined RESET as pin 10,
    // which for many arduino's is not the SS pin.
    // So we have to configure RESET as output here,
    // (reset_target() first sets the correct level)
    if(instance->spi) avr_isp_spi_sw_free(instance->spi);
    instance->spi = avr_isp_spi_sw_init(spi_speed);

    avr_isp_spi_sw_res_set(instance->spi, false);
    // See avr datasheets, chapter "SERIAL_PRG Programming Algorithm":

    // Pulse RESET after PIN_SCK is low:
    avr_isp_spi_sw_sck_set(instance->spi, false);

    // discharge PIN_SCK, value arbitrally chosen
    furi_delay_ms(20);
    avr_isp_spi_sw_res_set(instance->spi, true);

    // Pulse must be minimum 2 target CPU speed cycles
    // so 100 usec is ok for CPU speeds above 20KHz
    furi_delay_ms(1);

    avr_isp_spi_sw_res_set(instance->spi, false);

    // Send the enable programming command:
    // datasheet: must be > 20 msec
    furi_delay_ms(50);
    if(avr_isp_set_pmode(instance, AVR_ISP_SET_PMODE)) {
        instance->pmode = true;
        return true;
    }
    return false;
}

bool avr_isp_auto_set_spi_speed_start_pmode(AvrIsp* instance) {
    AvrIspSpiSwSpeed spi_speed[] = {
        AvrIspSpiSwSpeed1Mhz,
        AvrIspSpiSwSpeed400Khz,
        AvrIspSpiSwSpeed250Khz,
        AvrIspSpiSwSpeed125Khz,
        AvrIspSpiSwSpeed40Khz,
        AvrIspSpiSwSpeed20Khz,
    };
    for(uint8_t i = 0; i < COUNT_OF(spi_speed); i++) {
        if(avr_isp_start_pmode(instance, spi_speed[i])) {
            return true;
        }
    }
    return false;
}

// static void avr_isp_universal(AvrIsp* instance) {
//     furi_assert(instance);
//     uint8_t data;

//     avr_isp_fill(instance, 4);
//     data = avr_isp_spi_transaction(
//         instance, instance->buff[0], instance->buff[1], instance->buff[2], instance->buff[3]);
//     avr_isp_breply(instance, data);
// }

// static void avr_isp_commit(AvrIsp* instance, uint16_t addr, uint8_t data) {
//     furi_assert(instance);
//     avr_isp_spi_transaction(instance, AVR_ISP_COMMIT(addr));
//     /* polling flash */
//     if(data == 0xFF) {
//         furi_delay_ms(5);
//     } else {
//         /* polling flash */
//         uint32_t starttime = furi_get_tick();
//         while((furi_get_tick() - starttime) < 30) {
//             if(avr_isp_spi_transaction(instance, AVR_ISP_READ_FLASH_HI(addr)) != 0xFF) {
//                 break;
//             };
//         }
//     }
// }

// static uint16_t avr_isp_current_page(AvrIsp* instance) {
//     furi_assert(instance);
//     uint16_t page = 0;
//     switch(instance->cfg->pagesize) {
//     case 32:
//         page = instance->addr & 0xFFFFFFF0;
//         break;
//     case 64:
//         page = instance->addr & 0xFFFFFFE0;
//         break;
//     case 128:
//         page = instance->addr & 0xFFFFFFC0;
//         break;
//     case 256:
//         page = instance->addr & 0xFFFFFF80;
//         break;

//     default:
//         page = instance->addr;
//         break;
//     }

//     return page;
// }

// static uint8_t avr_isp_write_flash_pages(AvrIsp* instance, size_t length) {
//     furi_assert(instance);
//     size_t x = 0;
//     uint16_t page = avr_isp_current_page(instance);
//     while(x < length) {
//         if(page != avr_isp_current_page(instance)) {
//             --x;
//             avr_isp_commit(instance, page, instance->buff[x++]);
//             page = avr_isp_current_page(instance);
//         }
//         avr_isp_spi_transaction(
//             instance, AVR_ISP_WRITE_FLASH_LO(instance->addr, instance->buff[x++]));

//         avr_isp_spi_transaction(
//             instance, AVR_ISP_WRITE_FLASH_HI(instance->addr, instance->buff[x++]));
//         instance->addr++;
//     }

//     avr_isp_commit(instance, page, instance->buff[--x]);
//     return STK_OK;
// }

// static void avr_isp_write_flash(AvrIsp* instance, size_t length) {
//     furi_assert(instance);
//     avr_isp_fill(instance, length);
//     if(avr_isp_getch(instance) == CRC_EOP) {
//         avr_isp_tx_ch(instance, STK_INSYNC);
//         avr_isp_tx_ch(instance, avr_isp_write_flash_pages(instance, length));
//     } else {
//         instance->error++;
//         avr_isp_tx_ch(instance, STK_NOSYNC);
//     }
// }

// // write (length) bytes, (start) is a byte address
// static uint8_t
//     avr_isp_write_eeprom_chunk(AvrIsp* instance, uint16_t start, uint16_t length) {
//     furi_assert(instance);
//     // this writes byte-by-byte,
//     // page writing may be faster (4 bytes at a time)
//     avr_isp_fill(instance, length);
//     for(uint16_t x = 0; x < length; x++) {
//         uint16_t addr = start + x;
//         avr_isp_spi_transaction(instance, AVR_ISP_WRITE_EEPROM(addr, instance->buff[x]));
//         furi_delay_ms(10);
//     }
//     return STK_OK;
// }

// static uint8_t avr_isp_write_eeprom(AvrIsp* instance, size_t length) {
//     furi_assert(instance);
//     // here is a word address, get the byte address
//     uint16_t start = instance->addr * 2;
//     uint16_t remaining = length;
//     if(length > instance->cfg->eepromsize) {
//         instance->error++;
//         return STK_FAILED;
//     }
//     while(remaining > AVR_ISP_EECHUNK) {
//         avr_isp_write_eeprom_chunk(instance, start, AVR_ISP_EECHUNK);
//         start += AVR_ISP_EECHUNK;
//         remaining -= AVR_ISP_EECHUNK;
//     }
//     avr_isp_write_eeprom_chunk(instance, start, remaining);
//     return STK_OK;
// }

// static void avr_isp_program_page(AvrIsp* instance) {
//     furi_assert(instance);
//     uint8_t result = STK_FAILED;
//     uint16_t length = avr_isp_getch(instance) << 8 | avr_isp_getch(instance);
//     uint8_t memtype = avr_isp_getch(instance);
//     // flash memory @addr, (length) bytes
//     if(memtype == STK_SET_FLASH_TYPE) {
//         avr_isp_write_flash(instance, length);
//         return;
//     }
//     if(memtype == STK_SET_EEPROM_TYPE) {
//         result = avr_isp_write_eeprom(instance, length);
//         if(avr_isp_getch(instance) == CRC_EOP) {
//             avr_isp_tx_ch(instance, STK_INSYNC);
//             avr_isp_tx_ch(instance, result);

//         } else {
//             instance->error++;
//             avr_isp_tx_ch(instance, STK_NOSYNC);
//         }
//         return;
//     }
//     avr_isp_tx_ch(instance, STK_FAILED);
//     return;
// }

static bool avr_isp_flash_read_page(
    AvrIsp* instance,
    uint16_t addr,
    uint16_t size_page,
    uint8_t* data,
    uint32_t size_data) {
    furi_assert(instance);
    if(size_page > size_data) return false;
    for(uint16_t i = 0; i < size_page; i += 2) {
        data[i] = avr_isp_spi_transaction(instance, AVR_ISP_READ_FLASH_LO(addr));
        data[i + 1] = avr_isp_spi_transaction(instance, AVR_ISP_READ_FLASH_HI(addr));
        addr++;
    }
    return true;
}

static bool avr_isp_eeprom_read_page(
    AvrIsp* instance,
    uint16_t addr,
    uint16_t size_page,
    uint8_t* data,
    uint32_t size_data) {
    furi_assert(instance);
    if(size_page > size_data) return false;
    for(uint16_t i = 0; i < size_page; i++) {
        data[i] = avr_isp_spi_transaction(instance, AVR_ISP_READ_EEPROM(addr));
        addr++;
    }
    return true;
}

bool avr_isp_read_page(
    AvrIsp* instance,
    uint32_t memtype,
    uint16_t addr,
    uint16_t size_page,
    uint8_t* data,
    uint32_t size_data) {
    furi_assert(instance);
    bool res = false;
    if(memtype == STK_SET_FLASH_TYPE)
        res = avr_isp_flash_read_page(instance, addr, size_page, data, size_data);
    if(memtype == STK_SET_EEPROM_TYPE)
        res = avr_isp_eeprom_read_page(instance, addr, size_page, data, size_data);

    return res;
}

void avr_isp_read_signature(AvrIsp* instance, uint8_t* data) {
    furi_assert(instance);
    data[0] = avr_isp_spi_transaction(instance, AVR_ISP_READ_VENDOR);
    data[1] = avr_isp_spi_transaction(instance, AVR_ISP_READ_PART_FAMILY);
    data[2] = avr_isp_spi_transaction(instance, AVR_ISP_READ_PART_NUMBER);
}

// void avr_isp_avrisp(AvrIsp* instance) {
//     furi_assert(instance);
//     uint8_t ch = avr_isp_getch(instance);

//     switch(ch) {
//     case STK_GET_SYNC:
//         FURI_LOG_D(TAG, "cmd STK_GET_SYNC");
//         instance->error = 0;
//         avr_isp_empty_reply(instance);
//         break;
//     case STK_GET_SIGN_ON:
//         FURI_LOG_D(TAG, "cmd STK_GET_SIGN_ON");
//         if(avr_isp_getch(instance) == CRC_EOP) {
//             avr_isp_tx_ch(instance, STK_INSYNC);

//             avr_isp_tx_ch(instance, 'A');
//             avr_isp_tx_ch(instance, 'V');
//             avr_isp_tx_ch(instance, 'R');
//             avr_isp_tx_ch(instance, ' ');
//             avr_isp_tx_ch(instance, 'I');
//             avr_isp_tx_ch(instance, 'S');
//             avr_isp_tx_ch(instance, 'P');

//             avr_isp_tx_ch(instance, STK_OK);
//         } else {
//             instance->error++;
//             avr_isp_tx_ch(instance, STK_NOSYNC);
//         }
//         break;
//     case STK_GET_PARAMETER:
//         FURI_LOG_D(TAG, "cmd STK_GET_PARAMETER");
//         avr_isp_get_version(instance, avr_isp_getch(instance));
//         break;
//     case STK_SET_DEVICE:
//         FURI_LOG_D(TAG, "cmd STK_SET_DEVICE");
//         avr_isp_fill(instance, 20);
//         avr_isp_set_cfg(instance);
//         avr_isp_empty_reply(instance);
//         break;
//     case STK_SET_DEVICE_EXT: // ignore for now
//         FURI_LOG_D(TAG, "cmd STK_SET_DEVICE_EXT");
//         avr_isp_fill(instance, 5);
//         avr_isp_empty_reply(instance);
//         break;
//     case STK_ENTER_PROGMODE:
//         FURI_LOG_D(TAG, "cmd STK_ENTER_PROGMODE");
//         if(!instance->pmode) avr_isp_auto_set_spi_speed_start_pmode(instance);
//         avr_isp_empty_reply(instance);
//         break;
//     case STK_LOAD_ADDRESS:
//         FURI_LOG_D(TAG, "cmd STK_LOAD_ADDRESS");
//         instance->addr = avr_isp_getch(instance) | avr_isp_getch(instance) << 8;
//         avr_isp_empty_reply(instance);
//         break;
//     case STK_PROG_FLASH: // ignore for now
//         FURI_LOG_D(TAG, "cmd STK_PROG_FLASH");
//         avr_isp_getch(instance);
//         avr_isp_getch(instance);
//         avr_isp_empty_reply(instance);
//         break;
//     case STK_PROG_DATA: // ignore for now
//         FURI_LOG_D(TAG, "cmd STK_PROG_DATA");
//         avr_isp_getch(instance);
//         avr_isp_empty_reply(instance);
//         break;
//     case STK_PROG_PAGE:
//         FURI_LOG_D(TAG, "cmd STK_PROG_PAGE");
//         avr_isp_program_page(instance);
//         break;
//     case STK_READ_PAGE:
//         FURI_LOG_D(TAG, "cmd STK_READ_PAGE");
//         avr_isp_read_page(instance);
//         break;
//     case STK_UNIVERSAL:
//         FURI_LOG_D(TAG, "cmd STK_UNIVERSAL");
//         avr_isp_universal(instance);
//         break;
//     case STK_LEAVE_PROGMODE:
//         FURI_LOG_D(TAG, "cmd STK_LEAVE_PROGMODE");
//         instance->error = 0;
//         if(instance->pmode) avr_isp_end_pmode(instance);
//         avr_isp_empty_reply(instance);
//         break;
//     case STK_READ_SIGN:
//         FURI_LOG_D(TAG, "cmd STK_READ_SIGN");
//         avr_isp_read_signature(instance);
//         break;
//     // expecting a command, not CRC_EOP
//     // this is how we can get back in sync
//     case CRC_EOP:
//         FURI_LOG_D(TAG, "cmd CRC_EOP");
//         instance->error++;
//         avr_isp_tx_ch(instance, STK_NOSYNC);
//         break;
//     // anything else we will return STK_UNKNOWN
//     default:
//         FURI_LOG_D(TAG, "cmd STK_ERROR_CMD");
//         instance->error++;
//         if(avr_isp_getch(instance) == CRC_EOP)
//             avr_isp_tx_ch(instance, STK_UNKNOWN);
//         else
//             avr_isp_tx_ch(instance, STK_NOSYNC);
//     }

//     if(instance->callback) {
//         instance->callback(instance->context);
//     }
// }
