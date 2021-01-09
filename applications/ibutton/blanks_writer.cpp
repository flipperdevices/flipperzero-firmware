#include "blanks_writer.h"

class RW1990_1 {
public:
    constexpr static const uint8_t CMD_WRITE_RECORD_FLAG = 0xD1;
    constexpr static const uint8_t CMD_READ_RECORD_FLAG = 0xB5;
};

class RW1990_2 {
public:
    constexpr static const uint8_t CMD_WRITE_RECORD_FLAG = 0x1D;
    constexpr static const uint8_t CMD_READ_RECORD_FLAG = 0x1E;
};

class TM2004 {
public:
    constexpr static const uint8_t CMD_READ_STATUS = 0x1D;
    constexpr static const uint8_t CMD_WRITE_ROM = 0x3C;
};

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
            onewire->write(0x33);

            // read key data
            for(uint8_t i = 0; i < 8; i++) onewire->read();

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
                onewire->reset();

                // this is TM2004
                result = BLANK_TM2004;
            }
        }

        if(result == BLANK_UNKNOWN) {
            onewire->reset();

            // unknown type, maybe TM-01
            result = BLANK_TM01;
        }

        osKernelUnlock();
    }

    return result;
}

bool BlanksWriter::write(const uint8_t* key, uint8_t key_length) {
    BlankType result = get_blank_type();

    switch(result) {
    case BLANK_UNKNOWN:
        printf("unknown\n");
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

    uint8_t write_result = -1;

    switch(result) {
    case BLANK_UNKNOWN:
        break;
    case BLANK_RW1990_1:
        break;
    case BLANK_RW1990_2:
        break;
    case BLANK_TM2004:
        write_result = write_TM2004(key, key_length);
        break;
    case BLANK_TM01:
        break;
    }

    if(write_result == 1) {
        printf("key writed\n");
    } else if(write_result == 0) {
        printf("cannot write key\n");
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
    uint8_t answer;
    bool result = true;

    osKernelLock();

    osKernelUnlock();

    return result;
}