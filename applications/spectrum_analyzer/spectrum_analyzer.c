#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include "spectrum_analyzer.h"

void spectrum_analyzer_draw_scale(Canvas* canvas, const SpectrumAnalyzerModel* model) {

#define FREQ_BOTTOM_Y 50
#define FREQ_START_X 0
#define FREQ_LENGTH_X 127

    // Draw line
    canvas_draw_line(canvas, FREQ_START_X, FREQ_BOTTOM_Y, FREQ_START_X + FREQ_LENGTH_X, FREQ_BOTTOM_Y);

    // Draw minor scale
    for(int i = FREQ_START_X; i < FREQ_START_X + FREQ_LENGTH_X ; i += 4) {
        canvas_draw_line(canvas, i, FREQ_BOTTOM_Y, i, FREQ_BOTTOM_Y + 2);
    }
    // Draw major scale
    for(int i = FREQ_START_X; i < FREQ_START_X + FREQ_LENGTH_X ; i += 16) {
        canvas_draw_line(canvas, i, FREQ_BOTTOM_Y, i, FREQ_BOTTOM_Y + 4);
    }

    // Draw scale tags
    uint16_t tag_left;
    uint16_t tag_center;
    uint16_t tag_right;
    char temp_str[18];

    tag_center = user_freq;

    switch (width) {
        case NARROW:
            tag_left = user_freq - 2;
            tag_right = user_freq + 2;
            break;
        case ULTRAWIDE:
            tag_left = user_freq - 40;
            tag_right = user_freq + 40;
            break;
        default:
            tag_left = user_freq - 10;
            tag_right = user_freq + 10;
    }

    canvas_set_font(canvas, FontSecondary);
    snprintf(temp_str, 18, "%u", tag_left);
    canvas_draw_str_aligned(canvas, 0, 63, AlignLeft, AlignBottom, temp_str);
    snprintf(temp_str, 18, "%u", tag_center);
    canvas_draw_str_aligned(canvas, 128/2, 63, AlignCenter, AlignBottom, temp_str);
    snprintf(temp_str, 18, "%u", tag_right);
    canvas_draw_str_aligned(canvas, 127, 63, AlignRight, AlignBottom, temp_str);
}

static void spectrum_analyzer_render_callback(Canvas* const canvas, void* ctx) {
    const SpectrumAnalyzerModel* model = acquire_mutex((ValueMutex*)ctx, 25);
    FURI_LOG_T("Spectrum", "mutex acquired");
    if(model == NULL) {
        return;
    }
    
    spectrum_analyzer_draw_scale(canvas, model);
    
    ; 
    for (uint8_t channel = 0; channel < FREQ_START_X + FREQ_LENGTH_X; channel++) { 
    
        uint8_t ss = model->chan_table[channel].ss;
        uint8_t s;

        if(height == VERTICAL_TALL){
            s = MAX((ss - vscroll)>>1,0);
        }else{
            s = MAX((ss - vscroll)>>2,0);
        }

        uint8_t y =  (FREQ_BOTTOM_Y - s); // bar height
        canvas_draw_line(canvas, channel, FREQ_BOTTOM_Y, channel, y + 2);    
    }

    release_mutex((ValueMutex*)ctx, model);
    FURI_LOG_T("Spectrum", "mutex released");
}

static void spectrum_analyzer_input_callback(InputEvent* input_event, osMessageQueueId_t event_queue) {
    furi_assert(event_queue); 

    SpectrumAnalyzerEvent event = {.type = EventTypeKey, .input = *input_event};
    osMessageQueuePut(event_queue, &event, 0, osWaitForever);
}

static void spectrum_analyzer_model_init(SpectrumAnalyzerModel* const model) {
    model->chan_table[0].ss = 0;
} 

/* freq in Hz */
void calibrate_freq(SpectrumAnalyzerModel* model, uint32_t freq, uint8_t ch) {
    model->chan_table[ch].frequency = freq;
}

