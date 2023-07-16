#ifndef  RPS_H_
#define  RPS_H_

typedef struct RPSApp {
    // Flipper Firmware Variables 
    FuriMessageQueue*   event_queue;
    ViewPort*           view_port;
    Gui*                gui;
    FuriMutex**         mutex;
    
    // App Variables
} RPSApp_t;

#endif // RPS_H_
