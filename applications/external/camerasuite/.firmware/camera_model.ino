#include "camera_model.h"

/** The camera model. */
CameraModel camera_model;

void initialize_camera_model() {
    // Set up camera model defaults.
    camera_model.isDitheringEnabled = true;
    camera_model.isFlashEnabled = false;
    camera_model.isInvertEnabled = false;
    camera_model.isStreamEnabled = true;
    camera_model.ditherAlgorithm = FLOYD_STEINBERG;
}
