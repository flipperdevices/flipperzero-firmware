#include "usb_ccb_start.h"
#include <furi.h>
#include <furi_hal_usb_hid.h>
#include <gui/elements.h>

struct UsbCcbStart {
    View* view;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool back_pressed;
    bool connected;
} UsbCcbStartModel;

typedef struct {
    uint16_t value;
    const char* name;
} HID_CONSUMER;

// All of the strings are prefixed with "HID_CONSUMER_", which has been removed to save some space.
HID_CONSUMER hidConsumerArray[] = {
    {0x00, "UNASSIGNED"}, // Generic Consumer Control Device
    {0x01, "CONTROL"},
    {0x02, "NUMERIC_KEY_PAD"},
    {0x03, "PROGRAMMABLE_BUTTONS"},
    {0x04, "MICROPHONE"},
    {0x05, "HEADPHONE"},
    {0x06, "GRAPHIC_EQUALIZER"},
    {0x20, "PLUS_10"}, // Numeric Key Pad
    {0x21, "PLUS_100"},
    {0x22, "AM_PM"},
    {0x30, "POWER"}, // General Controls
    {0x31, "RESET"},
    {0x32, "SLEEP"},
    {0x33, "SLEEP_AFTER"},
    {0x34, "SLEEP_MODE"},
    {0x35, "ILLUMINATION"},
    {0x36, "FUNCTION_BUTTONS"},
    {0x40, "MENU"}, // Menu Controls
    {0x41, "MENU_PICK"},
    {0x42, "MENU_UP"},
    {0x43, "MENU_DOWN"},
    {0x44, "MENU_LEFT"},
    {0x45, "MENU_RIGHT"},
    {0x46, "MENU_ESCAPE"},
    {0x47, "MENU_VALUE_INCREASE"},
    {0x48, "MENU_VALUE_DECREASE"},
    {0x60, "DATA_ON_SCREEN"}, // Display Controls
    {0x61, "CLOSED_CAPTION"},
    {0x62, "CLOSED_CAPTION_SELECT"},
    {0x63, "VCR_TV"},
    {0x64, "BROADCAST_MODE"},
    {0x65, "SNAPSHOT"},
    {0x66, "STILL"},
    {0x80, "SELECTION"}, // Selection Controls
    {0x81, "ASSIGN_SELECTION"},
    {0x82, "MODE_STEP"},
    {0x83, "RECALL_LAST"},
    {0x84, "ENTER_CHANNEL"},
    {0x85, "ORDER_MOVIE"},
    {0x86, "CHANNEL"},
    {0x87, "MEDIA_SELECTION"},
    {0x88, "MEDIA_SELECT_COMPUTER"},
    {0x89, "MEDIA_SELECT_TV"},
    {0x8A, "MEDIA_SELECT_WWW"},
    {0x8B, "MEDIA_SELECT_DVD"},
    {0x8C, "MEDIA_SELECT_TELEPHONE"},
    {0x8D, "MEDIA_SELECT_PROGRAM_GUIDE"},
    {0x8E, "MEDIA_SELECT_VIDEO_PHONE"},
    {0x8F, "MEDIA_SELECT_GAMES"},
    {0x90, "MEDIA_SELECT_MESSAGES"},
    {0x91, "MEDIA_SELECT_CD"},
    {0x92, "MEDIA_SELECT_VCR"},
    {0x93, "MEDIA_SELECT_TUNER"},
    {0x94, "QUIT"},
    {0x95, "HELP"},
    {0x96, "MEDIA_SELECT_TAPE"},
    {0x97, "MEDIA_SELECT_CABLE"},
    {0x98, "MEDIA_SELECT_SATELLITE"},
    {0x99, "MEDIA_SELECT_SECURITY"},
    {0x9A, "MEDIA_SELECT_HOME"},
    {0x9B, "MEDIA_SELECT_CALL"},
    {0x9C, "CHANNEL_INCREMENT"},
    {0x9D, "CHANNEL_DECREMENT"},
    {0x9E, "MEDIA_SELECT_SAP"},
    {0xA0, "VCR_PLUS"},
    {0xA1, "ONCE"},
    {0xA2, "DAILY"},
    {0xA3, "WEEKLY"},
    {0xA4, "MONTHLY"},
    {0xB0, "PLAY"}, // Transport Controls
    {0xB1, "PAUSE"},
    {0xB2, "RECORD"},
    {0xB3, "FAST_FORWARD"},
    {0xB4, "REWIND"},
    {0xB5, "SCAN_NEXT_TRACK"},
    {0xB6, "SCAN_PREVIOUS_TRACK"},
    {0xB7, "STOP"},
    {0xB8, "EJECT"},
    {0xB9, "RANDOM_PLAY"},
    {0xBA, "SELECT_DISC"},
    {0xBB, "ENTER_DISC"},
    {0xBC, "REPEAT"},
    {0xBD, "TRACKING"},
    {0xBE, "TRACK_NORMAL"},
    {0xBF, "SLOW_TRACKING"},
    {0xC0, "FRAME_FORWARD"},
    {0xC1, "FRAME_BACK"},
    {0xC2, "MARK"}, // Search Controls
    {0xC3, "CLEAR_MARK"},
    {0xC4, "REPEAT_FROM_MARK"},
    {0xC5, "RETURN_TO_MARK"},
    {0xC6, "SEARCH_MARK_FORWARD"},
    {0xC7, "SEARCH_MARK_BACKWARDS"},
    {0xC8, "COUNTER_RESET"},
    {0xC9, "SHOW_COUNTER"},
    {0xCA, "TRACKING_INCREMENT"},
    {0xCB, "TRACKING_DECREMENT"},
    {0xCC, "STOP_EJECT"},
    {0xCD, "PLAY_PAUSE"},
    {0xCE, "PLAY_SKIP"},
    {0xE0, "VOLUME"}, // Audio Controls
    {0xE1, "BALANCE"},
    {0xE2, "MUTE"},
    {0xE3, "BASS"},
    {0xE4, "TREBLE"},
    {0xE5, "BASS_BOOST"},
    {0xE6, "SURROUND_MODE"},
    {0xE7, "LOUDNESS"},
    {0xE8, "MPX"},
    {0xE9, "VOLUME_INCREMENT"},
    {0xEA, "VOLUME_DECREMENT"},
    {0xF0, "SPEED_SELECT"}, // Speed Controls
    {0xF1, "PLAYBACK_SPEED"},
    {0xF2, "STANDARD_PLAY"},
    {0xF3, "LONG_PLAY"},
    {0xF4, "EXTENDED_PLAY"},
    {0xF5, "SLOW"},
    {0x100, "FAN_ENABLE"}, // Home and Security Controls
    {0x101, "FAN_SPEED"},
    {0x102, "LIGHT_ENABLE"},
    {0x103, "LIGHT_ILLUMINATION_LEVEL"},
    {0x104, "CLIMATE_CONTROL_ENABLE"},
    {0x105, "ROOM_TEMPERATURE"},
    {0x106, "SECURITY_ENABLE"},
    {0x107, "FIRE_ALARM"},
    {0x108, "POLICE_ALARM"},
    {0x109, "PROXIMITY"},
    {0x10A, "MOTION"},
    {0x10B, "DURESS_ALARM"},
    {0x10C, "HOLDUP_ALARM"},
    {0x10D, "MEDICAL_ALARM"},
    {0x150, "BALANCE_RIGHT"}, // Speaker Channels
    {0x151, "BALANCE_LEFT"},
    {0x152, "BASS_INCREMENT"},
    {0x153, "BASS_DECREMENT"},
    {0x154, "TREBLE_INCREMENT"},
    {0x155, "TREBLE_DECREMENT"},
    {0x160, "SPEAKER_SYSTEM"},
    {0x161, "CHANNEL_LEFT"},
    {0x162, "CHANNEL_RIGHT"},
    {0x163, "CHANNEL_CENTER"},
    {0x164, "CHANNEL_FRONT"},
    {0x165, "CHANNEL_CENTER_FRONT"},
    {0x166, "CHANNEL_SIDE"},
    {0x167, "CHANNEL_SURROUND"},
    {0x168, "CHANNEL_LOW_FREQUENCY_ENHANCEMENT"},
    {0x169, "CHANNEL_TOP"},
    {0x16A, "CHANNEL_UNKNOWN"},
    {0x170, "SUB_CHANNEL"}, // PC Theatre
    {0x171, "SUB_CHANNEL_INCREMENT"},
    {0x172, "SUB_CHANNEL_DECREMENT"},
    {0x173, "ALTERNATE_AUDIO_INCREMENT"},
    {0x174, "ALTERNATE_AUDIO_DECREMENT"},
    {0x180, "APPLICATION_LAUNCH_BUTTONS"}, // Application Launch Buttons
    {0x181, "AL_LAUNCH_BUTTON_CONFIGURATION_TOOL"},
    {0x182, "AL_PROGRAMMABLE_BUTTON_CONFIGURATION"},
    {0x183, "AL_CONSUMER_CONTROL_CONFIGURATION"},
    {0x184, "AL_WORD_PROCESSOR"},
    {0x185, "AL_TEXT_EDITOR"},
    {0x186, "AL_SPREADSHEET"},
    {0x187, "AL_GRAPHICS_EDITOR"},
    {0x188, "AL_PRESENTATION_APP"},
    {0x189, "AL_DATABASE_APP"},
    {0x18A, "AL_EMAIL_READER"},
    {0x18B, "AL_NEWSREADER"},
    {0x18C, "AL_VOICEMAIL"},
    {0x18D, "AL_CONTACTS_ADDRESS_BOOK"},
    {0x18E, "AL_CALENDAR_SCHEDULE"},
    {0x18F, "AL_TASK_PROJECT_MANAGER"},
    {0x190, "AL_LOG_JOURNAL_TIMECARD"},
    {0x191, "AL_CHECKBOOK_FINANCE"},
    {0x192, "AL_CALCULATOR"},
    {0x193, "AL_A_V_CAPTURE_PLAYBACK"},
    {0x194, "AL_LOCAL_MACHINE_BROWSER"},
    {0x195, "AL_LAN_WAN_BROWSER"},
    {0x196, "AL_INTERNET_BROWSER"},
    {0x197, "AL_REMOTE_NETWORKING_ISP_CONNECT"},
    {0x198, "AL_NETWORK_CONFERENCE"},
    {0x199, "AL_NETWORK_CHAT"},
    {0x19A, "AL_TELEPHONY_DIALER"},
    {0x19B, "AL_LOGON"},
    {0x19C, "AL_LOGOFF"},
    {0x19D, "AL_LOGON_LOGOFF"},
    {0x19E, "AL_TERMINAL_LOCK_SCREENSAVER"},
    {0x19F, "AL_CONTROL_PANEL"},
    {0x1A0, "AL_COMMAND_LINE_PROCESSOR_RUN"},
    {0x1A1, "AL_PROCESS_TASK_MANAGER"},
    {0x1A2, "AL_SELECT_TASK_APPLICATION"},
    {0x1A3, "AL_NEXT_TASK_APPLICATION"},
    {0x1A4, "AL_PREVIOUS_TASK_APPLICATION"},
    {0x1A5, "AL_PREEMPTIVE_HALT_TASK_APPLICATION"},
    {0x1A6, "AL_INTEGRATED_HELP_CENTER"},
    {0x1A7, "AL_DOCUMENTS"},
    {0x1A8, "AL_THESAURUS"},
    {0x1A9, "AL_DICTIONARY"},
    {0x1AA, "AL_DESKTOP"},
    {0x1AB, "AL_SPELL_CHECK"},
    {0x1AC, "AL_GRAMMAR_CHECK"},
    {0x1AD, "AL_WIRELESS_STATUS"},
    {0x1AE, "AL_KEYBOARD_LAYOUT"},
    {0x1AF, "AL_VIRUS_PROTECTION"},
    {0x1B0, "AL_ENCRYPTION"},
    {0x1B1, "AL_SCREEN_SAVER"},
    {0x1B2, "AL_ALARMS"},
    {0x1B3, "AL_CLOCK"},
    {0x1B4, "AL_FILE_BROWSER"},
    {0x1B5, "AL_POWER_STATUS"},
    {0x1B6, "AL_IMAGE_BROWSER"},
    {0x1B7, "AL_AUDIO_BROWSER"},
    {0x1B8, "AL_MOVIE_BROWSER"},
    {0x1B9, "AL_DIGITAL_RIGHTS_MANAGER"},
    {0x1BA, "AL_DIGITAL_WALLET"},
    {0x1BC, "AL_INSTANT_MESSAGING"},
    {0x1BD, "AL_OEM_FEATURES_TIPS_TUTORIAL_BROWSER"},
    {0x1BE, "AL_OEM_HELP"},
    {0x1BF, "AL_ONLINE_COMMUNITY"},
    {0x1C0, "AL_ENTERTAINMENT_CONTENT_BROWSER"},
    {0x1C1, "AL_ONLINE_SHOPPING_BROWSER"},
    {0x1C2, "AL_SMARTCARD_INFORMATION_HELP"},
    {0x1C3, "AL_MARKET_MONITOR_FINANCE_BROWSER"},
    {0x1C4, "AL_CUSTOMIZED_CORPORATE_NEWS_BROWSER"},
    {0x1C5, "AL_ONLINE_ACTIVITY_BROWSER"},
    {0x1C6, "AL_RESEARCH_SEARCH_BROWSER"},
    {0x1C7, "AL_AUDIO_PLAYER"},
    {0x200, "GENERIC_GUI_APPLICATION_CONTROLS"}, // Generic GUI Application Controls
    {0x201, "AC_NEW"},
    {0x202, "AC_OPEN"},
    {0x203, "AC_CLOSE"},
    {0x204, "AC_EXIT"},
    {0x205, "AC_MAXIMIZE"},
    {0x206, "AC_MINIMIZE"},
    {0x207, "AC_SAVE"},
    {0x208, "AC_PRINT"},
    {0x209, "AC_PROPERTIES"},
    {0x21A, "AC_UNDO"},
    {0x21B, "AC_COPY"},
    {0x21C, "AC_CUT"},
    {0x21D, "AC_PASTE"},
    {0x21E, "AC_SELECT_ALL"},
    {0x21F, "AC_FIND"},
    {0x220, "AC_FIND_AND_REPLACE"},
    {0x221, "AC_SEARCH"},
    {0x222, "AC_GO_TO"},
    {0x223, "AC_HOME"},
    {0x224, "AC_BACK"},
    {0x225, "AC_FORWARD"},
    {0x226, "AC_STOP"},
    {0x227, "AC_REFRESH"},
    {0x228, "AC_PREVIOUS_LINK"},
    {0x229, "AC_NEXT_LINK"},
    {0x22A, "AC_BOOKMARKS"},
    {0x22B, "AC_HISTORY"},
    {0x22C, "AC_SUBSCRIPTIONS"},
    {0x22D, "AC_ZOOM_IN"},
    {0x22E, "AC_ZOOM_OUT"},
    {0x22F, "AC_ZOOM"},
    {0x230, "AC_FULL_SCREEN_VIEW"},
    {0x231, "AC_NORMAL_VIEW"},
    {0x232, "AC_VIEW_TOGGLE"},
    {0x233, "AC_SCROLL_UP"},
    {0x234, "AC_SCROLL_DOWN"},
    {0x235, "AC_SCROLL"},
    {0x236, "AC_PAN_LEFT"},
    {0x237, "AC_PAN_RIGHT"},
    {0x238, "AC_PAN"},
    {0x239, "AC_NEW_WINDOW"},
    {0x23A, "AC_TILE_HORIZONTALLY"},
    {0x23B, "AC_TILE_VERTICALLY"},
    {0x23C, "AC_FORMAT"},
    {0x23D, "AC_EDIT"},
    {0x23E, "AC_BOLD"},
    {0x23F, "AC_ITALICS"},
    {0x240, "AC_UNDERLINE"},
    {0x241, "AC_STRIKETHROUGH"},
    {0x242, "AC_SUBSCRIPT"},
    {0x243, "AC_SUPERSCRIPT"},
    {0x244, "AC_ALL_CAPS"},
    {0x245, "AC_ROTATE"},
    {0x246, "AC_RESIZE"},
    {0x247, "AC_FLIP_HORIZONTAL"},
    {0x248, "AC_FLIP_VERTICAL"},
    {0x249, "AC_MIRROR_HORIZONTAL"},
    {0x24A, "AC_MIRROR_VERTICAL"},
    {0x24B, "AC_FONT_SELECT"},
    {0x24C, "AC_FONT_COLOR"},
    {0x24D, "AC_FONT_SIZE"},
    {0x24E, "AC_JUSTIFY_LEFT"},
    {0x24F, "AC_JUSTIFY_CENTER_H"},
    {0x250, "AC_JUSTIFY_RIGHT"},
    {0x251, "AC_JUSTIFY_BLOCK_H"},
    {0x252, "AC_JUSTIFY_TOP"},
    {0x253, "AC_JUSTIFY_CENTER_V"},
    {0x254, "AC_JUSTIFY_BOTTOM"},
    {0x255, "AC_JUSTIFY_BLOCK_V"},
    {0x256, "AC_INDENT_DECREASE"},
    {0x257, "AC_INDENT_INCREASE"},
    {0x258, "AC_NUMBERED_LIST"},
    {0x259, "AC_RESTART_NUMBERING"},
    {0x25A, "AC_BULLETED_LIST"},
    {0x25B, "AC_PROMOTE"},
    {0x25C, "AC_DEMOTE"},
    {0x25D, "AC_YES"},
    {0x25E, "AC_NO"},
    {0x25F, "AC_CANCEL"},
    {0x260, "AC_CATALOG"},
    {0x261, "AC_BUY_CHECKOUT"},
    {0x262, "AC_ADD_TO_CART"},
    {0x263, "AC_EXPAND"},
    {0x264, "AC_EXPAND_ALL"},
    {0x265, "AC_COLLAPSE"},
    {0x266, "AC_COLLAPSE_ALL"},
    {0x267, "AC_PRINT_PREVIEW"},
    {0x268, "AC_PASTE_SPECIAL"},
    {0x269, "AC_INSERT_MODE"},
    {0x26A, "AC_DELETE"},
    {0x26B, "AC_LOCK"},
    {0x26C, "AC_UNLOCK"},
    {0x26D, "AC_PROTECT"},
    {0x26E, "AC_UNPROTECT"},
    {0x26F, "AC_ATTACH_COMMENT"},
    {0x270, "AC_DELETE_COMMENT"},
    {0x271, "AC_VIEW_COMMENT"},
    {0x272, "AC_SELECT_WORD"},
    {0x273, "AC_SELECT_SENTENCE"},
    {0x274, "AC_SELECT_PARAGRAPH"},
    {0x275, "AC_SELECT_COLUMN"},
    {0x276, "AC_SELECT_ROW"},
    {0x277, "AC_SELECT_TABLE"},
    {0x278, "AC_SELECT_OBJECT"},
    {0x279, "AC_REDO_REPEAT"},
    {0x27A, "AC_SORT"},
    {0x27B, "AC_SORT_ASCENDING"},
    {0x27C, "AC_SORT_DESCENDING"},
    {0x27D, "AC_FILTER"},
    {0x27E, "AC_SET_CLOCK"},
    {0x27F, "AC_VIEW_CLOCK"},
    {0x280, "AC_SELECT_TIME_ZONE"},
    {0x281, "AC_EDIT_TIME_ZONES"},
    {0x282, "AC_SET_ALARM"},
    {0x283, "AC_CLEAR_ALARM"},
    {0x284, "AC_SNOOZE_ALARM"},
    {0x285, "AC_RESET_ALARM"},
    {0x286, "AC_SYNCHRONIZE"},
    {0x287, "AC_SEND_RECEIVE"},
    {0x288, "AC_SEND_TO"},
    {0x289, "AC_REPLY"},
    {0x28A, "AC_REPLY_ALL"},
    {0x28B, "AC_FORWARD_MSG"},
    {0x28C, "AC_SEND"},
    {0x28D, "AC_ATTACH_FILE"},
    {0x28E, "AC_UPLOAD"},
    {0x28F, "AC_DOWNLOAD_SAVE_TARGET_AS"},
    {0x290, "AC_SET_BORDERS"},
    {0x291, "AC_INSERT_ROW"},
    {0x292, "AC_INSERT_COLUMN"},
    {0x293, "AC_INSERT_FILE"},
    {0x294, "AC_INSERT_PICTURE"},
    {0x295, "AC_INSERT_OBJECT"},
    {0x296, "AC_INSERT_SYMBOL"},
    {0x297, "AC_SAVE_AND_CLOSE"},
    {0x298, "AC_RENAME"},
    {0x299, "AC_MERGE"},
    {0x29A, "AC_SPLIT"},
    {0x29B, "AC_DISRIBUTE_HORIZONTALLY"},
    {0x29C, "AC_DISTRIBUTE_VERTICALLY"}};

