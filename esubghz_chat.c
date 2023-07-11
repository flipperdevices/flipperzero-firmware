#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/modules/text_box.h>
#include <gui/modules/text_input.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#define APPLICATION_NAME "ESubGhzChat"

#define CHAT_BOX_STORE_SIZE 4096
#define TEXT_INPUT_STORE_SIZE 512

typedef struct {
	SceneManager *scene_manager;
	ViewDispatcher *view_dispatcher;
	TextBox *chat_box;
	FuriString *chat_box_store;
	TextInput *text_input;
	char text_input_store[TEXT_INPUT_STORE_SIZE + 1];
	bool encrypted;
	uint32_t frequency;
} ESubGhzChatState;

typedef enum {
	ESubGhzChatScene_FreqInput,
	ESubGhzChatScene_PassInput,
	ESubGhzChatScene_ChatInput,
	ESubGhzChatScene_ChatBox,
	ESubGhzChatScene_MAX
} ESubGhzChatScene;

typedef enum {
	ESubGhzChatView_Input,
	ESubGhzChatView_ChatBox,
} ESubGhzChatView;

typedef enum {
	ESubGhzChatEvent_FreqEntered,
	ESubGhzChatEvent_PassEntered,
	ESubGhzChatEvent_MsgEntered
} ESubGhzChatEvent;

static void freq_input_cb(void *context)
{
	furi_assert(context);
	ESubGhzChatState* state = context;

	furi_string_cat_printf(state->chat_box_store, "Frequency: %lu\n",
			state->frequency);

	scene_manager_handle_custom_event(state->scene_manager,
			ESubGhzChatEvent_FreqEntered);
}

static bool freq_input_validator(const char *text, FuriString *error,
		void *context)
{
	furi_assert(text);
	furi_assert(error);

	furi_assert(context);
	ESubGhzChatState* state = context;

        int ret = sscanf(text, "%lu", &(state->frequency));
	if (ret != 1) {
		furi_string_printf(error, "Please enter\nfrequency\nin Hz!");
		return false;
	}

	if (!furi_hal_subghz_is_frequency_valid(state->frequency)) {
		furi_string_printf(error, "Frequency\n%lu\n is invalid!",
				state->frequency);
		return false;
	}

	if (!furi_hal_subghz_is_tx_allowed(state->frequency)) {
		furi_string_printf(error, "TX forbidden\non frequency\n%lu!",
				state->frequency);
		return false;
	}

	return true;
}

static void pass_input_cb(void *context)
{
	furi_assert(context);
	ESubGhzChatState* state = context;

	if (strlen(state->text_input_store) == 0) {
		state->encrypted = false;
	} else {
		state->encrypted = true;
		// TODO
	}

	furi_string_cat_printf(state->chat_box_store, "Encrypted: %s\n",
			(state->encrypted ? "true" : "false"));

	scene_manager_handle_custom_event(state->scene_manager,
			ESubGhzChatEvent_PassEntered);
}

static void chat_input_cb(void *context)
{
	furi_assert(context);
	ESubGhzChatState* state = context;

	scene_manager_handle_custom_event(state->scene_manager,
			ESubGhzChatEvent_MsgEntered);
}

static void scene_on_enter_freq_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_freq_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	state->text_input_store[0] = 0;
	text_input_reset(state->text_input);
	text_input_set_result_callback(
			state->text_input,
			freq_input_cb,
			state,
			state->text_input_store,
			sizeof(state->text_input_store),
			true);
	text_input_set_validator(
			state->text_input,
			freq_input_validator,
			state);
	text_input_set_header_text(
			state->text_input,
			"Frequency");

	view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

static bool scene_on_event_freq_input(void* context, SceneManagerEvent event)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_event_freq_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	bool consumed = false;

	switch(event.type) {
	case SceneManagerEventTypeCustom:
		switch(event.event) {
		case ESubGhzChatEvent_FreqEntered:
			scene_manager_next_scene(state->scene_manager,
					ESubGhzChatScene_PassInput);
			consumed = true;
			break;
		}
		break;

	case SceneManagerEventTypeBack:
		view_dispatcher_stop(state->view_dispatcher);
		consumed = true;
		break;

	default:
		consumed = false;
		break;
	}

	return consumed;
}

