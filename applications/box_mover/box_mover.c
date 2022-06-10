#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include "assets_icons.h"
#include <gui/icon_i.h>


/* 0x01,0x00,0xa4,0x01
typedef struct {
    uint8_t is_compressed;
    uint8_t reserved;
    uint16_t compressed_buff_size;
} FuriHalCompressHeader;
*/

const uint8_t _I_BoxMover_128x64_0[] = {0x01,0x00,0xa4,0x01,0x00,0x78,0x03,0xc0,0x1e,0x00,0xab,0x00,0x3f,0xbf,0x90,0x30,0x29,0xfc,0x23,0xfe,0x0c,0x1e,0x0c,0x12,0x99,0xf8,0x78,0x3d,0x27,0xff,0x8c,0x78,0x3d,0x3c,0x10,0x3b,0xf0,0x3d,0x2f,0xe0,0xf1,0xe3,0x83,0xcf,0xf0,0x0a,0x18,0x08,0x3c,0xef,0x13,0xc8,0xf7,0x1c,0x10,0x71,0xf0,0x88,0xc1,0xc1,0xed,0xef,0x07,0x97,0x00,0x18,0x33,0xe9,0xf0,0x3e,0xbf,0x10,0x10,0x88,0x8c,0x84,0x1e,0xbf,0xe0,0x39,0x06,0xc4,0x7c,0x3f,0x01,0x81,0x13,0xc4,0x1e,0x98,0x03,0x60,0x84,0x46,0xf8,0x43,0x13,0xf9,0x03,0xd0,0x38,0x21,0x12,0x87,0x8c,0x08,0xfe,0x20,0xf4,0xbe,0x04,0x49,0x02,0x20,0x11,0x17,0xbc,0x78,0x22,0x41,0xd1,0xc0,0x10,0x04,0x9e,0xd7,0xe1,0x71,0x0f,0x47,0xc0,0x0f,0x5f,0x70,0x3c,0x7c,0xde,0x38,0x1a,0x04,0xaf,0x90,0x63,0xfb,0xe1,0xbf,0xe2,0xe2,0x10,0x88,0x3d,0x7c,0xe0,0xf1,0x83,0x0f,0x84,0xde,0x24,0xe1,0x07,0xaa,0xfc,0xa0,0xdf,0xce,0x08,0xb8,0x44,0x22,0x0f,0x4c,0xf3,0x7c,0xa0,0xdc,0xcf,0xb8,0x50,0x67,0xe0,0xf3,0x77,0xac,0x1a,0x18,0xfd,0x12,0x81,0x03,0xca,0x7e,0x0f,0x1c,0x18,0x3c,0xff,0x8f,0xf3,0x07,0x94,0x7f,0xc1,0x83,0x07,0xa7,0x62,0x0e,0xee,0x20,0x78,0x80,0x18,0x1e,0x31,0x8c,0xfa,0x44,0x41,0xf9,0xfc,0x17,0x08,0x3f,0x2c,0x00,0x63,0x07,0xf8,0x3f,0xff,0xdf,0xf0,0x02,0x43,0xc1,0xff,0x06,0x8e,0x03,0xfb,0xf0,0x0f,0xef,0x04,0x7c,0x1e,0x90,0xe8,0x74,0x7d,0xbc,0x3c,0x08,0x08,0x3c,0x70,0x10,0xf0,0x7b,0x43,0xe3,0x80,0xf2,0x87,0x1a,0x06,0x18,0x0f,0x68,0x3c,0x60,0x1e,0xb0,0x00,0x7a,0xc1,0xb8,0xe0,0xf1,0xfc,0x4c,0x41,0xf3,0x04,0xe3,0xce,0x3c,0x40,0xff,0x07,0xd6,0x3a,0x28,0x0f,0x31,0xfc,0x83,0xd3,0x81,0x81,0x37,0x88,0x3d,0xe2,0x00,0xf3,0x3f,0x90,0x3f,0x07,0xf4,0x0f,0x32,0x7d,0x83,0xc6,0xf1,0xf2,0x07,0xf8,0x3e,0xe3,0x20,0xf1,0xf8,0x03,0xf2,0x0e,0x0f,0x1c,0x00,0x3c,0x61,0xc0,0xf5,0x83,0x83,0xc6,0x1f,0x7c,0x07,0x9d,0xf0,0x1e,0x9e,0x08,0x18,0x3c,0x63,0xf7,0xe0,0x79,0xfc,0x20,0x20,0xf3,0xfc,0x40,0x3f,0xdf,0xf0,0x02,0x43,0xf8,0x10,0xf0,0x79,0xcf,0xc1,0xf1,0x00,0x9f,0x03,0xcb,0x81,0x07,0x07,0xcb,0x4c,0x41,0xe2,0x2e,0x10,0x7c,0x86,0xc4,0x1e,0x22,0x31,0x07,0xcc,0x02,0x3f,0x60,0x21,0x90,0x02,0xbf,0x16,0x03,0x19,0x00,0x2b,0xc1,0x63,0x41,0x90,0x02,0xc6,0x86,0x00,0xbf,0xe4,0x0c,0x1f,0xab,0xf3,0x00,0x78,0x03,0xc0,0x1f,0x00,};
const uint8_t* _I_BoxMover_128x64[] = {_I_BoxMover_128x64_0};
const Icon I_BoxMover_128x64 = {.width=128,.height=64,.frame_count=1,.frame_rate=0,.frames=_I_BoxMover_128x64};
int moves; //Count moves. Text testing.
//const uint8_t _I_Splash_128x64_0[] = {
int splash; //Have we seen the title