const int hidConsumerArraySize = sizeof(hidConsumerArray) / sizeof(hidConsumerArray[0]);

void strrev(char* arr, int start, int end) {
    char temp;

    if(start >= end) return;

    temp = *(arr + start);
    *(arr + start) = *(arr + end);
    *(arr + end) = temp;

    start++;
    end--;
    strrev(arr, start, end);
}

char* itoa(int number, char* arr, int base) {
    int i = 0, r, negative = 0;

    if(number == 0) {
        arr[i] = '0';
        arr[i + 1] = '\0';
        return arr;
    }

    if(number < 0 && base == 10) {
        number *= -1;
        negative = 1;
    }

    while(number != 0) {
        r = number % base;
        arr[i] = (r > 9) ? (r - 10) + 'a' : r + '0';
        i++;
        number /= base;
    }

    if(negative) {
        arr[i] = '-';
        i++;
    }

    strrev(arr, 0, i - 1);

    arr[i] = '\0';

    return arr;
}

// Function to convert a single hex digit to its character representation
char hexDigitToChar(uint8_t digit) {
    if(digit < 10) {
        return '0' + digit;
    } else {
        return 'A' + (digit - 10);
    }
}

// Function to convert a uint16_t value to its hex string representation
void uint16ToHexString(uint16_t value, char* hexString) {
    hexString[0] = '0';
    hexString[1] = 'x';
    int startIndex = 2;
    for(int i = 3; i >= 0; --i) {
        uint8_t digit = (value >> (i * 4)) & 0xF;
        if(digit != 0 || startIndex != 2) {
            hexString[startIndex++] = hexDigitToChar(digit);
        }
    }
    if(startIndex == 2) { // If there are no non-zero digits, display at least "0x0"
        hexString[startIndex++] = '0';
    }
    hexString[startIndex] = '\0';
}

