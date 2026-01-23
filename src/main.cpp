/**
 * System Sterowania Malowaniem Pasów Drogowych
 * Wersja: 1.0.0
 *
 * Główny plik programu dla ESP32-S3
 * Sterowanie 6 pistoletami malarskimi z obsługą 13 wzorców malowania
 *
 * Copyright (c) 2026 MT220126 Engineering Team
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "config.h"
#include "patterns.h"
#include "display_manager.h"
#include "encoder_handler.h"
#include "relay_controller.h"
#include "menu_system.h"
#include "calibration.h"

// Wersja oprogramowania
const char* SOFTWARE_VERSION = "1.0.0";
const char* BUILD_DATE = __DATE__;
const char* BUILD_TIME = __TIME__;

// Obiekty globalne
TFT_eSPI tft = TFT_eSPI();
DisplayManager display(&tft);
EncoderHandler encoder(ENCODER_CLK_PIN, ENCODER_DT_PIN, ENCODER_SW_PIN);
RelayController relays;
MenuSystem menu(&display, &encoder);
CalibrationManager calibration(&encoder);

// Zmienne stanu systemu
SystemState systemState;
volatile bool interruptFlag = false;

// Przerwanie od enkodera
void IRAM_ATTR encoderISR() {
    interruptFlag = true;
}

/**
 * Inicjalizacja wszystkich przycisków wzorców
 */
void initPatternButtons() {
    pinMode(BTN_P1A_PIN, INPUT_PULLUP);
    pinMode(BTN_P1B_PIN, INPUT_PULLUP);
    pinMode(BTN_P1C_PIN, INPUT_PULLUP);
    pinMode(BTN_P1D_PIN, INPUT_PULLUP);
    pinMode(BTN_P1E_PIN, INPUT_PULLUP);
    pinMode(BTN_P2A_PIN, INPUT_PULLUP);
    pinMode(BTN_P2B_PIN, INPUT_PULLUP);
    pinMode(BTN_P3A_PIN, INPUT_PULLUP);
    pinMode(BTN_P3B_PIN, INPUT_PULLUP);
    pinMode(BTN_P4_PIN, INPUT_PULLUP);
    pinMode(BTN_P6_PIN, INPUT_PULLUP);
    pinMode(BTN_P7A_PIN, INPUT_PULLUP);
    pinMode(BTN_P7B_PIN, INPUT_PULLUP);
    pinMode(BTN_P7C_PIN, INPUT_PULLUP);
    pinMode(BTN_P7D_PIN, INPUT_PULLUP);
    pinMode(BTN_START_PIN, INPUT_PULLUP);
    pinMode(BTN_STOP_PIN, INPUT_PULLUP);
    pinMode(BTN_REVERSE_PIN, INPUT_PULLUP);
}

/**
 * Inicjalizacja joysticka
 */
void initJoystick() {
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);
    pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);
}

/**
 * Sprawdzanie wciśniętych przycisków wzorców
 */
void checkPatternButtons() {
    if (digitalRead(BTN_P1A_PIN) == LOW && systemState.currentPattern != PATTERN_P1A) {
        systemState.currentPattern = PATTERN_P1A;
        systemState.patternChanged = true;
        delay(200); // Debounce
    }
    if (digitalRead(BTN_P1B_PIN) == LOW && systemState.currentPattern != PATTERN_P1B) {
        systemState.currentPattern = PATTERN_P1B;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P1C_PIN) == LOW && systemState.currentPattern != PATTERN_P1C) {
        systemState.currentPattern = PATTERN_P1C;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P1D_PIN) == LOW && systemState.currentPattern != PATTERN_P1D) {
        systemState.currentPattern = PATTERN_P1D;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P1E_PIN) == LOW && systemState.currentPattern != PATTERN_P1E) {
        systemState.currentPattern = PATTERN_P1E;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P2A_PIN) == LOW && systemState.currentPattern != PATTERN_P2A) {
        systemState.currentPattern = PATTERN_P2A;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P2B_PIN) == LOW && systemState.currentPattern != PATTERN_P2B) {
        systemState.currentPattern = PATTERN_P2B;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P3A_PIN) == LOW && systemState.currentPattern != PATTERN_P3A) {
        systemState.currentPattern = PATTERN_P3A;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P3B_PIN) == LOW && systemState.currentPattern != PATTERN_P3B) {
        systemState.currentPattern = PATTERN_P3B;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P4_PIN) == LOW && systemState.currentPattern != PATTERN_P4) {
        systemState.currentPattern = PATTERN_P4;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P6_PIN) == LOW && systemState.currentPattern != PATTERN_P6) {
        systemState.currentPattern = PATTERN_P6;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P7A_PIN) == LOW && systemState.currentPattern != PATTERN_P7A) {
        systemState.currentPattern = PATTERN_P7A;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P7B_PIN) == LOW && systemState.currentPattern != PATTERN_P7B) {
        systemState.currentPattern = PATTERN_P7B;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P7C_PIN) == LOW && systemState.currentPattern != PATTERN_P7C) {
        systemState.currentPattern = PATTERN_P7C;
        systemState.patternChanged = true;
        delay(200);
    }
    if (digitalRead(BTN_P7D_PIN) == LOW && systemState.currentPattern != PATTERN_P7D) {
        systemState.currentPattern = PATTERN_P7D;
        systemState.patternChanged = true;
        delay(200);
    }
}

