#include "UnitempViews.h"
#include <gui/modules/variable_item_list.h>
#include <stdio.h>

//Текущий вид
static View* view;
//Список
static VariableItemList* variable_item_list;

#define VIEW_ID SENSORSLIST_VIEW

/**
 * @brief Функция обработки нажатия кнопки "Назад"
 *
 * @param context Указатель на данные приложения
 * @return ID вида в который нужно переключиться
 */
static uint32_t _exit_callback(void* context) {
    UNUSED(context);

    //Возврат предыдущий вид
    return MAINMENU_VIEW;
}
/**
 * @brief Функция обработки нажатия средней кнопки
 *
 * @param context Указатель на данные приложения
 * @param index На каком элементе списка была нажата кнопка
 */
static void _enter_callback(void* context, uint32_t index) {
    UNUSED(context);
    const SensorType* type = unitemp_sensors_getTypes()[index];
    //Имя датчка
    char sensor_name[11];
    snprintf(sensor_name, 11, "%s", type->typename);

    char args[22] = {0};
    //Выбор первого доступного порта для датчика single wire
    if(type->interface == &SINGLE_WIRE) {
        snprintf(
            args, 4, "%d", unitemp_gpio_toInt(unitemp_gpio_getAviablePort(type->interface, 0)));
    }
    //Выбор первого доступного порта для датчика one wire и запись нулевого ID
    if(type->interface == &ONE_WIRE) {
        snprintf(
            args,
            21,
            "%d %02X%02X%02X%02X%02X%02X%02X%02X",
            unitemp_gpio_toInt(unitemp_gpio_getAviablePort(type->interface, 0)),
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0);
    }
    //Для I2C адрес выберется автоматически

    unitemp_SensorEdit_switch(unitemp_sensor_alloc(sensor_name, type, args));
}

/**
 * @brief Создание меню редактирования настроек
 */
void unitemp_SensorsList_alloc(void) {
    variable_item_list = variable_item_list_alloc();
    //Сброс всех элементов меню
    variable_item_list_reset(variable_item_list);

    //Добавление в список доступных датчиков
    for(uint8_t i = 0; i < unitemp_sensors_getTypesCount(); i++) {
        variable_item_list_add(
            variable_item_list, unitemp_sensors_getTypes()[i]->typename, 1, NULL, app);
    }

    //Добавление колбека на нажатие средней кнопки
    variable_item_list_set_enter_callback(variable_item_list, _enter_callback, app);

    //Создание вида из списка
    view = variable_item_list_get_view(variable_item_list);
    //Добавление колбека на нажатие кнопки "Назад"
    view_set_previous_callback(view, _exit_callback);
    //Добавление вида в диспетчер
    view_dispatcher_add_view(app->view_dispatcher, VIEW_ID, view);
}

void unitemp_SensorsList_switch(void) {
    //Обнуление последнего выбранного пункта
    variable_item_list_set_selected_item(variable_item_list, 0);

    view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_ID);
}

void unitemp_SensorsList_free(void) {
    //Очистка списка элементов
    variable_item_list_free(variable_item_list);
    //Очистка вида
    view_free(view);
    //Удаление вида после обработки
    view_dispatcher_remove_view(app->view_dispatcher, VIEW_ID);
}