bool is_running = false;
uint32_t autofire_delay = 1000;
char autofire_delay_str[12];
char hexString[7];

// Start at the Application Launch Buttons by default, since that's the most interesting
int i = 153;
int currentSubsetIndex = 13;

// Array to store indexes of different CCB subsets
// The idea would be to try using these different subsets against different types of devices
int hidConsumerSubsets[] = {
    0, // Generic Consumer Control Device
    7, // Numeric Key Pad
    10, // General Controls
    17, // Menu Controls
    26, // Display Controls
    33, // Selection Controls
    69, // Transport Controls (nice)
    87, // Search Controls
    100, // Audio Controls
    111, // Speed Controls
    117, // Home and Security Controls
    131, // Speaker Channels
    148, // PC Theatre
    153, // Application Launch Buttons
    224 // Generic GUI Application Controls
};

const int hidConsumerSubsetsSize = sizeof(hidConsumerSubsets) / sizeof(hidConsumerSubsets[0]);

const char* getConsumerSubsetName(int i) {
    if(i >= 0 && i < 7) {
        return "Generic Consumer Control";
    } else if(i >= 7 && i < 10) {
        return "Numeric Key Pad";
    } else if(i >= 10 && i < 17) {
        return "General Controls";
    } else if(i >= 17 && i < 26) {
        return "Menu Controls";
    } else if(i >= 26 && i < 33) {
        return "Display Controls";
    } else if(i >= 33 && i < 69) {
        return "Selection Controls";
    } else if(i >= 69 && i < 87) {
        return "Transport Controls";
    } else if(i >= 87 && i < 100) {
        return "Search Controls";
    } else if(i >= 100 && i < 111) {
        return "Audio Controls";
    } else if(i >= 111 && i < 117) {
        return "Speed Controls";
    } else if(i >= 117 && i < 131) {
        return "Home and Security Controls";
    } else if(i >= 131 && i < 148) {
        return "Speaker Channels";
    } else if(i >= 148 && i < 153) {
        return "PC Theatre";
    } else if(i >= 153 && i < 224) {
        return "Application Launch Buttons";
    } else if(i >= 224) {
        return "GUI Application Controls";
    }
    // Won't ever happen though
    return "Invalid Index";
}

