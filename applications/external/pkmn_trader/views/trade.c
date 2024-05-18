/*
 * NOTE: 
 * The documentation below is slightly out of date but mostly still correct,
 * and only for gen i trades. Gen ii trades are very similar but have a few
 * different patterns. I'm currently lazy and working on features, so better
 * documentation on the trade protocol to follow, and potentially will push
 * it all to bulbapedia or similar for the world to benefit from.
 *
 *
 * This setup always forces the flipper to the follower/slave role in the link.
 * This just makes our logic consistent and since we're going to be gobs faster
 * than a real Game Boy, we can be guaranteed to always be ready to respond.
 *
 * As documented here: http://www.adanscotney.com/2014/01/spoofing-pokemon-trades-with-stellaris.html
 * The general gist of the communication is as follows:
 * 1) Each Game Boy tries to listen for an external clock coming in on the link cable.
 *     After some unknown timeout, this Game Boy decides its going to take the leader/master role.
 *     In this state, it generates a clock and repeatedly sends out PKMN_MASTER(0x01)
 * 2) The other side, sensing a clock from the leader/master, then responds with PKMN_SLAVE(0x02)
 * 3) Once both sides understand their roles, they both respond with PKMN_BLANK(0x00) for a bit.
 * 4) Next, the leader/master sends CONNECTED(0x60) bytes that the follower/slave repeats
 *     back. Then a bunch of BLANK bytes.
 * 5) At this point, each Game Boy repeatedly sends the menu item it has highlighted,
 *     prepended by a BLANK, in groups of 3 bytes. These are ITEM_*_HIGHLIGHTED.
 * 6) Then, once both sides send ITEM_*_SELECTED, the next step occurs.
 *     This application, from steps 3 through 6, just repeats bytes back and lets the Game Boy
 *     dictate the steps. We stay here until we start seeing PREAMBLE(0xFD) bytes,
 *     as those dictate the start of the next sections.
 *
 * The Flipper is now in the "READY" state.
 *
 * 7) Once the player on the Game Boy side uses the trade table, a block of data is
 *     transmitted. This starts with 10x PREAMBLE(0xFD) bytes, 10x random bytes (to
 *
 *     I missed another 9x fd bytes after rand? State machine below confirms these bytes
 *
 *     sync the RNG between two devices, unused at this time), and then the 415 trade_block,
 *     struct gets transferred. At the end of this is 3 ending bytes, DF FE 15. And, weirdly,
 *     3 PREAMBLE(0xFD) bytes.
 * 8) The patch list starts with 3x more PREAMBLE(0xFD) bytes for a total of 6x PREAMBLE,
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
 * 9) At this point, both sides have full copies of each other's current party. The sides
 *     simply indicate which Pokemon they are sending. This is done with a BLANK byte to
 *     start, and then each side indicates which Pokemon it wants to trade with SEL_NUM_MASK(0x60)
 *     + party index. We always transmit the first Pokemon. Once in a agreement, both
 *     sides transmit a handful of BLANK bytes.
 *
 * The Flipper is now in the "DEAL?" state.
 *
 * A) Starting with another BLANK byte, both sides need to agree to the trade by
 *     sending TRADE_ACCEPT(0x62) repeatedly, and then a handful of BLANK bytes.
 *     To disagree with a trade, either side would send TRADE_REJECT(0x61), the
 *     Flipper will never send this on its own. If the Game Boy does, both it and
 *     the flipper got back to step 9 again.
 *
 * The Flipper is now in the "TRADING" state.
 *
 * B) The Flipper actually goes back to step 7, but keeps the drawing mode as
 *     TRADING. After the trade is complete on the Game Boy, it re-sends the
 *     trade_block data. This re-syncs the states between the Flipper and
 *     Game Boy and another trade can occur.
 *
 * *) A point of note is that the Flipper can go back to the main menu from
 *     any state. Though, doing so in the TRADING state might actually cause
 *     the Game Boy to have issues. When in READY or WAITING state, the Flipper
 *     can go back and modify the Pokemon that the Game Boy sent to it. If the
 *     Flipper then goes back to Trade from the main menu, it will be in the
 *     READY state. If the Game Boy is still on the trade menu, and it tries
 *     to trade, the trade will be rejected. The Game Boy needs to exit the
 *     trade menu, and then re-enter it by selecting the table in the trade
 *     center. This will then push the Flipper to the WAITING state, and the
 *     trade_blocks will re-sync between them with the new data. If the Game Boy
 *     leave the trade menu while the Flipper is in the WAITING state, the
 *     Flipper will go back to the READY state.
 */

