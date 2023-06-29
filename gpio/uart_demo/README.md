# UART_DEMO
This is a demonstration of using the UART to read (RX) and transmit (TX) data.  You can connect
pins RX and TX together with a wire or resistor to create a loopback.  If you have a second 
Flipper Zero then you can connect TX of the first Flipper Zero to RX of the second and the RX of
the first to TX of the second.  You also should connect one of the ground pins from the first 
Flipper Zero to the second Flipper Zero.

When you run the demo application, you will see three options.
- The first option "Clear" will clear the menu.
- The second option "Seng Msg 1" will send "Hello World!" follow by new-line delimiter.
- The third option "Seng Msg 2" will send "Testing 123." follow by new-line delimiter.

Whenever data is received and the delimiter is detected, the new text will be added as an additional
entry in the menu.

# How it works
uart_demo.c is the main class that you should look at.  The goal was to simplify all
the complexity of worker threads and buffers into other files that you should not have
to look at.

We include uart_helper.h...
```c
#include "uart_helper.h"
```

We create a callback to get invoked whenever a new line (text+delimiter) is detected...
```c
// This callback will be invoked whenever a new line is read on the UART!
static void uart_demo_process_line(FuriString* line, void* context) {
    // For the demo, we have code to add the line to the submenu.
}
```

We configure the UartHelper as follows. We specify the baud rate, the delimiter of our
message, if we want the delimiter included in our callback data, and which callback to
invoke.  When data is received by the UART it will be added to a ring buffer.  Once the
delimiter is detected in the data, our callback will be invoked with the received data.
The UartHelper uses worker threads and ring buffers to do the processing, but all we need
to do is call ``uart_helper_alloc`` to start receiving data.
```c
#define DEVICE_BAUDRATE 115200
#define LINE_DELIMITER '\n'
#define INCLUDE_LINE_DELIMITER false

app->uart_helper = uart_helper_alloc();
uart_helper_set_baud_rate(app->uart_helper, DEVICE_BAUDRATE);
uart_helper_set_delimiter(app->uart_helper, LINE_DELIMITER, INCLUDE_LINE_DELIMITER);
uart_helper_set_callback(app->uart_helper, uart_demo_process_line, app);
```

We can also send a string using two helper methods...
```c
// Send a c-style string
uart_helper_send(app->uart_helper, "Hello World!\n", 13);

// Send a FuriString
uart_helper_send_string(app->uart_helper, my_furi_string);
```

