// Credit: Adafruit SCD30 library

#include "scd30.h"
#include "scd30_logging.h"

static uint8_t crc8(const uint8_t* data, int len);

SCD30::SCD30()
    : I2C(SCD30_I2CADDR_DEFAULT) {
}

bool SCD30::begin(int32_t sensor_id) {
    sensorIDHumidity = sensor_id;
    sensorIDTemp = sensor_id + 1;

    resetSensor();

    // First I2C transfer after reset can fail, double tapping seems to get by it.
    if((startContinuousMeasurement() || startContinuousMeasurement()) && setMeasurementInterval(2))
        return begun = true;
    return false;
}

bool SCD30::beginIfNecessary() {
    return begun ? true : begin();
}

void SCD30::resetData() {
    carbonDioxide = 0.f;
    temperature = 0.f;
    relativeHumidity = 0.f;
}

bool SCD30::readData() {
    uint8_t buffer[18];

    buffer[0] = (SCD30_CMD_READ_MEASUREMENT >> 8) & 0xff;
    buffer[1] = SCD30_CMD_READ_MEASUREMENT & 0xff;

    if(!write(buffer, 2)) {
        ERROR("%d: Write failure", __LINE__);
        return false;
    }

    furi_delay_ms(4); // delay between write and read specified by the datasheet

    if(!read(buffer, 18)) {
        ERROR("%d: Read failure", __LINE__);
        return false;
    }

    // loop through the bytes we read, 3 at a time for i=MSB, i+1=LSB, i+2=CRC
    for(uint8_t i = 0; i < 18; i += 3) {
        if(crc8(buffer + i, 2) != buffer[i + 2]) {
            // we got a bad CRC, fail out
            ERROR("%d: CRC failure", __LINE__);
            return false;
        }
    }

    // CRCs are good, unpack floats
    uint32_t co2 = 0, temp = 0, hum = 0;

    co2 |= buffer[0];
    co2 <<= 8;
    co2 |= buffer[1];
    co2 <<= 8;
    co2 |= buffer[3];
    co2 <<= 8;
    co2 |= buffer[4];

    temp |= buffer[6];
    temp <<= 8;
    temp |= buffer[7];
    temp <<= 8;
    temp |= buffer[9];
    temp <<= 8;
    temp |= buffer[10];

    hum |= buffer[12];
    hum <<= 8;
    hum |= buffer[13];
    hum <<= 8;
    hum |= buffer[15];
    hum <<= 8;
    hum |= buffer[16];

    memcpy(&carbonDioxide, &co2, sizeof(carbonDioxide));
    memcpy(&temperature, &temp, sizeof(temperature));
    memcpy(&relativeHumidity, &hum, sizeof(relativeHumidity));

    return true;
}

void SCD30::resetSensor() {
    sendCommand(SCD30_CMD_SOFT_RESET);
    furi_delay_ms(30);
}

bool SCD30::dataReady() {
    return readRegister(SCD30_CMD_GET_DATA_READY) == 1;
}

bool SCD30::setMeasurementInterval(uint16_t interval) {
    return (2 <= interval) && (interval <= 1800) &&
           sendCommand(SCD30_CMD_SET_MEASUREMENT_INTERVAL, interval);
}

uint16_t SCD30::getMeasurementInterval() {
    return readRegister(SCD30_CMD_SET_MEASUREMENT_INTERVAL);
}

bool SCD30::selfCalibrationEnabled() {
    return readRegister(SCD30_CMD_AUTOMATIC_SELF_CALIBRATION) == 1;
}

bool SCD30::selfCalibrationEnabled(bool enabled) {
    return sendCommand(SCD30_CMD_AUTOMATIC_SELF_CALIBRATION, enabled);
}

bool SCD30::startContinuousMeasurement(uint16_t pressure) {
    return sendCommand(SCD30_CMD_CONTINUOUS_MEASUREMENT, pressure);
}

uint16_t SCD30::getAmbientPressureOffset() {
    return readRegister(SCD30_CMD_CONTINUOUS_MEASUREMENT);
}

bool SCD30::setAltitudeOffset(uint16_t altitude) {
    return sendCommand(SCD30_CMD_SET_ALTITUDE_COMPENSATION, altitude);
}

uint16_t SCD30::getAltitudeOffset() {
    return readRegister(SCD30_CMD_SET_ALTITUDE_COMPENSATION);
}

bool SCD30::setTemperatureOffset(uint16_t temp_offset) {
    return sendCommand(SCD30_CMD_SET_TEMPERATURE_OFFSET, temp_offset);
}

uint16_t SCD30::getTemperatureOffset() {
    return readRegister(SCD30_CMD_SET_TEMPERATURE_OFFSET);
}

bool SCD30::forceRecalibrationWithReference(uint16_t reference) {
    return (400 <= reference) && (2000 <= reference) &&
           sendCommand(SCD30_CMD_SET_FORCED_RECALIBRATION_REF, reference);
}

uint16_t SCD30::getForcedCalibrationReference() {
    return readRegister(SCD30_CMD_SET_FORCED_RECALIBRATION_REF);
}

bool SCD30::sendCommand(uint16_t command) {
    const uint8_t buffer[2]{
        static_cast<uint8_t>((command >> 8) & 0xff), static_cast<uint8_t>(command & 0xff)};
    return write(buffer, sizeof(buffer));
}

bool SCD30::sendCommand(uint16_t command, uint16_t argument) {
    const uint8_t buffer[5]{
        static_cast<uint8_t>((command >> 8) & 0xff),
        static_cast<uint8_t>(command & 0xff),
        static_cast<uint8_t>(argument >> 8),
        static_cast<uint8_t>(argument & 0xff),
        crc8(buffer + 2, 2),
    };
    return write(buffer, sizeof(buffer));
}

uint16_t SCD30::readRegister(uint16_t reg_addr) {
    uint8_t buffer[2]{
        static_cast<uint8_t>((reg_addr >> 8) & 0xff),
        static_cast<uint8_t>(reg_addr & 0xff),
    };
    // the SCD30 really wants a stop before the read!
    write(buffer, 2);
    furi_delay_ms(4); // delay between write and read specified by the datasheet
    read(buffer, 2);
    return static_cast<uint16_t>((buffer[0] << 8) | (buffer[1] & 0xff));
}

uint32_t SCD30::millis() const {
    const auto timer = furi_hal_cortex_timer_get(1000);
    return timer.start / timer.value;
}

static uint8_t crc8(const uint8_t* data, int len) {
    /* CRC-8 formula from page 14 of SHT spec pdf
	 *
	 * Test data 0xbe, 0xef should yield 0x92
	 *
	 * Initialization data 0xff
	 * Polynomial 0x31 (x8 + x5 +x4 +1)
	 * Final XOR 0x00
	 */

    const uint8_t polynomial = 0x31;
    uint8_t crc = 0xff;

    for(int j = len; j; --j) {
        crc ^= *data++;
        for(int i = 8; i; --i) crc = (crc & 0x80) ? (crc << 1) ^ polynomial : (crc << 1);
    }

    return crc;
}