/* set the center frequency in MHz */
uint16_t set_center_freq(SpectrumAnalyzerModel* model, uint16_t freq) {

	uint8_t new_band;
	uint32_t spacing;
	uint32_t hz;
	uint32_t min_hz;
	uint32_t max_hz;
	uint8_t margin;
	uint8_t step;
	uint16_t upper_limit;
	uint16_t lower_limit;
	uint16_t next_up;
	uint16_t next_down;
	uint8_t next_band_up;
	uint8_t next_band_down;
	// uint8_t i;

	switch (width) {
        case NARROW:
            margin = NARROW_MARGIN;
            step = NARROW_STEP;
            spacing = NARROW_SPACING;
            break;
        case ULTRAWIDE:
            margin = ULTRAWIDE_MARGIN;
            step = ULTRAWIDE_STEP;
            spacing = ULTRAWIDE_SPACING;

            /* nearest 20 MHz step */
            freq = ((freq + 10) / 20) * 20;
            break;
        default:
            margin = WIDE_MARGIN;
            step = WIDE_STEP;
            spacing = WIDE_SPACING;

            /* nearest 5 MHz step */
            freq = ((freq + 2) / 5) * 5;
            break;
	}

	/* handle cases near edges of bands */
	if (freq > EDGE_900) {
		new_band = BAND_900;
		upper_limit = UPPER(MAX_900, margin, step);
		lower_limit = LOWER(MIN_900, margin, step);
		next_up = LOWER(MIN_300, margin, step);
		next_down = UPPER(MAX_400, margin, step);
		next_band_up = BAND_300;
		next_band_down = BAND_400;
	} else if (freq > EDGE_400) {
		new_band = BAND_400;
		upper_limit = UPPER(MAX_400, margin, step);
		lower_limit = LOWER(MIN_400, margin, step);
		next_up = LOWER(MIN_900, margin, step);
		next_down = UPPER(MAX_300, margin, step);
		next_band_up = BAND_900;
		next_band_down = BAND_300;
	} else {
		new_band = BAND_300;
		upper_limit = UPPER(MAX_300, margin, step);
		lower_limit = LOWER(MIN_300, margin, step);
		next_up = LOWER(MIN_400, margin, step);
		next_down = UPPER(MAX_900, margin, step);
		next_band_up = BAND_400;
		next_band_down = BAND_900;
	}

	if (freq > upper_limit) {
		freq = upper_limit;
		if (new_band == band) {
			new_band = next_band_up;
			freq = next_up;
		}
	} else if (freq < lower_limit) {
		freq = lower_limit;
		if (new_band == band) {
			new_band = next_band_down;
			freq = next_down;
		}
	}

	band = new_band;

	/* doing everything in Hz from here on */
	switch (band) {
	case BAND_400:
		min_hz = MIN_400 * 1000000;
		max_hz = MAX_400 * 1000000;
		break;
	case BAND_300:
		min_hz = MIN_300 * 1000000;
		max_hz = MAX_300 * 1000000;
		break;
	default:
		min_hz = MIN_900 * 1000000;
		max_hz = MAX_900 * 1000000;
		break;
	}

	/* calibrate upper channels */
	hz = freq * 1000000;
	max_chan = NUM_CHANNELS / 2;
	while (hz <= max_hz && max_chan < NUM_CHANNELS) {
		calibrate_freq(model, hz, max_chan);
		hz += spacing;
		// for (i = 0; i < persistence; i++)
		// 	model->chan_table[max_chan].ss[i] = 0;
		max_chan++;
	}

	/* calibrate lower channels */
	hz = freq * 1000000 - spacing;
	min_chan = NUM_CHANNELS / 2;
	while (hz >= min_hz && min_chan > 0) {
		min_chan--;
		calibrate_freq(model, hz, min_chan);
		// for (i = 0; i < persistence; i++)
		// 	model->chan_table[min_chan].ss[i] = 0;
		hz -= spacing;
	}

    FURI_LOG_D("Spectrum", "set_center_freq - max_hz: %u - min_hz: %u - spacing: %u", max_hz, min_hz, spacing);

	center_freq = freq;

	return freq;
}

void spectrum_analyzer_thread(void* p) {
    SpectrumAnalyzerContext* context = (SpectrumAnalyzerContext*)p;

    // Start CC1101
    furi_hal_subghz_reset();
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);
    furi_hal_subghz_set_frequency(433920000);
    furi_hal_subghz_flush_rx();
    furi_hal_subghz_rx();

    while(1) {
        furi_hal_delay_ms(10);

        SpectrumAnalyzerModel* model = (SpectrumAnalyzerModel*)acquire_mutex(context->model_mutex, 25);
        // FURI_LOG_D("Spectrum", "spectrum_analyzer_thread: mutex acquired");

        furi_hal_subghz_idle();
        furi_hal_subghz_load_registers(radio_config);

        uint8_t ch = 0; 

        if (user_freq != center_freq){
            user_freq = set_center_freq(model, user_freq);
        }

        for (ch = 0; ch < NUM_CHANNELS-1; ch++) { 
            // tune(ch);
            furi_hal_subghz_set_frequency(model->chan_table[ch].frequency);

            furi_hal_subghz_rx();
            osDelay(3);
            
            // model->chan_table[ch].ss[sweep] = furi_hal_subghz_get_rssi_dec() ^ 0x80;
            model->chan_table[ch].ss = furi_hal_subghz_get_rssi_dec() ^ 0x80;
            
            furi_hal_subghz_idle();
        }  

        release_mutex(context->model_mutex, model);
    }

}

