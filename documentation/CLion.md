# Writing and debugging in CLion

CLion is a cross-platform IDE for C and C++ from JetBrains. While it does support some features
for [embedded development](https://www.jetbrains.com/help/clion/embedded-overview.html),
there are still some caveats, especially with debugging.

## Setup core functionality

CLion syntax highlighting and autocomplete engine works out of the box only with CMake, but it supports
[compilation database](https://www.jetbrains.com/help/clion/compilation-database.html), which `fbt` can generate.
So to start:

1. Clone this repo by hand or using CLion.
2. Open it as a project.
3. Go to `File -> Settings -> Build, Execution, Deployment -> Toolchains` and create toolkit:

   - C Compiler: `toolchain/x86_64-linux/bin/arm-none-eabi-gcc`
   - С++ Compiler: `toolchain/x86_64-linux/bin/arm-none-eabi-g++`
   - Debugger: `toolchain/x86_64-linux/bin/arm-none-eabi-gdb-py`

4. Build it and generate compilation database:

   ```shell
   ./fbt
   ./fbt firmware_cdb
   ```

5. Go and find `compile_commands.json` file at `build/latest`, then click right mouse button and choose
`Load Compilation Database Project`.
6. It should try to sync a project, but probably will fail.

It will cause inability to index a project, so most features won't work.

### Problems and their solutions

- CLion does not understand `ccache`.

   **Solution 1:** remove `ccache` from system and regenerate compilation database.

   **Solution 2:** just remove it from file:

   ```shell
   cd built/target
   sed -i 's/ccache\ //g' compile_commands.json
   ```

- CLion does not understand `arm-none-eabi-as`.

   **Solution:** remove it from compilation database:

   ```shell
   cd build/latest
   jq 'map(select(.command | contains("arm-none-eabi-as") | not))' compile_commands.json > compile_commands.json.tmp && mv compile_commands.json.tmp compile_commands.json
   ```

- CLion cannot find stdlib.

   **Solution 1:** Launch CLion under `fbt` environment:

   ```shell
   source scripts/toolchain/fbtenv.sh
   gtk-launch jetbrains-clion.desktop
   ```

   **Solution 2:** Replace path to compiler with absolute ones:

   ```shell
   cd built/target
   sed -i 's/arm-none-eabi-gcc/\/home\/user\/documents\/flipperzero-firmware\/toolchain\/x86_64-linux\/bin\/arm-none-eabi-gсс/g' compile_commands.json
   ```
  
   **Solution 3:** Install the latest `arm-none-eabi` toolkit with your system package manager:

   ```shell
   sudo pacman -S arm-none-eabi-gcc arm-none-eabi-gdb arm-none-eabi-newlib  
   ```
  
- Compilation database contains links to source files that are not presented in the main source tree.
  
  **Solution 1:** wait a patch of SCons from Flipper team.

  **Solution 2:** remove these links from compilation database file by hand.

After this troubleshooting CLion should work as expected: highlighting and autocompleting your code.

### Setup debugging

1. Go to Run Configuration and create your own with type `Remote Debug`:

   - Pick debugger from toolchain you created before.
   - Fill `'target remote' args` with content from output of `./fbt get_blackmagic`
   - Point to symbol file: `build/latest/firmware.elf`

2. Create a `.gdbinit` file with this content in the project directory:

   ```
   set confirm off
   set trace-commands on
  
   define target remote
   target extended-remote $arg0
   set mem inaccessible-by-default off
   source debug/flipperapps.py
   end
   ```
   
3. Then create a `.gdbinit` file in your home directory:

   ```
   set auto-load safe-path /path/to/your/project
   ```
   
4. Before first debug session you have to attach Flipper Zero:

   ```bash
   toolchain/x86_64-linux/bin/arm-none-eabi-gdb-py -q -ex "target remote `./fbt get_blackmagic`" -ex "monitor swdp_scan" -ex "attach 1" -ex "quit" build/latest/firmware.elf
   ```
   
5. Now you can set breakpoints and try to debug using the configuration you create. Ensure that your debugger
   connects successfully, otherwise try to reboot Flipper and start again from step 4.

6. _(Optionally)_ After successful session of debug you can edit your debug configuration and add in "Before launch"
   block "External tool" to automate step 4.

   ![](/documentation/images/clion_external_tool.png)