typedef struct {
    int x;
    int y;
    int sizex;
    int sizey;
    
} BoxMoverModel;

typedef struct {
    BoxMoverModel* model;
    osMutexId_t* model_mutex;
    
    osMessageQueueId_t event_queue;

    ViewPort*  view_port;
    Gui* gui;
} BoxMover;

void shake(void){
        NotificationApp* notification = furi_record_open("notification");
        notification_message(notification, &sequence_single_vibro);
        furi_record_close("notification");
    }

void draw_callback(Canvas* canvas, void* ctx){

    BoxMover* box_mover = ctx;
    furi_check(osMutexAcquire(box_mover->model_mutex, osWaitForever)==osOK);
    if(splash==0)
    {
    //canvas_draw_icon(canvas, 22, 20, &I_UsbTree_48x22); //testing :)
    canvas_draw_icon(canvas, 0, 0, &I_BoxMover_128x64);
    osMutexRelease(box_mover->model_mutex); // lol
    }
    else{
    canvas_clear(canvas);
    canvas_draw_box(canvas, box_mover->model->x,box_mover->model->y, box_mover->model->sizex, box_mover->model->sizey);
    char buffer[13];
    snprintf(buffer, sizeof(buffer), "Moves: %u", moves);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 5, 5, AlignLeft, AlignTop, buffer);
    osMutexRelease(box_mover->model_mutex);
    }
}

void input_callback(InputEvent* input, void* ctx){
    BoxMover* box_mover = ctx;
    osMessageQueuePut(box_mover->event_queue, input, 0, osWaitForever);
}

BoxMover* box_mover_alloc(){
    BoxMover* instance = malloc(sizeof(BoxMover));
    instance->model = malloc(sizeof(BoxMoverModel));
    instance->model->x = 10;
    instance->model->y = 10;
    instance->model->sizex = 4;
    instance->model->sizey = 4;
    moves=0;
    splash=0;

instance->model_mutex = osMutexNew(NULL);
    instance->event_queue = osMessageQueueNew(8, sizeof(InputEvent), NULL);
    instance->view_port = view_port_alloc();
    view_port_draw_callback_set(instance->view_port, draw_callback, instance);

    view_port_input_callback_set(instance->view_port, input_callback, instance);
    

    instance->gui = furi_record_open("gui");
    gui_add_view_port(instance->gui, instance->view_port, GuiLayerFullscreen);



    return instance;
}

void box_mover_free(BoxMover* instance){
    view_port_enabled_set(instance->view_port,false);
    gui_remove_view_port(instance->gui, instance->view_port);
    furi_record_close("gui");
    view_port_free(instance->view_port);
    osMessageQueueDelete(instance->event_queue);
    osMutexDelete(instance->model_mutex);

    free(instance->model);
    free(instance);
}


int32_t box_mover_app(void* p){
    UNUSED(p);
    BoxMover* box_mover = box_mover_alloc();
        

    InputEvent event;
    for(bool processing = true; processing;){
        osStatus_t status = osMessageQueueGet(box_mover->event_queue, &event, NULL, 100);
        furi_check(osMutexAcquire(box_mover->model_mutex, osWaitForever) == osOK);
        if(status==osOK){
                if(event.type==InputTypePress){
                    if(splash==0){splash=1;}
                    switch(event.key){
                            case InputKeyUp:
                                if (box_mover->model->y >= 1){
                                    box_mover->model->y-=2;
                                    moves++;
                                }
                                else{
                                    shake();
                                }
                                break;
                            case InputKeyDown:
                                if (box_mover->model->y <= (63-(box_mover->model->sizey+(box_mover->model->sizey % 2 != 0)))){ // So we're trying to see if it's even or odd, to determine how much to offset the bounds check by, since we're doing this based of the square origin+its size. idk.
                                    box_mover->model->y+=2;
                                    moves++;
                                }
                                else{
                                    shake();
                                }
                                break;   
                            case InputKeyLeft:
                                if (box_mover->model->x >= 1){
                                    box_mover->model->x-=2;
                                    moves++;
                                }
                                else{
                                    shake();
                                }
                                break;
                            case InputKeyRight:
                                if (box_mover->model->x <= (127-(box_mover->model->sizex+(box_mover->model->sizex % 2 != 0)))){
                                    box_mover->model->x+=2;
                                    moves++;
                                }
                                else{
                                    shake();
                                }
                                break;
                            case InputKeyOk:
                                if (box_mover->model->sizex<=50 && box_mover->model->sizey<=50){
                                    if (box_mover->model->x <= (127-(box_mover->model->sizex+(box_mover->model->sizex % 2 != 0)))){
                                        if (box_mover->model->y <= (63-(box_mover->model->sizey+(box_mover->model->sizey % 2 != 0)))){
                                            box_mover->model->sizex+=1;
                                            box_mover->model->sizey+=1;
                                    //TODO - also check the box will not grow past boundary.
                                            }
                                            else
                                            shake();
                                    }
                                    else
                                        shake();                        
                                }
                                else
                                    shake();
                                
                                
                                break;
                            case InputKeyBack:
                                processing = false;
                                break;
                        
                    }
                }
            
        }
        osMutexRelease(box_mover->model_mutex);
        view_port_update(box_mover->view_port);
    }



    box_mover_free(box_mover);
    return 0;
}




