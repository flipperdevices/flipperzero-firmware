#include "tullave_worker_i.h"
#include "../drv/tullave_drv/tullave_drv.h"

#define LOG_TAG "TuLlaveCOWorker"
#define THREAD_NAME LOG_TAG
#define THREAD_STACK_SIZE 4 * 1024
#define WORKER_DELAY_MS 300

static void tullave_worker_change_state(TuLlaveWorker* worker, TuLlaveWorkerState state) {
    furi_assert(worker);
    worker->state = state;
}

TuLlaveWorker* tullave_worker_alloc() {
    FURI_LOG_D(LOG_TAG, "Starting allocation for TuLlaveWorker");

    TuLlaveWorker* t_worker = malloc(sizeof(TuLlaveWorker));

    t_worker->thread =
        furi_thread_alloc_ex(THREAD_NAME, THREAD_STACK_SIZE, tullave_worker_task, t_worker);

    t_worker->card_info = malloc(sizeof(TuLlaveInfo));
    t_worker->callback = NULL;
    t_worker->context = NULL;

    tullave_worker_change_state(t_worker, TuLlaveWorkerStateReady);

    FURI_LOG_D(LOG_TAG, "End allocation");
    return t_worker;
}

void tullave_worker_free(TuLlaveWorker* t_worker) {
    furi_assert(t_worker);

    free(t_worker->card_info);
    furi_thread_free(t_worker->thread);
    free(t_worker);
}

void tullave_worker_stop(TuLlaveWorker* t_worker) {
    furi_assert(t_worker);

    tullave_drv_exit_sleep();
    tullave_worker_change_state(t_worker, TuLlaveWorkerStop);
    furi_thread_join(t_worker->thread);
}

void tullave_worker_start(TuLlaveWorker* t_worker, TuLlaveWorkerCallback callback, void* context) {
    furi_assert(t_worker);
    furi_assert(context);

    FURI_LOG_D(LOG_TAG, "Starting TuLlaveWorker");

    t_worker->callback = callback;
    t_worker->context = context;

    tullave_drv_start_sleep();
    tullave_worker_change_state(t_worker, TuLlaveWorkerStateCheck);
    furi_thread_start(t_worker->thread);

    FURI_LOG_D(LOG_TAG, "TuLlaveWorker Thread Started");
}

int32_t tullave_worker_task(void* context) {
    TuLlaveWorker* t_worker = context;
    if(t_worker->state == TuLlaveWorkerStateCheck) {
        tullave_worker_check(t_worker);
    }
    return 0;
}

void tullave_worker_check(TuLlaveWorker* t_worker) {
    while(t_worker->state == TuLlaveWorkerStateCheck) {
        if(tullave_drv_req_card_info(t_worker->card_info)) {
            t_worker->callback(TuLlaveWorkerEventCardDetected, t_worker->context);
            break;
        } else {
            t_worker->callback(TuLLaveWorkerEventNoCardDetected, t_worker->context);
        }
        furi_delay_ms(WORKER_DELAY_MS);
    }
}