#include <flipper_application/flipper_application.h>
#include <flipper_application/elf/elf_file_i.h>
#include <loader/firmware_api/firmware_api.h>
#include <toolbox/path.h>
#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/loading.h>

#define APP_ARGS "run_in_background"
#define APP_EXIT_FLAG 0x01

#define TAG "BG Loader"

struct FlipperApplication {
	ELFDebugInfo state;
	FlipperApplicationManifest manifest;
	ELFFile* elf;
	FuriThread* thread;
	void* ep_thread_args;
};

typedef struct {
	FlipperApplication *fap;
	FuriThread *thread;
} BGLoaderApp;

typedef enum {
	BGLoaderView_Loading,
} BGLoaderView;

static BGLoaderApp *bgloader_app_alloc(void)
{
	return malloc(sizeof(BGLoaderApp));
}

static bool bgloader_start_external_app(BGLoaderApp *app, Storage *storage,
		const char *path, const char *args)
{
	FlipperApplication *fap = NULL;
	FuriThread *thread = NULL;
	bool success = false;

	do {
		fap = flipper_application_alloc(storage, firmware_api_interface);

		FURI_LOG_I(TAG, "Loading %s", path);
		
		FlipperApplicationPreloadStatus preload_res =
			flipper_application_preload(fap, path);
		if (preload_res != FlipperApplicationPreloadStatusSuccess) {
			const char* err_msg =
				flipper_application_preload_status_to_string(preload_res);
			FURI_LOG_W(TAG, "Preload failed, %s: %s", path, err_msg);
			break;
		}

		FlipperApplicationLoadStatus load_status =
			flipper_application_map_to_memory(fap);
		if(load_status != FlipperApplicationLoadStatusSuccess) {
			const char* err_msg = flipper_application_load_status_to_string(load_status);
			FURI_LOG_W(TAG, "Load failed, %s: %s", path, err_msg);
			break;
		}

		FURI_LOG_I(TAG, "Loaded");

		thread = flipper_application_alloc_thread(fap, args);
		FuriString* app_name = furi_string_alloc();
		path_extract_filename_no_ext(path, app_name);
		furi_thread_set_appid(thread, furi_string_get_cstr(app_name));
		furi_string_free(app_name);

		furi_thread_start(thread);
		success = true;
	} while (0);

	if (!success) {
		flipper_application_free(fap);
		return false;
	}

	app->fap = fap;
	app->thread = thread;

	return true;
}

static BGLoaderApp *bgloader_take_app(const char *path)
{
	if (furi_record_exists(path)) {
		return furi_record_open(path);
	}

	BGLoaderApp *app = bgloader_app_alloc();
	app->fap = NULL;
	app->thread = NULL;

	furi_record_create(path, app);

	return furi_record_open(path);
}

static void bgloader_put_app(const char *path)
{
	furi_record_close(path);
}

static bool bgloader_app_is_loaded(BGLoaderApp *app)
{
	return (app->fap != NULL);
}

static bool bgloader_load_app(BGLoaderApp *app, const char *path)
{
	furi_check(app->fap == NULL);
	furi_check(app->thread == NULL);

	Storage *storage = furi_record_open(RECORD_STORAGE);
	if (!storage_file_exists(storage, path)) {
		FURI_LOG_W(TAG, "Application \"%s\" not found", path);
		furi_record_close(RECORD_STORAGE);
		return false;
	}

	bool status = bgloader_start_external_app(app, storage, path,
			APP_ARGS);

	if (status) {
		furi_check(app->fap != NULL);
		furi_check(app->thread != NULL);
		storage_file_close(app->fap->elf->fd);
	} else {
		furi_check(app->fap == NULL);
		furi_check(app->thread == NULL);
	}

	furi_record_close(RECORD_STORAGE);

	return status;
}

static bool bgloader_unload_app(BGLoaderApp *app)
{
	furi_check(app->fap != NULL);
	furi_check(app->thread != NULL);

	furi_thread_flags_set(furi_thread_get_id(app->thread), APP_EXIT_FLAG);
	furi_thread_join(app->thread);
	FURI_LOG_I(TAG, "App returned: %li",
			furi_thread_get_return_code(app->thread));

	// this frees the thread too
	flipper_application_free(app->fap);

	app->fap = NULL;
	app->thread = NULL;

	return true;
}

static void bgloader_load_unload_app(const char* path)
{
	BGLoaderApp *app = bgloader_take_app(path);

	if (!bgloader_app_is_loaded(app)) {
		if (!bgloader_load_app(app, path)) {
			FURI_LOG_W(TAG, "Failed to load application \"%s\"",
					path);
		} else {
			FURI_LOG_I(TAG, "Application loaded");
		}
	} else {
		if (!bgloader_unload_app(app)) {
			FURI_LOG_W(TAG, "Failed to unload application \"%s\"",
					path);
		} else {
			FURI_LOG_I(TAG, "Application unloaded");
		}
	}

	bgloader_put_app(path);
}

int32_t bgloader(const char *args)
{
	UNUSED(args);

	FuriString *fap_path = furi_string_alloc_set(EXT_PATH("apps"));
	DialogsApp *dialogs = furi_record_open(RECORD_DIALOGS);
	Gui *gui = furi_record_open(RECORD_GUI);
	Loading *loading = loading_alloc();
	ViewDispatcher *view_dispatcher = view_dispatcher_alloc();

	view_dispatcher_add_view(view_dispatcher, BGLoaderView_Loading,
			loading_get_view(loading));
	view_dispatcher_attach_to_gui(view_dispatcher, gui,
			ViewDispatcherTypeFullscreen);
	view_dispatcher_switch_to_view(view_dispatcher, BGLoaderView_Loading);
	
	const DialogsFileBrowserOptions browser_options = {
		.extension = ".fap",
		.skip_assets = true,
		.icon = NULL,
		.hide_ext = true,
		.item_loader_callback = NULL,
		.item_loader_context = NULL,
		.base_path = EXT_PATH("apps"),
	};
	
	bool fap_selected = dialog_file_browser_show(
			dialogs,
			fap_path,
			fap_path,
			&browser_options);

	if (fap_selected) {
		FURI_LOG_I(TAG, "Selected fap \"%s\"",
				furi_string_get_cstr(fap_path));
		bgloader_load_unload_app(furi_string_get_cstr(fap_path));
	} else {
		FURI_LOG_I(TAG, "No fap selected");
	}

	furi_record_close(RECORD_GUI);
	furi_record_close(RECORD_DIALOGS);

	view_dispatcher_remove_view(view_dispatcher, BGLoaderView_Loading);

	view_dispatcher_free(view_dispatcher);
	loading_free(loading);
	furi_string_free(fap_path);

	FURI_LOG_I(TAG, "Exiting");

	return 0;
}
