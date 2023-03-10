#include "avr_isp_prog.h"
#include "avr_isp_prog_cmd.h"

#include <furi.h>

#define AVR_ISP_PROG_TX_RX_BUF_SIZE 256

struct AvrIspProgCfgDevice {
    uint8_t devicecode;
    uint8_t revision;
    uint8_t progtype;
    uint8_t parmode;
    uint8_t polling;
    uint8_t selftimed;
    uint8_t lockbytes;
    uint8_t fusebytes;
    uint8_t flashpoll;
    uint16_t eeprompoll;
    uint16_t pagesize;
    uint16_t eepromsize;
    uint32_t flashsize;
};

typedef struct AvrIspProgCfgDevice AvrIspProgCfgDevice;

struct AvrIspProg {
    AvrIspSpiSw* spi;
    AvrIspProgCfgDevice* cfg;
    FuriStreamBuffer* stream_rx;
    FuriStreamBuffer* stream_tx;

    uint16_t error;
    uint16_t addr;
    bool pmode;
    bool tx_ok;
    bool exit;
    bool rst_active_high;
    uint8_t buff[AVR_ISP_PROG_TX_RX_BUF_SIZE];
    AvrIspSpiSwSpeed spi_speed;
};

AvrIspProg* avr_isp_prog_init(AvrIspSpiSwSpeed spi_speed) {
    AvrIspProg* instance = malloc(sizeof(AvrIspProg));
    instance->cfg = malloc(sizeof(AvrIspProgCfgDevice));
    instance->stream_rx =
        furi_stream_buffer_alloc(sizeof(int8_t) * AVR_ISP_PROG_TX_RX_BUF_SIZE, sizeof(int8_t));
    instance->stream_tx =
        furi_stream_buffer_alloc(sizeof(int8_t) * AVR_ISP_PROG_TX_RX_BUF_SIZE, sizeof(int8_t));
    instance->spi_speed = spi_speed;
    instance->tx_ok = false;
    instance->rst_active_high = false;
    instance->exit = false;
    return instance;
}

void avr_isp_prog_free(AvrIspProg* instance) {
    furi_assert(instance);

    furi_stream_buffer_free(instance->stream_tx);
    furi_stream_buffer_free(instance->stream_rx);
    free(instance->cfg);
    free(instance);
}

bool avr_isp_prog_rx(AvrIspProg* instance, uint8_t* data, size_t len) {
    furi_assert(instance);
    furi_assert(data);
    furi_assert(len != 0);
    size_t ret = furi_stream_buffer_send(instance->stream_rx, data, sizeof(uint8_t) * len, 100);
    return ret == sizeof(uint8_t) * len;
}

size_t avr_isp_prog_tx(AvrIspProg* instance, uint8_t* data, size_t max_len) {
    furi_assert(instance);
    if(!instance->tx_ok) return 0;
    size_t len = 0;
    if(furi_stream_buffer_bytes_available(instance->stream_tx) > max_len) {
        len = furi_stream_buffer_receive(instance->stream_tx, data, sizeof(int8_t) * max_len, 100);
    } else {
        len = furi_stream_buffer_receive(
            instance->stream_tx,
            data,
            sizeof(int8_t) * furi_stream_buffer_bytes_available(instance->stream_tx),
            100);
        instance->tx_ok = false;
    }
    return len;
}

void avr_isp_prog_exit(AvrIspProg* instance) {
    instance->exit = true;
}

static bool avr_isp_prog_tx_ch(AvrIspProg* instance, uint8_t data) {
    furi_assert(instance);
    size_t ret = furi_stream_buffer_send(instance->stream_tx, &data, sizeof(uint8_t), 10);
    return ret == sizeof(uint8_t);
}

static uint8_t avr_isp_prog_getch(AvrIspProg* instance) {
    furi_assert(instance);
    //ToDo !!!!!!!!!!!!!!!
    while(!furi_stream_buffer_bytes_available(instance->stream_rx)) {
        furi_delay_ms(5);
        if(instance->exit) break;
    }
    uint8_t data[1] = {0};
    furi_stream_buffer_receive(instance->stream_rx, &data, sizeof(int8_t), 10);
    return data[0];
}

static void avr_isp_prog_fill(AvrIspProg* instance, size_t len) {
    furi_assert(instance);
    for(size_t x = 0; x < len; x++) {
        instance->buff[x] = avr_isp_prog_getch(instance);
    }
}

