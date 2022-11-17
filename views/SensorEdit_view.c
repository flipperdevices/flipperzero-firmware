#include "UnitempViews.h"
#include <gui/modules/variable_item_list.h>

#include "../interfaces/SingleWireSensor.h"
#include "../interfaces/OneWireSensor.h"
#include "../interfaces/I2CSensor.h"

//Текущий вид
static View* view;
//Список
static VariableItemList* variable_item_list;
//Текущий редактируемый датчик
Sensor* editable_sensor;

//Элемент списка - имя датчика
VariableItem* sensor_name_item;
#define VIEW_ID SENSOREDIT_VIEW

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
    //Смена имени
    if(index == 0) {
        //
    }
    //Сохранение
    if(index == 3) {
        app->sensors[app->sensors_count++] = editable_sensor;
        unitemp_sensors_save();
        unitemp_sensors_reload();
        unitemp_Summary_switch();
    }
}

/**
 * @brief Функция обработки изменения значения GPIO
 * 
 * @param item Указатель на элемент списка
 */
static void _gpio_change_callback(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    if(editable_sensor->type->interface == &SINGLE_WIRE) {
        SingleWireSensor* instance = editable_sensor->instance;
        instance->gpio = unitemp_gpio_getAviablePort(editable_sensor->type->interface, index);
        variable_item_set_current_value_text(item, instance->gpio->name);
    }
    if(editable_sensor->type->interface == &ONE_WIRE) {
        OneWireSensor* instance = editable_sensor->instance;
        instance->bus->gpio = unitemp_gpio_getAviablePort(editable_sensor->type->interface, index);
        variable_item_set_current_value_text(item, instance->bus->gpio->name);
    }
}
/**
 * @brief Функция обработки изменения значения GPIO
 * 
 * @param item Указатель на элемент списка
 */
static void _i2caddr_change_callback(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    ((I2CSensor*)editable_sensor->instance)->currentI2CAdr =
        ((I2CSensor*)editable_sensor->instance)->minI2CAdr + index;
    char buff[5];
    snprintf(buff, 5, "0x%2X", ((I2CSensor*)editable_sensor->instance)->currentI2CAdr);
    variable_item_set_current_value_text(item, buff);
}

/**
 * @brief Создание меню редактирования датчка
 */
void unitemp_SensorEdit_alloc(void) {
    variable_item_list = variable_item_list_alloc();
    //Сброс всех элементов меню
    variable_item_list_reset(variable_item_list);

    //Добавление колбека на нажатие средней кнопки
    variable_item_list_set_enter_callback(variable_item_list, _enter_callback, app);

    //Создание вида из списка
    view = variable_item_list_get_view(variable_item_list);
    //Добавление колбека на нажатие кнопки "Назад"
    view_set_previous_callback(view, _exit_callback);
    //Добавление вида в диспетчер
    view_dispatcher_add_view(app->view_dispatcher, VIEW_ID, view);
}

void unitemp_SensorEdit_switch(Sensor* sensor) {
    editable_sensor = sensor;
    //Сброс всех элементов меню
    variable_item_list_reset(variable_item_list);
    //Обнуление последнего выбранного пункта
    variable_item_list_set_selected_item(variable_item_list, 0);
    //Имя датчика
    sensor_name_item = variable_item_list_add(variable_item_list, "Name", 1, NULL, NULL);
    variable_item_set_current_value_index(sensor_name_item, 0);
    variable_item_set_current_value_text(sensor_name_item, sensor->name);

    //Тип датчика (не редактируется)
    VariableItem* item = variable_item_list_add(variable_item_list, "Type", 1, NULL, NULL);
    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, sensor->type->typename);

    //Порт подключения датчка (для one wire и single wire)
    if(sensor->type->interface == &ONE_WIRE || sensor->type->interface == &SINGLE_WIRE) {
        uint8_t aviable_gpio_count = unitemp_gpio_getAviablePortsCount(sensor->type->interface);
        item = variable_item_list_add(
            variable_item_list, "GPIO", aviable_gpio_count, _gpio_change_callback, app);
        variable_item_set_current_value_index(item, 0);
        variable_item_set_current_value_text(
            item, unitemp_gpio_getAviablePort(sensor->type->interface, 0)->name);
    }
    //Адрес устройства на шине I2C (для I2C)
    if(sensor->type->interface == &I2C) {
        item = variable_item_list_add(
            variable_item_list,
            "I2C address",
            ((I2CSensor*)sensor->instance)->maxI2CAdr - ((I2CSensor*)sensor->instance)->minI2CAdr +
                1,
            _i2caddr_change_callback,
            app);
        char buff[5];
        snprintf(buff, 5, "0x%2X", ((I2CSensor*)sensor->instance)->currentI2CAdr);
        variable_item_set_current_value_text(item, buff);
    }
    //Сохранение
    variable_item_list_add(variable_item_list, "Save", 1, NULL, NULL);

    view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_ID);
}

void unitemp_SensorEdit_free(void) {
    //Очистка списка элементов
    variable_item_list_free(variable_item_list);
    //Очистка вида
    view_free(view);
    //Удаление вида после обработки
    view_dispatcher_remove_view(app->view_dispatcher, VIEW_ID);
}