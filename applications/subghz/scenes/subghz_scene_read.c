#include "../subghz_i.h"

#define GUBGHZ_READ_CUSTOM_EVENT (10UL)

void subghz_read_protocol_callback(SubGhzProtocolCommon* parser, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    subghz->protocol_result = parser;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, GUBGHZ_READ_CUSTOM_EVENT);
}
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

    //Start CC1101 rx
    subghz_begin(ApiHalSubGhzPresetOokAsync);
    subghz_rx(433920000);
    api_hal_subghz_start_async_rx(subghz_worker_rx_callback, subghz->worker);
    subghz_worker_start(subghz->worker);
    subghz_protocol_enable_dump(subghz->protocol, subghz_read_protocol_callback, subghz);

    //subghz_protocol_load_keeloq_file(subghz->protocol, "/ext/assets/subghz/keeloq_mfcodes");
    //subghz_protocol_load_nice_flor_s_file(subghz->protocol, "/ext/assets/subghz/nice_floor_s_rx");

    //dialog_ex_set_context(dialog_ex, subghz); //регистрируем объект
    //dialog_ex_set_result_callback(dialog_ex, subghz_scene_read_callback); //добавляем колбек
    
    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewDialogEx); //отрисовываем экран

}

const bool subghz_scene_read_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GUBGHZ_READ_CUSTOM_EVENT) {
            scene_manager_next_scene(subghz->scene_manager, SubGhzViewReceiver); //добавить сохранение
            return true;
        }
    }
    return false;
}

const void subghz_scene_read_on_exit(void* context) {
    SubGhz* subghz = context;

    //Stop CC1101
    subghz_worker_stop(subghz->worker);
    api_hal_subghz_stop_async_rx();
    subghz_end();

    //удаляем ресурсы которые заюзали
    DialogEx* dialog_ex = subghz->dialog_ex;
    dialog_ex_set_header(dialog_ex, NULL, 0, 0, AlignCenter, AlignCenter);
    //dialog_ex_set_text(dialog_ex, NULL, 0, 0, AlignCenter, AlignTop);
    dialog_ex_set_icon(dialog_ex, 0, 0, NULL);
    //dialog_ex_set_left_button_text(dialog_ex, NULL);
    //dialog_ex_set_right_button_text(dialog_ex, NULL);
    //dialog_ex_set_result_callback(dialog_ex, NULL);
    //dialog_ex_set_context(dialog_ex, NULL);
}
