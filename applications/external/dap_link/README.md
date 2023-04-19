# Flipper Zero as CMSIS DAP/DAP Link

Flipper Zero as a [Free-DAP](https://github.com/ataradov/free-dap) based SWD\JTAG debugger. Free-DAP is a free and open source firmware implementation of the [CMSIS-DAP](https://www.keil.com/pack/doc/CMSIS_Dev/DAP/html/index.html) debugger.

## Protocols

SWD, JTAG , CMSIS-DAP v1 (18 KiB/s), CMSIS-DAP v2 (46 KiB/s), VCP (USB-UART).

WinUSB for driverless installation for Windows 8 and above.

## Usage

### VSCode + Cortex-Debug

  Set `"device": "cmsis-dap"`
  
#### BluePill configuration example
  
  ```json
{
    "name": "Attach (DAP)",
    "cwd": "${workspaceFolder}",
    "executable": "./build/firmware.elf",
    "request": "attach",
    "type": "cortex-debug",
    "servertype": "openocd",
    "device": "cmsis-dap",
    "configFiles": [
        "interface/cmsis-dap.cfg",
        "target/stm32f1x.cfg",
    ],
},
  ```

#### Flipper Zero configuration example
  
  ```json
{
    "name": "Attach (DAP)",
    "cwd": "${workspaceFolder}",
    "executable": "./build/latest/firmware.elf",
    "request": "attach",
    "type": "cortex-debug",
    "servertype": "openocd",
    "device": "cmsis-dap",
    "svdFile": "./debug/STM32WB55_CM4.svd",
    "rtos": "FreeRTOS",
    "configFiles": [
        "interface/cmsis-dap.cfg",
        "./debug/stm32wbx.cfg",
    ],
    "postAttachCommands": [
        "source debug/flipperapps.py",
    ],
},
  ```

### OpenOCD

Use `interface/cmsis-dap.cfg`. You will need OpenOCD v0.11.0.

Additional commands:

* `cmsis_dap_backend hid` for CMSIS-DAP v1 protocol.
* `cmsis_dap_backend usb_bulk` for CMSIS-DAP v2 protocol.
* `cmsis_dap_serial DAP_Oyevoxo` use DAP-Link running on Flipper named `Oyevoxo`.
* `cmsis-dap cmd 81` - reboot connected DAP-Link.

#### Flash BluePill
  
  ```bash
openocd -f interface/cmsis-dap.cfg -f target/stm32f1x.cfg -c init -c "program build/firmware.bin reset exit 0x8000000"
  ```

#### Flash Flipper Zero using DAP v2 protocol
  
  ```bash
openocd -f interface/cmsis-dap.cfg -c "cmsis_dap_backend usb_bulk" -f debug/stm32wbx.cfg -c init -c "program build/latest/firmware.bin reset exit 0x8000000"
  ```

#### Reboot connected DAP-Link on Flipper named Oyevoxo</summary>
  
  ```bash
openocd -f interface/cmsis-dap.cfg -c "cmsis_dap_serial DAP_Oyevoxo" -c "transport select swd" -c "adapter speed 4000000" -c init -c "cmsis-dap cmd 81" -c "exit"
  ```

### PlatformIO

Use `debug_tool = cmsis-dap` and `upload_protocol = cmsis-dap`. [Documentation](https://docs.platformio.org/en/latest/plus/debug-tools/cmsis-dap.html#debugging-tool-cmsis-dap). Remember that Windows 8 and above do not require drivers.

#### BluePill platformio.ini example</summary>
  
  ```ini
[env:bluepill_f103c8]
platform = ststm32
board = bluepill_f103c8
debug_tool = cmsis-dap
upload_protocol = cmsis-dap
  ```
