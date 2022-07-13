// THIS APP DOESNT WORK, YET
#include <furi.h>
#include <furi/record.h>
#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/empty_screen.h>
#include <m-string.h>
#include <furi_hal_version.h>
#include "dolphin/helpers/dolphin_deed.h"
#include "dolphin/helpers/dolphin_state.h"
#include <dolphin/dolphin.h>

#include "cmsis_os2.h"
#include "furi/common_defines.h"
#include "furi/log.h"
#include "loader/loader.h"
#include <stdint.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>

#include <gui/modules/widget.h>
#include <gui/modules/popup.h>
#include <storage/storage.h>
#include <storage/storage_i.h>
#include <storage/storage_message.h>
#include <storage/storage_sd_api.h>
// #include <storage/storage_external_api.c>

#include "toolbox/path.h"
#include <toolbox/stream/file_stream.h>
#include <toolbox/dir_walk.h>

#define LEVEL2_THRESHOLD 735
#define LEVEL3_THRESHOLD 2940

static const char* backupFiles[] = {
    "bt.settings",
    "desktop.settings",
    "dolphin.state",
    "notification.settings",
    "bt.keys",
};
#define MOVE_SRC "/int"
#define MOVE_DST "/ext/doplphin_restorer"

FS_Error backup_restorer_copy(Storage* storage, const char* old_path, const char* new_path) {
	FS_Error error;
	
	Stream* stream_from = file_stream_alloc(storage);
	Stream* stream_to = file_stream_alloc(storage);

	do {
		if(!file_stream_open(stream_from, old_path, FSAM_READ, FSOM_OPEN_EXISTING)) break;
		if(!file_stream_open(stream_to, new_path, FSAM_WRITE, FSOM_CREATE_NEW)) break;
		stream_copy_full(stream_from, stream_to);
	} while(false);

	error = file_stream_get_error(stream_from);
	if(error == FSE_OK) {
		error = file_stream_get_error(stream_to);
	}

	stream_free(stream_from);
	stream_free(stream_to);
    return error;
}

typedef DialogMessageButton (*AboutDialogScreen)(DialogsApp* dialogs, DialogMessage* message);
uint32_t backupStatusI = 0;
char* backupStatus = "Not Run";
bool backupDone = false;
bool restoreDone = false;

static DialogMessageButton welcome_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;
    string_t buffer;
    string_init(buffer);
    Dolphin* dolphin = furi_record_open("dolphin");
    DolphinStats stats = dolphin_stats(dolphin);
    furi_record_close("dolphin");
    uint32_t remainingXP = 0;
    if(stats.icounter <= LEVEL2_THRESHOLD) {
        remainingXP = LEVEL2_THRESHOLD - stats.icounter;
    } else if(stats.icounter <= LEVEL3_THRESHOLD) {
        remainingXP = LEVEL3_THRESHOLD - stats.icounter;
    } else {
        remainingXP = 0;
    }
    string_cat_printf( buffer, "%s%d - %s%d\n%s%d\n%s%s",
        "Level: ", stats.level, "XP: ", stats.icounter,
        "XP to Next Level: ", remainingXP, "Status: ", backupStatus );
    const char* screen_header = "Welcome to\n"
                                "Dolphin Restorer\n";
    dialog_message_set_header(message, screen_header, 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(message, string_get_cstr(buffer), 0, 24, AlignLeft, AlignTop);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_header(message, NULL, 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);
    return result;
}

static DialogMessageButton backup_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;
    const char* screen_text = "Backup Dolphin to SD\n"
                              "Push 'Backup' to start!\n";
    dialog_message_set_text(message, screen_text, 0, 0, AlignLeft, AlignTop);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);
    return result;
}

static DialogMessageButton restore_screen(DialogsApp* dialogs, DialogMessage* message) {
    DialogMessageButton result;
    const char* screen_text = "Restore Dolphin from SD\n"
                              "Push 'Restore' to start!\n";
    dialog_message_set_text(message, screen_text, 0, 0, AlignLeft, AlignTop);
    result = dialog_message_show(dialogs, message);
    dialog_message_set_text(message, NULL, 0, 0, AlignLeft, AlignTop);
    return result;
}

const AboutDialogScreen drestorer_screens[] = {
	welcome_screen,
	backup_screen,
	restore_screen};

const size_t drestorer_screens_count = sizeof(drestorer_screens) / sizeof(AboutDialogScreen);

