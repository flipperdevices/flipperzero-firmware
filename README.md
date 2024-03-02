# Flipper Zero intervalometer

## Hardware

### Parts
+ 2 LTV4N35 octocopuler
+ 2 220 Ohm resistor
+ Wires

### Schema

![schematics](schematics.png)

(The photo from the N3 connector created by Martin Pot. [Source](https://martybugs.net/blog/blog.cgi/gear/CanonN3Connector.html))

### Final product

![prototype](https://m.blog.hu/cy/cybernetic/image/barkacs/flippertest.jpg)

![final](https://m.blog.hu/cy/cybernetic/image/barkacs/final.jpg)

### Limitations

Right now the code is working in manual focus mode only. The focus is connected, but the code not ready for it. The main problem
there is no feedback about the focus process. Slower lens need longer time to focus. In the case of poor light conditions the focus
motor just go back and forth.
