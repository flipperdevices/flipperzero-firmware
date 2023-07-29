# ublox
Flipper Zero app to read from a u-blox GPS over I2C. It currently
exhibits a somewhat nasty memory leak, which happens if you try to run
the app two or more times after rebooting (but not the first time,
that works fine).
