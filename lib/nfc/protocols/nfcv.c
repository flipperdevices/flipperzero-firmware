#include <limits.h>
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_nfc.h>
#include <furi_hal_spi.h>
#include <furi_hal_gpio.h>
#include <furi_hal_cortex.h>
#include <furi_hal_resources.h>
#include <st25r3916.h>
#include <st25r3916_irq.h>
#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_dmamux.h>
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_exti.h>

#include "nfcv.h"
#include "nfc_util.h"
#include "slix.h"

#define TAG "NfcV"

ReturnCode nfcv_inventory(uint8_t* uid) {
    uint16_t received = 0;
    rfalNfcvInventoryRes res;
    ReturnCode ret = ERR_NONE;

    for(int tries = 0; tries < 5; tries++) {
        /* TODO: needs proper abstraction via fury_hal(_ll)_* */
        ret = rfalNfcvPollerInventory(
            RFAL_NFCV_NUM_SLOTS_1, 0, NULL, &res, &received);

        if(ret == ERR_NONE) {
            break;
        }
    }

    if(ret == ERR_NONE) {
        if(uid != NULL) {
            memcpy(uid, res.UID, 8);
        }
    }

    return ret;
}

ReturnCode nfcv_read_blocks(
    NfcVReader* reader,
    NfcVData* data) {

    UNUSED(reader);

    uint16_t received = 0;
    for(size_t block = 0; block < data->block_num; block++) {
        uint8_t rxBuf[32];
        FURI_LOG_D(TAG, "Reading block %d/%d", block, (data->block_num - 1));

        ReturnCode ret = ERR_NONE;
        for(int tries = 0; tries < 5; tries++) {
            ret = rfalNfcvPollerReadSingleBlock(
                RFAL_NFCV_REQ_FLAG_DEFAULT, NULL, block, 
                rxBuf, sizeof(rxBuf), &received);

            if(ret == ERR_NONE) {
                break;
            }
        }
        if(ret != ERR_NONE) {
            FURI_LOG_D(TAG, "failed to read: %d", ret);
            return ret;
        }
        memcpy(&(data->data[block * data->block_size]), &rxBuf[1], data->block_size);
        FURI_LOG_D(TAG, "  %02X %02X %02X %02X", 
            data->data[block * data->block_size + 0], data->data[block * data->block_size + 1], 
            data->data[block * data->block_size + 2], data->data[block * data->block_size + 3]);
    }

    return ERR_NONE;
}

ReturnCode nfcv_read_sysinfo(FuriHalNfcDevData* nfc_data, NfcVData* data) {
    uint8_t rxBuf[32];
    uint16_t received = 0;
    ReturnCode ret = ERR_NONE;

    FURI_LOG_D(TAG, "Read SYSTEM INFORMATION...");

    for(int tries = 0; tries < 5; tries++) {
        /* TODO: needs proper abstraction via fury_hal(_ll)_* */
        ret = rfalNfcvPollerGetSystemInformation(
            RFAL_NFCV_REQ_FLAG_DEFAULT, NULL, rxBuf, sizeof(rxBuf), &received);

        if(ret == ERR_NONE) {
            break;
        }
    }

    if(ret == ERR_NONE) {
        nfc_data->type = FuriHalNfcTypeV;
        nfc_data->uid_len = 8;
        /* UID is stored reversed in this response */
        for(int pos = 0; pos < nfc_data->uid_len; pos++) {
            nfc_data->uid[pos] = rxBuf[2 + (7 - pos)];
        }
        data->dsfid = rxBuf[10];
        data->afi = rxBuf[11];
        data->block_num = rxBuf[12] + 1;
        data->block_size = rxBuf[13] + 1;
        data->ic_ref = rxBuf[14];
        FURI_LOG_D(TAG, "  UID:          %02X %02X %02X %02X %02X %02X %02X %02X", 
            nfc_data->uid[0], nfc_data->uid[1], nfc_data->uid[2], nfc_data->uid[3], 
            nfc_data->uid[4], nfc_data->uid[5], nfc_data->uid[6], nfc_data->uid[7]);
        FURI_LOG_D(TAG, "  DSFID %d, AFI %d, Blocks %d, Size %d, IC Ref %d", data->dsfid, data->afi, data->block_num, data->block_size, data->ic_ref);
        return ret;
    }
    FURI_LOG_D(TAG, "Failed: %d", ret);

    return ret;
}

