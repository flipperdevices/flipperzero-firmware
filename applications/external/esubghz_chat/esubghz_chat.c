#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <gui/gui.h>
#include <gui/modules/text_box.h>
#include <gui/modules/text_input.h>
#include <gui/view_dispatcher_i.h>
#include <gui/view_port_i.h>
#include <gui/scene_manager.h>
#include <toolbox/sha256.h>
#include <notification/notification_messages.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
#include <lib/subghz/subghz_tx_rx_worker.h>

#include "esubghz_chat_icons.h"

#include "crypto/gcm.h"

#define APPLICATION_NAME "ESubGhzChat"

#define DEFAULT_FREQ 433920000

#define KEY_BITS 256
#define IV_BYTES 12
#define TAG_BYTES 16

#define RX_TX_BUFFER_SIZE 1024

#define CHAT_BOX_STORE_SIZE 4096
#define TEXT_INPUT_STORE_SIZE 256

#define TICK_INTERVAL 50
#define MESSAGE_COMPLETION_TIMEOUT 500
#define TIMEOUT_BETWEEN_MESSAGES 500
#define CHAT_LEAVE_DELAY 10

#define KBD_UNLOCK_CNT 3
#define KBD_UNLOCK_TIMEOUT 1000

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    NotificationApp* notification;

    // UI elements
    TextBox* chat_box;
    FuriString* chat_box_store;
    TextInput* text_input;
    char text_input_store[TEXT_INPUT_STORE_SIZE + 1];

    // for Sub-GHz
    uint32_t frequency;
    SubGhzTxRxWorker* subghz_worker;
    const SubGhzDevice* subghz_device;

    // message assembly before TX
    FuriString* name_prefix;
    FuriString* msg_input;

    // encryption
    bool encrypted;
    gcm_context gcm_ctx;

    // RX and TX buffers
    uint8_t rx_buffer[RX_TX_BUFFER_SIZE];
    uint8_t tx_buffer[RX_TX_BUFFER_SIZE];
    char rx_str_buffer[RX_TX_BUFFER_SIZE + 1];
    volatile uint32_t last_time_rx_data;

    // for locking
    ViewPortDrawCallback orig_draw_cb;
    ViewPortInputCallback orig_input_cb;
    bool kbd_locked;
    uint32_t kbd_lock_msg_ticks;
    uint8_t kbd_lock_count;
    bool kbd_ok_input_ongoing;
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

/* Function to clear sensitive memory. */
static void esubghz_chat_explicit_bzero(void* s, size_t len) {
    memset(s, 0, len);
    asm volatile("" ::: "memory");
}

/* Callback for RX events from the Sub-GHz worker. Records the current ticks as
 * the time of the last reception. */
static void have_read_cb(void* context) {
    furi_assert(context);
    ESubGhzChatState* state = context;

    state->last_time_rx_data = furi_get_tick();
}

/* Decrypts a message for post_rx(). */
static bool post_rx_decrypt(ESubGhzChatState* state, size_t rx_size) {
    if(rx_size < IV_BYTES + TAG_BYTES + 1) {
        return false;
    }

    int ret = gcm_auth_decrypt(
        &(state->gcm_ctx),
        state->rx_buffer,
        IV_BYTES,
        NULL,
        0,
        state->rx_buffer + IV_BYTES,
        (uint8_t*)state->rx_str_buffer,
        rx_size - (IV_BYTES + TAG_BYTES),
        state->rx_buffer + rx_size - TAG_BYTES,
        TAG_BYTES);
    state->rx_str_buffer[rx_size - (IV_BYTES + TAG_BYTES)] = 0;

    return (ret == 0);
}

/* Post RX handler, decrypts received messages, displays them in the text box
 * and sends a notification. */
