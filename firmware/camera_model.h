#ifndef CAMERA_MODEL_H
#define CAMERA_MODEL_H

#include <stdint.h>

/** The dithering algorithms available. */
typedef enum {
    FLOYD_STEINBERG,
    JARVIS_JUDICE_NINKE,
    STUCKI,
} DitheringAlgorithm;

typedef struct {
    /** Flag to enable or disable dithering. */
    bool isDitheringEnabled;
    /** Flag to represent the flash state when saving pictures to the Flipper Zero. */
    bool isFlashEnabled;
    /** Flag to invert pixel colors. */
    bool isInvertEnabled;
    /** Flag to stop or start the stream to the Flipper Zero. */
    bool isStreamToSerialEnabled;
    /** Holds the currently selected dithering algorithm. */
    DitheringAlgorithm ditherAlgorithm;
} CameraModel;

/** The camera model. */
extern CameraModel camera_model;

/** Set the camera model to the default values. */
void set_camera_model_defaults();

#endif