#include <furi.h>
#include <furi_hal.h>

#include <dolphin/dolphin.h>
#include <notification/notification_messages.h>

#include <gui/elements.h>
#include <gui/view.h>
#include <pokemon_icons.h>
#include <gblink.h>

#include "../pokemon_app.h"
#include "../pokemon_data.h"
#include "trade_patch_list.h"

/* Uncomment the following line to enable graphics testing for the different
 * phases of the trade view. Pressing the okay button will step through each
 * gameboy_status. Note that while trades will still function with this enabled,
 * forcing the advance of the status will certainly break trades.
 */
//#define GRAPHICS_TESTING

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
#define PKMN_CONNECTED_II 0x61
#define PKMN_TRADE_ACCEPT_GEN_I 0x62
#define PKMN_TRADE_ACCEPT_GEN_II 0x72
#define PKMN_TRADE_REJECT_GEN_I 0x61
#define PKMN_TRADE_REJECT_GEN_II 0x71
#define PKMN_TABLE_LEAVE_GEN_I 0x6f
#define PKMN_TABLE_LEAVE_GEN_II 0x7f
#define PKMN_SEL_NUM_MASK_GEN_I 0x60
#define PKMN_SEL_NUM_MASK_GEN_II 0x70
#define PKMN_SEL_NUM_ONE_GEN_I 0x60
#define PKMN_SEL_NUM_ONE_GEN_II 0x70

#define PKMN_ACTION 0x60

#define PKMN_TRADE_CENTRE ITEM_1_SELECTED
#define PKMN_COLOSSEUM ITEM_2_SELECTED
#define PKMN_BREAK_LINK ITEM_3_SELECTED

struct important_bytes {
    const uint8_t connected;
    const uint8_t trade_accept;
    const uint8_t trade_reject;
    const uint8_t table_leave;
    const uint8_t sel_num_mask;
    const uint8_t sel_num_one;
};

static const struct important_bytes gen_i = {
    PKMN_CONNECTED,
    PKMN_TRADE_ACCEPT_GEN_I,
    PKMN_TRADE_REJECT_GEN_I,
    PKMN_TABLE_LEAVE_GEN_I,
    PKMN_SEL_NUM_MASK_GEN_I,
    PKMN_SEL_NUM_ONE_GEN_I,
};

static const struct important_bytes gen_ii = {
    PKMN_CONNECTED_II,
    PKMN_TRADE_ACCEPT_GEN_II,
    PKMN_TRADE_REJECT_GEN_II,
    PKMN_TABLE_LEAVE_GEN_II,
    PKMN_SEL_NUM_MASK_GEN_II,
    PKMN_SEL_NUM_ONE_GEN_II,
};

/* States specific to the trade process. */
typedef enum {
    TRADE_RESET,
    TRADE_INIT,
    TRADE_RANDOM,
    TRADE_DATA,
    TRADE_PATCH_HEADER,
    TRADE_PATCH_DATA,
    TRADE_SELECT,
    TRADE_MAIL,
    TRADE_PENDING,
    TRADE_CONFIRMATION,
    TRADE_DONE,
    TRADE_CANCEL
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
    GAMEBOY_TRADE_CANCEL,
    GAMEBOY_COLOSSEUM,
    GAMEBOY_STATE_COUNT
} render_gameboy_state_t;

/* Anonymous struct */
struct trade_ctx {
    trade_centre_state_t trade_centre_state;
    FuriTimer* draw_timer;
    View* view;
    uint8_t in_data;
    uint8_t out_data;
    uint8_t shift;
    PokemonData* input_pdata;
    struct patch_list* patch_list;
    void* gblink_handle;
    struct gblink_pins* gblink_pins;
    PokemonData* pdata;
    NotificationApp* notifications;
};