static void post_rx(ESubGhzChatState* state, size_t rx_size) {
    furi_assert(state);

    if(rx_size == 0) {
        return;
    }

    furi_check(rx_size <= RX_TX_BUFFER_SIZE);

    /* decrypt if necessary */
    if(!state->encrypted) {
        memcpy(state->rx_str_buffer, state->rx_buffer, rx_size);
        state->rx_str_buffer[rx_size] = 0;

        /* remove trailing newline if it is there, for compat with CLI
		 * Sub-GHz chat */
        if(state->rx_str_buffer[rx_size - 1] == '\n') {
            state->rx_str_buffer[rx_size - 1] = 0;
        }
    } else {
        /* if decryption fails output an error message */
        if(!post_rx_decrypt(state, rx_size)) {
            strcpy(state->rx_str_buffer, "ERR: Decryption failed!");
        }
    }

    /* append message to text box */
    furi_string_cat_printf(state->chat_box_store, "\n%s", state->rx_str_buffer);

    /* send notification (make the flipper vibrate) */
    notification_message(state->notification, &sequence_single_vibro);

    /* reset text box contents and focus */
    text_box_set_text(state->chat_box, furi_string_get_cstr(state->chat_box_store));
    text_box_set_focus(state->chat_box, TextBoxFocusEnd);
}

/* Reads the message from msg_input, encrypts it if necessary and then
 * transmits it. */
static void tx_msg_input(ESubGhzChatState* state) {
    /* encrypt message if necessary */
    size_t msg_len = strlen(furi_string_get_cstr(state->msg_input));
    size_t tx_size = msg_len;
    if(state->encrypted) {
        tx_size += IV_BYTES + TAG_BYTES;
        furi_check(tx_size <= sizeof(state->tx_buffer));

        furi_hal_random_fill_buf(state->tx_buffer, IV_BYTES);
        gcm_crypt_and_tag(
            &(state->gcm_ctx),
            ENCRYPT,
            state->tx_buffer,
            IV_BYTES,
            NULL,
            0,
            (unsigned char*)furi_string_get_cstr(state->msg_input),
            state->tx_buffer + IV_BYTES,
            msg_len,
            state->tx_buffer + IV_BYTES + msg_len,
            TAG_BYTES);
    } else {
        tx_size += 2;
        furi_check(tx_size <= sizeof(state->tx_buffer));
        memcpy(state->tx_buffer, furi_string_get_cstr(state->msg_input), msg_len);

        /* append \r\n for compat with Sub-GHz CLI chat */
        state->tx_buffer[msg_len] = '\r';
        state->tx_buffer[msg_len + 1] = '\n';
    }

    /* transmit */
    subghz_tx_rx_worker_write(state->subghz_worker, state->tx_buffer, tx_size);
}

/* Sends FreqEntered event to scene manager and displays the frequency in the
 * text box. */
static void freq_input_cb(void* context) {
    furi_assert(context);
    ESubGhzChatState* state = context;

    furi_string_cat_printf(state->chat_box_store, "Frequency: %lu", state->frequency);

    scene_manager_handle_custom_event(state->scene_manager, ESubGhzChatEvent_FreqEntered);
}

