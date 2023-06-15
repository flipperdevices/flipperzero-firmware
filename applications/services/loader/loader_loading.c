#include <gui/gui.h>
#include <gui/view_holder.h>
#include <gui/modules/loading.h>

#include "loader_loading.h"

#define TAG "LoaderLoading"
#define LOADING_STOP_EVENT 0x1

struct LoaderLoading {
    FuriThread* thread;
};

static int32_t loader_loading_thread(void* p);

LoaderLoading* loader_loading_alloc() {
    LoaderLoading* loader_loading = malloc(sizeof(LoaderLoading));
    loader_loading->thread = furi_thread_alloc_ex(TAG, 512, loader_loading_thread, NULL);
    furi_thread_start(loader_loading->thread);
    return loader_loading;
}

void loader_loading_free(LoaderLoading* loader_loading) {
    furi_assert(loader_loading);
    furi_thread_flags_set(furi_thread_get_id(loader_loading->thread), LOADING_STOP_EVENT);
    furi_thread_join(loader_loading->thread);
    furi_thread_free(loader_loading->thread);
    free(loader_loading);
}

static int32_t loader_loading_thread(void* p) {
    UNUSED(p);

    Gui* gui = furi_record_open(RECORD_GUI);
    ViewHolder* view_holder = view_holder_alloc();
    Loading* loading = loading_alloc();

    view_holder_attach_to_gui(view_holder, gui);
    view_holder_set_view(view_holder, loading_get_view(loading));
    view_holder_start(view_holder);

    furi_thread_flags_wait(LOADING_STOP_EVENT, FuriFlagWaitAny, FuriWaitForever);

    view_holder_stop(view_holder);
    view_holder_free(view_holder);
    loading_free(loading);
    furi_record_close(RECORD_GUI);

    return 0;
}
