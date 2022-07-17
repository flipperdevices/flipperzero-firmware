#include "flipper_application.h"
#include "flipper_application_i.h"

#define TAG "fapp"

FlipperApplication*
    flipper_application_alloc(Storage* storage, const ElfApiInterface* api_interface) {
    FlipperApplication* app = malloc(sizeof(FlipperApplication));
    app->api_interface = api_interface;
    app->fd = storage_file_alloc(storage);
    return app;
}

void flipper_application_free(FlipperApplication* app) {
    if(app->thread) {
        furi_thread_join(app->thread);
        furi_thread_free(app->thread);
    }

    if(app->state.entries) {
        free(app->state.entries);
        app->state.entries = NULL;
    }

    ELFSection_t* sections[] = {&app->text, &app->rodata, &app->data, &app->bss};
    for(size_t i = 0; i < COUNT_OF(sections); i++) {
        flipper_application_free_section(sections[i]);
    }

    storage_file_free(app->fd);
}

/* Parse headers, load manifest */
FlipperApplicationPreloadStatus
    flipper_application_preload(FlipperApplication* app, const char* path) {
    if(!flipper_application_load_elf_headers(app, path) ||
       !flipper_application_load_section_table(app)) {
        return FlipperApplicationPreloadStatusInvalidFile;
    }

    if((app->manifest.base.manifest_magic != FAP_MANIFEST_MAGIC) &&
       (app->manifest.base.manifest_version == FAP_MANIFEST_SUPPORTED_VERSION)) {
        return FlipperApplicationPreloadStatusInvalidManifest;
    }

    // TODO: api version check
    // return FlipperApplicationPreloadStatusApiMismatch;

    return FlipperApplicationPreloadStatusSuccess;
}

const FlipperApplicationManifest* flipper_application_get_manifest(FlipperApplication* app) {
    return &app->manifest;
}

FlipperApplicationLoadStatus flipper_application_map_to_memory(FlipperApplication* app) {
    return flipper_application_load_sections(app);
}

const FlipperApplicationState* flipper_application_get_state(FlipperApplication* app) {
    return &app->state;
}

FuriThread* flipper_application_spawn(FlipperApplication* app, void* args) {
    furi_check(app->thread == NULL);

    const FlipperApplicationManifest* manifest = flipper_application_get_manifest(app);
    furi_check(manifest->stack_size > 0);

    app->thread = furi_thread_alloc();
    furi_thread_set_stack_size(app->thread, manifest->stack_size);
    furi_thread_set_name(app->thread, manifest->name);
    furi_thread_set_callback(app->thread, (entry_t*)app->entry);
    furi_thread_set_context(app->thread, args);

    furi_thread_start(app->thread);
    return app->thread;
}
