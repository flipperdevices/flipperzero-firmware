#pragma once

#include <gui/view.h>

typedef struct MassStorage MassStorage;

MassStorage* mass_storage_alloc();

void mass_storage_free(MassStorage* mass_storage);

View* mass_storage_get_view(MassStorage* mass_storage);

void mass_storage_set_file_name(MassStorage* mass_storage, FuriString* name);
