# BASIC DEMO
## Introduction
This is a basic application for the Flipper Zero.  The goal of this project is to use it as a starting point for other applications.


## Installation Directions
This project is intended to be overlayed on top of an existing firmware repo.
- Clone, Build & Deploy an existing flipper zero firmware repo.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "basic_demo" [folder](..) to the \applications\plugins\basic_demo folder in your firmware.
- Build &amp; deploy the firmware.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also extract the basic_demo.FAP from resources.tar file and use qFlipper to copy the file to the SD Card/apps/Misc folder.


## Running the updated firmware
These directions assume you are starting at the flipper desktop.  If not, please press the back button until you are at the desktop.

- Press the OK button on the flipper to pull up the main menu.
- Choose "Applications" from the menu.
- Choose "Misc" from the sub-menu.
- Choose "Basic Demo"

- The flipper should say "Basic Demo".

- Press the BACK button to exit.


## How it works
- application.fam
  - specifies the name of our application.
  - specifies the entry point for our application.
  - specifies we use the GUI.
  - specifies our icon is the basic_demo.png file.
  - specifies our application can be found in the "Misc" category.

- basic_demo.png
  - The icon for our application.

- basic_demo_app.c
  - We #include the libraries we referece.
  - We define DemoEventType (so we know the reason for events)
  - We define DemoEvent (which has the event type and its data) used for adding to an event queue.
  - We define DemoData (data used by our application)
  - We define DemoContext (pointer to event queue, pointer to application data, and pointer to mutex [to safely access the data])
  - We create a basic_demo_input_callback(...) method that queues a key event.
  - We create a basic_demo_render_callback(...) method that does the screen rendering.
    - We acquire the mutex, so that no other thread can modify the data.
      - If unsuccessful, we don't render anything this frame.
    - We select the Primary font.  We render the text "Basic Demo".
    - We release the mutex, so other threads may modify the data.
  - We create the entrypoint basic_demo_app(...) method
    - We configure our initial data state
    - We create a queue for events.
    - We setup view_port_draw_callback_set(...) to invoke basic_demo_render_callback when rendering should happen.
    - We setup view_port_input_callback_set(...) to invoke basic_demo_input_callback when button is pressed.
    - We open GUI and register view_port.
    - We create a message pump loop
      - We get an event from the queue.
        - If it is key message.
          - If it is a short press of back key, we set processing=false which will exit our message loop.
    - The message loop continues until processing is false.
    - We release our application resources.
    - We exit the program.
    