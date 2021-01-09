#pragma once
#include "one_wire_gpio.h"
#include "maxim_crc.h"

typedef enum {
    BLANK_UNKNOWN, /**< unknown blank type */
    BLANK_TM01, /**< ТМ-01 */
    BLANK_RW1990_1, /**< RW1990.1 */
    BLANK_RW1990_2, /**< RW1990.2 */
    BLANK_TM2004, /**< ТМ2004 */
} BlankType;

class BlanksWriter {
private:
    const GpioPin* gpio;
    OneWireGpio* onewire;

    void onewire_release(void);
    void onewire_write_one_bit(bool value, uint32_t delay);

public:
    BlanksWriter(const GpioPin* one_wire_gpio);
    ~BlanksWriter();

    BlankType get_blank_type();
    bool write(const uint8_t* key, uint8_t key_length);
    
    bool write_TM2004(const uint8_t* key, uint8_t key_length);
    bool write_1990_1(const uint8_t* key, uint8_t key_length);
};
