/**
 * Implementacja trybu serwisowego
 * v1.4.2 - Naprawiono static variables
 */

#include "service_mode.h"

ServiceMode::ServiceMode(DisplayManager* disp, RelayController* rel) {
    display = disp;
    relays = rel;
    serviceActive = false;
    currentPattern = PATTERN_P1A;
    gunsActive = false;
    activationTime = 0;
    wasPressed = false;  // NOWE v1.4.2
}

void ServiceMode::init() {
    DEBUG_PRINTLN("ServiceMode: Zainicjalizowany");
}

void ServiceMode::show() {
    serviceActive = true;
    gunsActive = false;
    deactivateAllGuns();

    // Rysuj ekran serwisu
    TFT_eSPI* tft = display->getTFT();
    tft->fillScreen(COLOR_BACKGROUND);

    // Nagłówek
    tft->setTextSize(2);
    tft->setTextColor(COLOR_HEADER, COLOR_BACKGROUND);
    tft->setCursor(10, 5);
    tft->println("SERWIS - CZYSZCZENIE");

    // Instrukcja
    tft->setTextSize(1);
    tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
    tft->setCursor(10, 30);
    tft->println("1. Wybierz wzorzec (przyciski)");
    tft->setCursor(10, 45);
    tft->println("2. TRZYMAJ START/PAUZA = pistolety ON");
    tft->setCursor(10, 60);
    tft->println("3. Pusc START/PAUZA = pistolety OFF");

    // Wzorzec
    tft->setTextSize(2);
    tft->setTextColor(COLOR_ACTIVE, COLOR_BACKGROUND);
    tft->setCursor(10, 80);
    tft->print("Wzorzec: ");
    tft->println(getPatternShortName(currentPattern));

    // Wizualizacja pistoletów
    drawGuns();

    // Stopka
    tft->setTextSize(1);
    tft->setTextColor(COLOR_WARNING, COLOR_BACKGROUND);
    tft->setCursor(10, 220);
    tft->println("STOP (2s) = Wyjscie");

    DEBUG_PRINTLN("ServiceMode: Ekran pokazany");
}

void ServiceMode::hide() {
    serviceActive = false;
    gunsActive = false;
    wasPressed = false;  // NOWE v1.4.2: Reset state variables
    deactivateAllGuns();
    display->forceRedraw();
    DEBUG_PRINTLN("ServiceMode: Ukryty");
}

bool ServiceMode::isActive() {
    return serviceActive;
}

void ServiceMode::setPattern(PatternType pattern) {
    if (currentPattern != pattern) {
        currentPattern = pattern;

        // Jeśli pistolety są aktywne - zaktualizuj je
        if (gunsActive) {
            activateGunsByPattern(pattern);
        }

        // Odśwież ekran
        TFT_eSPI* tft = display->getTFT();
        tft->fillRect(100, 80, 120, 20, COLOR_BACKGROUND);
        tft->setTextSize(2);
        tft->setTextColor(COLOR_ACTIVE, COLOR_BACKGROUND);
        tft->setCursor(100, 80);
        tft->println(getPatternShortName(currentPattern));

        drawGuns();

        DEBUG_PRINTF("ServiceMode: Zmiana wzorca na %s\n", getPatternShortName(pattern));
    }
}

PatternType ServiceMode::getPattern() {
    return currentPattern;
}

void ServiceMode::drawGuns() {
    // Rysowanie 6 pistoletów jako kwadraty
    // Układ:
    // [P1] [P2] [P3]
    // [P4] [P5] [P6]

    int16_t startX = 30;
    int16_t startY = 110;
    int16_t gunWidth = 80;
    int16_t gunHeight = 40;
    int16_t spacing = 10;

    // Górny rząd: P1, P2, P3 (oś jezdni, 12cm)
    drawGun(1, startX + 0 * (gunWidth + spacing), startY,
            gunsActive && isGunEnabledForPattern(1, currentPattern),
            isGunEnabledForPattern(1, currentPattern));

    drawGun(2, startX + 1 * (gunWidth + spacing), startY,
            gunsActive && isGunEnabledForPattern(2, currentPattern),
            isGunEnabledForPattern(2, currentPattern));

    drawGun(3, startX + 2 * (gunWidth + spacing), startY,
            gunsActive && isGunEnabledForPattern(3, currentPattern),
            isGunEnabledForPattern(3, currentPattern));

    // Dolny rząd: P4, P5, P6
    drawGun(4, startX + 0 * (gunWidth + spacing), startY + gunHeight + spacing,
            gunsActive && isGunEnabledForPattern(4, currentPattern),
            isGunEnabledForPattern(4, currentPattern));

    drawGun(5, startX + 1 * (gunWidth + spacing), startY + gunHeight + spacing,
            gunsActive && isGunEnabledForPattern(5, currentPattern),
            isGunEnabledForPattern(5, currentPattern));

    drawGun(6, startX + 2 * (gunWidth + spacing), startY + gunHeight + spacing,
            gunsActive && isGunEnabledForPattern(6, currentPattern),
            isGunEnabledForPattern(6, currentPattern));
}

