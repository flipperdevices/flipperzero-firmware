В этом примере мы посмотрим, как обмениваться данными между приложениями.
Как мы уже знаем, для обмена данными испольузются FURI Record: одно приложение создает запись, а другое открывает ее по имени.

В этом примере мы сымитируем работу с дисплеем: одно приложение, назовем его `application_ipc_display` будет выполнять роль драйвера экрана, а второе приложение `application_ipc_widget` будет рисовать простую демку.

# Dsiplay definition

Работа с экраном будет осуществляться путем записи "пикселей" во фреймбуфер.

Для начала, создадим фреймбуфер

```C
#define FB_WIDTH 10
#define FB_HEIGHT 3
#define FB_SIZE (FB_WIDTH * FB_HEIGHT)

char _framebuffer[FB_SIZE];

for(size_t i = 0; i < FB_SIZE; i++) {
    _framebuffer[i] = ' ';
}
```

На компе просто будем его рисовать:
```
    +==========+
    |          |
    |          |
    |          |
    +==========+
```

```C
fuprintf(log, "+==========+\n");
for(uint8_t i = 0; i < FB_HEIGHT; i++) {
    strncpy(row_buffer, &fb[FB_WIDTH * i], FB_WIDTH);
    fuprintf(log, "|%s|\n", row_buffer);
}
fuprintf(log, "+==========+\n");
```

_Замечание: после добавления эмуляции дисплея этот пример будет работать с настоящим 128×64 фреймбуфером._

# Demo "widget" application

Приложение открывает запись, содержащую фреймбуфер

```C
FuriRecordSubscriber* fb_record = furi_open(
    "test_fb", false, false, NULL, NULL, NULL
);
```

Далее оно каждые 120 мс очищает экран и записывает во фреймбуфер "пиксель", перемещая его по экрану. Для этого необходимо захватить фреймбуфер:

`char* fb = (char*)furi_take(fb_record);`

Записать в него данные:

```C
if(fb == NULL) furiac_exit(NULL);

for(size_t i = 0; i < FB_SIZE; i++) {
    fb[i] = ' ';
}

fb[counter % FB_SIZE] = '#';
```

И отпустить фреймбуфер:

`furi_commit(fb_record);`

`counter` увеличивается на каждой итерации, создавая перемещение "пикселя".

# Display driver

Приложение драйвера экрана после старта инициализирует фреймбуфер и создает новую запись, помещая в нее ссылку на буфер:

`furi_create("test_fb", (void*)_framebuffer, FB_SIZE)`

Затем он открывает эту запись и подписывается на изменения

```
FuriRecordSubscriber* fb_record = furi_open(
    "test_fb", false, false, handle_fb_change, NULL, &ctx
);
```

Теперь при записи в фреймбуфер (когда в приложении `application_ipc_widget` вызывается функция `furi_commit`) вызывается обработчик:

```C
static void handle_fb_change(const void* fb, size_t fb_size, void* raw_ctx) {
    IpcCtx* ctx = (IpcCtx*)raw_ctx; // make right type

    fuprintf(ctx->log, "[cb] framebuffer updated\n");

    // send event to app thread
    xSemaphoreGive(ctx->events);

    // Attention! Please, do not make blocking operation like IO and waits inside callback
    // Remember that callback execute in calling thread/context
}
```

That callback execute in calling thread/context, поэтому обработчик с помощью семафора сообщает основному потоку приложения о том, что фреймбуфер обновился. Основной поток ожидает семафор и при получении события выполняет "отрисовку":

```C
if(xSemaphoreTake(events, portMAX_DELAY) == pdTRUE) {
    fuprintf(log, "[display] get fb update\n\n");

    #ifdef HW_DISPLAY
    // on Flipper target draw the screen
    #else
    // on local target just print
    {
        void* fb = furi_take(fb_record);
        print_fb((char*)fb, log);
        furi_give(fb_record);
    }
    #endif
}
```

Для того, чтобы callback получил доступ к семафору, используется структура, содержащая контекст. Эта структура передается в виде аргумента в `furi_open` и попадает в обработчик:

```C
typedef struct {
    SemaphoreHandle_t events; // queue to pass events from callback to app thread
    FuriRecordSubscriber* log; // app logger
} IpcCtx;
```

Нам остается только создать семафор и сформировать контекст:

```C
StaticSemaphore_t event_descriptor;
// create stack-based counting semaphore
SemaphoreHandle_t events = xSemaphoreCreateCountingStatic(255, 0, &event_descriptor);

IpcCtx ctx = {.events = events, .log = log};
```

Вы можете найти полный текст примера в `applications/examples/ipc.c`, для запуска выполните `docker-compose exec dev make -C target_lo example_ipc`.

![]()
