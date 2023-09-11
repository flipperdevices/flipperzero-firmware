# Flipper Zero Background Loader

This is a plugin for the Flipper Zero that allows compatible apps to be loaded
and to remain running in the background.

Once started the plugin will display a file browser that allows you to select
an application's FAP file. If the application is not already running, it will
be loaded. The application then has the choice of terminating normally, in
which case the loader will unload the application as normal, or to relinquish
control of the GUI and to keep active in the background, in which case the
loader will not unload the application.

If an already running application is selected, the loader will inform the
application that it can reassert control over the GUI and continue in the
foreground. Once again, the reattached application can chose to terminate
normally or to keep active in the background.

Make sure you only select compatible applications (see below) and that you
never start an application more than once at the same time (by launching it in
the background and then selecting it in the Flipper's apps menu).

## Warning

This plugin is in the early stages of development. It will inevitably have
bugs. You have been warned.

## Writing Compatible Apps

An application needs to be compatible with this loader. Compatible apps must
check their startup parameters to determine if they have been launched via the
background loader. If so, they must inform the loader when they wish to keep
running in the background. If they decide to keep running in the background,
they must listen for the loader's signal that they are being reopened and thus
allowed to use the GUI again.

The loader's API can be found in `bgloader_api.h`. In the simplest case, the
app will need to do the following:

1. Check for the loader's special parameter. The parameter will have the form
   `run_in_background:<path to the app's FAP file>`.
2. Retrieve the FAP file path from the parameter and use it to access the
   `BGLoaderApp` structure. This can be done by calling
   `furi_record_open(<FAP file path>)`.
3. Once the app is ready to go into the background it needs to detach from the
   GUI and send a `BGLoaderMessage` with type
   `BGLoaderMessageType_LoaderBackground` to the `to_loader` queue in the
   `BGLoaderApp` structure.
4. While in the background the app need to check the `to_app` queue in the
   `BGLoaderApp` structure for a message of the type
   `BGLoaderMessageType_AppReattached`. This message informs the app that it is
   being reattached and can attach itself to the GUI again.
5. To terminate, the app just exits normally.

An example of what a compatible app might look like is given below:

```
static const char *get_bgloader_app_path(const char *args)
{
	size_t base_args_len = strlen(APP_BASE_ARGS);

	return (args + base_args_len + 1);
}

static bool run_with_bgloader(const char *args)
{
	size_t base_args_len = strlen(APP_BASE_ARGS);

	if (args == NULL) {
		return false;
	}

	if (strncmp(args, APP_BASE_ARGS, base_args_len) != 0) {
		return false;
	}

	if (strlen(args) < base_args_len + 2) {
		return false;
	}

	if (args[base_args_len] != ':') {
		return false;
	}

	const char *app_path = get_bgloader_app_path(args);
	return furi_record_exists(app_path);
}

static void bgloader_loop(AppState *state, const char *bg_app_path)
{
	while (true) {
		view_dispatcher_run(state->view_dispatcher);

		// 2.
		BGLoaderApp *bg_app = furi_record_open(bg_app_path);

		// exit_for_real is set elsewhere to indicate the app is ready
		// to terminate
		if (state->exit_for_real) {
			// 5.
			// exit normally
			furi_record_close(bg_app_path);
			break;
		}

		// 3.
		// signal loader that we're ready to go to background
		BGLoaderMessage msg;
		msg.type = BGLoaderMessageType_LoaderBackground;
		furi_check(furi_message_queue_put(bg_app->to_loader, &msg,
					FuriWaitForever) == FuriStatusOk);

		detach_from_gui(state);

		// 4.
		// wait for loader to wake us up again
		furi_check(furi_message_queue_get(bg_app->to_app, &msg,
					FuriWaitForever) == FuriStatusOk);
		switch(msg.type) {
		case BGLoaderMessageType_AppReattached:
			break;
		default:
			furi_check(0);
		}

		furi_record_close(bg_app_path);

		attach_to_gui(state);
	}

}

int32_t app_entry_point(const char *args)
{
	...
	// regular setup
	...
	// 1.
	bool run_with_bgl = run_with_bgloader(args);
	...
	if (run_with_bgl) {
		const char *bg_app_path = get_bgloader_app_path(args);
		bgloader_loop(state, bg_app_path);
	} else {
		// regular main loop (e.g. view_dispatcher_run())
	}
	...
	// regular teardown
	...
}
```

For examples of compatible apps check out
[f0forth](https://github.com/twisted-pear/f0forth) or
[ESubGhz Chat](https://github.com/twisted-pear/esubghz_chat).

## Acknowledgements

Thanks to [Willy-JL](https://github.com/Willy-JL) for coming up with an idea
for how to store the pointers to an application's data structures and for how
to signal an application's thread to terminate.

The app icon was made by [xMasterX](https://github.com/xMasterX).
