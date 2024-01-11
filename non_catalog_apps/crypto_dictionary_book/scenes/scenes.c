#include "scenes.h"
#include "../callbacks/callbacks.h"
#include "../resource/resource.h"
#include "../constants/constants.h"
#include "../buffer/dynamic_buffer.h"

void main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Cryptography Dict");

    for(size_t i = 0; i < number_of_chapters; i++) {
        submenu_add_item(app->submenu, chapters[i].name, i, menu_callback, app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, SubmenuView);
}

bool main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case MainMenuEvent:
            scene_manager_next_scene(app->scene_manager, ChapterScene);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}

void main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}

void chapter_scene_on_enter(void* context) {
    App* app = (App*)context;
    size_t chapterIndex = app->current_chapter_index;
    Chapter currentChapter = chapters[chapterIndex];

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, currentChapter.name);

    for(size_t i = 0; i < currentChapter.number_of_topics; i++) {
        const char* label = currentChapter.content[i].name;
        submenu_add_item(app->submenu, label, i, chapter_callback, app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, SubmenuView);
}

bool chapter_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void chapter_scene_on_exit(void* context) {
    UNUSED(context);
}

char* wrap_text(const char* text, size_t max_line_width) {
    size_t len = strlen(text);
    // More realistic initial size estimation
    size_t allocated_size = len + len / max_line_width + 1;
    char* wrapped = malloc(allocated_size);
    if (!wrapped) return NULL;

    size_t cur_line_len = 0;
    size_t wrapped_index = 0;
    size_t word_len = 0;

    for (size_t i = 0; i < len; ++i) {
        word_len++;
        if (text[i] == '\n') {
            for (size_t j = i - word_len + 1; j <= i; ++j) {
                // Check and reallocate if necessary before writing to the buffer
                if (wrapped_index >= allocated_size) {
                    allocated_size *= 2;
                    char* new_wrapped = realloc(wrapped, allocated_size);
                    if (!new_wrapped) {
                        free(wrapped);
                        return NULL;
                    }
                    wrapped = new_wrapped;
                }
                wrapped[wrapped_index++] = text[j];
            }
            cur_line_len = 0;
            word_len = 0;
            continue;
        }

        if (text[i] == ' ' || i == len - 1) {
            if (word_len >= max_line_width) {
                if (cur_line_len > 0) {
                    wrapped[wrapped_index++] = '\n';
                    cur_line_len = 0;
                }
                for (size_t j = i - word_len + 1; j <= i; ++j) {
                    // Check and reallocate if necessary before writing to the buffer
                    if (wrapped_index >= allocated_size) {
                        allocated_size *= 2;
                        char* new_wrapped = realloc(wrapped, allocated_size);
                        if (!new_wrapped) {
                            free(wrapped);
                            return NULL;
                        }
                        wrapped = new_wrapped;
                    }
                    wrapped[wrapped_index++] = text[j];
                    if (++cur_line_len >= max_line_width && text[j] != '\n') {
                        wrapped[wrapped_index++] = '\n';
                        cur_line_len = 0;
                    }
                }
            } else if (cur_line_len + word_len > max_line_width) {
                if (cur_line_len > 0) {
                    wrapped[wrapped_index++] = '\n';
                    cur_line_len = 0;
                }
            }
            for (size_t j = i - word_len + 1; j <= i; ++j) {
                // Check and reallocate if necessary before writing to the buffer
                if (wrapped_index >= allocated_size) {
                    allocated_size *= 2;
                    char* new_wrapped = realloc(wrapped, allocated_size);
                    if (!new_wrapped) {
                        free(wrapped);
                        return NULL;
                    }
                    wrapped = new_wrapped;
                }
                wrapped[wrapped_index++] = text[j];
            }
            cur_line_len += word_len;
            word_len = 0;
        }
    }

    wrapped[wrapped_index] = '\0';
    return wrapped;
}


void topic_scene_on_enter(void* context) {
    furi_assert(context);
    App* app = (App*)context;
    widget_reset(app->widget);

    const char* file_path = app->current_topic;

    DynamicBuffer dynamic_content;
    dynamic_buffer_init(&dynamic_content, 64);

    if(file_stream_open(app->file_stream, file_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        FuriString* line = furi_string_alloc();
        while(stream_read_line(app->file_stream, line)) {
            dynamic_buffer_append(&dynamic_content, furi_string_get_cstr(line), furi_string_size(line));
            dynamic_buffer_append(&dynamic_content, "\r\n", 1);
        }
        dynamic_buffer_append(&dynamic_content, "\0", 1);
        furi_string_free(line);
        file_stream_close(app->file_stream);
        size_t max_line_width = WIDGET_WIDTH / CHAR_WIDTH;
        char* wrapped_text = wrap_text(dynamic_content.data, max_line_width);
        dynamic_buffer_free(&dynamic_content);

        if(wrapped_text) {
            widget_add_text_scroll_element(
                app->widget, 2, 1, WIDGET_WIDTH, WIDGET_HEIGHT, wrapped_text);
            free(wrapped_text);
        }
    } else {
        dynamic_buffer_free(&dynamic_content);
        dynamic_buffer_init(&dynamic_content, 30);
        strcpy(dynamic_content.data, "Failed to open asset file.");
        widget_add_text_scroll_element(
            app->widget, 2, 1, WIDGET_WIDTH, WIDGET_HEIGHT, dynamic_content.data);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, WidgetView);
    dynamic_buffer_free(&dynamic_content);
}

bool topic_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void topic_scene_on_exit(void* context) {
    furi_assert(context);
    App* app = (App*)context;
    widget_reset(app->widget);
}