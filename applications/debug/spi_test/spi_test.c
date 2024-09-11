#include <furi_hal.h>
#include <furi_hal_spi.h>
#include <furi_hal_resources.h>
#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>

#include <lib/softio/softio_spi.h>

#define TAG "SpiTest"

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
} SpiTest;

typedef enum {
    SpiTestViewSubmenu,
} SpiTestView;

typedef enum {
    SpiTestSubmenuHardwareTx,
    SpiTestSubmenuSoftwareTx,
} SpiTestSubmenu;

static void spi_test_submenu_callback(void* context, uint32_t index) {
    SpiTest* instance = (SpiTest*)context;
    UNUSED(instance);

    uint8_t tx_buffer[] = {0x55, 0xAA};
    uint8_t rx_buffer[sizeof(tx_buffer)] = {0};

    if(index == SpiTestSubmenuHardwareTx) {
        FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_external;
        furi_hal_spi_bus_handle_init(handle);
        furi_hal_spi_acquire(handle);
        furi_hal_spi_bus_trx(handle, tx_buffer, rx_buffer, sizeof(tx_buffer), FuriWaitForever);
        furi_hal_spi_release(handle);
        furi_hal_spi_bus_handle_deinit(handle);
    } else {
        // initialize
        SoftIoSpiBusConfig bus_cfg = {
            .miso = &gpio_ext_pa6,
            .mosi = &gpio_ext_pa7,
            .sck = &gpio_ext_pb3,
            .clk_polarity = 0,
        };
        SoftIoSpiSlaveConfig dev_cfg = {
            .cs = &gpio_ext_pa4,
            .clk_fq_khz = 200,
            .clk_phase = 0,
        };
        SoftIoSpiBus* bus = softio_spi_alloc(&bus_cfg);
        SoftIoSpiSlave* device = softio_spi_attach_slave(bus, &dev_cfg);
        softio_spi_init(bus);
        furi_delay_ms(100);

        // transmit
        softio_spi_acquire(device);
        softio_spi_trx(device, tx_buffer, rx_buffer, sizeof(tx_buffer), FuriWaitForever);
        softio_spi_release(device);

        // deinitialize
        softio_spi_deinit(bus);
        softio_spi_detach_slave(bus, device);
        softio_spi_free(bus);
    }

    FURI_LOG_I(
        TAG,
        "sent %02hhx %02hhX, received %02hhx %02hhX",
        tx_buffer[0],
        tx_buffer[1],
        rx_buffer[0],
        rx_buffer[1]);
}

static uint32_t spi_test_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

SpiTest* spi_test_alloc(void) {
    SpiTest* instance = malloc(sizeof(SpiTest));

    View* view = NULL;

    instance->gui = furi_record_open(RECORD_GUI);
    instance->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);

    // Menu
    instance->submenu = submenu_alloc();
    view = submenu_get_view(instance->submenu);
    view_set_previous_callback(view, spi_test_exit_callback);
    view_dispatcher_add_view(instance->view_dispatcher, SpiTestViewSubmenu, view);
    submenu_add_item(
        instance->submenu,
        "Hardware TRX",
        SpiTestSubmenuHardwareTx,
        spi_test_submenu_callback,
        instance);
    submenu_add_item(
        instance->submenu,
        "Software TRX",
        SpiTestSubmenuSoftwareTx,
        spi_test_submenu_callback,
        instance);

    return instance;
}

void spi_test_free(SpiTest* instance) {
    view_dispatcher_remove_view(instance->view_dispatcher, SpiTestViewSubmenu);
    submenu_free(instance->submenu);

    view_dispatcher_free(instance->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(instance);
}

int32_t spi_test_run(SpiTest* instance) {
    view_dispatcher_switch_to_view(instance->view_dispatcher, SpiTestViewSubmenu);
    view_dispatcher_run(instance->view_dispatcher);
    return 0;
}

int32_t spi_test_app(void* p) {
    UNUSED(p);

    SpiTest* instance = spi_test_alloc();

    int32_t ret = spi_test_run(instance);

    spi_test_free(instance);

    return ret;
}
