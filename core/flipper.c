#include "flipper.h"
#include <applications.h>
#include <furi.h>
#include <version.h>
#include <api-hal-boot.h>

static void flipper_print_version(const Version * version) {
    printf("Version:\t%s\r\n", version_get_version(version));
    printf("Build date:\t%s\r\n", version_get_builddate(version));
    printf("Git Commit:\t%s (%s)\r\n",
        version_get_githash(version),
        version_get_gitbranchnum(version));
    printf("Git Branch:\t%s\r\n", version_get_gitbranch(version));
}

void flipper_init() {

#ifdef NO_BOOTLOADER
    printf("No bootloader.\r\n");
#else
    const Version *boot_version_adr = (const Version *) api_hal_boot_version_address_get();
    printf("Boot build info\r\n");
    flipper_print_version(boot_version_adr);
#endif  // NO_BOOTLOADER
    printf("Firmware build info\r\n");
    flipper_print_version(0);

    printf("[flipper] starting services\r\n");

    for(size_t i = 0; i < FLIPPER_SERVICES_COUNT; i++) {
        printf("[flipper] starting service %s\r\n", FLIPPER_SERVICES[i].name);

        FuriThread* thread = furi_thread_alloc();

        furi_thread_set_name(thread, FLIPPER_SERVICES[i].name);
        furi_thread_set_stack_size(thread, FLIPPER_SERVICES[i].stack_size);
        furi_thread_set_callback(thread, FLIPPER_SERVICES[i].app);

        furi_thread_start(thread);
    }

    printf("[flipper] services startup complete\r\n");
}
