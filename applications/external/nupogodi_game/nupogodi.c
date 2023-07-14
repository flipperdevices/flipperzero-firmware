#include <furi.h>
#include <furi_hal_random.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>
#include <storage/storage.h>
#include <stdlib.h>
#include <power/power_service/power.h>
#include "nupogodi_icons.h"
#include "notifications.h"

#define TAG "NuPogodi"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    View* view;
    FuriThread* worker_thread;
    FuriTimer* timer;
} NuPogodiApp;

typedef enum {
    Logo,
    Ready,
    Play,
    Pause,
    Fail,
    Over,
} NuPogodiMode;

typedef struct {
    bool sound;
    NuPogodiMode mode;
    uint8_t tick;
    bool top;
    bool left;
    uint8_t missed;
    uint16_t scores;
    uint8_t eggs[4];
} NuPogodiModel;

typedef enum {
    WorkerEventReserved = (1 << 0),
    WorkerEventStop = (1 << 1),
    WorkerEventTick = (1 << 2),
} WorkerEventFlags;

#define WORKER_EVENTS_MASK (WorkerEventStop | WorkerEventTick)

static void nupogodi_view_draw_wolf(Canvas* canvas, NuPogodiModel* model) {
    if(model->mode == Play) {
        if(model->left) {
            if(model->top) {
                canvas_draw_icon(canvas, 41, 16, &I_WolfLeftUp);
            } else {
                canvas_draw_icon(canvas, 41, 16, &I_WolfLeftDown);
            }
        } else {
            if(model->top) {
                canvas_draw_icon(canvas, 41, 16, &I_WolfRightUp);
            } else {
                canvas_draw_icon(canvas, 41, 16, &I_WolfRightDown);
            }
        }
    } else if(model->mode == Fail) {
        if(model->left) {
            canvas_draw_icon(canvas, 46, 0, &I_WolfLeftFail);
        } else {
            canvas_draw_icon(canvas, 36, 0, &I_WolfRightFail);
        }
    }
}
static void nupogodi_view_draw_eggs(Canvas* canvas, NuPogodiModel* model) {
    // Top Right
    if(model->eggs[0] == 1) {
        canvas_draw_icon(canvas, 108, 12, &I_Egg1);
    } else if(model->eggs[0] == 2) {
        canvas_draw_icon(canvas, 103, 13, &I_Egg2);
    } else if(model->eggs[0] == 3) {
        canvas_draw_icon(canvas, 98, 14, &I_Egg3);
    } else if(model->eggs[0] == 4) {
        canvas_draw_icon(canvas, 93, 16, &I_Egg4);
    } else if(model->eggs[0] == 5) {
        canvas_draw_icon(canvas, 88, 17, &I_Egg5);
    }

    // Bottom Right
    if(model->eggs[1] == 1) {
        canvas_draw_icon_ex(canvas, 108, 36, &I_Egg1, IconRotation90);
    } else if(model->eggs[1] == 2) {
        canvas_draw_icon_ex(canvas, 103, 37, &I_Egg2, IconRotation90);
    } else if(model->eggs[1] == 3) {
        canvas_draw_icon_ex(canvas, 98, 38, &I_Egg3, IconRotation90);
    } else if(model->eggs[1] == 4) {
        canvas_draw_icon_ex(canvas, 93, 39, &I_Egg4, IconRotation90);
    } else if(model->eggs[1] == 5) {
        canvas_draw_icon_ex(canvas, 88, 41, &I_Egg5, IconRotation90);
    }

    // Top Left
    if(model->eggs[2] == 1) {
        canvas_draw_icon_ex(canvas, 10, 13, &I_Egg1, IconRotation180);
    } else if(model->eggs[2] == 2) {
        canvas_draw_icon_ex(canvas, 15, 13, &I_Egg2, IconRotation180);
    } else if(model->eggs[2] == 3) {
        canvas_draw_icon_ex(canvas, 20, 15, &I_Egg3, IconRotation180);
    } else if(model->eggs[2] == 4) {
        canvas_draw_icon_ex(canvas, 25, 17, &I_Egg4, IconRotation180);
    } else if(model->eggs[2] == 5) {
        canvas_draw_icon_ex(canvas, 30, 18, &I_Egg5, IconRotation180);
    }

    // Bottom Left
    if(model->eggs[3] == 1) {
        canvas_draw_icon_ex(canvas, 10, 35, &I_Egg1, IconRotation270);
    } else if(model->eggs[3] == 2) {
        canvas_draw_icon_ex(canvas, 15, 38, &I_Egg2, IconRotation270);
    } else if(model->eggs[3] == 3) {
        canvas_draw_icon_ex(canvas, 20, 39, &I_Egg3, IconRotation270);
    } else if(model->eggs[3] == 4) {
        canvas_draw_icon_ex(canvas, 25, 40, &I_Egg4, IconRotation270);
    } else if(model->eggs[3] == 5) {
        canvas_draw_icon_ex(canvas, 30, 42, &I_Egg5, IconRotation270);
    }
}

