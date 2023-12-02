# THIS IS EXPERIMENTAL TOOL!

**It is not tested properly. You need to compile it by yourself using the makefile. It will print help if called without arguments. It should automatically call ffmpeg and clean all the intermediate files and folders made for or by ffmpeg.**

# How to use:

First, place your video (which **MUST** be named `source.mp4`) somewhere, preferable in some empty folder. Then launch the `conv` app with a command like

````
./conv -p /home/amogus/whatever/69/420 -w 96 -h 64 -f 30 -sr 44100 -sw 990 -sh 720
````

On Windows it may be something like

````
./conv.exe -p C:/Users/amogus/whatever/69/420 -w 96 -h 64 -f 30 -sr 44100 -sw 990 -sh 720
````

Launch without arguments to see what each param does if you are confised.