// This is the main program loop
static void usb_ccb_start_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    itoa(autofire_delay, autofire_delay_str, 10);
    uint16ToHexString(hidConsumerArray[i].value, hexString);

    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, is_running ? "Running" : "Not running");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 24, "Delay [ms]: ");
    canvas_draw_str(canvas, 50, 24, autofire_delay_str);

    canvas_draw_str(canvas, 0, 38, "Current key subset:");
    canvas_draw_str(canvas, 0, 46, getConsumerSubsetName(i));

    canvas_draw_str(
        canvas,
        0,
        56,
        is_running ? "Sent:                HID_CONSUMER_" : "Next:                HID_CONSUMER_");
    canvas_draw_str(canvas, 24, 56, hexString);
    canvas_draw_str(canvas, 0, 64, hidConsumerArray[i].name);

    if(is_running) {
        uint16_t consumer_key = hidConsumerArray[i].value;
        // Sending the consumer control button
        furi_delay_us(autofire_delay * 500);
        furi_hal_hid_consumer_key_press(consumer_key);
        furi_delay_us(2000); // Hold the key pressed for a short amount of time

        // Stop sending the consumer control button
        furi_hal_hid_consumer_key_release(consumer_key);
        furi_delay_us(autofire_delay * 500);

        // Cycle onto next consumer control button
        i += 1;

        // Stop once we've cycled all consumer control buttons
        if(i == hidConsumerArraySize) {
            i = 0;
            is_running = false;
        }
    }
}

