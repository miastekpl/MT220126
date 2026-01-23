/**
 * System kalibracji enkodera
 * Kalibracja na dokładnie 10 metrów
 */

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>
#include <Preferences.h>
#include "config.h"
#include "encoder_handler.h"

class CalibrationManager {
private:
    EncoderHandler* encoder;
    Preferences preferences;

    bool calibrationInProgress;
    long startPosition;
    CalibrationData calData;

    void saveCalibration();
    void loadCalibration();
    uint8_t calculateChecksum(CalibrationData* data);

public:
    CalibrationManager(EncoderHandler* enc);

    void init();
    bool isCalibrated();
    void startCalibration();
    void finishCalibration();
    void cancelCalibration();
    CalibrationResult process();

    float getPulsesPerCm();
    uint32_t getCalibrationDate();
};

#endif // CALIBRATION_H
