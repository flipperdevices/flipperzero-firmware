#pragma once

#include <gui/view.h>

typedef struct DocSelection DocSelection;
typedef void (*DocSelectionCallback)(void* ctx, uint8_t index);

//? Basic Functions
View* doc_selection_get_view(DocSelection* instance);
void doc_selection_set_callback(DocSelection* instance, DocSelectionCallback callback, void* ctx);
DocSelection* doc_selection_alloc();
void doc_selection_free(DocSelection* instance);
//? Basic Functions End

//? Custom Functions
void doc_selection_set_title(DocSelection* instance, const char* title);
FuriString* doc_selection_get_string(DocSelection* instance);
void doc_selection_set_footer(DocSelection* instance, const char* footer);
void doc_selection_set_index(DocSelection* instance, uint8_t index);
void doc_selection_set_size(DocSelection* instance, uint8_t size);
void doc_selection_request_redraw(DocSelection* instance);
void doc_selection_force_redraw(DocSelection* instance);
//? Custom Functions End