int32_t drestorer_app(void* p) {
	backupStatusI = 0;
    backupStatus = "Not Run";
    backupDone = false;
    restoreDone = false;
    DialogsApp* dialogs = furi_record_open("dialogs");
    DialogMessage* message = dialog_message_alloc();
    Gui* gui = furi_record_open("gui");
    ViewDispatcher* view_dispatcher = view_dispatcher_alloc();
    EmptyScreen* empty_screen = empty_screen_alloc();
    const uint32_t empty_screen_index = 0;
    size_t screen_index = 0;
    DialogMessageButton screen_result;
    view_dispatcher_add_view(
        view_dispatcher, empty_screen_index, empty_screen_get_view(empty_screen));
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_switch_to_view(view_dispatcher, empty_screen_index);
    while(1) {
        if(screen_index == 1) {
            if(!backupDone) dialog_message_set_buttons(message, NULL, "Backup", NULL);
        } else if(screen_index == 2) {
            if(!restoreDone) dialog_message_set_buttons(message, NULL, "Restore", NULL);
        } else {
			if(backupDone && !restoreDone) {
				dialog_message_set_buttons(message, NULL, NULL, "Restore");
			} else if(!backupDone && restoreDone) {
				dialog_message_set_buttons(message, "Backup", NULL, NULL);
			} else if(!backupDone && !restoreDone) {
				dialog_message_set_buttons(message, "Backup", NULL, "Restore");
			} else {
				dialog_message_set_buttons(message, NULL, NULL, NULL);
			}
        }

        screen_result = drestorer_screens[screen_index](dialogs, message);
        if(screen_result == DialogMessageButtonLeft) {
			screen_index=1;
        } else if(screen_result == DialogMessageButtonRight) {
            screen_index=2;
        } else if(screen_result == DialogMessageButtonCenter) {
			if(screen_index==1) {
		        backupStatus = "Backed Up";
				backupDone=true;
			    backupStatusI=1;
				Storage* storage = furi_record_open("storage");
				string_t path_int;
				string_t path_sd;
				string_init(path_int);
				string_init(path_sd);
				for(uint32_t i = 0; i < COUNT_OF(backupFiles); i++) {
					string_printf(path_int, "%s/%s", MOVE_SRC, backupFiles[i]);
					string_printf(path_sd, "%s/%s", MOVE_DST, backupFiles[i]);
					// FS_Error error = 
					backup_restorer_copy(storage, string_get_cstr(path_int), string_get_cstr(path_sd));
					// unused(error);
				}
				string_clear(path_int);
				string_clear(path_sd);
				furi_record_close("storage");
                screen_index=0;
			}
			if(screen_index==2) {
		        backupStatus = "Restored";
				restoreDone=true;
			    backupStatusI=2;
				Storage* storage = furi_record_open("storage");
				string_t path_int;
				string_t path_sd;
				string_init(path_int);
				string_init(path_sd);
				for(uint32_t i = 0; i < COUNT_OF(backupFiles); i++) {
					string_printf(path_int, "%s/%s", MOVE_SRC, backupFiles[i]);
					string_printf(path_sd, "%s/%s", MOVE_DST, backupFiles[i]);
					FS_Error error = backup_restorer_copy(storage, string_get_cstr(path_sd), string_get_cstr(path_int));
					// unused(error);
				}
				string_clear(path_int);
				string_clear(path_sd);
				furi_record_close("storage");
                screen_index=0;
			}
			if(screen_index==1 || screen_index==2) {
				string_t buffer;
				string_init(buffer);
				Dolphin* dolphin = furi_record_open("dolphin");
				DolphinStats stats = dolphin_stats(dolphin);
				furi_record_close("dolphin");
				uint32_t remainingXP = 0;
				if(stats.icounter <= LEVEL2_THRESHOLD) {
					remainingXP = LEVEL2_THRESHOLD - stats.icounter;
				} else if(stats.icounter <= LEVEL3_THRESHOLD) {
					remainingXP = LEVEL3_THRESHOLD - stats.icounter;
				} else {
					remainingXP = 0;
				}
				if (backupStatusI==0) {
					backupStatus = "Not Run";
				} else if (backupStatusI==1) {
					backupStatus = "Backed Up";
				} else if (backupStatusI==2) {
					backupStatus = "Restored";
				} 
				string_cat_printf( buffer, "%s%d - %s%d\n%s%d\n%s%s",
					"Level: ", stats.level, "XP: ", stats.icounter,
					"XP to Next Level: ", remainingXP, "Status: ", backupStatus );
				const char* screen_header = "Welcome to\n"
											"Dolphin Restorer\n";
				dialog_message_set_header(message, screen_header, 0, 0, AlignLeft, AlignTop);
				dialog_message_set_text(message, string_get_cstr(buffer), 0, 24, AlignLeft, AlignTop);
				dialog_message_show(dialogs, message);
			}
        } else if(screen_result == DialogMessageButtonBack) {
            if(screen_index==0) {
				break;
		    } else {
                screen_index=0;
			}
        }
    }
    dialog_message_free(message);
    furi_record_close("dialogs");
    view_dispatcher_remove_view(view_dispatcher, empty_screen_index);
    view_dispatcher_free(view_dispatcher);
    empty_screen_free(empty_screen);
    furi_record_close("gui");
    return 0;
}