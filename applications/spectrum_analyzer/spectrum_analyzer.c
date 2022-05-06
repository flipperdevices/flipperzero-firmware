#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include "spectrum_analyzer.h"

void spectrum_analyzer_draw_scale(Canvas* canvas, const SpectrumAnalyzerModel* model) {

    // Draw line
    canvas_draw_line(canvas, FREQ_START_X, FREQ_BOTTOM_Y, FREQ_START_X + FREQ_LENGTH_X, FREQ_BOTTOM_Y);
    // Draw minor scale
    for(int i = FREQ_START_X; i < FREQ_START_X + FREQ_LENGTH_X ; i += 5) {
        canvas_draw_line(canvas, i, FREQ_BOTTOM_Y, i, FREQ_BOTTOM_Y + 2);
    }
    // Draw major scale
    for(int i = FREQ_START_X; i < FREQ_START_X + FREQ_LENGTH_X ; i += 25) {
        canvas_draw_line(canvas, i, FREQ_BOTTOM_Y, i, FREQ_BOTTOM_Y + 4);
    }

    // Draw scale tags
    uint16_t tag_left;
    uint16_t tag_center;
    uint16_t tag_right;
    char temp_str[18];

    tag_center = model->freq;

    switch (model->width) {
        case NARROW:
            tag_left = model->freq - 2;
            tag_right = model->freq + 2;
            break;
        case ULTRAWIDE:
            tag_left = model->freq - 40;
            tag_right = model->freq + 40;
            break;
        default:
            tag_left = model->freq - 10;
            tag_right = model->freq + 10;
    }

    canvas_set_font(canvas, FontSecondary);
    snprintf(temp_str, 18, "%u", tag_left);
    canvas_draw_str_aligned(canvas, FREQ_START_X, 63, AlignCenter, AlignBottom, temp_str);
    snprintf(temp_str, 18, "%u", tag_center);
    canvas_draw_str_aligned(canvas, 128/2, 63, AlignCenter, AlignBottom, temp_str);
    snprintf(temp_str, 18, "%u", tag_right);
    canvas_draw_str_aligned(canvas, FREQ_START_X + FREQ_LENGTH_X - 1, 63, AlignCenter, AlignBottom, temp_str);
}

