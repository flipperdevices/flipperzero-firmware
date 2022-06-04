#####################################  
encode.py  
decode.py  
icondecode.py

A set of python3 scripts for processing the Flipper image files.  
  
#####################################  
PREREQUISITES  
  
  
You'll need heatshrink installed - a small embedded/RTOS compression and decompression library  
You can get that here https://github.com/atomicobject/heatshrink  
  
#####################################  
HOW TO USE  
  
##  
# decode.  
  
Decode a .mb into .xbm:  
decode.py input_image output_image [width] [height]   
Dimensions are not stored in .bm so you need to specify  
If you have the meta.txt available for the animation set the dimensions will be in here.  
It may also be part of the directory name for the animation files as well.  
  
If you do not enter anything here it will assume 128x64. THIS WILL NOT ALWAYS BE CORRECT.  
  
##  
# encode  
Encode an .xbm file into .xb  
encode.py input_image output_image  
You will also get the image dimensions for use in meta.txt  
That's it.  

##  
# icondecode  
Decompress an icon asset (as found in assets_icons.c and elsewhere)  
icondecodepy input_image output_image [trim] [width] [height]  
The icons in this file have a different header format. This will need to be trimmed.  
A value of 8 for trim appears to be correct.  
As with regular decoding, the width and height are not listed - but can be found in code/const/variable/etc names.  