/**
 * Sprawdzanie przycisku odwracania wzorca
 */
void checkReverseButton() {
    static unsigned long lastPressTime = 0;

    if (digitalRead(BTN_REVERSE_PIN) == LOW &&
        (systemState.currentPattern == PATTERN_P3A || systemState.currentPattern == PATTERN_P3B)) {
        if (millis() - lastPressTime > 200) {
            systemState.patternReversed = !systemState.patternReversed;
            systemState.patternChanged = true;
            lastPressTime = millis();
        }
    }
}

/**
 * Sprawdzanie przycisków Start/Pauza i Stop
 */
void checkControlButtons() {
    static unsigned long stopPressTime = 0;
    static bool stopPressed = false;

    // Przycisk START/PAUZA
    if (digitalRead(BTN_START_PIN) == LOW) {
        delay(50); // Debounce
        if (digitalRead(BTN_START_PIN) == LOW) {
            if (systemState.state == STATE_IDLE) {
                systemState.state = STATE_PAINTING;
                systemState.startTime = millis();
            } else if (systemState.state == STATE_PAINTING) {
                systemState.state = STATE_PAUSED;
            } else if (systemState.state == STATE_PAUSED) {
                systemState.state = STATE_PAINTING;
            }
            while (digitalRead(BTN_START_PIN) == LOW) delay(10);
            delay(50);
        }
    }

    // Przycisk STOP - krótkie i długie przytrzymanie
    if (digitalRead(BTN_STOP_PIN) == LOW) {
        if (!stopPressed) {
            stopPressed = true;
            stopPressTime = millis();
        }

        if (millis() - stopPressTime > 1000 && systemState.state != STATE_MENU) {
            // Długie przytrzymanie - wejście do menu
            systemState.state = STATE_MENU;
            menu.show();
            stopPressed = false;
        }
    } else {
        if (stopPressed && millis() - stopPressTime < 1000) {
            // Krótkie naciśnięcie - stop
            systemState.state = STATE_IDLE;
            relays.stopAll();
            systemState.totalPaintedArea = 0;
            systemState.distance = 0;
        }
        stopPressed = false;
    }
}

/**
 * Obliczanie ilości wymalowanej powierzchni
 */
void calculatePaintedArea() {
    Pattern* currentPattern = getPattern(systemState.currentPattern);
    if (currentPattern) {
        float widthMeters = currentPattern->width / 100.0; // Konwersja cm na metry
        float distanceMeters = systemState.distance / 100.0; // Konwersja cm na metry

        // Dla wzorców przerwanych obliczamy tylko faktycznie malowaną powierzchnię
        if (currentPattern->lineLength > 0) {
            float cycleLength = currentPattern->lineLength + currentPattern->gapLength;
            float paintedLength = distanceMeters * (currentPattern->lineLength / cycleLength);
            systemState.totalPaintedArea = paintedLength * widthMeters;
        } else {
            // Dla linii ciągłych
            systemState.totalPaintedArea = distanceMeters * widthMeters;
        }
    }
}

/**
 * Główna logika malowania wzorców
 */
