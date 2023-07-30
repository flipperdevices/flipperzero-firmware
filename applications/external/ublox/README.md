# ublox
Flipper Zero app to read from a u-blox GPS over I2C. 

This app has many features but currently exhibits a somewhat nasty
memory leak, which is hard to track down but may be related to
starting the app multiple times without reboot.
