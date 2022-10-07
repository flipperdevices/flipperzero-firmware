#ifndef _SPI_MEM_APP_H_
#define _SPI_MEM_APP_H_

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SPIMemRead* view_read;
    SPIMemChipinfo* view_chipinfo;
    uint32_t view_id;
} SPIMemApp;

enum { SPIMemViewSubmenu, SPIMemViewRead, SPIMemViewSaved, SPIMemViewChipinfo } SPIMemView;

enum {
    SPIMemSubmenuIndexRead,
    SPIMemSubmenuIndexSaved,
    SPIMemSubmenuIndexChipinfo
} SPIMemAppSubmenuIndex;

#endif // _SPI_MEM_APP_H_
