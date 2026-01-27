/**
 * Tryb serwisowy - czyszczenie pistoletów malarskich
 * v1.4.2 - Naprawiono static variables (przenoszone do członków klasy)
 *
 * FUNKCJONALNOŚĆ:
 * - Wizualizacja 6 pistoletów (kwadraty na ekranie)
 * - Wybór wzorca malowania
 * - Hold START/PAUZA → aktywacja pistoletów
 * - JEDYNY moment gdy pistolety mogą być aktywne na postoju
 * - Zalecane: przed każdym malowaniem i raz w tygodniu
 */

#ifndef SERVICE_MODE_H
#define SERVICE_MODE_H

#include <Arduino.h>
#include "config_v140_NEW.h"  // BUGFIX v1.6.4: Zmiana z config.h (konflikt include guards!)
#include "display_manager.h"
#include "relay_controller.h"
#include "patterns.h"

class ServiceMode {
private:
    DisplayManager* display;
    RelayController* relays;

    bool serviceActive;
    PatternType currentPattern;
    bool gunsActive;
    unsigned long activationTime;

    // NOWE v1.4.2: State variables dla hold-to-fire (były static w update())
    bool wasPressed;

    // Wizualizacja pistoletów
    void drawGuns();
    void drawGun(uint8_t gunNumber, int16_t x, int16_t y, bool active, bool enabled);

    // Obsługa wzorców
    void activateGunsByPattern(PatternType pattern);
    void deactivateAllGuns();
    bool isGunEnabledForPattern(uint8_t gunNumber, PatternType pattern);

    // Pomocnicze
    const char* getPatternShortName(PatternType pattern);

public:
    /**
     * Konstruktor
     */
    ServiceMode(DisplayManager* disp, RelayController* rel);

    /**
     * Inicjalizacja
     */
    void init();

    /**
     * Pokazuje ekran serwisu
     */
    void show();

    /**
     * Ukrywa ekran serwisu
     */
    void hide();

    /**
     * Aktualizacja - wywoływać w loop()
     * Obsługuje:
     * - Wybór wzorca (przyciski P-1a do P-7d)
     * - Hold START/PAUZA → aktywacja pistoletów
     * - Release START/PAUZA → dezaktywacja
     */
    void update();

    /**
     * Czy serwis aktywny?
     */
    bool isActive();

    /**
     * Zmień wzorzec
     */
    void setPattern(PatternType pattern);

    /**
     * Pobierz aktualny wzorzec
     */
    PatternType getPattern();
};

#endif // SERVICE_MODE_H
