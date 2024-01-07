#pragma once

#include <gui/view.h>
#include <gui/icon_i.h>
#include "../helpers/boilerplate_custom_event.h"
#include <malveke_gb_photo_icons.h>
#define BMP_COMPAT // write bottom to top
#include "../helpers/bmp.h"
#include "../helpers/malveke_photo.h"

#define WIDTH 128L
#define HEIGHT 112L

#define FRAME_WIDTH 128
#define FRAME_HEIGTH 64

typedef struct BoilerplateScene1 BoilerplateScene1;

typedef void (*BoilerplateScene1Callback)(BoilerplateCustomEvent event, void* context);

void boilerplate_scene_1_set_callback(
    BoilerplateScene1* boilerplate_scene_1,
    BoilerplateScene1Callback callback,
    void* context);

View* boilerplate_scene_1_get_view(BoilerplateScene1* boilerplate_static);

BoilerplateScene1* boilerplate_scene_1_alloc();

void boilerplate_scene_1_free(BoilerplateScene1* boilerplate_static);