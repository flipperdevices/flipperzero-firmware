#include "../archive_i.h"
#include "../helpers/archive_files.h"
#include "../helpers/archive_favorites.h"
#include "../helpers/archive_browser.h"
#include "../helpers/archive_helpers_ext.h"
#include "../views/archive_browser_view.h"
#include "archive/scenes/archive_scene.h"

#define TAG "ArchiveSceneBrowser"

#define SCENE_STATE_DEFAULT (0)
#define SCENE_STATE_NEED_REFRESH (1)

const char* archive_get_flipper_app_name(ArchiveFileTypeEnum file_type) {
    switch(file_type) {
    case ArchiveFileTypeIButton:
        return "iButton";
    case ArchiveFileTypeNFC:
        return "NFC";
    case ArchiveFileTypeSubGhz:
        return "Sub-GHz";
    case ArchiveFileTypeLFRFID:
        return "125 kHz RFID";
    case ArchiveFileTypeInfrared:
        return "Infrared";
    case ArchiveFileTypeSubghzPlaylist:
        return EXT_PATH("apps/Sub-GHz/subghz_playlist.fap");
    case ArchiveFileTypeSubghzRemote:
        return EXT_PATH("apps/Sub-GHz/subghz_remote_refactored.fap");
    case ArchiveFileTypeInfraredRemote:
        return EXT_PATH("apps/Infrared/ir_remote.fap");
    case ArchiveFileTypeBadKb:
        return "Bad KB";
    case ArchiveFileTypeWAV:
        return EXT_PATH("apps/Media/wav_player.fap");
    case ArchiveFileTypeMag:
        return EXT_PATH("apps/GPIO/magspoof.fap");
    case ArchiveFileTypeU2f:
        return "U2F";
    case ArchiveFileTypeUpdateManifest:
        return "UpdaterApp";
    case ArchiveFileTypeDiskImage:
        return EXT_PATH("apps/USB/mass_storage.fap");
    case ArchiveFileTypeJS:
        return EXT_PATH("apps/Main/js_app.fap");
    default:
        return NULL;
    }
}

static void archive_loader_callback(const void* message, void* context) {
    furi_assert(message);
    furi_assert(context);
    const LoaderEvent* event = message;
    ArchiveApp* archive = (ArchiveApp*)context;

    if(event->type == LoaderEventTypeApplicationStopped) {
        view_dispatcher_send_custom_event(
            archive->view_dispatcher, ArchiveBrowserEventListRefresh);
    }
}

static void archive_show_file(Loader* loader, const char* path) {
    File* file = storage_file_alloc(furi_record_open(RECORD_STORAGE));
    bool text = true;
    if(storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint8_t buf[1000];
        size_t read = storage_file_read(file, buf, sizeof(buf));
        for(size_t i = 0; i < read; i++) {
            const char c = buf[i];
            if((c < ' ' || c > '~') && c != '\r' && c != '\n') {
                text = false;
                break;
            }
        }
    }
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    if(text) {
        loader_start_detached_with_gui_error(loader, EXT_PATH("apps/Tools/text_viewer.fap"), path);
    } else {
        loader_start_detached_with_gui_error(loader, EXT_PATH("apps/Tools/hex_viewer.fap"), path);
    }
}

static void archive_mount_disk_image(ArchiveBrowserView* browser, ArchiveFile_t* selected) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* disk_image = NULL;
    do {
        if(browser->disk_image) {
            // Deinit and recycle File object
            if(storage_virtual_quit(storage) != FSE_OK) break;
            storage_file_close(browser->disk_image);
            disk_image = browser->disk_image;
            browser->disk_image = NULL;
        } else {
            disk_image = storage_file_alloc(storage);
        }

        if(!storage_file_open(
               disk_image,
               furi_string_get_cstr(selected->path),
               FSAM_READ | FSAM_WRITE,
               FSOM_OPEN_EXISTING))
            break;

        FS_Error init = storage_virtual_init(storage, disk_image);
        if(init == FSE_ALREADY_OPEN) {
            if(storage_virtual_quit(storage) == FSE_OK) {
                init = storage_virtual_init(storage, disk_image);
            }
        }
        if(init != FSE_OK) break;

        if(storage_virtual_mount(storage) != FSE_OK) {
            storage_virtual_quit(storage);
            break;
        }

        browser->disk_image = disk_image;

        while(archive_get_tab(browser) != ArchiveTabDiskImage) {
            archive_switch_tab(browser, TAB_LEFT);
        }
    } while(0);
    if(disk_image && !browser->disk_image) storage_file_free(disk_image);
    furi_record_close(RECORD_STORAGE);
}

