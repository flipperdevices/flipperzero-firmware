# flipperzero-geigercounter
A geiger counter application for the Flipper Zero

![banner](https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/logo.jpg)

You need a **geiger counter** board to run this application. This board can be used : https://aliexpress.com/item/1005004074447209.html

**Note :** this board uses a **J305** geiger tube. According this [website](https://www.rhelectronics.store/j305-glassy-geiger-muller-tube-nuclear-radiation-sensor) gamma conversion factor is **0.0081** for this tube. This value has been declared in the header of the source file so you can change it easily if needed. Incorrect conversion factor will give false measurements when **μSv/h** / **mSv/y** is selected.

The geiger counter board can be powered with +5V power pin of the **Flipper Zero**. This pin will automatically be enabled when the program is launched. 

Output pin for measure on arduino cannot be used on the **Flipper Zero** because output voltage is too low. You can use jack out port instead. This port must be connected on **A7** GPIO :

<p align="center"><img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/schematic.jpg" width=75% height=75%></p>

## Build the program

Assuming the toolchain is already installed, copy **flipper_geiger** directory to **applications_user**

Plug your **Flipper Zero** and build the geiger counter :
```
./fbt launch_app APPSRC=applications_user/flipper_geiger
```

The program will automatically be launched after compilation

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper1.png" width=25% height=25%>

**A4** GPIO can be connected on **A7** GPIO to test this application without using a geiger tube. **A4** GPIO is generating a signal whose frequency changes every second.

Press Ok button to clear the graph, press left/right to choose unit (cpm, μSv/h, mSv/y), press back button to quit

If you don't want to build this application, just simply copy **flipper_geiger.fap** on your **Flipper Zero**

## Use cases

Ambient radioactivity :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper2.png" width=25% height=25%> <img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper8.png" width=25% height=25%> <img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper9.png" width=25% height=25%>

**Note :** measures in **μSv/h** / **mSv/y** are not precise

Measure of uranium ore piece inside a lead container :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper3.png" width=25% height=25%>

Measure of uranium ore piece (the most radioactive part) in contact with the geiger tube :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper4.png" width=25% height=25%>

Measure of radium dial pointers in contact with the geiger tube :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper5.png" width=25% height=25%>

All previous measures in a row (the scale of the graph is automatically adjusted) :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper6.png" width=25% height=25%>

**A4** GPIO on **A7** GPIO :

<img src="https://github.com/nmrr/flipperzero-geigercounter/blob/main/img/flipper7.png" width=25% height=25%>

## Changelog

* 2023-02-02
  * μSv/h and mSv/y have been added
  * 5V pin is automatically enabled when the program is launched

* 2023-01-15
  * Code fix & optimizations
  * More events can be handled without any issue

* 2023-01-09
  * Code fix
  * Schematic was added

* 2023-01-08
  * Initial release
