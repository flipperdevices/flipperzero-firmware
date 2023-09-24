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

typedef enum {
    GAMEBOY_INITIAL,
    GAMEBOY_READY,
    GAMEBOY_WAITING,
    GAMEBOY_TRADE_READY,
    GAMEBOY_SEND,
    GAMEBOY_PENDING,
    GAMEBOY_TRADING
} render_gameboy_state_t;

/* Anonymous struct */
struct Trade {
    trade_centre_state_t trade_centre_state;
    connection_state_t connection_state; // Should be made in to view model struct
    FuriTimer* draw_timer;
    View* view;
    uint8_t in_data; //Should be able to be made as part of view model, is used in multiple funcs
    uint8_t shift; //Should be able to be made as part of view model, is used in multiple funcs
    /* We still need some information from the main context */
    PokemonFap* pokemon_fap;
};

/* These are the needed variables for the draw callback */
/* Technically, I think the "right" way to do this would be
 * to keep these vars in the Trade struct and copy them in to
 * the model when they may have changed. In the interest of
 * saving space they are separated. Though it may make sense
 * later down the line to keep this as a copy.
 */
struct trade_model {
    render_gameboy_state_t gameboy_status;
    bool trading;
    bool connected;
    uint8_t curr_pokemon;
    const PokemonTable* pokemon_table;
};

/* TODO: Convert all of these to be maintained in a struct in the Trade context */
unsigned char INPUT_BLOCK[405]; // Put this in pokemon_fap? Not sure yet

void screen_gameboy_connect(Canvas* const canvas) {
    furi_assert(canvas);

    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_icon(canvas, 1, 21, &I_Connect_me_62x31);
    canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);
    canvas_draw_icon(canvas, 80, 0, &I_game_boy);
    canvas_draw_icon(canvas, 8, 2, &I_Space_65x18);
    canvas_draw_str(canvas, 18, 13, "Connect GB");
}

void screen_gameboy_connected(Canvas* const canvas) {
    furi_assert(canvas);

    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_icon(canvas, 1, 21, &I_Connected_62x31);
    canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);
    canvas_draw_icon(canvas, 80, 0, &I_game_boy);
    canvas_draw_icon(canvas, 8, 2, &I_Space_65x18);
    canvas_draw_str(canvas, 18, 13, "Connected!");
}

static void trade_draw_callback(Canvas* canvas, void* view_model) {
    furi_assert(view_model);

    const char* gameboy_status_text = NULL;
    struct trade_model* model = view_model;
    uint8_t curr_pokemon = model->curr_pokemon;
    int time_in_seconds;

    time_in_seconds = (int)DWT->CYCCNT / (72000000.0f / 4); //  250ms

    canvas_clear(canvas);
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
            canvas_draw_icon(canvas, 38, 11, model->pokemon_table[curr_pokemon].icon);
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
    }
}

uint32_t micros() {
    return DWT->CYCCNT / 64;
}

/* Get the response byte from the link partner, updating the connection
 * state if needed.
 */
static byte getConnectResponse(byte in, struct Trade* trade) {
    furi_assert(trade);

    byte ret;

    switch(in) {
    case PKMN_CONNECTED:
        trade->connection_state = CONNECTED;
        ret = PKMN_CONNECTED;
        break;
    case PKMN_MASTER:
        ret = PKMN_SLAVE;
        break;
    case PKMN_BLANK:
        ret = PKMN_BLANK;
        break;
    default:
        trade->connection_state = NOT_CONNECTED;
        ret = PKMN_BREAK_LINK;
        break;
    }

    return ret;
}

/* Figure out what the pokemon game is requesting and move to that mode.
 */
static byte getMenuResponse(byte in, struct Trade* trade) {
    furi_assert(trade);

    /* TODO: Find out what this byte means */
    byte response = 0x00;

    switch(in) {
    case PKMN_CONNECTED:
        response = PKMN_CONNECTED;
        break;
    case PKMN_TRADE_CENTRE:
        trade->connection_state = TRADE_CENTRE;
        break;
    case PKMN_COLOSSEUM:
        trade->connection_state = COLOSSEUM;
        break;
    case PKMN_BREAK_LINK:
    case PKMN_MASTER:
        trade->connection_state = NOT_CONNECTED;
        response = PKMN_BREAK_LINK;
        break;
    default:
        response = in;
        break;
    }

    return response;
}

