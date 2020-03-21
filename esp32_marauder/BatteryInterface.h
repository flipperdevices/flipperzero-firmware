#ifndef BatteryInterface_h
#define BatteryInterface_h

#include <Wire.h>

#define I2C_SDA 33
#define I2C_SCL 22
#define IP5306_ADDR 0x75

class BatteryInterface {
  private:

  public:
    int8_t battery_level = 0;
    bool i2c_supported = false;

    BatteryInterface();

    void RunSetup();
    int8_t getBatteryLevel();
};

#endif
