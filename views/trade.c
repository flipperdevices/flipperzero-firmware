/*
 * This setup always forces the flipper to the follower/slave role in the link.
 * This just makes our logic consistent and since we're going to be gobs faster
 * than a real Gameboy, we can be guaranteed to always be ready to respond.
 *
 * As documented here: http://www.adanscotney.com/2014/01/spoofing-pokemon-trades-with-stellaris.html
 * The general gist of the communication is as follows:
 * 1) Each gameboy tries to listen for an external clock coming in on the link cable.
 *     After some unknown timeout, this gameboy decides its going to take the leader/master role.
 *     In this state, it generates a clock and repeatedly sends out PKMN_MASTER(0x01)
 * 2) The other side, sensing a clock from the leader/master, then responds with PKMN_SLAVE(0x02)
 * 3) Once both sides understand their roles, they both respond with PKMN_BLANK(0x00) for a bit.
 * 4) Next, the leader/master sends CONNECTED(0x60) bytes that the follower/slave repeats
 *     back. Then a bunch of BLANK bytes.
 * 5) At this point, each gameboy repeatedly sends the menu item it has highlighted,
 *     prepended by a BLANK, in groups of 3 bytes. These are ITEM_*_HIGHLIGHTED.
 * 6) Then, once both sides send ITEM_*_SELECTED, the next step occurs.
 *     This application, from steps 3 through 6, just repeats bytes back and lets the Gameboy
 *     dictate the steps. We stay here until we start seeing PREAMBLE(0xFD) bytes,
 *     as those dictate the start of the next sections.
 *
 * The Flipper is now in the "READY" state.
 *
 * 7) Once the player on the gameboy side uses the trade table, a block of data is
 *     transmitted. This starts with 10x PREAMBLE(0xFD) bytes, 10x random bytes (to
 *     sync the RNG between two devices, unused at this time), and then the 415 trade_block,
 *     struct gets transferred. At the end of this is 3 ending bytes, DF FE 15. And, wierdly,
 *     3 PREAMBLE(0xFD) bytes.
 * 8) The patch list starts with 3x more PREAMBLE(0xFD) bytes for a total of 6x PREABMLE,
 *     followed by 7x BLANK bytes. Then remaining 189 bytes of patch list data. The patch
 *     list is used to compensate for byte values of NO_DATA_BYE(0xFE) being transmitted.
 *     The patch list is specifically for the party data of the trade_block. To patch
 *     outgoing data, if a byte is 0xFE, it is changed to 0xFF, and the index+1 is
 *     added to the patch list. There are two parts to the patch list as the data it
 *     covers is longer than 0xFC. After each part is complete, 0xFF is added to the
 *     patch list. The first part of the patch list can patch 0x00:0xFB of the party,
 *     the second part can patch 0xFC:0x107 of the party. If there are no bytes to
 *     patch in a part, 0xFF is just appended. After both terminators, it is expected
 *     all remaining bytes are 0x00.
 *
 * The Flipper is now in the "WAITING" state.
 *
 * 9) At this point, both sides have full copies of each other's currenty party. The sides
 *     simply indicate which pokemon they are sending. This is done with a BLANK byte to
 *     start, and then each side indicates which pokemon it wants to trade with SEL_NUM_MASK(0x60)
 *     + party index. We always transmit the first pokemon. Once in a agreement, both
 *     sides transmit a handful of BLANK bytes.
 *
 * The Flipper is now in the "DEAL?" state.
 *
 * A) Starting with another BLANK byte, both sides need to agree to the trade by
 *     sending TRADE_ACCEPT(0x62) repeatedly, and then a handful of BLANK bytes.
 *     To disagree with a trade, either side would send TRADE_REJECT(0x61), the
 *     Flipper will never send this on its own. If the Gameboy does, both it and
 *     the flipper got back to step 9 again.
 *
 * The Flipper is now in the "TRADING" state.
 *
 * B) The Flipper actually goes back to step 7, but keeps the drawing mode as
 *     TRADING. After the trade is complete on the Gameboy, it re-sends the
 *     trade_block data. This re-syncs the states between the Flipper and
 *     Gameboy and another trade can occur.
 *
 * *) A point of note is that the Flipper can go back to the main menu from
 *     any state. Though, doing so in the TRADING state might actually cause
 *     the Gameboy to have issues. When in READY or WAITING state, the Flipper
 *     can go back and modify the pokemon that the Gameboy sent to it. If the
 *     Flipper then goes back to Trade from the main menu, it will be in the
 *     READY state. If the Gameboy is still on the trade menu, and it tries
 *     to trade, the trade will be rejected. The Gameboy needs to exit the
 *     trade menu, and then re-enter it by selecting the table in the trade
 *     center. This will then push the Flipper to the WAITING state, and the
 *     trade_blocks will re-sync between them with the new data. If the Gameboy
 *     leave the trade menu while the Flipper is in the WAITING state, the
 *     Flipper will go back to the READY state.
 *
 *    TODO: Set up requiring a long back press to go back to the main menu
 *     from the TRADING state or from the main menu to exit the application.
 */

