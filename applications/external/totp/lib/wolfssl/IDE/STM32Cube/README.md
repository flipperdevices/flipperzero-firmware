# wolfSSL STM32 Example for STM32 Cube IDE

This example includes:

* wolfCrypt test
* wolfCrypt benchmark
* wolfSSL TLS client/server test using in-memory transfers

These examples use the Cube HAL for STM32.

## Requirements

You need both the STM32 IDE and the STM32 initialization code generator (STM32CubeMX) tools. The STM32CubeMX tool is used to setup a project which is used by the IDE to make any required code level changes and program / debug the STM32.

* STM32CubeIDE: Integrated Development Environment for STM32 [https://www.st.com/en/development-tools/stm32cubeide.html](https://www.st.com/en/development-tools/stm32cubeide.html)
* STM32CubeMX: STM32Cube initialization code generator [https://www.st.com/en/development-tools/stm32cubemx.html](https://www.st.com/en/development-tools/stm32cubemx.html)

## STM32 Cube Pack

### STM32 Cube Pack Installation

1. Download [wolfSSL Cube Pack](https://www.wolfssl.com/files/ide/I-CUBE-wolfSSL.pack)
2. Run the “STM32CubeMX” tool.
3. Under “Manage software installations” pane on the right, click “INSTALL/REMOVE” button. This can be also found by clicking "Help" -> "Managed embedded software packages"
4. From Local and choose “I-CUBE-wolfSSL.pack”.
5. Accept the GPLv2 license. Contact wolfSSL at sales@wolfssl.com for a commercial license and support/maintenance.

### STM32 Cube Pack Usage

1. Create or open a Cube Project based on your hardware. See the sections below for creating a project and finding the example projects.
2. Under “Software Packs” choose “Select Components”.
3. Find and check all components for the wolfSSL.wolfSSL packs (wolfSSL / Core, wolfCrypt / Core and wolfCrypt / Test). Close
4. Under the “Software Packs” section click on “wolfSSL.wolfSSL” and configure the parameters.
5. For Cortex-M recommend “Math Configuration” -> “Single Precision Cortex-M Math” for the fastest option.
6. Hit the "Generate Code" button
7. Open the project in STM32CubeIDE
8. The Benchmark example uses float. To enable go to "Project Properties" -> "C/C++ Build" -> "Settings" -> "Tool Settings" -> "MCU Settings" -> Check "Use float with printf".
9. To enable printf make the `main.c` changes below in the [STM32 Printf](#stm32-printf) section.

### Creating your own STM32CubeMX configuration

If none of the examples fit your STM32 type then you can create your own in STM32CubeMX by doing the following:

1. Create a project with the correct STM32 model.
2. Click on the "Software Packs" drop down near the top and choose "Select Components".
3. Expand the "wolfSSL" pack twice and check all the components. Then exit this menu.
4. Under "System Core" select "SYS" and changed the "Timebase Source" to TIM1.
5. Under "Timers" select "RTC" and make sure this is enabled.
6. Under "Connectivity" enable whichever UART/USART you have a serial I/O connected to.
7. Under "Middleware" select "FREERTOS" and change the interface to "CMSIS_V2".
    1. Increase the "TOTAL_HEAP_SIZE", preferably to 120000 but on smaller chips such as the F107 you may only be able to increase this to 40000.
    2. Enable "USE_MALLOC_FAILED_HOOK".
    3. Change "CHECK_FOR_STACK_OVERFLOW" to "Option2".
    4. Under "Tasks and Queues" select Add for a new task.
    5. Set the "Task Name" to "wolfCrypt".
    6. Set the "Stack Size" to 8960 or as high as you can close to that. The "Heap Usage" will show an error if this is too high.
    7. Set the "Entry Function" to "wolfCryptDemo".
    8. Set the "Code Generation Option" to "As external".
8. In "Software Packs" select "wolfSSL" and change any options as required.
9. Go to "Clock Configuration" and set the "HCLK" as high as the tool will let you.
10. In "Project Manager" select the "STM32CubeIDE" toolchain.

When you get to the IDE make sure you edit `wolfSSL.I-CUBE-wolfSSL_conf.h` to set the `HAL_CONSOLE_UART` to the correct one for your configuration.

## Configuration

The settings for the wolfSSL CubeMX pack are in the generated `wolfSSL.I-CUBE-wolfSSL_conf.h` file. An example of a generated file can be found at `examples/configs/user_settings_stm32.h`.

The template used for generation is `IDE/STM32Cube/default_conf.ftl`, which is stored in the pack here: `STM32Cube/Repository/Packs/wolfSSL/wolfSSL/[Version]/CubeMX/templates/default_conf.ftl`.

If the default settings for the Cube GUI are insufficient you can customize the build using one of these methods to prevent the changes from being overwritten when generating the code:

* Copy the `wolfSSL.I-CUBE-wolfSSL_conf.h` to `Core/Inc` and rename to `user_settings.h`. Then add the preprocessor macro `WOLFSSL_USER_SETTINGS` to your project. This will use the `user_settings.h` instead of the generated configuration.

OR

* Edit the source template file used for Cube pack generation here: `STM32Cube/Repository/Packs/wolfSSL/wolfSSL/[Version]/CubeMX/templates/default_conf.ftl`.


The section for "Hardware platform" may need to be adjusted depending on your processor and board:

* To enable STM32F1 support define `WOLFSSL_STM32F1`.
* To enable STM32F2 support define `WOLFSSL_STM32F2`.
* To enable STM32F4 support define `WOLFSSL_STM32F4`.
* To enable STM32F7 support define `WOLFSSL_STM32F7`.
* To enable STM32L4 support define `WOLFSSL_STM32L4`.
* To enable STM32L5 support define `WOLFSSL_STM32L5`.
* To enable STM32H7 support define `WOLFSSL_STM32H7`.
* To enable STM32WB support define `WOLFSSL_STM32WB`.
* To enable STM32U5 support define `WOLFSSL_STM32U5`.
* To enable STM32H5 support define `WOLFSSL_STM32H5`.

To use the STM32 Cube HAL support make sure `WOLFSSL_STM32_CUBEMX` is defined.

The PKA acceleration for ECC is available on some U5, L5 and WB55 chips.
This is enabled with `WOLFSSL_STM32_PKA`. You can see some of the benchmarks [here](STM32_Benchmarks.md).

To disable hardware crypto acceleration you can define:

* `NO_STM32_HASH`
* `NO_STM32_CRYPTO`
* `NO_STM32_RNG`

To enable the latest Cube HAL support please define `STM32_HAL_V2`.

If you'd like to use the older Standard Peripheral library undefine `WOLFSSL_STM32_CUBEMX`.

With STM32 Cube HAL v2 some AES GCM hardware has a limitation for the AAD header, which must be a multiple of 4 bytes. If your HAL does not support `CRYP_HEADERWIDTHUNIT_BYTE` then consider adding `STM32_AESGCM_PARTIAL` if you are getting AES GCM authentication failures. This bug existed in v1.16.0 or later.

The STM32F7 v1.17.0 pack has a bug in the AES GCM code for handling of additional authentication data when not a multiple of 4 bytes. To patch see `stm32f7xx_hal_cryp.c` -> `CRYP_GCMCCM_SetHeaderPhase`:

```diff
diff --git a/stm32f7xx_hal_cryp.c b/stm32f7xx_hal_cryp.c
index 2ae42d0..9666f26 100644
--- a/stm32f7xx_hal_cryp.c
+++ b/stm32f7xx_hal_cryp.c
@@ -5600,7 +5600,6 @@ static HAL_StatusTypeDef CRYP_GCMCCM_SetHeaderPhase(CRYP_HandleTypeDef *hcryp, u
   uint32_t size_in_bytes;
   uint32_t tmp;
   uint32_t mask[12] = {0x0U, 0xFF000000U, 0xFFFF0000U, 0xFFFFFF00U,  /* 32-bit data type */
-                       0x0U, 0x0000FF00U, 0x0000FFFFU, 0xFF00FFFFU,  /* 16-bit data type */
                        0x0U, 0x000000FFU, 0x0000FFFFU, 0x00FFFFFFU}; /*  8-bit data type */

   /***************************** Header phase for GCM/GMAC or CCM *********************************/
@@ -5842,7 +5841,7 @@ static HAL_StatusTypeDef CRYP_GCMCCM_SetHeaderPhase(CRYP_HandleTypeDef *hcryp, u
       {
         /* Enter last bytes, padded with zeroes */
         tmp =  *(uint32_t *)(hcryp->Init.Header + hcryp->CrypHeaderCount);
-        tmp &= mask[(hcryp->Init.DataType * 2U) + (size_in_bytes % 4U)];
+        tmp &= mask[(hcryp->Init.HeaderWidthUnit * 4U) + (size_in_bytes % 4U)];
         hcryp->Instance->DINR = tmp;
         loopcounter++;
         /* Pad the data with zeros to have a complete block */
```

If you are using FreeRTOS make sure your `FreeRTOSConfig.h` has its `configTOTAL_HEAP_SIZE` increased.

The TLS client/server benchmark example requires about 76 KB for allocated tasks (with stack) and peak heap. This uses both a TLS client and server to test a TLS connection locally for each enabled TLS cipher suite.

## Example `IDE/STM32Cube/wolfssl_example.c` Output

```
....MENU

.t. WolfCrypt Test
.b. WolfCrypt Benchmark
.l. WolfSSL TLS Bench
.e. Show Cipher List
.s. Run TLS 1.3 Server over UART
.c. Run TLS 1.3 Client over UART

Please select one of the above options:
```

### Example for TLS v1.3 over UART

A tutorial for setting this up can be found here: https://www.youtube.com/watch?v=OK6MKXYiVBY

The TLS v1.3 client/server examples over UART are paired with these host-side applications:
* https://github.com/wolfSSL/wolfssl-examples/blob/master/tls/client-tls-uart.c
* https://github.com/wolfSSL/wolfssl-examples/blob/master/tls/server-tls-uart.c

To use this example you will need to use the STM32Cube interface to enable an additional USART and enable DMA for the RX with defaults. Enabling DMA for the USART requires adding the USART RX DMA in the STM32Cube tool. Under Connectivity click on your TLS USART# and goto DMA Settings and "Add" one for USART#_RX with default options.

On some boards, such as U5, there is GPDMA support. In this case when you click on "DMA Settings" you will be given a button to take you to GPDMA1 configuration. Click it. You can then enable a channel (any of the ones from 0 to 11 should be fine.) as "Standard Request Mode" and set the "Request Configuration" section's "Request" to USART#_RX. In the "System Core" tab, find NVIC and click on it. Make sure that the GPDMA1 global interrupt for your channel is enabled as well as USARTx global interrupt.

Then set the TLS_UART macro to the correct `huart#` instance. This USART will be used as a TLS transport.

```c
#define TLS_UART huart2
```

To disable the TLS UART example you can define `NO_TLS_UART_TEST`.

## Benchmarks

See [STM32_Benchmarks.md](STM32_Benchmarks.md).

Note: The Benchmark example uses float. To enable go to "Project Properties" -> "C/C++ Build" -> "Settings" -> "Tool Settings" -> "MCU Settings" -> Check "Use float with printf".

## STM32 Printf

In main.c make the following changes:

This section needs to go below the `UART_HandleTypeDef` line, otherwise `wolfssl/wolfcrypt/settings.h` will error.

```c
/* Retargets the C library printf function to the USART. */
#include <stdio.h>
#include <wolfssl/wolfcrypt/settings.h>
#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    HAL_UART_Transmit(&HAL_CONSOLE_UART, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}
#ifdef __GNUC__
int _write(int file,char *ptr, int len)
{
    int DataIdx;
    for (DataIdx= 0; DataIdx< len; DataIdx++) {
        __io_putchar(*ptr++);
    }
    return len;
}
#endif
```

In the `main()` function make the follow `setvbuf()` additions after `HAL_Init()`.

```c
int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Turn off buffers, so I/O occurs immediately */
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

```

## Support

For questions please email [support@wolfssl.com](mailto:support@wolfssl.com)
