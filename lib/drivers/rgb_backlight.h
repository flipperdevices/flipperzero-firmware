/*
    RGB backlight FlipperZero driver
    Copyright (C) 2022-2023 Victor Nikitchuk (https://github.com/quen0n)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <furi.h>
#include "SK6805.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char* name;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGBBacklightColor;

typedef struct {
    uint8_t version;
    uint8_t display_color_index;
    bool settings_is_loaded;
} RGBBacklightSettings;

/**
 * @brief Получить текущие настройки RGB-подсветки
 *
 * @return Указатель на структуру настроек
 */
RGBBacklightSettings* rgb_backlight_get_settings(void);

/**
 * @brief Загрузить настройки подсветки с SD-карты
 */
void rgb_backlight_load_settings(void);

/**
 * @brief Сохранить текущие настройки RGB-подсветки
 */
void rgb_backlight_save_settings(void);

/**
 * @brief Применить текущие настройки RGB-подсветки
 *
 * @param brightness Яркость свечения (0-255)
 */
void rgb_backlight_update(uint8_t brightness);

/**
 * @brief Установить цвет RGB-подсветки
 *
 * @param color_index Индекс цвета (0 - rgb_backlight_get_color_count())
 */
void rgb_backlight_set_color(uint8_t color_index);

/**
 * @brief Получить количество доступных цветов
 *
 * @return Число доступных вариантов цвета
 */
uint8_t rgb_backlight_get_color_count(void);

/**
 * @brief Получить текстовое название цвета
 *
 * @param index Индекс из доступных вариантов цвета
 * @return Указатель на строку с названием цвета
 */
const char* rgb_backlight_get_color_text(uint8_t index);

#ifdef __cplusplus
}
#endif
