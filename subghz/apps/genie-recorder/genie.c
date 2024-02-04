#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>

#include "genie_about.h"
#include "genie_app.h"
#include "genie_file.h"
#include "genie_learn.h"
#include "genie_subghz_receive.h"
#include "genie_submenu.h"

const GpioPin* const pin_remote = &gpio_ext_pa7;

int32_t genie_record_app(void* p) {
    UNUSED(p);
    GenieApp* app = genie_app_alloc();
    view_dispatcher_run(genie_app_get_view_dispatcher(app));
    genie_app_free(app);
    return 0;
}