#ifndef _SPI_MEM_MANAGER_APP_H_
#define _SPI_MEM_MANAGER_APP_H_

typedef struct {
    ViewPort* view_port;
    Gui* gui;
    FuriMessageQueue* event_queue;
} SPIMemManager;

#endif // _SPI_MEM_MANAGER_APP_H_
