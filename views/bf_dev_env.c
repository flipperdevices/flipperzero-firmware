#include "bf_dev_env.h"
#include <gui/elements.h>

typedef struct BFDevEnv {
    View* view;
    DevEnvOkCallback callback;
    void* context;
    BFApp* appDev;
} BFDevEnv;

typedef struct {
    rByte row;
    rByte col;
} BFDevEnvModel;

typedef struct{
    int up;
    int down;
    int left;
    int right;
}bMapping;

static bool bf_dev_process_up(BFDevEnv* devEnv);
static bool bf_dev_process_down(BFDevEnv* devEnv);
static bool bf_dev_process_left(BFDevEnv* devEnv);
static bool bf_dev_process_right(BFDevEnv* devEnv);
static bool bf_dev_process_ok(BFDevEnv* devEnv, InputEvent* event);

BFApp* appDev;

int selectedButton = 0;
int saveNotifyCountdown = 0;
static bMapping buttonMappings[11] = {
    {4, 4, 8, 1},
    {5, 5, 0, 2},
    {6, 6, 1, 3},
    {7, 7, 2, 8},
    {0, 0, 10, 5},
    {1, 1, 4, 6},
    {2, 2, 5, 7},
    {3, 3, 6, 10},
    {10, 9, 3, 0},
    {8, 10, 3, 0},
    {9, 8, 7, 4}
};

static void bf_dev_draw_button(Canvas* canvas, int x, int y, bool selected, const char* lbl){
    UNUSED(lbl);

    if(selected){
        canvas_draw_rbox(canvas, x, y, 18, 18, 3);
        canvas_invert_color(canvas);
        canvas_set_font(canvas, FontKeyboard);
        canvas_draw_str_aligned(canvas, x + (18/2), y + (18/2) - 1, AlignCenter, AlignCenter, lbl);
        canvas_invert_color(canvas);
    }
    else{
        canvas_draw_rbox(canvas, x, y, 18, 18, 3);
        canvas_invert_color(canvas);
        canvas_draw_rbox(canvas, x+2, y-1, 16, 17, 3);
        canvas_invert_color(canvas);
        canvas_draw_rframe(canvas, x, y, 18, 18, 3);
        canvas_set_font(canvas, FontKeyboard);
        canvas_draw_str_aligned(canvas, x + (18/2), y + (18/2) - 1, AlignCenter, AlignCenter, lbl);
    }
}

static void bf_dev_draw_callback(Canvas* canvas, void* _model) {
    UNUSED(_model);

    //buttons
    bf_dev_draw_button(canvas, 2, 24, (selectedButton == 0), "+"); //T 0
    bf_dev_draw_button(canvas, 22, 24, (selectedButton == 1),  "-"); //T 1
    bf_dev_draw_button(canvas, 42, 24, (selectedButton == 2),  "<"); //T 2
    bf_dev_draw_button(canvas, 62, 24, (selectedButton == 3),  ">"); //T 3
    bf_dev_draw_button(canvas, 2, 44, (selectedButton == 4),  "["); //B 0
    bf_dev_draw_button(canvas, 22, 44, (selectedButton == 5),  "]"); //B 1
    bf_dev_draw_button(canvas, 42, 44, (selectedButton == 6),  "."); //B 2
    bf_dev_draw_button(canvas, 62, 44, (selectedButton == 7),  ","); //B 3

    //save, run, backspace
    canvas_draw_icon(canvas, 110, 24, (selectedButton == 8) ? &I_KeyBackspaceSelected_16x9 : &I_KeyBackspace_16x9);
    canvas_draw_icon(canvas, 102, 38, (selectedButton == 9) ? &I_KeyRunSel_24x11 : &I_KeyRun_24x11);
    canvas_draw_icon(canvas, 102, 51, (selectedButton == 10) ? &I_KeySaveSelected_24x11 : &I_KeySave_24x11);

    if(saveNotifyCountdown > 0){
        canvas_draw_icon(canvas, 98, 54, &I_ButtonRightSmall_3x5);
        saveNotifyCountdown--;
    }

    //textbox
    canvas_draw_rframe(canvas, 2, 2, 124, 20, 2);
    canvas_set_font(canvas, FontKeyboard);

    int dbOffset = 0;
    if(appDev->dataSize > 20){
        dbOffset = (appDev->dataSize - 20);
    }
    
    //canvas_draw_str(canvas, 5, 15, (char*)(appDev->dataBuffer + dbOffset));
    canvas_draw_str_aligned(canvas, 4, 12, AlignLeft, AlignCenter, (char*)(appDev->dataBuffer + dbOffset));
}


static bool bf_dev_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    BFDevEnv* devEnv = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight) {
            consumed = bf_dev_process_right(devEnv);
        } else if(event->key == InputKeyLeft) {
            consumed = bf_dev_process_left(devEnv);
        } else if(event->key == InputKeyUp) {
            consumed = bf_dev_process_up(devEnv);
        } else if(event->key == InputKeyDown) {
            consumed = bf_dev_process_down(devEnv);
        }
    } else if(event->key == InputKeyOk) {
        consumed = bf_dev_process_ok(devEnv, event);
    }

    return consumed;
}

static bool bf_dev_process_up(BFDevEnv* devEnv) {
    UNUSED(devEnv);
    selectedButton = buttonMappings[selectedButton].up;
    return true;
}

static bool bf_dev_process_down(BFDevEnv* devEnv) {
    UNUSED(devEnv);
    selectedButton = buttonMappings[selectedButton].down;
    return true;
}

