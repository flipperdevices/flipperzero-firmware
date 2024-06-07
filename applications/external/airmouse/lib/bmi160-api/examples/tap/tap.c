/**
 * Copyright (C) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*********************************************************************/
/* system header files */
/*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*********************************************************************/
/* own header files */
/*********************************************************************/
#include "coines.h"
#include "bmi160.h"

/*********************************************************************/
/* local macro definitions */
/*! i2c interface communication, 1 - Enable; 0- Disable */
#define BMI160_INTERFACE_I2C  0

/*! spi interface communication, 1 - Enable; 0- Disable */
#define BMI160_INTERFACE_SPI  1

#if (!((BMI160_INTERFACE_I2C == 1) && (BMI160_INTERFACE_SPI == 0)) && \
    (!((BMI160_INTERFACE_I2C == 0) && (BMI160_INTERFACE_SPI == 1))))
#error "Invalid value given for the macros BMI160_INTERFACE_I2C / BMI160_INTERFACE_SPI"
#endif

/*! bmi160 shuttle id */
#define BMI160_SHUTTLE_ID     0x38

/*! bmi160 Device address */
#define BMI160_DEV_ADDR       BMI160_I2C_ADDR

/*********************************************************************/
/* global variables */
/*********************************************************************/

/*! @brief This structure containing relevant bmi160 info */
struct bmi160_dev bmi160dev;

/*! @brief variable to hold the bmi160 accel data */
struct bmi160_sensor_data bmi160_accel;

/*! @brief variable to hold the bmi160 gyro data */
struct bmi160_sensor_data bmi160_gyro;

/*********************************************************************/
/* static function declarations */
/*********************************************************************/

/*!
 * @brief   internal API is used to initialize the sensor interface
 */
static void init_sensor_interface(void);

/*!
 * @brief   This internal API is used to initialize the bmi160 sensor with default
 */
static void init_bmi160(void);

/*!
 * @brief   This internal API is used to initialize the sensor driver interface
 */
static void init_bmi160_sensor_driver_interface(void);

/*!
 * @brief   This internal API is used to set tap configurations
 */
static int8_t set_tap_config(uint8_t feature_enable);

/*********************************************************************/
/* functions */
/*********************************************************************/

/*!
 *  @brief This internal API is used to initialize the sensor interface depending
 *   on selection either SPI or I2C.
 *
 *  @param[in] void
 *
 *  @return void
 *
 */
static void init_sensor_interface(void)
{
    /* Switch VDD for sensor off */
    coines_set_shuttleboard_vdd_vddio_config(0, 0);

    /* wait until the sensor goes off */
    coines_delay_msec(10);
#if BMI160_INTERFACE_I2C == 1

    /* SDO pin is made low for selecting I2C address 0x68 */
    coines_set_pin_config(COINES_SHUTTLE_PIN_15, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_LOW);

    /* set the sensor interface as I2C */
    coines_config_i2c_bus(COINES_I2C_BUS_0, COINES_I2C_FAST_MODE);
    coines_delay_msec(10);

    /* CSB pin is made high for selecting I2C protocol*/
    coines_set_pin_config(COINES_SHUTTLE_PIN_7, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);
#endif
#if BMI160_INTERFACE_SPI == 1

    /* CSB pin is made low for selecting SPI protocol*/
    coines_set_pin_config(COINES_SHUTTLE_PIN_7, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_LOW);

    coines_delay_msec(10);
    coines_config_spi_bus(COINES_SPI_BUS_0, COINES_SPI_SPEED_5_MHZ, COINES_SPI_MODE3);
#endif
    coines_delay_msec(10);

    /* Switch VDD for sensor on */
    coines_set_shuttleboard_vdd_vddio_config(3300, 3300);

#if BMI160_INTERFACE_SPI == 1
    coines_delay_msec(10);

    /* CSB pin is made high for selecting SPI protocol
     * Note: CSB has to see rising after power up, to switch to SPI protocol */
    coines_set_pin_config(COINES_SHUTTLE_PIN_7, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);
#endif
}

/*!
 *  @brief This internal API is used to initializes the bmi160 sensor
 *  settings like power mode and OSRS settings.
 *
 *  @param[in] void
 *
 *  @return void
 *
 */
