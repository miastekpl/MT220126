/**
 * Implementacja kontrolera przekaźników
 */

#include "relay_controller.h"

RelayController::RelayController() {
    relayPins[0] = RELAY_1_PIN;
    relayPins[1] = RELAY_2_PIN;
    relayPins[2] = RELAY_3_PIN;
    relayPins[3] = RELAY_4_PIN;
    relayPins[4] = RELAY_5_PIN;
    relayPins[5] = RELAY_6_PIN;

    for (int i = 0; i < 6; i++) {
        relayStates[i] = false;
    }
}

void RelayController::init() {
    for (int i = 0; i < 6; i++) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], LOW); // Przekaźniki wyłączone
    }

    DEBUG_PRINTLN("Przekazniki zainicjalizowane");
}

void RelayController::setRelay(uint8_t relayNum, bool state) {
    if (relayNum >= 1 && relayNum <= 6) {
        uint8_t index = relayNum - 1;
        relayStates[index] = state;
        digitalWrite(relayPins[index], state ? HIGH : LOW);

        DEBUG_PRINTF("Przekaznik %d: %s\n", relayNum, state ? "ON" : "OFF");
    }
}

void RelayController::setRelayPattern(bool r1, bool r2, bool r3, bool r4, bool r5, bool r6) {
    setRelay(1, r1);
    setRelay(2, r2);
    setRelay(3, r3);
    setRelay(4, r4);
    setRelay(5, r5);
    setRelay(6, r6);
}

void RelayController::stopAll() {
    for (uint8_t i = 1; i <= 6; i++) {
        setRelay(i, false);
    }
    DEBUG_PRINTLN("Wszystkie przekazniki wylaczone");
}

void RelayController::startAll() {
    for (uint8_t i = 1; i <= 6; i++) {
        setRelay(i, true);
    }
    DEBUG_PRINTLN("Wszystkie przekazniki wlaczone");
}

bool RelayController::getRelayState(uint8_t relayNum) {
    if (relayNum >= 1 && relayNum <= 6) {
        return relayStates[relayNum - 1];
    }
    return false;
}

void RelayController::testSequence() {
    DEBUG_PRINTLN("Start testu przekaznikow...");

    for (uint8_t i = 1; i <= 6; i++) {
        setRelay(i, true);
        delay(200);
        setRelay(i, false);
        delay(100);
    }

    DEBUG_PRINTLN("Test przekaznikow zakonczony");
}

void RelayController::printStatus() {
    DEBUG_PRINT("Status przekaznikow: ");
    for (int i = 0; i < 6; i++) {
        DEBUG_PRINTF("R%d:%s ", i+1, relayStates[i] ? "ON" : "OFF");
    }
    DEBUG_PRINTLN();
}