static void nupogodi_view_draw_scores(Canvas* canvas, NuPogodiModel* model) {
    if(model->mode == Play) {
        if(model->missed >= 1) {
            canvas_draw_icon(canvas, 78, 0, &I_Chick);
        }
        if(model->missed >= 2) {
            canvas_draw_icon(canvas, 92, 0, &I_Chick);
        }
        if(model->missed >= 3) {
            canvas_draw_icon(canvas, 106, 0, &I_Chick);
        }

        canvas_draw_icon(canvas, 30, 0, &I_Egg3);

        FuriString* scores_str = furi_string_alloc_printf("%1d", model->scores);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 46, 10, furi_string_get_cstr(scores_str));
        furi_string_free(scores_str);
    }
}

static void nupogodi_view_draw_scene(Canvas* canvas) {
    canvas_draw_icon(canvas, 0, 0, &I_ChickenL);
    canvas_draw_icon(canvas, 0, 24, &I_ChickenL);

    canvas_draw_icon(canvas, 120, 0, &I_ChickenR);
    canvas_draw_icon(canvas, 120, 24, &I_ChickenR);

    canvas_draw_line(canvas, 0, 21, 8, 21);
    canvas_draw_line(canvas, 0, 22, 8, 22);
    canvas_draw_line(canvas, 0, 23, 8, 23);
    canvas_draw_line(canvas, 8, 21, 36, 29);
    canvas_draw_line(canvas, 8, 22, 36, 30);
    canvas_draw_line(canvas, 8, 23, 36, 31);

    canvas_draw_line(canvas, 0, 45, 10, 45);
    canvas_draw_line(canvas, 0, 46, 10, 46);
    canvas_draw_line(canvas, 0, 47, 10, 47);
    canvas_draw_line(canvas, 10, 45, 36, 53);
    canvas_draw_line(canvas, 10, 46, 36, 54);
    canvas_draw_line(canvas, 10, 47, 36, 55);

    canvas_draw_line(canvas, 120, 21, 127, 21);
    canvas_draw_line(canvas, 120, 22, 127, 22);
    canvas_draw_line(canvas, 120, 23, 127, 23);
    canvas_draw_line(canvas, 92, 29, 120, 21);
    canvas_draw_line(canvas, 92, 30, 120, 22);
    canvas_draw_line(canvas, 92, 31, 120, 23);

    canvas_draw_line(canvas, 120, 45, 127, 45);
    canvas_draw_line(canvas, 120, 46, 127, 46);
    canvas_draw_line(canvas, 120, 47, 127, 47);
    canvas_draw_line(canvas, 92, 53, 120, 45);
    canvas_draw_line(canvas, 92, 54, 120, 46);
    canvas_draw_line(canvas, 92, 55, 120, 47);
}

static void nupogodi_view_draw_logo(Canvas* canvas) {
    canvas_draw_icon(canvas, 0, 0, &I_NuPogodi);
}