bool nfcv_read_card(
    NfcVReader* reader,
    FuriHalNfcDevData* nfc_data,
    NfcVData* nfcv_data) {
    furi_assert(reader);
    furi_assert(nfc_data);
    furi_assert(nfcv_data);

    if(nfcv_read_sysinfo(nfc_data, nfcv_data) != ERR_NONE) {
        return false;
    }

    if(nfcv_read_blocks(reader, nfcv_data) != ERR_NONE) {
        return false;
    }

    if(slix_check_card_type(nfc_data)) {
        FURI_LOG_I(TAG, "NXP SLIX detected");
        nfcv_data->type = NfcVTypeSlix;
    } else if(slix2_check_card_type(nfc_data)) {
        FURI_LOG_I(TAG, "NXP SLIX2 detected");
        nfcv_data->type = NfcVTypeSlix2;
    } else if(slix_s_check_card_type(nfc_data)) {
        FURI_LOG_I(TAG, "NXP SLIX-S detected");
        nfcv_data->type = NfcVTypeSlixS;
    } else if(slix_l_check_card_type(nfc_data)) {
        FURI_LOG_I(TAG, "NXP SLIX-L detected");
        nfcv_data->type = NfcVTypeSlixL;
    } else {
        nfcv_data->type = NfcVTypePlain;
    }

    return true;
}

/* emulation part */

static const uint32_t clocks_in_ms = 64 * 1000;
static const uint32_t bit_time = 64 * 9.44f;

DigitalSignal* nfcv_resp_pulse_32 = NULL;
DigitalSignal* nfcv_resp_unmod = NULL;
DigitalSignal* nfcv_resp_one = NULL;
DigitalSignal* nfcv_resp_zero = NULL;
DigitalSignal* nfcv_resp_sof = NULL;
DigitalSignal* nfcv_resp_eof = NULL;
DigitalSignal* nfcv_resp_unmod_256 = NULL;
DigitalSignal* nfcv_resp_unmod_768 = NULL;

DigitalSignal* nfcv_signal = NULL;


void nfcv_crc(uint8_t* data, uint32_t length, uint8_t* out) {
    uint32_t reg = 0xFFFF;
    uint32_t i = 0;
    uint32_t j = 0;

    for (i = 0; i < length; i++) {
        reg = reg ^ ((uint32_t)data[i]);
        for (j = 0; j < 8; j++) {
            if (reg & 0x0001) {
                reg = (reg >> 1) ^ 0x8408;
            } else {
                reg = (reg >> 1);
            }
        }
    }

    uint16_t crc = ~(uint16_t)(reg & 0xffff);

    out[0] = crc & 0xFF;
    out[1] = crc >> 8;
}

void nfcv_emu_free() {
    digital_signal_free(nfcv_signal);
    digital_signal_free(nfcv_resp_unmod_256);
    digital_signal_free(nfcv_resp_pulse_32);
    digital_signal_free(nfcv_resp_one);
    digital_signal_free(nfcv_resp_zero);
    digital_signal_free(nfcv_resp_sof);
    digital_signal_free(nfcv_resp_eof);

    nfcv_signal = NULL;
    nfcv_resp_unmod_256 = NULL;
    nfcv_resp_pulse_32 = NULL;
    nfcv_resp_one = NULL;
    nfcv_resp_zero = NULL;
    nfcv_resp_sof = NULL;
    nfcv_resp_eof = NULL;
}

