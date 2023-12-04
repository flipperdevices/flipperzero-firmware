#include "../gba_cartridge_app.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include <stdio.h>   // Para sprintf
#include <string.h>  // Para strlen

struct GBACartridgeScene1 {
    View* view;
    GBACartridgeScene1Callback callback;
    void* context;
    GBACartridge* app;
};

typedef struct  {
    char* cart_title;
    char* cart_serial;
    char* cart_checksum;
    char* cart_ROMSize;
    char* cart_RAMSize;
    char* cart_gb_type;
    bool cart_gb_sgb;
    char* cart_logo;
    // int ramBanks;
    // int romBanks;
    // uint8_t cart_logo[48*8];

} GameBoyCartridgeModel;



void gameboy_information_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    UNUSED(len);
    UNUSED(buf);
    GBACartridge* instance = context;

    with_view_model(
        instance->gba_cartridge_scene_1->view,
        GameBoyCartridgeModel * model,
        {
            cJSON* json = cJSON_Parse((char*)buf);
            if (json == NULL) {
                model->cart_title = "Parse error";
            } else {;
                //  Title to LONG
                // cJSON* title =  cJSON_GetObjectItemCaseSensitive(json, "title");
                // if (cJSON_IsString(title) && (title->valuestring != NULL))
                // {
                //     model->cart_title = strdup(title->valuestring);
                    
                // } else {
                //     model->cart_title = "None";
                // }
                //  Title
                cJSON* title =  cJSON_GetObjectItemCaseSensitive(json, "rom_title");
                if (cJSON_IsString(title) && (title->valuestring != NULL))
                {
                    model->cart_title = strdup(title->valuestring);
                    
                } else {
                    model->cart_title = "None";
                }
                //  Serial
                cJSON* serial =  cJSON_GetObjectItemCaseSensitive(json, "serial");
                if (cJSON_IsString(serial) && (serial->valuestring != NULL))
                {
                    model->cart_serial = strdup(serial->valuestring);
                } else {
                    model->cart_serial = "";
                }                
                //  Checksum
                cJSON* checksum =  cJSON_GetObjectItemCaseSensitive(json, "checksum");
                if (cJSON_IsString(checksum) && (checksum->valuestring != NULL))
                {
                    model->cart_checksum = strdup(checksum->valuestring);
                } else {
                    model->cart_checksum = "None";
                }
                //  ROMSize

                cJSON* ROMSize = cJSON_GetObjectItemCaseSensitive(json, "ROMSize");
                if (cJSON_IsNumber(ROMSize)) {
                    char buffer[20];  // Tamaño suficiente para almacenar un entero
                    snprintf(buffer, sizeof(buffer), "%d", ROMSize->valueint);
                    model->cart_ROMSize = strdup(buffer);  // Asigna una copia dinámica de la cadena
                } else {
                    model->cart_ROMSize = NULL;  // Puedes asignar NULL o cualquier otro valor predeterminado
                }

                //  RAMSize
                cJSON* RAMSize =  cJSON_GetObjectItemCaseSensitive(json, "RAMSize");
                if (cJSON_IsString(RAMSize) && (RAMSize->valuestring != NULL))
                {
                    model->cart_RAMSize = strdup(RAMSize->valuestring);
                } else {
                    model->cart_RAMSize = "None";
                }
                //  GB Type
                cJSON* gb_type =  cJSON_GetObjectItemCaseSensitive(json, "gb_type");
                if (cJSON_IsString(gb_type) && (gb_type->valuestring != NULL))
                {
                    model->cart_gb_type = strdup(gb_type->valuestring);
                } else {
                    model->cart_gb_type = "dump";
                }


                cJSON* logo_correct =  cJSON_GetObjectItemCaseSensitive(json, "logo_correct");
                if (cJSON_IsBool(logo_correct))
                {
                    model->cart_logo = cJSON_IsTrue(logo_correct) ? "OK" : "NO";
                } else {
                    model->cart_logo = "NO";
                }

                FuriString* path = furi_string_alloc();
                //  int buffer_size = strlen(model->cart_title) + strlen(model->cart_serial) + strlen(model->cart_gb_type) + 3; // 3 para los guiones bajos y el punto
                // char filename[255];
                if (strcmp(model->cart_serial, "") == 0) {
                    furi_string_cat_printf(path, "%s", model->cart_title);
                } else {
                    furi_string_cat_printf(path, "%s_%s", model->cart_title, model->cart_serial);
                }
                // snprintf(filename,255,  "%s_%s.%s", model->cart_title, model->cart_serial, model->cart_gb_type);
                instance->cart_dump_rom_filename = (char*)furi_string_get_cstr(path);
                instance->cart_dump_rom_extension = model->cart_gb_type;

                instance->cart_dump_ram_filename = (char*)furi_string_get_cstr(path);
                instance->cart_dump_ram_extension = "sav";

                // instance->rom_banks = model->romBanks;
                // instance->ram_banks = model->ramBanks;
                
            }
        },true);
}
void gba_cartridge_scene_1_set_callback(
    GBACartridgeScene1* instance,
    GBACartridgeScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
    instance->app = (GBACartridge*)context;
}