static void nupogodi_view_draw_over(Canvas* canvas, NuPogodiModel* model) {
    canvas_draw_icon(canvas, 30, 0, &I_Egg3);

    FuriString* scores_str = furi_string_alloc_printf("%1d", model->scores);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 46, 10, furi_string_get_cstr(scores_str));
    furi_string_free(scores_str);

    if(model->scores >= 10) {
        canvas_draw_icon(canvas, 22, 12, &I_OverWin);
    } else {
        canvas_draw_icon(canvas, 22, 12, &I_OverLose);
    }
}

static void nupogodi_view_draw_callback(Canvas* canvas, void* _model) {
    NuPogodiModel* model = _model;
    furi_assert(model);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    switch(model->mode) {
    case Logo:
        nupogodi_view_draw_logo(canvas);
        break;
    case Play:
    case Fail:
        nupogodi_view_draw_eggs(canvas, model);
        nupogodi_view_draw_scene(canvas);
        nupogodi_view_draw_scores(canvas, model);
        nupogodi_view_draw_wolf(canvas, model);
        break;
    case Pause:
        nupogodi_view_draw_scene(canvas);
        nupogodi_view_draw_scores(canvas, model);
        break;
    case Over:
        nupogodi_view_draw_over(canvas, model);
        break;
    default:
        break;
    }
}

static bool nupogodi_view_input_callback(InputEvent* event, void* context) {
    NuPogodiApp* app = context;
    furi_assert(app);
    bool consumed = false;

    FURI_LOG_D(TAG, "Input %u %u", event->type, event->key);

    with_view_model(
        app->view,
        NuPogodiModel * model,
        {
            if(model->mode == Fail) {
                return false;
            }

            if(event->type == InputTypeShort) {
                switch(event->key) {
                case InputKeyUp:
                    consumed = true;
                    model->top = true;
                    break;
                case InputKeyDown:
                    consumed = true;
                    model->top = false;
                    break;
                case InputKeyLeft:
                    consumed = true;
                    model->left = true;
                    break;
                case InputKeyRight:
                    consumed = true;
                    model->left = false;
                    break;
                case InputKeyOk:
                    consumed = true;
                    if(model->mode == Play) {
                        model->mode = Pause;
                    } else if(model->mode == Pause) {
                        model->mode = Play;
                    } else if(model->mode == Over) {
                        model->mode = Ready;
                    }
                    break;
                default:
                    break;
                }
            } else if(event->type == InputTypeLong) {
                switch(event->key) {
                case InputKeyOk:
                    consumed = true;
                    model->sound = !model->sound;
                    break;
                default:
                    break;
                }
            }
        },
        consumed);

    return consumed;
}

static uint32_t nupogodi_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;

    //return VIEW_IGNORE;
}