static bool bf_dev_process_left(BFDevEnv* devEnv) {
    UNUSED(devEnv);
    selectedButton = buttonMappings[selectedButton].left;
    return true;
}

static bool bf_dev_process_right(BFDevEnv* devEnv) {
    UNUSED(devEnv);
    selectedButton = buttonMappings[selectedButton].right;
    return true;
}

static bool bf_dev_process_ok(BFDevEnv* devEnv, InputEvent* event) {
    UNUSED(devEnv);
    UNUSED(event);

    if(event->type != InputTypePress){ return false; }

    switch(selectedButton){
        case 0:
        {
            if(appDev->dataSize < BF_INST_BUFFER_SIZE){ 
                appDev->dataBuffer[appDev->dataSize] = (rByte)'+'; 
                appDev->dataSize++; }
            break;
        }

        case 1:
        {
            if(appDev->dataSize < BF_INST_BUFFER_SIZE){ 
                appDev->dataBuffer[appDev->dataSize] = (rByte)'-'; 
                appDev->dataSize++; }
            break;
        }

        case 2:
        {
            if(appDev->dataSize < BF_INST_BUFFER_SIZE){ 
                appDev->dataBuffer[appDev->dataSize] = (rByte)'<'; 
                appDev->dataSize++; }
            break;
        }

        case 3:
        {
            if(appDev->dataSize < BF_INST_BUFFER_SIZE){ 
                appDev->dataBuffer[appDev->dataSize] = (rByte)'>'; 
                appDev->dataSize++; }
            break;
        }

        case 4:
        {
            if(appDev->dataSize < BF_INST_BUFFER_SIZE){ 
                appDev->dataBuffer[appDev->dataSize] = (rByte)'['; 
                appDev->dataSize++; }
            break;
        }

        case 5:
        {
            if(appDev->dataSize < BF_INST_BUFFER_SIZE){ 
                appDev->dataBuffer[appDev->dataSize] = (rByte)']'; 
                appDev->dataSize++; }
            break;
        }

        case 6:
        {
            if(appDev->dataSize < BF_INST_BUFFER_SIZE){ 
                appDev->dataBuffer[appDev->dataSize] = (rByte)'.'; 
                appDev->dataSize++; }
            break;
        }

        case 7:
        {
            if(appDev->dataSize < BF_INST_BUFFER_SIZE){ 
                appDev->dataBuffer[appDev->dataSize] = (rByte)','; 
                appDev->dataSize++; }
            break;
        }

        case 8:
        {
            if(appDev->dataSize > 0){
                appDev->dataSize--; 
                appDev->dataBuffer[appDev->dataSize] = (rByte)0x00;}
            break;
        }

        case 9:
        {
            scene_manager_next_scene(appDev->scene_manager, brainfuckSceneExecEnv);
            break;
        }

        case 10:
        {
            //remove old file
            Storage* storage = furi_record_open(RECORD_STORAGE);
            storage_simply_remove(storage, furi_string_get_cstr(appDev->BF_file_path));

            //save new file
            Stream* stream = buffered_file_stream_alloc(storage);
            buffered_file_stream_open(stream, furi_string_get_cstr(appDev->BF_file_path), FSAM_WRITE, FSOM_CREATE_ALWAYS);
            stream_write(stream, appDev->dataBuffer, appDev->dataSize);
            buffered_file_stream_close(stream);

            //notify
            saveNotifyCountdown = 5;
            break;
        }
    }

    bool consumed = false;
    return consumed;
}

static void bf_dev_enter_callback(void* context) {
    furi_assert(context);
    BFDevEnv* devEnv = context;

    with_view_model(
        devEnv->view,
        BFDevEnvModel* model,
        {
            model->col = 0;
            model->row = 0;
        },
        true);

    appDev = devEnv->appDev;

    //clear the bf instruction buffer
    memset(appDev->dataBuffer, 0x00, BF_INST_BUFFER_SIZE);

    //open the file
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = buffered_file_stream_alloc(storage);
    buffered_file_stream_open(stream, furi_string_get_cstr(appDev->BF_file_path), FSAM_READ, FSOM_OPEN_EXISTING);

    //read into the buffer
    appDev->dataSize = stream_size(stream);
    stream_read(stream, appDev->dataBuffer, appDev->dataSize);
    buffered_file_stream_close(stream);

    //find the end of the file to begin editing
    int tptr = 0;
    while(appDev->dataBuffer[tptr] != 0x00){ tptr++; }
    appDev->dataSize = tptr;
}

BFDevEnv* bf_dev_env_alloc(BFApp* appDev) {
    BFDevEnv* devEnv = malloc(sizeof(BFDevEnv));

    devEnv->view = view_alloc();
    devEnv->appDev = appDev;
    view_allocate_model(devEnv->view, ViewModelTypeLocking, sizeof(BFDevEnvModel));

    with_view_model(
        devEnv->view,
        BFDevEnvModel* model,
        {
            model->col = 0;
            model->row = 0;
        },
        true);

    view_set_context(devEnv->view, devEnv);
    view_set_draw_callback(devEnv->view, bf_dev_draw_callback);
    view_set_input_callback(devEnv->view, bf_dev_input_callback);
    view_set_enter_callback(devEnv->view, bf_dev_enter_callback);
    return devEnv;
}

void bf_dev_env_free(BFDevEnv* devEnv) {
    furi_assert(devEnv);
    view_free(devEnv->view);
    free(devEnv);
}

View* bf_dev_env_get_view(BFDevEnv* devEnv) {
    furi_assert(devEnv);
    return devEnv->view;
}