void nfcv_emu_alloc() {
    
    if(!nfcv_signal) {
        nfcv_signal = digital_signal_alloc(8192);
    }

    if(!nfcv_resp_unmod_256) {
        /* unmodulated 256/fc signal as building block */
        nfcv_resp_unmod_256 = digital_signal_alloc(4);
        nfcv_resp_unmod_256->start_level = false;
        nfcv_resp_unmod_256->edge_timings[0] = (uint32_t)(NFCV_RESP_SUBC1_UNMOD_256 * DIGITAL_SIGNAL_UNIT_S);
        nfcv_resp_unmod_256->edge_cnt = 1;
    }
    if(!nfcv_resp_pulse_32) {
        /* modulated fc/32 pulse as building block */
        nfcv_resp_pulse_32 = digital_signal_alloc(4);
        nfcv_resp_pulse_32->start_level = true;
        nfcv_resp_pulse_32->edge_timings[0] = (uint32_t)(NFCV_RESP_SUBC1_PULSE_32 * DIGITAL_SIGNAL_UNIT_S);
        nfcv_resp_pulse_32->edge_timings[1] = (uint32_t)(NFCV_RESP_SUBC1_PULSE_32 * DIGITAL_SIGNAL_UNIT_S);
        nfcv_resp_pulse_32->edge_cnt = 2;
    }
    if(!nfcv_resp_one) {
        /* logical one: 256/fc unmodulated then 8 pulses fc/32 */
        nfcv_resp_one = digital_signal_alloc(24);
        digital_signal_append(nfcv_resp_one, nfcv_resp_unmod_256);
        for(size_t i = 0; i < 8; i++) {
            digital_signal_append(nfcv_resp_one, nfcv_resp_pulse_32);
        }
    }
    if(!nfcv_resp_zero) {
        /* logical zero: 8 pulses fc/32 then 256/fc unmodulated */
        nfcv_resp_zero = digital_signal_alloc(24);
        for(size_t i = 0; i < 8; i++) {
            digital_signal_append(nfcv_resp_zero, nfcv_resp_pulse_32);
        }
        digital_signal_append(nfcv_resp_zero, nfcv_resp_unmod_256);
    }
    if(!nfcv_resp_sof) {
        /* SOF: unmodulated 768/fc, 24 pulses fc/32, logic 1 */
        nfcv_resp_sof = digital_signal_alloc(128);
        digital_signal_append(nfcv_resp_sof, nfcv_resp_unmod_256);
        digital_signal_append(nfcv_resp_sof, nfcv_resp_unmod_256);
        digital_signal_append(nfcv_resp_sof, nfcv_resp_unmod_256);
        for(size_t i = 0; i < 24; i++) {
            digital_signal_append(nfcv_resp_sof, nfcv_resp_pulse_32);
        }
        digital_signal_append(nfcv_resp_sof, nfcv_resp_one);
    }
    if(!nfcv_resp_eof) {
        /* EOF: logic 0, 24 pulses fc/32, unmodulated 768/fc */
        nfcv_resp_eof = digital_signal_alloc(128);
        digital_signal_append(nfcv_resp_eof, nfcv_resp_zero);
        for(size_t i = 0; i < 24; i++) {
            digital_signal_append(nfcv_resp_eof, nfcv_resp_pulse_32);
        }
        digital_signal_append(nfcv_resp_eof, nfcv_resp_unmod_256);
        digital_signal_append(nfcv_resp_eof, nfcv_resp_unmod_256);
        digital_signal_append(nfcv_resp_eof, nfcv_resp_unmod_256);
        /* add extra silence */
        digital_signal_append(nfcv_resp_eof, nfcv_resp_unmod_256);
    }
}


void nfcv_emu_send_raw(uint8_t* data, uint8_t length) {
    
    int bits = length * 8;

    nfcv_signal->start_level = false;
    nfcv_signal->edge_cnt = 0;

    digital_signal_append(nfcv_signal, nfcv_resp_sof);

    for(int bit_total = 0; bit_total < bits; bit_total++) {
        uint32_t byte_pos = bit_total / 8;
        uint32_t bit_pos = bit_total % 8;
        uint8_t bit_val = 0x01 << bit_pos;

        if(data[byte_pos] & bit_val) {
            digital_signal_append(nfcv_signal, nfcv_resp_one);
        } else {
            digital_signal_append(nfcv_signal, nfcv_resp_zero);
        }
    }

    digital_signal_append(nfcv_signal, nfcv_resp_eof);

    /* digital signal setup will take some time. win some time by tricking the VCD into thinking that something happens */
    furi_hal_gpio_write(&gpio_spi_r_mosi, false);
    furi_delay_us(10);
    furi_hal_gpio_write(&gpio_spi_r_mosi, true);
    furi_delay_us(10);
    furi_hal_gpio_write(&gpio_spi_r_mosi, false);
    
    FURI_CRITICAL_ENTER();
    digital_signal_send(nfcv_signal, &gpio_spi_r_mosi);
    FURI_CRITICAL_EXIT();
    furi_hal_gpio_write(&gpio_spi_r_mosi, false);
}