static void avr_isp_prog_reset_target(AvrIspProg* instance, bool reset) {
    furi_assert(instance);
    avr_isp_spi_sw_res_set(
        instance->spi,
        ((reset && instance->rst_active_high) || (!reset && !instance->rst_active_high)) ? true :
                                                                                           false);
}

static uint8_t
    avr_isp_prog_spi_transaction(AvrIspProg* instance, uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    furi_assert(instance);
    avr_isp_spi_sw_txrx(instance->spi, a);
    avr_isp_spi_sw_txrx(instance->spi, b);
    avr_isp_spi_sw_txrx(instance->spi, c);
    return avr_isp_spi_sw_txrx(instance->spi, d);
}

static void avr_isp_prog_empty_reply(AvrIspProg* instance) {
    furi_assert(instance);
    if(avr_isp_prog_getch(instance) == CRC_EOP) {
        avr_isp_prog_tx_ch(instance, STK_INSYNC);
        avr_isp_prog_tx_ch(instance, STK_OK);
    } else {
        instance->error++;
        avr_isp_prog_tx_ch(instance, STK_NOSYNC);
    }
}

static void avr_isp_prog_breply(AvrIspProg* instance, uint8_t data) {
    furi_assert(instance);
    if(avr_isp_prog_getch(instance) == CRC_EOP) {
        avr_isp_prog_tx_ch(instance, STK_INSYNC);
        avr_isp_prog_tx_ch(instance, data);
        avr_isp_prog_tx_ch(instance, STK_OK);
    } else {
        instance->error++;
        avr_isp_prog_tx_ch(instance, STK_NOSYNC);
    }
}

static void avr_isp_prog_get_version(AvrIspProg* instance, uint8_t data) {
    furi_assert(instance);
    switch(data) {
    case STK_HW_VER:
        avr_isp_prog_breply(instance, AVR_ISP_HWVER);
        break;
    case STK_SW_MAJOR:
        avr_isp_prog_breply(instance, AVR_ISP_SWMAJ);
        break;
    case STK_SW_MINOR:
        avr_isp_prog_breply(instance, AVR_ISP_SWMIN);
        break;
    case AVP_ISP_GET_CONNECT_TYPE:
        avr_isp_prog_breply(instance, AVP_ISP_SERIAL_CONNECT_TYPE);
        break;
    default:
        avr_isp_prog_breply(instance, AVR_ISP_RESP_0);
    }
}

static void avr_isp_prog_set_cfg(AvrIspProg* instance) {
    furi_assert(instance);
    // call this after reading cfg packet into buff[]
    instance->cfg->devicecode = instance->buff[0];
    instance->cfg->revision = instance->buff[1];
    instance->cfg->progtype = instance->buff[2];
    instance->cfg->parmode = instance->buff[3];
    instance->cfg->polling = instance->buff[4];
    instance->cfg->selftimed = instance->buff[5];
    instance->cfg->lockbytes = instance->buff[6];
    instance->cfg->fusebytes = instance->buff[7];
    instance->cfg->flashpoll = instance->buff[8];
    // ignore (instance->buff[9] == instance->buff[8]) //FLASH polling value. Same as “flashpoll”
    instance->cfg->eeprompoll = instance->buff[10] << 8 | instance->buff[11];
    instance->cfg->pagesize = instance->buff[12] << 8 | instance->buff[13];
    instance->cfg->eepromsize = instance->buff[14] << 8 | instance->buff[15];
    instance->cfg->flashsize = instance->buff[16] << 24 | instance->buff[17] << 16 |
                               instance->buff[18] << 8 | instance->buff[19];

    // avr devices have active low reset, at89sx are active high
    instance->rst_active_high = (instance->cfg->devicecode >= 0xe0);
}
static bool
    avr_isp_prog_set_pmode(AvrIspProg* instance, uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    furi_assert(instance);
    uint8_t res = 0;
    avr_isp_spi_sw_txrx(instance->spi, a);
    avr_isp_spi_sw_txrx(instance->spi, b);
    res = avr_isp_spi_sw_txrx(instance->spi, c);
    avr_isp_spi_sw_txrx(instance->spi, d);
    return res == 0x53;
}
static bool avr_isp_prog_start_pmode(AvrIspProg* instance) {
    furi_assert(instance);
    // Reset target before driving PIN_SCK or PIN_MOSI

    // SPI.begin() will configure SS as output,
    // so SPI master mode is selected.
    // We have defined RESET as pin 10,
    // which for many arduino's is not the SS pin.
    // So we have to configure RESET as output here,
    // (reset_target() first sets the correct level)
    instance->spi = avr_isp_spi_sw_init(instance->spi_speed);
    avr_isp_prog_reset_target(instance, true);

    // See avr datasheets, chapter "SERIAL_PRG Programming Algorithm":

    // Pulse RESET after PIN_SCK is low:
    avr_isp_spi_sw_sck_set(instance->spi, false);

    furi_delay_ms(20); // discharge PIN_SCK, value arbitrally chosen
    avr_isp_prog_reset_target(instance, false);

    // Pulse must be minimum 2 target CPU speed cycles
    // so 100 usec is ok for CPU speeds above 20KHz
    //furi_delay_us(100);
    furi_delay_ms(1);

    avr_isp_prog_reset_target(instance, true);

    // Send the enable programming command:
    furi_delay_ms(50); // datasheet: must be > 20 msec
     //if(avr_isp_prog_set_pmode(instance, 0xAC, 0x53, 0x00, 0x00)) {
    if(avr_isp_prog_set_pmode(instance, AVR_ISP_SET_PMODE)) {
        instance->pmode = true;
        return true;
    }
    return false;
}

