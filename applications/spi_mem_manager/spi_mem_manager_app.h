#ifndef _SPI_MEM_MANAGER_APP_H_
#define _SPI_MEM_MANAGER_APP_H_

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SPIMemManagerRead* view_read;
    SPIMemManagerChipinfo* view_chipinfo;
    uint32_t view_id;
} SPIMemManager;

enum {
    SPIMemManagerViewSubmenu,
    SPIMemManagerViewRead,
    SPIMemManagerViewSaved,
    SPIMemManagerViewChipinfo
} SPIMemManagerView;

enum {
    SPIMemManagerSubmenuIndexRead,
    SPIMemManagerSubmenuIndexSaved,
    SPIMemManagerSubmenuIndexChipinfo
} SPIMemManagerSubmenuIndex;

#endif // _SPI_MEM_MANAGER_APP_H_
