# SUBGHZ DEMO
## Introduction
This is a demonstration of sending &amp; receiving radio signals using the subghz_tx_rx worker library.  This demo requires that you have two Flipper Zero devices.  If you only have one device, you can use a HackRF One or similar to capture/replay the RF signals to simulate a second device.

## Installation Directions
This project is intended to be overlayed on top of an existing firmware repo.
- Clone, Build &amp; Deploy an existing flipper zero firmware repo.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "subghz_demo" [folder](..) to the \applications\plugins\subghz_demo folder in your firmware.
- Build &amp; deploy the firmware.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also extract the subghz_demo.FAP from resources.tar file and use qFlipper to copy the file to the SD Card/apps/Misc folder.


## Running the updated firmware
These directions assume you are starting at the flipper desktop.  If not, please press the back button until you are at the desktop.

- Press the OK button on the flipper to pull up the main menu.
- Choose "Applications" from the menu.
- Choose "Misc" from the sub-menu.
- Choose "Subghz Demo"

- Do the same steps on your second Flipper.

- The flipper should show a 4 digit number that keeps incrementing.
- One the first Flipper press the OK button to send a count message to the other Flipper.
- The other Flipper should show "Received ####" with the number you sent.
- One the first Flipper press the UP button to send a tone message to the other Flipper.
- The other Flipper should play a 440Hz tone.
- If long press the UP button, the other Flipper should play a 880Hz tone. 

- Press the BACK button to exit.