static void
    archive_run_in_app(ArchiveBrowserView* browser, ArchiveFile_t* selected, bool favorites) {
    Loader* loader = furi_record_open(RECORD_LOADER);

    const char* app_name = archive_get_flipper_app_name(selected->type);

    if(selected->type == ArchiveFileTypeSearch) {
        while(archive_get_tab(browser) != ArchiveTabSearch) {
            archive_switch_tab(browser, TAB_LEFT);
        }
        browser->callback(ArchiveBrowserEventSearch, browser->context);
    } else if(app_name) {
        if(selected->is_app) {
            char* param = strrchr(furi_string_get_cstr(selected->path), '/');
            if(param != NULL) {
                param++;
            }
            loader_start_with_gui_error(loader, app_name, param);
        } else {
            const char* str = furi_string_get_cstr(selected->path);
            if(favorites &&
               (selected->type == ArchiveFileTypeIButton ||
                selected->type == ArchiveFileTypeLFRFID || selected->type == ArchiveFileTypeNFC ||
                selected->type == ArchiveFileTypeSubGhz)) {
                char arg[strlen(str) + 4];
                snprintf(arg, sizeof(arg), "fav%s", str);
                loader_start_with_gui_error(loader, app_name, arg);
            } else {
                loader_start_detached_with_gui_error(loader, app_name, str);
            }
        }
    } else if(selected->type == ArchiveFileTypeApplication) {
        loader_start_detached_with_gui_error(loader, furi_string_get_cstr(selected->path), NULL);
    } else {
        archive_show_file(loader, furi_string_get_cstr(selected->path));
    }

    furi_record_close(RECORD_LOADER);
}

// Hijack existing archive code for default app choosing without needing archive running
void run_with_default_app(const char* path) {
    // Kostily
    FileInfo info;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool is_dir = storage_common_stat(storage, path, &info) == FSE_OK &&
                  info.flags & FSF_DIRECTORY;
    furi_record_close(RECORD_STORAGE);

    // Velosipedy
    ArchiveFile_t item;
    ArchiveFile_t_init(&item);
    furi_string_set(item.path, path);
    archive_set_file_type(&item, path, is_dir, false);

    // Bydlo kod go brrr
    archive_run_in_app(NULL, &item, false);
    ArchiveFile_t_clear(&item);
}

void archive_scene_browser_callback(ArchiveBrowserEvent event, void* context) {
    ArchiveApp* archive = (ArchiveApp*)context;
    view_dispatcher_send_custom_event(archive->view_dispatcher, event);
}

void archive_scene_browser_on_enter(void* context) {
    ArchiveApp* archive = (ArchiveApp*)context;
    ArchiveBrowserView* browser = archive->browser;
    browser->is_root = true;

    archive_browser_set_callback(browser, archive_scene_browser_callback, archive);
    if(archive_get_tab(browser) == ArchiveTabFavorites && archive_favorites_count() < 1) {
        archive_switch_tab(browser, TAB_LEFT);
    }
    archive_update_focus(browser, archive->text_store);
    view_dispatcher_switch_to_view(archive->view_dispatcher, ArchiveViewBrowser);

    archive->loader_stop_subscription = furi_pubsub_subscribe(
        loader_get_pubsub(archive->loader), archive_loader_callback, archive);

    uint32_t state = scene_manager_get_scene_state(archive->scene_manager, ArchiveAppSceneBrowser);

    if(state == SCENE_STATE_NEED_REFRESH) {
        view_dispatcher_send_custom_event(
            archive->view_dispatcher, ArchiveBrowserEventListRefresh);
    }

    scene_manager_set_scene_state(
        archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_DEFAULT);
}

