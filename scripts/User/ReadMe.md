encode.py  
decode.py  
A set of python3 scripts for processing the Flipper image files.  
  
  
PREREQUISITES  
You'll need heatshrink installed - a small embedded/RTOS compression and decompression library  
You can get that here https://github.com/atomicobject/heatshrink  
  
  
HOW TO USE  
Decode a .mb into .xbm:  
decode.py input_image output_image [width] [height]  
Dimensions are not stored in .bm so you need to specify.  
If you do not enter anything here it will assume 128x64. THIS WILL NOT ALWAYS BE CORRECT.  
  
  
Encode an .xbm file into .xb  
encode.py input_image output_image  
That's it.   



