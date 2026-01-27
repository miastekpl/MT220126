/**
 * System menu z obsługą joysticka
 */

#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include <Arduino.h>
#include "config_v140_NEW.h"  // BUGFIX v1.6.3: Użyj NOWEGO pliku config!
#include "display_manager.h"
#include "encoder_handler.h"

enum MenuItem {
    MENU_ITEM_CALIBRATION = 0,
    MENU_ITEM_MEASURE_DISTANCE,
    MENU_ITEM_RESET_COUNTER,
    MENU_ITEM_SYSTEM_INFO,
    MENU_ITEM_SERVICE,      // NOWE v1.4.1 - tryb serwisowy (czyszczenie pistoletów)
    MENU_ITEM_EXIT,
    MENU_ITEM_COUNT
};

class MenuSystem {
private:
    DisplayManager* display;
    EncoderHandler* encoder;

    bool menuActive;
    uint8_t selectedItem;
    bool inSubMenu;

    unsigned long lastJoystickRead;
    unsigned long lastButtonPress;

    int readJoystickX();
    int readJoystickY();
    bool isJoystickPressed();
    void drawMenu();
    void drawMenuItem(uint8_t index, const char* text, bool selected);
    void handleSelection();

public:
    MenuSystem(DisplayManager* disp, EncoderHandler* enc);

    void init();
    void show();
    void hide();
    MenuResult update();
    bool isActive();
};

#endif // MENU_SYSTEM_H
