#include "f0forth_i.h"
#include "f0forth_cli.h"

#define F0FORTH_BG_EXIT_FLAG 0x01
#define F0FORTH_BG_ARGS "run_in_background"
#define F0FORTH_BG_FLAG_CHECK_DELAY 1000

static void f0forth_wait_for_exit(void)
{
	while ((furi_thread_flags_get() & F0FORTH_BG_EXIT_FLAG) == 0) {
		furi_delay_ms(F0FORTH_BG_FLAG_CHECK_DELAY);
	}
}

static bool f0forth_navigation_event_callback(void* context)
{
	UNUSED(context);

	return false;
}

static void f0forth_setup_gui(F0ForthState *f0f_state)
{
	furi_string_printf(f0f_state->text_box_store, "Use the \"" F0FORTH_CMD
			"\" command on the CLI.\n");

	text_box_set_text(f0f_state->text_box,
			furi_string_get_cstr(f0f_state->text_box_store));
	text_box_set_focus(f0f_state->text_box, TextBoxFocusEnd);

	view_dispatcher_enable_queue(f0f_state->view_dispatcher);

	view_dispatcher_set_event_callback_context(f0f_state->view_dispatcher,
			f0f_state);
	view_dispatcher_set_navigation_event_callback(f0f_state->view_dispatcher,
			f0forth_navigation_event_callback);

	view_dispatcher_add_view(f0f_state->view_dispatcher,
			F0ForthView_TextBox,
			text_box_get_view(f0f_state->text_box));

	Gui *gui = furi_record_open(RECORD_GUI);
	view_dispatcher_attach_to_gui(f0f_state->view_dispatcher, gui,
			ViewDispatcherTypeFullscreen);

	view_dispatcher_switch_to_view(f0f_state->view_dispatcher,
			F0ForthView_TextBox);
}

static void f0forth_run_gui(F0ForthState *f0f_state)
{
	view_dispatcher_run(f0f_state->view_dispatcher);

	furi_string_cat_printf(f0f_state->text_box_store, "Waiting for all "
			"CLI commands to terminate...\n");
	text_box_set_text(f0f_state->text_box,
			furi_string_get_cstr(f0f_state->text_box_store));
	text_box_set_focus(f0f_state->text_box, TextBoxFocusEnd);
}

static void f0forth_terminate_gui(F0ForthState *f0f_state)
{
	furi_record_close(RECORD_GUI);

	view_dispatcher_remove_view(f0f_state->view_dispatcher,
			F0ForthView_TextBox);
}

static void f0forth_run_bg(F0ForthState *f0f_state)
{
	UNUSED(f0f_state);

	f0forth_wait_for_exit();
}

int32_t f0forth(const char *args)
{
	F0ForthState *f0f_state = malloc(sizeof(F0ForthState));

	f0f_state->view_dispatcher = view_dispatcher_alloc();
	f0f_state->text_box = text_box_alloc();
	f0f_state->text_box_store = furi_string_alloc();
	f0f_state->cli_running_mutex = furi_mutex_alloc(FuriMutexTypeNormal);

	bool run_in_bg = false;
	if (args != NULL) {
		run_in_bg = (strcmp(args, F0FORTH_BG_ARGS) == 0);
	}

	if (!run_in_bg) {
		f0forth_setup_gui(f0f_state);
	}

	Cli *cli = furi_record_open(RECORD_CLI);
	cli_add_command(cli, F0FORTH_CMD, CliCommandFlagParallelSafe,
			f0forth_cmd, f0f_state);
	furi_record_close(RECORD_CLI);

	if (run_in_bg) {
		f0forth_run_bg(f0f_state);
	} else {
		f0forth_run_gui(f0f_state);
	}

	cli = furi_record_open(RECORD_CLI);
	cli_delete_command(cli, F0FORTH_CMD);
	furi_record_close(RECORD_CLI);

	furi_check(furi_mutex_acquire(f0f_state->cli_running_mutex,
				FuriWaitForever) == FuriStatusOk);
	furi_check(furi_mutex_release(f0f_state->cli_running_mutex) ==
			FuriStatusOk);

	if (!run_in_bg) {
		f0forth_terminate_gui(f0f_state);
	}

	furi_string_free(f0f_state->text_box_store);
	text_box_free(f0f_state->text_box);
	view_dispatcher_free(f0f_state->view_dispatcher);
	furi_mutex_free(f0f_state->cli_running_mutex);
	free(f0f_state);

	return 0;
}