static byte getTradeCentreResponse(byte in, struct Trade* trade) {
    furi_assert(trade);

    uint8_t* trade_block_flat = (uint8_t*)trade->pokemon_fap->trade_block;
    static int counter; // Should be able to be made static in used function
    struct trade_model* model = NULL;
    byte send = in;

    /* Since this is a fairly long function, it doesn't call any other functions,
     * the view model isn't locked, and we're in an interrupt context, lets just
     * mape the view model to a local var and commit it back when we're done.
     */
    model = view_get_model(trade->view);

    switch(trade->trade_centre_state) {
    case INIT:
        // TODO: What does this value of in mean?
        if(in == 0x00) {
            // TODO: What does counter signify here?
            if(counter == 5) {
                trade->trade_centre_state = READY_TO_GO;
                //  CLICK EN LA MESA
                model->gameboy_status = GAMEBOY_READY;
            }
            counter++;
        }
        break;

    case READY_TO_GO:
        if((in & 0xF0) == 0xF0) trade->trade_centre_state = SEEN_FIRST_WAIT;
        break;

    case SEEN_FIRST_WAIT:
        if((in & 0xF0) != 0xF0) {
            counter = 0;
            trade->trade_centre_state = SENDING_RANDOM_DATA;
        }
        break;

    case SENDING_RANDOM_DATA:
        if((in & 0xF0) == 0xF0) {
            if(counter == 5) {
                trade->trade_centre_state = WAITING_TO_SEND_DATA;
                model->gameboy_status = GAMEBOY_WAITING;
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
            trade->trade_centre_state = SENDING_DATA;
        }
        break;

    case SENDING_DATA:
        INPUT_BLOCK[counter] = in;
        send = trade_block_flat[counter];
        counter++;
        if(counter == 405) //TODO: replace with sizeof struct rather than static number
            trade->trade_centre_state = SENDING_PATCH_DATA;
        break;

    case SENDING_PATCH_DATA:
        if(in == 0xFD) {
            counter = 0;
            send = 0xFD;
        } else {
            counter++;
            if(counter == 197) // TODO: What is this magic value?
                trade->trade_centre_state = TRADE_PENDING;
        }
        break;

    case TRADE_PENDING:
        /* TODO: What are these states */
        if(in == 0x6F) {
            trade->trade_centre_state = READY_TO_GO;
            send = 0x6F;
            model->gameboy_status = GAMEBOY_TRADE_READY;
        } else if((in & 0x60) == 0x60) {
            send = 0x60; // first pokemon
            model->gameboy_status = GAMEBOY_SEND;
        } else if(in == 0x00) {
            send = 0;
            trade->trade_centre_state = TRADE_CONFIRMATION;
        }
        break;

    case TRADE_CONFIRMATION:
        if(in == 0x61) {
            trade->trade_centre_state = TRADE_PENDING;
            model->gameboy_status = GAMEBOY_PENDING;
        } else if((in & 0x60) == 0x60) {
            trade->trade_centre_state = DONE;
        }
        break;

    case DONE:
        if(in == 0x00) {
            send = 0;
            trade->trade_centre_state = INIT;
            model->gameboy_status = GAMEBOY_TRADING;
        }
        break;

    default:
        // Do Nothing
        break;
    }

    view_commit_model(trade->view, false);

    return send;
}

void transferBit(void* context) {
    furi_assert(context);

    struct Trade* trade = (struct Trade*)context;
    static uint8_t
        out_data; // XXX: If we need to clear this between runs of trade view, this needs to be moved to Trade
    bool connected;
    bool trading;

    with_view_model(
        trade->view,
        struct trade_model * model,
        {
            connected = model->connected;
            trading = model->trading;
        },
        false);

    byte raw_data = furi_hal_gpio_read(&GAME_BOY_SI);
    trade->in_data |= raw_data << (7 - trade->shift);
    if(++trade->shift > 7) {
        trade->shift = 0;
        switch(trade->connection_state) {
        case NOT_CONNECTED:
            connected = false;
            out_data = getConnectResponse(trade->in_data, trade);
            break;
        case CONNECTED:
            connected = true;
            out_data = getMenuResponse(trade->in_data, trade);
            break;
        case TRADE_CENTRE:
            out_data = getTradeCentreResponse(trade->in_data, trade);
            break;
        default:
            out_data = trade->in_data;
            break;
        }

        trade->in_data = 0; // TODO: I don't think this is necessary?
    }

    while(!furi_hal_gpio_read(&GAME_BOY_CLK))
        ;

    furi_hal_gpio_write(&GAME_BOY_SO, out_data & 0x80 ? true : false);
    furi_delay_us(
        DELAY_MICROSECONDS); // Wait 20-60us ... 120us max (in slave mode is not necessary)
    // TODO: The above comment doesn't make sense as DELAY_MICROSECONDS is defined as 15

    if(trade->trade_centre_state == READY_TO_GO) trading = true;

    out_data = out_data << 1;

    with_view_model(
        trade->view,
        struct trade_model * model,
        {
            model->trading = trading;
            model->connected = connected;
        },
        false);
}

void input_clk_gameboy(void* context) {
    furi_assert(context);

    struct Trade* trade = (struct Trade*)context;
    static uint32_t time; //This should be fine

    if(time > 0) {
        //  if there is no response from the master in 120 microseconds, the counters are reset
        if(micros() - time > 120) {
            //  IDLE & Reset
            trade->in_data = 0;
            trade->shift = 0;
        }
    }

    transferBit(trade);
    time = micros();
}

void trade_draw_timer_callback(void* context) {
    furi_assert(context);

    struct Trade* trade = (struct Trade*)context;

    with_view_model(
        trade->view, struct trade_model * model, { UNUSED(model); }, true);
}

void trade_enter_callback(void* context) {
    furi_assert(context);

    struct Trade* trade = (struct Trade*)context;

    /* Re-init variables */
    with_view_model(
        trade->view,
        struct trade_model * model,
        {
            model->trading = false;
            model->connected = false;
            model->gameboy_status = GAMEBOY_INITIAL;
        },
        true);
    trade->connection_state = NOT_CONNECTED;
    trade->trade_centre_state = INIT;

    trade->in_data = 0;
    trade->shift = 0;

    trade->draw_timer = furi_timer_alloc(trade_draw_timer_callback, FuriTimerTypePeriodic, trade);
    /* Every 100 ms, trigger a draw update */
    furi_timer_start(trade->draw_timer, 100);

    with_view_model(
        trade->view,
        struct trade_model * model,
        {
            model->pokemon_table = trade->pokemon_fap->pokemon_table;
            model->curr_pokemon = (uint8_t)trade->pokemon_fap->curr_pokemon;
        },
        true);

    /* XXX: Figure out "proper" GPIO setup/use/teardown */
    // B3 (Pin6) / SO (2)
    furi_hal_gpio_write(&GAME_BOY_SO, false);
    furi_hal_gpio_init(&GAME_BOY_SO, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    // B2 (Pin5) / SI (3)
    furi_hal_gpio_write(&GAME_BOY_SI, false);
    furi_hal_gpio_init(&GAME_BOY_SI, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
    // // C3 (Pin7) / CLK (5)
    furi_hal_gpio_init(&GAME_BOY_CLK, GpioModeInterruptRise, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_remove_int_callback(&GAME_BOY_CLK);
    furi_hal_gpio_add_int_callback(&GAME_BOY_CLK, input_clk_gameboy, trade);
}

void disconnect_pin(const GpioPin* pin) {
    /* XXX: Why is this analog? Is that its default? */
    /* XXX: May be able to use existing API functions for this as well */
    furi_hal_gpio_init(pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(pin, true);
}

void trade_exit_callback(void* context) {
    furi_assert(context);

    struct Trade* trade = (struct Trade*)context;

    furi_hal_light_set(LightGreen, 0x00);
    furi_hal_light_set(LightBlue, 0x00);
    furi_hal_light_set(LightRed, 0x00);

    /* Stop the timer, and deallocate it as the enter callback allocates it on entry */
    furi_timer_free(trade->draw_timer);
}

void* trade_alloc(PokemonFap* pokemon_fap) {
    furi_assert(pokemon_fap);

    struct Trade* trade = malloc(sizeof(struct Trade));

    trade->view = view_alloc();
    trade->pokemon_fap = pokemon_fap;

    view_set_context(trade->view, trade);
    view_allocate_model(trade->view, ViewModelTypeLockFree, sizeof(struct trade_model));

    view_set_draw_callback(trade->view, trade_draw_callback);
    view_set_enter_callback(trade->view, trade_enter_callback);
    view_set_exit_callback(trade->view, trade_exit_callback);

    return trade;
}

void trade_free(void* context) {
    furi_assert(context);

    struct Trade* trade = (struct Trade*)context;

    // Free resources
    furi_hal_gpio_remove_int_callback(&GAME_BOY_CLK);

    disconnect_pin(&GAME_BOY_CLK);

    view_free_model(trade->view);
    view_free(trade->view);
    free(trade);
}
