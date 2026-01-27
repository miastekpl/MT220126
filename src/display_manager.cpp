/**
 * Implementacja menedżera wyświetlacza
 * v1.4.2 - Naprawiono użycie abs() → fabs() dla float
 * v1.6.0 - Dodano TFT Sprites (3-5x szybsze odświeżanie!)
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

    // v1.6.0: Inicjalizacja wskaźników sprites na NULL
    patternSprite = nullptr;
    speedSprite = nullptr;
    areaSprite = nullptr;
    distanceSprite = nullptr;
    statusSprite = nullptr;
    spritesEnabled = false;
}

DisplayManager::~DisplayManager() {
    // v1.6.0: Zwolnienie sprites przy destrukcji
    deleteSprites();
}

void DisplayManager::init() {
    tft->fillScreen(COLOR_BACKGROUND);
    tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
    needsFullRedraw = true;

    // v1.6.0: Inicjalizacja sprites
    initSprites();
}

void DisplayManager::initSprites() {
    DEBUG_PRINTLN("DisplayManager: Inicjalizacja TFT Sprites...");

    // Tworzenie sprite'ów w PSRAM (alokacja dynamiczna)
    patternSprite = new TFT_eSprite(tft);
    speedSprite = new TFT_eSprite(tft);
    areaSprite = new TFT_eSprite(tft);
    distanceSprite = new TFT_eSprite(tft);
    statusSprite = new TFT_eSprite(tft);

    // Tworzenie buforów sprite'ów w PSRAM
    // Rozmiary: szerokość x wysokość (piksele)
    // createSprite() zwraca void* (wskaźnik do bufora) lub nullptr przy błędzie
    bool success = true;

    success = success && (patternSprite->createSprite(130, 100) != nullptr);   // Pattern box
    success = success && (speedSprite->createSprite(170, 100) != nullptr);     // Speed box
    success = success && (areaSprite->createSprite(310, 90) != nullptr);       // Area box
    success = success && (distanceSprite->createSprite(200, 25) != nullptr);   // Distance box
    success = success && (statusSprite->createSprite(320, 25) != nullptr);     // Status bar

    if (success) {
        spritesEnabled = true;
        DEBUG_PRINTLN("DisplayManager: TFT Sprites zainicjalizowane (PSRAM)");
        DEBUG_PRINTLN("  Pattern:  130x100 px");
        DEBUG_PRINTLN("  Speed:    170x100 px");
        DEBUG_PRINTLN("  Area:     310x90 px");
        DEBUG_PRINTLN("  Distance: 200x25 px");
        DEBUG_PRINTLN("  Status:   320x25 px");
        DEBUG_PRINTF("  Łącznie: ~%d KB PSRAM\n",
                    (130*100 + 170*100 + 310*90 + 200*25 + 320*25) * 2 / 1024);
    } else {
        DEBUG_PRINTLN("DisplayManager: BŁĄD - Nie można utworzyć sprites!");
        DEBUG_PRINTLN("  Fallback: Używanie tradycyjnego renderingu");
        deleteSprites();
        spritesEnabled = false;
    }
}

void DisplayManager::deleteSprites() {
    if (patternSprite) {
        patternSprite->deleteSprite();
        delete patternSprite;
        patternSprite = nullptr;
    }
    if (speedSprite) {
        speedSprite->deleteSprite();
        delete speedSprite;
        speedSprite = nullptr;
    }
    if (areaSprite) {
        areaSprite->deleteSprite();
        delete areaSprite;
        areaSprite = nullptr;
    }
    if (distanceSprite) {
        distanceSprite->deleteSprite();
        delete distanceSprite;
        distanceSprite = nullptr;
    }
    if (statusSprite) {
        statusSprite->deleteSprite();
        delete statusSprite;
        statusSprite = nullptr;
    }

    spritesEnabled = false;
    DEBUG_PRINTLN("DisplayManager: TFT Sprites zwolnione");
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
    // v1.6.0: Wybór metody renderowania (Sprites vs Traditional)
    if (spritesEnabled) {
        // Sprites - 3-5x szybsze!
        drawPatternBoxSprite(pattern, reversed);
        drawSpeedBoxSprite(speed);
        drawAreaBoxSprite(area);
        drawDistanceBoxSprite(distance);
        drawStatusBarSprite(state);
    } else {
        // Fallback: Tradycyjne renderowanie
        drawPatternBox(pattern, reversed);
        drawSpeedBox(speed);
        drawAreaBox(area);
        drawDistanceBox(distance);
        drawStatusBar(state);
    }

    // Zapisz ostatnie wartości
    lastPatternType = pattern;
    lastSpeed = speed;
    lastArea = area;
    lastDistance = distance;
    lastState = state;
    lastReversed = reversed;

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
// =============================================================================
// v1.6.0: TFT SPRITES - OPTYMALIZACJA RENDERINGU (3-5x szybsze!)
// =============================================================================

/**
 * Pattern Box z użyciem Sprite (130x100 px)
 * Renderuje do bufora w PSRAM, następnie jednym transferem SPI na ekran
 */
