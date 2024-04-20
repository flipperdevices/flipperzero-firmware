#include <stdint.h>
#include <furi.h>
#include <furi_hal_i2c.h>
#include <furi_hal_resources.h>

#define I2C_BUS &furi_hal_i2c_handle_external
#define I2C_TIMEOUT 50

typedef struct {
    // the current frequency in khz / 10
    uint16_t freq;
    // idk what the rest means
    uint8_t dBuV;
    uint8_t ant_cap;
    uint8_t noise_level;
} SI4713TuneStatus;

typedef struct {
    uint16_t firmware;
    uint16_t patch;
    uint16_t cmp;
    uint8_t product_num;
    uint8_t revision;
} SI4713DeviceInfo;

typedef struct {
    // the address of the device, usually 0x63
    // it gets shifted to the right 1 bit because
    // for whatever reason the flipper wants me to
    // (something something 7 bit addresses)
    uint8_t address;
    // the pin for the reset line, i use pin 14
    // (yes, it works, use &gpio_usart_rx)
    const GpioPin* reset_pin;
    union {
        // the device normally only responds with
        // two bytes so this is safe most of the time,
        // if your response needs more than two bytes
        // just use si4713_trx_command()
        uint8_t result[2];
        // the circuitpython library mentioned that having a
        // 10 byte buffer reduced heap fragmentation and
        // allocations so i'm doing it here too
        uint8_t buffer[10];
    };
    SI4713DeviceInfo info;
    SI4713TuneStatus status;
} SI4713Device;

// sends an array of 8 bit commands of length size
void si4713_send_command(SI4713Device* device, const uint8_t* commands, uint8_t length);

// sends an array of 8 bit commands of length size and anticipates
// rx_length 8 bit numbers to return
void si4713_trx_command(
    SI4713Device* device,
    const uint8_t* commands,
    uint8_t length,
    uint8_t* rx,
    uint8_t rx_length);

void si4713_set_property(SI4713Device* device, uint16_t property, uint16_t value);

uint8_t si4713_get_status(SI4713Device* device);

void si4713_update_info(SI4713Device* device);

void si4713_update_status(SI4713Device* device);

void si4713_reset(SI4713Device* device);

void si4713_powerup(SI4713Device* device);

void si4713_set_tx_power(SI4713Device* device, uint8_t power, uint8_t antcap);

void si4713_tune_fm(SI4713Device* device, uint16_t freq);

SI4713Device* si4713_alloc(uint8_t addr, const GpioPin* reset, bool initialize);

void si4713_free(SI4713Device* device, bool reset_low);