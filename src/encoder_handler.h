/**
 * Obsługa enkodera KY-040
 * Pomiar odległości i prędkości
 * v1.4.2 - Dodano thread-safety (mutex locks)
 */

#ifndef ENCODER_HANDLER_H
#define ENCODER_HANDLER_H

#include <Arduino.h>
#include "config_v140_NEW.h"  // BUGFIX v1.6.4: Zmiana z config.h (konflikt include guards!)
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class EncoderHandler {
private:
    uint8_t clkPin;
    uint8_t dtPin;
    uint8_t swPin;

    volatile long position;
    volatile long distance;  // w cm
    volatile bool aState;
    volatile bool aLastState;

    float pulsesPerCm;
    bool calibrated;

    unsigned long lastUpdateTime;
    long lastPosition;

public:
    EncoderHandler(uint8_t clk, uint8_t dt, uint8_t sw);

    void init();
    void update();
    void reset();
    void resetDistance();

    long getDistance();  // Zwraca dystans w cm
    long getPosition();
    float getSpeed();    // Zwraca prędkość w m/s

    void setPulsesPerCm(float pulses);
    float getPulsesPerCm();
    bool isCalibrated();

    bool isButtonPressed();
};

#endif // ENCODER_HANDLER_H
