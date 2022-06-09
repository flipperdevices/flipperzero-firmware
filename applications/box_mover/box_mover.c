#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

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



void draw_callback(Canvas* canvas, void* ctx){
    BoxMover* box_mover = ctx;
    furi_check(osMutexAcquire(box_mover->model_mutex, osWaitForever)==osOK);

    canvas_draw_box(canvas, box_mover->model->x,box_mover->model->y, box_mover->model->sizex, box_mover->model->sizey);

    osMutexRelease(box_mover->model_mutex);
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
                switch(event.key){
                    case InputKeyUp:
                        if (box_mover->model->y >= 1)
                            box_mover->model->y-=2;
                        else{
                            NotificationApp* notification = furi_record_open("notification");
                            notification_message(notification, &sequence_single_vibro);
                            furi_record_close("notification");
                        }
                        break;
                    case InputKeyDown:
                        if (box_mover->model->y <= (64-(box_mover->model->sizey+1)))
                            box_mover->model->y+=2;
                        else{
                            NotificationApp* notification = furi_record_open("notification");
                            notification_message(notification, &sequence_single_vibro);
                            furi_record_close("notification");
                        }
                        break;   
                    case InputKeyLeft:
                        if (box_mover->model->x >= 1)
                            box_mover->model->x-=2;
                        else{
                            NotificationApp* notification = furi_record_open("notification");
                            notification_message(notification, &sequence_single_vibro);
                            furi_record_close("notification");
                        }
                        break;
                    case InputKeyRight:
                        if (box_mover->model->x <= (128-(box_mover->model->sizex+1)))
                            box_mover->model->x+=2;
                        else{
                            NotificationApp* notification = furi_record_open("notification");
                            notification_message(notification, &sequence_single_vibro);
                            furi_record_close("notification");
                        }
                        break;
                    case InputKeyOk:
                        if (box_mover->model->sizex<=50 && box_mover->model->sizey<=50){
                            box_mover->model->sizex+=1;
                            box_mover->model->sizey+=1;
                        }
                        else{
                            NotificationApp* notification = furi_record_open("notification");
                            notification_message(notification, &sequence_single_vibro);
                            furi_record_close("notification");
                        }   
                        
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