static void init_bmi160(void)
{
    int8_t rslt;

    rslt = bmi160_init(&bmi160dev);

    if (rslt == BMI160_OK)
    {
        printf("BMI160 initialization success !\n");
        printf("Chip ID 0x%X\n", bmi160dev.chip_id);
    }
    else
    {
        printf("BMI160 initialization failure !\n");
        exit(COINES_E_FAILURE);
    }

    /* Select the Output data rate, range of accelerometer sensor */
    bmi160dev.accel_cfg.odr = BMI160_ACCEL_ODR_100HZ;
    bmi160dev.accel_cfg.range = BMI160_ACCEL_RANGE_8G;
    bmi160dev.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;

    /* Select the power mode of accelerometer sensor */
    bmi160dev.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

    /* Set the sensor configuration */
    rslt = bmi160_set_sens_conf(&bmi160dev);
}

/*!
 *  @brief This internal API is used to set the sensor driver interface to
 *  read/write the data.
 *
 *  @param[in] void
 *
 *  @return void
 *
 */
static void init_bmi160_sensor_driver_interface(void)
{
#if BMI160_INTERFACE_I2C == 1

    /* I2C setup */

    /* link read/write/delay function of host system to appropriate
     * bmi160 function call prototypes */
    bmi160dev.write = coines_write_i2c;
    bmi160dev.read = coines_read_i2c;
    bmi160dev.delay_ms = coines_delay_msec;

    /* set correct i2c address */
    bmi160dev.id = BMI160_DEV_ADDR;
    bmi160dev.intf = BMI160_I2C_INTF;
#endif
#if BMI160_INTERFACE_SPI == 1

    /* SPI setup */

    /* link read/write/delay function of host system to appropriate
     *  bmi160 function call prototypes */
    bmi160dev.write = coines_write_spi;
    bmi160dev.read = coines_read_spi;
    bmi160dev.delay_ms = coines_delay_msec;
    bmi160dev.id = COINES_SHUTTLE_PIN_7;
    bmi160dev.intf = BMI160_SPI_INTF;
#endif
}

/*!
 *  @brief Main Function where the execution getting started to test the code.
 *
 *  @param[in] argc
 *  @param[in] argv
 *
 *  @return status
 *
 */
int main(int argc, char *argv[])
{
    struct coines_board_info board_info;
    int16_t rslt;

    init_bmi160_sensor_driver_interface();

    rslt = coines_open_comm_intf(COINES_COMM_INTF_USB);

    if (rslt < 0)
    {
        printf(
            "\n Unable to connect with Application Board ! \n" " 1. Check if the board is connected and powered on. \n" " 2. Check if Application Board USB driver is installed. \n"
            " 3. Check if board is in use by another application. (Insufficient permissions to access USB) \n");
        exit(rslt);
    }

    rslt = coines_get_board_info(&board_info);

    if (rslt == COINES_SUCCESS)
    {
        if (board_info.shuttle_id != BMI160_SHUTTLE_ID)
        {

            printf("! Warning invalid sensor shuttle \n ," "This application will not support this sensor \n");
            exit(COINES_E_FAILURE);
        }
    }

    init_sensor_interface();

    /* after sensor init introduce 200 msec sleep */
    coines_delay_msec(200);
    init_bmi160();

    rslt = set_tap_config(BMI160_ENABLE);
    if (rslt == BMI160_OK)
    {
        union bmi160_int_status int_status;
        uint8_t loop = 0;
        uint32_t last_time = 0;
        uint32_t current_time = 0;

        printf("Do Single or Double Tap the board\n");
        fflush(stdout);

        memset(int_status.data, 0x00, sizeof(int_status.data));

        while (loop < 10)
        {
            /* Read interrupt status */
            rslt = bmi160_get_int_status(BMI160_INT_STATUS_ALL, &int_status, &bmi160dev);
            current_time = coines_get_millis();

            /* Enters only if the obtained interrupt is single-tap */
            if (rslt == BMI160_OK)
            {
                /* Enters only if the obtained interrupt is single-tap */
                if (int_status.bit.s_tap)
                {
                    printf("Single tap, iter:%d, time:%d ms, delta:%d ms, int_status:0x%x\n",
                           loop++,
                           current_time,
                           current_time - last_time,
                           int_status.data[0]);
                }
                /* Enters only if the obtained interrupt is double-tap */
                else if (int_status.bit.d_tap)
                {
                    printf("Double tap, iter:%d, time:%d ms, delta:%d ms, int_status:0x%x\n",
                           loop++,
                           current_time,
                           current_time - last_time,
                           int_status.data[0]);
                }

                fflush(stdout);
            }
            else
            {
                break;
            }

            memset(int_status.data, 0x00, sizeof(int_status.data));
            last_time = current_time;
        }

        /* Disable tap feature */
        printf("\nDisable tap test...\n");
        rslt = set_tap_config(BMI160_DISABLE);
        printf("bmi160_set_int_config(tap enable) status:%d\n", rslt);

        fflush(stdout);
    }

    coines_close_comm_intf(COINES_COMM_INTF_USB);

    return EXIT_SUCCESS;
}

