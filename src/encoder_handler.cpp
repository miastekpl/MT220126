/**
 * Implementacja obsługi enkodera
 * v1.4.2 - Dodano thread-safety (mutex locks)
 */

#include "encoder_handler.h"

// NOWE v1.4.2: Deklaracja zewnętrznego mutexu
extern SemaphoreHandle_t encoderMutex;

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
    // NOWE v1.4.2: Thread-safe dostęp do position i distance
    if (encoderMutex != NULL && xSemaphoreTake(encoderMutex, portMAX_DELAY) == pdTRUE) {
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

        xSemaphoreGive(encoderMutex);
    } else {
        // Fallback bez mutexu (na wypadek gdyby mutex nie był zainicjalizowany)
        aState = digitalRead(clkPin);
        if (aState != aLastState) {
            if (digitalRead(dtPin) != aState) {
                position++;
            } else {
                position--;
            }
            if (calibrated && pulsesPerCm > 0) {
                distance = (long)(position / pulsesPerCm);
            }
            aLastState = aState;
        }
    }
}

void EncoderHandler::reset() {
    // NOWE v1.4.2: Thread-safe reset
    if (encoderMutex != NULL && xSemaphoreTake(encoderMutex, portMAX_DELAY) == pdTRUE) {
        position = 0;
        distance = 0;
        lastPosition = 0;
        lastUpdateTime = millis();
        xSemaphoreGive(encoderMutex);
    } else {
        // Fallback bez mutexu
        position = 0;
        distance = 0;
        lastPosition = 0;
        lastUpdateTime = millis();
    }
}

void EncoderHandler::resetDistance() {
    // NOWE v1.4.2: Thread-safe reset distance
    if (encoderMutex != NULL && xSemaphoreTake(encoderMutex, portMAX_DELAY) == pdTRUE) {
        distance = 0;
        position = 0;
        xSemaphoreGive(encoderMutex);
    } else {
        // Fallback bez mutexu
        distance = 0;
        position = 0;
    }
}

long EncoderHandler::getDistance() {
    // NOWE v1.4.2: Thread-safe odczyt distance
    long result = 0;
    if (encoderMutex != NULL && xSemaphoreTake(encoderMutex, portMAX_DELAY) == pdTRUE) {
        result = distance;
        xSemaphoreGive(encoderMutex);
    } else {
        // Fallback bez mutexu
        result = distance;
    }
    return result;
}

long EncoderHandler::getPosition() {
    // NOWE v1.4.2: Thread-safe odczyt position
    long result = 0;
    if (encoderMutex != NULL && xSemaphoreTake(encoderMutex, portMAX_DELAY) == pdTRUE) {
        result = position;
        xSemaphoreGive(encoderMutex);
    } else {
        // Fallback bez mutexu
        result = position;
    }
    return result;
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
