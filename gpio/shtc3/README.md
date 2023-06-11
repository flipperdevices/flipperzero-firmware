# SHTC3

This is just a simple console application that uses i2c to communicate with an SHTC3 temperature and humidity device.  It is assumed that you have debug mode turned on (Settings, System, Debug ON).  To see the logs, either connect to serial port or use https://lab.flipper.net/cli and type ``log`` then enter.  Once you are viewing the logs, run the application.

The code does the following:
- The address of the device is hard-coded to 0x70.
- Wake-up (0x3517) is sent to wake up the device.
- Read id (0xefc8) is sent to read the device ID.
- The output is expected to match "(xxxx1xxxxx000111)".

Resources:
- [I2CDevices.org info](https://i2cdevices.org/devices/shtc3)
- [SHTC3 Datasheet](https://www.waveshare.com/w/upload/3/33/SHTC3_Datasheet.pdf)