static void avr_isp_prog_end_pmode(AvrIspProg* instance) {
    furi_assert(instance);
    avr_isp_prog_reset_target(instance, false);
    // We're about to take the target out of reset
    // so configure SPI pins as input
    avr_isp_spi_sw_free(instance->spi);
    instance->pmode = false;
}

static void avr_isp_prog_universal(AvrIspProg* instance) {
    furi_assert(instance);
    uint8_t data;

    avr_isp_prog_fill(instance, 4);
    data = avr_isp_prog_spi_transaction(
        instance, instance->buff[0], instance->buff[1], instance->buff[2], instance->buff[3]);
    avr_isp_prog_breply(instance, data);
}

static void avr_isp_prog_flash(AvrIspProg* instance, uint8_t hilo, uint16_t addr, uint8_t data) {
    avr_isp_prog_spi_transaction(instance, 0x40 + 8 * hilo, addr >> 8 & 0xFF, addr & 0xFF, data);
}

static void avr_isp_prog_commit(AvrIspProg* instance, uint16_t addr) {
    // if(PROG_FLICKER) {
    //     prog_lamp(0);
    // }
    avr_isp_prog_spi_transaction(instance, 0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);
    // if(PROG_FLICKER) {
    //     delay(PTIME);
    //     prog_lamp(1);
    // }
}

static uint16_t avr_isp_prog_current_page(AvrIspProg* instance) {
    if(instance->cfg->pagesize == 32) {
        return instance->addr & 0xFFFFFFF0;
    }
    if(instance->cfg->pagesize == 64) {
        return instance->addr & 0xFFFFFFE0;
    }
    if(instance->cfg->pagesize == 128) {
        return instance->addr & 0xFFFFFFC0;
    }
    if(instance->cfg->pagesize == 256) {
        return instance->addr & 0xFFFFFF80;
    }
    return instance->addr;
}

static uint8_t avr_isp_prog_write_flash_pages(AvrIspProg* instance, size_t length) {
    size_t x = 0;
    uint16_t page = avr_isp_prog_current_page(instance);
    while(x < length) {
        if(page != avr_isp_prog_current_page(instance)) {
            avr_isp_prog_commit(instance, page);
            page = avr_isp_prog_current_page(instance);
        }
        avr_isp_prog_flash(instance, 0, instance->addr, instance->buff[x++]);
        avr_isp_prog_flash(instance, 1, instance->addr, instance->buff[x++]);
        instance->addr++;
    }

    avr_isp_prog_commit(instance, page);

    return STK_OK;
}

static void avr_isp_prog_write_flash(AvrIspProg* instance, size_t length) {
    avr_isp_prog_fill(instance, length);
    if(avr_isp_prog_getch(instance) == CRC_EOP) {
        avr_isp_prog_tx_ch(instance, STK_INSYNC);
        avr_isp_prog_tx_ch(instance, avr_isp_prog_write_flash_pages(instance, length));
    } else {
        instance->error++;
        avr_isp_prog_tx_ch(instance, STK_NOSYNC);
    }
}

