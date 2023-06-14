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
    canvas_clear(canvas);
    SelectPokemonModel* model = (SelectPokemonModel*)context;
    if(!model->trading) {
        if(!model->connected) {
            furi_hal_light_set(LightGreen, 0x00);
            furi_hal_light_set(LightBlue, 0x00);
            furi_hal_light_set(LightRed, 0xff);
            screen_gameboy_connect(canvas);
        } else if(model->connected) {
            furi_hal_light_set(LightGreen, 0xff);
            furi_hal_light_set(LightBlue, 0x00);
            furi_hal_light_set(LightRed, 0x00);
            screen_gameboy_connected(canvas);
        }
    } else {
        if(model->gameboy_status == GAMEBOY_TRADING) {
            furi_hal_light_set(LightGreen, 0x00);
            furi_hal_light_set(LightRed, 0x00);
            if(time_in_seconds % 2 == 1) {
                furi_hal_light_set(LightBlue, 0xff);
                canvas_draw_icon(canvas, 0, 0, &I_gb_step_1);
            } else {
                furi_hal_light_set(LightBlue, 0x00);
                canvas_draw_icon(canvas, 0, 0, &I_gb_step_2);
            }
        } else if(
            model->gameboy_status == GAMEBOY_READY || model->gameboy_status == GAMEBOY_WAITING ||
            model->gameboy_status == GAMEBOY_SEND) {
            canvas_draw_icon(canvas, 38, 11, pokemon_table[model->current_pokemon].icon);
        }
        canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);

        canvas_draw_frame(canvas, 0, 0, 128, 64);
        canvas_draw_icon(canvas, 24, 0, &I_Space_80x18);

        const char* gameboy_status_text = "INITIAL";
        if(model->gameboy_status == GAMEBOY_READY) {
            gameboy_status_text = "READY";
        } else if(model->gameboy_status == GAMEBOY_WAITING) {
            gameboy_status_text = "WAITING";
        } else if(model->gameboy_status == GAMEBOY_TRADE_READY) {
            gameboy_status_text = "READY";
        } else if(model->gameboy_status == GAMEBOY_SEND) {
            gameboy_status_text = "DEAL...";
        } else if(model->gameboy_status == GAMEBOY_PENDING) {
            gameboy_status_text = "PENDING...";
        } else if(model->gameboy_status == GAMEBOY_TRADING) {
            gameboy_status_text = "TRADING...";
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
byte getConnectResponse(byte in) {
    if(in == PKMN_CONNECTED) {
        connection_state = CONNECTED;
        return PKMN_CONNECTED;
    }

    if(in == PKMN_MASTER) {
        return PKMN_SLAVE;
    }

    if(in == PKMN_BLANK) {
        return PKMN_BLANK;
    }

    connection_state = NOT_CONNECTED;
    return PKMN_BREAK_LINK;
}
byte getMenuResponse(byte in) {
    byte response = 0x00;
    if(in == PKMN_CONNECTED) {
        response = PKMN_CONNECTED;
    } else if(in == PKMN_TRADE_CENTRE) {
        connection_state = TRADE_CENTRE;
    } else if(in == PKMN_COLOSSEUM) {
        connection_state = COLOSSEUM;
    } else if(in == PKMN_BREAK_LINK || in == PKMN_MASTER) {
        connection_state = NOT_CONNECTED;
        response = PKMN_BREAK_LINK;
    } else {
        response = in;
    }
    return response;
}

byte getTradeCentreResponse(byte in, void* context) {
    UNUSED(context);
    Trade* trade = (Trade*)context;
    byte send = 0x00;
    if(trade_centre_state == INIT && in == 0x00) {
        if(counter++ == 5) {
            trade_centre_state = READY_TO_GO;
            //  CLICK EN LA MESA
            with_view_model_cpp(
                trade->view,
                SelectPokemonModel*,
                model,
                { model->gameboy_status = GAMEBOY_READY; },
                false);
        }
        send = in;
    } else if(trade_centre_state == READY_TO_GO && (in & 0xF0) == 0xF0) {
        trade_centre_state = SEEN_FIRST_WAIT;
        send = in;
    } else if(trade_centre_state == SEEN_FIRST_WAIT && (in & 0xF0) != 0xF0) {
        send = in;
        counter = 0;
        trade_centre_state = SENDING_RANDOM_DATA;
    } else if(trade_centre_state == SENDING_RANDOM_DATA && (in & 0xF0) == 0xF0) {
        if(counter++ == 5) {
            trade_centre_state = WAITING_TO_SEND_DATA;

            with_view_model_cpp(
                trade->view,
                SelectPokemonModel*,
                model,
                { model->gameboy_status = GAMEBOY_WAITING; },
                false);
        }
        send = in;
    } else if(trade_centre_state == WAITING_TO_SEND_DATA && (in & 0xF0) != 0xF0) {
        counter = 0;
        INPUT_BLOCK[counter] = in;
        send = DATA_BLOCK[counter++];
        trade_centre_state = SENDING_DATA;
    } else if(trade_centre_state == SENDING_DATA) {
        INPUT_BLOCK[counter] = in;
        send = DATA_BLOCK[counter++];
        if(counter == 405) {
            trade_centre_state = SENDING_PATCH_DATA;
        }
    } else if(trade_centre_state == SENDING_PATCH_DATA && in == 0xFD) {
        counter = 0;
        send = 0xFD;
    } else if(trade_centre_state == SENDING_PATCH_DATA && in != 0xFD) {
        send = in;
        counter++;
        if(counter == 197) {
            trade_centre_state = TRADE_PENDING;
        }
    } else if(trade_centre_state == TRADE_PENDING && (in & 0x60) == 0x60) {
        if(in == 0x6f) {
            trade_centre_state = READY_TO_GO;
            send = 0x6f;
            with_view_model_cpp(
                trade->view,
                SelectPokemonModel*,
                model,
                { model->gameboy_status = GAMEBOY_TRADE_READY; },
                false);

        } else {
            send = 0x60; // first pokemon
            with_view_model_cpp(
                trade->view,
                SelectPokemonModel*,
                model,
                { model->gameboy_status = GAMEBOY_SEND; },
                false);
        }
    } else if(trade_centre_state == TRADE_PENDING && in == 0x00) {
        send = 0;
        trade_centre_state = TRADE_CONFIRMATION;

    } else if(trade_centre_state == TRADE_CONFIRMATION && (in & 0x60) == 0x60) {
        send = in;
        if(in == 0x61) {
            trade_centre_state = TRADE_PENDING;
            with_view_model_cpp(
                trade->view,
                SelectPokemonModel*,
                model,
                { model->gameboy_status = GAMEBOY_PENDING; },
                false);
        } else {
            trade_centre_state = DONE;
        }
    } else if(trade_centre_state == DONE && in == 0x00) {
        send = 0;
        trade_centre_state = INIT;
        with_view_model_cpp(
            trade->view,
            SelectPokemonModel*,
            model,
            { model->gameboy_status = GAMEBOY_TRADING; },
            false);
    } else {
        send = in;
    }

    return send;
}

void transferBit(void* context) {
    Trade* trade = (Trade*)context;

    byte raw_data = furi_hal_gpio_read(&GAME_BOY_SI);
    in_data |= raw_data << (7 - shift);
    if(++shift > 7) {
        shift = 0;
        if(connection_state == NOT_CONNECTED) {
            with_view_model_cpp(
                trade->view, SelectPokemonModel*, model, { model->connected = false; }, true);
            out_data = getConnectResponse(in_data);
        } else if(connection_state == CONNECTED) {
            with_view_model_cpp(
                trade->view, SelectPokemonModel*, model, { model->connected = true; }, true);
            out_data = getMenuResponse(in_data);
        } else if(connection_state == TRADE_CENTRE) {
            out_data = getTradeCentreResponse(in_data, trade);

        } else {
            out_data = in_data;
        }

        in_data = 0;
    }
    while(procesing && !furi_hal_gpio_read(&GAME_BOY_CLK))
        ;
    furi_hal_gpio_write(&GAME_BOY_SO, out_data & 0x80 ? true : false);
    furi_delay_us(
        DELAY_MICROSECONDS); // Wait 20-60us ... 120us max (in slave mode is not necessary)

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