/* Validates the entered frequency. */
static bool freq_input_validator(const char* text, FuriString* error, void* context) {
    furi_assert(text);
    furi_assert(error);

    furi_assert(context);
    ESubGhzChatState* state = context;

    int ret = sscanf(text, "%lu", &(state->frequency));
    if(ret != 1) {
        furi_string_printf(error, "Please enter\nfrequency\nin Hz!");
        return false;
    }

    if(!subghz_devices_is_frequency_valid(state->subghz_device, state->frequency)) {
        furi_string_printf(error, "Frequency\n%lu\n is invalid!", state->frequency);
        return false;
    }

#ifdef FW_ORIGIN_Official
    if(!furi_hal_region_is_frequency_allowed(state->frequency)) {
#else /* FW_ORIGIN_Official */
    if(!furi_hal_subghz_is_tx_allowed(state->frequency)) {
#endif /* FW_ORIGIN_Official */
        furi_string_printf(error, "TX forbidden\non frequency\n%lu!", state->frequency);
        return false;
    }

    return true;
}

/* Sends PassEntered event to scene manager and displays whether or not
 * encryption has been enabled in the text box. Also clears the text input
 * buffer to remove the password and starts the Sub-GHz worker. After starting
 * the worker a join message is transmitted. */
static void pass_input_cb(void* context) {
    furi_assert(context);
    ESubGhzChatState* state = context;

    furi_string_cat_printf(
        state->chat_box_store, "\nEncrypted: %s", (state->encrypted ? "yes" : "no"));

    /* clear the text input buffer to remove the password */
    esubghz_chat_explicit_bzero(state->text_input_store, sizeof(state->text_input_store));

    subghz_tx_rx_worker_start(state->subghz_worker, state->subghz_device, state->frequency);

    /* concatenate the name prefix and join message */
    furi_string_set(state->msg_input, state->name_prefix);
    furi_string_cat_str(state->msg_input, " joined chat.");

    /* encrypt and transmit message */
    tx_msg_input(state);

    /* clear message input buffer */
    furi_string_set_char(state->msg_input, 0, 0);

    scene_manager_handle_custom_event(state->scene_manager, ESubGhzChatEvent_PassEntered);
}

/* If a password was entered this derives a key from the password using a
 * single pass of SHA256 and initiates the AES-GCM context for encryption. If
 * the initiation fails, the password is rejected. */
static bool pass_input_validator(const char* text, FuriString* error, void* context) {
    furi_assert(text);
    furi_assert(error);

    furi_assert(context);
    ESubGhzChatState* state = context;

#ifdef FW_ORIGIN_Official
    if(strlen(text) == 0) {
        furi_string_printf(error, "Enter a\npassword!");
        return false;
    }

    if(strcmp(text, " ") == 0) {
#else /* FW_ORIGIN_Official */
    if(strlen(text) == 0) {
#endif /* FW_ORIGIN_Official */
        state->encrypted = false;
        return true;
    }

    unsigned char key[KEY_BITS / 8];

    /* derive a key from the password */
    sha256((unsigned char*)text, strlen(text), key);

    /* initiate the AES-GCM context */
    int ret = gcm_setkey(&(state->gcm_ctx), key, KEY_BITS / 8);

    /* cleanup */
    esubghz_chat_explicit_bzero(key, sizeof(key));

    if(ret != 0) {
        esubghz_chat_explicit_bzero(&(state->gcm_ctx), sizeof(state->gcm_ctx));
        furi_string_printf(error, "Failed to\nset key!");
        return false;
    }

    state->encrypted = true;

    return true;
}

/* If no message was entred this simply emits a MsgEntered event to the scene
 * manager to switch to the text box. If a message was entered it is appended
 * to the name string. The result is encrypted, if encryption is enabled, and
 * then copied into the TX buffer. The contents of the TX buffer are then
 * transmitted. The sent message is appended to the text box and a MsgEntered
 * event is sent to the scene manager to switch to the text box view. */
static void chat_input_cb(void* context) {
    furi_assert(context);
    ESubGhzChatState* state = context;

    /* no message, just switch to the text box view */
#ifdef FW_ORIGIN_Official
    if(strcmp(state->text_input_store, " ") == 0) {
#else /* FW_ORIGIN_Official */
    if(strlen(state->text_input_store) == 0) {
#endif /* FW_ORIGIN_Official */
        scene_manager_handle_custom_event(state->scene_manager, ESubGhzChatEvent_MsgEntered);
        return;
    }

    /* concatenate the name prefix and the actual message */
    furi_string_set(state->msg_input, state->name_prefix);
    furi_string_cat_str(state->msg_input, ": ");
    furi_string_cat_str(state->msg_input, state->text_input_store);

    /* append the message to the chat box */
    furi_string_cat_printf(state->chat_box_store, "\n%s", furi_string_get_cstr(state->msg_input));

    /* encrypt and transmit message */
    tx_msg_input(state);

    /* clear message input buffer */
    furi_string_set_char(state->msg_input, 0, 0);

    /* switch to text box view */
    scene_manager_handle_custom_event(state->scene_manager, ESubGhzChatEvent_MsgEntered);
}

/* Prepares the frequency input scene. */
static void scene_on_enter_freq_input(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_freq_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    snprintf(state->text_input_store, TEXT_INPUT_STORE_SIZE, "%lu", (uint32_t)DEFAULT_FREQ);
    text_input_reset(state->text_input);
    text_input_set_result_callback(
        state->text_input,
        freq_input_cb,
        state,
        state->text_input_store,
        sizeof(state->text_input_store),
        true);
    text_input_set_validator(state->text_input, freq_input_validator, state);
    text_input_set_header_text(state->text_input, "Frequency");

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

/* Handles scene manager events for the frequency input scene. */
static bool scene_on_event_freq_input(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_freq_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        /* switch to password input scene */
        case ESubGhzChatEvent_FreqEntered:
            scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_PassInput);
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

/* Cleans up the frequency input scene. */
static void scene_on_exit_freq_input(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_freq_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    text_input_reset(state->text_input);
}

/* Prepares the password input scene. */
static void scene_on_enter_pass_input(void* context) {
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
    text_input_set_validator(state->text_input, pass_input_validator, state);
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
static bool scene_on_event_pass_input(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_pass_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        /* switch to message input scene */
        case ESubGhzChatEvent_PassEntered:
            scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_ChatInput);
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
static void scene_on_exit_pass_input(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_pass_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    text_input_reset(state->text_input);
}

/* Prepares the message input scene. */
static void scene_on_enter_chat_input(void* context) {
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
    text_input_set_validator(state->text_input, NULL, NULL);
    text_input_set_header_text(
        state->text_input,
#ifdef FW_ORIGIN_Official
        "Message (space for none)");
#else /* FW_ORIGIN_Official */
        "Message");
    text_input_set_minimum_length(state->text_input, 0);
#endif /* FW_ORIGIN_Official */

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_Input);
}

/* Handles scene manager events for the message input scene. */
static bool scene_on_event_chat_input(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_chat_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        /* switch to text box scene */
        case ESubGhzChatEvent_MsgEntered:
            scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_ChatBox);
            consumed = true;
            break;
        }
        break;

    case SceneManagerEventTypeBack:
        /* stop the application and send a leave message if the user
		 * presses back here */

        /* concatenate the name prefix and leave message */
        furi_string_set(state->msg_input, state->name_prefix);
        furi_string_cat_str(state->msg_input, " left chat.");

        /* encrypt and transmit message */
        tx_msg_input(state);

        /* clear message input buffer */
        furi_string_set_char(state->msg_input, 0, 0);

        /* wait for leave message to be delivered */
        furi_delay_ms(CHAT_LEAVE_DELAY);

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
static void scene_on_exit_chat_input(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_chat_input");

    furi_assert(context);
    ESubGhzChatState* state = context;

    text_input_reset(state->text_input);
}

/* Prepares the text box scene. */
static void scene_on_enter_chat_box(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_chat_box");

    furi_assert(context);
    ESubGhzChatState* state = context;

    text_box_reset(state->chat_box);
    text_box_set_text(state->chat_box, furi_string_get_cstr(state->chat_box_store));
    text_box_set_focus(state->chat_box, TextBoxFocusEnd);

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_ChatBox);
}

/* Handles scene manager events for the text box scene. No events are handled
 * here. */
static bool scene_on_event_chat_box(void* context, SceneManagerEvent event) {
    UNUSED(event);

    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_chat_box");

    furi_assert(context);

    return false;
}

/* Cleans up the text box scene. */
static void scene_on_exit_chat_box(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_chat_box");

    furi_assert(context);
    ESubGhzChatState* state = context;

    text_box_reset(state->chat_box);
}

/* Scene entry handlers. */
static void (*const esubghz_chat_scene_on_enter_handlers[])(void*) = {
    scene_on_enter_freq_input,
    scene_on_enter_pass_input,
    scene_on_enter_chat_input,
    scene_on_enter_chat_box};

/* Scene event handlers. */
static bool (*const esubghz_chat_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    scene_on_event_freq_input,
    scene_on_event_pass_input,
    scene_on_event_chat_input,
    scene_on_event_chat_box};

/* Scene exit handlers. */
static void (*const esubghz_chat_scene_on_exit_handlers[])(void*) = {
    scene_on_exit_freq_input,
    scene_on_exit_pass_input,
    scene_on_exit_chat_input,
    scene_on_exit_chat_box};

/* Handlers for the scene manager. */
static const SceneManagerHandlers esubghz_chat_scene_event_handlers = {
    .on_enter_handlers = esubghz_chat_scene_on_enter_handlers,
    .on_event_handlers = esubghz_chat_scene_on_event_handlers,
    .on_exit_handlers = esubghz_chat_scene_on_exit_handlers,
    .scene_num = ESubGhzChatScene_MAX};

/* Whether or not to display the locked message. */
static bool kbd_lock_msg_display(ESubGhzChatState* state) {
    return (state->kbd_lock_msg_ticks != 0);
}

/* Whether or not to hide the locked message again. */
static bool kbd_lock_msg_reset_timeout(ESubGhzChatState* state) {
    if(state->kbd_lock_msg_ticks == 0) {
        return false;
    }

    if(furi_get_tick() - state->kbd_lock_msg_ticks > KBD_UNLOCK_TIMEOUT) {
        return true;
    }

    return false;
}

/* Resets the timeout for the locked message and turns off the backlight if
 * specified. */
static void kbd_lock_msg_reset(ESubGhzChatState* state, bool backlight_off) {
    state->kbd_lock_msg_ticks = 0;
    state->kbd_lock_count = 0;

    if(backlight_off) {
        notification_message(state->notification, &sequence_display_backlight_off);
    }
}

/* Locks the keyboard. */
static void kbd_lock(ESubGhzChatState* state) {
    state->kbd_locked = true;
    kbd_lock_msg_reset(state, true);
}

/* Unlocks the keyboard. */
static void kbd_unlock(ESubGhzChatState* state) {
    state->kbd_locked = false;
    kbd_lock_msg_reset(state, false);
}

/* Custom event callback for view dispatcher. Just calls scene manager. */
static bool esubghz_chat_custom_event_callback(void* context, uint32_t event) {
    FURI_LOG_T(APPLICATION_NAME, "esubghz_chat_custom_event_callback");
    furi_assert(context);
    ESubGhzChatState* state = context;
    return scene_manager_handle_custom_event(state->scene_manager, event);
}

/* Navigation event callback for view dispatcher. Just calls scene manager. */
static bool esubghz_chat_navigation_event_callback(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "esubghz_chat_navigation_event_callback");
    furi_assert(context);
    ESubGhzChatState* state = context;
    return scene_manager_handle_back_event(state->scene_manager);
}

