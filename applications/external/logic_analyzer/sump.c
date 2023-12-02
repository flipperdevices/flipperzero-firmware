
#include <furi.h>

#include "sump.h"

void sump_handle_query(Sump* sump) {
    sump->tx_data(sump->tx_data_ctx, (uint8_t*)"1ALS", 4);
}

void sump_handle_get_metadata(Sump* sump) {
    uint8_t buf[128];
    size_t pos = 0;

    const char* name = "Flipper LogicAnalyzer v1.0 (g3gg0.de)";
    const char* fpga = "(none)";
    const char* firmware = "v1.0";
    const uint8_t probes = 8;
    uint32_t max_sample_rate = 10000000;
    uint32_t max_sample_mem = MAX_SAMPLE_MEM;

    /* 0x01 	device name (e.g. "Openbench Logic Sniffer v1.0", "Bus Pirate v3b"  */
    buf[pos++] = 0x01;
    strcpy((char*)&buf[pos], name);
    pos += strlen(name) + 1;

    /* 0x02 	Version of the FPGA firmware */
    buf[pos++] = 0x02;
    strcpy((char*)&buf[pos], fpga);
    pos += strlen(fpga) + 1;

    /* 0x03 	Ancillary version (PIC firmware) */
    buf[pos++] = 0x03;
    strcpy((char*)&buf[pos], firmware);
    pos += strlen(firmware) + 1;

    /* 0x40	Number of usable probes (short) */
    buf[pos++] = 0x40;
    buf[pos++] = probes;

    /* 0x41 	Protocol version (short) */
    buf[pos++] = 0x41;
    buf[pos++] = 0x02;

    /* 0x21 	Amount of sample memory available (bytes) */
    buf[pos++] = 0x21;
    buf[pos++] = (max_sample_mem >> 24) & 0xFF;
    buf[pos++] = (max_sample_mem >> 16) & 0xFF;
    buf[pos++] = (max_sample_mem >> 8) & 0xFF;
    buf[pos++] = (max_sample_mem >> 0) & 0xFF;

    /* 0x23 	Maximum sample rate (hz)  */
    buf[pos++] = 0x23;
    buf[pos++] = (max_sample_rate >> 24) & 0xFF;
    buf[pos++] = (max_sample_rate >> 16) & 0xFF;
    buf[pos++] = (max_sample_rate >> 8) & 0xFF;
    buf[pos++] = (max_sample_rate >> 0) & 0xFF;

    /* 0x00 	not used, key means end of metadata*/
    buf[pos++] = 0x00;

    sump->tx_data(sump->tx_data_ctx, buf, pos);
}

uint32_t get_word(uint8_t* data) {
    return (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | (data[0] << 0);
}

size_t sump_handle(Sump* sump, uint8_t* data, size_t length) {
    size_t pos = 0;

    while(pos < length) {
        uint8_t command = data[pos];
        uint32_t extra = 0;

        if(command & 0x80) {
            if(length - pos < 5) {
                return pos;
            }
            pos++;
            extra = get_word(&data[pos]);
            pos += 4;
        } else {
            pos++;
        }

        switch(command) {
        case SUMP_CMD_RESET:
            sump->armed = false;
            break;

        case SUMP_CMD_ARM:
            sump->armed = true;
            break;

        case SUMP_CMD_QUERY_ID:
            sump_handle_query(sump);
            break;

        case SUMP_CMD_SELF_TEST:
            break;

        case SUMP_CMD_GET_METADATA:
            sump_handle_get_metadata(sump);
            break;

        case SUMP_CMD_FINISH_NOW:
            sump->armed = false;
            break;

        case SUMP_CMD_XON:
            break;

        case SUMP_CMD_XOFF:
            break;

        case SUMP_CMD_SET_READ_DELAY_COUNT:
            sump->read_count = 4 * ((extra >> 16) + 1);
            sump->delay_count = 4 * ((extra & 0xFFFF) + 1);
            break;

        case SUMP_CMD_SET_FLAGS:
            sump->flags = (extra >> 24);
            break;

        case SUMP_CMD_SET_DIVIDER:
            sump->divider = extra;
            break;

        case SUMP_CMD_TRIGGER_MASK:
            sump->trig_mask = extra;
            break;

        case SUMP_CMD_TRIGGER_VALUES:
            sump->trig_values = extra;
            break;

        case SUMP_CMD_TRIGGER_CONFIG:
            sump->trig_delay = (extra >> 16);
            sump->trig_config = (extra & 0xFFFF);
            break;

        default:
            break;
        }
    }

    return pos;
}

Sump* sump_alloc() {
    Sump* sump = malloc(sizeof(Sump));

    return sump;
}

void sump_free(Sump* sump) {
    free(sump);
}
