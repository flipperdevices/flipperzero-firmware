#include "camera_model.h"

/** The camera model. */
CameraModel camera_model;

void set_camera_model_defaults() {
  camera_model.isDitheringEnabled = true;
  camera_model.isFlashEnabled = false;
  camera_model.isInvertEnabled = false;
  camera_model.ditherAlgorithm = FLOYD_STEINBERG;
}
