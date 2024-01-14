#include "camera_model.h"

/** The camera model. */
CameraModel camera_model;

void set_camera_model_defaults(CameraFunction camera_function) {
    if(camera_function == CAMERA_FUNCTION_SERIAL) {
        camera_model.isDitheringEnabled = true;
    } else if(camera_function == CAMERA_FUNCTION_WIFI) {
        camera_model.isDitheringEnabled = false;
    }

    camera_model.isFlashEnabled = false;
    camera_model.isInvertEnabled = false;
    camera_model.isStreamToSerialEnabled = false;
    camera_model.isStreamToWiFiEnabled = false;
    camera_model.ditherAlgorithm = FLOYD_STEINBERG;
}
