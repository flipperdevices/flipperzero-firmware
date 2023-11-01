# GPIO POLLING DEMO
## Introduction
This is a "hello world" demonstration of reading a GPIO pin using polling.  For this demo, we connect a 1K resistor between pin GND (8) and pin A4 (4). When the resistor is not connected it says "Hello" and when connected it says "World". In theory you can use a wire, but I like to use a resistor in case I accidently run a demo that does GPIO output with the pins still connected.


## Installation Directions
This project is intended to be overlayed on top of an existing firmware repo.
- Clone, Build & Deploy an existing flipper zero firmware repo.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "gpio_polling_demo" [folder](..) to the \applications\plugins\gpio_polling_demo folder in your firmware.
- Build &amp; deploy the firmware.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also extract the gpio_polling_demo.FAP from resources.tar file and use qFlipper to copy the file to the SD Card/apps/Misc folder.


## Running the updated firmware
These directions assume you are starting at the flipper desktop.  If not, please press the back button until you are at the desktop.

- Press the OK button on the flipper to pull up the main menu.
- Choose "Applications" from the menu.
- Choose "Misc" from the sub-menu.
- Choose "Gpio Polling Demo"

- The flipper should say "Hello".
- A counter should keep increasing and it should show if value is "Even" or "Odd".
- A tone should play.

- Connect a 1K resistor between pin GND (8) and pin A4 (4).
- The message should change to "World".
- Remve the resistor and the message should change back to "Hello".

- Press the BACK button to exit.


## How it works
- application.fam
  - specifies the name of our application.
  - specifies the entry point for our application.
  - specifies we use the GUI.
  - specifies our icon is the gpio_polling_demo.png file.
  - specifies our application can be found in the "Misc" category.

- gpio_polling_demo.png
  - The icon for our application that shows up in the "Misc" folder.

- gpio_polling_demo_app.c
  - We #include the libraries we referece.
  - We define DemoEventType (so we know the reason for events)
  - We define DemoEvent (which has the event type and its data) used for adding to an event queue.
  - We define DemoData (data used by our application)
  - We define DemoContext (pointer to event queue, pointer to application data, and pointer to mutex [to safely access the data])
  - We define demo_message_pin for the GPIO pin that we will be using.
  - We create a gpio_polling_demo_input_callback(...) method that queues a key event.
  - We create a gpio_polling_demo_tick(...) method that queues a tick event.
  - We create a gpio_polling_demo_render_callback(...) method that does the screen rendering.
    - We acquire the mutex, so that no other thread can modify the data.
      - If unsuccessful, we don't render anything this frame.
    - From our context, we get the application data, setting "show_hello" to true if the pin was grounded when the data was last updated.
    - From our context, we get the application data, setting "counter" to the counter from when the data was last updated.
    - We set "even_counter" to true is the counter value is even, otherwise we set it to false.  
    - We select the Primary font.  We render the text "Hello" (if pin was not grounded) or "World" (pin was grounded.)
    - We put the lowest 4 digits of counter value into the data buffer
    - We concatenate the text "Even" or "Odd" to the data buffer.
    - We select the Secondary font.  We render the data buffer.
    - We release the mutex, so other threads may modify the data.
    - We try to acquire the speaker.
      - If the speaker is available, we play a tone with the frequency based on the counter.
  - We create gpio_polling_demo_update_pin_status(...) method that updates out application data.
    - We increment the counter.
    - We use uri_hal_gpio_read(...) to get the status of the GPIO pin. &lt;*********
  - We create the entrypoint gpio_polling_demo_app(...) method
    - We configure our initial data state
    - We set demo_message_pin to gpio_ext_pa4.  &lt;**********
      - For a different GPIO pin, see \firmware\targets\f7\furi_hal\furi_hal_resources.c
    - We set the pin configuration   &lt;**********
      - The mode of the pin is GpioModeInput (so it is an input pin) 
      - The pull of the pin is GpioPullUp (so it has VCC by default via a pullup-resistor)
        - You can also use GpioPullDown (so it has GND by default via a pulldown-resistor) then connect resitior between the 3V3 pin (9) and A4 (4).
        - You can also use GpioPullNo (so it is floating).  Then you should provide your own pull-up/down resistor.
    - We create a queue for events.
    - We setup view_port_draw_callback_set(...) to invoke gpio_polling_demo_render_callback when rendering should happen.
    - We setup view_port_input_callback_set(...) to invoke gpio_polling_demo_input_callback when button is pressed.
    - We open GUI and register view_port.
    - We setup furi_timer_alloc(...) to allocate a timer to ivoke gpio_polling_demo_tick on each tick.
    - We start the timer with 250 millisecond ticks.
    - We create a message pump loop
      - We get an event from the queue.
        - If it is key message.
          - If it is a short press of back key, we set processing=false which will exit our message loop.
        - If it is a tick message.
          - We acquire the mutex.
          - We invoke gpio_polling_demo_update_pin_status(...) to update the data, polling the GPIO pin.  &lt;**********
          - We release the mutex.
    - The message loop continues until processing is false.
    - We set the GPIO pin to GpioPullNo (so that the pin is no longer set to GND or VCC.)
    - We release the rest of our application resources.
    - We exit the program.
    