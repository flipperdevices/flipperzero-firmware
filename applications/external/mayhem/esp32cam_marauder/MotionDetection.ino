#define MAX_RESOLUTION_VGA
// Using EloquentEsp32cam library
#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/motion/Detector.h"
#include "esp32cam/motion/SimpleChange.h"

Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::JpegDecoder decoder;
Eloquent::Esp32cam::Motion::SimpleChange algorithm;
Eloquent::Esp32cam::Motion::Detector detector(algorithm);

void motion_detection_setup() {
  cam.aithinker();
  cam.highQuality();
  cam.vga();
  cam.highestSaturation();
  /**
     * For motion detection to perform well, I suggest
     * you disable automatic controls, otherwise
     * the camera sensor will artificially alter the
     * pixels and those will be mis-labelled as foreground
     */
  cam.disableAutomaticWhiteBalance();
  cam.disableAutomaticExposureControl();
  cam.disableGainControl();

  /**
     * Configure the detector
     */
  /**
      * use the first N frames to train the algorithm
      */
  detector.trainFor(30);
  /**
     * re-run the training aftern N frames
     * (at 33 FPS, 33 * 600 = 10 minutes)
     */
  detector.retrainAfter(33ULL * 600);
  /**
     * detection motion when 20% or more pixels of the frame
     * are labelled as background
     */
  detector.triggerAbove(0.2);
  /**
     * try to remove spurious foreground pixels
     */
  detector.denoise();

  /**
     * Configure algorithm
     * (each algorithm has its own set of parameters)
     */
  /**
      * label pixel as foreground if its value changed
      * by more than 20 (in a range from 0 to 255)
      */
  algorithm.differBy(20);
  /**
     * when updating the pixel value, how much shall we
     * take into consideration its previous value?
     * The higher this value, the stronger influence
     * the pixel history has w.r.t. its current value
     * The update formula is
     * updated value = a * old value + (1 - a) * new value
     * Where a in in the range 0 - 1 (1 excluded)
     */
  algorithm.smooth(0.9);
  /**
     * when a pixel is labelled as foreground, should we
     * update its value or not?
     * It is updated by default, so if that's what you want,
     * remove the following line.
     */
  algorithm.onlyUpdateBackground();


  while (!cam.begin())
    Serial.println(cam.getErrorMessage());
}

unsigned long nextMotionAlert = 0;

void motion_detection_loop() {
  if (!cam.capture()) {
    Serial.println(cam.getErrorMessage());
    return;
  }

  if (!decoder.decode(cam)) {
    Serial.println(decoder.getErrorMessage());
    return;
  }

  /**
     * Update the background model
     * If there's an error, print it
     *
     * Note: while training, `update()` will return False
     * even if it cannot really considered an error.
     * If you want to check if the error is due to training or not,
     * you can check for `detector.isTraining()`
     */
  if (!detector.update(decoder.luma)) {
    Serial.println(detector.getErrorMessage());
    return;
  }

  /**
     * Test if motion was detected
     */
  if (detector.triggered() && millis()>nextMotionAlert) {
    Serial.println("Motion!");
    nextMotionAlert = millis()+1000;
  }

  /**
     * After the call to `triggered()`, you can debug the internal
     * status of the detector if you want to find out why it triggered or not
     */
  Serial.println(detector.getTriggerStatus());
}
