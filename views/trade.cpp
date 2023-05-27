#include "../pokemon_app.h"
#include "trade.hpp"

#include "../pokemon_data.h"

uint8_t out_data = 0;
uint8_t in_data = 0;
uint8_t shift = 0;
uint32_t time = 0;
volatile int counter = 0;
volatile bool procesing = true;
volatile connection_state_t connection_state = NOT_CONNECTED;
volatile trade_centre_state_t trade_centre_state = INIT;

void screen_gameboy_connect(Canvas* const canvas) {
    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_icon(canvas, 1, 21, &I_Connect_me_62x31);
    canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);
    canvas_draw_icon(canvas, 80, 0, &I_game_boy);
    canvas_draw_icon(canvas, 8, 2, &I_Space_65x18);
    canvas_draw_str(canvas, 18, 13, "Connect GB");
}
void screen_gameboy_connected(Canvas* const canvas) {
    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_icon(canvas, 1, 21, &I_Connected_62x31);
    canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);
    canvas_draw_icon(canvas, 80, 0, &I_game_boy);
    canvas_draw_icon(canvas, 8, 2, &I_Space_65x18);
    canvas_draw_str(canvas, 18, 13, "Connected!");
}

int time_in_seconds = 0;

static void trade_draw_callback(Canvas* canvas, void* context) {
    const char* gameboy_status_text = NULL;

    canvas_clear(canvas);
    SelectPokemonModel* model = (SelectPokemonModel*)context;
    if(!model->trading) {
        if(!model->connected) {
            furi_hal_light_set(LightGreen, 0x00);
            furi_hal_light_set(LightBlue, 0x00);
            furi_hal_light_set(LightRed, 0xff);
            screen_gameboy_connect(canvas);
        } else {
            furi_hal_light_set(LightGreen, 0xff);
            furi_hal_light_set(LightBlue, 0x00);
            furi_hal_light_set(LightRed, 0x00);
            screen_gameboy_connected(canvas);
        }
    } else {
        switch(model->gameboy_status) {
        case GAMEBOY_TRADING:
            furi_hal_light_set(LightGreen, 0x00);
            furi_hal_light_set(LightRed, 0x00);
            if(time_in_seconds % 2 == 1) {
                furi_hal_light_set(LightBlue, 0xff);
                canvas_draw_icon(canvas, 0, 0, &I_gb_step_1);
            } else {
                furi_hal_light_set(LightBlue, 0x00);
                canvas_draw_icon(canvas, 0, 0, &I_gb_step_2);
            }
            break;
        case GAMEBOY_READY:
        case GAMEBOY_WAITING:
        case GAMEBOY_SEND:
            canvas_draw_icon(canvas, 38, 11, pokemon_table[model->current_pokemon].icon);
            break;
        default:
            // Default state added to eliminated enum warning
            break;
        }
        canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);

        canvas_draw_frame(canvas, 0, 0, 128, 64);
        canvas_draw_icon(canvas, 24, 0, &I_Space_80x18);

        switch(model->gameboy_status) {
        case GAMEBOY_READY:
            gameboy_status_text = "READY";
            break;
        case GAMEBOY_WAITING:
            gameboy_status_text = "WAITING";
            break;
        case GAMEBOY_TRADE_READY:
            gameboy_status_text = "READY";
            break;
        case GAMEBOY_SEND:
            gameboy_status_text = "DEAL...";
            break;
        case GAMEBOY_PENDING:
            gameboy_status_text = "PENDING...";
            break;
        case GAMEBOY_TRADING:
            gameboy_status_text = "TRADING...";
            break;
        default:
            gameboy_status_text = "INITIAL";
            break;
        }

        canvas_draw_str(canvas, 48, 12, gameboy_status_text);

        canvas_draw_icon(canvas, 27, 1, &I_red_16x15);

        time_in_seconds = (int)DWT->CYCCNT / (72000000.0f / 4); //  250ms
    }
}

static bool trade_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    Trade* trade = (Trade*)context;
    bool consumed = false;

    if(event->type == InputTypePress && event->key == InputKeyBack) {
        view_dispatcher_switch_to_view(trade->app->view_dispatcher, AppViewSelectPokemon);
        consumed = true;
    }

    return consumed;
}

uint32_t micros() {
    return DWT->CYCCNT / 64;
}

/* Get the response byte from the link partner, updating the connection
 * state if needed.
 */
byte getConnectResponse(byte in) {
    byte ret;

    switch(in) {
    case PKMN_CONNECTED:
        connection_state = CONNECTED;
        ret = PKMN_CONNECTED;
        break;
    case PKMN_MASTER:
        ret = PKMN_SLAVE;
        break;
    case PKMN_BLANK:
        ret = PKMN_BLANK;
        break;
    default:
        connection_state = NOT_CONNECTED;
        ret = PKMN_BREAK_LINK;
        break;
    }

    return ret;
}

