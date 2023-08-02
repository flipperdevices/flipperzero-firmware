#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

static int matrix[6][7] = {0};
static int cursorx = 3;
static int cursory = 5;
static int player = 1;
static int scoreX = 0;
static int scoreO = 0;

typedef struct {
    FuriMutex* mutex;
} FourInRowState;

void init() {
    for(size_t i = 0; i < 6; i++) {
        for(size_t j = 0; j < 7; j++) {
            matrix[i][j] = 0;
        }
    }
    cursorx = 3;
    cursory = 5;
    player = 1;
}

const NotificationSequence end = {
    &message_vibro_on,

    &message_note_ds4,
    &message_delay_10,
    &message_sound_off,
    &message_delay_10,

    &message_note_ds4,
    &message_delay_10,
    &message_sound_off,
    &message_delay_10,

    &message_note_ds4,
    &message_delay_10,
    &message_sound_off,
    &message_delay_10,

    &message_vibro_off,
    NULL,
};

void intToStr(int num, char* str) {
    int i = 0, sign = 0;

    if(num < 0) {
        num = -num;
        sign = 1;
    }

    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while(num > 0);

    if(sign) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // Reverse the string
    int j, len = i;
    char temp;
    for(j = 0; j < len / 2; j++) {
        temp = str[j];
        str[j] = str[len - j - 1];
        str[len - j - 1] = temp;
    }
}

int next_height(int x) {
    if(matrix[0][x] != 0) {
        return -1;
    }
    for(size_t y = 1; y < 6; y++) {
        if(matrix[y][x] != 0) {
            return y - 1;
        }
    }
    return 5;
}

int wincheck() {
    for(size_t y = 0; y <= 2; y++) {
        for(size_t x = 0; x <= 6; x++) {
            if(matrix[y][x] != 0 && matrix[y][x] == matrix[y + 1][x] &&
               matrix[y][x] == matrix[y + 2][x] && matrix[y][x] == matrix[y + 3][x]) {
                return matrix[y][x];
            }
        }
    }

    for(size_t y = 0; y <= 5; y++) {
        for(size_t x = 0; x <= 3; x++) {
            if(matrix[y][x] != 0 && matrix[y][x] == matrix[y][x + 1] &&
               matrix[y][x] == matrix[y][x + 2] && matrix[y][x] == matrix[y][x + 3]) {
                return matrix[y][x];
            }
        }
    }

    for(size_t y = 0; y <= 2; y++) {
        for(size_t x = 0; x <= 3; x++) {
            if(matrix[y][x] != 0 && matrix[y][x] == matrix[y + 1][x + 1] &&
               matrix[y][x] == matrix[y + 2][x + 2] && matrix[y][x] == matrix[y + 3][x + 3]) {
                return matrix[y][x];
            }
        }
    }

    for(size_t y = 3; y <= 5; y++) {
        for(size_t x = 0; x <= 3; x++) {
            if(matrix[y][x] != 0 && matrix[y][x] == matrix[y - 1][x + 1] &&
               matrix[y][x] == matrix[y - 2][x + 2] && matrix[y][x] == matrix[y - 3][x + 3]) {
                return matrix[y][x];
            }
        }
    }

    bool tf = true;
    for(size_t y = 0; y < 6; y++) {
        for(size_t x = 0; x < 7; x++) {
            if(matrix[y][x] == 0) {
                tf = false;
            }
        }
    }
    if(tf) {
        return 0;
    }

    return -1;
}

