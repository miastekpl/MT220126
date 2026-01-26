/**
 * Implementacja systemu menu
 */

#include "menu_system.h"

const char* MENU_ITEMS_TEXT[MENU_ITEM_COUNT] = {
    "Kalibracja",
    "Pomiar dystansu",
    "Zeruj liczniki",
    "Informacje",
    "Serwis",        // NOWE v1.4.1 - czyszczenie pistoletów
    "Wyjscie"
};

MenuSystem::MenuSystem(DisplayManager* disp, EncoderHandler* enc) {
    display = disp;
    encoder = enc;
    menuActive = false;
    selectedItem = 0;
    inSubMenu = false;
    lastJoystickRead = 0;
    lastButtonPress = 0;
}

void MenuSystem::init() {
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);
    pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);

    DEBUG_PRINTLN("Menu zainicjalizowane");
}

void MenuSystem::show() {
    menuActive = true;
    selectedItem = 0;
    inSubMenu = false;
    display->clear();
    drawMenu();
}

void MenuSystem::hide() {
    menuActive = false;
    display->forceRedraw();
}

bool MenuSystem::isActive() {
    return menuActive;
}

int MenuSystem::readJoystickX() {
    return analogRead(JOYSTICK_X_PIN);
}

int MenuSystem::readJoystickY() {
    return analogRead(JOYSTICK_Y_PIN);
}

bool MenuSystem::isJoystickPressed() {
    return digitalRead(JOYSTICK_SW_PIN) == LOW;
}

void MenuSystem::drawMenu() {
    TFT_eSPI* tft = display->getTFT();

    tft->fillScreen(COLOR_BACKGROUND);

    // Nagłówek
    tft->setTextSize(2);
    tft->setTextColor(COLOR_HEADER, COLOR_BACKGROUND);
    tft->setCursor(10, 10);
    tft->println("MENU GLOWNE");

    tft->drawLine(10, 35, 310, 35, COLOR_HEADER);

    // Pozycje menu
    for (uint8_t i = 0; i < MENU_ITEM_COUNT; i++) {
        drawMenuItem(i, MENU_ITEMS_TEXT[i], i == selectedItem);
    }

    // Instrukcje
    tft->setTextSize(1);
    tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
    tft->setCursor(10, 220);
    tft->println("Joystick: GORA/DOL | SELECT: Wybierz");
}

void MenuSystem::drawMenuItem(uint8_t index, const char* text, bool selected) {
    TFT_eSPI* tft = display->getTFT();

    int16_t y = 50 + (index * 30);

    if (selected) {
        tft->fillRect(10, y, 300, 25, COLOR_BUTTON);
        tft->setTextColor(COLOR_ACTIVE, COLOR_BUTTON);
    } else {
        tft->fillRect(10, y, 300, 25, COLOR_BACKGROUND);
        tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
    }

    tft->setTextSize(2);
    tft->setCursor(20, y + 5);
    tft->println(text);
}

void MenuSystem::handleSelection() {
    switch (selectedItem) {
        case MENU_ITEM_CALIBRATION:
            DEBUG_PRINTLN("Menu: Wybrano kalibracje");
            // Kalibracja będzie obsługiwana w głównym programie
            hide();
            break;

        case MENU_ITEM_MEASURE_DISTANCE:
            DEBUG_PRINTLN("Menu: Wybrano pomiar dystansu");
            hide();
            break;

        case MENU_ITEM_RESET_COUNTER:
            DEBUG_PRINTLN("Menu: Zerowanie licznikow");
            encoder->resetDistance();
            display->showSuccess("Liczniki wyzerowane!");
            delay(1500);
            drawMenu();
            break;

        case MENU_ITEM_SYSTEM_INFO:
            DEBUG_PRINTLN("Menu: Informacje systemowe");
            display->clear();
            {
                TFT_eSPI* tft = display->getTFT();
                tft->setTextSize(2);
                tft->setTextColor(COLOR_HEADER, COLOR_BACKGROUND);
                tft->setCursor(10, 10);
                tft->println("INFORMACJE");

                tft->setTextSize(1);
                tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
                tft->setCursor(10, 50);
                tft->println("System Malowania Pasow Drogowych");
                tft->setCursor(10, 70);
                tft->println("Wersja: 1.4.1");
                tft->setCursor(10, 90);
                tft->println("Platforma: ESP32-S3 N16R8");
                tft->setCursor(10, 110);
                tft->printf("Impulsy/cm: %.4f\n", encoder->getPulsesPerCm());
                tft->setCursor(10, 130);
                tft->printf("Kalibrowany: %s\n", encoder->isCalibrated() ? "TAK" : "NIE");

                tft->setCursor(10, 200);
                tft->setTextColor(COLOR_WARNING, COLOR_BACKGROUND);
                tft->println("Nacisnij joystick aby wrocic");
            }
            inSubMenu = true;
            break;

        case MENU_ITEM_SERVICE:
            DEBUG_PRINTLN("Menu: Wybrano serwis (czyszczenie pistoletow)");
            // Serwis będzie obsługiwany w głównym programie
            hide();
            break;

        case MENU_ITEM_EXIT:
            DEBUG_PRINTLN("Menu: Wyjscie");
            hide();
            break;
    }
}

MenuResult MenuSystem::update() {
    if (!menuActive) {
        return MENU_NONE;
    }

    unsigned long currentTime = millis();

    // Obsługa joysticka (odczyt co 200ms)
    if (currentTime - lastJoystickRead > 200) {
        int joyY = readJoystickY();

        if (!inSubMenu) {
            // Nawigacja w menu
            if (joyY < 1000) { // Góra
                if (selectedItem > 0) {
                    selectedItem--;
                    drawMenu();
                }
            } else if (joyY > 3000) { // Dół
                if (selectedItem < MENU_ITEM_COUNT - 1) {
                    selectedItem++;
                    drawMenu();
                }
            }
        }

        lastJoystickRead = currentTime;
    }

    // Obsługa przycisku joysticka
    if (isJoystickPressed() && currentTime - lastButtonPress > 300) {
        delay(50); // Debounce

        if (isJoystickPressed()) {
            if (inSubMenu) {
                // Wyjście z podmenu
                inSubMenu = false;
                drawMenu();
            } else {
                // Wybór pozycji
                handleSelection();

                if (selectedItem == MENU_ITEM_CALIBRATION) {
                    return MENU_CALIBRATION_START;
                } else if (selectedItem == MENU_ITEM_MEASURE_DISTANCE) {
                    return MENU_MEASURE_START;
                } else if (selectedItem == MENU_ITEM_SERVICE) {
                    return MENU_SERVICE_START;  // NOWE v1.4.1
                } else if (selectedItem == MENU_ITEM_EXIT) {
                    return MENU_EXIT;
                }
            }

            lastButtonPress = currentTime;
        }
    }

    // Sprawdzanie długiego przytrzymania STOP (wyjście z menu)
    static unsigned long stopPressTime = 0;
    static bool stopPressed = false;

    if (digitalRead(BTN_STOP_PIN) == LOW) {
        if (!stopPressed) {
            stopPressed = true;
            stopPressTime = millis();
        }

        if (millis() - stopPressTime > 2000) {
            hide();
            stopPressed = false;
            return MENU_EXIT;
        }
    } else {
        stopPressed = false;
    }

    return MENU_NONE;
}