// This function is the controller
static void usb_ccb_start_process(UsbCcbStart* usb_ccb_start, InputEvent* event) {
    with_view_model(
        usb_ccb_start->view,
        UsbCcbStartModel * model,
        {
            if(event->type == InputTypeLong) {
                if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    currentSubsetIndex = (currentSubsetIndex + 1) % hidConsumerSubsetsSize;
                    i = hidConsumerSubsets[currentSubsetIndex];
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = true;
                    currentSubsetIndex =
                        (currentSubsetIndex - 1 + hidConsumerSubsetsSize) % hidConsumerSubsetsSize;
                    i = hidConsumerSubsets[currentSubsetIndex];
                }
            } else if(event->type == InputTypePress) {
                if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    i = (i + 1) % hidConsumerArraySize;
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = true;
                    i = (i - 1 + hidConsumerArraySize) % hidConsumerArraySize;
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = true;
                    if(autofire_delay > 0) {
                        autofire_delay -= 100;
                    }
                } else if(event->key == InputKeyUp) {
                    model->up_pressed = true;
                    autofire_delay += 100;
                } else if(event->key == InputKeyOk) {
                    model->ok_pressed = true;
                    is_running = !is_running;
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = true;
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = false;
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = false;
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = false;
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = false;
                } else if(event->key == InputKeyOk) {
                    model->ok_pressed = false;
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = false;
                }
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyBack) {
                }
            }
        },
        true);
}

