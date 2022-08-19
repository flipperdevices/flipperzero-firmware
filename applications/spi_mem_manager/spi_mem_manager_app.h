#ifndef _SPI_MEM_MANAGER_APP_H_
#define _SPI_MEM_MANAGER_APP_H_

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    uint32_t view_id;
} SPIMemManager;

enum { SPIMemManagerViewSubmenu } SPIMemManagerView;

enum {
    SPIMemManagerSubmenuIndexRead,
    SPIMemManagerSubmenuIndexSaved,
    SPIMemManagerSubmenuIndexChipInfo
} SPIMemManagerSubmenuIndex;

#endif // _SPI_MEM_MANAGER_APP_H_
