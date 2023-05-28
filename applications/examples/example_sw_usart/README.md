# Software USART
This example application demonstrates the use of the software usart library,
which makes it possible to connect devices with usart interface to any gpio. 
It also covers basic GUI, input handling, threads and localisation.

## Electrical connections
Before launching the application, connect the sensor to Flipper's external GPIO according to the table below:
| Device | Flipper |
| :-----: | :-----: |
| VDD +3.3 | 9 |
| VDD +5 |  1 |
| GND | 8,11,18, |
| Tx  | 2,3,4,5,6,7,13,14,15,16 |
| Rx  | 2,3,4,5,6,7,13,14,15,16 |

*NOTE 1*: When using SwUsartModeRxTxHalfDuplex mode
gpio Tx and Rx are the same


## Launching the application
In order to launch this demo, follow the steps below:
1. Make sure your Flipper has an SD card installed.
2. Connect your Flipper to the computer via a USB cable.
3. Run `./fbt launch_app APPSRC=example_sw_usart` in your terminal emulator of choice.

## Changing the data pin
It is possible to use other GPIO Tx/Rx pin. In order to change it, set the `SW_USART_GPIO_PIN_TX` and `SW_USART_GPIO_PIN_RX` macro to any of the options listed below:

```c
/* Possible GPIO pin choices:
 - gpio_ext_pc0
 - gpio_ext_pc1
 - gpio_ext_pc3
 - gpio_ext_pb2
 - gpio_ext_pb3
 - gpio_ext_pa4
 - gpio_ext_pa6
 - gpio_ext_pa7
 - gpio_ibutton
*/

#define SW_USART_GPIO_PIN_TX (gpio_ext_pc3)
#define SW_USART_GPIO_PIN_RX (gpio_ext_pa4)
```

With the changes been made, recompile and launch the application again. 

Allocate/Free SwUsart
```c
sw_usart_alloc(void);
sw_usart_free(SwUsart* sw_usart);
```
Start SwUsart
```c
sw_usart_start(
    SwUsart* sw_usart,
    uint32_t baudrate,
    const GpioPin* tx_pin,
    const GpioPin* rx_pin,
    bool inverted);
```

If desired, the package format, as well as mode, can be changed
```c
sw_usart_set_config(
    SwUsart* sw_usart,
    SwUsartMode mode,
    SwUsartWordLength data_bit,
    SwUsartParity parity,
    SwUsartStopBits stop_bit);
/* Default setting:
 - 8 data bits, 
 - no parity
 - 1 stop bit, 
*/
```

Ñhange the size of the receive buffer
```c
sw_usart_set_rx_buffer_size(SwUsart* sw_usart, size_t size);
```

Output sync signal to work in synchronous mode
```c
sw_usart_set_sync_config(SwUsart* sw_usart, const GpioPin* gpio, bool inverse);
sw_usart_sync_start(SwUsart* sw_usart);
sw_usart_sync_stop(SwUsart* sw_usart);
```

Start transmission in non-blocking mode
```c
sw_usart_dma_tx(SwUsart* sw_usart, uint8_t* data, uint8_t len);
```

Set a callback at the end of the transfer in non-blocking mode
```c
sw_usart_set_tx_callback(SwUsart* sw_usart, SwUsartTxCallbackEnd callback, void* context);
```

Start transmission in blocking mode
```c
sw_usart_tx(SwUsart* sw_usart, uint8_t* data, uint8_t len, uint32_t timeout_ms);
```

Get the number of bytes in the receive buffer
```c
sw_usart_available(SwUsart* sw_usart);
```

Read 1 byte data from the receive buffer
```c
sw_usart_read(SwUsart* sw_usart, uint8_t* data, uint32_t timeout_ms);
```

Read line from the receive buffer
```c
sw_usart_read_line(SwUsart* sw_usart, FuriString* data, uint32_t timeout_ms);
```
