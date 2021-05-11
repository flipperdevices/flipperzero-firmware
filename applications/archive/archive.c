#include "archive.h"

// temp, for testing
const char* elements[3][5] = {
    {"test1", "test2", "test3", "test4", "test5"},
    {"test6", "test7", "test8", "test9", "test0"},
    {NULL, NULL, NULL, NULL, NULL},
};

static void update_offset(ArchiveState* archive, int length, int bounds, int offset) {
    View* tabs[] = {
        archive->view_favorite_items, archive->view_all_items, archive->view_ibutton_keys};

    ArchiveViewModelDefault* model = view_get_model(tabs[archive->current_tab]);

    if(model->idx > model->list_offset + (bounds - offset)) {
        model->list_offset = model->idx + 1;
    } else if(model->idx < bounds + (model->idx - (1 - offset))) {
        model->list_offset = model->idx - 1;
    }

    if(model->list_offset > length - bounds - 1) {
        model->list_offset = length - bounds - 1;
    }

    if(model->idx == 0) {
        model->list_offset = 0;
    }

    view_commit_model(tabs[archive->current_tab], true);
}

bool archive_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    ArchiveState* archive = context;

    View* tabs[] = {
        archive->view_favorite_items, archive->view_all_items, archive->view_ibutton_keys};

    if(event->type != InputTypeShort) return false;

    if(event->key == InputKeyLeft) {
        archive->current_tab = CLAMP(archive->current_tab - 1, ArchiveTabTotal, 0);

        ArchiveViewModelDefault* model = view_get_model(tabs[archive->current_tab]);

        model->tab_idx = archive->current_tab;
        model->list = *&elements[archive->current_tab];
        view_commit_model(tabs[archive->current_tab], true);
        view_dispatcher_switch_to_view(archive->view_dispatcher, archive->current_tab);

    } else if(event->key == InputKeyRight) {
        archive->current_tab = CLAMP(archive->current_tab + 1, ArchiveTabTotal - 1, 0);

        ArchiveViewModelDefault* model = view_get_model(tabs[archive->current_tab]);

        model->tab_idx = archive->current_tab;
        model->list = *&elements[archive->current_tab];

        view_commit_model(tabs[archive->current_tab], true);
        view_dispatcher_switch_to_view(archive->view_dispatcher, archive->current_tab);
    }

    if(event->key == InputKeyUp) {
        ArchiveViewModelDefault* model = view_get_model(tabs[archive->current_tab]);
        model->idx = CLAMP(model->idx - 1, 4, 0);
        update_offset(archive, 4, 2, 1);
        view_commit_model(tabs[archive->current_tab], true);
    }

    if(event->key == InputKeyDown) {
        ArchiveViewModelDefault* model = view_get_model(tabs[archive->current_tab]);

        model->idx = CLAMP(model->idx + 1, 4, 0);
        update_offset(archive, 4, 2, 1);
        view_commit_model(tabs[archive->current_tab], true);
    }

    return true;
}

ArchiveState* archive_alloc() {
    ArchiveState* archive = furi_alloc(sizeof(ArchiveState));
    // Message queue
    archive->event_queue = osMessageQueueNew(2, sizeof(AppEvent), NULL);

    furi_check(archive->event_queue);
    // Scene thread
    archive->app_thread = furi_thread_alloc();
    // GUI
    archive->gui = furi_record_open("gui");
    // Dispatcher
    archive->view_dispatcher = view_dispatcher_alloc();

    archive->fs_api = furi_record_open("sdcard");

    archive->file_select = file_select_alloc();
    // file_select_init(archive->file_select);
    // file_select_set_api(archive->file_select, archive->fs_api);

    // Favorites View
    archive->view_favorite_items = view_alloc();
    view_allocate_model(
        archive->view_favorite_items, ViewModelTypeLockFree, sizeof(ArchiveViewModelDefault));
    view_set_context(archive->view_favorite_items, archive);

    view_set_draw_callback(archive->view_favorite_items, archive_view_favorites);
    view_set_input_callback(archive->view_favorite_items, archive_view_input);
    view_dispatcher_add_view(
        archive->view_dispatcher, ArchiveTabFavorites, archive->view_favorite_items);

    // All Items View
    archive->view_all_items = view_alloc();
    view_set_context(archive->view_all_items, archive);
    view_allocate_model(
        archive->view_all_items, ViewModelTypeLockFree, sizeof(ArchiveViewModelDefault));

    view_set_draw_callback(archive->view_all_items, archive_view_all);
    view_set_input_callback(archive->view_all_items, archive_view_input);
    view_dispatcher_add_view(archive->view_dispatcher, ArchiveTabAll, archive->view_all_items);

    // Ibutton keys View
    archive->view_ibutton_keys = view_alloc();
    view_set_context(archive->view_ibutton_keys, archive);
    view_allocate_model(
        archive->view_ibutton_keys, ViewModelTypeLockFree, sizeof(ArchiveViewModelDefault));
    view_set_draw_callback(archive->view_ibutton_keys, archive_view_ibutton);
    view_set_input_callback(archive->view_ibutton_keys, archive_view_input);
    view_dispatcher_add_view(
        archive->view_dispatcher, ArchiveTabIButton, archive->view_ibutton_keys);

    view_dispatcher_attach_to_gui(
        archive->view_dispatcher, archive->gui, ViewDispatcherTypeFullscreen);

    return archive;
}

void archive_free(ArchiveState* archive) {
    furi_assert(archive);

    file_select_free(archive->file_select);

    furi_record_close("sdcard");
    archive->fs_api = NULL;

    view_dispatcher_free(archive->view_dispatcher);

    furi_record_close("gui");
    archive->gui = NULL;

    furi_thread_free(archive->app_thread);

    osMessageQueueDelete(archive->event_queue);

    free(archive);
}

static void event_cb(const void* value, void* ctx) {
    furi_assert(value);
    furi_assert(ctx);
    const InputEvent* event = value;
    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        osThreadFlagsSet((osThreadId_t)ctx, EXIT_FLAG);
    }
}

int32_t app_archive(void* p) {
    ArchiveState* archive = archive_alloc();

    PubSub* event_record = furi_record_open("input_events");
    PubSubItem* event_pubsub = subscribe_pubsub(event_record, event_cb, (void*)osThreadGetId());

    // default tab
    ArchiveViewModelDefault* model = view_get_model(archive->view_favorite_items);
    model->tab_idx = archive->current_tab;
    model->list = *&elements[archive->current_tab];
    view_commit_model(archive->view_favorite_items, true);

    view_dispatcher_switch_to_view(archive->view_dispatcher, archive->current_tab);

    while(1) {
        if(osThreadFlagsWait(EXIT_FLAG, osFlagsWaitAny, osWaitForever)) {
            break;
        }
    }
    unsubscribe_pubsub(event_pubsub);
    archive_free(archive);
    return 0;
}