// write (length) bytes, (start) is a byte address
static uint8_t
    avr_isp_prog_write_eeprom_chunk(AvrIspProg* instance, uint16_t start, uint16_t length) {
    // this writes byte-by-byte,
    // page writing may be faster (4 bytes at a time)
    avr_isp_prog_fill(instance, length);
    //prog_lamp(0);
    for(uint16_t x = 0; x < length; x++) {
        uint16_t addr = start + x;
        avr_isp_prog_spi_transaction(
            instance, 0xC0, (addr >> 8) & 0xFF, addr & 0xFF, instance->buff[x]);
        furi_delay_ms(45);
    }
    //prog_lamp(1);
    return STK_OK;
}

static uint8_t avr_isp_prog_write_eeprom(AvrIspProg* instance, size_t length) {
    // here is a word address, get the byte address
    uint16_t start = instance->addr * 2;
    uint16_t remaining = length;
    if(length > instance->cfg->eepromsize) {
        instance->error++;
        return STK_FAILED;
    }
    while(remaining > EECHUNK) {
        avr_isp_prog_write_eeprom_chunk(instance, start, EECHUNK);
        start += EECHUNK;
        remaining -= EECHUNK;
    }
    avr_isp_prog_write_eeprom_chunk(instance, start, remaining);
    return STK_OK;
}

static void avr_isp_prog_program_page(AvrIspProg* instance) {
    uint8_t result = STK_FAILED;
    uint16_t length = avr_isp_prog_getch(instance) << 8 | avr_isp_prog_getch(instance);
    uint8_t memtype = avr_isp_prog_getch(instance);
    // flash memory @addr, (length) bytes
    if(memtype == STK_SET_FLASH_TYPE) {
        avr_isp_prog_write_flash(instance, length);
        return;
    }
    if(memtype == STK_SET_EEROM_TYPE) {
        result = avr_isp_prog_write_eeprom(instance, length);
        if(avr_isp_prog_getch(instance) == CRC_EOP) {
            avr_isp_prog_tx_ch(instance, STK_INSYNC);
            avr_isp_prog_tx_ch(instance, result);

        } else {
            instance->error++;
            avr_isp_prog_tx_ch(instance, STK_NOSYNC);
        }
        return;
    }
    avr_isp_prog_tx_ch(instance, STK_FAILED);
    return;
}

static uint8_t avr_isp_prog_flash_read(AvrIspProg* instance, uint8_t hilo, uint16_t addr) {
    return avr_isp_prog_spi_transaction(
        instance, 0x20 + hilo * 8, (addr >> 8) & 0xFF, addr & 0xFF, 0);
}

static uint8_t avr_isp_prog_flash_read_page(AvrIspProg* instance, uint16_t length) {
    for(uint16_t x = 0; x < length; x += 2) {
        avr_isp_prog_tx_ch(instance, avr_isp_prog_flash_read(instance, 0, instance->addr));
        avr_isp_prog_tx_ch(instance, avr_isp_prog_flash_read(instance, 1, instance->addr));
        instance->addr++;
    }
    return STK_OK;
}

static uint8_t avr_isp_prog_eeprom_read_page(AvrIspProg* instance, uint16_t length) {
    // here again we have a word address
    uint16_t start = instance->addr * 2;
    for(uint16_t x = 0; x < length; x++) {
        uint16_t addr = start + x;
        avr_isp_prog_tx_ch(
            instance,
            avr_isp_prog_spi_transaction(instance, 0xA0, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF));
    }
    return STK_OK;
}

static void avr_isp_prog_read_page(AvrIspProg* instance) {
    uint8_t result = STK_FAILED;
    uint16_t length = avr_isp_prog_getch(instance) << 8 | avr_isp_prog_getch(instance);
    uint8_t memtype = avr_isp_prog_getch(instance);
    if(avr_isp_prog_getch(instance) != CRC_EOP) {
        instance->error++;
        avr_isp_prog_tx_ch(instance, STK_NOSYNC);
        return;
    }
    avr_isp_prog_tx_ch(instance, STK_INSYNC);
    if(memtype == STK_SET_FLASH_TYPE) result = avr_isp_prog_flash_read_page(instance, length);
    if(memtype == STK_SET_EEROM_TYPE) result = avr_isp_prog_eeprom_read_page(instance, length);
    avr_isp_prog_tx_ch(instance, result);
}

