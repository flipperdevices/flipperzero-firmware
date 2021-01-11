#include "blanks_writer.h"

class RW1990_1 {
public:
    constexpr static const uint8_t CMD_WRITE_RECORD_FLAG = 0xD1;
    constexpr static const uint8_t CMD_READ_RECORD_FLAG = 0xB5;
    constexpr static const uint8_t CMD_WRITE_ROM = 0xD5;
};
class RW1990_2 {
public:
    constexpr static const uint8_t CMD_WRITE_RECORD_FLAG = 0x1D;
    constexpr static const uint8_t CMD_READ_RECORD_FLAG = 0x1E;
    constexpr static const uint8_t CMD_WRITE_ROM = 0xD5;
};
class TM2004 {
public:
    constexpr static const uint8_t CMD_READ_STATUS = 0xAA;
    constexpr static const uint8_t CMD_READ_MEMORY = 0xF0;
    constexpr static const uint8_t CMD_WRITE_ROM = 0x3C;
    constexpr static const uint8_t CMD_FINALIZATION = 0x35;

    constexpr static const uint8_t ANSWER_READ_MEMORY = 0xF5;
};

class TM01 {
public:
    constexpr static const uint8_t CMD_WRITE_RECORD_FLAG = 0xC1;
    constexpr static const uint8_t CMD_WRITE_ROM = 0xC5;
    constexpr static const uint8_t CMD_SWITCH_TO_CYFRAL = 0xCA;
    constexpr static const uint8_t CMD_SWITCH_TO_METAKOM = 0xCB;
};

class DS1990 {
public:
    constexpr static const uint8_t CMD_READ_ROM = 0x33;
};

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

extern "C" {
extern void handle_uart_write(const void* data, size_t size, void* ctx);
}

void low_level_printf(const char* format, ...) {
    char buffer[128];

    va_list args;
    va_start(args, format);
    size_t size = vsnprintf(buffer, 128, format, args);
    va_end(args);

    handle_uart_write(buffer, size, NULL);
}

void BlanksWriter::onewire_release(void) {
    gpio_write(gpio, false);
}

void BlanksWriter::onewire_write_one_bit(bool value, uint32_t delay = 10000) {
    onewire->write_bit(value);
    delay_us(delay);
    onewire_release();
}

BlanksWriter::BlanksWriter(const GpioPin* one_wire_gpio) {
    gpio = one_wire_gpio;
    onewire = new OneWireGpio(gpio);
}

BlanksWriter::~BlanksWriter() {
}

BlankType BlanksWriter::get_blank_type() {
    uint8_t data;
    bool presence;
    BlankType result = BLANK_UNKNOWN;

    // check for device on bus
    osKernelLock();
    presence = onewire->reset();
    osKernelUnlock();

    if(presence) {
        osKernelLock();

        // test for RW-1990.1
        if(result == BLANK_UNKNOWN) {
            // disable record flag
            onewire->reset();
            onewire->write(RW1990_1::CMD_WRITE_RECORD_FLAG);
            onewire_write_one_bit(1);

            // read record flag
            onewire->reset();
            onewire->write(RW1990_1::CMD_READ_RECORD_FLAG);
            data = onewire->read();

            // if record flag disabled
            if(data == ((uint8_t)~0x01)) {
                // this is RW1990.1
                result = BLANK_RW1990_1;
            }
        }

        // test for RW-1990.2
        if(result == BLANK_UNKNOWN) {
            // enable record flag
            onewire->reset();
            onewire->write(RW1990_2::CMD_WRITE_RECORD_FLAG);
            onewire_write_one_bit(1);

            // read record flag
            onewire->reset();
            onewire->write(RW1990_2::CMD_READ_RECORD_FLAG);
            data = onewire->read();

            // if record flag enabled
            if(data == ((uint8_t)~0x01)) {
                // disable record flag
                onewire->reset();
                onewire->write(RW1990_2::CMD_WRITE_RECORD_FLAG);
                onewire_write_one_bit(0);

                // this is RW1990.2
                result = BLANK_RW1990_2;
            }
        }

        // test for TM-2004
        if(result == BLANK_UNKNOWN) {
            // read rom, to enable 3-byte mode
            onewire->reset();
            onewire->write(DS1990::CMD_READ_ROM);

            // read key data
            for(uint8_t i = 0; i < 8; i++) {
                uint8_t key_data = onewire->read();
            };

            // read blank status, addr is 0x0000
            // data is cmd + addr high + addr low
            uint8_t cmd_data[3] = {TM2004::CMD_READ_STATUS, 0x00, 0x00};

            onewire->write(cmd_data[0]);
            onewire->write(cmd_data[1]);
            onewire->write(cmd_data[2]);

            // read answer (crc of acquired command)
            data = onewire->read();

            // if answer is correct
            if(maxim_crc8(cmd_data, 3) == data) {
                // read status
                // TODO: what we can do with it?
                data = onewire->read();

                // this is TM2004
                result = BLANK_TM2004;
            }
        }

        if(result == BLANK_UNKNOWN) {
            onewire->reset();

            // unknown type, maybe TM-01
            result = BLANK_TM01;
        }

        // reset bus afterwards
        onewire->reset();

        osKernelUnlock();
    }

    return result;
}

