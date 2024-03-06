
#include <furi.h>
#include <storage/storage.h>
#include <toolbox/dir_walk.h>
#include <lib/toolbox/path.h>

#include "app_state.h"
#include "item.h"
#include <m-array.h>

// Location of our actions and folders
#define QUAC_PATH "apps_data/quac"
// Full path to actions
#define QUAC_DATA_PATH EXT_PATH(QUAC_PATH)

ARRAY_DEF(FileArray, FuriString*, FURI_STRING_OPLIST);

ItemsView* item_get_items_view_from_path(void* context, FuriString* input_path) {
    App* app = context;

    if(input_path == NULL) {
        input_path = furi_string_alloc_set_str(QUAC_DATA_PATH);
    }
    const char* cpath = furi_string_get_cstr(input_path);

    FURI_LOG_I(TAG, "Getting items from: %s", cpath);
    ItemsView* iview = malloc(sizeof(ItemsView));
    iview->path = furi_string_alloc_set(input_path);

    iview->name = furi_string_alloc();
    if(app->depth == 0) {
        FURI_LOG_I(TAG, "Depth is ZERO!");
        furi_string_set_str(iview->name, QUAC_NAME);
    } else {
        FURI_LOG_I(TAG, "Depth is %d", app->depth);
        path_extract_basename(cpath, iview->name);
        item_prettify_name(iview->name);
    }

    DirWalk* dir_walk = dir_walk_alloc(app->storage);
    dir_walk_set_recursive(dir_walk, false);

    FuriString* path = furi_string_alloc();
    FileArray_t flist;
    FileArray_init(flist);

    // FURI_LOG_I(TAG, "About to walk the dir");
    if(dir_walk_open(dir_walk, cpath)) {
        while(dir_walk_read(dir_walk, path, NULL) == DirWalkOK) {
            // FURI_LOG_I(TAG, "> dir_walk: %s", furi_string_get_cstr(path));
            const char* cpath = furi_string_get_cstr(path);

            // Insert the new file path in sorted order to flist
            uint32_t i = 0;
            FileArray_it_t it;
            for(FileArray_it(it, flist); !FileArray_end_p(it); FileArray_next(it), ++i) {
                if(strcmp(cpath, furi_string_get_cstr(*FileArray_ref(it))) > 0) {
                    continue;
                }
                // FURI_LOG_I(TAG, ">> Inserting at %lu", i);
                FileArray_push_at(flist, i, path);
                break;
            }
            if(i == FileArray_size(flist)) {
                // FURI_LOG_I(TAG, "Couldn't insert, so adding at the end!");
                FileArray_push_back(flist, path);
            }
        }
    }
    furi_string_free(path);

    // DEBUG: Now print our array in original order
    FileArray_it_t iter;
    for(FileArray_it(iter, flist); !FileArray_end_p(iter); FileArray_next(iter)) {
        const char* f = furi_string_get_cstr(*FileArray_cref(iter));
        FURI_LOG_I(TAG, "Found: %s", f);
    }

    FURI_LOG_I(TAG, "Creating our ItemsArray");
    ItemArray_init(iview->items);
    for(FileArray_it(iter, flist); !FileArray_end_p(iter); FileArray_next(iter)) {
        path = *FileArray_ref(iter);
        const char* found_path = furi_string_get_cstr(path);

        Item* item = ItemArray_push_new(iview->items);

        // Action files have extensions, so item->ext starts with '.' - ehhhh
        item->ext[0] = 0;
        path_extract_extension(path, item->ext, MAX_EXT_LEN);
        item->type = (item->ext[0] == '.') ? Item_Action : Item_Group;

        item->name = furi_string_alloc();
        path_extract_filename_no_ext(found_path, item->name);
        FURI_LOG_I(TAG, "Basename: %s", furi_string_get_cstr(item->name));
        item_prettify_name(item->name);

        item->path = furi_string_alloc();
        furi_string_set(item->path, path);
        FURI_LOG_I(TAG, "Path: %s", furi_string_get_cstr(item->path));
    }

    FileArray_clear(flist);
    dir_walk_free(dir_walk);

    return iview;
}

void item_items_view_free(ItemsView* items_view) {
    FURI_LOG_I(TAG, "item_items_view_free - begin");
    furi_string_free(items_view->name);
    furi_string_free(items_view->path);
    ItemArray_it_t iter;
    for(ItemArray_it(iter, items_view->items); !ItemArray_end_p(iter); ItemArray_next(iter)) {
        furi_string_free(ItemArray_ref(iter)->name);
        furi_string_free(ItemArray_ref(iter)->path);
    }
    ItemArray_clear(items_view->items);
    free(items_view);
    FURI_LOG_I(TAG, "item_items_view_free - end");
}

void item_prettify_name(FuriString* name) {
    // FURI_LOG_I(TAG, "Converting %s to...", furi_string_get_cstr(name));
    if(furi_string_size(name) > 3) {
        char c = furi_string_get_char(name, 2);
        if(c == '_') {
            char a = furi_string_get_char(name, 0);
            char b = furi_string_get_char(name, 1);
            if(a >= '0' && a <= '9' && b >= '0' && b <= '9') {
                furi_string_right(name, 3);
            }
        }
    }
    furi_string_replace_str(name, "_", " ", 0);
    // FURI_LOG_I(TAG, "... %s", furi_string_get_cstr(name));
}