static void avr_isp_prog_read_signature(AvrIspProg* instance) {
    if(avr_isp_prog_getch(instance) != CRC_EOP) {
        instance->error++;
        avr_isp_prog_tx_ch(instance, STK_NOSYNC);
        return;
    }
    avr_isp_prog_tx_ch(instance, STK_INSYNC);

    avr_isp_prog_tx_ch(instance, avr_isp_prog_spi_transaction(instance, 0x30, 0x00, 0x00, 0x00));
    avr_isp_prog_tx_ch(instance, avr_isp_prog_spi_transaction(instance, 0x30, 0x00, 0x01, 0x00));
    avr_isp_prog_tx_ch(instance, avr_isp_prog_spi_transaction(instance, 0x30, 0x00, 0x02, 0x00));

    avr_isp_prog_tx_ch(instance, STK_OK);
}

void avr_isp_prog_avrisp(AvrIspProg* instance) {
    uint8_t ch = avr_isp_prog_getch(instance);
    switch(ch) {
    case STK_GET_SYNC:
        instance->error = 0;
        avr_isp_prog_empty_reply(instance);
        break;
    case STK_GET_SIGN_ON:
        if(avr_isp_prog_getch(instance) == CRC_EOP) {
            avr_isp_prog_tx_ch(instance, STK_INSYNC);

            avr_isp_prog_tx_ch(instance, 'A');
            avr_isp_prog_tx_ch(instance, 'V');
            avr_isp_prog_tx_ch(instance, 'R');
            avr_isp_prog_tx_ch(instance, ' ');
            avr_isp_prog_tx_ch(instance, 'I');
            avr_isp_prog_tx_ch(instance, 'S');
            avr_isp_prog_tx_ch(instance, 'P');

            avr_isp_prog_tx_ch(instance, STK_OK);
        } else {
            instance->error++;
            avr_isp_prog_tx_ch(instance, STK_NOSYNC);
        }
        break;
    case STK_GET_PARAMETER:
        avr_isp_prog_get_version(instance, avr_isp_prog_getch(instance));
        break;
    case STK_SET_DEVICE:
        avr_isp_prog_fill(instance, 20);
        avr_isp_prog_set_cfg(instance);
        avr_isp_prog_empty_reply(instance);
        break;
    case STK_SET_DEVICE_EXT: // ignore for now
        avr_isp_prog_fill(instance, 5);
        avr_isp_prog_empty_reply(instance);
        break;
    case STK_ENTER_PROGMODE:
        if(!instance->pmode) avr_isp_prog_start_pmode(instance);
        avr_isp_prog_empty_reply(instance);
        break;
    case STK_LOAD_ADDRESS:
        instance->addr = avr_isp_prog_getch(instance) | avr_isp_prog_getch(instance) << 8;
        // instance->addr = avr_isp_prog_getch(instance);
        // instance->addr += 256 * avr_isp_prog_getch(instance);
        avr_isp_prog_empty_reply(instance);
        break;

    case STK_PROG_FLASH: // ignore for now
        avr_isp_prog_getch(instance);
        avr_isp_prog_getch(instance);
        avr_isp_prog_empty_reply(instance);
        break;
    case STK_PROG_DATA: // ignore for now
        avr_isp_prog_getch(instance);
        avr_isp_prog_empty_reply(instance);
        break;

    case STK_PROG_PAGE:
        avr_isp_prog_program_page(instance);
        break;

    case STK_READ_PAGE:
        avr_isp_prog_read_page(instance);
        break;

    case STK_UNIVERSAL:
        avr_isp_prog_universal(instance);
        break;
    case STK_LEAVE_PROGMODE:
        instance->error = 0;
        if(instance->pmode) avr_isp_prog_end_pmode(instance);
        avr_isp_prog_empty_reply(instance);
        break;

    case STK_READ_SIGN:
        avr_isp_prog_read_signature(instance);
        break;

    // expecting a command, not CRC_EOP
    // this is how we can get back in sync
    case CRC_EOP:
        instance->error++;
        avr_isp_prog_tx_ch(instance, STK_NOSYNC);
        break;

    // anything else we will return STK_UNKNOWN
    default:
        instance->error++;
        if(avr_isp_prog_getch(instance) == CRC_EOP)
            avr_isp_prog_tx_ch(instance, STK_UNKNOWN);
        else
            avr_isp_prog_tx_ch(instance, STK_NOSYNC);
    }
    instance->tx_ok = true;
}