static void scene_on_exit_freq_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_freq_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_input_reset(state->text_input);
}

static void scene_on_enter_pass_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_pass_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	state->text_input_store[0] = 0;
	text_input_reset(state->text_input);
	text_input_set_result_callback(
			state->text_input,
			pass_input_cb,
			state,
			state->text_input_store,
			sizeof(state->text_input_store),
			true);
	text_input_set_validator(
			state->text_input,
			NULL,
			NULL);
	text_input_set_header_text(
			state->text_input,
			"Password (empty for no encr.)");
	text_input_set_minimum_length(state->text_input, 0);

	view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

static bool scene_on_event_pass_input(void* context, SceneManagerEvent event)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_event_pass_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	bool consumed = false;

	switch(event.type) {
	case SceneManagerEventTypeCustom:
		switch(event.event) {
		case ESubGhzChatEvent_PassEntered:
			scene_manager_next_scene(state->scene_manager,
					ESubGhzChatScene_ChatInput);
			consumed = true;
			break;
		}
		break;

	case SceneManagerEventTypeBack:
		view_dispatcher_stop(state->view_dispatcher);
		consumed = true;
		break;

	default:
		consumed = false;
		break;
	}

	return consumed;
}

static void scene_on_exit_pass_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_pass_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_input_reset(state->text_input);
}

static void scene_on_enter_chat_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_chat_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	state->text_input_store[0] = 0;
	text_input_reset(state->text_input);
	text_input_set_result_callback(
			state->text_input,
			chat_input_cb,
			state,
			state->text_input_store,
			sizeof(state->text_input_store),
			true);
	text_input_set_validator(
			state->text_input,
			NULL,
			NULL);
	text_input_set_header_text(
			state->text_input,
			"Message");
	text_input_set_minimum_length(state->text_input, 0);

	view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

static bool scene_on_event_chat_input(void* context, SceneManagerEvent event)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_event_chat_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	bool consumed = false;

	switch(event.type) {
	case SceneManagerEventTypeCustom:
		switch(event.event) {
		case ESubGhzChatEvent_MsgEntered:
			scene_manager_next_scene(state->scene_manager,
					ESubGhzChatScene_ChatBox);
			consumed = true;
			break;
		}
		break;

	case SceneManagerEventTypeBack:
		view_dispatcher_stop(state->view_dispatcher);
		consumed = true;
		break;

	default:
		consumed = false;
		break;
	}

	return consumed;
}

static void scene_on_exit_chat_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_chat_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_input_reset(state->text_input);
}

static void scene_on_enter_chat_box(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_chat_box");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_box_reset(state->chat_box);
	text_box_set_text(state->chat_box,
			furi_string_get_cstr(state->chat_box_store));
	text_box_set_focus(state->chat_box, TextBoxFocusEnd);

	view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_ChatBox);
}

static bool scene_on_event_chat_box(void* context, SceneManagerEvent event)
{
	UNUSED(event);

	FURI_LOG_T(APPLICATION_NAME, "scene_on_event_chat_box");
	furi_assert(context);
	// TODO
	return false;
}

static void scene_on_exit_chat_box(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_chat_box");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_box_reset(state->chat_box);
}

static void (*const esubghz_chat_scene_on_enter_handlers[])(void*) = {
	scene_on_enter_freq_input,
	scene_on_enter_pass_input,
	scene_on_enter_chat_input,
	scene_on_enter_chat_box
};

static bool (*const esubghz_chat_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
	scene_on_event_freq_input,
	scene_on_event_pass_input,
	scene_on_event_chat_input,
	scene_on_event_chat_box
};

static void (*const esubghz_chat_scene_on_exit_handlers[])(void*) = {
	scene_on_exit_freq_input,
	scene_on_exit_pass_input,
	scene_on_exit_chat_input,
	scene_on_exit_chat_box
};

static const SceneManagerHandlers esubghz_chat_scene_event_handlers = {
	.on_enter_handlers = esubghz_chat_scene_on_enter_handlers,
	.on_event_handlers = esubghz_chat_scene_on_event_handlers,
	.on_exit_handlers = esubghz_chat_scene_on_exit_handlers,
	.scene_num = ESubGhzChatScene_MAX};

