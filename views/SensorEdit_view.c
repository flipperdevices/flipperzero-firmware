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
static Sensor* editable_sensor;
//Элемент списка - имя датчика
static VariableItem* sensor_name_item;
//Элемент списка - адрес датчика one wire
static VariableItem* onewire_addr_item;

//Элемент списка - адрес датчика one wire
static VariableItem* onewire_type_item;

#define VIEW_ID SENSOREDIT_VIEW

void scan(void) {
    OneWireSensor* ow_sensor = editable_sensor->instance;

    //Сканирование шины one wire
    unitemp_onewire_bus_init(ow_sensor->bus);
    uint8_t* id = unitemp_onewire_enum_next(ow_sensor->bus);

    if(id == NULL) {
        unitemp_onewire_enum_init();
        id = unitemp_onewire_enum_next(ow_sensor->bus);
        if(id == NULL) {
            memset(ow_sensor->deviceID, 0, 8);
            ow_sensor->familyCode = 0;
            unitemp_onewire_bus_deinit(ow_sensor->bus);
            variable_item_set_current_value_text(onewire_addr_item, "empty");
            variable_item_set_current_value_text(
                onewire_type_item, editable_sensor->type->typename);
            return;
        }
    }

    unitemp_onewire_bus_deinit(ow_sensor->bus);

    memcpy(ow_sensor->deviceID, id, 8);
    ow_sensor->familyCode = id[0];
    FURI_LOG_D(
        APP_NAME,
        "Found sensor's ID: %02X%02X%02X%02X%02X%02X%02X%02X",
        id[0],
        id[1],
        id[2],
        id[3],
        id[4],
        id[5],
        id[6],
        id[7]);

    if(ow_sensor->familyCode != 0) {
        char id_buff[10];
        snprintf(
            id_buff,
            10,
            "%02X%02X%02X",
            ow_sensor->deviceID[1],
            ow_sensor->deviceID[2],
            ow_sensor->deviceID[3]);
        //А больше не лезет(
        variable_item_set_current_value_text(onewire_addr_item, id_buff);
        switch(ow_sensor->deviceID[0]) {
        case FC_DS18B20:
            variable_item_set_current_value_text(onewire_type_item, "DS18B20");
            break;
        case FC_DS18S20:
            variable_item_set_current_value_text(onewire_type_item, "DS18S20");
            break;
        case FC_DS1822:
            variable_item_set_current_value_text(onewire_type_item, "DS1822");
            break;
        default:
            variable_item_set_current_value_text(onewire_type_item, "unknown");
        }
    } else {
        variable_item_set_current_value_text(onewire_addr_item, "empty");
        variable_item_set_current_value_text(onewire_type_item, editable_sensor->type->typename);
    }
}

/**
 * @brief Функция обработки нажатия кнопки "Назад"
 *
 * @param context Указатель на данные приложения
 * @return ID вида в который нужно переключиться
 */
static uint32_t _exit_callback(void* context) {
    UNUSED(context);
    unitemp_sensor_free(editable_sensor);
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
        unitemp_SensorNameEdit_switch(editable_sensor);
    }
    //Сохранение
    if((index == 3 && editable_sensor->type->interface != &ONE_WIRE) ||
       (index == 4 && editable_sensor->type->interface == &ONE_WIRE)) {
        app->sensors[app->sensors_count++] = editable_sensor;
        unitemp_sensors_save();
        unitemp_sensors_reload();

        unitemp_Summary_switch();
    }
    //Адрес устройства на шине one wire
    if(index == 3 && editable_sensor->type->interface == &ONE_WIRE) {
        scan();
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
 * @brief Функция обработки изменения значения имени датчика
 * 
 * @param item Указатель на элемент списка
 */
static void _name_change_callback(VariableItem* item) {
    variable_item_set_current_value_index(item, 0);
    unitemp_SensorNameEdit_switch(editable_sensor);
}
/**
 * @brief Функция обработки изменения значения адреса датчика one wire
 * 
 * @param item Указатель на элемент списка
 */
static void _onwire_addr_change_callback(VariableItem* item) {
    variable_item_set_current_value_index(item, 0);
    scan();
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
    sensor_name_item = variable_item_list_add(
        variable_item_list, "Name", strlen(sensor->name) > 7 ? 1 : 2, _name_change_callback, NULL);
    variable_item_set_current_value_index(sensor_name_item, 0);
    variable_item_set_current_value_text(sensor_name_item, sensor->name);

    //Тип датчика (не редактируется)
    onewire_type_item = variable_item_list_add(variable_item_list, "Type", 1, NULL, NULL);
    variable_item_set_current_value_index(onewire_type_item, 0);
    variable_item_set_current_value_text(onewire_type_item, sensor->type->typename);

    //Порт подключения датчка (для one wire и single wire)
    if(sensor->type->interface == &ONE_WIRE || sensor->type->interface == &SINGLE_WIRE) {
        uint8_t aviable_gpio_count = unitemp_gpio_getAviablePortsCount(sensor->type->interface);
        VariableItem* item = variable_item_list_add(
            variable_item_list, "GPIO", aviable_gpio_count, _gpio_change_callback, app);
        variable_item_set_current_value_index(item, 0);
        variable_item_set_current_value_text(
            item, unitemp_gpio_getAviablePort(sensor->type->interface, 0)->name);
    }
    //Адрес устройства на шине I2C (для датчиков I2C)
    if(sensor->type->interface == &I2C) {
        VariableItem* item = variable_item_list_add(
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

    //Адрес устройства на шине one wire (для датчиков one wire)
    if(sensor->type->interface == &ONE_WIRE) {
        //Сохранение
        onewire_addr_item = variable_item_list_add(
            variable_item_list, "Address", 2, _onwire_addr_change_callback, NULL);
        OneWireSensor* ow_sensor = sensor->instance;
        if(ow_sensor->familyCode == 0) {
            variable_item_set_current_value_text(onewire_addr_item, "Scan");
        } else {
            char id_buff[10];
            snprintf(
                id_buff,
                10,
                "%2X..%2X%2X",
                ow_sensor->deviceID[0],
                ow_sensor->deviceID[6],
                ow_sensor->deviceID[7]);
            variable_item_set_current_value_text(onewire_addr_item, id_buff);
        }
    }
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