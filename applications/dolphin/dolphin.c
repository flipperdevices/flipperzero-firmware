#include "dolphin_i.h"
#include <stdlib.h>
#include "applications.h"

static void lock_icon_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    Dolphin* dolphin = context;
    canvas_draw_icon_animation(canvas, 0, 0, dolphin->lock_icon);
}

// void dolphin_save(Dolphin* dolphin) {
//     furi_assert(dolphin);
//     DolphinEvent event;
//     event.type = DolphinEventTypeSave;
//     furi_check(osMessageQueuePut(dolphin->event_queue, &event, 0, osWaitForever) == osOK);
// }

// void dolphin_deed(Dolphin* dolphin, DolphinDeed deed) {
//     furi_assert(dolphin);
//     DolphinEvent event;
//     event.type = DolphinEventTypeDeed;
//     event.deed = deed;
//     furi_check(osMessageQueuePut(dolphin->event_queue, &event, 0, osWaitForever) == osOK);
// }

// int32_t dolphin_srv() {
//     Dolphin* dolphin = dolphin_alloc();

//     if(dolphin_state_load(dolphin->state)) {
//         view_dispatcher_switch_to_view(dolphin->view_dispatcher, DolphinViewIdleMain);
//     } else {
//         view_dispatcher_switch_to_view(dolphin->view_dispatcher, DolphinViewFirstStart);
//     }

//     with_view_model(
//         dolphin->idle_view_dolphin_stats, (DolphinViewStatsModel * model) {
//             model->icounter = dolphin_state_get_icounter(dolphin->state);
//             model->butthurt = dolphin_state_get_butthurt(dolphin->state);
//             return true;
//         });

//     furi_record_create("dolphin", dolphin);

//     if(!furi_hal_version_do_i_belong_here()) {
//         view_dispatcher_switch_to_view(dolphin->view_dispatcher, DolphinViewHwMismatch);
//     }

//     DolphinEvent event;
//     while(1) {
//         furi_check(osMessageQueueGet(dolphin->event_queue, &event, NULL, osWaitForever) == osOK);

//         DolphinViewLockMenuModel* lock_model = view_get_model(dolphin->view_lockmenu);

//         if(lock_model->locked && lock_model->exit_timeout == 0 &&
//            osTimerIsRunning(dolphin->timeout_timer)) {
//             osTimerStop(dolphin->timeout_timer);
//             osDelay(1); // smol enterprise delay
//             view_dispatcher_switch_to_view(dolphin->view_dispatcher, DolphinViewIdleMain);
//         }

//         if(event.type == DolphinEventTypeTick) {
//             view_commit_model(dolphin->view_lockmenu, true);

//         } else if(event.type == DolphinEventTypeDeed) {
//             dolphin_state_on_deed(dolphin->state, event.deed);
//             with_view_model(
//                 dolphin->idle_view_dolphin_stats, (DolphinViewStatsModel * model) {
//                     model->icounter = dolphin_state_get_icounter(dolphin->state);
//                     model->butthurt = dolphin_state_get_butthurt(dolphin->state);
//                     return true;
//                 });
//         } else if(event.type == DolphinEventTypeSave) {
//             dolphin_state_save(dolphin->state);
//         }
//     }
//     dolphin_free(dolphin);
//     return 0;
// }

bool dolphin_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Dolphin* dolphin = (Dolphin*)context;
    return scene_manager_handle_custom_event(dolphin->scene_manager, event);
}

bool dolphin_back_event_callback(void* context) {
    furi_assert(context);
    Dolphin* dolphin = (Dolphin*)context;
    return scene_manager_handle_back_event(dolphin->scene_manager);
}

