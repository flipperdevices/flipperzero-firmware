#include "BatteryInterface.h"

BatteryInterface::BatteryInterface() {
  
}

void BatteryInterface::RunSetup() {
  Wire.begin(I2C_SDA, I2C_SCL);
}

int8_t BatteryInterface::getBatteryLevel() {
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(0x78);
  if (Wire.endTransmission(false) == 0 &&
      Wire.requestFrom(0x75, 1)) {
    this->i2c_supported = true;
    switch (Wire.read() & 0xF0) {
      case 0xE0: return 25;
      case 0xC0: return 50;
      case 0x80: return 75;
      case 0x00: return 100;
      default: return 0;
    }
  }
  this->i2c_supported = false;
  return -1;
}
