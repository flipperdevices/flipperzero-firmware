#include "imu.h"
#include "../../lib/lsm6ds3tr-api/lsm6ds3tr-c_reg.h"

#define LSM6DS3_TAG "LSM6DS3"
#define LSM6DS3_DEV_ADDRESS (0x6A << 1)

stmdev_ctx_t lsm6ds3trc_ctx;

int32_t lsm6ds3trc_write_i2c(void* handle, uint8_t reg_addr, const uint8_t* data, uint16_t len) {
    if(furi_hal_i2c_write_mem(handle, LSM6DS3_DEV_ADDRESS, reg_addr, (uint8_t*)data, len, 50))
        return 0;
    return -1;
}

int32_t lsm6ds3trc_read_i2c(void* handle, uint8_t reg_addr, uint8_t* read_data, uint16_t len) {
    if(furi_hal_i2c_read_mem(handle, LSM6DS3_DEV_ADDRESS, reg_addr, read_data, len, 50)) return 0;
    return -1;
}

bool lsm6ds3trc_begin() {
    FURI_LOG_I(LSM6DS3_TAG, "Init LSM6DS3TR-C");

    if(!furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, LSM6DS3_DEV_ADDRESS, 50)) {
        FURI_LOG_E(LSM6DS3_TAG, "Not ready");
        return false;
    }

    lsm6ds3trc_ctx.write_reg = lsm6ds3trc_write_i2c;
    lsm6ds3trc_ctx.read_reg = lsm6ds3trc_read_i2c;
    lsm6ds3trc_ctx.mdelay = furi_delay_ms;
    lsm6ds3trc_ctx.handle = &furi_hal_i2c_handle_external;

    uint8_t whoami;
    lsm6ds3tr_c_device_id_get(&lsm6ds3trc_ctx, &whoami);
    if(whoami != LSM6DS3TR_C_ID) {
        FURI_LOG_I(LSM6DS3_TAG, "Unknown model: %x", (int)whoami);
        return false;
    }

    lsm6ds3tr_c_reset_set(&lsm6ds3trc_ctx, PROPERTY_ENABLE);
    uint8_t rst = PROPERTY_ENABLE;
    while(rst) lsm6ds3tr_c_reset_get(&lsm6ds3trc_ctx, &rst);

    lsm6ds3tr_c_block_data_update_set(&lsm6ds3trc_ctx, PROPERTY_ENABLE);
    lsm6ds3tr_c_fifo_mode_set(&lsm6ds3trc_ctx, LSM6DS3TR_C_BYPASS_MODE);

    lsm6ds3tr_c_xl_data_rate_set(&lsm6ds3trc_ctx, LSM6DS3TR_C_XL_ODR_104Hz);
    lsm6ds3tr_c_xl_full_scale_set(&lsm6ds3trc_ctx, LSM6DS3TR_C_4g);
    lsm6ds3tr_c_xl_lp1_bandwidth_set(&lsm6ds3trc_ctx, LSM6DS3TR_C_XL_LP1_ODR_DIV_4);

    lsm6ds3tr_c_gy_data_rate_set(&lsm6ds3trc_ctx, LSM6DS3TR_C_GY_ODR_104Hz);
    lsm6ds3tr_c_gy_full_scale_set(&lsm6ds3trc_ctx, LSM6DS3TR_C_2000dps);
    lsm6ds3tr_c_gy_power_mode_set(&lsm6ds3trc_ctx, LSM6DS3TR_C_GY_HIGH_PERFORMANCE);
    lsm6ds3tr_c_gy_band_pass_set(&lsm6ds3trc_ctx, LSM6DS3TR_C_LP2_ONLY);

    FURI_LOG_I(LSM6DS3_TAG, "Init OK");
    return true;
}

void lsm6ds3trc_end() {
    lsm6ds3tr_c_xl_data_rate_set(&lsm6ds3trc_ctx, LSM6DS3TR_C_XL_ODR_OFF);
    lsm6ds3tr_c_gy_data_rate_set(&lsm6ds3trc_ctx, LSM6DS3TR_C_GY_ODR_OFF);
}

int lsm6ds3trc_read(double* vec) {
    int ret = 0;
    int16_t data[3];
    lsm6ds3tr_c_reg_t reg;
    lsm6ds3tr_c_status_reg_get(&lsm6ds3trc_ctx, &reg.status_reg);

    if(reg.status_reg.xlda) {
        lsm6ds3tr_c_acceleration_raw_get(&lsm6ds3trc_ctx, data);
        vec[1] = (double)lsm6ds3tr_c_from_fs2g_to_mg(data[0]) / 1000;
        vec[0] = (double)lsm6ds3tr_c_from_fs2g_to_mg(data[1]) / 1000;
        vec[2] = -(double)lsm6ds3tr_c_from_fs2g_to_mg(data[2]) / 1000;
        ret |= ACC_DATA_READY;
    }

    if(reg.status_reg.gda) {
        lsm6ds3tr_c_angular_rate_raw_get(&lsm6ds3trc_ctx, data);
        vec[4] = (double)lsm6ds3tr_c_from_fs2000dps_to_mdps(data[0]) * DEG_TO_RAD / 1000;
        vec[3] = (double)lsm6ds3tr_c_from_fs2000dps_to_mdps(data[1]) * DEG_TO_RAD / 1000;
        vec[5] = -(double)lsm6ds3tr_c_from_fs2000dps_to_mdps(data[2]) * DEG_TO_RAD / 1000;
        ret |= GYR_DATA_READY;
    }

    return ret;
}

struct imu_t imu_lsm6ds3trc =
    {LSM6DS3_DEV_ADDRESS, lsm6ds3trc_begin, lsm6ds3trc_end, lsm6ds3trc_read, LSM6DS3_TAG};
