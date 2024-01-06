#include "scenes.h"
#include "../callbacks/callbacks.h"
#include "../constants/topics.h"
#include "../constants/constants.h"

void book_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Chapters");
    for(size_t i = 0; i < number_of_chapters; i++) {
        submenu_add_item(app->submenu, chapters[i].chapter_title, i, book_menu_callback, app);
    }
    view_dispatcher_switch_to_view(app->view_dispatcher, BookSubmenuView);
}

bool book_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case BookChapterEvent:
            scene_manager_next_scene(app->scene_manager, BookChapterScene);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}

void book_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}

void book_chapter_scene_on_enter(void* context) {
    App* app = (App*)context;
    size_t chapterIndex = app->current_chapter_index;
    BookChapter currentChapter = chapters[chapterIndex];
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, currentChapter.chapter_title);
    for(size_t i = 0; i < currentChapter.number_of_details; i++) {
        const char* label = currentChapter.details[i].name;
        submenu_add_item(app->submenu, label, i, book_chapter_callback, app);
    }
    view_dispatcher_switch_to_view(app->view_dispatcher, BookSubmenuView);
}

bool book_chapter_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void book_chapter_scene_on_exit(void* context) {
    UNUSED(context);
}

char* wrap_text(const char* text, size_t max_line_width) {
    size_t len = strlen(text);
    size_t allocated_size = len / 2 + 1;  // Start with half the length of the original text
    char* wrapped = malloc(allocated_size);
    if (!wrapped) return NULL;

    size_t cur_line_len = 0;
    size_t wrapped_index = 0;
    size_t word_len = 0;

    for (size_t i = 0; i < len; ++i) {
        word_len++;

        // When a newline character is encountered
        if (text[i] == '\n') {
            // Add the current word to the wrapped text
            for (size_t j = i - word_len + 1; j <= i; ++j) {
                if (wrapped_index >= allocated_size - 1) {
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
            cur_line_len = 0; // Reset current line length as we're starting a new line
            word_len = 0; // Reset word length
            continue; // Move to the next character
        }

        if (text[i] == ' ' || i == len - 1) {
            // Check if word length exceeds max line width
            if (word_len >= max_line_width) {
                if (cur_line_len > 0) {
                    wrapped[wrapped_index++] = '\n'; // Insert a line break
                    cur_line_len = 0;
                }
                // Split the long word across multiple lines
                for (size_t j = i - word_len + 1; j <= i; ++j) {
                    if (wrapped_index >= allocated_size - 1) {
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
            // Add the current word to the wrapped text
            for (size_t j = i - word_len + 1; j <= i; ++j) {
                if (wrapped_index >= allocated_size - 1) {
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
            word_len = 0; // Reset word length after adding it to the wrapped text
        }
    }

    wrapped[wrapped_index] = '\0';
    return wrapped;
}

void book_topic_scene_on_enter(void* context) {
    furi_assert(context);
    App* app = (App*)context;
    widget_reset(app->widget);
    size_t max_chars_per_line = WIDGET_WIDTH / CHAR_WIDTH;
    char* wrapped_text = wrap_text(app->current_detail_text, max_chars_per_line);
    if(wrapped_text) {
        widget_add_text_scroll_element(
            app->widget, 0, 0, WIDGET_WIDTH, WIDGET_HEIGHT, wrapped_text);
        free(wrapped_text);
    } else {
        widget_add_text_box_element(
            app->widget, 0, 0, WIDGET_WIDTH, WIDGET_HEIGHT, AlignLeft, AlignCenter,
            "\e#Error:\nMemory allocation failed\e#", false);
    }
    view_dispatcher_switch_to_view(app->view_dispatcher, BookWidgetView);
}

bool book_topic_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void book_topic_scene_on_exit(void* context) {
    furi_assert(context);
    App* app = (App*)context;
    widget_reset(app->widget);
}