static bool usb_ccb_start_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    UsbCcbStart* usb_ccb_start = context;
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
        furi_hal_hid_kb_release_all();
    } else {
        usb_ccb_start_process(usb_ccb_start, event);
        consumed = true;
    }

    return consumed;
}

UsbCcbStart* usb_ccb_start_alloc() {
    UsbCcbStart* usb_ccb_start = malloc(sizeof(UsbCcbStart));
    usb_ccb_start->view = view_alloc();
    view_set_context(usb_ccb_start->view, usb_ccb_start);
    view_allocate_model(usb_ccb_start->view, ViewModelTypeLocking, sizeof(UsbCcbStartModel));
    view_set_draw_callback(usb_ccb_start->view, usb_ccb_start_draw_callback);
    view_set_input_callback(usb_ccb_start->view, usb_ccb_start_input_callback);

    return usb_ccb_start;
}

void usb_ccb_start_free(UsbCcbStart* usb_ccb_start) {
    furi_assert(usb_ccb_start);
    view_free(usb_ccb_start->view);
    free(usb_ccb_start);
    i = 0;
}

View* usb_ccb_start_get_view(UsbCcbStart* usb_ccb_start) {
    furi_assert(usb_ccb_start);
    return usb_ccb_start->view;
}

void usb_ccb_start_set_connected_status(UsbCcbStart* usb_ccb_start, bool connected) {
    furi_assert(usb_ccb_start);
    with_view_model(
        usb_ccb_start->view, UsbCcbStartModel * model, { model->connected = connected; }, true);
}
