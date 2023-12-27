# Developer documentation
This page is about the code design of the
game, the purpose of the various components
and the interaction between them.

## Entry point, init and destroy functions
The initial functions are in the file `entry_point.c`.
The main function is `matagotchi_app()`.

The init function is `context_alloc()` and the
corresponding destroy function is `context_free()`.
`context_alloc()` init the data structures and
the GUI, through the function `init_gui()`.

## Threads
There are 3 threads in the application.
The starting functions for them are in `threads.c`.

### Main thread
It is the same thread that starts when entering
the application.
Once the init is completed it is confined in the
function `main_thread()`. The purpose is to start
and stop the secondary thread. It starts it, then
blocks waiting the UI thread to end and once finished
notifies the secondary thread to stop.

### Secondary thread
This thread is started by the main thread. Its
purpose is to perform background operations and to
process user inputs. The main function for
this thread is `secondary_thread()`.

It receives messages from the other two threads.
The main thread sends shutdown requests. The
UI thread sends actions to be performed because
the user selected something on the UI.

If no message is received, this thread performs
background operations.

### UI thread
This is the thread where the UI lives.
It is started by the function call
`start_gui_and_block()` in the main thread.
It ends when the user tries to go back from
the main screen. When the back button is pressed,
the view dispatcher is stopped with the function
call `view_dispatcher_stop()` in the file
`src/scene/main_scene.c`.

When the view dispatcher stops, the
`start_gui_and_block()` function returns, freeing
the main thread.

## Game parameters
The game can be tuned by changing some configurations.
It is possile to modify the speed of the events,
the frequency and the probability of all the
game features.

All configurations are in the file `constants.h`.

## GUI
The application makes use of `view_dispatcher`,
`scene_manager` and `modules`.

The `view_dispatcher` is responsible for the low
level APIs. It handles the user inputs and pass them
to the high level APIs such as the modules.

For every visible screen, the `view_dispatcher` registers
the `module` element to show.

The `module` is a screen with some predefined graphical
elements.

The `scene_manager` is a collection of callback functions
that can be loaded or unloaded to pass from a scene
to another one. This could be done manually with the
`view_dispatcher` APIs, but it's easier to associate to
each screen a set of callbacks to use and ask the
`scene_manager` to swap the when moving from one screen
to another.