/* Tick event callback for view dispatcher. Called every TICK_INTERVAL. Resets
 * the locked message if necessary. Retrieves a received message from the
 * Sub-GHz worker and calls post_rx(). Then calls the scene manager. */
static void esubghz_chat_tick_event_callback(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "esubghz_chat_tick_event_callback");

    furi_assert(context);
    ESubGhzChatState* state = context;

    /* reset locked message if necessary */
    if(kbd_lock_msg_reset_timeout(state)) {
        kbd_lock_msg_reset(state, true);
    }

    /* if the maximum message size was reached or the
	 * MESSAGE_COMPLETION_TIMEOUT has expired, retrieve a message and call
	 * post_rx() */
    size_t avail = 0;
    while((avail = subghz_tx_rx_worker_available(state->subghz_worker)) > 0) {
        volatile uint32_t since_last_rx = furi_get_tick() - state->last_time_rx_data;
        if(avail < RX_TX_BUFFER_SIZE && since_last_rx < MESSAGE_COMPLETION_TIMEOUT) {
            break;
        }

        size_t rx_size =
            subghz_tx_rx_worker_read(state->subghz_worker, state->rx_buffer, RX_TX_BUFFER_SIZE);
        post_rx(state, rx_size);
    }

    /* call scene manager */
    scene_manager_handle_tick_event(state->scene_manager);
}