static void draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    const FourInRowState* fourinrow_state = ctx;

    furi_mutex_acquire(fourinrow_state->mutex, FuriWaitForever);
    canvas_clear(canvas);

    if(wincheck() != -1) {
        canvas_set_font(canvas, FontPrimary);

        if(wincheck() == 0) {
            canvas_draw_str(canvas, 30, 35, "Draw! O_o");
        }
        if(wincheck() == 1) {
            canvas_draw_str(canvas, 30, 35, "Player X win!");
        }
        if(wincheck() == 2) {
            canvas_draw_str(canvas, 30, 35, "Player O win!");
        }

        furi_mutex_release(fourinrow_state->mutex);

        return;
    }

    for(size_t i = 0; i < 6; i++) {
        for(size_t j = 0; j < 7; j++) {
            char el[2];
            switch(matrix[i][j]) {
            case 0:
                strcpy(el, "_\0");
                break;

            case 1:
                strcpy(el, "X\0");
                break;

            case 2:
                strcpy(el, "O\0");
                break;
            }
            canvas_draw_str(canvas, j * 10 + 10, i * 10 + 10, el);
        }
    }
    canvas_draw_str(canvas, cursorx * 10 + 8, cursory * 10 + 10, "[ ]");

    if(player == 1) {
        canvas_draw_str(canvas, 80, 10, "Turn: X");
    }
    if(player == 2) {
        canvas_draw_str(canvas, 80, 10, "Turn: O");
    }
    char scX[1];
    intToStr(scoreX, scX);
    char scO[1];
    intToStr(scoreO, scO);

    canvas_draw_str(canvas, 80, 20, "X:");
    canvas_draw_str(canvas, 90, 20, scX);

    canvas_draw_str(canvas, 80, 30, "O:");
    canvas_draw_str(canvas, 90, 30, scO);

    furi_mutex_release(fourinrow_state->mutex);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    // Проверяем, что контекст не нулевой
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;

    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t four_in_row_app(void* p) {
    UNUSED(p);

    // Текущее событие типа InputEvent
    InputEvent event;
    // Очередь событий на 8 элементов размера InputEvent
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    FourInRowState* fourinrow_state = malloc(sizeof(FourInRowState));

    fourinrow_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal); // Alloc Mutex
    if(!fourinrow_state->mutex) {
        FURI_LOG_E("4inRow", "cannot create mutex\r\n");
        furi_message_queue_free(event_queue);
        free(fourinrow_state);
        return 255;
    }

    // Создаем новый view port
    ViewPort* view_port = view_port_alloc();
    // Создаем callback отрисовки, без контекста
    view_port_draw_callback_set(view_port, draw_callback, fourinrow_state);
    // Создаем callback нажатий на клавиши, в качестве контекста передаем
    // нашу очередь сообщений, чтоб запихивать в неё эти события
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Создаем GUI приложения
    Gui* gui = furi_record_open(RECORD_GUI);
    // Подключаем view port к GUI в полноэкранном режиме
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(notification, &sequence_display_backlight_enforce_on);

    // Бесконечный цикл обработки очереди событий
    while(1) {
        // Выбираем событие из очереди в переменную event (ждем бесконечно долго, если очередь пуста)
        // и проверяем, что у нас получилось это сделать
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);
        furi_mutex_acquire(fourinrow_state->mutex, FuriWaitForever);
        // Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
        if(wincheck() != -1) {
            notification_message(notification, &end);
            furi_delay_ms(1000);
            if(wincheck() == 1) {
                scoreX++;
            }
            if(wincheck() == 2) {
                scoreO++;
            }
            init();
        }

        if(event.type == InputTypePress) {
            if(event.key == InputKeyOk) {
                int nh = next_height(cursorx);
                if(nh != -1) {
                    matrix[nh][cursorx] = player;
                    player = 3 - player;
                }
            }
            if(event.key == InputKeyUp) {
                //cursory--;
            }
            if(event.key == InputKeyDown) {
                //cursory++;
            }
            if(event.key == InputKeyLeft) {
                if(cursorx > 0) {
                    cursorx--;
                }
            }
            if(event.key == InputKeyRight) {
                if(cursorx < 6) {
                    cursorx++;
                }
            }
            if(event.key == InputKeyBack) {
                break;
            }
        }
        view_port_update(view_port);
        furi_mutex_release(fourinrow_state->mutex);
    }

    // Clear notification
    notification_message_block(notification, &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);

    // Специальная очистка памяти, занимаемой очередью
    furi_message_queue_free(event_queue);

    // Чистим созданные объекты, связанные с интерфейсом
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_mutex_free(fourinrow_state->mutex);
    furi_record_close(RECORD_GUI);
    free(fourinrow_state);

    return 0;
}
