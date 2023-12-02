#include <furi_hal_i2c.h>

// Rad Sens address
#define RAD_SENS_ADDRESS (0x66 << 1)
#define RAD_SENS_ID_RG 0x00
#define RAD_SENS_ID 0x7D
#define RAD_SENS_DYN_INTENSITY_RG 0x03
#define RAD_SENS_STAT_INTENSITY_RG 0x06
#define RAD_SENS_IMP_CNT_RG 0x09

// External I2C BUS
#define I2C_BUS &furi_hal_i2c_handle_external

typedef struct RadSensModel RadSensModel;
bool rad_sens_read_data(RadSensModel* model);