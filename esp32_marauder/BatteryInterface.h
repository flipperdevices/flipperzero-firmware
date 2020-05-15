#ifndef BatteryInterface_h
#define BatteryInterface_h

#include <Arduino.h>

#include <Wire.h>

#define I2C_SDA 33
#define I2C_SCL 22
#define IP5306_ADDR 0x75

class BatteryInterface {
  private:
    uint32_t initTime = 0;

  public:
    int8_t battery_level = 0;
    int8_t old_level = 0;
    bool i2c_supported = false;

    BatteryInterface();

    void RunSetup();
    void main(uint32_t currentTime);
    int8_t getBatteryLevel();
};

#endif