static int32_t nupogodi_worker(void* context) {
    furi_assert(context);
    NuPogodiApp* app = context;
    UNUSED(app);

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        FURI_LOG_D(TAG, "Worker %lu", events);

        if(events & WorkerEventStop) break;
        if(events & WorkerEventTick) {
            with_view_model(
                app->view,
                NuPogodiModel * model,
                {
                    uint8_t sound = model->sound ? 0 : 1;
                    switch(model->mode) {
                    case Logo:
                        if(model->tick > 0) {
                            model->tick--;
                        } else {
                            model->mode = Play;
                        }
                        break;
                    case Ready:
                        model->eggs[0] = 0;
                        model->eggs[1] = 0;
                        model->eggs[2] = 0;
                        model->eggs[3] = 0;
                        model->scores = 0;
                        model->missed = 0;
                        model->mode = Play;
                        break;
                    case Play:
                        if((model->eggs[0] == 0) && (model->eggs[1] == 0) &&
                           (model->eggs[2] == 0) && (model->eggs[3] == 0)) {
                            // Если ни одного яйца нет - создаем
                            uint32_t rnd = furi_hal_random_get() % 4;
                            model->eggs[rnd] = 1;
                            notification_message(app->notification, notification_eggs[rnd][sound]);
                        } else {
                            // Прокатываем все яйца на одно деление вперед
                            for(uint8_t i = 0; i < 4; i++) {
                                if(model->eggs[i] > 0) {
                                    model->eggs[i]++;
                                    if(model->eggs[i] < 6) {
                                        notification_message(
                                            app->notification, notification_eggs[i][sound]);
                                    }
                                }
                            }
                            if((model->eggs[0] == 6) && (model->top) && (!model->left)) {
                                model->eggs[0] = 0;
                                model->scores++;
                                notification_message(app->notification, notification_done[sound]);
                            } else if((model->eggs[1] == 6) && (!model->top) && (!model->left)) {
                                model->eggs[1] = 0;
                                model->scores++;
                                notification_message(app->notification, notification_done[sound]);
                            } else if((model->eggs[2] == 6) && (model->top) && (model->left)) {
                                model->eggs[2] = 0;
                                model->scores++;
                                notification_message(app->notification, notification_done[sound]);
                            } else if((model->eggs[3] == 6) && (!model->top) && (model->left)) {
                                model->eggs[3] = 0;
                                model->scores++;
                                notification_message(app->notification, notification_done[sound]);
                            } else {
                                // Если яйцо было не поймано - зануляем все, запускаем анимацию разбития
                                for(uint8_t i = 0; i < 4; i++) {
                                    if(model->eggs[i] == 6) {
                                        model->eggs[0] = 0;
                                        model->eggs[1] = 0;
                                        model->eggs[2] = 0;
                                        model->eggs[3] = 0;
                                        model->left = i >= 2;
                                        model->missed++;
                                        if(model->missed < 4) {
                                            model->mode = Fail;
                                            model->tick = 3;
                                        } else {
                                            model->mode = Over;
                                        }

                                        notification_message(
                                            app->notification, notification_fail[sound]);
                                    }
                                }
                            }
                        }
                        break;
                    case Pause:
                        // nop
                        break;
                    case Fail:
                        if(model->tick > 0) {
                            model->tick--;
                        } else {
                            model->mode = Play;
                        }
                        break;
                    case Over:
                        break;
                    }
                },
                true);
        }
    }

    return 0;
}

static void nupogodi_timer_callback(void* context) {
    furi_assert(context);
    NuPogodiApp* app = context;
    furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventTick);
}

static NuPogodiApp* nupogodi_app_alloc() {
    NuPogodiApp* app = malloc(sizeof(NuPogodiApp));

    // Gui
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->view = view_alloc();
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, nupogodi_view_draw_callback);
    view_set_input_callback(app->view, nupogodi_view_input_callback);
    view_allocate_model(app->view, ViewModelTypeLocking, sizeof(NuPogodiModel));
    with_view_model(
        app->view,
        NuPogodiModel * model,
        {
            model->mode = Logo;
            model->tick = 1;
            model->top = false;
            model->left = true;
            model->missed = 0;
            model->scores = 0;
            model->eggs[0] = 0;
            model->eggs[1] = 0;
            model->eggs[2] = 0;
            model->eggs[3] = 0;
        },
        true);

    view_set_previous_callback(app->view, nupogodi_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, app->view);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    app->worker_thread = furi_thread_alloc_ex("NuPogodiWorker", 1024, nupogodi_worker, app);
    furi_thread_start(app->worker_thread);

    app->timer = furi_timer_alloc(nupogodi_timer_callback, FuriTimerTypePeriodic, app);
    furi_timer_start(app->timer, furi_ms_to_ticks(1000));

    return app;
}

static void nupogodi_app_free(NuPogodiApp* app) {
    furi_assert(app);

    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);

    furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventStop);
    furi_thread_join(app->worker_thread);
    furi_thread_free(app->worker_thread);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, 0);
    view_free(app->view);
    view_dispatcher_free(app->view_dispatcher);

    // Close gui record
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    app->notification = NULL;

    // Free rest
    free(app);
}

int32_t nupogodi_app(void* p) {
    UNUSED(p);
    NuPogodiApp* app = nupogodi_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    nupogodi_app_free(app);
    return 0;
}