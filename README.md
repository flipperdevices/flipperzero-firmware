# Flipper Zero Background Loader

This is a plugin for the Flipper Zero that allows compatible apps to be loaded
in the background.

Once started the plugin will display a file browser that allows you to select
an application's FAP file. If the application is not already running, it will
be loaded in the background. If it is running in the background it will be
terminated.

Make sure you only select compatible applications (see below) and that you
never start an application more than once at the same time (by launching it in
the background and then selecting it in the Flipper's apps menu).

## Warning

This plugin is in the early stages of development. It will inevitably have
bugs. You have been warned.

## Writing Compatible Apps

An application needs to be compatible with this loader in order to be launched
in the background. Compatible apps are not allowed to use the GUI and must have
a thread running that checks if the app is to be terminated. In addition, a
compatible app must check for a special parameter passed to its entry point
that determines if it will be launched in the background.

In the simplest case the app will need to conform to the following:

```
...
#define APP_BG_PARAM "run_in_background"
#define APP_EXIT_FLAG 0x01

static void app_wait_for_exit(void)
{
	while ((furi_thread_flags_get() & APP_EXIT_FLAG) == 0) {
		furi_delay_ms(1000);
	}
}
...
int32_t app_entry_point(const char *args)
{
	if ((args != NULL) && (strcmp(args, APP_BG_PARAM) == 0)) {
		...
		// do setup for running in background
		// call app_wait_for_exit() somewhere
		// after app_wait_for_exit() make sure to free all data
		// structures and properly deinit your app
		...
	}
}
```

Note that the delay when checking for the `APP_EXIT_FLAG` is _not_ optional.
You may get away with a smaller delay but not having one will cause the loader
to hang.

For an example of a compatible app check out
[f0forth](https://github.com/twisted-pear/f0forth).

## Acknowledgements

Thanks to [Willy-JL](https://github.com/Willy-JL) for coming up with an idea
for how to store the pointers to an application's data structures and for how
to signal an application's thread to terminate.
