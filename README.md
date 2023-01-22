# EXTRA
A pack of extra commands for the Flipper Zero (a C library). Check out the demo [here](https://github.com/Milk-Cool/EXTRA-demo)!

## Downloading and importing
In your app directory run:
```bash
git clone https://github.com/Milk-Cool/EXTRA.git
```

## Usage
To include it in a plugin:
```c
#include "extra/extra.h"
```
To register extra commands:
```c
extra_init_register();
```
To unregister and free the used memory:
```c
extra_deinit_unregister();
```
You can also use `extra_registered()` to check if extra commands are currently registered.

## Commands
For more info refer to `extra_help`.

## Scripts
Use `start <path_to_script>` to run a script.
A script is just CLI commands in a file, each one is on a new line.
If a line starts with `#` it's a comment. It's ignored by the `start` command, the 

This example outputs "Hello World", waits a second and plays the success sequence (the one that plays, for example, when the Flipper wrote data to a tag).

```bash
# Example script

echo Hello World
sleep 1000
sequence success
```
