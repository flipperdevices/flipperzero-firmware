#include "flipper.h"
#include <applications.h>
#include <furi.h>
#include <api-hal-boot.h>

void flipper_init() {

#if NO_BOOTLOADER
    printf("[boot] No version info\n");
#else
    const Version *boot_version_adr = api_hal_boot_version_address_get();
    printf("[boot] Version: %s\n", version_get_version(boot_version_adr));
    printf("       Build date: %s\n", version_get_builddate(boot_version_adr));
    printf("       Git Commit: %s\n", version_get_githash(boot_version_adr));
    printf("       Git Branch: %s\n", version_get_gitbranch(boot_version_adr));
    printf("       Commit Number: %s\n", version_get_gitbranchnum(boot_version_adr));
#endif

    printf("[flipper] Version: %s\n", version_get_version(0));
    printf("          Build date: %s\n", version_get_builddate(0));
    printf("          Git Commit: %s\n", version_get_githash(0));
    printf("          Git Branch: %s\n", version_get_gitbranch(0));
    printf("          Commit Number: %s\n", version_get_gitbranchnum(0));

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
