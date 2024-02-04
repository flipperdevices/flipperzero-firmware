#include <furi_hal_light.h>
#include <pokemon_icons.h>

#include "pokemon_app.h"
#include "pokemon_data.h"
#include "scenes/pokemon_menu.h"
#include "views/trade.h"
#include "views/select_pokemon.h"
#include "pokemon_char_encode.h"

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
    memcpy(&pokemon_fap->pins, &common_pinouts[PINOUT_ORIGINAL], sizeof(struct gblink_pins));

    /* Set up gui modules used. It would be nice if these could be allocated and
     * freed as needed, however, the scene manager still requires pointers that
     * get set up as a part of the scene. Therefore, individual scene's exit
     * callbacks cannot free the buffer.
     *
     * In order to do this properly, I think each scene, or maybe common to all
     * scenes, would end up needing to set a delayed callback of some kind. But
     * I'm not sure how to guarantee this gets called in a reasonable amount of
     * time.
     */
    pokemon_fap->text_input = text_input_alloc();
    pokemon_fap->submenu = submenu_alloc();
    pokemon_fap->variable_item_list = variable_item_list_alloc();
    pokemon_fap->dialog_ex = dialog_ex_alloc();

    // Set up menu scene
    pokemon_fap->scene_manager = scene_manager_alloc(&pokemon_scene_manager_handlers, pokemon_fap);
    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher, AppViewMainMenu, submenu_get_view(pokemon_fap->submenu));
    scene_manager_next_scene(pokemon_fap->scene_manager, MainMenuScene);

    return pokemon_fap;
}

void free_app(PokemonFap* pokemon_fap) {
    furi_assert(pokemon_fap);

    view_dispatcher_remove_view(pokemon_fap->view_dispatcher, AppViewMainMenu);

    view_dispatcher_free(pokemon_fap->view_dispatcher);

    // Free scenes
    scene_manager_free(pokemon_fap->scene_manager);

    // Free gui modules
    submenu_free(pokemon_fap->submenu);
    text_input_free(pokemon_fap->text_input);
    variable_item_list_free(pokemon_fap->variable_item_list);
    dialog_ex_free(pokemon_fap->dialog_ex);

    // Close records
    furi_record_close(RECORD_GUI);

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