#include <furi.h>
#include <furi_hal.h>

#include <gui/view.h>
#include <pokemon_icons.h>

#include "../pokemon_app.h"
#include "trade_patch_list.h"

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
#define SERIAL_TRADE_PREAMBLE_LENGTH 9
#define SERIAL_RNS_LENGTH 10
#define SERIAL_PATCH_LIST_PART_TERMINATOR 0xFF
#define SERIAL_NO_DATA_BYTE 0xFE

#define PKMN_MASTER 0x01
#define PKMN_SLAVE 0x02
#define PKMN_CONNECTED 0x60
#define PKMN_TRADE_ACCEPT 0x62
#define PKMN_TRADE_REJECT 0x61
#define PKMN_TABLE_LEAVE 0x6f
#define PKMN_SEL_NUM_MASK 0x60
#define PKMN_SEL_NUM_ONE 0x60

#define PKMN_ACTION 0x60

#define PKMN_TRADE_CENTRE ITEM_1_SELECTED
#define PKMN_COLOSSEUM ITEM_2_SELECTED
#define PKMN_BREAK_LINK ITEM_3_SELECTED

/* States specific to the trade process. */
typedef enum {
    TRADE_RESET,
    TRADE_INIT,
    TRADE_RANDOM,
    TRADE_DATA,
    TRADE_PATCH_HEADER,
    TRADE_PATCH_DATA,
    TRADE_SELECT,
    TRADE_PENDING,
    TRADE_CONFIRMATION,
    TRADE_DONE
} trade_centre_state_t;

/* Global states for the trade logic. These are used to dictate what gets drawn
 * to the screen but also handle a few sync states. The CONN states are to denote
 * if a link has been established or note. READY through TRADING are all specific
 * screens to draw in the trade center. COLOSSEUM causes a data loopback so the
 * player can fight themselves.
 */
typedef enum {
    GAMEBOY_CONN_FALSE,
    GAMEBOY_CONN_TRUE,
    GAMEBOY_READY,
    GAMEBOY_WAITING,
    GAMEBOY_TRADE_PENDING,
    GAMEBOY_TRADING,
    GAMEBOY_COLOSSEUM
} render_gameboy_state_t;

/* Anonymous struct */
struct trade_ctx {
    trade_centre_state_t trade_centre_state;
    FuriTimer* draw_timer;
    View* view;
    uint8_t in_data;
    uint8_t out_data;
    uint8_t shift;
    TradeBlock* trade_block;
    TradeBlock* input_block;
    const PokemonTable* pokemon_table;
    struct patch_list* patch_list;
};

/* These are the needed variables for the draw callback */
struct trade_model {
    render_gameboy_state_t gameboy_status;
    bool ledon; // Controlls the blue LED during trade
    uint8_t curr_pokemon;
    const PokemonTable* pokemon_table;
};

/* A callback function that must be called outside of an interrupt context,
 * This will completely destroy the current patch list, and then rebuild it from
 * the current trade_block state. This is used mostly after a trade to rebuild
 * the list with the new data we just copied in.
 */
static void pokemon_plist_recreate_callback(void* context, uint32_t arg) {
    furi_assert(context);
    UNUSED(arg);
    struct trade_ctx* trade = context;

    plist_create(&(trade->patch_list), trade->trade_block);
}

