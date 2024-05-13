Displays the screen of the Pwnagotchi on the Flipper's screen. Interfaces over UART with a Raspberry Pi Zero W running the Pwnagotchi program.

## Setup instructions
### PwnZero Setup (Pwnagotchi side)
This procedure will explain how to configure the Pwnagotchi to use the PwnZero plugin to communicate with the Flipper. Note: You may need to change the pyserial file name based on whichever version pip downloaded for you.
1. On your host machine run `pip3 download pyserial`, this should download a `.whl` file.
2. Take note of the filename of the `.whl` and insert that instead of mine
3. Also on the host, run `scp pyserial-3.5-py2.py3-none-any.whl pi@10.0.0.2:/home/pi` to transfer the `.whl` file to the pwnagotchi
4. Now on the pwnagotchi install the module as root with `sudo pip3 install /home/pi/pyserial-3.5-py2.py3-none-any.whl`
5. Disable Bluetooth on the Pi by adding ```dtoverlay=disable-bt``` at the bottom of the ```/boot/config.txt``` file
    1. This needs to be disabled so that the full UART is directed to ```/dev/serial0```
6. Enter the raspberry pi configuration settings with `sudo raspi-config`
    1. Select `Interface Options`
    2. Select `Serial Port`
    3. Select `No` for shell over serial
    4. Select `Yes` for serial enabled
6. Place the PwnZero.py file somewhere on the Pi in either its own folder or a folder with other plugins
7. Edit ```/etc/pwnagotchi/config.toml``` file and set ```main.custom_plugins = "/path/to/plugin/folder"```
8. Follow hardware setup shown in `doc/HardwareSetup.md` to connect the devices
9. Restart the Pwnagotchi and open the Flipagotchi app on the Flipper Zero

## License
[MIT](https://choosealicense.com/licenses/mit/)

## Disclaimer
<b>This program is meant for educational purposes ONLY. I disclaim any and all responsibility for the usage of this program by external parties (not me).</b>

See GitHub repository for more detailed information and setup instructions.