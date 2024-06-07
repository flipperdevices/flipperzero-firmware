#include "imu.h"
#include "../../lib/lsm6dso-api/lsm6dso_reg.h"

#define LSM6DSO_TAG "LSM6DO"
#define LSM6DSO_DEV_ADDRESS (0x6B << 1)

stmdev_ctx_t lsm6dso_ctx;

int32_t lsm6dso_write_i2c(void* handle, uint8_t reg_addr, uint8_t* data, uint16_t len) {
    if(furi_hal_i2c_write_mem(handle, LSM6DSO_DEV_ADDRESS, reg_addr, data, len, 50)) return 0;
    return -2;
}

int32_t lsm6dso_read_i2c(void* handle, uint8_t reg_addr, uint8_t* read_data, uint16_t len) {
    if(furi_hal_i2c_read_mem(handle, LSM6DSO_DEV_ADDRESS, reg_addr, read_data, len, 50)) return 0;
    return -2;
}

bool lsm6dso_begin() {
    FURI_LOG_I(LSM6DSO_TAG, "Init LSM6DSOTR-C");

    if(!furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, LSM6DSO_DEV_ADDRESS, 50)) {
        FURI_LOG_E(LSM6DSO_TAG, "Not ready");
        return false;
    }

    lsm6dso_ctx.write_reg = lsm6dso_write_i2c;
    lsm6dso_ctx.read_reg = lsm6dso_read_i2c;
    lsm6dso_ctx.mdelay = furi_delay_ms;
    lsm6dso_ctx.handle = &furi_hal_i2c_handle_external;

    uint8_t whoami;
    lsm6dso_device_id_get(&lsm6dso_ctx, &whoami);
    if(whoami != LSM6DSO_ID) {
        FURI_LOG_I(LSM6DSO_TAG, "Unknown model: %x", (int)whoami);
        return false;
    }

    lsm6dso_reset_set(&lsm6dso_ctx, PROPERTY_ENABLE);
    uint8_t rst = PROPERTY_ENABLE;
    while(rst) lsm6dso_reset_get(&lsm6dso_ctx, &rst);

    lsm6dso_block_data_update_set(&lsm6dso_ctx, PROPERTY_ENABLE);
    lsm6dso_fifo_mode_set(&lsm6dso_ctx, LSM6DSO_BYPASS_MODE);

    lsm6dso_xl_data_rate_set(&lsm6dso_ctx, LSM6DSO_XL_ODR_104Hz);
    lsm6dso_xl_full_scale_set(&lsm6dso_ctx, LSM6DSO_4g);
    //lsm6dso_xl_lp1_bandwidth_set(&lsm6dso_ctx, LSM6DSO_XL_LP1_ODR_DIV_4);

    lsm6dso_gy_data_rate_set(&lsm6dso_ctx, LSM6DSO_GY_ODR_104Hz);
    lsm6dso_gy_full_scale_set(&lsm6dso_ctx, LSM6DSO_2000dps);
    lsm6dso_gy_power_mode_set(&lsm6dso_ctx, LSM6DSO_GY_HIGH_PERFORMANCE);
    //lsm6dso_gy_band_pass_set(&lsm6dso_ctx, LSM6DSO_LP2_ONLY);

    FURI_LOG_I(LSM6DSO_TAG, "Init OK");
    return true;
}

void lsm6dso_end() {
    lsm6dso_xl_data_rate_set(&lsm6dso_ctx, LSM6DSO_XL_ODR_OFF);
    lsm6dso_gy_data_rate_set(&lsm6dso_ctx, LSM6DSO_GY_ODR_OFF);
}

int lsm6dso_read(double* vec) {
    int ret = 0;
    int16_t data[3];
    lsm6dso_reg_t reg;
    lsm6dso_status_reg_get(&lsm6dso_ctx, &reg.status_reg);

    if(reg.status_reg.xlda) {
        lsm6dso_acceleration_raw_get(&lsm6dso_ctx, data);
        vec[2] = (double)lsm6dso_from_fs2_to_mg(data[0]) / 1000;
        vec[0] = (double)lsm6dso_from_fs2_to_mg(data[1]) / 1000;
        vec[1] = (double)lsm6dso_from_fs2_to_mg(data[2]) / 1000;
        ret |= ACC_DATA_READY;
    }

    if(reg.status_reg.gda) {
        lsm6dso_angular_rate_raw_get(&lsm6dso_ctx, data);
        vec[5] = (double)lsm6dso_from_fs2000_to_mdps(data[0]) * DEG_TO_RAD / 1000;
        vec[3] = (double)lsm6dso_from_fs2000_to_mdps(data[1]) * DEG_TO_RAD / 1000;
        vec[4] = (double)lsm6dso_from_fs2000_to_mdps(data[2]) * DEG_TO_RAD / 1000;
        ret |= GYR_DATA_READY;
    }

    return ret;
}

struct imu_t imu_lsm6dso =
    {LSM6DSO_DEV_ADDRESS, lsm6dso_begin, lsm6dso_end, lsm6dso_read, LSM6DSO_TAG};
