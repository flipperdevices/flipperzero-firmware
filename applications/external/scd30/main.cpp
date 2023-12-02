#include <stdarg.h>
#include <stdlib.h>

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

#include <memory>

#define LOG_LEVEL 5

#include "scd30_logging.h"
#include "err.h"
#include "main.h"
#include "scd30.h"

constexpr static int FPS = 2;

std::unique_ptr<State> state;

static void input_cb(InputEvent* event, FuriMessageQueue* queue) {
    furi_assert(event);
    furi_assert(queue);

    EventMessage message{EventID::Key, *event};
    furi_message_queue_put(queue, &message, FuriWaitForever);
}

static void draw_cb(Canvas* canvas, void* ctx) {
    auto* mutex = reinterpret_cast<FuriMutex*>(ctx);

    if(furi_mutex_acquire(mutex, 0) != FuriStatusOk) {
        ERROR(errs[(ERR_MUTEX_BLOCK)]);
        goto bail;
    }

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);

    if(state->hasRead) {
        if(state->lastReadResult) {
            char buffer[46];
            auto& scd30 = *state->scd30;
            int row = 0;
            constexpr uint8_t x = 70;
            auto print = [&](const char* label, float value) {
                snprintf(buffer, sizeof(buffer), "%d.%.3d", int(value), int((value - int(value)) * 1'000));
				canvas_set_font(canvas, FontPrimary);
				canvas_draw_str_aligned(canvas, 0, row   * 10, AlignLeft, AlignTop, label);
				canvas_set_font(canvas, FontSecondary);
				canvas_draw_str_aligned(canvas, x, row++ * 10 + 1, AlignLeft, AlignTop, buffer);
            };

            print("CO2", scd30.carbonDioxide);
            print("Temperature", scd30.temperature);
            print("Rel Humidity", scd30.relativeHumidity);
        } else {
            canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Reading failed.");
        }
    } else {
        canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Waiting for reading...");
    }

    goto release;

bail:
    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Couldn't acquire state.");

release:
    if(furi_mutex_release(mutex) != FuriStatusOk) {
        canvas_clear(canvas);
        canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Couldn't release mutex.");
    }
}

static void timer_cb(FuriMessageQueue* queue) {
    furi_assert(queue);

    EventMessage message{EventID::Tick};
    furi_message_queue_put(queue, &message, 0);
}

extern "C" int32_t scd30_main() {
    err_t error = static_cast<err_t>(0);
    Gui* gui = nullptr;
    ViewPort* viewport = nullptr;
    FuriMutex* mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    FuriMessageQueue* queue = nullptr;
    constexpr uint32_t queue_size = 8;
    EventMessage message;
    FuriStatus status = FuriStatusOk;

    if(!(queue = furi_message_queue_alloc(queue_size, sizeof(message)))) {
        ERROR(errs[(error = ERR_MALLOC_QUEUE)]);
        goto bail;
    }

    if(!(gui = reinterpret_cast<Gui*>(furi_record_open(RECORD_GUI)))) {
        ERROR(errs[(error = ERR_NO_GUI)]);
        goto bail;
    }

    if(!(state = std::make_unique<State>())) {
        ERROR(errs[(error = ERR_MALLOC_STATE)]);
        goto bail;
    }

    if(mutex == nullptr) {
        ERROR(errs[(error = ERR_NO_MUTEX)]);
        goto bail;
    }

    if(!(viewport = view_port_alloc())) {
        ERROR(errs[(error = ERR_MALLOC_VIEW)]);
        goto bail;
    }

    if(!state->init(queue)) {
        ERROR(errs[(error = ERR_INIT_STATE)]);
    }

    if(!state->scd30 || !state->scd30->begin()) {
        ERROR("Couldn't begin SCD30");
    }

    view_port_input_callback_set(viewport, input_cb, queue);
    view_port_draw_callback_set(viewport, draw_cb, &mutex);
    gui_add_view_port(gui, viewport, GuiLayerFullscreen);

    if(!(state->timer = furi_timer_alloc(timer_cb, FuriTimerTypePeriodic, queue))) {
        ERROR(errs[(error = ERR_NO_TIMER)]);
        goto bail;
    }

    state->timerFrequency = furi_kernel_get_tick_frequency();

    if((status = furi_timer_start(state->timer, state->timerFrequency / FPS)) != FuriStatusOk) {
        ERROR("Couldn't start timer (%d)", status);
        goto bail;
    }

    INFO("Initialized");

    for(bool run = true; run;) {
        FuriStatus status = FuriStatusErrorTimeout;
        while((status = furi_message_queue_get(queue, &message, 60000)) == FuriStatusErrorTimeout)
            ;

        if(status != FuriStatusOk) {
            switch(status) {
            case FuriStatusErrorTimeout:
                DEBUG(errs[DEBUG_QUEUE_TIMEOUT]);
                break;
            case FuriStatusError:
                ERROR(errs[(error = ERR_QUEUE_RTOS)]);
                goto bail;
            case FuriStatusErrorResource:
                ERROR(errs[(error = ERR_QUEUE_RESOURCE)]);
                goto bail;
            case FuriStatusErrorParameter:
                ERROR(errs[(error = ERR_QUEUE_BADPRM)]);
                goto bail;
            case FuriStatusErrorNoMemory:
                ERROR(errs[(error = ERR_QUEUE_NOMEM)]);
                goto bail;
            case FuriStatusErrorISR:
                ERROR(errs[(error = ERR_QUEUE_ISR)]);
                goto bail;
            default:
                ERROR(errs[(error = ERR_QUEUE_UNK)]);
                goto bail;
            }
        } else {
            if(!furi_mutex_acquire(mutex, 0)) {
                ERROR(errs[(error = ERR_MUTEX_BLOCK)]);
                goto bail;
            }

            if(message.id == EventID::Tick) {
                state->readData();
            } else if(message.id == EventID::Key) {
                if(message.event.type == InputTypeRelease && message.event.key == InputKeyBack)
                    run = false;
            } else
                WARN("Unknown message ID [%d]", static_cast<int>(message.id));

            view_port_update(viewport);

            if(!furi_mutex_release(mutex)) {
                ERROR(errs[(error = ERR_MUTEX_RELEASE)]);
                goto bail;
            }
        }
    }

bail:
    if(state->timer) {
        furi_timer_stop(state->timer);
        furi_timer_free(state->timer);
        state->timer = nullptr;
    }

    gui_remove_view_port(gui, viewport);

    if(viewport) {
        view_port_enabled_set(viewport, false);
        view_port_free(viewport);
        viewport = nullptr;
    }

    state.reset();

    furi_record_close(RECORD_GUI);
    furi_mutex_free(mutex);

    if(queue) {
        furi_message_queue_free(queue);
        queue = nullptr;
    }

    return 0;
}

bool State::init(FuriMessageQueue* queue) {
    furi_assert(this);

    if(!(timer = furi_timer_alloc(timer_cb, FuriTimerTypePeriodic, queue))) {
        ERROR(errs[ERR_NO_TIMER]);
        return false;
    }

    if(!(scd30 = std::make_unique<SCD30>())) {
        ERROR(errs[ERR_NO_SCD30]);
        return false;
    }

    return true;
}

bool State::readData() {
    hasRead = true;

    if(scd30 == nullptr) {
        ERROR("scd30 is null");
        return false;
    }

    if(scd30->dataReady()) {
        lastReadResult = (scd30->beginIfNecessary() && scd30->readData());

        if(!lastReadResult) scd30->begun = false;
    }

    return lastReadResult;
}
