/**
 * Menedżer wyświetlacza ILI9341
 * Obsługa wszystkich operacji graficznych
 * v1.6.0 - Dodano TFT Sprites (3-5x szybsze odświeżanie!)
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "config.h"
#include "patterns.h"

class DisplayManager {
private:
    TFT_eSPI* tft;

    // v1.6.0: TFT Sprites dla optymalizacji (double buffering w PSRAM)
    TFT_eSprite* patternSprite;   // Sprite dla wzorca (130x90)
    TFT_eSprite* speedSprite;     // Sprite dla prędkości (170x100)
    TFT_eSprite* areaSprite;      // Sprite dla powierzchni (310x90)
    TFT_eSprite* distanceSprite;  // Sprite dla dystansu (200x25)
    TFT_eSprite* statusSprite;    // Sprite dla paska statusu (320x25)

    bool spritesEnabled;          // Czy sprites włączone (można wyłączyć dla debug)

    uint16_t lastPatternType;
    float lastSpeed;
    float lastArea;
    long lastDistance;
    SystemStateType lastState;
    bool lastReversed;
    bool needsFullRedraw;

    // v1.6.0: Nowe metody z sprites
    void drawPatternBoxSprite(PatternType pattern, bool reversed);
    void drawSpeedBoxSprite(float speed);
    void drawAreaBoxSprite(float area);
    void drawDistanceBoxSprite(long distance);
    void drawStatusBarSprite(SystemStateType state);

    // Stare metody (fallback gdy sprites wyłączone)
    void drawPatternBox(PatternType pattern, bool reversed);
    void drawSpeedBox(float speed);
    void drawAreaBox(float area);
    void drawDistanceBox(long distance);
    void drawStatusBar(SystemStateType state);

    void clearArea(int16_t x, int16_t y, int16_t w, int16_t h);

public:
    DisplayManager(TFT_eSPI* tftDisplay);
    ~DisplayManager();  // v1.6.0: Destruktor (zwalnianie sprites)

    void init();
    void initSprites();  // v1.6.0: Inicjalizacja sprites
    void deleteSprites(); // v1.6.0: Zwolnienie sprites

    void showSplashScreen(const char* version);
    void showMainScreen(PatternType pattern, float speed, float area,
                       long distance, SystemStateType state, bool reversed);
    void showWarning(const char* title, const char* message);
    void showError(const char* title, const char* message);
    void showSuccess(const char* message);
    void showMessage(const char* message, uint16_t color);
    void clear();
    void forceRedraw();

    // v1.6.0: Zarządzanie sprites
    void enableSprites(bool enable) { spritesEnabled = enable; }
    bool areSpritesEnabled() { return spritesEnabled; }

    TFT_eSPI* getTFT() { return tft; }
};

#endif // DISPLAY_MANAGER_H
