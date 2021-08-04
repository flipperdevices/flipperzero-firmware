#include "../subghz_i.h"

void subghz_scene_read_callback(DialogExResult result, void* context) {
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, result); //передаем в евент действия на экране
}

const void subghz_scene_read_on_enter(void* context) {
    SubGhz* subghz = context;

    // Setup view
    DialogEx* dialog_ex = subghz->dialog_ex;

    dialog_ex_set_header(dialog_ex, "SubGhz 433.92", 36, 6, AlignLeft, AlignCenter); //рисуем заголовок
    dialog_ex_set_icon(dialog_ex, 10, 12, &I_RFIDDolphinReceive_97x61); //присуем иконку
    
    dialog_ex_set_context(dialog_ex, subghz); //регистрируем объект
    dialog_ex_set_result_callback(dialog_ex, subghz_scene_read_callback); //добавляем колбек
    
    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewDialogEx); //отрисовываем экран

}

const bool subghz_scene_read_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;

    // if(event.type == SceneManagerEventTypeCustom) {
    //     if(event.event == DialogExResultLeft) {
    //         return scene_manager_previous_scene(subghz->scene_manager); //если тыкнули в право выходим в основное меню
    //     } else if(event.event == DialogExResultRight) {
    //         //scene_manager_next_scene(subghz->scene_manager, NfcSceneCardMenu); //добавить сохранение
    //         return true;
    //     }
    // }
    // return false;


    return false;
}

const void subghz_scene_read_on_exit(void* context) {
    SubGhz* subghz = context;
    //удаляем ресурсы которые заюзали
    DialogEx* dialog_ex = subghz->dialog_ex;
    dialog_ex_set_header(dialog_ex, NULL, 0, 0, AlignCenter, AlignCenter);
    //dialog_ex_set_text(dialog_ex, NULL, 0, 0, AlignCenter, AlignTop);
    dialog_ex_set_icon(dialog_ex, 0, 0, NULL);
    dialog_ex_set_left_button_text(dialog_ex, NULL);
    dialog_ex_set_right_button_text(dialog_ex, NULL);
    dialog_ex_set_result_callback(dialog_ex, NULL);
    dialog_ex_set_context(dialog_ex, NULL);
}
