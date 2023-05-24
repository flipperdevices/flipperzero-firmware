#include "rad_sens_i2c.h"
#include "rad_sens.h"

bool rad_sens_read_data(RadSensModel* model) {
    furi_hal_i2c_acquire(I2C_BUS);

    uint32_t timeout = furi_ms_to_ticks(100);
    model->connected = false;
    model->verified = false;

    if(furi_hal_i2c_is_device_ready(I2C_BUS, RAD_SENS_ADDRESS, timeout) > 0) {
        model->connected = true;

        uint8_t buffer[4];
        uint8_t device_id = 0;

        buffer[0] = RAD_SENS_ID_RG;
        if(furi_hal_i2c_tx(I2C_BUS, RAD_SENS_ADDRESS, buffer, 1, timeout)) {
            if(furi_hal_i2c_rx(I2C_BUS, (uint8_t)RAD_SENS_ADDRESS, buffer, 1, timeout)) {
                device_id = buffer[0];
            }
        }

        if(device_id == RAD_SENS_ID) {
            model->verified = true;

            // Read dynamic intensity
            buffer[0] = RAD_SENS_DYN_INTENSITY_RG;
            if(furi_hal_i2c_tx(I2C_BUS, RAD_SENS_ADDRESS, buffer, 1, timeout)) {
                if(furi_hal_i2c_rx(I2C_BUS, (uint8_t)RAD_SENS_ADDRESS, buffer, 3, timeout)) {
                    model->dyn_intensity =
                        (((uint32_t)buffer[0] << 16) | ((uint32_t)buffer[1] << 8) |
                         (uint32_t)buffer[2]);
                }
            }

            // Read static intensity
            buffer[0] = RAD_SENS_STAT_INTENSITY_RG;
            if(furi_hal_i2c_tx(I2C_BUS, RAD_SENS_ADDRESS, buffer, 1, timeout)) {
                if(furi_hal_i2c_rx(I2C_BUS, (uint8_t)RAD_SENS_ADDRESS, buffer, 3, timeout)) {
                    model->stat_intensity =
                        (((uint32_t)buffer[0] << 16) | ((uint32_t)buffer[1] << 8) |
                         (uint32_t)buffer[2]);
                }
            }

            // Read impulses
            buffer[0] = RAD_SENS_IMP_CNT_RG;
            if(furi_hal_i2c_tx(I2C_BUS, RAD_SENS_ADDRESS, buffer, 1, timeout)) {
                if(furi_hal_i2c_rx(I2C_BUS, (uint8_t)RAD_SENS_ADDRESS, buffer, 2, timeout)) {
                    model->new_impulse_count = (((uint16_t)buffer[0] << 8) | (uint16_t)buffer[1]);
                    model->impulse_count += model->new_impulse_count;
                }
            }
        }
    }

    furi_hal_i2c_release(I2C_BUS);

    return model->verified;
}