void nfcv_emu_send(uint8_t* data, uint8_t length) {
    uint8_t buffer[64];

    if(length + 2 > (uint8_t)sizeof(buffer)) {
        return;
    }

    memcpy(buffer, data, length);
    nfcv_crc(buffer, length, &buffer[length]);
    nfcv_emu_send_raw(buffer, length + 2);
}


void nfcv_uidcpy(uint8_t *dst, uint8_t *src) {
    for(int pos = 0; pos < 8; pos++) {
        dst[pos] = src[7-pos];
    }
}

int nfcv_uidcmp(uint8_t *dst, uint8_t *src) {
    for(int pos = 0; pos < 8; pos++) {
        if(dst[pos] != src[7-pos]) {
            return 1;
        }
    }
    return 0;
}

uint32_t nfcv_read_le(uint8_t *data, uint32_t length) {
    uint32_t value = 0;

    for(uint32_t pos = 0; pos < length; pos++) {
        value |= data[pos] << ((int)pos * 8);
    }

    return value;
}

uint32_t nfcv_read_be(uint8_t *data, uint32_t length) {
    uint32_t value = 0;

    for(uint32_t pos = 0; pos < length; pos++) {
        value <<= 8;
        value |= data[pos];
    }

    return value;
}

void nfcv_emu_handle_packet(FuriHalNfcDevData* nfc_data, NfcVData* nfcv_data, uint8_t* payload, uint32_t payload_length) {

    if(!payload_length) {
        return;
    }
    
    uint8_t flags = payload[0];
    uint8_t command = payload[1];
    bool addressed = !(flags & RFAL_NFCV_REQ_FLAG_INVENTORY) && (flags & RFAL_NFCV_REQ_FLAG_ADDRESS);
    bool advanced = (command >= 0xA0);
    uint8_t address_offset = 2 + (advanced ? 1 : 0);
    uint8_t payload_offset = address_offset + (addressed ? 8 : 0);
    uint8_t *address = &payload[address_offset];

    if(addressed && nfcv_uidcmp(address, nfc_data->uid)) {
        printf("addressed packet, but not for us:\r\n");
        printf(" destination: %02X%02X%02X%02X%02X%02X%02X%02X\r\n", address[7], address[6], address[5], address[4], address[3], address[2], address[1], address[0]);
        printf(" our UID:     %02X%02X%02X%02X%02X%02X%02X%02X\r\n", nfc_data->uid[0], nfc_data->uid[1], nfc_data->uid[2], nfc_data->uid[3], nfc_data->uid[4], nfc_data->uid[5], nfc_data->uid[6], nfc_data->uid[7]);
        return;
    }

    uint8_t response_buffer[32];

    switch(nfcv_data->type) {
        case NfcVTypeSlixL:
            if(nfcv_data->sub_data.slix_l.privacy && 
                command != ISO15693_CMD_NXP_GET_RANDOM_NUMBER && 
                command != ISO15693_CMD_NXP_SET_PASSWORD) {
                snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "command 0x%02X ignored, privacy mode", command);
                FURI_LOG_D(TAG, "%s", nfcv_data->last_command);
                return;
            }
            break;

        default:
            break;
    }
    
    switch(command) {

        case ISO15693_INVENTORY:
        {
            response_buffer[0] = ISO15693_NOERROR;
            response_buffer[1] = nfcv_data->dsfid; /* DSFID */
            nfcv_uidcpy(&response_buffer[2], nfc_data->uid);

            nfcv_emu_send(response_buffer, 10);
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "INVENTORY");
            break;
        }

        case ISO15693_STAYQUIET:
        {
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "STAYQUIET");
            break;
        }

        case ISO15693_LOCKBLOCK:
        {
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "LOCKBLOCK");
            break;
        }

        case ISO15693_READ_MULTI_BLOCK:
        {
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "READ_MULTI_BLOCK");
            break;
        }

        case ISO15693_WRITE_MULTI_BLOCK:
        {
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "WRITE_MULTI_BLOCK");
            break;
        }

        case ISO15693_SELECT:
        {
            response_buffer[0] = ISO15693_NOERROR;
            nfcv_emu_send(response_buffer, 1);
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "SELECT");
            break;
        }

        case ISO15693_READBLOCK:
        {
            uint8_t block = payload[payload_offset];

            if(block >= nfcv_data->block_num) {
                response_buffer[0] = ISO15693_ERROR_BLOCK_WRITE;
                nfcv_emu_send(response_buffer, 1);
            } else {
                response_buffer[0] = ISO15693_NOERROR;
                memcpy(&response_buffer[1], &nfcv_data->data[nfcv_data->block_size * block], nfcv_data->block_size);
                nfcv_emu_send(response_buffer, 1 + nfcv_data->block_size);
            }
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "READ BLOCK %d", block);
            break;
        }

        case ISO15693_WRITEBLOCK:
        {
            uint8_t block = payload[payload_offset];
            uint8_t *data = &payload[payload_offset + 1];

            if(block >= nfcv_data->block_num) {
                response_buffer[0] = ISO15693_ERROR_BLOCK_WRITE;
            } else {
                response_buffer[0] = ISO15693_NOERROR;
                memcpy(&nfcv_data->data[nfcv_data->block_size * block], &response_buffer[1], nfcv_data->block_size);
            }
            nfcv_emu_send(response_buffer, 1);
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "WRITE BLOCK %d <- %02X %02X %02X %02X", block, data[0], data[1], data[2], data[3]);
            break;
        }

        case ISO15693_GET_SYSTEM_INFO:
        {
            response_buffer[0] = ISO15693_NOERROR; 
            response_buffer[1] = 0x0F;
            nfcv_uidcpy(&response_buffer[2], nfc_data->uid);
            response_buffer[10] = nfcv_data->dsfid; /* DSFID */
            response_buffer[11] = nfcv_data->afi; /* AFI */
            response_buffer[12] = nfcv_data->block_num - 1; /* number of blocks */
            response_buffer[13] = nfcv_data->block_size - 1; /* block size */
            response_buffer[14] = nfcv_data->ic_ref; /* IC reference */

            nfcv_emu_send(response_buffer, 15);
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "SYSTEMINFO");

            break;
        }

        case ISO15693_CMD_NXP_GET_RANDOM_NUMBER:
        {
            nfcv_data->sub_data.slix_l.rand[0] = 0x00;
            nfcv_data->sub_data.slix_l.rand[1] = 0x00;

            response_buffer[0] = ISO15693_NOERROR;
            response_buffer[1] = nfcv_data->sub_data.slix_l.rand[1];
            response_buffer[2] = nfcv_data->sub_data.slix_l.rand[0];

            nfcv_emu_send(response_buffer, 3);
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "GET_RANDOM_NUMBER");
            break;
        }

        case ISO15693_CMD_NXP_SET_PASSWORD:
        {
            uint8_t password_id = payload[payload_offset];
            uint8_t *password_xored = &payload[payload_offset + 1];
            uint8_t *rand = nfcv_data->sub_data.slix_l.rand;
            uint8_t status = ISO15693_ERROR_GENERIC;
            uint8_t *password = NULL;
            uint8_t password_rcv[4];

            switch(password_id) {
                case 4:
                    password = nfcv_data->sub_data.slix_l.key_privacy;
                    break;
                case 8:
                    password = nfcv_data->sub_data.slix_l.key_destroy;
                    break;
                case 10:
                    password = nfcv_data->sub_data.slix_l.key_eas;
                    break;
                default:
                    break;
            }

            for(int pos = 0; pos < 4; pos++) {
                password_rcv[pos] = password_xored[3 - pos] ^ rand[pos % 2];
            }
            uint32_t pass_expect = nfcv_read_be(password, 4);
            uint32_t pass_received = nfcv_read_be(password_rcv, 4);

            if(pass_expect == pass_received) {
                status = ISO15693_NOERROR;
                nfcv_data->sub_data.slix_l.privacy = false;
            } else {
                FURI_LOG_D(TAG, "Password #%d mismatch. Expected 0x%08lX, got 0x%08lX", password_id, pass_expect, pass_received);
            }

            response_buffer[0] = status;

            nfcv_emu_send(response_buffer, 1);
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "SET_PASSWORD #%02X", password_id);

            break;
        }

        case ISO15693_CMD_NXP_ENABLE_PRIVACY:
        {
            response_buffer[0] = ISO15693_NOERROR;

            nfcv_emu_send(response_buffer, 1);
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "ISO15693_CMD_NXP_ENABLE_PRIVACY");

            nfcv_data->sub_data.slix_l.privacy = true;
            break;
        }

        default:
            snprintf(nfcv_data->last_command, sizeof(nfcv_data->last_command), "unsupported: %02X", command);
            break;
    }
}