void processPainting() {
    if (systemState.state != STATE_PAINTING) {
        relays.stopAll();
        return;
    }

    Pattern* pattern = getPattern(systemState.currentPattern);
    if (!pattern) {
        relays.stopAll();
        return;
    }

    // Obliczanie aktualnej pozycji w cyklu wzorca
    float distanceMeters = systemState.distance / 100.0;

    // Dla wzorców ciągłych
    if (pattern->lineLength <= 0) {
        if (pattern->width == 12) {
            // Wąska linia - używamy środkowych pistoletów (2,3,4)
            relays.setRelay(2, true);
            relays.setRelay(3, true);
            relays.setRelay(4, true);
            relays.setRelay(1, false);
            relays.setRelay(5, false);
            relays.setRelay(6, false);
        } else if (pattern->width == 24) {
            // Szeroka linia - wszystkie pistolety
            relays.setRelay(1, true);
            relays.setRelay(2, true);
            relays.setRelay(3, true);
            relays.setRelay(4, true);
            relays.setRelay(5, true);
            relays.setRelay(6, true);
        }
        return;
    }

    // Dla wzorców przerwanych
    float cycleLength = pattern->lineLength + pattern->gapLength;
    float positionInCycle = fmod(distanceMeters, cycleLength);

    bool shouldPaint = (positionInCycle < pattern->lineLength);

    if (shouldPaint) {
        if (pattern->width == 12) {
            relays.setRelay(2, true);
            relays.setRelay(3, true);
            relays.setRelay(4, true);
            relays.setRelay(1, false);
            relays.setRelay(5, false);
            relays.setRelay(6, false);
        } else if (pattern->width == 24) {
            relays.setRelay(1, true);
            relays.setRelay(2, true);
            relays.setRelay(3, true);
            relays.setRelay(4, true);
            relays.setRelay(5, true);
            relays.setRelay(6, true);
        }
    } else {
        relays.stopAll();
    }

    // Specjalna obsługa dla wzorców odwracalnych (P-3a, P-3b, P-4)
    if (systemState.currentPattern == PATTERN_P3A ||
        systemState.currentPattern == PATTERN_P3B) {

        float doubleCycleLength = cycleLength * 2;
        float posInDouble = fmod(distanceMeters, doubleCycleLength);

        if (systemState.patternReversed) {
            // Linia ciągła po prawej
            if (posInDouble < cycleLength) {
                // Pierwsza część - przerywana lewa, ciągła prawa
                if (positionInCycle < pattern->lineLength) {
                    relays.setRelay(1, true);
                    relays.setRelay(2, true);
                    relays.setRelay(3, true);
                } else {
                    relays.setRelay(1, false);
                    relays.setRelay(2, false);
                    relays.setRelay(3, false);
                }
                relays.setRelay(4, true);
                relays.setRelay(5, true);
                relays.setRelay(6, true);
            }
        } else {
            // Linia ciągła po lewej (standardowo)
            if (posInDouble < cycleLength) {
                relays.setRelay(1, true);
                relays.setRelay(2, true);
                relays.setRelay(3, true);
                if (positionInCycle < pattern->lineLength) {
                    relays.setRelay(4, true);
                    relays.setRelay(5, true);
                    relays.setRelay(6, true);
                } else {
                    relays.setRelay(4, false);
                    relays.setRelay(5, false);
                    relays.setRelay(6, false);
                }
            }
        }
    }
}

/**
 * Aktualizacja wyświetlacza
 */
void updateDisplay() {
    if (systemState.state == STATE_MENU) {
        return; // Menu zarządza swoim własnym wyświetlaczem
    }

    display.showMainScreen(
        systemState.currentPattern,
        systemState.speed,
        systemState.totalPaintedArea,
        systemState.distance,
        systemState.state,
        systemState.patternReversed
    );
}

/**
 * Setup - inicjalizacja systemu
 */