bool BlanksWriter::write(KeyType type, const uint8_t* key, uint8_t key_length) {
    BlankType blank_type = get_blank_type();
    bool same_key;
    uint8_t write_result = -1;

    switch(blank_type) {
    case BLANK_UNKNOWN:
        //printf("unknown\n");
        break;
    case BLANK_RW1990_1:
        printf("RW-1990.1\n");
        break;
    case BLANK_RW1990_2:
        printf("RW-1990.2\n");
        break;
    case BLANK_TM2004:
        printf("TM2004\n");
        break;
    case BLANK_TM01:
        printf("unknown, maybe TM-01\n");
        break;
    }

    switch(type) {
    case KeyType::KEY_DS1990:
        same_key = compare_key_ds1990(key, key_length);
        break;
    default:
        break;
    }

    if(!same_key) {
        switch(blank_type) {
        case BLANK_UNKNOWN:
            break;
        case BLANK_RW1990_1:
            write_result = write_1990_1(key, key_length);
            break;
        case BLANK_RW1990_2:
            write_result = write_1990_2(key, key_length);
            break;
        case BLANK_TM2004:
            write_result = write_TM2004(key, key_length);
            break;
        case BLANK_TM01:
            //write_result = write_TM01(type, key, key_length);
            write_result = write_1990_2(key, key_length);
            break;
        }

        /*switch(blank_type) {
        case BLANK_UNKNOWN:
            break;
        case BLANK_RW1990_1:
        case BLANK_RW1990_2:
        case BLANK_TM2004:
        case BLANK_TM01:
            if(write_result != 1) {
                write_result = write_1990_1(key, key_length);
            }
            if(write_result != 1) {
                write_result = write_1990_2(key, key_length);
            }
            if(write_result != 1) {
                write_result = write_TM2004(key, key_length);
            }
            if(write_result != 1) {
                write_result = write_TM01(type, key, key_length);
            }
            break;
        }*/

        if(write_result == 1) {
            printf("key writed\n");
        } else if(write_result == 0) {
            printf("cannot write key\n");
        }
    } else {
        printf("key are same\n");
        write_result = 1;
    }

    return (write_result == 1);
}

bool BlanksWriter::write_TM2004(const uint8_t* key, uint8_t key_length) {
    uint8_t answer;
    bool result = true;

    osKernelLock();

    // write rom, addr is 0x0000
    onewire->reset();
    onewire->write(TM2004::CMD_WRITE_ROM);
    onewire->write(0x00);
    onewire->write(0x00);

    // write key
    for(uint8_t i = 0; i < key_length; i++) {
        // write key byte
        onewire->write(key[i]);
        answer = onewire->read();
        // TODO: check answer CRC

        // pulse indicating that data is correct
        delay_us(600);
        onewire_write_one_bit(1, 50000);

        // read writed key byte
        answer = onewire->read();

        // check that writed and readed are same
        if(key[i] != answer) {
            result = false;
            break;
        }
    }
    osKernelUnlock();

    return result;
}