static int8_t set_tap_config(uint8_t feature_enable)
{
    int8_t rslt = BMI160_OK;
    struct bmi160_int_settg int_config;

    if (feature_enable > 0)
    {
        /* Select the Interrupt channel/pin */
        int_config.int_channel = BMI160_INT_CHANNEL_1; /* Interrupt channel/pin 1 */

        /* Select the interrupt channel/pin settings */
        int_config.int_pin_settg.output_en = BMI160_ENABLE; /* Enabling interrupt pins to act as output pin */
        int_config.int_pin_settg.output_mode = BMI160_DISABLE; /* Choosing push-pull mode for interrupt pin */
        int_config.int_pin_settg.output_type = BMI160_ENABLE; /* Choosing active low output */
        int_config.int_pin_settg.edge_ctrl = BMI160_DISABLE; /* Choosing edge triggered output */
        int_config.int_pin_settg.input_en = BMI160_DISABLE; /* Disabling interrupt pin to act as input */
        int_config.int_pin_settg.latch_dur = BMI160_LATCH_DUR_NONE; /* non-latched output */

        /* Select the Interrupt type */
        int_config.int_type = BMI160_ACC_SINGLE_TAP_INT; /* Choosing tap interrupt */

        /* Select the Any-motion interrupt parameters */
        int_config.int_type_cfg.acc_tap_int.tap_en = BMI160_ENABLE; /* 1- Enable tap, 0- disable tap */
        int_config.int_type_cfg.acc_tap_int.tap_thr = 2; /* Set tap threshold */
        int_config.int_type_cfg.acc_tap_int.tap_dur = 2; /* Set tap duration */
        int_config.int_type_cfg.acc_tap_int.tap_shock = 0; /* Set tap shock value */
        int_config.int_type_cfg.acc_tap_int.tap_quiet = 0; /* Set tap quiet duration */
        int_config.int_type_cfg.acc_tap_int.tap_data_src = 1; /* data source 0 : filter or 1 : pre-filter */

        /* Set the Any-motion interrupt */
        rslt = bmi160_set_int_config(&int_config, &bmi160dev); /* sensor is an instance of the structure bmi160_dev  */
        printf("bmi160_set_int_config(tap enable) status:%d\n", rslt);
    }
    else
    {
        /* Select the Interrupt channel/pin */
        int_config.int_channel = BMI160_INT_CHANNEL_1;
        int_config.int_pin_settg.output_en = BMI160_DISABLE; /* Disabling interrupt pins to act as output pin */
        int_config.int_pin_settg.edge_ctrl = BMI160_DISABLE; /* Choosing edge triggered output */

        /* Select the Interrupt type */
        int_config.int_type = BMI160_ACC_SINGLE_TAP_INT; /* Choosing Tap interrupt */
        int_config.int_type_cfg.acc_tap_int.tap_en = BMI160_DISABLE; /* 1- Enable tap, 0- disable tap */

        /* Set the Data ready interrupt */
        rslt = bmi160_set_int_config(&int_config, &bmi160dev); /* sensor is an instance of the structure bmi160_dev */
        printf("bmi160_set_int_config(tap disable) status:%d\n", rslt);
    }

    return rslt;
}
