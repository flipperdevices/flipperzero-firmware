#include "../gb_cartridge_app.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include <stdio.h>   // Para sprintf
#include <string.h>  // Para strlen

struct GBCartridgeScene1 {
    View* view;
    GBCartridgeScene1Callback callback;
    void* context;
    GBCartridge* app;
};

typedef struct  {
    char* cart_title;
    char* cart_serial;
    char* cart_checksum;
    char* cart_ROMSize;
    char* cart_RAMSize;
    char* cart_gb_type;
    bool cart_gb_sgb;
    int ramBanks;
    int romBanks;
    uint8_t cart_logo[48*8];

} GameBoyCartridgeModel;



void gameboy_information_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    UNUSED(len);
    UNUSED(buf);
    GBCartridge* instance = context;

    with_view_model(
        instance->gb_cartridge_scene_1->view,
        GameBoyCartridgeModel * model,
        {
            cJSON* json = cJSON_Parse((char*)buf);
            if (json == NULL) {
                model->cart_title = "Parse error";
            } else {;
                //  Title
                cJSON* title =  cJSON_GetObjectItemCaseSensitive(json, "title");
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
                cJSON* ROMSize =  cJSON_GetObjectItemCaseSensitive(json, "ROMSize");
                if (cJSON_IsString(ROMSize) && (ROMSize->valuestring != NULL))
                {
                    model->cart_ROMSize = strdup(ROMSize->valuestring);
                } else {
                    model->cart_ROMSize = "None";
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

                
                //  SGB ?
                cJSON* gb_sgb =  cJSON_GetObjectItemCaseSensitive(json, "gb_sgb");
                if (cJSON_IsBool(gb_sgb))
                {
                    model->cart_gb_sgb = cJSON_IsTrue(gb_sgb);
                } else {
                    model->cart_gb_sgb = false;
                }

                //  Rom Banks
                cJSON* romBanks = cJSON_GetObjectItemCaseSensitive(json, "romBanks");
                if(cJSON_IsNumber(romBanks)) {
                    model->romBanks = romBanks->valueint;
                } else {
                    model->romBanks = 0;
                }

                //  Ram Banks
                cJSON* ramBanks = cJSON_GetObjectItemCaseSensitive(json, "ramBanks");
                if(cJSON_IsNumber(ramBanks)) {
                    model->ramBanks = ramBanks->valueint;
                } else {
                    model->ramBanks = 0;
                }



                cJSON* gb_logo =  cJSON_GetObjectItemCaseSensitive(json, "logo");
                if (cJSON_IsArray(gb_logo)) {
                    // Leer los elementos del arreglo "logo"
                    for (int i = 0; i < cJSON_GetArraySize(gb_logo); i++) {
                        cJSON *logoElement = cJSON_GetArrayItem(gb_logo, i);
                        if (cJSON_IsNumber(logoElement)) {
                            model->cart_logo[i] = logoElement->valueint;
                        }
                    }
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

                instance->rom_banks = model->romBanks;
                instance->ram_banks = model->ramBanks;
                
            }
        },true);
}
void gb_cartridge_scene_1_set_callback(
    GBCartridgeScene1* instance,
    GBCartridgeScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
    instance->app = (GBCartridge*)context;
}

//  https://www.youtube.com/watch?v=ix5yZm4fwFQ
void draw_logo(Canvas* canvas, GameBoyCartridgeModel* model, int start_x, int start_y) {
    uint16_t x, y;
    uint8_t row[4];  
    uint8_t list[48*8] = {0};
    for (x = 0; x < 48/2; x += 2) {
        row[0] = (model->cart_logo[x] >> 4) & 0xF;
        row[1] = model->cart_logo[x] & 0xF;
        row[2] = (model->cart_logo[x + 1] >> 4) & 0xF;
        row[3] = model->cart_logo[x + 1] & 0xF;
        for (y = 0; y < 4; y++) {
            // set first bit
            if ((row[y] / 8) == 1) {
                list[(x*2) + (y * 48)] = 1;
                row[y] -= 8;
            }
            // then second bit
            if ((row[y] / 4) == 1) {
                list[((x*2) + 1) + (y * 48)] = 1;
                row[y] -= 4;
            }
            // then third bit
            if ((row[y] / 2) == 1) {
                list[((x*2) + 2) + (y * 48)] = 1;
                row[y] -= 2;
            }
            // then fourth bit
            if ((row[y] / 1) == 1) {
                list[((x*2) + 3) + (y * 48)] = 1;
            }
        }
    }
    
    // then do bottom half
    for (x = 48/2; x < 96/2; x += 2) {
        // convert 2 bytes of data
        row[0] = (model->cart_logo[x] >> 4) & 0xF;
        row[1] = model->cart_logo[x] & 0xF;
        row[2] = (model->cart_logo[x + 1] >> 4) & 0xF;
        row[3] = model->cart_logo[x + 1] & 0xF;
        
        for (y = 0; y < 4; y++) {
            // set first bit
            if ((row[y] / 8) == 1) {
                list[144 + (x*2) + (y * 48)] = 1;
                row[y] -= 8;
            }
            // then second bit
            if ((row[y] / 4) == 1) {
                list[145 + (x*2) + (y * 48)] = 1;
                row[y] -= 4;
            }
            // then third bit
            if ((row[y] / 2) == 1) {
                list[146 + (x*2) + (y * 48)] = 1;
                row[y] -= 2;
            }
            // then fourth bit
            if ((row[y] / 1) == 1) {
                list[147 + (x*2) + (y * 48)] = 1;
            }
        }
    }
    // UNUSED(row);
    
    //  ESCALA 1
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 48; x++) {
            int indice = y * 48 + x;
            if(list[indice] == 1) {
                canvas_draw_dot(canvas, x + start_x, y + start_y);
            }
        }
    }
}
void gb_cartridge_scene_1_draw(Canvas* canvas, GameBoyCartridgeModel* model) {
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
    // canvas_draw_box(canvas, 78, 22, 48, 8); //  TODO: Implementar
    draw_logo(canvas, model, 78, 22);
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

static void gb_cartridge_scene_1_model_init(GameBoyCartridgeModel* const model) {
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
    model->cart_gb_sgb = false;
    for (int i = 0; i < 48*8; i++) {
        model->cart_logo[i] = 0;
    }
}

bool gb_cartridge_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context); 
    GBCartridgeScene1* instance = context;
    bool consumed = false;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                // with_view_model(
                //     instance->view,
                //     GameBoyCartridgeModel * model,
                //     {
                //         UNUSED(model);
                //         instance->callback(GBCartridgeCustomEventScene1Back, instance->context);
                //     },
                //     true);
                consumed = true;
                break;
            case InputKeyOk:

                with_view_model(
                    ((GBCartridge*)instance->app)->gb_cartridge_scene_1->view,
                    GameBoyCartridgeModel * model,
                    { 
                        model->cart_title = "Refresh...";
                        model->cart_serial = "";
                        model->cart_checksum = "";
                        model->cart_ROMSize = "";
                        model->cart_RAMSize = "";
                        // Reiniciar el array a 0 utilizando un bucle
                        for (size_t i = 0; i < sizeof(model->cart_logo)/sizeof(model->cart_logo[0]); i++) {
                            model->cart_logo[i] = 0;
                        }
                        // Register callbacks to receive data
                        uart_set_handle_rx_data_cb(((GBCartridge*)instance->app)->uart, gameboy_information_handle_rx_data_cb); // setup callback for general log rx thread
                        const char gbcartridge_command[] = "gbcartridge -i\n";
                        uart_tx((uint8_t*)gbcartridge_command, strlen(gbcartridge_command));
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

void gb_cartridge_scene_1_exit(void* context) {
    furi_assert(context);
}

void gb_cartridge_scene_1_enter(void* context) {
    furi_assert(context);
    GBCartridgeScene1* instance = (GBCartridgeScene1*)context;
    with_view_model(
        instance->view,
        GameBoyCartridgeModel * model,
        {
            gb_cartridge_scene_1_model_init(model);
        },
        true
    );
    
    // Register callbacks to receive data
    uart_set_handle_rx_data_cb(((GBCartridge*)instance->app)->uart, gameboy_information_handle_rx_data_cb); // setup callback for general log rx thread
    const char gbcartridge_command[] = "gbcartridge -i\n";
    uart_tx((uint8_t*)gbcartridge_command, strlen(gbcartridge_command));
}

GBCartridgeScene1* gb_cartridge_scene_1_alloc() {
    GBCartridgeScene1* instance = malloc(sizeof(GBCartridgeScene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(GameBoyCartridgeModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)gb_cartridge_scene_1_draw);
    view_set_input_callback(instance->view, gb_cartridge_scene_1_input);
    view_set_enter_callback(instance->view, gb_cartridge_scene_1_enter);
    view_set_exit_callback(instance->view, gb_cartridge_scene_1_exit);

    with_view_model(
        instance->view,
        GameBoyCartridgeModel * model,
        {
            gb_cartridge_scene_1_model_init(model);
        },
        true
    );
    

    return instance;
}

void gb_cartridge_scene_1_free(GBCartridgeScene1* instance) {
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

View* gb_cartridge_scene_1_get_view(GBCartridgeScene1* instance) {
    furi_assert(instance);
    return instance->view;
}