/* Draws a whole screen image with Flipper mascot, Gameboy, etc. */
static void trade_draw_connect(Canvas* const canvas) {
    furi_assert(canvas);

    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_icon(canvas, 1, 21, &I_Connect_me_62x31);
    canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);
    canvas_draw_icon(canvas, 80, 0, &I_game_boy);
    canvas_draw_icon(canvas, 8, 2, &I_Space_65x18);
    canvas_draw_str(canvas, 18, 13, "Connect GB");
}

/* Draws a whole screen image with Flipper mascot, Gameboy, etc. */
static void trade_draw_connected(Canvas* const canvas) {
    furi_assert(canvas);

    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_icon(canvas, 1, 21, &I_Connected_62x31);
    canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);
    canvas_draw_icon(canvas, 80, 0, &I_game_boy);
    canvas_draw_icon(canvas, 8, 2, &I_Space_65x18);
    canvas_draw_str(canvas, 18, 13, "Connected!");
}

/* Draws a frame around the screen, with a box at the top for a text string,
 * and an icon of the player.
 */
static void trade_draw_frame(Canvas* canvas, const char* str) {
    furi_assert(canvas);

    canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);
    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_icon(canvas, 24, 0, &I_Space_80x18);
    canvas_draw_str(canvas, 48, 12, str);
    canvas_draw_icon(canvas, 27, 1, &I_red_16x15);
}

/* Draws the pokemon's image in the middle of the screen */
static void trade_draw_pkmn_avatar(Canvas* canvas, const Icon* icon) {
    furi_assert(canvas);
    furi_assert(icon);

    canvas_draw_icon(canvas, 38, 11, icon);
    furi_hal_light_set(LightBlue, 0x00);
    furi_hal_light_set(LightGreen, 0x00);
}

/* Called every 250 ms on a timer. This controls the blue LED when in TRADING
 * state. This is necessary as Flipper OS does not make any guarantees on when
 * draw updates may or may not be called. There are situations where a draw
 * update is called much faster. Therefore, we need to control the update rate
 * via the ledon view_model variable.
 */
static void trade_draw_timer_callback(void* context) {
    furi_assert(context);

    struct trade_ctx* trade = (struct trade_ctx*)context;

    with_view_model(
        trade->view, struct trade_model * model, { model->ledon ^= 1; }, true);
}

static void trade_draw_callback(Canvas* canvas, void* view_model) {
    furi_assert(view_model);
    struct trade_model* model = view_model;
    const Icon* icon = model->pokemon_table[model->curr_pokemon].icon;

    canvas_clear(canvas);
    switch(model->gameboy_status) {
    case GAMEBOY_CONN_FALSE:
        furi_hal_light_set(LightGreen, 0x00);
        furi_hal_light_set(LightRed, 0xff);
        trade_draw_connect(canvas);
        break;
    case GAMEBOY_CONN_TRUE:
        furi_hal_light_set(LightGreen, 0xff);
        furi_hal_light_set(LightRed, 0x00);
        trade_draw_connected(canvas);
        break;
    case GAMEBOY_READY:
        trade_draw_pkmn_avatar(canvas, icon);
        trade_draw_frame(canvas, "READY");
        break;
    case GAMEBOY_WAITING:
        trade_draw_pkmn_avatar(canvas, icon);
        trade_draw_frame(canvas, "WAITING");
        break;
    case GAMEBOY_TRADE_PENDING:
        trade_draw_pkmn_avatar(canvas, icon);
        trade_draw_frame(canvas, "DEAL?");
        break;
    case GAMEBOY_TRADING:
        furi_hal_light_set(LightGreen, 0x00);
        if(model->ledon) {
            furi_hal_light_set(LightBlue, 0xff);
            canvas_draw_icon(canvas, 0, 0, &I_gb_step_1);
        } else {
            furi_hal_light_set(LightBlue, 0x00);
            canvas_draw_icon(canvas, 0, 0, &I_gb_step_2);
        }
        trade_draw_frame(canvas, "TRADING");
        break;
    case GAMEBOY_COLOSSEUM:
        trade_draw_frame(canvas, "FIGHT!");
        break;
    default:
        trade_draw_frame(canvas, "INITIAL");
        break;
    }
}

/* Get the response byte from the link partner, updating the connection
 * state if needed.
 */
