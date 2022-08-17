# FlipperZero_plugin_howto
A simple plugin for the FlipperZero written as a tutorial example [ie. excessive documentation &amp; error handling]

# If you have not yet installed the FlipperZero devkit
```
mkdir -p ~/flipperZero/official/
cd ~/flipperZero/official/
git clone --recursive  https://github.com/flipperdevices/flipperzero-firmware.git  ./
./fbt
```
...and wait while it downloads all the required tools and performs the first build of the code.

# After the devkit is installed
Assuming your devkit is in: `~/flipperZero/official/`<br/>
...checkout this demo with:
```
cd ~/flipperZero/official//applications
mkdir bc_demo
cd bc_demo
git clone https://github.com/csBlueChip/FlipperZero_plugin_howto.git ./
```

# After bc_demo is downloaded
Compile with:
```
cd ~/flipperZero/official/
./fbt
```
...And/Or (Compile and) Flash with:
```
cd ~/flipperZero/official/
./fbt flash_usb
```
...Flashing will fail if the FlipperZero is running an app/plugin!

# About the bc_demo code & documentation
* Review the `README.txt` file for an overview of the code, and how to get started.
* Take a look in `bc_demo.c` for, frankly, an abusive amount of commenting; and overkill error-handling.

The code is all M.I.T. Licensed ...take it ...use it ...butcher it ...polish it ...whatever. <br/>
...If you do anything cool with it, I'd love to see your creation :)

# Compiling the demo
You will need to add bc_demo to the system menu by editing `~/flipperZero/official//applications/meta/application.fam` in accordance with the intructions in `README.txt (Plugins) #2`<br>
...then simply use the same compile & flash command we used previously:
```
cd ~/flipperZero/official/
./fbt flash_usb
```