static void spectrum_analyzer_render_callback(Canvas* const canvas, void* ctx) {
    const SpectrumAnalyzerModel* model = acquire_mutex((ValueMutex*)ctx, 25);
    FURI_LOG_T("Spectrum", "mutex acquired");
    if(model == NULL) {
        return;
    }
    
    spectrum_analyzer_draw_scale(canvas, model);
    
    for (uint8_t column = 0; column < 128; column++) { 
     
        uint8_t ss = model->chan_table[column + 2].ss;
        // Compress height to max of 64 values (255>>2)
        uint8_t s = MAX((ss - model->vscroll)>>2,0);
        uint8_t y = FREQ_BOTTOM_Y - s; // bar height

        // Draw each bar
        canvas_draw_line(canvas, column, FREQ_BOTTOM_Y, column, y);    
    }

    // Draw cross and label
    if (model->max_rssi > PEAK_THRESHOLD) {
        // Compress height to max of 64 values (255>>2)
        uint8_t max_y = MAX((model->max_rssi_dec - model->vscroll)>>2,0);
        max_y =  (FREQ_BOTTOM_Y - max_y);

        // Cross
        canvas_draw_line(canvas, model->max_rssi_channel - 2 - 2 , max_y - 2, model->max_rssi_channel - 2 + 2, max_y + 2);    
        canvas_draw_line(canvas, model->max_rssi_channel - 2 + 2 , max_y - 2, model->max_rssi_channel - 2 - 2, max_y + 2);    

        // Label
        char temp_str[36];
        snprintf(temp_str, 36, "Peak: %3.2f Mhz %3.1f dbm", ((float)model->chan_table[model->max_rssi_channel].frequency / 1000000), model->max_rssi);
        canvas_draw_str_aligned(canvas, 127, 0, AlignRight, AlignTop, temp_str);

        // FURI_LOG_D("Spectrum", "max_ss %u -  %d", max_ss, max_rssi);

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
    uint8_t ch = 0; 

    for (ch = 0; ch < NUM_CHANNELS-1; ch++) { 
        model->chan_table[ch].ss = 0;
    }

    model->max_rssi_dec = 0;
    model->max_rssi_channel = 0;
    model->max_rssi = 0;

    model->update_values_flag = false;

    model->freq = DEFAULT_FREQ;
    model->band = BAND_400;
    model->width = WIDE;
    model->vscroll = DEFAULT_VSCROLL;
    model->min_chan = 0;
    model->max_chan = NUM_CHANNELS - 1;

} 

/* set the channel bandwidth */
void set_filter(SpectrumAnalyzerModel* model) {

    uint8_t filter_config[2][2] = {
        {CC1101_MDMCFG4, 0},
        {0, 0},
    };

	/* channel spacing should fit within 80% of channel filter bandwidth */
	switch (model->width) {
	case NARROW:
        filter_config[0][1] = 0xFC; /* 39.2 kHz / .8 = 49 kHz --> 58 kHz */
		break;
	case ULTRAWIDE:
        filter_config[0][1] = 0x0C; /* 784 kHz / .8 = 980 kHz --> 812 kHz */
		break;
	default:
        filter_config[0][1] = 0x6C; /* 196 kHz / .8 = 245 kHz --> 270 kHz */
		break;
	}

    furi_hal_subghz_load_registers(filter_config);

}

/* set the center frequency in MHz */
uint16_t setup_frequencies(SpectrumAnalyzerModel* model) {

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

    switch (model->width) {
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
            model->freq = ((model->freq + 10) / 20) * 20;
            break;
        default:
            margin = WIDE_MARGIN;
            step = WIDE_STEP;
            spacing = WIDE_SPACING;

            /* nearest 5 MHz step */
            model->freq = ((model->freq + 2) / 5) * 5;
            break;
    }

    /* handle cases near edges of bands */
    if (model->freq > EDGE_900) {
        new_band = BAND_900;
        upper_limit = UPPER(MAX_900, margin, step);
        lower_limit = LOWER(MIN_900, margin, step);
        next_up = LOWER(MIN_300, margin, step);
        next_down = UPPER(MAX_400, margin, step);
        next_band_up = BAND_300;
        next_band_down = BAND_400;
    } else if (model->freq > EDGE_400) {
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

    if (model->freq > upper_limit) {
        model->freq = upper_limit;
        if (new_band == model->band) {
            new_band = next_band_up;
            model->freq = next_up;
        }
    } else if (model->freq < lower_limit) {
        model->freq = lower_limit;
        if (new_band == model->band) {
            new_band = next_band_down;
            model->freq = next_down;
        }
    }

    model->band = new_band;

    /* doing everything in Hz from here on */
    switch (model->band) {
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
    hz = model->freq * 1000000;
    model->max_chan = NUM_CHANNELS / 2;
    while (hz <= max_hz && model->max_chan < NUM_CHANNELS) {
        model->chan_table[model->max_chan].frequency = hz;

        FURI_LOG_T("Spectrum", "calibrate_freq ch[%u]: %lu", model->max_chan, hz);

        hz += spacing;
        model->max_chan++;
    }

    /* calibrate lower channels */
    hz = model->freq * 1000000 - spacing;
    model->min_chan = NUM_CHANNELS / 2;
    while (hz >= min_hz && model->min_chan > 0) {
        model->min_chan--;
        model->chan_table[model->min_chan].frequency = hz;

        FURI_LOG_T("Spectrum", "calibrate_freq ch[%u]: %lu", model->min_chan, hz);

        hz -= spacing;
    }

    model->max_rssi = -200.0;
    model->max_rssi_dec = 0;

    FURI_LOG_D("Spectrum", "setup_frequencies - max_hz: %u - min_hz: %u - spacing: %u Hz", max_hz, min_hz, spacing);
    FURI_LOG_D("Spectrum", "ch[0]: %lu - ch[%u]: %lu", model->chan_table[0].frequency, NUM_CHANNELS - 1, model->chan_table[NUM_CHANNELS - 1].frequency);

    // center_freq = model->freq;

    set_filter(model);

    return model->freq;
}

void spectrum_analyzer_thread(void* p) {
    SpectrumAnalyzerContext* context = (SpectrumAnalyzerContext*)p;

    // Start CC1101
    furi_hal_subghz_reset();
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);
    furi_hal_subghz_set_frequency(433920000);
    furi_hal_subghz_flush_rx();
    furi_hal_subghz_rx();

    static const uint8_t radio_config[][2] = {
        {CC1101_FSCTRL1,0x12},
        {CC1101_FSCTRL0,0x00},

        {CC1101_AGCCTRL2, 0xC0},
        
        {CC1101_MDMCFG4, 0x6C},
        {CC1101_TEST2, 0x88},
        {CC1101_TEST1, 0x31},
        {CC1101_TEST0, 0x09},
        /* End  */
        {0, 0},
    };

    while(1) {

        float		local_max_rssi;
        uint8_t		local_max_rssi_dec;
        uint8_t		local_max_rssi_channel;
        ChannelInfo local_chan_table[NUM_CHANNELS];

        furi_hal_delay_ms(50);

        // Thread hangs if using FURI_LOG_X...
        // FURI_LOG_D("Spectrum", "spectrum_analyzer_thread: mutex acquired");

        furi_hal_subghz_idle();
        furi_hal_subghz_load_registers(radio_config);
        
        // Generate local copy
        SpectrumAnalyzerModel* model = (SpectrumAnalyzerModel*)acquire_mutex(context->model_mutex, 25);
        if (model->update_values_flag == true){
            model->freq = setup_frequencies(model);
            model->update_values_flag = false;
        }
        memcpy(&local_chan_table, model->chan_table, sizeof(ChannelInfo) * NUM_CHANNELS);
        release_mutex(context->model_mutex, model);

        local_max_rssi_dec = 0;

        for (uint8_t ch = 0; ch < NUM_CHANNELS-1; ch++) { 
            furi_hal_subghz_set_frequency(local_chan_table[ch].frequency);

            furi_hal_subghz_rx();
            furi_hal_delay_ms(3);
            
            //max_ss = 127 -> -10.5
            //max_ss = 0 -> -74
            //max_ss = 255 -> -74.5
            //max_ss = 128 -> -138

            local_chan_table[ch].ss = (furi_hal_subghz_get_rssi() + 138) * 2 ;

            if (local_chan_table[ch].ss > local_max_rssi_dec) {
                local_max_rssi_dec = local_chan_table[ch].ss;
                local_max_rssi = (local_chan_table[ch].ss / 2) - 138;
                local_max_rssi_channel = ch;
            }

            furi_hal_subghz_idle();
        }  

        // Update Model
        model = (SpectrumAnalyzerModel*)acquire_mutex(context->model_mutex, 25);
        memcpy(model->chan_table, &local_chan_table, sizeof(ChannelInfo) * NUM_CHANNELS);
        model->max_rssi = local_max_rssi;
        model->max_rssi_dec = local_max_rssi_dec;
        model->max_rssi_channel = local_max_rssi_channel;
        release_mutex(context->model_mutex, model);
    }
}

int32_t spectrum_analyzer_app(void* p) { 
    UNUSED(p);

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

    setup_frequencies(model);

    // start scan thread
    osThreadAttr_t thread_attr = {.name = "spectrum_analyzer_thread", .stack_size = 2048};
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

            uint8_t vstep = VERTICAL_SHORT_STEP;
            uint8_t hstep;
            switch (model->width) {
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
                        model->vscroll = MAX(model->vscroll - vstep, MIN_VSCROLL);
                        FURI_LOG_D("Spectrum", "Vscroll: %u", model->vscroll);
                        break; 
                    case InputKeyDown: 
                        model->vscroll = MIN(model->vscroll + vstep, MAX_VSCROLL);
                        FURI_LOG_D("Spectrum", "Vscroll: %u", model->vscroll);
                        break; 
                    case InputKeyRight: 
                        model->freq += hstep;
                        FURI_LOG_D("Spectrum", "freq: %lu", model->freq);
                        model->update_values_flag = true;
                        break; 
                    case InputKeyLeft:  
                        model->freq -= hstep;
                        FURI_LOG_D("Spectrum", "freq: %lu", model->freq);
                        model->update_values_flag = true;
                        break; 
                    case InputKeyOk: 
                        {
                            switch (model->width) {
                                case WIDE:
                                    model->width = NARROW;
                                    break;
                                case NARROW:
                                    model->width = ULTRAWIDE;
                                    break;
                                case ULTRAWIDE:
                                default:
                                    model->width = WIDE;
                            }
                        }
                        model->update_values_flag = true;
                        FURI_LOG_D("Spectrum", "Width: %u", model->width);
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
        release_mutex(&model_mutex, model);
        view_port_update(view_port);
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