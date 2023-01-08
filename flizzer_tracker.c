#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <furi_hal.h>
#include <u8g2_glue.h>
#include <stm32wbxx_ll_tim.h>

/*
Fontname: -Raccoon-Fixed4x6-Medium-R-Normal--6-60-75-75-P-40-ISO10646-1
Copyright: 
Glyphs: 95/203
BBX Build Mode: 0
*/
const uint8_t u8g2_font_tom_thumb_4x6_tr[725] U8G2_FONT_SECTION("u8g2_font_tom_thumb_4x6_tr") =
	"_\0\2\2\2\3\3\4\4\3\6\0\377\5\377\5\0\0\352\1\330\2\270 \5\340\315\0!\6\265\310"
	"\254\0\42\6\213\313$\25#\10\227\310\244\241\206\12$\10\227\310\215\70b\2%\10\227\310d\324F\1"
	"&\10\227\310(\65R\22'\5\251\313\10(\6\266\310\251\62)\10\226\310\304\224\24\0*\6\217\312\244"
	"\16+\7\217\311\245\225\0,\6\212\310)\0-\5\207\312\14.\5\245\310\4/\7\227\310Ve\4\60"
	"\7\227\310-k\1\61\6\226\310\255\6\62\10\227\310h\220\312\1\63\11\227\310h\220\62X\0\64\10\227"
	"\310$\65b\1\65\10\227\310\214\250\301\2\66\10\227\310\315\221F\0\67\10\227\310\314TF\0\70\10\227"
	"\310\214\64\324\10\71\10\227\310\214\64\342\2:\6\255\311\244\0;\7\222\310e\240\0<\10\227\310\246\32"
	"d\20=\6\217\311l\60>\11\227\310d\220A*\1\77\10\227\310\314\224a\2@\10\227\310UC\3"
	"\1A\10\227\310UC\251\0B\10\227\310\250\264\322\2C\7\227\310\315\32\10D\10\227\310\250d-\0"
	"E\10\227\310\214\70\342\0F\10\227\310\214\70b\4G\10\227\310\315\221\222\0H\10\227\310$\65\224\12"
	"I\7\227\310\254X\15J\7\227\310\226\252\2K\10\227\310$\265\222\12L\7\227\310\304\346\0M\10\227"
	"\310\244\61\224\12N\10\227\310\244q\250\0O\7\227\310UV\5P\10\227\310\250\264b\4Q\10\227\310"
	"Uj$\1R\10\227\310\250\64V\1S\10\227\310m\220\301\2T\7\227\310\254\330\2U\7\227\310$"
	"W\22V\10\227\310$\253L\0W\10\227\310$\65\206\12X\10\227\310$\325R\1Y\10\227\310$U"
	"V\0Z\7\227\310\314T\16[\7\227\310\214X\16\134\10\217\311d\220A\0]\7\227\310\314r\4^"
	"\5\213\313\65_\5\207\310\14`\6\212\313\304\0a\7\223\310\310\65\2b\10\227\310D\225\324\2c\7"
	"\223\310\315\14\4d\10\227\310\246\245\222\0e\6\223\310\235\2f\10\227\310\246\264b\2g\10\227\307\35"
	"\61%\0h\10\227\310D\225\254\0i\6\265\310\244\1j\10\233\307f\30U\5k\10\227\310\304\264T"
	"\1l\7\227\310\310\326\0m\7\223\310<R\0n\7\223\310\250d\5o\7\223\310U\252\2p\10\227"
	"\307\250\244V\4q\10\227\307-\225d\0r\6\223\310\315\22s\10\223\310\215\70\22\0t\10\227\310\245"
	"\25\243\0u\7\223\310$+\11v\10\223\310$\65R\2w\7\223\310\244q\4x\7\223\310\244\62\25"
	"y\11\227\307$\225dJ\0z\7\223\310\254\221\6{\10\227\310\251\32D\1|\6\265\310(\1}\11"
	"\227\310\310\14RR\0~\6\213\313\215\4\0\0\0\4\377\377\0";

typedef enum {
	EventTypeTick,
	EventTypeInput,
} EventType;

typedef struct {
	EventType type;
	InputEvent input;
} HelloWorldEvent;

typedef struct 
{
	bool stop;
	uint32_t counter;
	uint32_t counter_2;
} DirectDraw;

#define FURI_HAL_SPEAKER_TIMER TIM2
#define FURI_HAL_SPEAKER_CHANNEL LL_TIM_CHANNEL_CH1

void timer_draw_callback(void* ctx)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM2)) 
	{
		LL_TIM_ClearFlag_UPDATE(TIM2);
	}

	DirectDraw* instance = (DirectDraw*)ctx;

	instance->counter++;
	//return;
}

void timer_draw_callback_2(void* ctx) 
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM1)) 
	{
		LL_TIM_ClearFlag_UPDATE(TIM1);
	}

	DirectDraw* instance = (DirectDraw*)ctx;

	instance->counter_2++;
	//return;
}

static void draw_callback(Canvas* canvas, void* ctx) 
{
	DirectDraw* instance = (DirectDraw*)ctx;

	char buffer[20] = {0};

	snprintf(buffer, 20, "FRAMES: %ld", instance->counter);

	canvas_clear(canvas);

	canvas_set_custom_font(canvas, u8g2_font_tom_thumb_4x6_tr);
	
	canvas_draw_str(canvas, 0, 10, buffer);
}

