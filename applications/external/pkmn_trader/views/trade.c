/*
 * This setup always forces the flipper to the follower/slave role in the link.
 * As far as I can tell, there is no specific reason for this other than it may
 * be a bit easier to handle an incoming clock rather than generating a clock.
 *
 * As documented here: http://www.adanscotney.com/2014/01/spoofing-pokemon-trades-with-stellaris.html
 * The general gist of the communication is as follows:
 * - Each gameboy tries to listen for an external clock coming in on the link cable.
 *   After some unknown timeout, this gameboy decides its going to take the leader/master role.
 *   In this state, it generates a clock and repeatedly sends out PKMN_MASTER(0x01)
 *   TODO: I'm not sure what kind of timeouts exist. Nor exactly how the GBs know they are connected.
 * - The other side, sensing a clock from the leader/master, then responds with PKMN_SLAVE(0x02)
 *
 *   In this application, we more or less force the flipper in to the follower/slave role. I'm
 *   not really sure why, but I assume it goes back to the original reference implementation.
 *   In the Flipper, it might also just be easier with the asynchronous context to be in the
 *   follower/slave role and just respond to clocks on an interrupt.
 *
 * - Once both sides understand their roles, they both respond with PKMN_BLANK(0x00)
 * - At this point, each gameboy repeatedly sends the menu item it has highlighted.
 *   These are ITEM_*_HIGHLIGHTED.
 * - Then, once both sides send ITEM_*_SELECTED, the next step occurs.
 *
 *   In this application, we simply repeat the same value back to the gameboy. That is,
 *   if the connected gameboy selected trade, we respond with trade as well.
 *
 * - Once the player on the gameboy side uses the trade table, a block of data is
 *   transmitted. This includes random bytes (presumably to set up the RNG seeds
 *   between two devices), and all trainer/pokemon data up from (this is the trade_block).
 * - At this point, both sides have full copies of each other's currenty party. The sides
 *   simply indicate which pokemon they are sending.
 *
 *   Interestingly, there is a close session byte (0x7f) that we don't seem to use at this time.
 *   Could be useful for, e.g. indicating to the flipper that we're done trading for a more
 *   clean exit.
 *
 *   Also, the above website mentions the data struct being 415 bytes, but we only receive
 *   405. The original Flipper implementation also used 405 bytes for the output. Finally,
 *   some other implementations of this that have surfaced use 418 bytes (with this original
 *   implementation having 3 bytes at the end of the struct commented out).
 *
 *   Doing the calculations myself, 415 should be the expected side of the trade block sent
 *   including the player name, appended with the 6-pokemon party structure:
 *   https://bulbapedia.bulbagarden.net/wiki/Pok%C3%A9mon_data_structure_(Generation_I)#6-Pok.C3.A9mon_Party_Structure
 *   (note that all of the OT names and pokemon nicknames in the table actually are 11 bytes
 *   in memory)
 *
 *   Digging through some disassembled and commented pokemon code, it does appear that there are
 *   3 extra bytes sent. So the 418 number may be more correct. 
 *
 *   Seems like there are 9 random numbers according to comments in disassembled pokemon code? But it could also be 17 based on RN+RNS lengths?
 *
 *   Once that is sent, serial preamble length is sent
 *
 *   I think I need to hook this up to a logic analyzer to see more.
 */
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

#define SERIAL_PREAMBLE_BYTE 0xFD

#define SERIAL_PREAMBLE_LENGTH 6
#define SERIAL_RN_PREAMBLE_LENGTH 7
#define SERIAL_RNS_LENGTH 10
#define SERIAL_PATCH_LIST_PART_TERMINATOR 0xFF
#define SERIAL_NO_DATA_BYTE 0xFE

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

uint8_t patch_list[205] = {0};
uint8_t patch_cnt;

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
 *
 * PKMN_BLANK is an agreement between the two devices that they have
 * determined their roles
 *
 * XXX: I'm not sure if PKMN_CONNECTED is correct or if the documentation is missing a detail
 * I think the documentation might be missing a detail as the code later does implement the saem
 * 0x60 value of "trade the first pokemon"
 */
