/**
 * Implementacja menedżera wyświetlacza
 * v1.4.2 - Naprawiono użycie abs() → fabs() dla float
 */

#include "display_manager.h"
#include <cmath>  // NOWE v1.4.2: dla fabs()

DisplayManager::DisplayManager(TFT_eSPI* tftDisplay) {
    tft = tftDisplay;
    lastPatternType = PATTERN_P1A;
    lastSpeed = -1;
    lastArea = -1;
    lastDistance = -1;
    lastState = STATE_IDLE;
    lastReversed = false;
    needsFullRedraw = true;
}

void DisplayManager::init() {
    tft->fillScreen(COLOR_BACKGROUND);
    tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
    needsFullRedraw = true;
}

void DisplayManager::showSplashScreen(const char* version) {
    tft->fillScreen(COLOR_BACKGROUND);
    tft->setTextColor(COLOR_HEADER);

    // Tytuł
    tft->setTextSize(3);
    tft->setCursor(10, 40);
    tft->println("SYSTEM MALOWANIA");
    tft->setCursor(10, 70);
    tft->println("PASOW DROGOWYCH");

    // Wersja
    tft->setTextColor(COLOR_TEXT);
    tft->setTextSize(2);
    tft->setCursor(10, 130);
    tft->print("Wersja: ");
    tft->println(version);

    // Copyright
    tft->setTextSize(1);
    tft->setCursor(10, 200);
    tft->setTextColor(COLOR_WARNING);
    tft->println("(C) 2026 MT220126 Engineering Team");
    tft->setCursor(10, 215);
    tft->println("200+ lat doswiadczenia");
}

void DisplayManager::clear() {
    tft->fillScreen(COLOR_BACKGROUND);
    needsFullRedraw = true;
}

void DisplayManager::forceRedraw() {
    needsFullRedraw = true;
}

void DisplayManager::clearArea(int16_t x, int16_t y, int16_t w, int16_t h) {
    tft->fillRect(x, y, w, h, COLOR_BACKGROUND);
}

void DisplayManager::drawPatternBox(PatternType pattern, bool reversed) {
    const int16_t x = 5;
    const int16_t y = 5;
    const int16_t w = 130;
    const int16_t h = 100;

    if (needsFullRedraw || pattern != lastPatternType || reversed != lastReversed) {
        // Ramka
        tft->drawRect(x, y, w, h, COLOR_HEADER);
        tft->drawRect(x+1, y+1, w-2, h-2, COLOR_HEADER);

        // Czyszczenie wnętrza
        clearArea(x+3, y+3, w-6, h-6);

        // Nazwa wzorca
        tft->setTextSize(4);
        tft->setTextColor(COLOR_SUCCESS, COLOR_BACKGROUND);
        tft->setCursor(x + 15, y + 15);
        tft->println(getPatternName(pattern));

        // Opis
        tft->setTextSize(1);
        tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        tft->setCursor(x + 8, y + 55);
        Pattern* p = getPattern(pattern);
        if (p) {
            tft->println(p->description);
        }

        // Wskaźnik odwrócenia dla P-3a/P-3b
        if (reversed && isPatternReversible(pattern)) {
            tft->setTextSize(2);
            tft->setTextColor(COLOR_WARNING, COLOR_BACKGROUND);
            tft->setCursor(x + 8, y + 75);
            tft->println("[ODWR]");
        }

        lastPatternType = pattern;
        lastReversed = reversed;
    }
}

void DisplayManager::drawSpeedBox(float speed) {
    const int16_t x = 145;
    const int16_t y = 5;
    const int16_t w = 170;
    const int16_t h = 100;

    if (needsFullRedraw || fabs(speed - lastSpeed) > 0.1) {  // NAPRAWA v1.4.2: fabs() dla float
        // Ramka
        tft->drawRect(x, y, w, h, COLOR_HEADER);
        tft->drawRect(x+1, y+1, w-2, h-2, COLOR_HEADER);

        // Czyszczenie wnętrza
        clearArea(x+3, y+3, w-6, h-6);

        // Etykieta
        tft->setTextSize(1);
        tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        tft->setCursor(x + 8, y + 10);
        tft->println("PREDKOSC");

        // Wartość prędkości
        tft->setTextSize(5);
        tft->setTextColor(COLOR_SUCCESS, COLOR_BACKGROUND);
        tft->setCursor(x + 15, y + 35);
        char speedStr[16];
        snprintf(speedStr, sizeof(speedStr), "%.1f", speed);  // v1.5.0: snprintf zamiast sprintf
        tft->println(speedStr);

        // Jednostka
        tft->setTextSize(2);
        tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        tft->setCursor(x + 125, y + 55);
        tft->println("km/h");

        lastSpeed = speed;
    }
}

void DisplayManager::drawAreaBox(float area) {
    const int16_t x = 5;
    const int16_t y = 115;
    const int16_t w = 310;
    const int16_t h = 90;

    if (needsFullRedraw || fabs(area - lastArea) > 0.01) {  // NAPRAWA v1.4.2: fabs() dla float
        // Ramka
        tft->drawRect(x, y, w, h, COLOR_HEADER);
        tft->drawRect(x+1, y+1, w-2, h-2, COLOR_HEADER);

        // Czyszczenie wnętrza
        clearArea(x+3, y+3, w-6, h-6);

        // Etykieta
        tft->setTextSize(1);
        tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        tft->setCursor(x + 8, y + 10);
        tft->println("WYMALOWANA POWIERZCHNIA");

        // Wartość powierzchni
        tft->setTextSize(5);
        tft->setTextColor(COLOR_SUCCESS, COLOR_BACKGROUND);
        tft->setCursor(x + 15, y + 35);
        char areaStr[32];
        snprintf(areaStr, sizeof(areaStr), "%.2f", area);  // v1.5.0: snprintf zamiast sprintf
        tft->println(areaStr);

        // Jednostka
        tft->setTextSize(3);
        tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        tft->setCursor(x + 230, y + 50);
        tft->println("m2");

        lastArea = area;
    }
}