static void input_callback(InputEvent* input_event, void* ctx) 
{
	// Проверяем, что контекст не нулевой
	furi_assert(ctx);
	FuriMessageQueue* event_queue = ctx;

	HelloWorldEvent event = {.type = EventTypeInput, .input = *input_event};
	furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void direct_draw_run(DirectDraw* instance) 
{
	vTaskPrioritySet(furi_thread_get_current_id(), FuriThreadPriorityIdle);

	furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdTim1UpTim16, 14, timer_draw_callback_2, (void*)instance);

	LL_TIM_InitTypeDef TIM_InitStruct = {0};
	// Prescaler to get 1kHz clock
	TIM_InitStruct.Prescaler = 32768;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	// Auto reload to get freq Hz interrupt
	TIM_InitStruct.Autoreload = 2500;

	//TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM1, &TIM_InitStruct);

	LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
	TIM_OC_InitStruct.CompareValue = 127;
	TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
	LL_TIM_OC_Init(TIM1, FURI_HAL_SPEAKER_CHANNEL, &TIM_OC_InitStruct);

	LL_TIM_EnableIT_UPDATE(TIM1);
	LL_TIM_EnableAllOutputs(TIM1);
	LL_TIM_EnableCounter(TIM1);

	furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdTIM2, 15, timer_draw_callback, (void*)instance);

	LL_TIM_InitTypeDef TIM_InitStruct2 = {0};
	// Prescaler to get 1kHz clock
	TIM_InitStruct2.Prescaler = 32768;
	TIM_InitStruct2.CounterMode = LL_TIM_COUNTERMODE_UP;
	// Auto reload to get freq Hz interrupt
	TIM_InitStruct2.Autoreload = 200;
	TIM_InitStruct2.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM2, &TIM_InitStruct2);
	LL_TIM_EnableIT_UPDATE(TIM2);
	LL_TIM_EnableAllOutputs(TIM2);
	LL_TIM_EnableCounter(TIM2);

	///

	bool unu = furi_hal_speaker_acquire(1000);
	UNUSED(unu);

	LL_TIM_InitTypeDef TIM_InitStruct3 = {0};
	//TIM_InitStruct.Prescaler = 4;
	TIM_InitStruct3.Prescaler = 200;
	TIM_InitStruct3.Autoreload =
		255; //in this fork used purely as PWM timer, the DMA now is triggered by SAMPLE_RATE_TIMER
	LL_TIM_Init(TIM16, &TIM_InitStruct3);

	LL_TIM_OC_InitTypeDef TIM_OC_InitStruct2 = {0};
	TIM_OC_InitStruct2.OCMode = LL_TIM_OCMODE_PWM1;
	TIM_OC_InitStruct2.OCState = LL_TIM_OCSTATE_ENABLE;
	TIM_OC_InitStruct2.CompareValue = 127;
	LL_TIM_OC_Init(TIM16, FURI_HAL_SPEAKER_CHANNEL, &TIM_OC_InitStruct2);
	LL_TIM_EnableAllOutputs(TIM16);
	LL_TIM_EnableCounter(TIM16);
}

int32_t flizzer_tracker_app(void* p) 
{
	UNUSED(p);

	// Текущее событие типа кастомного типа HelloWorldEvent
	HelloWorldEvent event;
	// Очередь событий на 8 элементов размера HelloWorldEvent
	FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(HelloWorldEvent));

	DirectDraw* instance = malloc(sizeof(DirectDraw));

	direct_draw_run(instance);

	// Создаем новый view port
	ViewPort* view_port = view_port_alloc();
	// Создаем callback отрисовки, без контекста
	view_port_draw_callback_set(view_port, draw_callback, instance);
	// Создаем callback нажатий на клавиши, в качестве контекста передаем
	// нашу очередь сообщений, чтоб запихивать в неё эти события
	view_port_input_callback_set(view_port, input_callback, event_queue);

	// Создаем GUI приложения
	Gui* gui = furi_record_open(RECORD_GUI);
	// Подключаем view port к GUI в полноэкранном режиме
	gui_add_view_port(gui, view_port, GuiLayerFullscreen);

	

	// Бесконечный цикл обработки очереди событий
	while(1) 
	{
		// Выбираем событие из очереди в переменную event (ждем бесконечно долго, если очередь пуста)
		// и проверяем, что у нас получилось это сделать
		furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

		// Наше событие — это нажатие кнопки
		if(event.type == EventTypeInput) 
		{
			// Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
			if(event.input.key == InputKeyBack) 
			{
				break;
			}
		}
	}

	furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdTIM2, 14, NULL, NULL);
	furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdTim1UpTim16, 15, NULL, NULL);

	FURI_CRITICAL_ENTER();
	LL_TIM_DeInit(TIM1);
	LL_TIM_DeInit(TIM2);
	LL_TIM_DeInit(TIM16);
	FURI_CRITICAL_EXIT();
	
	furi_hal_speaker_release();

	free(instance);
	
	// Специальная очистка памяти, занимаемой очередью
	furi_message_queue_free(event_queue);

	// Чистим созданные объекты, связанные с интерфейсом
	gui_remove_view_port(gui, view_port);
	view_port_free(view_port);
	furi_record_close(RECORD_GUI);

	return 0;
}