#include "stdglue.h"
#include "check.h"
#include "memmgr.h"
#include "thread_local_storage.h"

#include <FreeRTOS.h>
#include <task.h>

#include <furi_hal.h>
#include <m-dict.h>

DICT_DEF2(
    FuriStdglueCallbackDict,
    uint32_t,
    M_DEFAULT_OPLIST,
    FuriStdglueWriteCallback,
    M_PTR_OPLIST)

typedef struct {
    FuriMutex* mutex;
    FuriStdglueCallbackDict_t thread_outputs;
} FuriStdglue;

static FuriStdglue* furi_stdglue = NULL;

static ssize_t stdout_write(void* _cookie, const char* data, size_t size) {
    furi_assert(furi_stdglue);
    bool consumed = false;
    string_ptr buf = furi_thread_local_storage_get_buffer();

    // Flush
    if(size == 0 || data == NULL) {
        /*
         * This means that we should flush internal buffers.  Since we
         * don't we just return.  (Remember, "handle" == -1 means that all
         * handles should be flushed.)
         */
        FuriThreadId task_id = furi_thread_get_current_id();
        if(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING && task_id &&
           furi_mutex_acquire(furi_stdglue->mutex, FuriWaitForever) == FuriStatusOk) {
            // We are in the thread context
            // Handle thread callbacks
            FuriStdglueWriteCallback* callback_ptr =
                FuriStdglueCallbackDict_get(furi_stdglue->thread_outputs, (uint32_t)task_id);
            if(callback_ptr) {
                (*callback_ptr)(_cookie, string_get_cstr(buf), string_size(buf));
                consumed = true;
            }
            furi_check(furi_mutex_release(furi_stdglue->mutex) == FuriStatusOk);
        }

        // Debug uart
        if(!consumed) furi_hal_console_tx((const uint8_t*)string_get_cstr(buf), string_size(buf));

        string_reset(buf);

        return 0;
    } else {
        for(size_t i = 0; i < size; i++) {
            string_push_back(buf, data[i]);
        }

        return size;
    }
}

void furi_stdglue_init() {
    furi_stdglue = malloc(sizeof(FuriStdglue));
    // Init outputs structures
    furi_stdglue->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    furi_check(furi_stdglue->mutex);
    FuriStdglueCallbackDict_init(furi_stdglue->thread_outputs);
}

bool furi_stdglue_set_thread_stdout_callback(FuriStdglueWriteCallback callback) {
    furi_assert(furi_stdglue);
    FuriThreadId task_id = furi_thread_get_current_id();
    if(task_id) {
        furi_check(furi_mutex_acquire(furi_stdglue->mutex, FuriWaitForever) == FuriStatusOk);
        if(callback) {
            FuriStdglueCallbackDict_set_at(
                furi_stdglue->thread_outputs, (uint32_t)task_id, callback);
        } else {
            FuriStdglueCallbackDict_erase(furi_stdglue->thread_outputs, (uint32_t)task_id);
        }
        furi_check(furi_mutex_release(furi_stdglue->mutex) == FuriStatusOk);
        return true;
    } else {
        return false;
    }
}

size_t furi_stdglue_stdout_write(void* _cookie, const char* data, size_t size) {
    return stdout_write(_cookie, data, size);
}