Dolphin* dolphin_alloc() {
    Dolphin* dolphin = furi_alloc(sizeof(Dolphin));

    dolphin->state = dolphin_state_alloc();
    // Menu
    dolphin->menu_vm = furi_record_open("menu");

    dolphin->gui = furi_record_open("gui");
    // Scene thread
    dolphin->scene_thread = furi_thread_alloc();

    dolphin->view_dispatcher = view_dispatcher_alloc();
    dolphin->scene_manager = scene_manager_alloc(&dolphin_scene_handlers, dolphin);

    view_dispatcher_enable_queue(dolphin->view_dispatcher);
    view_dispatcher_attach_to_gui(
        dolphin->view_dispatcher, dolphin->gui, ViewDispatcherTypeWindow);

    view_dispatcher_set_event_callback_context(dolphin->view_dispatcher, dolphin);
    view_dispatcher_set_custom_event_callback(
        dolphin->view_dispatcher, dolphin_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        dolphin->view_dispatcher, dolphin_back_event_callback);

    dolphin->main_view = dolphin_main_view_alloc();
    dolphin->lock_menu = dolphin_lock_menu_alloc();
    dolphin->locked_view = dolphin_locked_view_alloc();
    dolphin->debug_view = dolphin_debug_view_alloc();
    dolphin->first_start_view = dolphin_first_start_view_alloc();
    dolphin->hw_mismatch_view = dolphin_hw_mismatch_view_alloc();

    view_dispatcher_add_view(
        dolphin->view_dispatcher, DolphinViewMain, dolphin_main_get_view(dolphin->main_view));
    view_dispatcher_add_view(
        dolphin->view_dispatcher,
        DolphinViewLockMenu,
        dolphin_lock_menu_get_view(dolphin->lock_menu));
    view_dispatcher_add_view(
        dolphin->view_dispatcher, DolphinViewDebug, dolphin_debug_get_view(dolphin->debug_view));
    view_dispatcher_add_view(
        dolphin->view_dispatcher,
        DolphinViewLocked,
        dolphin_locked_get_view(dolphin->locked_view));
    view_dispatcher_add_view(
        dolphin->view_dispatcher,
        DolphinViewFirstStart,
        dolphin_first_start_get_view(dolphin->first_start_view));
    view_dispatcher_add_view(
        dolphin->view_dispatcher,
        DolphinViewHwMismatch,
        dolphin_hw_mismatch_get_view(dolphin->hw_mismatch_view));

    // Lock icon
    dolphin->lock_icon = icon_animation_alloc(&I_Lock_8x8);
    dolphin->lock_viewport = view_port_alloc();
    view_port_set_width(dolphin->lock_viewport, icon_animation_get_width(dolphin->lock_icon));
    view_port_draw_callback_set(dolphin->lock_viewport, lock_icon_callback, dolphin);
    view_port_enabled_set(dolphin->lock_viewport, false);
    gui_add_view_port(dolphin->gui, dolphin->lock_viewport, GuiLayerStatusBarLeft);

    return dolphin;
}

void dolphin_free(Dolphin* dolphin) {
    furi_assert(dolphin);

    view_dispatcher_remove_view(dolphin->view_dispatcher, DolphinViewMain);
    view_dispatcher_remove_view(dolphin->view_dispatcher, DolphinViewLockMenu);
    view_dispatcher_remove_view(dolphin->view_dispatcher, DolphinViewLocked);
    view_dispatcher_remove_view(dolphin->view_dispatcher, DolphinViewDebug);
    view_dispatcher_remove_view(dolphin->view_dispatcher, DolphinViewFirstStart);
    view_dispatcher_remove_view(dolphin->view_dispatcher, DolphinViewHwMismatch);

    view_dispatcher_free(dolphin->view_dispatcher);
    scene_manager_free(dolphin->scene_manager);

    dolphin_main_view_free(dolphin->main_view);
    dolphin_lock_menu_free(dolphin->lock_menu);
    dolphin_locked_view_free(dolphin->locked_view);
    dolphin_debug_view_free(dolphin->debug_view);
    dolphin_first_start_view_free(dolphin->first_start_view);
    dolphin_hw_mismatch_view_free(dolphin->hw_mismatch_view);

    furi_record_close("gui");
    dolphin->gui = NULL;

    furi_thread_free(dolphin->scene_thread);

    furi_record_close("menu");
    dolphin->menu_vm = NULL;

    dolphin_state_free(dolphin->state);

    free(dolphin);
}

int32_t dolphin_srv(void* p) {
    Dolphin* dolphin = dolphin_alloc();

    if(dolphin_state_load(dolphin->state)) {
        scene_manager_next_scene(dolphin->scene_manager, DolphinSceneMain);
    } else {
        scene_manager_next_scene(dolphin->scene_manager, DolphinSceneFirstStart);
    }

    furi_record_create("dolphin", dolphin);

    if(!furi_hal_version_do_i_belong_here()) {
        scene_manager_next_scene(dolphin->scene_manager, DolphinSceneHwMismatch);
    }

    view_dispatcher_run(dolphin->view_dispatcher);
    dolphin_free(dolphin);

    return 0;
}
