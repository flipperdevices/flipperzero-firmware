#pragma once

#include <gui/view.h>

typedef struct DocDescription DocDescription;
typedef void (*DocDescriptionCallback)(void* ctx, uint8_t index);

//? Basic Functions
View* doc_description_get_view(DocDescription* instance);
void doc_description_set_callback(DocDescription* instance, DocDescriptionCallback callback, void* ctx);
DocDescription* doc_description_alloc();
void doc_description_free(DocDescription* instance);
//? Basic Functions End

//? Custom Functions
FuriString* doc_description_get_string(DocDescription* instance);
uint8_t doc_description_get_category(DocDescription* instance);
void doc_description_set_category(DocDescription* instance, uint8_t category);
void doc_description_set_size(DocDescription* instance, uint8_t size);
void doc_description_request_redraw(DocDescription* instance);
void doc_description_force_redraw(DocDescription* instance);
//? Custom Functions End