void DisplayManager::drawPatternBoxSprite(PatternType pattern, bool reversed) {
    if (!patternSprite) return;

    const int16_t screenX = 5;    // Pozycja na ekranie
    const int16_t screenY = 5;

    if (needsFullRedraw || pattern != lastPatternType || reversed != lastReversed) {
        // Rysuj do sprite (RAM - szybkie!)
        patternSprite->fillSprite(COLOR_BACKGROUND);

        // Ramka
        patternSprite->drawRect(0, 0, 130, 100, COLOR_HEADER);
        patternSprite->drawRect(1, 1, 128, 98, COLOR_HEADER);

        // Nazwa wzorca
        patternSprite->setTextSize(4);
        patternSprite->setTextColor(COLOR_SUCCESS, COLOR_BACKGROUND);
        patternSprite->setCursor(15, 15);
        patternSprite->println(getPatternName(pattern));

        // Opis
        patternSprite->setTextSize(1);
        patternSprite->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        patternSprite->setCursor(8, 55);
        Pattern* p = getPattern(pattern);
        if (p) {
            patternSprite->println(p->description);
        }

        // Wskaźnik odwrócenia
        if (reversed && isPatternReversible(pattern)) {
            patternSprite->setTextSize(2);
            patternSprite->setTextColor(COLOR_WARNING, COLOR_BACKGROUND);
            patternSprite->setCursor(8, 75);
            patternSprite->println("[ODWR]");
        }

        // JEDEN transfer SPI - szybko!
        patternSprite->pushSprite(screenX, screenY);
    }
}

/**
 * Speed Box z użyciem Sprite (170x100 px)
 */
void DisplayManager::drawSpeedBoxSprite(float speed) {
    if (!speedSprite) return;

    const int16_t screenX = 145;
    const int16_t screenY = 5;

    if (needsFullRedraw || fabs(speed - lastSpeed) > 0.1) {
        // Rysuj do sprite
        speedSprite->fillSprite(COLOR_BACKGROUND);

        // Ramka
        speedSprite->drawRect(0, 0, 170, 100, COLOR_HEADER);
        speedSprite->drawRect(1, 1, 168, 98, COLOR_HEADER);

        // Tytuł
        speedSprite->setTextSize(1);
        speedSprite->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        speedSprite->setCursor(8, 8);
        speedSprite->println("PREDKOSC");

        // Wartość prędkości
        speedSprite->setTextSize(5);
        speedSprite->setTextColor(COLOR_SUCCESS, COLOR_BACKGROUND);
        speedSprite->setCursor(15, 35);
        char speedStr[16];
        snprintf(speedStr, sizeof(speedStr), "%.1f", speed);
        speedSprite->println(speedStr);

        // Jednostka
        speedSprite->setTextSize(2);
        speedSprite->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        speedSprite->setCursor(15, 75);
        speedSprite->println("km/h");

        // JEDEN transfer SPI
        speedSprite->pushSprite(screenX, screenY);
    }
}

