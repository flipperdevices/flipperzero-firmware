#include <furi_hal_light.h>
#include <furi.h>

#include <gui/view.h>
#include <pokemon_icons.h>

#include "../pokemon_app.h"

#define GAME_BOY_CLK gpio_ext_pb2
#define GAME_BOY_SI gpio_ext_pc3
#define GAME_BOY_SO gpio_ext_pb3

#define DELAY_MICROSECONDS 15
#define PKMN_BLANK 0x00

#define ITEM_1_HIGHLIGHTED 0xD0
#define ITEM_2_HIGHLIGHTED 0xD1
#define ITEM_3_HIGHLIGHTED 0xD2
#define ITEM_1_SELECTED 0xD4
#define ITEM_2_SELECTED 0xD5
#define ITEM_3_SELECTED 0xD6

#define PKMN_MASTER 0x01
#define PKMN_SLAVE 0x02
#define PKMN_CONNECTED 0x60
#define PKMN_WAIT 0x7F

#define PKMN_ACTION 0x60

#define PKMN_TRADE_CENTRE ITEM_1_SELECTED
#define PKMN_COLOSSEUM ITEM_2_SELECTED
#define PKMN_BREAK_LINK ITEM_3_SELECTED

#define TRADE_CENTRE_WAIT 0xFD

struct trade_model {
    bool trading;
    bool connected;
    render_gameboy_state_t gameboy_status;
    uint8_t curr_pokemon;
    const PokemonTable* pokemon_table;
    FuriTimer* draw_timer;
};

typedef unsigned char byte;
typedef enum { NOT_CONNECTED, CONNECTED, TRADE_CENTRE, COLOSSEUM } connection_state_t;
typedef enum {
    INIT,
    READY_TO_GO,
    SEEN_FIRST_WAIT,
    SENDING_RANDOM_DATA,
    WAITING_TO_SEND_DATA,
    START_SENDING_DATA,
    SENDING_DATA,
    DATA_SENT,
    SENDING_PATCH_DATA,
    TRADE_PENDING,
    TRADE_CONFIRMATION,
    DONE
} trade_centre_state_t;

/* TODO: Convert all of these to be maintained in a struct in the Trade context */
uint8_t out_data = 0; // Should be able to be made as part of view model or static in used function
uint8_t in_data = 0; //Should be able to be made as part of view model, is used in multiple funcs
uint8_t shift = 0; //Should be able to be made as part of view model, is used in multiple funcs
uint32_t time = 0; //Should be able to be made static in used function
volatile int counter = 0; // Should be able to be made static in used function
volatile bool procesing = true; // Review this vars use, it could potentially be removed
volatile connection_state_t connection_state =
    NOT_CONNECTED; // Should be made in to view model struct
volatile trade_centre_state_t trade_centre_state =
    INIT; // Should be able to be made part of view model
unsigned char INPUT_BLOCK[405]; // Put this in pokemon_fap? Not sure yet

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