static uint8_t getConnectResponse(struct trade_ctx* trade) {
    furi_assert(trade);
    uint8_t ret;

    switch(trade->in_data) {
    case PKMN_CONNECTED:
        with_view_model(
            trade->view,
            struct trade_model * model,
            { model->gameboy_status = GAMEBOY_CONN_TRUE; },
            false);
        ret = PKMN_CONNECTED;
        break;
    case PKMN_MASTER:
        ret = PKMN_SLAVE;
        break;
    case PKMN_BLANK:
        ret = PKMN_BLANK;
        break;
    default:
        with_view_model(
            trade->view,
            struct trade_model * model,
            { model->gameboy_status = GAMEBOY_CONN_FALSE; },
            false);
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
 */
static uint8_t getMenuResponse(struct trade_ctx* trade) {
    furi_assert(trade);

    uint8_t response = PKMN_BLANK;

    switch(trade->in_data) {
    case PKMN_CONNECTED:
        response = PKMN_CONNECTED;
        break;
    case PKMN_TRADE_CENTRE:
        with_view_model(
            trade->view,
            struct trade_model * model,
            { model->gameboy_status = GAMEBOY_READY; },
            false);
        break;
    case PKMN_COLOSSEUM:
        with_view_model(
            trade->view,
            struct trade_model * model,
            { model->gameboy_status = GAMEBOY_COLOSSEUM; },
            false);
        break;
    case PKMN_BREAK_LINK:
    case PKMN_MASTER:
        with_view_model(
            trade->view,
            struct trade_model * model,
            { model->gameboy_status = GAMEBOY_CONN_FALSE; },
            false);
        response = PKMN_BREAK_LINK;
        break;
    default:
        response = trade->in_data;
        break;
    }

    return response;
}

static uint8_t getTradeCentreResponse(struct trade_ctx* trade) {
    furi_assert(trade);

    uint8_t* trade_block_flat = (uint8_t*)trade->trade_block;
    uint8_t* input_block_flat = (uint8_t*)trade->input_block;
    uint8_t* input_party_flat = (uint8_t*)trade->input_block->party;
    struct trade_model* model = NULL;
    uint8_t in = trade->in_data;
    uint8_t send = in;
    static bool patch_pt_2;
    static int counter;
    static uint8_t in_pkmn_idx;

    /* TODO: Figure out how we should respond to a no_data_byte and/or how to
     * send one and what response to expect.
     *
     * This isn't a high priority since it should be unlikely that we would
     * actually ever receive a NO_DATA_BYE as the Gameboy is the leader/master
     * and therefore would only transmit when it has data ready.
     */

    /* Since this is a fairly long function, it doesn't call any other functions,
     * the view model isn't locked, and we're in an interrupt context, lets just
     * map the view model to a local var and commit it back when we're done.
     */
    model = view_get_model(trade->view);

    /* There is a handful of commucations that happen once the gameboy
     * clicks on the table. For all of them, the Flipper can just mirror back
     * the byte the gameboy sends. We can spin in this forever until we see 10x
     * SERIAL_PREAMBLE_BYTEs. Once we receive those, the counters are synched,
     * and every byte after that can be easily counted for the actual transferr
     * of Pokemon data.
     */
    switch(trade->trade_centre_state) {
    case TRADE_RESET:
        /* Reset counters and other static variables */
        counter = 0;
        patch_pt_2 = false;
        trade->trade_centre_state = TRADE_INIT;
        break;

    /* This state runs through the end of the random preamble */
    case TRADE_INIT:
        if(in == SERIAL_PREAMBLE_BYTE) {
            counter++;
            model->gameboy_status = GAMEBOY_WAITING;
        } else if((in & PKMN_SEL_NUM_MASK) == PKMN_SEL_NUM_MASK) {
            send = PKMN_TABLE_LEAVE;
        }
        if(counter == SERIAL_RNS_LENGTH) {
            trade->trade_centre_state = TRADE_RANDOM;
            counter = 0;
        }
        break;

    /* Once we start getting PKMN_BLANKs, we mirror them until we get 10x
     * SERIAL_PREAMBLE_BYTE, and then 10 random numbers. The 10 random
     * numbers are for synchrinizing the PRNG between the two systems,
     * we do not use these numbers at this time.
     *
     * This waits through the end of the trade block preamble, a total of 20
     * bytes.
     */
    case TRADE_RANDOM:
        counter++;
        if(counter == (SERIAL_RNS_LENGTH + SERIAL_TRADE_PREAMBLE_LENGTH)) {
            trade->trade_centre_state = TRADE_DATA;
            counter = 0;
        }
        break;

    /* This is where we exchange trade_block data with the Gameboy */
    case TRADE_DATA:
        input_block_flat[counter] = in;
        send = trade_block_flat[counter];
        counter++;

        if(counter == sizeof(TradeBlock)) {
            trade->trade_centre_state = TRADE_PATCH_HEADER;
            counter = 0;
        }

        break;

    /* This absorbs the 3 byte ending sequence (DF FE 15) after the trade data is
     * swapped, then the 3x SERIAL_PREAMBLE_BYTEs that end the trade data, and
     * another 3x of them that start the patch data. By the time we're done with
     * this state, the patch list BLANK bytes are ready to be transmitted.
     * We only care about the 6x total preamble bytes.
     */
    case TRADE_PATCH_HEADER:
        if(in == SERIAL_PREAMBLE_BYTE) {
            counter++;
        }

        if(counter == 6) {
            counter = 0;
            trade->trade_centre_state = TRADE_PATCH_DATA;
        } else {
            break;
        }
        [[fallthrough]];
    case TRADE_PATCH_DATA:
        counter++;
        /* This magic number is basically the header length, 10, minus
	 * the 3x 0xFD that we should be transmitting as part of the patch
	 * list header.
	 */
        if(counter > 7) {
            send = plist_index_get(trade->patch_list, (counter - 8));
        }

        /* Patch received data */
        /* This relies on the data sent only ever sending 0x00 after
         * part 2 of the patch list has been terminated. This is the
         * case in official Gen I code at this time.
         */
        switch(in) {
        case PKMN_BLANK:
            break;
        case SERIAL_PATCH_LIST_PART_TERMINATOR:
            patch_pt_2 = true;
            break;
        default: // Any nonzero value will cause a patch
            if(!patch_pt_2) {
                /* Pt 1 is 0x00 - 0xFB */
                input_party_flat[in - 1] = SERIAL_NO_DATA_BYTE;
            } else {
                /* Pt 2 is 0xFC - 0x107
		 * 0xFC + in - 1
		 */
                input_party_flat[0xFB + in] = SERIAL_NO_DATA_BYTE;
            }
            break;
        }

        /* What is interesting about the following check, is the Pokemon code
	 * seems to allocate 203 bytes, 3x for the preamble, and then 200 bytes
	 * of patch list. But in practice, the Gameboy seems to transmit 3x
	 * preamble bytes, 7x 0x00, then 189 bytes for the patch list. A
	 * total of 199 bytes transmitted.
	 */
        if(counter == 196) trade->trade_centre_state = TRADE_SELECT;

        break;

    /* Resets the incoming pokemon index, and once a BLANK byte is received,
     * moves to the pending state.
     */
    case TRADE_SELECT:
        in_pkmn_idx = 0;
        if(in == PKMN_BLANK) {
            trade->trade_centre_state = TRADE_PENDING;
        } else {
            break;
        }
        [[fallthrough]];
    /* Handle the Gameboy selecting a pokemon to trade, or leaving the table */
    case TRADE_PENDING:
        /* If the player leaves the trade menu and returns to the room */
        if(in == PKMN_TABLE_LEAVE) {
            trade->trade_centre_state = TRADE_RESET;
            send = PKMN_TABLE_LEAVE;
            model->gameboy_status = GAMEBOY_READY;
            /* If the player selected a pokemon to send from the Gameboy */
        } else if((in & PKMN_SEL_NUM_MASK) == PKMN_SEL_NUM_MASK) {
            in_pkmn_idx = in;
            send = PKMN_SEL_NUM_ONE; // We always send the first pokemon
            model->gameboy_status = GAMEBOY_TRADE_PENDING;
            /* BLANKs are sent in a few places, we want to do nothing about them
	 * unless the Gameboy already sent us an index they want to trade.
	 */
        } else if(in == PKMN_BLANK) {
            if(in_pkmn_idx != 0) {
                send = 0;
                trade->trade_centre_state = TRADE_CONFIRMATION;
                in_pkmn_idx &= 0x0F;
            }
        }
        break;

    /* Handle the Gameboy accepting or rejecting a trade deal */
    case TRADE_CONFIRMATION:
        if(in == PKMN_TRADE_REJECT) {
            trade->trade_centre_state = TRADE_SELECT;
            model->gameboy_status = GAMEBOY_WAITING;
        } else if(in == PKMN_TRADE_ACCEPT) {
            trade->trade_centre_state = TRADE_DONE;
        }
        break;

    /* Start the actual trade. Waits in reset until the Gameboy is done with
     * its animation and re-exchanges updated party data.
     */
    case TRADE_DONE:
        if(in == PKMN_BLANK) {
            trade->trade_centre_state = TRADE_RESET;
            model->gameboy_status = GAMEBOY_TRADING;

            /* Copy the traded-in pokemon's main data to our struct */
            trade->trade_block->party_members[0] = trade->input_block->party_members[in_pkmn_idx];
            memcpy(
                &(trade->trade_block->party[0]),
                &(trade->input_block->party[in_pkmn_idx]),
                sizeof(struct pokemon_structure));
            memcpy(
                &(trade->trade_block->nickname[0]),
                &(trade->input_block->nickname[in_pkmn_idx]),
                sizeof(struct name));
            memcpy(
                &(trade->trade_block->ot_name[0]),
                &(trade->input_block->ot_name[in_pkmn_idx]),
                sizeof(struct name));
            model->curr_pokemon = pokemon_table_get_num_from_index(
                trade->pokemon_table, trade->trade_block->party_members[0]);

            /* Schedule a callback outside of ISR context to rebuild the patch
	     * list with the new pokemon that we just accepted.
	     */
            furi_timer_pending_callback(pokemon_plist_recreate_callback, trade, 0);
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

    struct trade_ctx* trade = (struct trade_ctx*)context;
    render_gameboy_state_t status;

    with_view_model(
        trade->view, struct trade_model * model, { status = model->gameboy_status; }, false);

    /* Shift data in every clock */
    trade->in_data <<= 1;
    trade->in_data |= furi_hal_gpio_read(&GAME_BOY_SI);
    trade->shift++;

    /* Once a byte of data has been shifted in, process it */
    if(trade->shift == 8) {
        trade->shift = 0;
        switch(status) {
        case GAMEBOY_CONN_FALSE:
            trade->out_data = getConnectResponse(trade);
            break;
        case GAMEBOY_CONN_TRUE:
            trade->out_data = getMenuResponse(trade);
            break;
        case GAMEBOY_COLOSSEUM:
            trade->out_data = trade->in_data;
            break;
        /* Every other state is trade related */
        default:
            trade->out_data = getTradeCentreResponse(trade);
            break;
        }
    }
}

void input_clk_gameboy(void* context) {
    furi_assert(context);

    struct trade_ctx* trade = (struct trade_ctx*)context;
    static uint32_t time;
    /* Clocks idle between bytes is nominally 430 us long for burst data,
     * 15 ms for idle polling (e.g. waiting for menu selection), some oddball
     * 2 ms gaps that appears between one 0xFE byte from the gameboy every trade;
     * clock period is nominally 122 us.
     * Therefore, if we havn't seen a clock in 500 us, reset our bit counter.
     * Note that, this should never actually be a concern, but it is an additional
     * safeguard against desyncing.
     */
    const uint32_t time_ticks = furi_hal_cortex_instructions_per_microsecond() * 500;

    if(furi_hal_gpio_read(&GAME_BOY_CLK)) {
        if((DWT->CYCCNT - time) > time_ticks) {
            trade->in_data = 0;
            trade->shift = 0;
        }
        transferBit(trade);
        time = DWT->CYCCNT;
    } else {
        /* On the falling edge of each clock, set up the next bit */
        furi_hal_gpio_write(&GAME_BOY_SO, !!(trade->out_data & 0x80));
        trade->out_data <<= 1;
    }
}

void trade_enter_callback(void* context) {
    furi_assert(context);
    struct trade_ctx* trade = (struct trade_ctx*)context;
    struct trade_model* model;

    model = view_get_model(trade->view);

    if(model->gameboy_status == GAMEBOY_COLOSSEUM) {
        model->gameboy_status = GAMEBOY_CONN_FALSE;
    } else if(model->gameboy_status > GAMEBOY_READY) {
        model->gameboy_status = GAMEBOY_READY;
    }
    trade->trade_centre_state = TRADE_RESET;
    model->pokemon_table = trade->pokemon_table;
    model->curr_pokemon = pokemon_table_get_num_from_index(
        trade->pokemon_table, trade->trade_block->party_members[0]);
    model->ledon = false;

    view_commit_model(trade->view, true);

    trade->in_data = 0;
    trade->out_data = 0;
    trade->shift = 0;

    /* Every 250 ms, trigger a draw update. 250 ms was chosen so that during
     * the trade process, each update can flip the LED and screen to make the
     * trade animation.
     */
    trade->draw_timer = furi_timer_alloc(trade_draw_timer_callback, FuriTimerTypePeriodic, trade);
    furi_timer_start(trade->draw_timer, furi_ms_to_ticks(250));

    // B3 (Pin6) / SO (2)
    furi_hal_gpio_write(&GAME_BOY_SO, false);
    furi_hal_gpio_init(&GAME_BOY_SO, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    // B2 (Pin5) / SI (3)
    furi_hal_gpio_write(&GAME_BOY_SI, false);
    furi_hal_gpio_init(&GAME_BOY_SI, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
    // // C3 (Pin7) / CLK (5)
    furi_hal_gpio_init(&GAME_BOY_CLK, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_remove_int_callback(&GAME_BOY_CLK);

    furi_hal_gpio_add_int_callback(&GAME_BOY_CLK, input_clk_gameboy, trade);

    /* Create a trade patch list from the current trade block */
    plist_create(&(trade->patch_list), trade->trade_block);
}

void disconnect_pin(const GpioPin* pin) {
    /* Existing projects seem to set the pin back to analog mode upon exit */
    furi_hal_gpio_init_simple(pin, GpioModeAnalog);
}

void trade_exit_callback(void* context) {
    furi_assert(context);

    struct trade_ctx* trade = (struct trade_ctx*)context;

    furi_hal_light_set(LightGreen, 0x00);
    furi_hal_light_set(LightBlue, 0x00);
    furi_hal_light_set(LightRed, 0x00);

    /* Stop the timer, and deallocate it as the enter callback allocates it on entry */
    furi_timer_free(trade->draw_timer);
    trade->draw_timer = NULL;

    /* Destroy the patch list, it is allocated on the enter callback */
    plist_free(trade->patch_list);
    trade->patch_list = NULL;
}

void* trade_alloc(
    TradeBlock* trade_block,
    const PokemonTable* table,
    ViewDispatcher* view_dispatcher,
    uint32_t view_id) {
    furi_assert(trade_block);

    struct trade_ctx* trade = malloc(sizeof(struct trade_ctx));

    memset(trade, '\0', sizeof(struct trade_ctx));
    trade->view = view_alloc();
    trade->trade_block = trade_block;
    trade->input_block = malloc(sizeof(TradeBlock));
    trade->pokemon_table = table;
    trade->patch_list = NULL;

    view_set_context(trade->view, trade);
    view_allocate_model(trade->view, ViewModelTypeLockFree, sizeof(struct trade_model));

    view_set_draw_callback(trade->view, trade_draw_callback);
    view_set_enter_callback(trade->view, trade_enter_callback);
    view_set_exit_callback(trade->view, trade_exit_callback);

    view_dispatcher_add_view(view_dispatcher, view_id, trade->view);

    return trade;
}

void trade_free(ViewDispatcher* view_dispatcher, uint32_t view_id, void* trade_ctx) {
    furi_assert(trade_ctx);

    struct trade_ctx* trade = (struct trade_ctx*)trade_ctx;

    // Free resources
    furi_hal_gpio_remove_int_callback(&GAME_BOY_CLK);
    disconnect_pin(&GAME_BOY_CLK);

    view_dispatcher_remove_view(view_dispatcher, view_id);

    view_free_model(trade->view);
    view_free(trade->view);
    free(trade->input_block);
    free(trade);
}
