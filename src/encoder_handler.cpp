/**
 * Implementacja obsługi enkodera
 */

#include "encoder_handler.h"

EncoderHandler::EncoderHandler(uint8_t clk, uint8_t dt, uint8_t sw) {
    clkPin = clk;
    dtPin = dt;
    swPin = sw;
    position = 0;
    distance = 0;
    aState = false;
    aLastState = false;
    pulsesPerCm = 1.0; // Domyślnie, zmieniane po kalibracji
    calibrated = false;
    lastUpdateTime = 0;
    lastPosition = 0;
}

void EncoderHandler::init() {
    pinMode(clkPin, INPUT_PULLUP);
    pinMode(dtPin, INPUT_PULLUP);
    pinMode(swPin, INPUT_PULLUP);

    aLastState = digitalRead(clkPin);

    DEBUG_PRINTLN("Enkoder zainicjalizowany");
}

void EncoderHandler::update() {
    aState = digitalRead(clkPin);

    if (aState != aLastState) {
        // Zmiana stanu CLK
        if (digitalRead(dtPin) != aState) {
            position++;
        } else {
            position--;
        }

        // Przeliczanie na dystans
        if (calibrated && pulsesPerCm > 0) {
            distance = (long)(position / pulsesPerCm);
        }

        aLastState = aState;
    }
}

void EncoderHandler::reset() {
    position = 0;
    distance = 0;
    lastPosition = 0;
    lastUpdateTime = millis();
}

void EncoderHandler::resetDistance() {
    distance = 0;
    position = 0;
}

long EncoderHandler::getDistance() {
    return distance;
}

long EncoderHandler::getPosition() {
    return position;
}

float EncoderHandler::getSpeed() {
    unsigned long currentTime = millis();
    unsigned long timeDiff = currentTime - lastUpdateTime;

    if (timeDiff == 0) {
        return 0;
    }

    long positionDiff = position - lastPosition;
    float distanceDiffCm = positionDiff / pulsesPerCm;
    float distanceDiffM = distanceDiffCm / 100.0;
    float timeDiffS = timeDiff / 1000.0;

    float speed = distanceDiffM / timeDiffS; // m/s

    lastUpdateTime = currentTime;
    lastPosition = position;

    return speed;
}

void EncoderHandler::setPulsesPerCm(float pulses) {
    pulsesPerCm = pulses;
    calibrated = true;
    DEBUG_PRINTF("Enkoder: ustawiono %f impulsow/cm\n", pulses);
}

float EncoderHandler::getPulsesPerCm() {
    return pulsesPerCm;
}

bool EncoderHandler::isCalibrated() {
    return calibrated;
}

bool EncoderHandler::isButtonPressed() {
    return digitalRead(swPin) == LOW;
}