static void trade_draw_callback(Canvas* canvas, void* model) {
    const char* gameboy_status_text = NULL;
    struct trade_model* view_model = model;
    uint8_t curr_pokemon = view_model->curr_pokemon;

    canvas_clear(canvas);
    if(!view_model->trading) {
        if(!view_model->connected) {
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
        switch(view_model->gameboy_status) {
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
            canvas_draw_icon(canvas, 38, 11, view_model->pokemon_table[curr_pokemon].icon);
            break;
        default:
            // Default state added to eliminated enum warning
            break;
        }
        canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);

        canvas_draw_frame(canvas, 0, 0, 128, 64);
        canvas_draw_icon(canvas, 24, 0, &I_Space_80x18);

        switch(view_model->gameboy_status) {
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
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    uint8_t* trade_block_flat = (uint8_t*)pokemon_fap->trade_block;
    render_gameboy_state_t gameboy_status;
    byte send = in;

    furi_assert(context);

    with_view_model(
        pokemon_fap->trade_view,
        struct trade_model * model,
        { gameboy_status = model->gameboy_status; },
        false);

    switch(trade_centre_state) {
    case INIT:
        // TODO: What does this value of in mean?
        if(in == 0x00) {
            // TODO: What does counter signify here?
            if(counter == 5) {
                trade_centre_state = READY_TO_GO;
                //  CLICK EN LA MESA
                gameboy_status = GAMEBOY_READY;
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
                gameboy_status = GAMEBOY_WAITING;
            }
            counter++;
        }
        break;

    case WAITING_TO_SEND_DATA:
        if((in & 0xF0) != 0xF0) {
            counter = 0;
            INPUT_BLOCK[counter] = in;
            send = trade_block_flat[counter];
            counter++;
            trade_centre_state = SENDING_DATA;
        }
        break;

    case SENDING_DATA:
        INPUT_BLOCK[counter] = in;
        send = trade_block_flat[counter];
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
            gameboy_status = GAMEBOY_TRADE_READY;
        } else if((in & 0x60) == 0x60) {
            send = 0x60; // first pokemon
            gameboy_status = GAMEBOY_SEND;
        } else if(in == 0x00) {
            send = 0;
            trade_centre_state = TRADE_CONFIRMATION;
        }
        break;

    case TRADE_CONFIRMATION:
        if(in == 0x61) {
            trade_centre_state = TRADE_PENDING;
            gameboy_status = GAMEBOY_PENDING;
        } else if((in & 0x60) == 0x60) {
            trade_centre_state = DONE;
        }
        break;

    case DONE:
        if(in == 0x00) {
            send = 0;
            trade_centre_state = INIT;
            gameboy_status = GAMEBOY_TRADING;
        }
        break;

    default:
        // Do Nothing
        break;
    }

    with_view_model(
        pokemon_fap->trade_view,
        struct trade_model * model,
        { model->gameboy_status = gameboy_status; },
        false);

    return send;
}

void transferBit(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    furi_assert(context);
    bool connected;
    bool trading;

    with_view_model(
        pokemon_fap->trade_view,
        struct trade_model * model,
        {
            connected = model->connected;
            trading = model->trading;
        },
        false);

    byte raw_data = furi_hal_gpio_read(&GAME_BOY_SI);
    in_data |= raw_data << (7 - shift);
    if(++shift > 7) {
        shift = 0;
        switch(connection_state) {
        case NOT_CONNECTED:
            connected = false;
            out_data = getConnectResponse(in_data);
            break;
        case CONNECTED:
            connected = true;
            out_data = getMenuResponse(in_data);
            break;
        case TRADE_CENTRE:
            out_data = getTradeCentreResponse(in_data, pokemon_fap);
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

    if(trade_centre_state == READY_TO_GO) trading = true;

    out_data = out_data << 1;

    with_view_model(
        pokemon_fap->trade_view,
        struct trade_model * model,
        {
            model->connected = connected;
            model->trading = trading;
        },
        false);
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

void trade_draw_timer_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    with_view_model(
        pokemon_fap->trade_view, struct trade_model * model, { UNUSED(model); }, true);
}

void trade_enter_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    furi_assert(context);

    with_view_model(
        pokemon_fap->trade_view,
        struct trade_model * model,
        {
            model->trading = false;
            model->connected = false;
            model->gameboy_status = GAMEBOY_INITIAL;
            model->pokemon_table = pokemon_fap->pokemon_table;
            model->curr_pokemon = (uint8_t)pokemon_fap->curr_pokemon;
            model->draw_timer =
                furi_timer_alloc(trade_draw_timer_callback, FuriTimerTypePeriodic, pokemon_fap);
            /* Every 100 ms, trigger a draw update */
            furi_timer_start(model->draw_timer, 100);
        },
        true);

    // B3 (Pin6) / SO (2)
    furi_hal_gpio_write(&GAME_BOY_SO, false);
    furi_hal_gpio_init(&GAME_BOY_SO, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    // B2 (Pin5) / SI (3)
    furi_hal_gpio_write(&GAME_BOY_SI, false);
    furi_hal_gpio_init(&GAME_BOY_SI, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
    // // C3 (Pin7) / CLK (5)
    furi_hal_gpio_init(&GAME_BOY_CLK, GpioModeInterruptRise, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_remove_int_callback(&GAME_BOY_CLK);
    furi_hal_gpio_add_int_callback(&GAME_BOY_CLK, input_clk_gameboy, pokemon_fap);
}

void disconnect_pin(const GpioPin* pin) {
    furi_hal_gpio_init(pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(pin, true);
}

void trade_exit_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    furi_assert(context);
    procesing = false;
    furi_hal_light_set(LightGreen, 0x00);
    furi_hal_light_set(LightBlue, 0x00);
    furi_hal_light_set(LightRed, 0x00);
    /* Stop the timer, and deallocate it as the enter callback allocates it on entry */
    with_view_model(
        pokemon_fap->trade_view,
        struct trade_model * model,
        { furi_timer_free(model->draw_timer); },
        false);
}

View* trade_alloc(PokemonFap* pokemon_fap) {
    View* view;

    view = view_alloc();
    procesing = true;

    view_set_context(view, pokemon_fap);
    view_allocate_model(view, ViewModelTypeLockFree, sizeof(struct trade_model));

    view_set_draw_callback(view, trade_draw_callback);
    view_set_enter_callback(view, trade_enter_callback);
    view_set_exit_callback(view, trade_exit_callback);

    return view;
}

void trade_free(PokemonFap* pokemon_fap) {
    furi_assert(pokemon_fap);
    // Free resources
    procesing = false;
    furi_hal_gpio_remove_int_callback(&GAME_BOY_CLK);

    disconnect_pin(&GAME_BOY_CLK);

    view_free_model(pokemon_fap->trade_view);
    view_free(pokemon_fap->trade_view);
}
