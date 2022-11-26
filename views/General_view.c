#include "UnitempViews.h"
#include "unitemp_icons.h"
static View* view;

static const uint8_t temp_positions[3][2] = {{37, 23}, {37, 16}, {9, 16}};
static const uint8_t hum_positions[2][2] = {{37, 38}, {65, 16}};

static uint8_t sensor_index = 0;
static char buff[7];
static void _draw_noSensors(Canvas* canvas) {
    canvas_draw_str(canvas, 0, 24, "Sensors not found");
}

static void _draw_temp(Canvas* canvas, float temp, uint8_t pos) {
    //Рисование рамки
    canvas_draw_rframe(canvas, temp_positions[pos][0], temp_positions[pos][1], 54, 20, 3);
    canvas_draw_rframe(canvas, temp_positions[pos][0], temp_positions[pos][1], 54, 19, 3);
    int16_t temp_int = temp;
    int8_t temp_dec = abs((int16_t)(temp * 10) % 10);

    //Рисование иконки
    canvas_draw_icon(
        canvas,
        temp_positions[pos][0] + 3,
        temp_positions[pos][1] + 3,
        (app->settings.unit == CELSIUS ? &I_temp_C_11x14 : &I_temp_F_11x14));

    if((int16_t)temp == -128) {
        snprintf(buff, 5, "--");
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(
            canvas,
            temp_positions[pos][0] + 27,
            temp_positions[pos][1] + 10,
            AlignCenter,
            AlignCenter,
            buff);
        snprintf(buff, 4, ". -");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(
            canvas,
            temp_positions[pos][0] + 50,
            temp_positions[pos][1] + 10 + 3,
            AlignRight,
            AlignCenter,
            buff);
        return;
    }

    //Целая часть температуры
    snprintf(buff, 7, "%d", temp_int);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(
        canvas,
        temp_positions[pos][0] + 27 + ((temp_int <= -10) ? 5 : 0),
        temp_positions[pos][1] + 10,
        AlignCenter,
        AlignCenter,
        buff);
    //Печать дробной части температуры в диапазоне от -9 до 99 (когда два знака в числе)
    if(temp_int > -10 && temp_int <= 99) {
        uint8_t int_len = canvas_string_width(canvas, buff);
        snprintf(buff, 4, ".%d", temp_dec);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(
            canvas,
            temp_positions[pos][0] + 27 + int_len / 2 + 2,
            temp_positions[pos][1] + 10 + 7,
            buff);
    }
}

void _draw_hum(Canvas* canvas, float hum, uint8_t pos) {
    //Рисование рамки
    canvas_draw_rframe(canvas, hum_positions[pos][0], hum_positions[pos][1], 54, 20, 3);
    canvas_draw_rframe(canvas, hum_positions[pos][0], hum_positions[pos][1], 54, 19, 3);

    //Рисование иконки
    canvas_draw_icon(canvas, hum_positions[pos][0] + 3, hum_positions[pos][1] + 2, &I_hum_9x15);

    if((int8_t)hum == -128) {
        snprintf(buff, 5, "--");
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(
            canvas,
            hum_positions[pos][0] + 27,
            hum_positions[pos][1] + 10,
            AlignCenter,
            AlignCenter,
            buff);
        snprintf(buff, 4, ". -");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(
            canvas,
            hum_positions[pos][0] + 50,
            hum_positions[pos][1] + 10 + 3,
            AlignRight,
            AlignCenter,
            buff);
        return;
    }

    //Целая часть влажности
    snprintf(buff, 5, "%d", (uint8_t)hum);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(
        canvas,
        hum_positions[pos][0] + 27,
        hum_positions[pos][1] + 10,
        AlignCenter,
        AlignCenter,
        buff);
    uint8_t int_len = canvas_string_width(canvas, buff);
    //Единица измерения
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(
        canvas, hum_positions[pos][0] + 27 + int_len / 2 + 4, hum_positions[pos][1] + 10 + 7, "%");
}

static void _draw_press(Canvas* canvas, float press) {
    UNUSED(press);
    const uint8_t x = 29, y = 39;
    //Рисование рамки
    canvas_draw_rframe(canvas, x, y, 69, 20, 3);
    canvas_draw_rframe(canvas, x, y, 69, 19, 3);

    //Рисование иконки
    canvas_draw_icon(canvas, x + 3, y + 4, &I_arrow_up_7x13);

    //Давление
    snprintf(buff, 6, "%d", (uint16_t)press);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, x + 30, y + 10, AlignCenter, AlignCenter, buff);
    //Единица измерения
    canvas_draw_icon(canvas, x + 50, y + 3, &I_mm_hg_17x15);
}