/* These are the needed variables for the draw callback */
struct trade_model {
    render_gameboy_state_t gameboy_status;
    bool ledon; // Controls the blue LED during trade
    uint8_t curr_pokemon;
    PokemonData* pdata;
};

/* Input callback, used to handle the user trying to back out of the trade
 * screen.
 * Normally, when trade_centre_state is <= READY, pressing back would just go
 * back without issue. However, when WAITING, we need to tell the gameboy that
 * the flipper wants to exit the trade menu. Anything beyond WAITING should not
 * go back nor try to tell the gameboy to cancel; instead, by holding back in
 * these states, we can forcefully go back one menu.
 *
 * Returning false here then ends up calling the view_dispatcher nav callback
 * if the button pressed/held is Back. Returning true tells the OS that we
 * dealt with the button press and no further action is needed.
 */
static bool trade_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    struct trade_ctx* trade = context;
    render_gameboy_state_t gameboy_status;

#ifdef GRAPHICS_TESTING
    if(event->type == InputTypePress) {
        with_view_model(
            trade->view,
            struct trade_model * model,
            {
                if(event->key == InputKeyRight) {
                    model->gameboy_status++;
                    if(model->gameboy_status == GAMEBOY_STATE_COUNT)
                        model->gameboy_status = GAMEBOY_CONN_FALSE;
                } else if(event->key == InputKeyLeft) {
                    if(model->gameboy_status == GAMEBOY_CONN_FALSE)
                        model->gameboy_status = GAMEBOY_COLOSSEUM;
                    else
                        model->gameboy_status--;
                }
            },
            true);
    }
#endif

    /* Only handling back button */
    if(event->key != InputKeyBack) return false;

    with_view_model(
        trade->view,
        struct trade_model * model,
        { gameboy_status = model->gameboy_status; },
        false);

    /* States READY or lower can be exited without issue, let the view_dispatcher
	 * nav callback handle it.
	 */
    if(gameboy_status <= GAMEBOY_READY) return false;

    /* Long presses we want the view_dispatcher nav callback to handle */
    if(event->type == InputTypeLong) return false;

    /* In the waiting state, we need to move to cancelled. This locks us up
	 * until the gameboy side gets the hint and cancels as well.
	 */
    if(gameboy_status == GAMEBOY_WAITING && event->type == InputTypeShort) {
        with_view_model(
            trade->view,
            struct trade_model * model,
            { model->gameboy_status = GAMEBOY_TRADE_CANCEL; },
            false);
        trade->trade_centre_state = TRADE_CANCEL;
    }

    /* Anything here, we should consider handled */
    return true;
}

/* A callback function that must be called outside of an interrupt context,
 * This will completely destroy the current patch list, and then rebuild it from
 * the current trade_block state. This is used mostly after a trade to rebuild
 * the list with the new data we just copied in.
 */
static void pokemon_plist_recreate_callback(void* context, uint32_t arg) {
    furi_assert(context);
    UNUSED(arg);
    struct trade_ctx* trade = context;

    /* Award some XP to the dolphin after a completed trade. This needs to
     * happen outside of an ISR context, so we slap it here.
     */
    dolphin_deed(DolphinDeedPluginGameWin);
    plist_create(&(trade->patch_list), trade->pdata);
}

/* Call this at any point to reset the timer on the backlight turning off.
 * During trade, this should get called pretty frequently so long as data
 * is moving in and out.
 *
 * I hesitate to force the backlight on, as I don't want to be responsible
 * for draining someone's battery on accident.
 */
static void trade_backlight_bump_callback(void* context, uint32_t arg) {
    furi_assert(context);
    UNUSED(arg);
    struct trade_ctx* trade = context;

    notification_message(trade->notifications, &sequence_display_backlight_on);
}

