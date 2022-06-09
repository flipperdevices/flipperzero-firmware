#include <furi.h>
#include <gui/gui.h>
#include <stdlib.h>

typedef struct {
    int x;
    int y;
} BoxMoverModel;

typedef struct {
    BoxMoverModel* model;
    osMutexId_t* model_mutex;
    
    osMessageQueueId_t event_queue;

    ViewPort*  view_port;
    Gui* gui;
} BoxMover;

BoxMover* box_mover_alloc(){
    BoxMover* instance = malloc(sizeof(BoxMover));
    instance->model = malloc(sizeof(BoxMoverModel));
    instance->model->x = 10;
    instance->model->y = 10;

    instance->view_port = view_port_alloc();
    view_port_draw_callback_set(instance->view_port, draw_callback, instance);

    view_port_input_callback_set(instance->view_port, input_callback, instance);
    instance->model_mutex = osMutexNew(NULL);

    instance->gui = furi_record_open("gui");
    gui_add_view_port(instance->view_port, GuiLayerFullscreen);

    instance->event_queue = osMessageQueueNew(8, sizeof(InputEvent), NULL);

    return instance;
}

void box_mover_free(BoxMover* instance){
    view_port_enabled_set(instance->view_port,false);
    gui_remove_view_port(instance->gui, view_port);
    furi_record_close("gui");
    osMessageQueueDelete(instance->event_queue);
    osMutexDelete(instance->model_mutex);
    view_port_free(instance->view_port);
    free(instance->model);
    free(instance);
}

int32_t box_mover_app(void* p){
    UNUSED(p);
    BoxMover* box_mover = box_mver_alloc();
        
    box_mover_free(box_mover)

    for(bool processing = true; processing;){
        osStatus_t status = osMessageQueueGet(box_mover->event_queue, &event, NULL, 100);
        furi_check(osMutexAcquire(box_mover->model_mutex, osWaitForever) == osOK);
        if(status==osOK){
            if(event.type==InputTypePress){
                switch(event.key){
                    case InputKeyUp:
                        box_mover->model->y-=2;
                        break;
                    case InputKeyDown:
                        box_mover->model->y+=2;
                        break;
                    case InputKeyLeft:
                        box_mover->model->x-=2;
                        break;
                    case InputKeyRight:
                        box_mover->model->x+=2;
                        break;
                    case InputKeyOk:
                    case InputKeyBack:
                        processing = false;
                        break;
                }
            }
        }
        osMutexRElease(box_mover->model_mutex);
        view_port_update(box_mover->view_port);
    }


    box_mover_free(box_mover)
    return 0;
}

void draw_callback(Canvas* cancas, void* ctx){
    BoxMover *box_mover = ctx;
    furi_check(osMutexAcquire(box_mover->model_mutex, osWaitForever)==osOK);

    canvas_draw_box(canvas, box_mover->x,box_mover->y, 4, 4);

    osReleaseMutex(box_mover->model_mutex);
}

void input_callback(InputEvent* input, osMessageQueueId_t event_queue){
    BoxMover* box_mover = ctx;
    osMessageQueuePut(box_mover->event_queue, input, 0, osWaitForever);

}


