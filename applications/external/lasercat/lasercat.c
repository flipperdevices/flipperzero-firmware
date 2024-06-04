
//подключаем модули
#include <stdio.h>
#include <furi.h>
#include <furi_hal.h> // подключается когда 5 вольт
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <furi_hal_power.h> //подключается когда используется GPO
#include "lasercat_icons.h" //подключаем когда есть images именно так idприложения_icons.h

// (*1) Это будет метка, указывающая вам, какая кнопка клавиатуры нажата.
char* currentKeyPressed;
int BUFFER = 10;
bool keyonoff = false;
// Эта функция предназначена для рисования графического интерфейса экрана каждый раз, когда
// flip обновляет дисплей
static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    // очистка дисплея
    canvas_clear(canvas);

    // Код с lopaka.app
    canvas_set_bitmap_mode(canvas, true);
    canvas_draw_icon(canvas, 0, 0, &I_cat);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 58, 12, "LaserCat");
    canvas_set_font(canvas, FontKeyboard);
    canvas_draw_str(canvas, 67, 26, "Touch");
    canvas_draw_str(canvas, 60, 38, "");
    canvas_draw_icon(canvas, 75, 29, &I_Ok_btn_pressed);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 53, 56, currentKeyPressed); // изменяемая перемнная
}

// (*2) Здесь мы можем определить обратный вызов для таймера: каждые 2 секунды
// система furi-timer будет вызывать наш определенный обратный вызов
static void timer_callback(void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_assert(event_queue);
    if(keyonoff == true) {
        currentKeyPressed = "Laser > ON";
        furi_hal_gpio_write(&gpio_ext_pc3, true);
    }

    if(keyonoff == false) {
        currentKeyPressed = "Laser > OFF";
        furi_hal_gpio_write(&gpio_ext_pc3, false);
    }
}

// Эта функция является обработчиком пользовательского ввода (кнопки справа
// на Flip используются для навигации, подтверждения и возврата назад)
static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

// Основная запись приложения, как определено внутри application.fam

int32_t main_fap(void* p) {
    UNUSED(p);
    // Инициализация (*1)
    keyonoff = false;
    // включаем 5 вольт при запуске
    uint8_t attempts = 0;
    if(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
        furi_hal_power_enable_otg();
        furi_delay_ms(10);
    }
    // конец
    //отключим порт при запуске
    furi_hal_gpio_write(&gpio_ext_pc3, false);
    //напишем что отключен
    currentKeyPressed = "Laser > OFF";
    // Текущее событие типа InputEvent
    InputEvent event;

    // Очередь событий для 8 элементов размера InputEvent
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // ViewPort нужен для рисования графического интерфейса пользователя
    ViewPort* view_port = view_port_alloc();

    // мы даем этому [view_port_draw_callback_set] функцию, определенную
    // before (draw_callback) для рисования графического интерфейса на дисплее Flip
    view_port_draw_callback_set(view_port, draw_callback, NULL);

    // Та же концепция применима к [view_port_input_callback_set], связанному с функцией (input_callback)
    //определенной ранее
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Cоздаем структуру графического интерфейса пользователя и связать ее с ранее определенным окном просмотра
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Создает furi-таймер и связывает соответствующий обратный вызов, определенный в (*2)
    FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, event_queue);
    // Запускает таймер - время истечения в миллисекундах (в данном случае 2 секунды)
    furi_timer_start(timer, 500);
    //проверем И подключаем GPO
    furi_hal_gpio_write(&gpio_ext_pc3, false);
    furi_hal_gpio_init(&gpio_ext_pc3, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    // Бесконечный цикл... (как у arduino)
    // uint8_t attempts = 0; //нужно на 5 вольт
    while(1) {
        // Продолжаем (бесконечно) выводить из очереди все события, накопленные внутри
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        // Если событием из очереди является нажатие кнопки кнопок и запись значений currentKeyPressed, мы выходим из цикла

        if(event.key == InputKeyOk) {
            if(keyonoff == true) {
                currentKeyPressed = "Laser > OFF";
                keyonoff = false;

            } else {
                currentKeyPressed = "Laser > ON";
                keyonoff = true;
            }
        }

        if(event.key == InputKeyBack) {
            //отключим порт
            furi_hal_gpio_write(&gpio_ext_pc3, false);
            // на всякий отключим 5 вольт
            if(furi_hal_power_is_otg_enabled()) {
                furi_hal_power_disable_otg();
                furi_delay_ms(10);
            }
            break;
        }
    }
    // Когда вы используете таймер, не забудьте остановить его
    furi_timer_stop(timer);
    // и освободите память, выделенную для таймера
    furi_timer_free(timer);
    // после выхода из цикла нам нужно освободить ресурсы:
    // очистить все элементы внутри очереди
    furi_message_queue_free(event_queue);

    // Мы удаляем графический интерфейс из связанного порта просмотра
    gui_remove_view_port(gui, view_port);
    //отключить 5 вольт
    if(furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
    }
    // Освобождаем память, удаляем view_port и закрываем
    // Запись GUI
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}
