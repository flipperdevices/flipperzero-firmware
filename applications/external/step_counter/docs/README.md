# Flipperzero-StepCounter-fap

This is a simple StepCounter/Pedometer for FlipperZero using a 2 axys accelerometer, the Memsic2125 module.
Thanks to @jamisonderek for the tutorial on how to implement Memsic2125 module on Flipper Zero, and for the support.

# GPIO Connections:
#### Mx2125 Pin (Name) -> Flipper Pin (Name) / [Purpose]

Pin 1	(Tout) -> not connected / [Temperature Out]       
Pin 2	(Yout) -> Pin 16 (C0) / [Y-axis PWM Out - 100Hz, duty cycle = value]        
Pin 3	(GND)	-> GND         
Pin 4	(GND)	-> GND         
Pin 5	(Xout) -> C1 / [X-axis PWM Out - 100Hz, duty cycle = value]         
Pin 6	(VDD) -> 3v3 / [Drain voltage - 3.3V to 5V DC]         

Coming Soon:
Add an INFO window with the link to the original Repo and the Pinout connection between the Flipper Zero and the Memsic Mx2125 module
Add a simple animation of a Dolphin Runner
Add a management of the possible errors
Add the possibility to restart the counting
Add the possibility to set a daily/weekly GOAL to reach
Add the possibility to save dayly results in files at the path: apps_data/stepcounter, to comtìpare them and improve (interacting with calendar FAP? maybe...)