static void trade_draw_bottom_bar(Canvas* const canvas) {
    furi_assert(canvas);

    /* Paint the area behind the bottom background bar white to prevent overlap */
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 53, 9, 7);
    canvas_draw_box(canvas, 6, 56, 59, 6);
    canvas_draw_box(canvas, 60, 53, 32, 7);
    canvas_draw_box(canvas, 87, 56, 38, 6);
    canvas_set_color(canvas, ColorBlack);

    /* Draw bar with transparencies */
    canvas_set_bitmap_mode(canvas, 1);
    canvas_draw_icon(canvas, 0, 53, &I_Background_128x11);
    canvas_set_bitmap_mode(canvas, 0);
}

/* Draws a whole screen image with Flipper mascot, Game Boy, etc. */
static void trade_draw_connection(Canvas* const canvas, bool connected) {
    furi_assert(canvas);

    canvas_draw_icon(canvas, 9, 26, &I_dolphin);
    trade_draw_bottom_bar(canvas);
    canvas_draw_icon(canvas, 80, 0, &I_game_boy);
    elements_frame(canvas, 9, 2, 64, 17);

    if(connected) {
        canvas_draw_str(canvas, 18, 13, "Connected!");
        canvas_draw_icon(canvas, 61, 23, &I_hand_thumbsup);
    } else {
        canvas_draw_str(canvas, 18, 13, "Connect GB");
        canvas_draw_icon(canvas, 56, 23, &I_hand_cable);
    }
}

/* Draws a frame around the screen, with a box at the top for a text string,
 * and an icon of the player.
 */
static void trade_draw_frame(Canvas* canvas, const char* str) {
    furi_assert(canvas);

    trade_draw_bottom_bar(canvas);

    /* Paint the area behind the text box white to prevent overlap, similar
     * to the bottom background bar */
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 59, 0, 67, 19);
    canvas_set_color(canvas, ColorBlack);

    /* Draw text box and populate it with string and Red icon */
    elements_frame(canvas, 59, 0, 67, 19);
    canvas_draw_str(canvas, 82, 12, str);
    canvas_draw_icon(canvas, 61, 2, &I_red_16x15);
}