void gba_cartridge_scene_1_draw(Canvas* canvas, GameBoyCartridgeModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    // canvas_set_color(canvas, ColorBlack);
    // canvas_set_font(canvas, FontPrimary);
    // canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "This is Scene 1"); 
    // canvas_set_font(canvas, FontSecondary);
    // canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, "An empty scene to be"); 
    // canvas_draw_str_aligned(canvas, 0, 32, AlignLeft, AlignTop, "used as boilerplate"); 
    // Clear the screen.
    canvas_set_color(canvas, ColorBlack);

    canvas_set_bitmap_mode(canvas, 1);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 128 / 2, 1, AlignCenter, AlignTop, model->cart_title);
    // canvas_draw_str_aligned(canvas, 128 / 2, 1, AlignCenter, AlignTop, "Prueba"); //  title
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 20, "Game Code / REV");
    canvas_set_font(canvas, FontPrimary);
    // canvas_draw_str(canvas, 87, 20, "APSS-0");  //  serial
    canvas_draw_str_aligned(canvas, 126, 20,  AlignRight, AlignBottom, model->cart_serial);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 30, "Boot Logo");    
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 126, 30,  AlignRight, AlignBottom, model->cart_logo);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 40, "ROM Checksum"); 
    canvas_set_font(canvas, FontPrimary);
    // canvas_draw_str(canvas, 87, 40, "0X04C7"); //  checksum
    canvas_draw_str_aligned(canvas, 126, 39,  AlignRight, AlignBottom, model->cart_checksum);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 50, "ROM Size");
    canvas_set_font(canvas, FontPrimary);
    // canvas_draw_str(canvas, 98, 49, "1 MiB"); //  ROMSize
    canvas_draw_str_aligned(canvas, 126, 49,  AlignRight, AlignBottom, model->cart_ROMSize);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 60, "Save Type");
    canvas_set_font(canvas, FontPrimary);
    // canvas_draw_str(canvas, 63, 60, "SRAM 32KiB"); //  RAMSize
    canvas_draw_str_aligned(canvas, 126, 59,  AlignRight, AlignBottom, model->cart_RAMSize);
}

static void gba_cartridge_scene_1_model_init(GameBoyCartridgeModel* const model) {
    UNUSED(model);
    // FuriString* cart_title;
    // FuriString* cart_serial;
    // FuriString* cart_checksum;
    // FuriString* cart_ROMSize;
    // FuriString* cart_RAMSize;
    model->cart_title = "Loading...";
    model->cart_serial = "";
    model->cart_checksum = "";
    model->cart_ROMSize = "";
    model->cart_RAMSize = "";
    model->cart_gb_type = "";
    model->cart_logo = "";
   
}

bool gba_cartridge_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context); 
    GBACartridgeScene1* instance = context;
    bool consumed = false;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                // with_view_model(
                //     instance->view,
                //     GameBoyCartridgeModel * model,
                //     {
                //         UNUSED(model);
                //         instance->callback(GBACartridgeCustomEventScene1Back, instance->context);
                //     },
                //     true);
                consumed = true;
                break;
            case InputKeyOk:

                with_view_model(
                    ((GBACartridge*)instance->app)->gba_cartridge_scene_1->view,
                    GameBoyCartridgeModel * model,
                    { 
                        model->cart_title = "Refresh...";
                        model->cart_serial = "";
                        model->cart_checksum = "";
                        model->cart_ROMSize = "";
                        model->cart_RAMSize = "";
                        
                        // Register callbacks to receive data
                        uart_set_handle_rx_data_cb(((GBACartridge*)instance->app)->uart, gameboy_information_handle_rx_data_cb); // setup callback for general log rx thread
                        const char GBACartridge_command[] = "gbacartridge -i\n";
                        uart_tx((uint8_t*)GBACartridge_command, strlen(GBACartridge_command));
                    },
                    true);
                consumed = true;
                break;
            case InputKeyLeft:
            case InputKeyRight:
            case InputKeyUp:
            case InputKeyDown:
            
                with_view_model(
                    instance->view,
                    GameBoyCartridgeModel* model,
                    {
                        UNUSED(model);
                    },
                    true);
                consumed = true;
                break;
            case InputKeyMAX:
                break;
        }
    }
    return consumed;
}

void gba_cartridge_scene_1_exit(void* context) {
    furi_assert(context);
}

void gba_cartridge_scene_1_enter(void* context) {
    furi_assert(context);
    GBACartridgeScene1* instance = (GBACartridgeScene1*)context;
    with_view_model(
        instance->view,
        GameBoyCartridgeModel * model,
        {
            gba_cartridge_scene_1_model_init(model);
        },
        true
    );
    
    // Register callbacks to receive data
    uart_set_handle_rx_data_cb(((GBACartridge*)instance->app)->uart, gameboy_information_handle_rx_data_cb); // setup callback for general log rx thread
    const char GBACartridge_command[] = "gbacartridge -i\n";
    uart_tx((uint8_t*)GBACartridge_command, strlen(GBACartridge_command));
}

GBACartridgeScene1* gba_cartridge_scene_1_alloc() {
    GBACartridgeScene1* instance = malloc(sizeof(GBACartridgeScene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(GameBoyCartridgeModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)gba_cartridge_scene_1_draw);
    view_set_input_callback(instance->view, gba_cartridge_scene_1_input);
    view_set_enter_callback(instance->view, gba_cartridge_scene_1_enter);
    view_set_exit_callback(instance->view, gba_cartridge_scene_1_exit);

    with_view_model(
        instance->view,
        GameBoyCartridgeModel * model,
        {
            gba_cartridge_scene_1_model_init(model);
        },
        true
    );
    

    return instance;
}

void gba_cartridge_scene_1_free(GBACartridgeScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        GameBoyCartridgeModel * model,
        {
            UNUSED(model);
        },
        true);
    view_free(instance->view);
    free(instance);
}

View* gba_cartridge_scene_1_get_view(GBACartridgeScene1* instance) {
    furi_assert(instance);
    return instance->view;
}

