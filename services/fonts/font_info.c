#include "font_info.h"
#include <furi/core/check.h>

FontInfo* totp_font_info_alloc() {
    FontInfo* font_info = malloc(sizeof(FontInfo));
    furi_check(font_info != NULL);
    font_info->data = NULL;
    font_info->char_info = NULL;
    return font_info;
}

void totp_font_info_free(FontInfo* font_info) {
    if(font_info == NULL) return;
    if(font_info->char_info != NULL) {
        free(font_info->char_info);
    }

    if(font_info->data != NULL) {
        free(font_info->data);
    }

    if(font_info->name != NULL) {
        free(font_info->name);
    }

    free(font_info);
}