#include "subghz_file_encoder_worker.h"
#include <stream_buffer.h>

#include "file-worker.h"

#define SUBGHZ_FILE_ENCODER_LOAD 512

struct SubGhzFileEncoderWorker {
    FuriThread* thread;
    StreamBufferHandle_t stream;
    FileWorker* file_worker;

    volatile bool worker_running;
    bool level;
    uint32_t duration;
    string_t str_data;
    string_t file_path;
};

/** Rx callback timer
 * 
 * @param level received signal level
 * @param duration received signal duration
 * @param context 
 */
void subghz_file_encoder_worker_add_livel_duration(SubGhzFileEncoderWorker* instance, int duration) {
    LevelDuration level_duration = {.level = 0, .duration = LEVEL_DURATION_RESERVED};

    if(duration < 0 && !instance->level) {
        instance->duration += duration;
    } else if(duration > 0 && instance->level) {
        instance->duration += duration;
    } else if(duration < 0 && instance->level) {
        instance->duration += duration;
        level_duration = level_duration_make(false, instance->duration * -1);
        instance->level = false;
        instance->duration = 0;
    } else if(duration > 0 && !instance->level) {
        instance->duration += duration;
        level_duration = level_duration_make(true, instance->duration);
        instance->level = true;
        instance->duration = 0;
    } else if(duration == 0) {
        level_duration = level_duration_reset();
    }

    if(level_duration.duration != LEVEL_DURATION_RESERVED) {
        //size_t ret =
        xStreamBufferSend(instance->stream, &level_duration, sizeof(LevelDuration), 10);
    }
}

bool subghz_file_encoder_worker_data_parse(
    SubGhzFileEncoderWorker* instance,
    const char* strStart,
    size_t len) {
    char* str1;
    size_t ind_start = (size_t)strStart; //store the start address of the beginning of the line
    bool res = false;

    str1 = strstr(
        strStart, "RAW_Data: "); //looking for the beginning of the desired title in the line
    if(str1 != NULL) {
        str1 = strchr(
            str1,
            ' '); //if found, shift the pointer by 1 element per line "RAW_Data: -1, 2, -2..."
        subghz_file_encoder_worker_add_livel_duration(instance, atoi(str1));
        while(
            strchr(str1, ',') != NULL &&
            ((size_t)str1 <
             (len +
              ind_start))) { //check that there is still an element in the line and that it has not gone beyond the line
            str1 = strchr(str1, ',');
            str1 += 2; //if found, shift the pointer by next element per line
            subghz_file_encoder_worker_add_livel_duration(instance, atoi(str1));
        }
        res = true;
    }
    return res;
}

LevelDuration subghz_file_encoder_worker_get_level_duration(void* context) {
    furi_assert(context);
    SubGhzFileEncoderWorker* instance = context;
    LevelDuration level_duration;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    int ret = xStreamBufferReceiveFromISR(
        instance->stream, &level_duration, sizeof(LevelDuration), &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    if(ret == sizeof(LevelDuration)) {
        return level_duration;
    } else {
        return level_duration_reset();
    }
}

/** Worker thread
 * 
 * @param context 
 * @return exit code 
 */
static int32_t subghz_file_encoder_worker_thread(void* context) {
    SubGhzFileEncoderWorker* instance = context;

    bool res = false;
    do {
        if(!file_worker_open(
               instance->file_worker,
               string_get_cstr(instance->file_path),
               FSAM_READ,
               FSOM_OPEN_EXISTING)) {
            break;
        }
        //todo skips 3 lines file header
        if(!file_worker_read_until(instance->file_worker, instance->str_data, '\n')) {
            // break;
        }
        if(!file_worker_read_until(instance->file_worker, instance->str_data, '\n')) {
            // break;
        }
        if(!file_worker_read_until(instance->file_worker, instance->str_data, '\n')) {
            // break;
        }
        res = true;
    } while(0);

    while(res && instance->worker_running) {
        size_t stream_free_byte = xStreamBufferSpacesAvailable(instance->stream);
        if((stream_free_byte / sizeof(LevelDuration)) > SUBGHZ_FILE_ENCODER_LOAD) {
            if(file_worker_read_until(instance->file_worker, instance->str_data, '\n')) {
                if(!subghz_file_encoder_worker_data_parse(
                       instance,
                       string_get_cstr(instance->str_data),
                       strlen(string_get_cstr(instance->str_data)))) {
                    subghz_file_encoder_worker_add_livel_duration(instance, 0);
                    break;
                }
            } else {
                subghz_file_encoder_worker_add_livel_duration(instance, 0);
                break;
            }
        }
    }
    //waiting for the end of the transfer
    while(instance->worker_running) {
        osDelay(50);
    }
    file_worker_close(instance->file_worker);
    return 0;
}

SubGhzFileEncoderWorker* subghz_file_encoder_worker_alloc() {
    SubGhzFileEncoderWorker* instance = furi_alloc(sizeof(SubGhzFileEncoderWorker));

    instance->thread = furi_thread_alloc();
    furi_thread_set_name(instance->thread, "subghz_file_encoder_worker");
    furi_thread_set_stack_size(instance->thread, 2048);
    furi_thread_set_context(instance->thread, instance);
    furi_thread_set_callback(instance->thread, subghz_file_encoder_worker_thread);
    instance->stream = xStreamBufferCreate(sizeof(LevelDuration) * 1024, sizeof(LevelDuration));

    instance->file_worker = file_worker_alloc(false);
    string_init(instance->str_data);
    string_init(instance->file_path);
    instance->level = false;

    return instance;
}

void subghz_file_encoder_worker_free(SubGhzFileEncoderWorker* instance) {
    furi_assert(instance);

    vStreamBufferDelete(instance->stream);
    furi_thread_free(instance->thread);

    string_clear(instance->str_data);
    string_clear(instance->file_path);
    file_worker_free(instance->file_worker);

    free(instance);
}

bool subghz_file_encoder_worker_start(SubGhzFileEncoderWorker* instance, const char* file_path) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    xStreamBufferReset(instance->stream);
    string_set(instance->file_path, file_path);
    instance->worker_running = true;
    furi_thread_start(instance->thread);
    return true;
}

void subghz_file_encoder_worker_stop(SubGhzFileEncoderWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    instance->worker_running = false;
    furi_thread_join(instance->thread);
}

bool subghz_file_encoder_worker_is_running(SubGhzFileEncoderWorker* instance) {
    furi_assert(instance);
    return instance->worker_running;
}