/* Hooks into the view port's draw callback to overlay the keyboard locked
 * message. */
static void esubghz_hooked_draw_callback(Canvas* canvas, void* context) {
    FURI_LOG_T(APPLICATION_NAME, "esubghz_hooked_draw_callback");

    furi_assert(canvas);

    furi_assert(context);
    ESubGhzChatState* state = context;

    /* call original callback */
    state->orig_draw_cb(canvas, state->view_dispatcher);

    /* display if the keyboard is locked */
    if(state->kbd_locked) {
        canvas_set_font(canvas, FontPrimary);
        elements_multiline_text_framed(canvas, 42, 30, "Locked");
    }

    /* display the unlock message if necessary */
    if(kbd_lock_msg_display(state)) {
        canvas_set_font(canvas, FontSecondary);
        elements_bold_rounded_frame(canvas, 14, 8, 99, 48);
        elements_multiline_text(canvas, 65, 26, "To unlock\npress:");
        canvas_draw_icon(canvas, 65, 42, &I_Pin_back_arrow_10x8);
        canvas_draw_icon(canvas, 80, 42, &I_Pin_back_arrow_10x8);
        canvas_draw_icon(canvas, 95, 42, &I_Pin_back_arrow_10x8);
        canvas_draw_icon(canvas, 16, 13, &I_WarningDolphin_45x42);
    }
}

