#include "unitemp.h"
#include "interfaces/SingleWireSensor.h"
#include "Sensors.h"
#include "./views/UnitempViews.h"

#include <furi_hal_power.h>
#include <m-string.h>

//TODO: Реализовать ограничение на добавление датчиков если интерфейс недоступен
//TODO: Исправить падения при добавлении и сохранении датчка
//TODO: Исключить добавление датчиков DS18x2x с одинаковыми ID
//TODO: Проверка корректности параметров датчика перед аллокацией
//TODO: Не выкидывать датчик в ошибку при первом же неудачном опросе
//TODO: Запускать преобразование DS18x2x разом, затем поочерёдно считывать

/* Переменные */
//Данные приложения
Unitemp* app;

void uintemp_celsiumToFarengate(Sensor* sensor) {
    sensor->temp = sensor->temp * (9.0 / 5.0) + 32;
}

bool unitemp_saveSettings(void) {
    //Выделение памяти для потока
    app->file_stream = file_stream_alloc(app->storage);

    //Переменная пути к файлу
    FuriString* filepath = furi_string_alloc();
    //Составление пути к файлу
    furi_string_printf(filepath, "%s/%s", APP_PATH_FOLDER, APP_FILENAME_SETTINGS);
    //Создание папки плагина
    storage_common_mkdir(app->storage, APP_PATH_FOLDER);
    //Открытие потока
    if(!file_stream_open(
           app->file_stream, furi_string_get_cstr(filepath), FSAM_READ_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(
            APP_NAME,
            "An error occurred while saving the settings file: %d\r\n",
            file_stream_get_error(app->file_stream));
        //Закрытие потока и освобождение памяти
        file_stream_close(app->file_stream);
        stream_free(app->file_stream);
        return false;
    }

    //Сохранение настроек
    stream_write_format(
        app->file_stream, "INFINITY_BACKLIGHT %d\n", app->settings.infinityBacklight);
    stream_write_format(app->file_stream, "UNIT %d\n", app->settings.unit);

    //Закрытие потока и освобождение памяти
    file_stream_close(app->file_stream);
    stream_free(app->file_stream);

    FURI_LOG_I(APP_NAME, "Settings have been successfully saved\r\n");
    return true;
}

bool unitemp_loadSettings(void) {
    FURI_LOG_D(APP_NAME, "Loading settings...");
    //Выделение памяти на поток
    app->file_stream = file_stream_alloc(app->storage);

    //Переменная пути к файлу
    FuriString* filepath = furi_string_alloc();
    //Составление пути к файлу
    furi_string_printf(filepath, "%s/%s", APP_PATH_FOLDER, APP_FILENAME_SETTINGS);

    //Открытие потока к файлу настроек
    if(!file_stream_open(
           app->file_stream, furi_string_get_cstr(filepath), FSAM_READ_WRITE, FSOM_OPEN_EXISTING)) {
        //Сохранение настроек по умолчанию в случае отсутствия файла
        if(file_stream_get_error(app->file_stream) == FSE_NOT_EXIST) {
            FURI_LOG_W(APP_NAME, "Missing settings file. Setting defaults and saving...\r\n");
            //Закрытие потока и освобождение памяти
            file_stream_close(app->file_stream);
            stream_free(app->file_stream);
            //Сохранение стандартного конфига
            unitemp_saveSettings();
            return false;
        } else {
            FURI_LOG_E(
                APP_NAME,
                "An error occurred while loading the settings file: %d. Standard values have been applied\r\n",
                file_stream_get_error(app->file_stream));
            //Закрытие потока и освобождение памяти
            file_stream_close(app->file_stream);
            stream_free(app->file_stream);
            return false;
        }
    }

    //Вычисление размера файла
    uint8_t file_size = stream_size(app->file_stream);
    FURI_LOG_D(APP_NAME, "Settings file size: %d bytes\r\n", file_size);
    //Если файл пустой, то:
    if(file_size == (uint8_t)0) {
        FURI_LOG_W(APP_NAME, "Settings file is empty\r\n");
        //Закрытие потока и освобождение памяти
        file_stream_close(app->file_stream);
        stream_free(app->file_stream);
        //Сохранение стандартного конфига
        unitemp_saveSettings();
        return false;
    }
    //Выделение памяти под загрузку файла
    uint8_t* file_buf = malloc(file_size);
    //Опустошение буфера файла
    memset(file_buf, 0, file_size);
    //Загрузка файла
    if(stream_read(app->file_stream, file_buf, file_size) != file_size) {
        //Выход при ошибке чтения
        FURI_LOG_E(APP_NAME, "Error reading settings file\r\n");
        //Закрытие потока и освобождение памяти
        file_stream_close(app->file_stream);
        stream_free(app->file_stream);
        free(file_buf);
        return false;
    }
    //Построчное чтение файла
    //Указатель на начало строки
    FuriString* file = furi_string_alloc_set_str((char*)file_buf);
    //Сколько байт до конца строки
    size_t line_end = 0;

    while(line_end != STRING_FAILURE) {
        char buff[20] = {0};
        sscanf(((char*)(file_buf + line_end)), "%s", buff);

        if(!strcmp(buff, "INFINITY_BACKLIGHT")) {
            //Чтение значения параметра
            int p = 0;
            sscanf(((char*)(file_buf + line_end)), "INFINITY_BACKLIGHT %d", &p);
            if(p == 0) {
                app->settings.infinityBacklight = false;
            } else {
                app->settings.infinityBacklight = true;
            }
        } else if(!strcmp(buff, "UNIT")) {
            //Чтение значения параметра
            int p = 0;
            sscanf(((char*)(file_buf + line_end)), "\nUNIT %d", &p);
            if(p == CELSIUS) {
                app->settings.unit = CELSIUS;
            } else {
                app->settings.unit = FAHRENHEIT;
            }
        } else {
            FURI_LOG_W(APP_NAME, "Unknown settings parameter: %s\r\n", buff);
        }

        //Вычисление конца строки
        line_end = furi_string_search_char(file, '\n', line_end + 1);
    }
    free(file_buf);
    file_stream_close(app->file_stream);
    stream_free(app->file_stream);

    FURI_LOG_I(APP_NAME, "Settings have been successfully loaded\r\n");
    return true;
}

/**
 * @brief Выделение места под переменные плагина
 * 
 * @return true Если всё прошло успешно
 * @return false Если в процессе загрузки произошла ошибка
 */
static bool unitemp_alloc(void) {
    //Выделение памяти под данные приложения
    app = malloc(sizeof(Unitemp));
    //Разрешение работы приложения
    app->processing = true;

    //Открытие хранилища (?)
    app->storage = furi_record_open(RECORD_STORAGE);

    //Уведомления
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    //Установка значений по умолчанию
    app->settings.infinityBacklight = true; //Подсветка горит всегда
    app->settings.unit = CELSIUS; //Единица измерения - градусы Цельсия

    app->gui = furi_record_open(RECORD_GUI);
    //Диспетчер окон
    app->view_dispatcher = view_dispatcher_alloc();

    unitemp_Summary_alloc();
    unitemp_MainMenu_alloc();
    unitemp_Settings_alloc();
    unitemp_SensorsList_alloc();
    unitemp_SensorEdit_alloc();
    unitemp_SensorNameEdit_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    return true;
}

/**
 * @brief Освыбождение памяти после работы приложения
 */
static void unitemp_free(void) {
    unitemp_SensorNameEdit_free();
    unitemp_SensorEdit_free();
    unitemp_SensorsList_free();
    unitemp_Settings_free();
    unitemp_MainMenu_free();
    unitemp_Summary_free();

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);
    //Очистка датчиков
    //Высвыбождение данных датчиков
    unitemp_sensors_free();

    //Закрытие уведомлений
    furi_record_close(RECORD_NOTIFICATION);
    //Закрытие хранилища
    furi_record_close(RECORD_STORAGE);
    //Удаление в самую последнюю очередь
    free(app);
}

/**
 * @brief Точка входа в приложение
 * 
 * @return Код ошибки
 */
int32_t unitemp_app() {
    //Выделение памяти под переменные
    //Выход если произошла ошибка
    if(unitemp_alloc() == false) {
        //Освобождение памяти
        unitemp_free();
        //Выход
        return 0;
    }

    //Загрузка настроек из SD-карты
    unitemp_loadSettings();
    //Применение настроек
    if(app->settings.infinityBacklight == true) {
        //Постоянное свечение подсветки
        notification_message(app->notifications, &sequence_display_backlight_enforce_on);
    }
    app->settings.lastOTGState = furi_hal_power_is_otg_enabled();
    //Загрузка датчиков из SD-карты
    unitemp_sensors_load();
    //Инициализация датчиков
    unitemp_sensors_init();

    unitemp_Summary_switch();

    while(app->processing) {
        if(app->sensors_ready) unitemp_sensors_updateValues();
        furi_delay_ms(100);
    }

    //Деинициализация датчиков
    unitemp_sensors_deInit();
    //Автоматическое управление подсветкой
    if(app->settings.infinityBacklight == true)
        notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
    //Освобождение памяти
    unitemp_free();
    //Выход
    return 0;
}