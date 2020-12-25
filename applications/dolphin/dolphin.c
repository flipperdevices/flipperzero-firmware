#include "dolphin_i.h"

bool dolphin_view_idle_main_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    Dolphin* dolphin = context;

    if(event->state) {
        if(event->input == InputOk) {
            with_value_mutex(
                dolphin->menu_vm, (Menu * menu) { menu_ok(menu); });
        } else if(event->input == InputUp) {
            view_dispatcher_switch_to_view(dolphin->idle_view_dispatcher, DolphinViewIdleStats);
        } else if(event->input == InputDown) {
            view_dispatcher_switch_to_view(dolphin->idle_view_dispatcher, DolphinViewIdleDebug);
        }
    }
    // All events consumed
    return true;
}

bool dolphin_view_idle_stats_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);
    Dolphin* dolphin = context;

    if(!event->state) return false;

    if(event->input == InputLeft) {
        dolphin_deed(dolphin, DolphinDeedWrong);
    } else if(event->input == InputRight) {
        dolphin_deed(dolphin, DolphinDeedIButtonRead);
    } else if(event->input == InputOk) {
        dolphin_state_save(dolphin->state);
    } else {
        return false;
    }

    return true;
}

Dolphin* dolphin_alloc() {
    Dolphin* dolphin = furi_alloc(sizeof(Dolphin));
    // Message queue
    dolphin->event_queue = osMessageQueueNew(8, sizeof(DolphinEvent), NULL);
    furi_check(dolphin->event_queue);
    // State
    dolphin->state = dolphin_state_alloc();
    dolphin_state_load(dolphin->state);
    // Menu
    dolphin->menu_vm = furi_open("menu");
    furi_check(dolphin->menu_vm);
    // GUI
    dolphin->idle_view_dispatcher = view_dispatcher_alloc();

    dolphin->idle_view_main = view_alloc();
    view_set_context(dolphin->idle_view_main, dolphin);
    view_set_draw_callback(dolphin->idle_view_main, dolphin_view_idle_main_draw);
    view_set_input_callback(dolphin->idle_view_main, dolphin_view_idle_main_input);
    view_dispatcher_add_view(
        dolphin->idle_view_dispatcher, DolphinViewIdleMain, dolphin->idle_view_main);

    dolphin->idle_view_stats = view_alloc();
    view_set_context(dolphin->idle_view_stats, dolphin);
    view_allocate_model(
        dolphin->idle_view_stats, ViewModelTypeLockFree, sizeof(DolphinViewIdleStatsModel));
    with_view_model(
        dolphin->idle_view_stats, (DolphinViewIdleStatsModel * model) {
            model->icounter = dolphin_state_get_icounter(dolphin->state);
            model->butthurt = dolphin_state_get_butthurt(dolphin->state);
        });

    view_set_draw_callback(dolphin->idle_view_stats, dolphin_view_idle_stats_draw);
    view_set_input_callback(dolphin->idle_view_stats, dolphin_view_idle_stats_input);
    view_set_previous_callback(dolphin->idle_view_stats, dolphin_view_idle_back);
    view_dispatcher_add_view(
        dolphin->idle_view_dispatcher, DolphinViewIdleStats, dolphin->idle_view_stats);

    dolphin->idle_view_debug = view_alloc();
    view_set_draw_callback(dolphin->idle_view_debug, dolphin_view_idle_debug_draw);
    view_set_previous_callback(dolphin->idle_view_debug, dolphin_view_idle_back);
    view_dispatcher_add_view(
        dolphin->idle_view_dispatcher, DolphinViewIdleDebug, dolphin->idle_view_debug);

    dolphin->main_view_dispatcher = view_dispatcher_alloc();

    return dolphin;
}

void dolphin_deed(Dolphin* dolphin, DolphinDeed deed) {
    furi_assert(dolphin);
    DolphinEvent event;
    event.type = DolphinEventTypeDeed;
    event.deed = deed;
    furi_check(osMessageQueuePut(dolphin->event_queue, &event, 0, osWaitForever) == osOK);
}

void dolphin_task() {
    Dolphin* dolphin = dolphin_alloc();

    Gui* gui = furi_open("gui");
    view_dispatcher_attach_to_gui(dolphin->idle_view_dispatcher, gui, ViewDispatcherTypeWindow);
    view_dispatcher_switch_to_view(dolphin->idle_view_dispatcher, DolphinViewIdleMain);
    view_dispatcher_attach_to_gui(dolphin->main_view_dispatcher, gui, ViewDispatcherTypeNone);

    if(!furi_create("dolphin", dolphin)) {
        printf("[dolphin_task] cannot create the dolphin record\n");
        furiac_exit(NULL);
    }

    furiac_ready();

    DolphinEvent event;
    while(1) {
        furi_check(osMessageQueueGet(dolphin->event_queue, &event, NULL, osWaitForever) == osOK);
        if(event.type == DolphinEventTypeDeed) {
            dolphin_state_on_deed(dolphin->state, event.deed);
            with_view_model(
                dolphin->idle_view_stats, (DolphinViewIdleStatsModel * model) {
                    model->icounter = dolphin_state_get_icounter(dolphin->state);
                    model->butthurt = dolphin_state_get_butthurt(dolphin->state);
                });
        }
    }
}
