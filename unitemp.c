#include "unitemp.h"
#include "interfaces/SingleWireSensor.h"
#include "Sensors.h"
#include "scenes/Scenes.h"

#include <furi_hal_power.h>

/* Статические переменные */
//Данные приложения
Unitemp* app;

void uintemp_celsiumToFarengate(Sensor* sensor) {
    sensor->temp = sensor->temp * (9.0 / 5.0) + 32;
}

/**
 * @brief Сохранение настроек на SD-карту
 * 
 * @return true Если сохранение прошло успешно
 * @return false Если во время сохранения произошла ошибка
 */
static bool unitemp_saveSettings(void) {
    FURI_LOG_D(APP_NAME, "Saving settings...\r\n");

    //Выделение памяти для потока
    app->file_stream = file_stream_alloc(app->storage);

    //Переменная пути к файлу
    char filepath[sizeof(APP_PATH_FOLDER) + sizeof(APP_FILENAME_SETTINGS)] = {0};
    //Составление пути к файлу
    strcpy(filepath, APP_PATH_FOLDER);
    strcat(filepath, "/");
    strcat(filepath, APP_FILENAME_SETTINGS);
    //Создание папки плагина
    storage_common_mkdir(app->storage, APP_PATH_FOLDER);
    //Открытие потока
    if(!file_stream_open(app->file_stream, filepath, FSAM_READ_WRITE, FSOM_CREATE_ALWAYS)) {
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

/**
 * @brief Загрузка настроек с SD-карты
 * 
 * @return true Загрузка успешная
 * @return false Произошла ошибка
 */
static bool unitemp_loadSettings(void) {
    FURI_LOG_D(APP_NAME, "Loading settings...");
    //Выделение памяти на поток
    app->file_stream = file_stream_alloc(app->storage);

    //Переменная пути к файлу
    char filepath[sizeof(APP_PATH_FOLDER) + sizeof(APP_FILENAME_SETTINGS)] = {0};
    //Составление пути к файлу
    strcpy(filepath, APP_PATH_FOLDER);
    strcat(filepath, "/");
    strcat(filepath, APP_FILENAME_SETTINGS);

    //Открытие потока к файлу настроек
    if(!file_stream_open(app->file_stream, filepath, FSAM_READ_WRITE, FSOM_OPEN_EXISTING)) {
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
    char* line = strtok((char*)file_buf, "\n");
    while(line != NULL) {
        char buff[20];
        sscanf(line, "%s", buff);

        if(!strcmp(buff, "INFINITY_BACKLIGHT")) {
            //Чтение значения параметра
            int p = 0;
            sscanf(line, "INFINITY_BACKLIGHT %d", &p);
            if(p == 0) {
                app->settings.infinityBacklight = false;
            } else {
                app->settings.infinityBacklight = true;
            }
        } else if(!strcmp(buff, "UNIT")) {
            //Чтение значения параметра
            int p = 0;
            sscanf(line, "UNIT %d", &p);
            if(p == CELSIUS) {
                app->settings.unit = CELSIUS;
            } else {
                app->settings.unit = FAHRENHEIT;
            }
        } else {
            FURI_LOG_W(APP_NAME, "Unknown settings parameter: %s\r\n", buff);
        }

        line = strtok((char*)NULL, "\n");
    }
    free(file_buf);
    file_stream_close(app->file_stream);
    stream_free(app->file_stream);

    //Применение настроек
    if(app->settings.infinityBacklight) {
        //Постоянное свечение подсветки
        notification_message(app->notifications, &sequence_display_backlight_enforce_on);
    } else {
        //Автоматическое управление
        notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
    }
    app->settings.lastOTGState = furi_hal_power_is_otg_enabled();

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

    TempHum_secene_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    return true;
}

/**
 * @brief Освыбождение памяти после работы приложения
 */
static void unitemp_free(void) {
    //Автоматическое управление подсветкой
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);

    TempHum_secene_free();

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);
    //Очистка датчиков
    //Высвыбождение данных датчиков
    unitemp_sensors_free();
    free(app->sensors);
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
    //Загрузка датчиков из SD-карты
    unitemp_sensors_load();
    //Инициализация датчиков
    unitemp_sensors_init();

    view_dispatcher_switch_to_view(app->view_dispatcher, TEMPHUM_VIEW);

    while(app->processing) {
        unitemp_sensors_updateValues();
    }

    //Деинициализация датчиков
    unitemp_sensors_deInit();
    //Освобождение памяти
    unitemp_free();
    //Выход
    return 0;
}