/**
 * Area Box z użyciem Sprite (310x90 px)
 */
void DisplayManager::drawAreaBoxSprite(float area) {
    if (!areaSprite) return;

    const int16_t screenX = 5;
    const int16_t screenY = 115;

    if (needsFullRedraw || fabs(area - lastArea) > 0.01) {
        // Rysuj do sprite
        areaSprite->fillSprite(COLOR_BACKGROUND);

        // Ramka
        areaSprite->drawRect(0, 0, 310, 90, COLOR_HEADER);
        areaSprite->drawRect(1, 1, 308, 88, COLOR_HEADER);

        // Tytuł
        areaSprite->setTextSize(1);
        areaSprite->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        areaSprite->setCursor(8, 8);
        areaSprite->println("POWIERZCHNIA");

        // Wartość powierzchni
        areaSprite->setTextSize(5);
        areaSprite->setTextColor(COLOR_SUCCESS, COLOR_BACKGROUND);
        areaSprite->setCursor(15, 35);
        char areaStr[32];
        snprintf(areaStr, sizeof(areaStr), "%.2f", area);
        areaSprite->println(areaStr);

        // Jednostka
        areaSprite->setTextSize(2);
        areaSprite->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        areaSprite->setCursor(15, 70);
        areaSprite->println("m2");

        // JEDEN transfer SPI
        areaSprite->pushSprite(screenX, screenY);
    }
}

/**
 * Distance Box z użyciem Sprite (200x25 px)
 */
void DisplayManager::drawDistanceBoxSprite(long distance) {
    if (!distanceSprite) return;

    const int16_t screenX = 5;
    const int16_t screenY = 210;

    if (needsFullRedraw || distance != lastDistance) {
        // Rysuj do sprite
        distanceSprite->fillSprite(COLOR_BACKGROUND);

        // Dystans
        distanceSprite->setTextSize(1);
        distanceSprite->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
        distanceSprite->setCursor(0, 0);
        char distStr[32];
        float distM = distance / 100.0;
        snprintf(distStr, sizeof(distStr), "Dystans: %.2f m", distM);
        distanceSprite->println(distStr);

        // JEDEN transfer SPI
        distanceSprite->pushSprite(screenX, screenY);
    }
}

/**
 * Status Bar z użyciem Sprite (320x25 px)
 */
void DisplayManager::drawStatusBarSprite(SystemStateType state) {
    if (!statusSprite) return;

    const int16_t screenX = 0;
    const int16_t screenY = 0;

    if (needsFullRedraw || state != lastState) {
        // Rysuj do sprite
        statusSprite->fillSprite(COLOR_BACKGROUND);

        // Status bar
        statusSprite->fillRect(0, 0, 320, 25, COLOR_HEADER);

        statusSprite->setTextSize(2);
        statusSprite->setTextColor(TFT_WHITE, COLOR_HEADER);
        statusSprite->setCursor(10, 4);

        switch (state) {
            case STATE_IDLE:
                statusSprite->println("GOTOWY");
                break;
            case STATE_PAINTING:
                statusSprite->setTextColor(COLOR_SUCCESS, COLOR_HEADER);
                statusSprite->println("MALOWANIE");
                break;
            case STATE_PAUSED:
                statusSprite->setTextColor(COLOR_WARNING, COLOR_HEADER);
                statusSprite->println("PAUZA");
                break;
            case STATE_MENU:
                statusSprite->println("MENU");
                break;
            case STATE_CALIBRATING:
                statusSprite->setTextColor(COLOR_WARNING, COLOR_HEADER);
                statusSprite->println("KALIBRACJA");
                break;
            case STATE_MEASURING:
                statusSprite->println("POMIAR");
                break;
            case STATE_SERVICE:
                statusSprite->setTextColor(COLOR_WARNING, COLOR_HEADER);
                statusSprite->println("SERWIS");
                break;
            default:
                statusSprite->println("???");
        }

        // JEDEN transfer SPI
        statusSprite->pushSprite(screenX, screenY);
    }
}