/* Hooks into the view port's input callback to handle the user locking the
 * keyboard. */
static void esubghz_hooked_input_callback(InputEvent* event, void* context) {
    FURI_LOG_T(APPLICATION_NAME, "esubghz_hooked_input_callback");

    furi_assert(event);

    furi_assert(context);
    ESubGhzChatState* state = context;

    /* if the keyboard is locked no key presses are forwarded */
    if(state->kbd_locked) {
        /* key has been pressed, display the unlock message and
		 * initiate the timer */
        if(state->kbd_lock_count == 0) {
            state->kbd_lock_msg_ticks = furi_get_tick();
        }

        /* back button has been pressed, increase the lock counter */
        if(event->key == InputKeyBack && event->type == InputTypeShort) {
            state->kbd_lock_count++;
        }

        /* unlock the keyboard */
        if(state->kbd_lock_count >= KBD_UNLOCK_CNT) {
            kbd_unlock(state);
        }

        /* do not handle the event */
        return;
    }

    if(event->key == InputKeyOk) {
        /* if we are in the chat view and no input is ongoing, allow
		 * locking */
        if(state->view_dispatcher->current_view == text_box_get_view(state->chat_box) &&
           !(state->kbd_ok_input_ongoing)) {
            /* lock keyboard upon long press of Ok button */
            if(event->type == InputTypeLong) {
                kbd_lock(state);
            }

            /* do not handle any Ok key events to prevent blocking
			 * of other keys */
            return;
        }

        /* handle ongoing inputs when chaning to chat view */
        if(event->type == InputTypePress) {
            state->kbd_ok_input_ongoing = true;
        } else if(event->type == InputTypeRelease) {
            state->kbd_ok_input_ongoing = false;
        }
    }

    /* call original callback */
    state->orig_input_cb(event, state->view_dispatcher);
}

static bool helper_strings_alloc(ESubGhzChatState* state) {
    furi_assert(state);

    state->name_prefix = furi_string_alloc();
    if(state->name_prefix == NULL) {
        return false;
    }

    state->msg_input = furi_string_alloc();
    if(state->msg_input == NULL) {
        furi_string_free(state->name_prefix);
        return false;
    }

    return true;
}

static void helper_strings_free(ESubGhzChatState* state) {
    furi_assert(state);

    furi_string_free(state->name_prefix);
    furi_string_free(state->msg_input);
}

static bool chat_box_alloc(ESubGhzChatState* state) {
    furi_assert(state);

    state->chat_box = text_box_alloc();
    if(state->chat_box == NULL) {
        return false;
    }

    state->chat_box_store = furi_string_alloc();
    if(state->chat_box_store == NULL) {
        text_box_free(state->chat_box);
        return false;
    }

    furi_string_reserve(state->chat_box_store, CHAT_BOX_STORE_SIZE);
    furi_string_set_char(state->chat_box_store, 0, 0);
    text_box_set_text(state->chat_box, furi_string_get_cstr(state->chat_box_store));
    text_box_set_focus(state->chat_box, TextBoxFocusEnd);

    return true;
}

static void chat_box_free(ESubGhzChatState* state) {
    furi_assert(state);

    text_box_free(state->chat_box);
    furi_string_free(state->chat_box_store);
}

