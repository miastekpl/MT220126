/**
 * Kontroler przekaźników
 * Sterowanie 6 pistoletami malarskimi
 */

#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>
#include "config_v140_NEW.h"  // BUGFIX v1.6.4: Zmiana z config.h (konflikt include guards!)

class RelayController {
private:
    uint8_t relayPins[6];
    bool relayStates[6];

public:
    RelayController();

    void init();
    void setRelay(uint8_t relayNum, bool state);
    void setRelayPattern(bool r1, bool r2, bool r3, bool r4, bool r5, bool r6);
    void stopAll();
    void startAll();
    bool getRelayState(uint8_t relayNum);
    void testSequence();
    void printStatus();
};

#endif // RELAY_CONTROLLER_H
