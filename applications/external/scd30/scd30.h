#pragma once

// Credit: Adafruit SCD30 library

#include "i2c.h"
#include "scd30_defs.h"

struct SCD30 : private I2C {
    bool begun = false;
    float carbonDioxide = 0.f;
    float temperature = 0.f;
    float relativeHumidity = 0.f;

    using I2C::I2C;
    SCD30();

    bool begin(int32_t sensor_id = 0);
    bool beginIfNecessary();

    void resetData();
    bool readData();

    /** Performs a software reset initializing registers to their power on state. */
    void resetSensor();

    /** Asks the sensor whether new data is ready to read. */
    bool dataReady();

    /** Set the amount of time between measurements in seconds. Must be from 2 to
	 *  1800 seconds. The default value set on sensor initialization is 2 seconds. */
    bool setMeasurementInterval(uint16_t interval);

    /** Reads the current amount of time between measurements in seconds. */
    uint16_t getMeasurementInterval();

    /** Gets the enabled status of the SCD30's self calibration routine. */
    bool selfCalibrationEnabled();

    /** Enable or disable the SCD30's self calibration routine. */
    bool selfCalibrationEnabled(bool enabled);

    /** @brief Tell the SCD30 to start taking measurements continuously
	 *  @param pressure an optional pressure offset to correct for in millibar (mBar)
	 *  @return true: success false: failure */
    bool startContinuousMeasurement(uint16_t pressure = 0);

    /** Read the current ambient pressure offset in millibar. */
    uint16_t getAmbientPressureOffset();

    /** Sets the altitude offset (in meters above sea level) that the SCD30 should correct for.
	 *  This value is saved to the SCD30's internal storage and is reloaded on sensor power up. */
    bool setAltitudeOffset(uint16_t altitude);

    /** Get the current altitude offset value in meters above sea level. */
    uint16_t getAltitudeOffset();

    /**
	 * @brief Set a temperature offset
	 *
	 * @param temp_offset The **positive** temperature offset to set in hundreths of
	 * a degree C ie:
	 *
	 * 1015 => 10.15 degrees C
	 * 31337 => 313.37 degrees C
	 *
	 * **Note:** This value is saved to the SCD30's internal storage and is reloaded
	 * on sensor power up.
	 * @return true: success false: failure
	 */
    bool setTemperatureOffset(uint16_t temp_offset);

    /** Gets the current temperature offset in hundreths of a degree C. */
    uint16_t getTemperatureOffset();

    /**
	 * @brief Force the SCD30 to recalibrate with a given reference value
	 *
	 * @param reference The calibration reference value in ppm from
	 * 400-2000 ppm.
	 *
	 * **Note:** This value is saved to the SCD30's internal storage and is reloaded
	 * on sensor power up.
	 *
	 * **Setting a reference value and forcing recalibration will override any
	 * previous automatic self-calibration.**
	 * @return true: success false: failure
	 */
    bool forceRecalibrationWithReference(uint16_t reference);

    /**
	 * @brief Get the current forced recalibration reference value
	 * @return uint16_t The current reference value in ppm
	 */
    uint16_t getForcedCalibrationReference();

private:
    int8_t interfaceResult = SCD30_OK;
    uint8_t chipID = 0;
    uint16_t sensorIDHumidity = 0;
    uint16_t sensorIDTemp = 0;

    bool init(int32_t sensor_id);
    bool sendCommand(uint16_t command);
    bool sendCommand(uint16_t command, uint16_t argument);
    uint16_t readRegister(uint16_t reg_addr);
    uint32_t millis() const;
};