/* Figure out what the pokemon game is requesting and move to that mode.
 */
byte getMenuResponse(byte in) {
    /* TODO: Find out what this byte means */
    byte response = 0x00;

    switch(in) {
    case PKMN_CONNECTED:
        response = PKMN_CONNECTED;
        break;
    case PKMN_TRADE_CENTRE:
        connection_state = TRADE_CENTRE;
        break;
    case PKMN_COLOSSEUM:
        connection_state = COLOSSEUM;
        break;
    case PKMN_BREAK_LINK:
    case PKMN_MASTER:
        connection_state = NOT_CONNECTED;
        response = PKMN_BREAK_LINK;
        break;
    default:
        response = in;
        break;
    }

    return response;
}

byte getTradeCentreResponse(byte in, void* context) {
    UNUSED(context);
    Trade* trade = (Trade*)context;
    byte send = in;

    switch(trade_centre_state) {
    case INIT:
        // TODO: What does this value of in mean?
        if(in == 0x00) {
            // TODO: What does counter signify here?
            if(counter == 5) {
                trade_centre_state = READY_TO_GO;
                //  CLICK EN LA MESA
                with_view_model_cpp(
                    trade->view,
                    SelectPokemonModel*,
                    model,
                    { model->gameboy_status = GAMEBOY_READY; },
                    false);
            }
            counter++;
        }
        break;

    case READY_TO_GO:
        if((in & 0xF0) == 0xF0) trade_centre_state = SEEN_FIRST_WAIT;
        break;

    case SEEN_FIRST_WAIT:
        if((in & 0xF0) != 0xF0) {
            counter = 0;
            trade_centre_state = SENDING_RANDOM_DATA;
        }
        break;

    case SENDING_RANDOM_DATA:
        if((in & 0xF0) == 0xF0) {
            if(counter == 5) {
                trade_centre_state = WAITING_TO_SEND_DATA;
                with_view_model_cpp(
                    trade->view,
                    SelectPokemonModel*,
                    model,
                    { model->gameboy_status = GAMEBOY_WAITING; },
                    false);
            }
            counter++;
        }
        break;

    case WAITING_TO_SEND_DATA:
        if((in & 0xF0) != 0xF0) {
            counter = 0;
            INPUT_BLOCK[counter] = in;
            send = DATA_BLOCK[counter];
            counter++;
            trade_centre_state = SENDING_DATA;
        }
        break;

    case SENDING_DATA:
        INPUT_BLOCK[counter] = in;
        send = DATA_BLOCK[counter];
        counter++;
        if(counter == 405) //TODO: replace with sizeof struct rather than static number
            trade_centre_state = SENDING_PATCH_DATA;
        break;

    case SENDING_PATCH_DATA:
        if(in == 0xFD) {
            counter = 0;
            send = 0xFD;
        } else {
            counter++;
            if(counter == 197) // TODO: What is this magic value?
                trade_centre_state = TRADE_PENDING;
        }
        break;

    case TRADE_PENDING:
        /* TODO: What are these states */
        if(in == 0x6F) {
            trade_centre_state = READY_TO_GO;
            send = 0x6F;
            with_view_model_cpp(
                trade->view,
                SelectPokemonModel*,
                model,
                { model->gameboy_status = GAMEBOY_TRADE_READY; },
                false);
        } else if((in & 0x60) == 0x60) {
            send = 0x60; // first pokemon
            with_view_model_cpp(
                trade->view,
                SelectPokemonModel*,
                model,
                { model->gameboy_status = GAMEBOY_SEND; },
                false);
        } else if(in == 0x00) {
            send = 0;
            trade_centre_state = TRADE_CONFIRMATION;
        }
        break;

    case TRADE_CONFIRMATION:
        if(in == 0x61) {
            trade_centre_state = TRADE_PENDING;
            with_view_model_cpp(
                trade->view,
                SelectPokemonModel*,
                model,
                { model->gameboy_status = GAMEBOY_PENDING; },
                false);
        } else if((in & 0x60) == 0x60) {
            trade_centre_state = DONE;
        }
        break;

    case DONE:
        if(in == 0x00) {
            send = 0;
            trade_centre_state = INIT;
            with_view_model_cpp(
                trade->view,
                SelectPokemonModel*,
                model,
                { model->gameboy_status = GAMEBOY_TRADING; },
                false);
        }
        break;

    default:
        // Do Nothing
        break;
    }

    return send;
}