bool BlanksWriter::write_1990_1(const uint8_t* key, uint8_t key_length) {
    bool result = true;

    osKernelLock();

    // unlock
    onewire->reset();
    onewire->write(RW1990_1::CMD_WRITE_RECORD_FLAG);
    onewire_write_one_bit(0);

    // write key
    onewire->reset();
    onewire->write(RW1990_1::CMD_WRITE_ROM);
    for(uint8_t i = 0; i < key_length; i++) {
        // inverted key for RW1990.1
        write_byte_ds1990(~key[i]);
    }

    // lock
    onewire->write(RW1990_1::CMD_WRITE_RECORD_FLAG);
    onewire_write_one_bit(1);

    if(!compare_key_ds1990(key, key_length)) {
        result = false;
    }

    osKernelUnlock();

    return result;
}

bool BlanksWriter::write_1990_2(const uint8_t* key, uint8_t key_length) {
    bool result = true;

    osKernelLock();

    // unlock
    onewire->reset();
    onewire->write(RW1990_2::CMD_WRITE_RECORD_FLAG);
    onewire_write_one_bit(1);

    // write key
    onewire->reset();
    onewire->write(RW1990_2::CMD_WRITE_ROM);
    for(uint8_t i = 0; i < key_length; i++) {
        write_byte_ds1990(key[i]);
    }

    // lock
    onewire->write(RW1990_2::CMD_WRITE_RECORD_FLAG);
    onewire_write_one_bit(0);

    if(!compare_key_ds1990(key, key_length)) {
        result = false;
    }

    osKernelUnlock();

    return result;
}

bool BlanksWriter::write_TM01(KeyType type, const uint8_t* key, uint8_t key_length) {
    bool result = true;

    osKernelLock();

    // unlock
    onewire->reset();
    onewire->write(TM01::CMD_WRITE_RECORD_FLAG);
    onewire_write_one_bit(1, 10000);

    // write key
    onewire->reset();
    onewire->write(TM01::CMD_WRITE_ROM);

    //if(type == KEY_METAKOM || type == KEY_CYFRAL) {
    //} else {
    for(uint8_t i = 0; i < key_length; i++) {
        write_byte_ds1990(key[i]);
        delay_us(10000);
    }
    //}

    // lock
    onewire->write(TM01::CMD_WRITE_RECORD_FLAG);
    onewire_write_one_bit(0, 10000);

    if(!compare_key_ds1990(key, key_length)) {
        result = false;
    }

    if(type == KEY_METAKOM || type == KEY_CYFRAL) {
        onewire->reset();
        if(type == KEY_CYFRAL)
            onewire->write(TM01::CMD_SWITCH_TO_CYFRAL);
        else
            onewire->write(TM01::CMD_SWITCH_TO_METAKOM);
        onewire_write_one_bit(1);
    }

    osKernelUnlock();

    return result;
}

void BlanksWriter::write_byte_ds1990(uint8_t data) {
    for(uint8_t n_bit = 0; n_bit < 8; n_bit++) {
        //onewire_write_one_bit(data & 1, 10000);
        onewire->write_bit(data & 1);
        delay_us(5000);
        data = data >> 1;
    }
    onewire_release();
}

bool BlanksWriter::compare_key_ds1990(const uint8_t* key, uint8_t key_length) {
    uint8_t buff[key_length];
    bool result = false;

    if(onewire->reset()) {
        onewire->write(DS1990::CMD_READ_ROM);
        onewire->read_bytes(buff, key_length);

        bool keys_are_same = true;
        for(uint8_t i = 0; i < 8; i++) {
            if(key[i] != buff[i]) {
                keys_are_same = false;
                break;
            }
        }

        for(uint8_t i = 0; i < 8; i++) {
            low_level_printf("%02x ", key[i]);
        }
        low_level_printf("\n");

        for(uint8_t i = 0; i < 8; i++) {
            low_level_printf("%02x ", buff[i]);
        }
        low_level_printf("\n");

        if(keys_are_same) result = true;
    }
    return result;
}

void BlanksWriter::start() {
    onewire->start();
}

void BlanksWriter::stop() {
    onewire->stop();
}