/**
 * Menedżer wyświetlacza ILI9341
 * Obsługa wszystkich operacji graficznych
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "config.h"
#include "patterns.h"

class DisplayManager {
private:
    TFT_eSPI* tft;
    uint16_t lastPatternType;
    float lastSpeed;
    float lastArea;
    long lastDistance;
    SystemStateType lastState;
    bool lastReversed;
    bool needsFullRedraw;

    void drawPatternBox(PatternType pattern, bool reversed);
    void drawSpeedBox(float speed);
    void drawAreaBox(float area);
    void drawDistanceBox(long distance);
    void drawStatusBar(SystemStateType state);
    void clearArea(int16_t x, int16_t y, int16_t w, int16_t h);

public:
    DisplayManager(TFT_eSPI* tftDisplay);
    void init();
    void showSplashScreen(const char* version);
    void showMainScreen(PatternType pattern, float speed, float area,
                       long distance, SystemStateType state, bool reversed);
    void showWarning(const char* title, const char* message);
    void showError(const char* title, const char* message);
    void showSuccess(const char* message);
    void showMessage(const char* message, uint16_t color);
    void clear();
    void forceRedraw();
    TFT_eSPI* getTFT() { return tft; }
};

#endif // DISPLAY_MANAGER_H