static bool esubghz_chat_custom_event_callback(void* context, uint32_t event)
{
	FURI_LOG_T(APPLICATION_NAME, "esubghz_chat_custom_event_callback");
	furi_assert(context);
	ESubGhzChatState* state = context;
	return scene_manager_handle_custom_event(state->scene_manager, event);
}

static bool esubghz_chat_navigation_event_callback(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "esubghz_chat_navigation_event_callback");
	furi_assert(context);
	ESubGhzChatState* state = context;
	return scene_manager_handle_back_event(state->scene_manager);
}

static bool chat_box_alloc(ESubGhzChatState *state)
{
	furi_assert(state);

	state->chat_box = text_box_alloc();
	if (state->chat_box == NULL) {
		return false;
	}

	state->chat_box_store = furi_string_alloc();
	if (state->chat_box_store == NULL) {
		text_box_free(state->chat_box);
		return false;
	}

	furi_string_reserve(state->chat_box_store, CHAT_BOX_STORE_SIZE);
	furi_string_set_char(state->chat_box_store, 0, 0);
	text_box_set_text(state->chat_box,
			furi_string_get_cstr(state->chat_box_store));
	text_box_set_focus(state->chat_box, TextBoxFocusEnd);

	return true;
}

static void chat_box_free(ESubGhzChatState *state)
{
	furi_assert(state);

	text_box_free(state->chat_box);
	furi_string_free(state->chat_box_store);
}

int32_t esubghz_chat(void)
{
	int32_t err = -1;

	FURI_LOG_I(APPLICATION_NAME, "Starting...");

	ESubGhzChatState *state = malloc(sizeof(ESubGhzChatState));
	if (state == NULL) {
		goto err_alloc;
	}
	memset(state, 0, sizeof(*state));

	state->scene_manager = scene_manager_alloc(
			&esubghz_chat_scene_event_handlers, state);
	if (state->scene_manager == NULL) {
		goto err_alloc_sm;
	}

	state->view_dispatcher = view_dispatcher_alloc();
	if (state->view_dispatcher == NULL) {
		goto err_alloc_vd;
	}

	state->text_input = text_input_alloc();
	if (state->text_input == NULL) {
		goto err_alloc_ti;
	}

	if (!chat_box_alloc(state)) {
		goto err_alloc_cb;
	}

	view_dispatcher_enable_queue(state->view_dispatcher);

	view_dispatcher_set_event_callback_context(state->view_dispatcher, state);
	view_dispatcher_set_custom_event_callback(
			state->view_dispatcher,
			esubghz_chat_custom_event_callback);
	view_dispatcher_set_navigation_event_callback(
			state->view_dispatcher,
			esubghz_chat_navigation_event_callback);

	view_dispatcher_add_view(state->view_dispatcher, ESubGhzChatView_Input,
			text_input_get_view(state->text_input));
	view_dispatcher_add_view(state->view_dispatcher, ESubGhzChatView_ChatBox,
			text_box_get_view(state->chat_box));

	/* no error handling here, don't know how */
	Gui *gui = furi_record_open(RECORD_GUI);
	view_dispatcher_attach_to_gui(state->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

	scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_FreqInput);
	view_dispatcher_run(state->view_dispatcher);

	err = 0;

    	furi_record_close(RECORD_GUI);
	view_dispatcher_remove_view(state->view_dispatcher, ESubGhzChatView_Input);
	view_dispatcher_remove_view(state->view_dispatcher, ESubGhzChatView_ChatBox);

	chat_box_free(state);

err_alloc_cb:
	text_input_free(state->text_input);

err_alloc_ti:
	view_dispatcher_free(state->view_dispatcher);

err_alloc_vd:
	scene_manager_free(state->scene_manager);

err_alloc_sm:
	free(state);

err_alloc:
	if (err != 0) {
		FURI_LOG_E(APPLICATION_NAME, "Failed to launch (alloc error)!");
	} else {
		FURI_LOG_I(APPLICATION_NAME, "Clean exit.");
	}

	return err;
}
