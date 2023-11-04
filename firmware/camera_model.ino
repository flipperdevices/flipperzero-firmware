#include "camera_model.h"

/** The camera model. */
CameraModel camera_model;

void reset_camera_model() {
    camera_model.isDitheringEnabled = true;
    camera_model.isFlashEnabled = false;
    camera_model.isInvertEnabled = false;
    camera_model.isStreamEnabled = true;
    camera_model.ditherAlgorithm = FLOYD_STEINBERG;
}
