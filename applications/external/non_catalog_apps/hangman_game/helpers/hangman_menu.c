#include "hangman.h"

char** hangman_menu_read(size_t* menu_size) {
    CONST storage = furi_record_open(RECORD_STORAGE);

    CONST stream = file_stream_alloc(storage);
    CONST line = furi_string_alloc();

    char** lines = NULL;
    size_t capacity = 0;
    size_t cnt = 0;

    if(file_stream_open(stream, HANGMAN_MENU_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
        while(stream_read_line(stream, line)) {
            furi_string_trim(line);

            if(cnt == capacity) {
                capacity = capacity > 0 ? capacity * 2 : 1;
                lines = (char**)realloc(lines, capacity * sizeof(char*));
            }

            lines[cnt++] = strdup(furi_string_get_cstr(line));
        }
    } else {
        furi_crash(NULL);
    }

    *menu_size = cnt;

    furi_string_free(line);
    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);

    return lines;
}

void hangman_free_menu_data(char** lines, size_t menu_size) {
    for(size_t i = 0; i < menu_size; i++) {
        free(lines[i]);
    }

    free(lines);
}