#define COUNT(x) ((sizeof(x))/(sizeof(x[0])))
uint32_t nfcv_timer_buffer_src[32];
uint32_t nfcv_timer_buffer[1024];
volatile uint32_t nfcv_gpio_calls = 0;

void nfcv_gpio_cb(void* ctx) {
    UNUSED(ctx);

    nfcv_gpio_calls++;
}

void nfcv_emu_init(FuriHalNfcDevData* nfc_data, NfcVData* nfcv_data) {
    nfcv_emu_alloc();
    rfal_platform_spi_acquire();

    st25r3916ExecuteCommand(ST25R3916_CMD_STOP);
    st25r3916WriteRegister(ST25R3916_REG_OP_CONTROL, 0xC3);
    st25r3916WriteRegister(ST25R3916_REG_MODE, 0x88);
    st25r3916ExecuteCommand(ST25R3916_CMD_TRANSPARENT_MODE);

    furi_hal_spi_bus_handle_deinit(&furi_hal_spi_bus_handle_nfc);

    
    FURI_LOG_D(TAG, "Starting NfcV emulation");
    FURI_LOG_D(TAG, "  UID:          %02X %02X %02X %02X %02X %02X %02X %02X", 
        nfc_data->uid[0], nfc_data->uid[1], nfc_data->uid[2], nfc_data->uid[3], 
        nfc_data->uid[4], nfc_data->uid[5], nfc_data->uid[6], nfc_data->uid[7]);
    FURI_LOG_D(TAG, "  Card type:    %d", nfcv_data->type);
    FURI_LOG_D(TAG, "  Privacy pass: 0x%08lX", nfcv_read_be(nfcv_data->sub_data.slix_l.key_privacy, 4));
    FURI_LOG_D(TAG, "  Privacy mode: %s", nfcv_data->sub_data.slix_l.privacy ? "ON" : "OFF");

    memset(nfcv_timer_buffer_src, 0xEE, sizeof(nfcv_timer_buffer_src));
    memset(nfcv_timer_buffer, 0xFA, sizeof(nfcv_timer_buffer));

    /* configure DMA to read from a timer peripheral */
    LL_DMA_InitTypeDef dma_config = {};
    dma_config.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) &(TIM2->CNT);
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstAddress = (uint32_t) nfcv_timer_buffer;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.NbData = COUNT(nfcv_timer_buffer); /* executes LL_DMA_SetDataLength */
    dma_config.PeriphRequest = LL_DMAMUX_REQ_GENERATOR0; /* executes LL_DMA_SetPeriphRequest */
    dma_config.Priority = LL_DMA_PRIORITY_VERYHIGH;

    /* now set up DMA with these settings */
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_4, &dma_config);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
    
    /* make some noise on the counter */
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    LL_TIM_SetAutoReload(TIM2, 0xFFFFFFFF);
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);
    
    /* make sure request generation is disabled before modifying registers */
    LL_DMAMUX_DisableRequestGen(NULL, LL_DMAMUX_REQ_GEN_0);
    /* generator 0 gets fed by EXTI_LINE4 */
    LL_DMAMUX_SetRequestSignalID(NULL, LL_DMAMUX_REQ_GEN_0, LL_DMAMUX_REQ_GEN_EXTI_LINE4);
    /* trigger on any edge */
    LL_DMAMUX_SetRequestGenPolarity(NULL, LL_DMAMUX_REQ_GEN_0, LL_DMAMUX_REQ_GEN_POL_RISING);
    /* now enable request generation again */
    LL_DMAMUX_EnableRequestGen(NULL, LL_DMAMUX_REQ_GEN_0);

    /* we need the EXTI to be configured as interrupt generating line, but no ISR registered */
    furi_hal_gpio_init_ex(&gpio_spi_r_miso, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedVeryHigh, GpioAltFnUnused);

}