byte getConnectResponse(byte in) {
    byte ret;

    /* XXX: Can streamline this code a bit by setting ret to in
     * and then only setting ret where needed? Might be a useless
     * optimization though */

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

/* Receive what the pokemon game is requesting and move to that mode.
 *
 * This reads bytes sent by the gameboy and responds. The only things
 * we care about are when menu items are actually selected. The protocol
 * seems to send data both when one of the link menu items is highlighted
 * and when one of them is selected.
 *
 * If somehow we get a leader/master byte received, then go back to the
 * NOT_CONNECTED state. For the leader/master byte likely means that
 * the linked gameboy is still trying to negotiate roles and we need to
 * respond with a follower/slave byte.
 *
 * Note that, we can probably eventually drop colosseum/battle connections,
 * though it may be an interesting exercise in better understanding how the
 * "random" seeding is done between the units. As noted here:
 * http://www.adanscotney.com/2014/01/spoofing-pokemon-trades-with-stellaris.html
 * it is presumed these bytes are to sync the RNG seed between the units to
 * not need arbitration on various die rolls.
 *
 * This is where we loop if we end up in the colosseum
 */
byte getMenuResponse(byte in) {
    /* TODO: Find out what this byte means */
    byte response = 0x00;
    /* XXX: Shouldn't this return a valid response for each option? 
     * e.g. if the gameboy selects trade center, should we also send trade center? 
     * or is the 0x00 an Agreement byte? I wonder if the leader/master is the
     * only one allowed to make the selection, and if the follower/slave selects a different
     * option it just instead returns BREAK_LINK? */

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
    furi_assert(context);
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    uint8_t* trade_block_flat = (uint8_t*)pokemon_fap->trade_block;
    render_gameboy_state_t gameboy_status;
    byte send = in;

    /* TODO: Figure out how we should respond to a no_data_byte and/or how to send one
     * and what response to expect.
     */

    furi_assert(context);

    with_view_model(
        pokemon_fap->trade_view,
        struct trade_model * model,
        { gameboy_status = model->gameboy_status; },
        false);

    switch(trade_centre_state) {
    /* XXX: Wait for 5x 00 on the bus. This should be safe to just leave alone. 0x00 appears to be a sync byte */
    case INIT:
        // TODO: What does this value of in mean?
        /* Currently, I believe this means OK/ACK */
        /* It looks like GB sends a bunch of 0x00s once both sides agreed to the selected menu item */
        if(in == PKMN_BLANK) {
            // TODO: What does counter signify here?
            /* It looks like counter is just intended to wait for a sequence of 00s, but its not even really a sequence, just, 5 bytes in a row. */
            if(counter == 5) {
                trade_centre_state = READY_TO_GO;
                //  CLICK EN LA MESA, when the gameboy clicks on the trade table
                gameboy_status = GAMEBOY_READY;
            }
            counter++;
        }
        break;

    case READY_TO_GO:
        /* While in this state we would mirror back whatever the GB sends us, which includes
	 * the trade/battle menu selection and highlighted option. TODO: Only respond with
	 * trade menu selection?
	 */
        /* XXX: Interestingly, the first byte we see seems to be 0xFE which is a no data byte byte */
        /* This might have some issues with the FE byte? Since I think the next state is waiting for
	 * not FD bytes */
        /* I believe this is specifically 0xFD*/
        /* Also specifically it is repeated 10 times to signify that the random block is about to start */
        if((in & 0xF0) == 0xF0) trade_centre_state = SEEN_FIRST_WAIT;
        break;

    case SEEN_FIRST_WAIT:
        if((in & 0xF0) != 0xF0) {
            counter = 0;
            trade_centre_state = SENDING_RANDOM_DATA;
        }
        break;

    /* The leader/master sends 10 random bytes. This is to synchronize the RNG
     * between the connected systems. I don't think this is really needed for
     * trade, only for battles so that both sides resolve chance events exactly
     * the same way.
     *
     * Note that every random number returned is forced to be less than FD
     *
     * Once random is doing being send, 9? more FD bytes are sent
     */
    case SENDING_RANDOM_DATA:
        if((in & 0xF0) == 0xF0) {
            if(counter == 5) {
                trade_centre_state = WAITING_TO_SEND_DATA;
                gameboy_status = GAMEBOY_WAITING;
            }
            counter++;
        }
        /* XXX: HACK: prep patch list */
        patch_list[0] = 0xFD;
        patch_list[1] = 0xFD;
        patch_list[2] = 0xFD;
        /* Patchs actually start at 11th byte? */
        patch_cnt = 10;
        break;

    /* This could fall in to the next case statement maybe? */
    /* XXX: I think this should actually be checking for in to NOT == 0xFD */
    case WAITING_TO_SEND_DATA:
        if((in & 0xF0) != 0xF0) {
            counter = 0;
            INPUT_BLOCK[counter] = in;
            if(trade_block_flat[counter] == 0xFE) {
                send = 0xFF;
                patch_list[patch_cnt] = counter + 1;
                patch_cnt++;
            } else {
                send = trade_block_flat[counter];
            }
            counter++;
            trade_centre_state = SENDING_DATA;
        }
        break;

    /* This is where we get the data from gameboy that is their trade struct */
    /* XXX: The current implementation ends up stopping short of sending data from flipper
     * and instead mirrors back data from the gameboy before we have technically sent our
     * whole struct */
    /* XXX: HACK: I think this will only work with the first patch list part */
    case SENDING_DATA:
        INPUT_BLOCK[counter] = in;
        if(trade_block_flat[counter] == 0xFE) {
            send = 0xFF;
            patch_list[patch_cnt] = counter + 1;
            patch_cnt++;
        } else {
            send = trade_block_flat[counter];
        }
        counter++;

        if(counter == 0xFE) {
            /* XXX: HACK: Just end the list instantly */
            patch_list[patch_cnt] = 0xFF;
            patch_cnt++;
            patch_list[patch_cnt] = 0xFF;
        }
        /* This should be 418? */
        /* XXX: It breaks when this is set to 418. Need to fix this */
        if(counter == 405) //TODO: replace with sizeof struct rather than static number
            trade_centre_state = SENDING_PATCH_DATA;
        break;

    /* XXX: I still have no idea what patch data is in context of the firmware */
    /* XXX: This seems to end with the gameboy sending DF FE 15? */

    /* A couple of FD bytes are sent, looks like 6, which means I don't think we can use count of FD bytes to see what mode we're in */
    /* We need to send our own patch data as well as receiving and then applying */
    case SENDING_PATCH_DATA:
        if(in == 0xFD) {
            counter = 0;
            patch_cnt = 3;
            send = 0xFD;
        } else {
            counter++;
            send = patch_list[patch_cnt];
            patch_cnt++;

            /* This is actually 200 bytes */
            /* XXX: Interestingly, it does appear to actually be 197 bytes from the first 00 after trade block, minus FFs, to the last 00 sent before long delay */
            if(counter == 197) // TODO: What is this magic value?
                trade_centre_state = TRADE_PENDING;
        }
        break;

        /* XXX: Patch incoming data here */

    case TRADE_PENDING:
        /* TODO: What are these states */
        /* 0x6f is "close session?" */
        if(in == 0x6F) {
            trade_centre_state = READY_TO_GO;
            send = 0x6F;
            gameboy_status = GAMEBOY_TRADE_READY;
            /* 0x6? says what pokemon the gameboy is sending us */
        } else if((in & 0x60) == 0x60) {
            send = 0x60; // first pokemon
            gameboy_status = GAMEBOY_SEND;
            /* I think this is a confirmation of what is being traded, likely from the dialog of:
	     * so and so will be traded for so and so, is that ok?
	     */
        } else if(in == 0x00) {
            send = 0;
            trade_centre_state = TRADE_CONFIRMATION;
        }
        /* XXX: Test to make sure saying no at is this okay does the right thing */
        break;

    /* XXX: The actual trade uses 0x62 a bunch? Is that the OKAY? Is 0x61 a NAK? Other docs show 0x6F? */
    case TRADE_CONFIRMATION:
        if(in == 0x61) {
            trade_centre_state = TRADE_PENDING;
            gameboy_status = GAMEBOY_PENDING;
        } else if((in & 0x60) == 0x60) {
            trade_centre_state = DONE;
        }
        break;

    /* XXX: I think at this point, we're just mirroring data back out so that is why the flipper now reports the same data as the gameboy */
    case DONE:
        if(in == 0x00) {
            send = 0;
            trade_centre_state = READY_TO_GO;
            gameboy_status = GAMEBOY_TRADING;
            counter = 0;
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

    /* Shift data in every clock */
    /* XXX: This logic can be made a little more clear I think.
     * Its just shifting a bit in at a time, doesn't need the clever shifting maths */
    byte raw_data = furi_hal_gpio_read(&GAME_BOY_SI);
    in_data |= raw_data << (7 - shift);

    /* Once a byte of data has been shifted in, process it */
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
            /* If we end up in the colosseum, then just repeat data back */
            /* Do we need a way to close the connection? Would that be useful? */
        default:
            out_data = in_data;
            break;
        }

        in_data = 0; // TODO: I don't think this is necessary?
    }

    /* Wait for the clock to go high again
     * XXX: I think this is superfluous since we can IRQ on falling edge, read data, delay a moment, then set data and wait until next IRQ */
    /* Basically, I don't want to stall in an interrupt context.
     * Could also maybe IRQ on either edge? and set data out when appropriate? */
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