bool archive_scene_browser_on_event(void* context, SceneManagerEvent event) {
    ArchiveApp* archive = (ArchiveApp*)context;
    ArchiveBrowserView* browser = archive->browser;
    ArchiveFile_t* selected = archive_get_current_file(browser);

    bool favorites = archive_get_tab(browser) == ArchiveTabFavorites;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case ArchiveBrowserEventFileMenuOpen:
            archive_show_file_menu(browser, true, false);
            consumed = true;
            break;
        case ArchiveBrowserEventManageMenuOpen:
            archive_show_file_menu(browser, true, true);
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuClose:
            archive_show_file_menu(browser, false, false);
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuRun:
            if(archive_is_known_app(selected->type)) {
                archive_run_in_app(browser, selected, favorites);
            }
            archive_show_file_menu(browser, false, false);
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuFavorite: {
            const char* name = archive_get_name(browser);
            if(favorites) {
                archive_favorites_delete("%s", name);
                archive_file_array_rm_selected(browser);
            } else if(archive_is_known_app(selected->type)) {
                if(archive_is_favorite("%s", name)) {
                    archive_favorites_delete("%s", name);
                } else {
                    archive_file_append(ARCHIVE_FAV_PATH, "%s\n", name);
                }
            }
            archive_show_file_menu(browser, false, false);
            consumed = true;
        } break;
        case ArchiveBrowserEventFileMenuInfo:
            archive_show_file_menu(browser, false, false);
            scene_manager_set_scene_state(
                archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_NEED_REFRESH);
            scene_manager_next_scene(archive->scene_manager, ArchiveAppSceneInfo);
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuShow:
            if(selected->type == ArchiveFileTypeDiskImage &&
               archive_get_tab(browser) != ArchiveTabDiskImage) {
                archive_mount_disk_image(browser, selected);
            } else {
                archive_show_file(
                    furi_record_open(RECORD_LOADER), furi_string_get_cstr(selected->path));
                furi_record_close(RECORD_LOADER);
            }
            archive_show_file_menu(browser, false, false);
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuPaste:
            archive_show_file_menu(browser, false, false);
            if(!favorites) {
                FuriString* path_src = NULL;
                FuriString* path_dst = NULL;
                bool copy;
                with_view_model(
                    browser->view,
                    ArchiveBrowserViewModel * model,
                    {
                        if(model->clipboard != NULL) {
                            path_src = furi_string_alloc_set(model->clipboard);
                            path_dst = furi_string_alloc();
                            FuriString* base = furi_string_alloc();
                            path_extract_basename(model->clipboard, base);
                            path_concat(
                                furi_string_get_cstr(browser->path),
                                furi_string_get_cstr(base),
                                path_dst);
                            furi_string_free(base);
                            copy = model->clipboard_copy;
                            free(model->clipboard);
                            model->clipboard = NULL;
                        }
                    },
                    false);
                if(path_src && path_dst) {
                    view_dispatcher_switch_to_view(archive->view_dispatcher, ArchiveViewStack);
                    archive_show_loading_popup(archive, true);
                    FS_Error error = archive_copy_rename_file_or_dir(
                        archive->browser, furi_string_get_cstr(path_src), path_dst, copy, true);
                    archive_show_loading_popup(archive, false);
                    if(error != FSE_OK) {
                        FuriString* dialog_msg;
                        dialog_msg = furi_string_alloc();
                        furi_string_cat_printf(
                            dialog_msg,
                            "Cannot %s:\n%s",
                            copy ? "copy" : "move",
                            storage_error_get_desc(error));
                        dialog_message_show_storage_error(
                            archive->dialogs, furi_string_get_cstr(dialog_msg));
                        furi_string_free(dialog_msg);
                    } else {
                        ArchiveFile_t* current = archive_get_current_file(archive->browser);
                        if(current != NULL) furi_string_set(current->path, path_dst);
                        view_dispatcher_send_custom_event(
                            archive->view_dispatcher, ArchiveBrowserEventListRefresh);
                    }
                    furi_string_free(path_src);
                    furi_string_free(path_dst);
                    view_dispatcher_switch_to_view(archive->view_dispatcher, ArchiveViewBrowser);
                }
            }
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuCut:
            archive_show_file_menu(browser, false, false);
            if(!favorites) {
                with_view_model(
                    browser->view,
                    ArchiveBrowserViewModel * model,
                    {
                        if(model->clipboard == NULL) {
                            model->clipboard = strdup(furi_string_get_cstr(selected->path));
                            model->clipboard_copy = false;
                        }
                    },
                    false);
            }
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuCopy:
            archive_show_file_menu(browser, false, false);
            if(!favorites) {
                with_view_model(
                    browser->view,
                    ArchiveBrowserViewModel * model,
                    {
                        if(model->clipboard == NULL) {
                            model->clipboard = strdup(furi_string_get_cstr(selected->path));
                            model->clipboard_copy = true;
                        }
                    },
                    false);
            }
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuNewDir:
            archive_show_file_menu(browser, false, false);
            if(!favorites) {
                scene_manager_set_scene_state(
                    archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_NEED_REFRESH);
                scene_manager_next_scene(archive->scene_manager, ArchiveAppSceneNewDir);
            }
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuRename:
            archive_show_file_menu(browser, false, false);
            scene_manager_set_scene_state(
                archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_NEED_REFRESH);
            scene_manager_next_scene(archive->scene_manager, ArchiveAppSceneRename);
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuDelete:
            archive_show_file_menu(browser, false, false);
            scene_manager_set_scene_state(
                archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_NEED_REFRESH);
            scene_manager_next_scene(archive->scene_manager, ArchiveAppSceneDelete);
            consumed = true;
            break;
        case ArchiveBrowserEventEnterDir:
            if(favorites) {
                archive_switch_tab(browser, TAB_LEFT);
            }
            archive_enter_dir(browser, selected->path);
            consumed = true;
            break;
        case ArchiveBrowserEventSearch: {
            bool open =
                !scene_manager_get_scene_state(archive->scene_manager, ArchiveAppSceneSearch);
            scene_manager_set_scene_state(archive->scene_manager, ArchiveAppSceneSearch, false);
            if(archive->thread) {
                furi_thread_join(archive->thread);
                furi_thread_free(archive->thread);
                archive->thread = NULL;
            }
            if(open) scene_manager_next_scene(archive->scene_manager, ArchiveAppSceneSearch);
            consumed = true;
            break;
        }
        case ArchiveBrowserEventFavMoveUp:
            archive_file_array_swap(browser, 1);
            consumed = true;
            break;
        case ArchiveBrowserEventFavMoveDown:
            archive_file_array_swap(browser, -1);
            consumed = true;
            break;
        case ArchiveBrowserEventEnterFavMove:
            archive_show_file_menu(browser, false, false);
            furi_string_set(archive->fav_move_str, selected->path);
            archive_favorites_move_mode(archive->browser, true);
            consumed = true;
            break;
        case ArchiveBrowserEventExitFavMove:
            archive_update_focus(browser, furi_string_get_cstr(archive->fav_move_str));
            archive_favorites_move_mode(archive->browser, false);
            consumed = true;
            break;
        case ArchiveBrowserEventSaveFavMove:
            archive_favorites_move_mode(archive->browser, false);
            archive_favorites_save(archive->browser);
            consumed = true;
            break;
        case ArchiveBrowserEventLoadPrevItems:
            archive_file_array_load(archive->browser, -1);
            consumed = true;
            break;
        case ArchiveBrowserEventLoadNextItems:
            archive_file_array_load(archive->browser, 1);
            consumed = true;
            break;
        case ArchiveBrowserEventListRefresh:
            if(!favorites) {
                archive_refresh_dir(browser);
            } else {
                archive_favorites_read(browser);
            }
            consumed = true;
            break;

        case ArchiveBrowserEventExit:
            if(!archive_is_home(browser)) {
                archive_leave_dir(browser);
            } else {
                if(archive->loader_stop_subscription) {
                    furi_pubsub_unsubscribe(
                        loader_get_pubsub(archive->loader), archive->loader_stop_subscription);
                    archive->loader_stop_subscription = NULL;
                }

                view_dispatcher_stop(archive->view_dispatcher);
            }
            consumed = true;
            break;

        default:
            break;
        }
    }
    return consumed;
}

void archive_scene_browser_on_exit(void* context) {
    ArchiveApp* archive = (ArchiveApp*)context;

    if(archive->loader_stop_subscription) {
        furi_pubsub_unsubscribe(
            loader_get_pubsub(archive->loader), archive->loader_stop_subscription);
        archive->loader_stop_subscription = NULL;
    }
}