void nfcv_emu_deinit() {
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_nfc);
    rfal_platform_spi_release();
    nfcv_emu_free();
}

bool nfcv_emu_loop(FuriHalNfcDevData* nfc_data, NfcVData* nfcv_data, uint32_t timeout_ms) {
    
    bool ret = false;
    uint32_t next_sleep = DWT->CYCCNT + (timeout_ms * clocks_in_ms);
    uint32_t timeout = 0;
    uint32_t last_change = 0;
    uint32_t edges_received = 0;

    uint32_t frame_state = NFCV_FRAME_STATE_SOF1;
    uint32_t periods_previous = 0;
    uint8_t frame_payload[128];
    uint32_t frame_pos = 0;
    uint32_t byte_value = 0;
    uint32_t bits_received = 0;
    char reset_reason[128];

    bool prev = furi_hal_gpio_read(&gpio_spi_r_miso);

    bool in_critical = false;
    FURI_CRITICAL_DEFINE();


    while(true) {

        bool state = furi_hal_gpio_read(&gpio_spi_r_miso);
        uint32_t cur_time = DWT->CYCCNT;

        if(state != prev) {
            uint32_t delta = cur_time - last_change;
            uint32_t periods = (delta + bit_time/2) / bit_time;

            last_change = cur_time;
            prev = state;
            next_sleep = cur_time + (timeout_ms * clocks_in_ms);

            /* start edge counting on first rising edge */
            if(0 && (state || edges_received)) {
                edges_received++;

                /* ignore periods which are too long, might happen on field start */
                if(periods > 1024) {
                    continue;
                }

                switch(frame_state) {
                    case NFCV_FRAME_STATE_SOF1:
                        /* got a rising edge, was it one period? */
                        if(state) {
                            if(periods == 1) {
                                FURI_CRITICAL_ENTER_ADV();
                                in_critical = true;
                                timeout = cur_time + bit_time * 16;
                                frame_state = NFCV_FRAME_STATE_SOF2;
                            } else {
                                snprintf(reset_reason, sizeof(reset_reason), "SOF: Expected 1 period, got %lu", periods);
                                frame_state = NFCV_FRAME_STATE_RESET;
                                break;
                            }
                        }
                        break;

                    case NFCV_FRAME_STATE_SOF2:
                        /* waiting for the second low period, telling us about coding */
                        if(!state) {
                            timeout = cur_time + bit_time * 16;
                            if(periods == 6) {
                                frame_state = NFCV_FRAME_STATE_CODING_256;
                                periods_previous = 0;
                            } else if(periods == 4) {
                                frame_state = NFCV_FRAME_STATE_CODING_4;
                                periods_previous = 2;
                            } else {
                                snprintf(reset_reason, sizeof(reset_reason), "SOF: Expected 4/6 periods, got %lu", periods);
                                frame_state = NFCV_FRAME_STATE_RESET;
                                break;
                            }
                        }
                        break;

                    case NFCV_FRAME_STATE_CODING_256:
                        if(!state) {
                            timeout = cur_time + bit_time * 1024;
                            if(periods_previous > periods) {
                                snprintf(reset_reason, sizeof(reset_reason), "1oo256: Missing %lu periods from previous symbol, got %lu", periods_previous, periods);
                                frame_state = NFCV_FRAME_STATE_RESET;
                                break;
                            }
                            /* previous symbol left us with some pulse periods */
                            periods -= periods_previous;

                            if(periods > 512) {
                                snprintf(reset_reason, sizeof(reset_reason), "1oo256: %lu periods is too much", periods);
                                frame_state = NFCV_FRAME_STATE_RESET;
                                break;
                            }
                            
                            if(periods == 2) {
                                frame_state = NFCV_FRAME_STATE_EOF;
                                break;
                            } 
                            
                            periods_previous = 512 - (periods + 1);
                            byte_value = (periods - 1) / 2;
                            frame_payload[frame_pos++] = (uint8_t)byte_value;

                        } else {
                            if(periods != 1) {
                                snprintf(reset_reason, sizeof(reset_reason), "1oo256: Expected a single low pulse");
                                frame_state = NFCV_FRAME_STATE_RESET;
                                break;
                            }
                        }
                        break;

                    case NFCV_FRAME_STATE_CODING_4:
                        /* evaluate high periods on falling edge */
                        if(!state) {
                            timeout = cur_time + bit_time * 16;
                            if(periods_previous > periods) {
                                snprintf(reset_reason, sizeof(reset_reason), "1oo4: Missing %lu periods from previous symbol, got %lu", periods_previous, periods);
                                frame_state = NFCV_FRAME_STATE_RESET;
                                break;
                            }
                            /* previous symbol left us with some pulse periods */
                            periods -= periods_previous;
                            periods_previous = 0;

                            byte_value >>= 2;
                            bits_received += 2;

                            if(periods == 1) {
                                byte_value |= 0x00 << 6;
                                periods_previous = 6;
                            } else if(periods == 3) {
                                byte_value |= 0x01 << 6;
                                periods_previous = 4;
                            } else if(periods == 5) {
                                byte_value |= 0x02 << 6;
                                periods_previous = 2;
                            } else if(periods == 7) {
                                byte_value |= 0x03 << 6;
                                periods_previous = 0;
                            } else if(periods == 2) {
                                frame_state = NFCV_FRAME_STATE_EOF;
                                break;
                            } else {
                                snprintf(reset_reason, sizeof(reset_reason), "1oo4: Expected 1/3/5/7 low pulses, but got %lu", periods);
                                frame_state = NFCV_FRAME_STATE_RESET;
                                break;
                            }

                            if(bits_received >= 8) {
                                frame_payload[frame_pos++] = (uint8_t)byte_value;
                                bits_received = 0;
                            }
                        } else {
                            if(periods != 1) {
                                snprintf(reset_reason, sizeof(reset_reason), "1oo4: Expected a single low pulse");
                                frame_state = NFCV_FRAME_STATE_RESET;
                                break;
                            }
                        }
                        break;
                }
            }
        }

        /* post-state-machine cleanup and reset */
        if(frame_state == NFCV_FRAME_STATE_RESET) {
            timeout = 0;
            edges_received = 0;
            frame_state = NFCV_FRAME_STATE_SOF1;
            FURI_CRITICAL_EXIT();
            printf("Reset state machine, reason: %s\r\n", reset_reason);
            in_critical = false;
            furi_delay_ms(50);
        } else if(frame_state == NFCV_FRAME_STATE_EOF) {
            FURI_CRITICAL_EXIT();
            in_critical = false;
            break;
        }

        /* no edges detected */
        if(timeout && cur_time > timeout) {
            break;
        }

        /* might exit early on overflows. guess thats okay. */
        if(cur_time > next_sleep) {
            break;
        }
    }

    if(in_critical) {
        FURI_CRITICAL_EXIT();
        in_critical = false;
    }

    if(frame_state == NFCV_FRAME_STATE_EOF) {

        furi_hal_gpio_write(&gpio_spi_r_mosi, false);
        furi_delay_us(10);
        furi_hal_gpio_write(&gpio_spi_r_mosi, true);
        furi_delay_us(10);
        furi_hal_gpio_write(&gpio_spi_r_mosi, false);
        furi_delay_us(10);
        furi_hal_gpio_write(&gpio_spi_r_mosi, true);
        furi_delay_us(10);
        furi_hal_gpio_write(&gpio_spi_r_mosi, false);

        nfcv_emu_handle_packet(nfc_data, nfcv_data, frame_payload, frame_pos);
        ret = true;
    }

    printf("edges_received:    %lu\r\n", edges_received);
    printf("nfcv_gpio_calls:   %lu\r\n", nfcv_gpio_calls);
    printf("nfcv_timer_buffer: %ld", LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_4));

    uint32_t prev_timer = 0;
    for(uint32_t pos = 0; pos < COUNT(nfcv_timer_buffer) - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_4); pos++) {
        if((pos % 4) == 0) {
            printf("\r\n");
        }
        printf(" 0x%08lX ", (nfcv_timer_buffer[pos] - prev_timer));

        prev_timer = nfcv_timer_buffer[pos];
    }
    printf("\r\n");
    //furi_delay_ms(100);
    

    return ret;
}