/* Draws the Pokemon's image in the middle of the screen */
static void trade_draw_pkmn_avatar(Canvas* canvas, PokemonData* pdata) {
    furi_assert(canvas);
    furi_assert(pdata);

    /* First, ensure the icon we want is already loaded in to pdata->bitmap */
    pokemon_icon_get(pdata, pokemon_stat_get(pdata, STAT_NUM, NONE) + 1);
    canvas_draw_xbm(
        canvas, 0, 0, pdata->bitmap->width, pdata->bitmap->height, pdata->bitmap->data);

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

    canvas_clear(canvas);
    switch(model->gameboy_status) {
    case GAMEBOY_CONN_FALSE:
        furi_hal_light_set(LightGreen, 0x00);
        furi_hal_light_set(LightRed, 0xff);
        trade_draw_connection(canvas, false);
        break;
    case GAMEBOY_CONN_TRUE:
        furi_hal_light_set(LightGreen, 0xff);
        furi_hal_light_set(LightRed, 0x00);
        trade_draw_connection(canvas, true);
        break;
    case GAMEBOY_READY:
        trade_draw_pkmn_avatar(canvas, model->pdata);
        trade_draw_frame(canvas, "READY");
        break;
    case GAMEBOY_WAITING:
        trade_draw_pkmn_avatar(canvas, model->pdata);
        trade_draw_frame(canvas, "WAITING");
        break;
    case GAMEBOY_TRADE_PENDING:
        trade_draw_pkmn_avatar(canvas, model->pdata);
        trade_draw_frame(canvas, "DEAL?");
        break;
    case GAMEBOY_TRADING:
        furi_hal_light_set(LightGreen, 0x00);
        if(model->ledon) {
            furi_hal_light_set(LightBlue, 0xff);
            canvas_draw_icon(canvas, 0, 5, &I_gb_step_1);
        } else {
            furi_hal_light_set(LightBlue, 0x00);
            canvas_draw_icon(canvas, 0, 5, &I_gb_step_2);
        }
        trade_draw_frame(canvas, "TRADING");
        break;
    case GAMEBOY_TRADE_CANCEL:
        trade_draw_frame(canvas, "CANCEL");
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
    uint8_t ret = trade->in_data;

    switch(trade->in_data) {
    case PKMN_CONNECTED:
    case PKMN_CONNECTED_II:
        with_view_model(
            trade->view,
            struct trade_model * model,
            { model->gameboy_status = GAMEBOY_CONN_TRUE; },
            false);
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

/* Receive what the Pokemon game is requesting and move to that mode.
 *
 * This reads bytes sent by the Game Boy and responds. The only things
 * we care about are when menu items are actually selected. The protocol
 * seems to send data both when one of the link menu items is highlighted
 * and when one of them is selected.
 *
 * If somehow we get a leader/master byte received, then go back to the
 * NOT_CONNECTED state. For the leader/master byte likely means that
 * the linked Game Boy is still trying to negotiate roles and we need to
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
    case PKMN_CONNECTED_II:
        response = trade->in_data;
        break;
    case ITEM_2_HIGHLIGHTED:
        if(trade->pdata->gen == GEN_I) {
            response = trade->in_data;
            break;
        }
        [[fallthrough]];
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

    uint8_t* trade_block_flat = (uint8_t*)trade->pdata->trade_block;
    uint8_t* input_block_flat = (uint8_t*)trade->input_pdata->trade_block;
    uint8_t* input_party_flat = (uint8_t*)trade->input_pdata->party;
    struct trade_model* model = NULL;
    uint8_t in = trade->in_data;
    uint8_t send = in;
    static bool patch_pt_2;
    static size_t counter;
    static uint8_t in_pkmn_idx;
    const struct important_bytes* bytes = NULL;

    if(trade->pdata->gen == GEN_I) bytes = &gen_i;
    if(trade->pdata->gen == GEN_II) bytes = &gen_ii;

    /* TODO: Figure out how we should respond to a no_data_byte and/or how to
     * send one and what response to expect.
     *
     * This isn't a high priority since it should be unlikely that we would
     * actually ever receive a NO_DATA_BYE as the Game Boy is the leader/master
     * and therefore would only transmit when it has data ready.
     */

    /* Since this is a fairly long function, it doesn't call any other functions,
     * the view model isn't locked, and we're in an interrupt context, lets just
     * map the view model to a local var and commit it back when we're done.
     */
    model = view_get_model(trade->view);

    /* There is a handful of communications that happen once the Game Boy
     * clicks on the table. For all of them, the Flipper can just mirror back
     * the byte the Game Boy sends. We can spin in this forever until we see 10x
     * SERIAL_PREAMBLE_BYTEs. Once we receive those, the counters are synced,
     * and every byte after that can be easily counted for the actual transfer
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
        }
        if(counter == SERIAL_RNS_LENGTH) {
            trade->trade_centre_state = TRADE_RANDOM;
            counter = 0;
        }
        break;

    /* Once we start getting PKMN_BLANKs, we mirror them until we get 10x
     * SERIAL_PREAMBLE_BYTE, and then 10 random numbers. The 10 random
     * numbers are for synchronizing the PRNG between the two systems,
     * we do not use these numbers at this time.
     *
     * This waits through the end of the trade block preamble, a total of 19
     * bytes.
     */
    case TRADE_RANDOM:
        counter++;
        if(counter == (SERIAL_RNS_LENGTH + SERIAL_TRADE_PREAMBLE_LENGTH)) {
            trade->trade_centre_state = TRADE_DATA;
            counter = 0;
        }
        break;

    /* This is where we exchange trade_block data with the Game Boy */
    case TRADE_DATA:
        input_block_flat[counter] = in;
        send = trade_block_flat[counter];
        counter++;

        if(counter == trade->input_pdata->trade_block_sz) {
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
        if(counter > 8) {
            send = plist_index_get(trade->patch_list, (counter - 9));
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
	 * of patch list. But in practice, the Game Boy seems to transmit 3x
	 * preamble bytes, 7x 0x00, then 189 bytes for the patch list. A
	 * total of 199 bytes transmitted.
	 */
        /* Gen I and II patch lists seem to be the same length */
        if(counter == 196) {
            if(trade->pdata->gen == GEN_I)
                trade->trade_centre_state = TRADE_SELECT;
            else if(trade->pdata->gen == GEN_II)
                trade->trade_centre_state = TRADE_MAIL;

            counter = 0;
        }

        break;

    /* Preambled with 6x 0x20 bytes; 33*6 == 198 bytes of Mail, for each pokemon,
     * even if they have no mail set; 14*6 == 84 bytes, for each pokemon's mail,
     * the OT Name and ID; a 0xff; 100 zero bytes (unsure if they are always 0).
     * This is 6 + 198 + 84 + 1 + 100 == 389.
     */
    case TRADE_MAIL:
        counter++;
        if(counter == 389) trade->trade_centre_state = TRADE_SELECT;
        break;

    /* Resets the incoming Pokemon index, and once a BLANK byte is received,
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
    /* Handle the Game Boy selecting a Pokemon to trade, or leaving the table */
    /* XXX: TODO: Clean this up. Easiest is probably to use vars rather than
     * macros to check against and set output to.
     */
    case TRADE_PENDING:
        /* If the player leaves the trade menu and returns to the room */
        if(in == bytes->table_leave) {
            trade->trade_centre_state = TRADE_RESET;
            send = bytes->table_leave;
            model->gameboy_status = GAMEBOY_READY;
            /* If the player selected a Pokemon to send from the Game Boy */
        } else if((in & bytes->sel_num_mask) == bytes->sel_num_mask) {
            in_pkmn_idx = in;
            send = bytes->sel_num_one; // We always send the first pokemon
            model->gameboy_status = GAMEBOY_TRADE_PENDING;
            /* BLANKs are sent in a few places, we want to do nothing about them
	 * unless the Game Boy already sent us an index they want to trade.
	 */
        } else if(in == PKMN_BLANK) {
            if(in_pkmn_idx != 0) {
                send = 0;
                trade->trade_centre_state = TRADE_CONFIRMATION;
                in_pkmn_idx &= 0x0F;
            }
        }
        break;

    /* Handle the Game Boy accepting or rejecting a trade deal */
    case TRADE_CONFIRMATION:
        if(in == bytes->trade_reject) {
            trade->trade_centre_state = TRADE_SELECT;
            model->gameboy_status = GAMEBOY_WAITING;
        } else if(in == bytes->trade_accept) {
            trade->trade_centre_state = TRADE_DONE;
        }
        break;

    /* Start the actual trade. Waits in reset until the Game Boy is done with
     * its animation and re-exchanges updated party data.
     */
    case TRADE_DONE:
        if(in == PKMN_BLANK) {
            trade->trade_centre_state = TRADE_RESET;
            model->gameboy_status = GAMEBOY_TRADING;

            /* Copy the traded-in Pokemon's main data to our struct */
            pokemon_stat_memcpy(trade->pdata, trade->input_pdata, in_pkmn_idx);
            model->curr_pokemon = pokemon_stat_get(trade->pdata, STAT_NUM, NONE);

            /* Schedule a callback outside of ISR context to rebuild the patch
	     * list with the new Pokemon that we just accepted.
	     */
            furi_timer_pending_callback(pokemon_plist_recreate_callback, trade, 0);
        }
        break;

    case TRADE_CANCEL:
        if(in == bytes->table_leave) {
            trade->trade_centre_state = TRADE_RESET;
            model->gameboy_status = GAMEBOY_READY;
        }
        send = bytes->table_leave;
        break;

    default:
        // Do Nothing
        break;
    }

    view_commit_model(trade->view, false);

    return send;
}

static void transferBit(void* context, uint8_t in_byte) {
    furi_assert(context);

    struct trade_ctx* trade = (struct trade_ctx*)context;
    render_gameboy_state_t status;

    with_view_model(
        trade->view, struct trade_model * model, { status = model->gameboy_status; }, false);

    trade->in_data = in_byte;

    /* Once a byte of data has been shifted in, process it */
    switch(status) {
    case GAMEBOY_CONN_FALSE:
        gblink_transfer(trade->gblink_handle, getConnectResponse(trade));
        break;
    case GAMEBOY_CONN_TRUE:
        gblink_transfer(trade->gblink_handle, getMenuResponse(trade));
        break;
    case GAMEBOY_COLOSSEUM:
        gblink_transfer(trade->gblink_handle, in_byte);
        break;
    /* Every other state is trade related */
    default:
        gblink_transfer(trade->gblink_handle, getTradeCentreResponse(trade));
        break;
    }

    /* Each byte that comes in, bump the backlight timer so it stays on during a trade */
    furi_timer_pending_callback(trade_backlight_bump_callback, trade, 0);
}

void trade_enter_callback(void* context) {
    furi_assert(context);
    struct trade_ctx* trade = (struct trade_ctx*)context;
    struct trade_model* model;
    struct gblink_def gblink_def = {0};

    model = view_get_model(trade->view);

    if(model->gameboy_status == GAMEBOY_COLOSSEUM) {
        model->gameboy_status = GAMEBOY_CONN_FALSE;
    } else if(model->gameboy_status > GAMEBOY_READY) {
        model->gameboy_status = GAMEBOY_READY;
    }
    trade->trade_centre_state = TRADE_RESET;
    model->curr_pokemon = pokemon_stat_get(trade->pdata, STAT_NUM, NONE);
    model->ledon = false;

    view_commit_model(trade->view, true);

    gblink_def.pins = trade->gblink_pins;
    gblink_def.callback = transferBit;
    gblink_def.cb_context = trade;

    trade->gblink_handle = gblink_alloc(&gblink_def);
    gblink_nobyte_set(trade->gblink_handle, SERIAL_NO_DATA_BYTE);

    /* Every 250 ms, trigger a draw update. 250 ms was chosen so that during
     * the trade process, each update can flip the LED and screen to make the
     * trade animation.
     */
    trade->draw_timer = furi_timer_alloc(trade_draw_timer_callback, FuriTimerTypePeriodic, trade);
    furi_timer_start(trade->draw_timer, furi_ms_to_ticks(250));

    /* Create a trade patch list from the current trade block */
    plist_create(&(trade->patch_list), trade->pdata);
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

    /* Unset the pin settings */
    gblink_free(trade->gblink_handle);

    /* Destroy the patch list, it is allocated on the enter callback */
    plist_free(trade->patch_list);
    trade->patch_list = NULL;
}

void* trade_alloc(
    PokemonData* pdata,
    struct gblink_pins* gblink_pins,
    ViewDispatcher* view_dispatcher,
    uint32_t view_id) {
    furi_assert(pdata);

    struct trade_ctx* trade = malloc(sizeof(struct trade_ctx));

    memset(trade, '\0', sizeof(struct trade_ctx));
    trade->view = view_alloc();
    trade->pdata = pdata;
    trade->input_pdata = pokemon_data_alloc(pdata->gen);
    trade->patch_list = NULL;
    trade->gblink_pins = gblink_pins;
    trade->notifications = furi_record_open(RECORD_NOTIFICATION);

    view_set_context(trade->view, trade);
    view_allocate_model(trade->view, ViewModelTypeLockFree, sizeof(struct trade_model));
    with_view_model(
        trade->view, struct trade_model * model, { model->pdata = pdata; }, false);

    view_set_draw_callback(trade->view, trade_draw_callback);
    view_set_input_callback(trade->view, trade_input_callback);
    view_set_enter_callback(trade->view, trade_enter_callback);
    view_set_exit_callback(trade->view, trade_exit_callback);

    view_dispatcher_add_view(view_dispatcher, view_id, trade->view);

    return trade;
}

void trade_free(ViewDispatcher* view_dispatcher, uint32_t view_id, void* trade_ctx) {
    furi_assert(trade_ctx);

    struct trade_ctx* trade = (struct trade_ctx*)trade_ctx;

    view_dispatcher_remove_view(view_dispatcher, view_id);

    furi_record_close(RECORD_NOTIFICATION);

    view_free(trade->view);
    pokemon_data_free(trade->input_pdata);
    free(trade);
}