int32_t spectrum_analyzer_app(void* p) { 
    osMessageQueueId_t event_queue = osMessageQueueNew(8, sizeof(SpectrumAnalyzerEvent), NULL); 
    
    SpectrumAnalyzerModel* model = malloc(sizeof(SpectrumAnalyzerModel));
    spectrum_analyzer_model_init(model);

    ValueMutex model_mutex; 
    if (!init_mutex(&model_mutex, model, sizeof(SpectrumAnalyzerModel))) {
        FURI_LOG_E("Spectrum", "Cannot create mutex");
        free(model); 
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc(); 
    view_port_draw_callback_set(view_port, spectrum_analyzer_render_callback, &model_mutex);
    view_port_input_callback_set(view_port, spectrum_analyzer_input_callback, event_queue);
 
    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui"); 
    gui_add_view_port(gui, view_port, GuiLayerFullscreen); 

	center_freq = DEFAULT_FREQ;
	user_freq = DEFAULT_FREQ;
	band = BAND_400;
	width = WIDE;
	// max_hold = 1;
	height = VERTICAL_SHORT;
	// sleepy = 0;
	vscroll = 0;
	min_chan = 0;
	max_chan = NUM_CHANNELS - 1;
	// sweep = 0;
	// persistence = PERSIST;

	set_center_freq(model, center_freq);

    // start scan thread
    osThreadAttr_t thread_attr = {.name = "spectrum_analyzer_thread", .stack_size = 1024};
    SpectrumAnalyzerContext context = {.model_mutex = &model_mutex, .event_queue = event_queue};
    osThreadId_t analyzer_thread = osThreadNew(spectrum_analyzer_thread, &context, &thread_attr);

    if(analyzer_thread == NULL) {
        FURI_LOG_E("Spectrum", "Cannot create thread");
        return 255;
    }

    SpectrumAnalyzerEvent event; 
    for(bool processing = true; processing;) { 
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, 100);
        model = (SpectrumAnalyzerModel*)acquire_mutex_block(&model_mutex);

        if(event_status == osOK) {

            // FURI_LOG_D("Spectrum", "Main Loop - event: %u", event.type);

            uint8_t vstep = (height == VERTICAL_TALL) ? VERTICAL_TALL_STEP : VERTICAL_SHORT_STEP;
            uint8_t hstep;
            switch (width) {
            case NARROW:
                hstep = NARROW_STEP;
                break;
            case ULTRAWIDE:
                hstep = ULTRAWIDE_STEP;
                break;
            default:
                hstep = WIDE_STEP;
                break;
            }

            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {  
                    switch(event.input.key) {
                    case InputKeyUp: 
                        vscroll = MAX(vscroll - vstep, MIN_VSCROLL);
                        FURI_LOG_D("Spectrum", "Vscroll: %u", vscroll);
                        break; 
                    case InputKeyDown: 
                        vscroll = MIN(vscroll + vstep, MAX_VSCROLL);
                        FURI_LOG_D("Spectrum", "Vscroll: %u", vscroll);
                        break; 
                    case InputKeyRight: 
                        user_freq += hstep;
                        break; 
                    case InputKeyLeft:  
                        user_freq -= hstep;
                        break; 
                    case InputKeyOk: 
                        {
                            switch (width) {
                                case WIDE:
                                    width = ULTRAWIDE;
                                    break;
                                case NARROW:
                                    width = WIDE;
                                    break;
                                case ULTRAWIDE:
                                default:
                                    width = NARROW;
                            }
                        }
                        FURI_LOG_D("Spectrum", "width: %u", width);
                        break;
                    case InputKeyBack: 
                        processing = false;
                        break;
                    }
                }
            } 
        } else {
            FURI_LOG_T("spectrum", "osMessageQueue: event timeout");
            // event timeout
        }

        FURI_LOG_T("Spectrum", "Vscroll: %u", vscroll);

        view_port_update(view_port);
        release_mutex(&model_mutex, model);
    }
    
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    
    osThreadTerminate(analyzer_thread);
    free(model);

    furi_hal_subghz_idle();
    furi_hal_subghz_sleep();
    
    osMessageQueueDelete(event_queue); 
    delete_mutex(&model_mutex);

    return 0;
}