## HackRF One
- If you do not have two Flipper Zero devices, you can use a HackRF One to record messages &amp; broadcast those messages at a future date.  I made a [YouTube](https://www.youtube.com/watch?v=S0sgcDQrVOc) video demo of how to record and broadcast messages.

- To record a message: (replace "flipper-chat.rf" with the file name you want to use, such as "select-middle-sqaure.rf".)
```
sudo hackrf_transfer -r flipper-chat.rf -f 433920000 -s 8000000
```

- To broadcast a message: (replace "flipper-chat.rf" with the saved file name.) 
```
sudo hackrf_transfer -r flipper-chat.rf -f 433920000 -s 8000000 -x 47
```

- What I typically do is:
  - Use the Flipper Zero to send a messasge that I record, then I play back that message at a later time when I want to pretend the other Flipper Zero is sending a message.  You can use the chat app in https://lab.flipper.net/cli, like shown in the video to send a specific packet (or you can use your own code to create the packet.) 
  - Use the HackRF One to record the message from my Fliiper Zero.  Then later I use the chat app in https://lab.flipper.net/cli, to see what the message was (or I replay the message to see how my application would respond.)

## Key concepts for subghz RX/TX
- RX = Receive on a subghz (less than 1GHz) frequency.
- TX = Transmit on a subghz (less than 1 GHZ) frequency.
- Different countries/regions have different laws about [allowed](https://docs.flipperzero.one/sub-ghz/frequencies) broadcast frequencies.  Typically a country allows for broadcasting at 433.92MHz and/or 315.0MHz.
- When sending data, it's useful for the packet to start with something unique to your application (so you know if the message you receive is for your application or some other application.)  The format used by this demo is:
  - First part of message is - SUBGHZ_GAME_NAME (like "SGDEMO:")  This ends with a ":" delimiter, so that "SGDEMO" won't match "SGDEMO2" (because "SGDEMO2:" does not start with "SGDEMO:").
  - The next part is the purpose for the message (which is a single byte.)  (This can be anything your game requires; like a player move, tone to play, message to display, etc.)
  - The next part is the major version of the application (which is a single byte.)
  - Next is any game parameter data (this can be empty).  (Like the square that was selected, new board layout, etc.)
  - Then is the ':' delimiter character.
  - Finally is the name of the flipper (as returned from furi_hal_version_get_name_ptr().)
- Broadcast packets should be truncated to MESSAGE_MAX_LEN (which is typically around 60 bytes.)
- Data in sent packets should not be trusted, always expect to get some malformed input.

## Adding subghz RX/TX to your project
- NOTE: Feel free to copy any code from this tutorial, as long as your license does not add any additional requirements for this tutorial. 

- Import the library:
```
    #include <lib/subghz/subghz_tx_rx_worker.h>
    #include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
```

- Define MESSAGE_MAX_LEN with the maximum size (in bytes) of a message.  Messages are typically UTF-8 encoded, but it is up to the sending application to decide.  I beleive the maximum for the CC1101 chip is in the low-60 bytes.  
```
  #define MESSAGE_MAX_LEN 60
```

- Define SUBGHZ_GAME_NAME to be the data you will send at the beginning part of each message.  This is optional, but it will help make your application more robust in the case where other unrelated applications are running on similar frequencies.
```
// This is sent at the beginning of all RF messages. You should make the SUBGHZ_GAME_NAME short but unique.
#define SUBGHZ_GAME_NAME "SGDEMO:"
```

- Create a callback method that will get invoked when data is available.  This callback method should just queue a message that data was detected.  Do not copy this code directly, but instead follow the pattern used in our application for queuing a mesage (and add a new "EventDataDetected" type.)
```
  static void rx_event_callback(void* ctx) { 
    // Queue a message that data was detected.
    AppContext* context = ctx;
    Event event = {.type = EventDataDetected};
    furi_message_queue_put(context->queue, &event, FuriWaitForever);
  }
```

- In your application entrypoint method: 

  - Decide on a frequency (if your application is global, you should provide a way to set it to a valid region):
  ```
      uint32_t frequency = 433920000;
  ```

  - Allocate the subghz TX/RX worker.
  ```
      demo_context->subghz_txrx = subghz_tx_rx_worker_alloc();
  ```

  - Initialize the list of devices
  ```
      subghz_devices_init();
  ```

  - Get a device references to the internal CC1101 Sub-GHz radio.
  ```
      const SubGhzDevice* device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);
      furi_assert(device);
  ```

  - Start the worker.
  ```
      bool worker_started = subghz_tx_rx_worker_start(demo_context->subghz_txrx, device, frequency);
  ```

  - If the worker failed (worker_started == false) then free resources and exit.

  - Register your callback method that will be invoked by the tx_rx_worker when it receives new data.
    - The second parameter should match the name of your function.  For example if your callback function is "static void rx_event_callback(void* ctx) { /* Queue got data event. */ }" then you would use "rx_event_callback". 
  ```
      subghz_tx_rx_worker_set_callback_have_read(
        demo_context->subghz_txrx,
        rx_event_callback,
        demo_context);
  ```

  -  All the subghz CLI apps disable charging; so our demo does it too.
  ```
      furi_hal_power_suppress_charge_enter();
  ```

  - At this point you are all configured for transmitting and receiving messages!

  - When your application exits, you should free all of the resources &amp; reenable charging.
  ```
    if(subghz_tx_rx_worker_is_running(demo_context->subghz_txrx)) {
        subghz_tx_rx_worker_stop(demo_context->subghz_txrx);
    }
    subghz_tx_rx_worker_free(demo_context->subghz_txrx);
    subghz_devices_deinit();
    furi_hal_power_suppress_charge_exit();
  ```

- Create a received_data method that will get invoked to read &amp; process new RX data.  
```
  static void receive_data(DemoContext* instance) {
    uint8_t message[MESSAGE_MAX_LEN] = {0};
    memset(message, 0x00, MESSAGE_MAX_LEN);
    size_t len = subghz_tx_rx_worker_read(instance->subghz_txrx, message, MESSAGE_MAX_LEN);
    
    // The data is now in message buffer!  
    
    // Typically you want to parse and validate the data.
  
    // If the message needs further processing, then queue a message with 
    // the parsed data set in the event properties.
  
    // Important -- If you allocate any memory and assign pointers in your event data,
    // make sure that the code that processes your event will free the memory!
  }
```

- In your furi_message_queue_get/switch(event.type) loop, add a case for EventDataDetected.  Call your receive_data method.
```
  case EventDataDetected:
    // Another Flipper sent us data! Process it, potentially queuing an event.
    receive_data(demo_context);
  break;
```

- In your furi_message_queue_get/switch(event.type) loop, you will typically want to add processing for the other events that are queued in your receive_data method. For example EventRemotePlayerMoved.

- If you want to send data, create a UTF8 buffer with the data to send, ensuring that the total buffer length is less than MESSAGE_MAX_LEN.  If your message is 7-bit ASCII (like A-Z a-z 0-9 symbols spaces) then you don't necessarily need to convert to UTF8 (since characters 0-127 are the same in both ASCII and UTF8.)  If you might have special characters (emojis, characters with accents, etc.) they you should convert to UTF8 first.
```
  static void subghz_demo_broadcast(DemoContext* demo_context, FuriString* buffer) {
    uint8_t* message = (uint8_t*)furi_string_get_cstr(buffer);

    // Make sure our message will fit into a packet; if not truncate it.
    size_t length = strlen((char*)message);
    if (length>MESSAGE_MAX_LEN) {
        message[MESSAGE_MAX_LEN-1] = 0;
        message[MESSAGE_MAX_LEN-2] = '\n';
        message[MESSAGE_MAX_LEN-3] = '\r';
        length = MESSAGE_MAX_LEN;
    }

    while(!subghz_tx_rx_worker_write(demo_context->subghz_txrx, message, length)) {
        // Wait a few milliseconds on failure before trying to send again.
        furi_delay_ms(20);
    }
  }
```

## Details about the project files
- application.fam
  - specifies the name of our application.
  - specifies the entry point for our application.
  - specifies we use the GUI.
  - specifies our icon is the subghz_demo.png file.
  - specifies our application can be found in the "Misc" category.

- subghz_demo.png
  - The icon for our application that shows up in the "Misc" folder.

- subghz_demo.c
  - This is the demo applcation.