static void _draw_sensorsCarousel(Canvas* canvas) {
    //Рисование рамки
    canvas_draw_rframe(canvas, 3, 0, 122, 63, 7);
    canvas_draw_rframe(canvas, 3, 0, 122, 64, 7);

    //Печать имени
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 64, 7, AlignCenter, AlignCenter, app->sensors[sensor_index]->name);
    //Подчёркивание
    uint8_t line_len = canvas_string_width(canvas, app->sensors[sensor_index]->name) + 2;
    canvas_draw_line(canvas, 64 - line_len / 2, 12, 64 + line_len / 2, 12);

    //Стрелка вправо
    if(unitemp_sensors_getTypesCount() > 0 && sensor_index < unitemp_sensors_getCount() - 1) {
        canvas_draw_icon(canvas, 117, 28, &I_arrow_right_5x9);
    }
    //Стрелка влево
    if(sensor_index > 0) {
        canvas_draw_icon(canvas, 6, 28, &I_arrow_left_5x9);
    }

    if(app->sensors[sensor_index]->status == UT_TIMEOUT) {
        const Icon* frames[] = {&I_happy_2_78x46, &I_happy_78x46, &I_sad_78x46};
        canvas_draw_icon(canvas, 25, 15, frames[furi_get_tick() % 2250 / 750]);
        return;
    }

    //Селектор значений для отображения
    switch(app->sensors[sensor_index]->type->datatype) {
    case UT_DATA_TYPE_TEMP:
        _draw_temp(canvas, app->sensors[sensor_index]->temp, 0);
        break;
    case UT_DATA_TYPE_TEMP_HUM:
        _draw_temp(canvas, app->sensors[sensor_index]->temp, 1);
        _draw_hum(canvas, app->sensors[sensor_index]->hum, 0);
        break;
    case UT_DATA_TYPE_TEMP_PRESS:
        _draw_temp(canvas, app->sensors[sensor_index]->temp, 1);
        _draw_press(canvas, app->sensors[sensor_index]->pressure);
        break;
    case UT_DATA_TYPE_TEMP_HUM_PRESS:
        _draw_temp(canvas, app->sensors[sensor_index]->temp, 2);
        _draw_hum(canvas, app->sensors[sensor_index]->hum, 1);
        _draw_press(canvas, app->sensors[sensor_index]->pressure);
        break;
    }
}

static void _draw_callback(Canvas* canvas, void* _model) {
    UNUSED(_model);

    app->sensors_ready = true;

    uint8_t sensors_count = unitemp_sensors_getCount();

    if(sensors_count == 0) {
        _draw_noSensors(canvas);
    }
    if(sensors_count > 0) {
        _draw_sensorsCarousel(canvas);
    }
}

static bool _input_callback(InputEvent* event, void* context) {
    Unitemp* app = context;

    //Выход по короткому нажатию "назад"
    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        app->processing = false;
    }
    //Пролистывание карусели по короткому нажатию "право"
    if(event->key == InputKeyRight && event->type == InputTypeShort) {
        if(++sensor_index >= unitemp_sensors_getCount()) sensor_index = 0;
    }
    //Пролистывание карусели по короткому нажатию "лево"
    if(event->key == InputKeyLeft && event->type == InputTypeShort) {
        if(--sensor_index >= unitemp_sensors_getCount())
            sensor_index = unitemp_sensors_getCount() - 1;
    }
    //Вход в главное меню по короткому нажатию "Ок"
    if(event->key == InputKeyOk && event->type == InputTypeShort) {
        app->sensors_ready = false;
        unitemp_MainMenu_switch();
    }

    return true;
}

void unitemp_General_alloc(void) {
    view = view_alloc();
    view_set_context(view, app);
    view_set_draw_callback(view, _draw_callback);
    view_set_input_callback(view, _input_callback);

    view_dispatcher_add_view(app->view_dispatcher, GENERAL_VIEW, view);
}

void unitemp_General_switch(void) {
    app->sensors_ready = true;
    view_dispatcher_switch_to_view(app->view_dispatcher, GENERAL_VIEW);
}

void unitemp_General_free(void) {
    view_free(view);
}
