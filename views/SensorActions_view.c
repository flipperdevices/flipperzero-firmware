#include "UnitempViews.h"
#include <gui/modules/variable_item_list.h>
#include <stdio.h>

//Текущий вид
static View* view;
//Список
static VariableItemList* variable_item_list;
//Текущий датчик
static Sensor* current_sensor;

typedef enum carousel_info {
    CAROUSEL_VALUES, //Отображение значений датчиков
    CAROUSEL_INFO, //Отображение информации о датчике
} carousel_info;
extern carousel_info carousel_info_selector;

#define VIEW_ID VIEW_SENSOR_ACTIONS

/* ================== Подтверждение удаления ================== */
/**
 * @brief Функция обработки нажатия кнопки "Назад"
 * 
 * @param context Указатель на данные приложения
 * @return ID вида в который нужно переключиться
 */
static uint32_t _delete_exit_callback(void* context) {
    UNUSED(context);
    //Возвращаем ID вида, в который нужно вернуться
    return VIEW_SENSOR_ACTIONS;
}
/**
 * @brief Обработчик нажатий на кнопку в виджете
 * 
 * @param result Какая из кнопок была нажата
 * @param type Тип нажатия
 * @param context Указатель на данные плагина
 */
static void _delete_click_callback(GuiButtonType result, InputType type, void* context) {
    UNUSED(context);
    //Коротко нажата левая кнопка (Cancel)
    if(result == GuiButtonTypeLeft && type == InputTypeShort) {
        unitemp_SensorActions_switch(current_sensor);
    }
    //Коротко нажата правая кнопка (Delete)
    if(result == GuiButtonTypeRight && type == InputTypeShort) {
        //Удаление датчика
        unitemp_sensor_delete(current_sensor);
        //Выход из меню
        unitemp_General_switch();
    }
}
/**
 * @brief Переключение в виджет удаления датчика
 */
static void _delete_widget_switch(void) {
    //Очистка виджета
    widget_reset(app->widget);
    //Добавление кнопок
    widget_add_button_element(
        app->widget, GuiButtonTypeLeft, "Cancel", _delete_click_callback, app);
    widget_add_button_element(
        app->widget, GuiButtonTypeRight, "Delete", _delete_click_callback, app);

    char delete_str[32];
    snprintf(delete_str, sizeof(delete_str), "\e#Delete %s?\e#", current_sensor->name);
    widget_add_text_box_element(
        app->widget, 0, 0, 128, 23, AlignCenter, AlignCenter, delete_str, false);

    if(current_sensor->type->interface == &ONE_WIRE) {
        OneWireSensor* s = current_sensor->instance;

        snprintf(
            delete_str,
            sizeof(delete_str),
            "\e#Type:\e# %s",
            unitemp_onewire_sensor_getModel(current_sensor));
        widget_add_text_box_element(
            app->widget, 0, 16, 128, 23, AlignLeft, AlignTop, delete_str, false);
        snprintf(delete_str, sizeof(delete_str), "\e#GPIO:\e# %s", s->bus->gpio->name);
        widget_add_text_box_element(
            app->widget, 0, 28, 128, 23, AlignLeft, AlignTop, delete_str, false);

        snprintf(
            delete_str,
            sizeof(delete_str),
            "\e#ID:\e# %02X%02X%02X%02X%02X%02X%02X%02X",
            s->deviceID[0],
            s->deviceID[1],
            s->deviceID[2],
            s->deviceID[3],
            s->deviceID[4],
            s->deviceID[5],
            s->deviceID[6],
            s->deviceID[7]);
        widget_add_text_box_element(
            app->widget, 0, 40, 128, 23, AlignLeft, AlignTop, delete_str, false);
    }

    if(current_sensor->type->interface == &SINGLE_WIRE) {
        snprintf(delete_str, sizeof(delete_str), "\e#Type:\e# %s", current_sensor->type->typename);
        widget_add_text_box_element(
            app->widget, 0, 16, 128, 23, AlignLeft, AlignTop, delete_str, false);
        snprintf(
            delete_str,
            sizeof(delete_str),
            "\e#GPIO:\e# %s",
            ((SingleWireSensor*)current_sensor->instance)->gpio->name);
        widget_add_text_box_element(
            app->widget, 0, 28, 128, 23, AlignLeft, AlignTop, delete_str, false);
    }

    if(current_sensor->type->interface == &I2C) {
        snprintf(delete_str, sizeof(delete_str), "\e#Type:\e# %s", current_sensor->type->typename);
        widget_add_text_box_element(
            app->widget, 0, 16, 128, 23, AlignLeft, AlignTop, delete_str, false);
        snprintf(
            delete_str,
            sizeof(delete_str),
            "\e#I2C addr:\e# 0x%02X",
            ((I2CSensor*)current_sensor->instance)->currentI2CAdr);
        widget_add_text_box_element(
            app->widget, 0, 28, 128, 23, AlignLeft, AlignTop, delete_str, false);
    }

    view_set_previous_callback(widget_get_view(app->widget), _delete_exit_callback);
    view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_SENSOR_DELETE);
}

/**
 * @brief Функция обработки нажатия кнопки "Назад"
 *
 * @param context Указатель на данные приложения
 * @return ID вида в который нужно переключиться
 */
static uint32_t _exit_callback(void* context) {
    UNUSED(context);

    //Возврат предыдущий вид
    return VIEW_GENERAL;
}
/**
 * @brief Функция обработки нажатия средней кнопки
 *
 * @param context Указатель на данные приложения
 * @param index На каком элементе списка была нажата кнопка
 */
static void _enter_callback(void* context, uint32_t index) {
    UNUSED(context);
    switch(index) {
    case 0:
        carousel_info_selector = CAROUSEL_INFO;
        unitemp_General_switch();
        return;
    case 1:
        unitemp_SensorEdit_switch(current_sensor);
        break;
    case 2:
        _delete_widget_switch();
        break;
    case 3:
        unitemp_SensorsList_switch();
        break;
    case 4:
        unitemp_Settings_switch();
        break;
    case 5:
        //Help
        break;
    case 6:
        //About
        break;
    }
}

/**
 * @brief Создание меню действий с датчиком
 */
void unitemp_SensorActions_alloc(void) {
    variable_item_list = variable_item_list_alloc();
    //Сброс всех элементов меню
    variable_item_list_reset(variable_item_list);

    variable_item_list_add(variable_item_list, "Info", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "Edit", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "Delete", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "Add new sensor", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "Settings", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "Help", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "About", 1, NULL, NULL);

    //Добавление колбека на нажатие средней кнопки
    variable_item_list_set_enter_callback(variable_item_list, _enter_callback, app);
    //Создание вида из списка
    view = variable_item_list_get_view(variable_item_list);
    //Добавление колбека на нажатие кнопки "Назад"
    view_set_previous_callback(view, _exit_callback);
    //Добавление вида в диспетчер
    view_dispatcher_add_view(app->view_dispatcher, VIEW_ID, view);
}

void unitemp_SensorActions_switch(Sensor* sensor) {
    current_sensor = sensor;
    //Обнуление последнего выбранного пункта
    variable_item_list_set_selected_item(variable_item_list, 0);

    view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_ID);
}

void unitemp_SensorActions_free(void) {
    //Очистка списка элементов
    variable_item_list_free(variable_item_list);
    //Очистка вида
    view_free(view);
    //Удаление вида после обработки
    view_dispatcher_remove_view(app->view_dispatcher, VIEW_ID);
}