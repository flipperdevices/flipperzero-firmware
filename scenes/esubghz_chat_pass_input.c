#include "../esubghz_chat_i.h"

/* Sends PassEntered event to scene manager and displays whether or not
 * encryption has been enabled in the text box. Also clears the text input
 * buffer to remove the password and starts the Sub-GHz worker. After starting
 * the worker a join message is transmitted. */
static void pass_input_cb(void *context)
{
	furi_assert(context);
	ESubGhzChatState* state = context;

	furi_string_cat_printf(state->chat_box_store, "\nEncrypted: %s",
			(state->encrypted ? "yes" : "no"));

	/* clear the text input buffer to remove the password */
	crypto_explicit_bzero(state->text_input_store,
			sizeof(state->text_input_store));

	subghz_tx_rx_worker_start(state->subghz_worker, state->subghz_device,
			state->frequency);

	/* concatenate the name prefix and join message */
	furi_string_set(state->msg_input, state->name_prefix);
	furi_string_cat_str(state->msg_input, " joined chat.");

	/* encrypt and transmit message */
	tx_msg_input(state);

	/* clear message input buffer */
	furi_string_set_char(state->msg_input, 0, 0);

	scene_manager_handle_custom_event(state->scene_manager,
			ESubGhzChatEvent_PassEntered);
}

/* If a password was entered this derives a key from the password using a
 * single pass of SHA256 and initiates the AES-GCM context for encryption. If
 * the initiation fails, the password is rejected. */
static bool pass_input_validator(const char *text, FuriString *error,
		void *context)
{
	furi_assert(text);
	furi_assert(error);

	furi_assert(context);
	ESubGhzChatState* state = context;

#ifdef FW_ORIGIN_Official
	if (strlen(text) == 0) {
		furi_string_printf(error, "Enter a\npassword!");
		return false;
	}

	if (strcmp(text, " ") == 0) {
#else /* FW_ORIGIN_Official */
	if (strlen(text) == 0) {
#endif /* FW_ORIGIN_Official */
		state->encrypted = false;
		return true;
	}

	unsigned char key[KEY_BITS / 8];

	/* derive a key from the password */
	sha256((unsigned char *) text, strlen(text), key);

	/* initiate the crypto context */
	bool ret = crypto_ctx_set_key(state->crypto_ctx, key);

	/* cleanup */
	crypto_explicit_bzero(key, sizeof(key));

	if (!ret) {
		crypto_ctx_clear(state->crypto_ctx);
		furi_string_printf(error, "Failed to\nset key!");
		return false;
	}

	state->encrypted = true;

	return true;
}

/* Prepares the password input scene. */
void scene_on_enter_pass_input(void* context)
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
			pass_input_validator,
			state);
	text_input_set_header_text(
			state->text_input,
#ifdef FW_ORIGIN_Official
			"Password (space for no encr.)");
#else /* FW_ORIGIN_Official */
			"Password (empty for no encr.)");
	text_input_set_minimum_length(state->text_input, 0);
#endif /* FW_ORIGIN_Official */

	view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

/* Handles scene manager events for the password input scene. */
bool scene_on_event_pass_input(void* context, SceneManagerEvent event)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_event_pass_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	bool consumed = false;

	switch(event.type) {
	case SceneManagerEventTypeCustom:
		switch(event.event) {
		/* switch to message input scene */
		case ESubGhzChatEvent_PassEntered:
			scene_manager_next_scene(state->scene_manager,
					ESubGhzChatScene_ChatInput);
			consumed = true;
			break;
		}
		break;

	case SceneManagerEventTypeBack:
		/* stop the application if the user presses back here */
		view_dispatcher_stop(state->view_dispatcher);
		consumed = true;
		break;

	default:
		consumed = false;
		break;
	}

	return consumed;
}

/* Cleans up the password input scene. */
void scene_on_exit_pass_input(void* context)
{
	FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_pass_input");

	furi_assert(context);
	ESubGhzChatState* state = context;

	text_input_reset(state->text_input);
}
