#include "archive_i.h"

ArchiveApp* archive_alloc() {
    ArchiveApp* archive = furi_alloc(sizeof(ArchiveApp));

    archive->gui = furi_record_open("gui");
    archive->text_input = text_input_alloc();

    archive->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_allocate_scene_manager(
        archive->view_dispatcher, &archive_scene_handlers, archive);
    view_dispatcher_set_start_scene(archive->view_dispatcher, ArchiveAppSceneBrowser);
    archive->scene_manager = view_dispatcher_get_scene_manager(archive->view_dispatcher);

    view_dispatcher_enable_queue(archive->view_dispatcher);
    view_dispatcher_attach_to_gui(
        archive->view_dispatcher, archive->gui, ViewDispatcherTypeFullscreen);

    archive->browser = browser_alloc();

    view_dispatcher_add_view(
        archive->view_dispatcher, ArchiveViewBrowser, archive_browser_get_view(archive->browser));

    view_dispatcher_add_view(
        archive->view_dispatcher, ArchiveViewTextInput, text_input_get_view(archive->text_input));

    return archive;
}

void archive_free(ArchiveApp* archive) {
    furi_assert(archive);

    view_dispatcher_remove_view(archive->view_dispatcher, ArchiveViewBrowser);
    view_dispatcher_remove_view(archive->view_dispatcher, ArchiveViewTextInput);
    view_dispatcher_free(archive->view_dispatcher);
    browser_free(archive->browser);

    text_input_free(archive->text_input);

    furi_record_close("gui");
    archive->gui = NULL;

    free(archive);
}

int32_t archive_app(void* p) {
    ArchiveApp* archive = archive_alloc();
    view_dispatcher_run(archive->view_dispatcher);
    archive_free(archive);

    return 0;
}
