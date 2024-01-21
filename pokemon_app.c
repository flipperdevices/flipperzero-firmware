#include <furi_hal_light.h>
#include <pokemon_icons.h>

#include "scenes/pokemon_menu.h"
#include "views/trade.h"
#include "views/select_pokemon.h"
#include "pokemon_app.h"
#include "pokemon_data.h"
#include "pokemon_char_encode.h"

/* Allocates a chunk of memory for the trade data block and sets up some
 * default values.
 */
/* TODO: Probably move this to pokemon_data */
static TradeBlock* trade_block_alloc(void) {
    TradeBlock* trade;

    trade = malloc(sizeof(TradeBlock));

    /* Clear struct to be all TERM_ bytes as the various name strings need this */
    memset(trade, TERM_, sizeof(TradeBlock));

    /* The party_members element needs to be 0xff for unused */
    memset(trade->party_members, 0xFF, sizeof(trade->party_members));

    /* Zero the main party data, TERM_ in there can cause weirdness */
    memset(trade->party, 0x00, sizeof(trade->party));

    /* Set our Name, the pokemon's default OT name and ID */
    trade->party_cnt = 1;

    /* Trainer/OT name, not to exceed 7 characters! */
    pokemon_name_set(trade, STAT_TRAINER_NAME, "Flipper");
    pokemon_name_set(trade, STAT_OT_NAME, "Flipper");

    /* OT trainer ID# */
    pokemon_stat_set(trade, STAT_OT_ID, NONE, 42069);

    /* Notes:
     * Move pp isn't explicitly set up, should be fine
     * Catch/held isn't explicitly set up, should be okay for only Gen I support now
     * Status condition isn't explicity let up, would you ever want to?
     */

    /* Set up initial pokemon and level */
    /* This causes all other stats to be recalculated */
    pokemon_stat_set(trade, STAT_NUM, NONE, 0); // First Pokemon
    pokemon_stat_set(trade, STAT_LEVEL, NONE, 2); // Minimum level of 2

    return trade;
}

static void trade_block_free(TradeBlock* trade) {
    free(trade);
}

/* The MALVEKE board has an esp32 which is set to TX on the flipper's default
 * UART pins. If this pin shows signs of something connected, assume a MALVEKE
 * board is being used.
 */
static bool detect_malveke(void) {
    bool rc;

    furi_hal_gpio_init(&gpio_usart_rx, GpioModeInput, GpioPullDown, GpioSpeedVeryHigh);
    furi_hal_gpio_init(&gpio_swdio, GpioModeInput, GpioPullDown, GpioSpeedVeryHigh);
    /* Delay a tick to let the IO pin changes settle */
    furi_delay_tick(1);
    rc = furi_hal_gpio_read(&gpio_usart_rx);
    /* XXX: HACK: Need to clean this up later, but, newer MALVEKE boards use the
     * original pinout. Using a second pin to detect if there is a pullup to
     * determine if this is the board in use. In the future, it looks like the
     * GPIO module auto-detect support might be the better way here.
     */
    if(furi_hal_gpio_read(&gpio_swdio)) rc = 0;
    furi_hal_gpio_init_simple(&gpio_usart_rx, GpioModeAnalog);
    furi_hal_gpio_init_simple(&gpio_swdio, GpioModeAnalog);

    return rc;
}

PokemonFap* pokemon_alloc() {
    PokemonFap* pokemon_fap = (PokemonFap*)malloc(sizeof(PokemonFap));

    // View dispatcher
    pokemon_fap->view_dispatcher = view_dispatcher_alloc();

    view_dispatcher_enable_queue(pokemon_fap->view_dispatcher);
    view_dispatcher_set_event_callback_context(pokemon_fap->view_dispatcher, pokemon_fap);
    view_dispatcher_attach_to_gui(
        pokemon_fap->view_dispatcher,
        (Gui*)furi_record_open(RECORD_GUI),
        ViewDispatcherTypeFullscreen);

    // Set up defaults
    pokemon_fap->malveke_detected = detect_malveke();
    memcpy(
        &pokemon_fap->pins,
        &common_pinouts[pokemon_fap->malveke_detected],
        sizeof(struct gblink_pins));

    // Set up trade party struct
    pokemon_fap->trade_block = trade_block_alloc();

    /* Set up gui modules used. It would be nice if these could be allocated and
     * freed as needed, however, the scene manager still requires pointers that
     * get set up as a part of the scene. Therefore, individual scene's exit
     * callbacks cannot free the buffer.
     */
    pokemon_fap->text_input = text_input_alloc();
    pokemon_fap->submenu = submenu_alloc();
    pokemon_fap->variable_item_list = variable_item_list_alloc();

    // Set up menu scene
    pokemon_fap->scene_manager = scene_manager_alloc(&pokemon_scene_manager_handlers, pokemon_fap);
    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher, AppViewMainMenu, submenu_get_view(pokemon_fap->submenu));
    scene_manager_next_scene(pokemon_fap->scene_manager, MainMenuScene);

    // Select Pokemon View
    /* Allocates its own view and adds it to the main view_dispatcher */
    pokemon_fap->select = select_pokemon_alloc(pokemon_fap, pokemon_fap->view_dispatcher, pokemon_fap->scene_manager, AppViewSelectPokemon);

    // Trade View
    /* Allocates its own view and adds it to the main view_dispatcher */
    pokemon_fap->trade = trade_alloc(
        pokemon_fap->trade_block, &pokemon_fap->pins, pokemon_fap->view_dispatcher, AppViewTrade);

    return pokemon_fap;
}

void free_app(PokemonFap* pokemon_fap) {
    furi_assert(pokemon_fap);

    // Free views
    /* These each remove themselves from the view_dispatcher */
    select_pokemon_free(pokemon_fap->view_dispatcher, AppViewSelectPokemon, pokemon_fap->select);
    trade_free(pokemon_fap->view_dispatcher, AppViewTrade, pokemon_fap->trade);

    view_dispatcher_remove_view(pokemon_fap->view_dispatcher, AppViewMainMenu);

    view_dispatcher_free(pokemon_fap->view_dispatcher);

    // Free scenes
    scene_manager_free(pokemon_fap->scene_manager);

    // Free gui modules
    submenu_free(pokemon_fap->submenu);
    text_input_free(pokemon_fap->text_input);
    variable_item_list_free(pokemon_fap->variable_item_list);

    // Close records
    furi_record_close(RECORD_GUI);

    // Free trade block
    trade_block_free(pokemon_fap->trade_block);

    // Free rest
    free(pokemon_fap);
    pokemon_fap = NULL;
}

int32_t pokemon_app(void* p) {
    UNUSED(p);
    PokemonFap* pokemon_fap = pokemon_alloc();

    furi_hal_light_set(LightRed, 0x00);
    furi_hal_light_set(LightGreen, 0x00);
    furi_hal_light_set(LightBlue, 0x00);

    //switch view and run dispatcher
    view_dispatcher_run(pokemon_fap->view_dispatcher);

    // Free resources
    free_app(pokemon_fap);

    return 0;
}
