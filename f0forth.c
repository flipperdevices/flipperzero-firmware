#include "f0forth_i.h"
#include "f0forth_cli.h"

static bool f0forth_navigation_event_callback(void* context)
{
	UNUSED(context);

	return false;
}

int32_t f0forth(void)
{
	F0ForthState *f0f_state = malloc(sizeof(F0ForthState));

	f0f_state->view_dispatcher = view_dispatcher_alloc();
	f0f_state->text_box = text_box_alloc();
	f0f_state->text_box_store = furi_string_alloc();
	f0f_state->cli_running_mutex = furi_mutex_alloc(FuriMutexTypeNormal);

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
 
	Cli *cli = furi_record_open(RECORD_CLI);
	cli_add_command(cli, F0FORTH_CMD, CliCommandFlagParallelSafe,
			f0forth_cmd, f0f_state);
	furi_record_close(RECORD_CLI);

	view_dispatcher_run(f0f_state->view_dispatcher);

	furi_string_cat_printf(f0f_state->text_box_store, "Waiting for all "
			"CLI commands to terminate...\n");
	text_box_set_text(f0f_state->text_box,
			furi_string_get_cstr(f0f_state->text_box_store));
	text_box_set_focus(f0f_state->text_box, TextBoxFocusEnd);

	cli = furi_record_open(RECORD_CLI);
	cli_delete_command(cli, F0FORTH_CMD);
	furi_record_close(RECORD_CLI);

	furi_check(furi_mutex_acquire(f0f_state->cli_running_mutex,
				FuriWaitForever) == FuriStatusOk);
	furi_check(furi_mutex_release(f0f_state->cli_running_mutex) ==
			FuriStatusOk);

	furi_record_close(RECORD_GUI);

	view_dispatcher_remove_view(f0f_state->view_dispatcher,
			F0ForthView_TextBox);

	furi_string_free(f0f_state->text_box_store);
	text_box_free(f0f_state->text_box);
	view_dispatcher_free(f0f_state->view_dispatcher);
	furi_mutex_free(f0f_state->cli_running_mutex);
	free(f0f_state);

	return 0;
}
