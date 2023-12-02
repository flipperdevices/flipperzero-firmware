#pragma once

// Credit: Adafruit SCD30 library

#include <stdint.h>

#define SCD30_I2CADDR_DEFAULT 0x61 ///< SCD30 default i2c address
#define SCD30_CHIP_ID 0x60 ///< SCD30 default device id from WHOAMI

#define SCD30_WHOAMI 0xd100 ///< Chip ID register

#define SCD30_CMD_READ_MEASUREMENT 0x0300 ///< Main data register

#define SCD30_CMD_CONTINUOUS_MEASUREMENT 0x0010 ///< Command to start continuous measurement
#define SCD30_CMD_STOP_MEASUREMENTS 0x0104 ///< Command to stop measurements
#define SCD30_CMD_SET_MEASUREMENT_INTERVAL 0x4600 ///< Command to set measurement interval
#define SCD30_CMD_GET_DATA_READY 0x0202 ///< Data ready reg
#define SCD30_CMD_AUTOMATIC_SELF_CALIBRATION 0x5306 ///< enables/disables auto calibration
#define SCD30_CMD_SET_FORCED_RECALIBRATION_REF 0x5204 ///< Forces calibration with given value
#define SCD30_CMD_SET_TEMPERATURE_OFFSET 0x5403 ///< Specifies the temp offset
#define SCD30_CMD_SET_ALTITUDE_COMPENSATION 0x5102 ///< Specifies altitude offset
#define SCD30_CMD_SOFT_RESET 0xD304 ///< Soft reset!
#define SCD30_CMD_READ_REVISION 0xD100 ///< Firmware revision number

#define SCD30_FAILURE -1
#define SCD30_OK 0