#include "camera_model.h"

CameraModel *CameraModel::instance = nullptr;

CameraModel::CameraModel()
{
    // Set up defaults.
    isDitheringDisabled = false;
    isFlashEnabled = false;
    isInverted = false;
    isStreamEnabled = true;
    ditherAlgorithm = FLOYD_STEINBERG;
}

CameraModel *CameraModel::getInstance()
{
    if (instance == nullptr)
    {
        instance = new CameraModel();
    }
    return instance;
}

// Getter implementations
bool CameraModel::getIsDitheringDisabled()
{
    return isDitheringDisabled;
}

bool CameraModel::getIsFlashEnabled()
{
    return isFlashEnabled;
}

bool CameraModel::getIsInverted()
{
    return isInverted;
}

bool CameraModel::getIsStreamEnabled()
{
    return isStreamEnabled;
}

DitheringAlgorithm CameraModel::getDitherAlgorithm()
{
    return ditherAlgorithm;
}

// Setter implementations
void CameraModel::setIsDitheringDisabled(bool value)
{
    isDitheringDisabled = value;
}

void CameraModel::setIsFlashEnabled(bool value)
{
    isFlashEnabled = value;
}

void CameraModel::setIsInverted(bool value)
{
    isInverted = value;
}

void CameraModel::setIsStreamEnabled(bool value)
{
    isStreamEnabled = value;
}

void CameraModel::setDitherAlgorithm(DitheringAlgorithm algorithm)
{
    ditherAlgorithm = algorithm;
}
