# Simple On/Off Remote
## Sub-GHz

Main Display
* Saved device 1
* Saved device 2
* Manage Devices

Saved device View
* ON
* OFF
* other


## File System Layout
Inside the data folder, create sub-folders per device. Inside each of the device folders, store the raw files that contain the sub-ghz data, etc.

The device order list, and button list, is based on the sorted file order. This is enforced by the following naming convention:

```
/data_folder
  - 00_Device_1
  - 01_Device_2
     - 00_Button_1.sub
     - 01_Button_2.sub
```

The first two digits and underscore will be stripped before display. Additionally, underscores in folder and filenames will be replaced with spaces.

