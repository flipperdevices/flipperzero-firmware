#ifndef CAMERA_MODEL_H
#define CAMERA_MODEL_H

#include <stdint.h>

/**
 * The dithering algorithms available.
 */
enum DitheringAlgorithm {
    FLOYD_STEINBERG,
    JARVIS_JUDICE_NINKE,
    STUCKI,
};

class CameraModel {
  private:
    static CameraModel* instance;
    // Private constructor to prevent instantiation.
    CameraModel();
    /**
     * Flag to enable or disable dithering.
     */
    bool isDitheringDisabled;
    /**
     * Flag to represent the flash state when saving pictures to the Flipper.
     */
    bool isFlashEnabled;
    /**
     * Flag to invert pixel colors.
     */
    bool isInverted;
    /**
     * Flag to stop or start the stream.
     */
    bool isStreamEnabled;
    /**
     * Holds the currently selected dithering algorithm.
     */
    DitheringAlgorithm ditherAlgorithm;

  public:
    static CameraModel* getInstance();

    // Getter functions
    bool getIsDitheringDisabled();
    bool getIsFlashEnabled();
    bool getIsInverted();
    bool getIsStreamEnabled();
    DitheringAlgorithm getDitherAlgorithm();

    // Setter functions
    void setIsDitheringDisabled(bool value);
    void setIsFlashEnabled(bool value);
    void setIsInverted(bool value);
    void setIsStreamEnabled(bool value);
    void setDitherAlgorithm(DitheringAlgorithm value);
};

#endif