void setup() {
    Serial.begin(115200);
    Serial.println("\n\n=================================");
    Serial.println("System Malowania Pasów Drogowych");
    Serial.printf("Wersja: %s\n", SOFTWARE_VERSION);
    Serial.printf("Build: %s %s\n", BUILD_DATE, BUILD_TIME);
    Serial.println("=================================\n");

    // Inicjalizacja wyświetlacza
    Serial.println("Inicjalizacja wyświetlacza...");
    tft.init();
    tft.setRotation(1); // Landscape
    display.init();
    display.showSplashScreen(SOFTWARE_VERSION);
    delay(2000);

    // Inicjalizacja enkodera
    Serial.println("Inicjalizacja enkodera...");
    encoder.init();
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR, CHANGE);

    // Inicjalizacja przekaźników
    Serial.println("Inicjalizacja przekaźników...");
    relays.init();
    relays.testSequence(); // Test wszystkich przekaźników

    // Inicjalizacja przycisków
    Serial.println("Inicjalizacja przycisków...");
    initPatternButtons();
    initJoystick();

    // Inicjalizacja systemu menu
    Serial.println("Inicjalizacja menu...");
    menu.init();

    // Inicjalizacja kalibracji
    Serial.println("Inicjalizacja kalibracji...");
    calibration.init();

    // Sprawdzenie czy jest kalibracja
    if (!calibration.isCalibrated()) {
        Serial.println("UWAGA: System wymaga kalibracji!");
        display.showWarning("WYMAGANA KALIBRACJA!", "Przytrzymaj STOP aby wejsc do menu");
        delay(3000);
    }

    // Inicjalizacja stanu systemu
    systemState.state = STATE_IDLE;
    systemState.currentPattern = PATTERN_P1A;
    systemState.patternReversed = false;
    systemState.speed = 0;
    systemState.distance = 0;
    systemState.totalPaintedArea = 0;
    systemState.patternChanged = false;
    systemState.startTime = 0;

    Serial.println("System gotowy do pracy!\n");

    // Pokazanie głównego ekranu
    display.showMainScreen(
        systemState.currentPattern,
        systemState.speed,
        systemState.totalPaintedArea,
        systemState.distance,
        systemState.state,
        systemState.patternReversed
    );
}

/**
 * Loop - główna pętla programu
 */
void loop() {
    static unsigned long lastUpdate = 0;
    static unsigned long lastEncoderUpdate = 0;
    unsigned long currentTime = millis();

    // Obsługa enkodera (co 10ms)
    if (currentTime - lastEncoderUpdate >= 10) {
        encoder.update();

        if (systemState.state == STATE_PAINTING || systemState.state == STATE_MEASURING) {
            long newDistance = encoder.getDistance();
            if (newDistance != systemState.distance) {
                systemState.distance = newDistance;

                // Obliczanie prędkości (km/h)
                float timeDiff = (currentTime - lastEncoderUpdate) / 1000.0; // sekundy
                float distanceDiff = abs(newDistance - systemState.distance) / 100.0; // metry
                if (timeDiff > 0) {
                    systemState.speed = (distanceDiff / timeDiff) * 3.6; // m/s -> km/h
                }
            }
        }

        lastEncoderUpdate = currentTime;
    }

    // Sprawdzanie przycisków
    checkPatternButtons();
    checkReverseButton();
    checkControlButtons();

    // Obsługa menu
    if (systemState.state == STATE_MENU) {
        MenuResult result = menu.update();
        if (result == MENU_EXIT) {
            systemState.state = STATE_IDLE;
            updateDisplay();
        } else if (result == MENU_CALIBRATION_START) {
            systemState.state = STATE_CALIBRATING;
        } else if (result == MENU_MEASURE_START) {
            systemState.state = STATE_MEASURING;
            systemState.distance = 0;
            encoder.resetDistance();
        }
    }

    // Obsługa kalibracji
    if (systemState.state == STATE_CALIBRATING) {
        CalibrationResult result = calibration.process();
        if (result == CALIBRATION_COMPLETE) {
            systemState.state = STATE_IDLE;
            display.showSuccess("Kalibracja zakonczona!");
            delay(2000);
            updateDisplay();
        } else if (result == CALIBRATION_CANCELLED) {
            systemState.state = STATE_IDLE;
            updateDisplay();
        }
    }

    // Obliczanie powierzchni
    if (systemState.state == STATE_PAINTING) {
        calculatePaintedArea();
    }

    // Główna logika malowania
    processPainting();

    // Aktualizacja wyświetlacza (co 100ms)
    if (currentTime - lastUpdate >= 100) {
        updateDisplay();
        lastUpdate = currentTime;
    }

    // Obsługa zmiennej wzorca
    if (systemState.patternChanged) {
        Serial.printf("Zmiana wzorca na: %d\n", systemState.currentPattern);
        systemState.patternChanged = false;
        updateDisplay();
    }

    // Małe opóźnienie aby nie obciążać procesora
    delay(1);
}