void ServiceMode::drawGun(uint8_t gunNumber, int16_t x, int16_t y, bool active, bool enabled) {
    TFT_eSPI* tft = display->getTFT();

    uint16_t fillColor, borderColor, textColor;

    if (active) {
        // Aktywny (maluje) - zielony
        fillColor = COLOR_SUCCESS;
        borderColor = COLOR_ACTIVE;
        textColor = COLOR_BACKGROUND;
    } else if (enabled) {
        // Włączony dla wzorca ale nieaktywny - żółty
        fillColor = COLOR_WARNING;
        borderColor = COLOR_TEXT;
        textColor = COLOR_BACKGROUND;
    } else {
        // Wyłączony dla wzorca - ciemny
        fillColor = 0x2104; // Ciemnoszary
        borderColor = 0x4208; // Jasnoszary
        textColor = 0x8410; // Średnioszary
    }

    // Prostokąt
    tft->fillRect(x, y, 80, 40, fillColor);
    tft->drawRect(x, y, 80, 40, borderColor);
    tft->drawRect(x + 1, y + 1, 78, 38, borderColor);

    // Numer pistoletu
    tft->setTextSize(3);
    tft->setTextColor(textColor, fillColor);

    // Wycentruj tekst
    char gunText[4];
    sprintf(gunText, "P%d", gunNumber);
    int16_t textX = x + 20;
    int16_t textY = y + 10;

    tft->setCursor(textX, textY);
    tft->println(gunText);
}

void ServiceMode::activateGunsByPattern(PatternType pattern) {
    // Pobierz informacje o wzorcu
    Pattern* pat = getPattern(pattern);
    if (!pat) {
        DEBUG_PRINTLN("ServiceMode: BLAD - Nieznany wzorzec!");
        return;
    }

    // Aktywuj pistolety zgodnie z wzorcem
    relays->setRelay(1, pat->gun1);
    relays->setRelay(2, pat->gun2);
    relays->setRelay(3, pat->gun3);
    relays->setRelay(4, pat->gun4);
    relays->setRelay(5, pat->gun5);
    relays->setRelay(6, pat->gun6);

    gunsActive = true;
    activationTime = millis();

    DEBUG_PRINTF("ServiceMode: Pistolety aktywowane dla wzorca %s\n", pat->name);
    DEBUG_PRINTF("ServiceMode: P1=%d P2=%d P3=%d P4=%d P5=%d P6=%d\n",
                pat->gun1, pat->gun2, pat->gun3, pat->gun4, pat->gun5, pat->gun6);
}

void ServiceMode::deactivateAllGuns() {
    relays->stopAll();
    gunsActive = false;

    if (serviceActive) {
        // Jeśli jesteśmy w serwisie - odśwież ekran
        drawGuns();
    }

    DEBUG_PRINTLN("ServiceMode: Wszystkie pistolety dezaktywowane");
}

bool ServiceMode::isGunEnabledForPattern(uint8_t gunNumber, PatternType pattern) {
    Pattern* pat = getPattern(pattern);
    if (!pat) return false;

    switch (gunNumber) {
        case 1: return pat->gun1;
        case 2: return pat->gun2;
        case 3: return pat->gun3;
        case 4: return pat->gun4;
        case 5: return pat->gun5;
        case 6: return pat->gun6;
        default: return false;
    }
}

const char* ServiceMode::getPatternShortName(PatternType pattern) {
    Pattern* pat = getPattern(pattern);
    return pat ? pat->name : "???";
}

void ServiceMode::update() {
    if (!serviceActive) {
        return;
    }

    // Obsługa przycisku START/PAUZA - HOLD TO FIRE
    // NAPRAWA v1.4.2: Używamy zmiennej członkowskiej zamiast static
    bool isPressed = (digitalRead(BTN_START_PIN) == LOW);

    if (isPressed && !wasPressed) {
        // Przycisk naciśnięty - aktywuj pistolety
        activateGunsByPattern(currentPattern);
        drawGuns();
        DEBUG_PRINTLN("ServiceMode: START/PAUZA nacisniete - pistolety ON");
    } else if (!isPressed && wasPressed) {
        // Przycisk puszczony - dezaktywuj
        deactivateAllGuns();
        DEBUG_PRINTLN("ServiceMode: START/PAUZA puszczony - pistolety OFF");
    }

    wasPressed = isPressed;

    // USUNIĘTE v1.4.2: Duplikacja obsługi STOP
    // Wyjście z serwisu jest obsługiwane w main.cpp (linie 655-673)
    // Ta duplikacja powodowała konflikt static variables i nieprzewidywalne zachowanie
}