void transferBit(void* context) {
    Trade* trade = (Trade*)context;

    byte raw_data = furi_hal_gpio_read(&GAME_BOY_SI);
    in_data |= raw_data << (7 - shift);
    if(++shift > 7) {
        shift = 0;
        switch(connection_state) {
        case NOT_CONNECTED:
            with_view_model_cpp(
                trade->view, SelectPokemonModel*, model, { model->connected = false; }, true);
            out_data = getConnectResponse(in_data);
            break;
        case CONNECTED:
            with_view_model_cpp(
                trade->view, SelectPokemonModel*, model, { model->connected = true; }, true);
            out_data = getMenuResponse(in_data);
            break;
        case TRADE_CENTRE:
            out_data = getTradeCentreResponse(in_data, trade);
            break;
        default:
            out_data = in_data;
            break;
        }

        in_data = 0; // TODO: I don't think this is necessary?
    }

    while(procesing && !furi_hal_gpio_read(&GAME_BOY_CLK))
        ;

    furi_hal_gpio_write(&GAME_BOY_SO, out_data & 0x80 ? true : false);
    furi_delay_us(
        DELAY_MICROSECONDS); // Wait 20-60us ... 120us max (in slave mode is not necessary)
    // TODO: The above comment doesn't make sense as DELAY_MICROSECONDS is defined as 15

    if(trade_centre_state == READY_TO_GO) {
        with_view_model_cpp(
            trade->view, SelectPokemonModel*, model, { model->trading = true; }, true);
    }
    out_data = out_data << 1;
}

void input_clk_gameboy(void* context) {
    furi_assert(context);
    if(time > 0) {
        //  if there is no response from the master in 120 microseconds, the counters are reset
        if(micros() - time > 120) {
            //  IDLE & Reset
            in_data = 0;
            shift = 0;
        }
    }

    transferBit(context);
    time = micros();
}

void trade_enter_callback(void* context) {
    furi_assert(context);
    Trade* trade = (Trade*)context;
    with_view_model_cpp(
        trade->view,
        SelectPokemonModel*,
        model,
        {
            model->current_pokemon = trade->app->current_pokemon;
            model->pokemon_hex_code = trade->app->pokemon_hex_code;
            model->trading = false;
            model->connected = false;
            model->gameboy_status = GAMEBOY_INITIAL;
        },
        true);

    DATA_BLOCK[12] = trade->app->pokemon_hex_code;
    // B3 (Pin6) / SO (2)
    furi_hal_gpio_write(&GAME_BOY_SO, false);
    furi_hal_gpio_init(&GAME_BOY_SO, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    // B2 (Pin5) / SI (3)
    furi_hal_gpio_write(&GAME_BOY_SI, false);
    furi_hal_gpio_init(&GAME_BOY_SI, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
    // // C3 (Pin7) / CLK (5)
    furi_hal_gpio_init(
        &GAME_BOY_CLK,
        GpioModeInterruptRise,
        GpioPullNo,
        GpioSpeedVeryHigh); // <-- This line causes the "OK" to stop functioning when exiting the application, so a reboot of the Flipper Zero is required.
    furi_hal_gpio_remove_int_callback(&GAME_BOY_CLK);
    furi_hal_gpio_add_int_callback(&GAME_BOY_CLK, input_clk_gameboy, trade);

    // furi_hal_gpio_disable_int_callback(&GAME_BOY_CLK);
    // furi_hal_gpio_remove_int_callback(&GAME_BOY_CLK);
    // Reset GPIO pins to default state
    // furi_hal_gpio_init(&GAME_BOY_CLK, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

bool trade_custom_callback(uint32_t event, void* context) {
    UNUSED(event);
    furi_assert(context);
    Trade* trade = (Trade*)context;
    view_dispatcher_send_custom_event(trade->app->view_dispatcher, 0);
    return true;
}

void disconnect_pin(const GpioPin* pin) {
    furi_hal_gpio_init(pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(pin, true);
}

void trade_exit_callback(void* context) {
    furi_assert(context);
    procesing = false;
    furi_hal_light_set(LightGreen, 0x00);
    furi_hal_light_set(LightBlue, 0x00);
    furi_hal_light_set(LightRed, 0x00);
}

Trade* trade_alloc(App* app) {
    Trade* trade = (Trade*)malloc(sizeof(Trade));
    trade->app = app;
    trade->view = view_alloc();
    procesing = true;
    view_set_context(trade->view, trade);
    view_allocate_model(trade->view, ViewModelTypeLockFree, sizeof(SelectPokemonModel));

    view_set_draw_callback(trade->view, trade_draw_callback);
    view_set_input_callback(trade->view, trade_input_callback);
    view_set_enter_callback(trade->view, trade_enter_callback);
    view_set_custom_callback(trade->view, trade_custom_callback);
    view_set_exit_callback(trade->view, trade_exit_callback);

    return trade;
}

void trade_free(App* app) {
    furi_assert(app);
    // Free resources
    procesing = false;
    furi_hal_gpio_remove_int_callback(&GAME_BOY_CLK);

    disconnect_pin(&GAME_BOY_CLK);
    view_free(app->trade->view);
    free(app->trade);
}

View* trade_get_view(App* app) {
    furi_assert(app);
    return app->trade->view;
}