void DisplayManager::drawDistanceBox(long distance) {
    const int16_t x = 5;
    const int16_t y = 215;
    const int16_t w = 150;
    const int16_t h = 20;

    if (needsFullRedraw || distance != lastDistance) {
        // Czyszczenie
        clearArea(x, y, w, h);

        // Dystans
        tft->setTextSize(1);
        tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        tft->setCursor(x, y);
        char distStr[32];
        float distM = distance / 100.0;
        snprintf(distStr, sizeof(distStr), "Dystans: %.2f m", distM);  // v1.5.0: snprintf zamiast sprintf
        tft->println(distStr);

        lastDistance = distance;
    }
}

void DisplayManager::drawStatusBar(SystemStateType state) {
    const int16_t x = 165;
    const int16_t y = 215;
    const int16_t w = 150;
    const int16_t h = 20;

    if (needsFullRedraw || state != lastState) {
        // Czyszczenie
        clearArea(x, y, w, h);

        // Status
        tft->setTextSize(1);
        const char* statusText;
        uint16_t statusColor;

        switch (state) {
            case STATE_IDLE:
                statusText = "GOTOWY";
                statusColor = COLOR_TEXT;
                break;
            case STATE_PAINTING:
                statusText = "MALOWANIE";
                statusColor = COLOR_SUCCESS;
                break;
            case STATE_PAUSED:
                statusText = "PAUZA";
                statusColor = COLOR_WARNING;
                break;
            case STATE_MENU:
                statusText = "MENU";
                statusColor = COLOR_HEADER;
                break;
            case STATE_CALIBRATING:
                statusText = "KALIBRACJA";
                statusColor = COLOR_WARNING;
                break;
            case STATE_MEASURING:
                statusText = "POMIAR";
                statusColor = COLOR_HEADER;
                break;
            default:
                statusText = "???";
                statusColor = COLOR_ERROR;
        }

        tft->setTextColor(statusColor, COLOR_BACKGROUND);
        tft->setCursor(x, y);
        tft->print("Status: ");
        tft->println(statusText);

        lastState = state;
    }
}

void DisplayManager::showMainScreen(PatternType pattern, float speed, float area,
                                   long distance, SystemStateType state, bool reversed) {
    drawPatternBox(pattern, reversed);
    drawSpeedBox(speed);
    drawAreaBox(area);
    drawDistanceBox(distance);
    drawStatusBar(state);

    needsFullRedraw = false;
}

void DisplayManager::showWarning(const char* title, const char* message) {
    tft->fillScreen(COLOR_BACKGROUND);
    tft->drawRect(10, 10, 300, 220, COLOR_WARNING);
    tft->drawRect(11, 11, 298, 218, COLOR_WARNING);

    // Tytuł
    tft->setTextSize(2);
    tft->setTextColor(COLOR_WARNING, COLOR_BACKGROUND);
    tft->setCursor(20, 30);
    tft->println(title);

    // Wiadomość
    tft->setTextSize(1);
    tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
    tft->setCursor(20, 80);

    // Zawijanie tekstu
    int cursorY = 80;
    char* msgCopy = strdup(message);
    char* token = strtok(msgCopy, " ");
    int lineLen = 0;
    String line = "";

    while (token != NULL) {
        String word = String(token);
        if (lineLen + word.length() > 35) {
            tft->setCursor(20, cursorY);
            tft->println(line);
            cursorY += 15;
            line = word + " ";
            lineLen = word.length() + 1;
        } else {
            line += word + " ";
            lineLen += word.length() + 1;
        }
        token = strtok(NULL, " ");
    }
    if (line.length() > 0) {
        tft->setCursor(20, cursorY);
        tft->println(line);
    }
    free(msgCopy);
}

void DisplayManager::showError(const char* title, const char* message) {
    tft->fillScreen(COLOR_BACKGROUND);
    tft->drawRect(10, 10, 300, 220, COLOR_ERROR);
    tft->drawRect(11, 11, 298, 218, COLOR_ERROR);

    tft->setTextSize(2);
    tft->setTextColor(COLOR_ERROR, COLOR_BACKGROUND);
    tft->setCursor(20, 30);
    tft->println(title);

    tft->setTextSize(1);
    tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
    tft->setCursor(20, 80);
    tft->println(message);
}

void DisplayManager::showSuccess(const char* message) {
    tft->fillScreen(COLOR_BACKGROUND);
    tft->drawRect(10, 10, 300, 220, COLOR_SUCCESS);
    tft->drawRect(11, 11, 298, 218, COLOR_SUCCESS);

    tft->setTextSize(2);
    tft->setTextColor(COLOR_SUCCESS, COLOR_BACKGROUND);
    tft->setCursor(20, 100);
    tft->println(message);
}

void DisplayManager::showMessage(const char* message, uint16_t color) {
    tft->fillScreen(COLOR_BACKGROUND);
    tft->setTextSize(2);
    tft->setTextColor(color, COLOR_BACKGROUND);
    tft->setCursor(20, 100);
    tft->println(message);
}
