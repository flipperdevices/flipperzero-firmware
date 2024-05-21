#include "../archive_i.h"
#include "../helpers/archive_browser.h"

#define TAG "Archive"

const char* units[] = {"Bytes", "KiB", "MiB", "GiB", "TiB"};

void archive_scene_info_widget_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);
    ArchiveApp* app = (ArchiveApp*)context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

uint32_t archive_scene_info_dirwalk(void* context) {
    furi_assert(context);
    ArchiveApp* instance = context;

    char buf[128];
    FileInfo fileinfo;
    uint64_t total = 0;
    DirWalk* dir_walk = dir_walk_alloc(furi_record_open(RECORD_STORAGE));
    ArchiveFile_t* current = archive_get_current_file(instance->browser);
    if(dir_walk_open(dir_walk, furi_string_get_cstr(current->path))) {
        while(scene_manager_get_scene_state(instance->scene_manager, ArchiveAppSceneInfo)) {
            DirWalkResult result = dir_walk_read(dir_walk, NULL, &fileinfo);
            if(result == DirWalkError) {
                snprintf(buf, sizeof(buf), "Size: \e#Error\e#");
                widget_element_text_box_set_text(instance->element, buf);
                break;
            }
            bool is_last = result == DirWalkLast;
            if(!file_info_is_dir(&fileinfo) || is_last) {
                if(!is_last) total += fileinfo.size;
                double show = total;
                size_t unit;
                for(unit = 0; unit < COUNT_OF(units); unit++) {
                    if(show < 1024) break;
                    show /= 1024;
                }
                snprintf(
                    buf,
                    sizeof(buf),
                    unit ? "Size: %s\e#%.2f\e# %s" : "Size: %s\e#%.0f\e# %s",
                    is_last ? "" : "... ",
                    show,
                    units[unit]);
                widget_element_text_box_set_text(instance->element, buf);
            }
            if(is_last) break;
        }
    } else {
        snprintf(buf, sizeof(buf), "Size: \e#Error\e#");
        widget_element_text_box_set_text(instance->element, buf);
    }
    dir_walk_free(dir_walk);
    furi_record_close(RECORD_STORAGE);

    view_dispatcher_switch_to_view(instance->view_dispatcher, ArchiveViewWidget);
    return 0;
}

void archive_scene_info_on_enter(void* context) {
    furi_assert(context);
    ArchiveApp* instance = context;

    widget_add_button_element(
        instance->widget, GuiButtonTypeLeft, "Back", archive_scene_info_widget_callback, instance);

    FuriString* filename = furi_string_alloc();
    FuriString* dirname = furi_string_alloc();

    ArchiveFile_t* current = archive_get_current_file(instance->browser);
    char buf[128];

    // Filename
    path_extract_filename(current->path, filename, false);
    snprintf(buf, sizeof(buf), "\e#%s\e#", furi_string_get_cstr(filename));
    widget_add_text_box_element(
        instance->widget, 0, 0, 128, 24, AlignLeft, AlignCenter, buf, false);

    // Directory path
    path_extract_dirname(furi_string_get_cstr(current->path), dirname);
    widget_add_text_box_element(
        instance->widget,
        0,
        42,
        128,
        12,
        AlignLeft,
        AlignCenter,
        furi_string_get_cstr(dirname),
        false);

    // This one to return and cursor select this file
    path_extract_filename_no_ext(furi_string_get_cstr(current->path), filename);
    strlcpy(instance->text_store, furi_string_get_cstr(filename), MAX_NAME_LEN);

    furi_string_free(filename);
    furi_string_free(dirname);

    // File size
    FileInfo fileinfo;
    bool is_dir = false;
    if(storage_common_stat(
           furi_record_open(RECORD_STORAGE), furi_string_get_cstr(current->path), &fileinfo) !=
       FSE_OK) {
        snprintf(buf, sizeof(buf), "Size: \e#Error\e#");
    } else if(file_info_is_dir(&fileinfo)) {
        is_dir = true;
        snprintf(buf, sizeof(buf), "Size: ... \e#0\e# %s", units[0]);

    } else {
        double show = fileinfo.size;
        size_t unit;
        for(unit = 0; unit < COUNT_OF(units); unit++) {
            if(show < 1024) break;
            show /= 1024;
        }
        snprintf(
            buf,
            sizeof(buf),
            unit ? "Size: \e#%.2f\e# %s" : "Size: \e#%.0f\e# %s",
            show,
            units[unit]);
    }
    instance->element = widget_add_text_box_element(
        instance->widget, 0, 24, 128, 24, AlignLeft, AlignCenter, buf, true);
    furi_record_close(RECORD_STORAGE);

    view_dispatcher_switch_to_view(instance->view_dispatcher, ArchiveViewWidget);

    if(is_dir) {
        scene_manager_set_scene_state(instance->scene_manager, ArchiveAppSceneInfo, true);
        instance->thread = furi_thread_alloc_ex(
            "ArchiveInfoDirWalk", 1024, (FuriThreadCallback)archive_scene_info_dirwalk, instance);
        furi_thread_start(instance->thread);
    }
}

bool archive_scene_info_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    ArchiveApp* app = (ArchiveApp*)context;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_previous_scene(app->scene_manager);
        return true;
    }
    return false;
}

void archive_scene_info_on_exit(void* context) {
    furi_assert(context);
    ArchiveApp* app = (ArchiveApp*)context;

    scene_manager_set_scene_state(app->scene_manager, ArchiveAppSceneInfo, false);
    if(app->thread) {
        furi_thread_join(app->thread);
        furi_thread_free(app->thread);
        app->thread = NULL;
    }
    widget_reset(app->widget);
}
