/**
 * Implementacja systemu kalibracji
 */

#include "calibration.h"

CalibrationManager::CalibrationManager(EncoderHandler* enc) {
    encoder = enc;
    calibrationInProgress = false;
    startPosition = 0;

    calData.magicNumber = 0;
    calData.pulsesPerCm = 0;
    calData.calibrationDate = 0;
    calData.checksum = 0;
}

void CalibrationManager::init() {
    preferences.begin("calibration", false);
    loadCalibration();
    DEBUG_PRINTLN("Kalibracja zainicjalizowana");
}

uint8_t CalibrationManager::calculateChecksum(CalibrationData* data) {
    uint8_t checksum = 0;
    uint8_t* ptr = (uint8_t*)data;

    for (size_t i = 0; i < sizeof(CalibrationData) - 1; i++) {
        checksum ^= ptr[i];
    }

    return checksum;
}

void CalibrationManager::saveCalibration() {
    calData.magicNumber = EEPROM_MAGIC_NUMBER;
    calData.calibrationDate = millis() / 1000; // Timestamp w sekundach
    calData.checksum = calculateChecksum(&calData);

    preferences.putBytes("caldata", &calData, sizeof(CalibrationData));
    preferences.end();

    DEBUG_PRINTLN("Kalibracja zapisana");
    DEBUG_PRINTF("Impulsy/cm: %.4f\n", calData.pulsesPerCm);
}

void CalibrationManager::loadCalibration() {
    size_t len = preferences.getBytes("caldata", &calData, sizeof(CalibrationData));

    if (len == sizeof(CalibrationData) &&
        calData.magicNumber == EEPROM_MAGIC_NUMBER &&
        calculateChecksum(&calData) == calData.checksum) {

        encoder->setPulsesPerCm(calData.pulsesPerCm);

        DEBUG_PRINTLN("Kalibracja wczytana");
        DEBUG_PRINTF("Impulsy/cm: %.4f\n", calData.pulsesPerCm);
        DEBUG_PRINTF("Data kalibracji: %u\n", calData.calibrationDate);
    } else {
        DEBUG_PRINTLN("Brak poprawnej kalibracji");
        calData.magicNumber = 0;
        calData.pulsesPerCm = 0;
        calData.calibrationDate = 0;
    }
}

bool CalibrationManager::isCalibrated() {
    return encoder->isCalibrated() && calData.magicNumber == EEPROM_MAGIC_NUMBER;
}

void CalibrationManager::startCalibration() {
    calibrationInProgress = true;
    startPosition = encoder->getPosition();
    encoder->resetDistance();

    DEBUG_PRINTLN("Start kalibracji");
    DEBUG_PRINTF("Pozycja startowa: %ld\n", startPosition);
}

void CalibrationManager::finishCalibration() {
    long endPosition = encoder->getPosition();
    long totalPulses = abs(endPosition - startPosition);

    // Obliczanie impulsów na centymetr (10m = 1000cm)
    calData.pulsesPerCm = (float)totalPulses / CALIBRATION_DISTANCE_CM;

    encoder->setPulsesPerCm(calData.pulsesPerCm);

    saveCalibration();

    calibrationInProgress = false;

    DEBUG_PRINTLN("Kalibracja zakonczona");
    DEBUG_PRINTF("Calkowite impulsy: %ld\n", totalPulses);
    DEBUG_PRINTF("Impulsy/cm: %.4f\n", calData.pulsesPerCm);
}

void CalibrationManager::cancelCalibration() {
    calibrationInProgress = false;
    encoder->resetDistance();
    DEBUG_PRINTLN("Kalibracja anulowana");
}

CalibrationResult CalibrationManager::process() {
    // Ta funkcja jest wywoływana w pętli głównej podczas kalibracji
    // Zwraca status kalibracji

    if (calibrationInProgress) {
        return CALIBRATION_IN_PROGRESS;
    }

    return CALIBRATION_NONE;
}

float CalibrationManager::getPulsesPerCm() {
    return calData.pulsesPerCm;
}

uint32_t CalibrationManager::getCalibrationDate() {
    return calData.calibrationDate;
}
