#include "bt_i.h"

BT* bt_alloc() {
    BT* bt = furi_alloc(sizeof(BT));
    bt->cli = furi_open("cli");
    return bt;
}

void bt_cli_info(string_t args, void* context) {
    string_t buffer;
    string_init(buffer);
    api_hal_bt_dump_state(buffer);
    cli_print(string_get_cstr(buffer));
    string_clear(buffer);
}

void bt_task() {
    BT* bt = bt_alloc();

    if(bt->cli) {
        cli_add_command(bt->cli, "bt_info", bt_cli_info, bt);
    }

    if(!furi_create("bt", bt)) {
        printf("[bt_task] unable to create bt record\n");
        furiac_exit(NULL);
    }

    furiac_ready();

    api_hal_bt_init();

    while(1) {
        osDelay(1000);
    }
}