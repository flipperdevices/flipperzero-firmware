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
    //Имя датчка
    char sensor_name[11];
    snprintf(sensor_name, 11, "Sensor%d", app->sensors_count + 1);
    const SensorType* st = unitemp_getSensorsTypes()[index];
    uint8_t anotherValues[1] = {0};
    //Выбор первого доступного порта для датчиков single wire и one wire
    if(st->interface == &SINGLE_WIRE || st->interface == &ONE_WIRE) {
        anotherValues[0] = unitemp_GPIO_toInt(unitemp_gpio_getAviablePort(st->interface, 0)->pin);
    }
    //Для I2C адрес выберится автоматически

    unitemp_SensorEdit_switch(unitemp_sensor_alloc(sensor_name, st, anotherValues));
}

/**
 * @brief Создание меню редактирования настроек
 */
void unitemp_SensorsList_alloc(void) {
    variable_item_list = variable_item_list_alloc();
    //Сброс всех элементов меню
    variable_item_list_reset(variable_item_list);

    //Добавление в список доступных датчиков
    for(uint8_t i = 0; i < unitemp_getSensorsTypesCount(); i++) {
        variable_item_list_add(
            variable_item_list, unitemp_getSensorsTypes()[i]->typename, 1, NULL, app);
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