int32_t esubghz_chat(void) {
    /* init the GCM and AES tables */
    gcm_initialize();

    int32_t err = -1;

    FURI_LOG_I(APPLICATION_NAME, "Starting...");

    /* allocate necessary structs and buffers */

    ESubGhzChatState* state = malloc(sizeof(ESubGhzChatState));
    if(state == NULL) {
        goto err_alloc;
    }
    memset(state, 0, sizeof(*state));

    state->scene_manager = scene_manager_alloc(&esubghz_chat_scene_event_handlers, state);
    if(state->scene_manager == NULL) {
        goto err_alloc_sm;
    }

    state->view_dispatcher = view_dispatcher_alloc();
    if(state->view_dispatcher == NULL) {
        goto err_alloc_vd;
    }

    if(!helper_strings_alloc(state)) {
        goto err_alloc_hs;
    }

    state->text_input = text_input_alloc();
    if(state->text_input == NULL) {
        goto err_alloc_ti;
    }

    if(!chat_box_alloc(state)) {
        goto err_alloc_cb;
    }

    state->subghz_worker = subghz_tx_rx_worker_alloc();
    if(state->subghz_worker == NULL) {
        goto err_alloc_worker;
    }

    /* set the have_read callback of the Sub-GHz worker */
    subghz_tx_rx_worker_set_callback_have_read(state->subghz_worker, have_read_cb, state);

    /* enter suppress charge mode */
    furi_hal_power_suppress_charge_enter();

    /* init internal device */
    subghz_devices_init();
    state->subghz_device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);

    /* set chat name prefix */
    furi_string_printf(state->name_prefix, "%s", furi_hal_version_get_name_ptr());

    /* get notification record, we use this to make the flipper vibrate */
    /* no error handling here, don't know how */
    state->notification = furi_record_open(RECORD_NOTIFICATION);

    /* hook into the view port's draw and input callbacks */
    state->orig_draw_cb = state->view_dispatcher->view_port->draw_callback;
    state->orig_input_cb = state->view_dispatcher->view_port->input_callback;
    view_port_draw_callback_set(
        state->view_dispatcher->view_port, esubghz_hooked_draw_callback, state);
    view_port_input_callback_set(
        state->view_dispatcher->view_port, esubghz_hooked_input_callback, state);

    view_dispatcher_enable_queue(state->view_dispatcher);

    /* set callbacks for view dispatcher */
    view_dispatcher_set_event_callback_context(state->view_dispatcher, state);
    view_dispatcher_set_custom_event_callback(
        state->view_dispatcher, esubghz_chat_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        state->view_dispatcher, esubghz_chat_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        state->view_dispatcher, esubghz_chat_tick_event_callback, TICK_INTERVAL);

    /* add our two views to the view dispatcher */
    view_dispatcher_add_view(
        state->view_dispatcher, ESubGhzChatView_Input, text_input_get_view(state->text_input));
    view_dispatcher_add_view(
        state->view_dispatcher, ESubGhzChatView_ChatBox, text_box_get_view(state->chat_box));

    /* get the GUI record and attach the view dispatcher to the GUI */
    /* no error handling here, don't know how */
    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(state->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    /* switch to the frequency input scene */
    scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_FreqInput);

    /* run the view dispatcher, this call only returns when we close the
	 * application */
    view_dispatcher_run(state->view_dispatcher);

    /* if it is running, stop the Sub-GHz worker */
    if(subghz_tx_rx_worker_is_running(state->subghz_worker)) {
        subghz_tx_rx_worker_stop(state->subghz_worker);
    }

    err = 0;

    /* close GUI record */
    furi_record_close(RECORD_GUI);

    /* remove our two views from the view dispatcher */
    view_dispatcher_remove_view(state->view_dispatcher, ESubGhzChatView_Input);
    view_dispatcher_remove_view(state->view_dispatcher, ESubGhzChatView_ChatBox);

    /* close notification record */
    furi_record_close(RECORD_NOTIFICATION);

    /* clear the key and potential password */
    esubghz_chat_explicit_bzero(state->text_input_store, sizeof(state->text_input_store));
    esubghz_chat_explicit_bzero(&(state->gcm_ctx), sizeof(state->gcm_ctx));

    /* deinit devices */
    subghz_devices_deinit();

    /* exit suppress charge mode */
    furi_hal_power_suppress_charge_exit();

    /* free everything we allocated */

    subghz_tx_rx_worker_free(state->subghz_worker);

err_alloc_worker:
    chat_box_free(state);

err_alloc_cb:
    text_input_free(state->text_input);

err_alloc_ti:
    helper_strings_free(state);

err_alloc_hs:
    view_dispatcher_free(state->view_dispatcher);

err_alloc_vd:
    scene_manager_free(state->scene_manager);

err_alloc_sm:
    free(state);

err_alloc:
    if(err != 0) {
        FURI_LOG_E(APPLICATION_NAME, "Failed to launch (alloc error)!");
    } else {
        FURI_LOG_I(APPLICATION_NAME, "Clean exit.");
    }

    return err;
}
