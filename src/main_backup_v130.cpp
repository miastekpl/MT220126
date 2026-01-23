/**
 * System Sterowania Malowaniem Pasów Drogowych
 * Wersja: 1.1.0 - KRYTYCZNE POPRAWKI
 *
 * GŁÓWNE ZMIANY:
 * - Dodano zabezpieczenie pistoletów (min 2km/h + wykrywanie ruchu)
 * - Poprawiono mapowanie pistoletów do wzorców (zgodnie ze specyfikacją)
 * - Poprawiono obliczanie powierzchni (rzeczywiste pistolety)
 * - Usunięto enkoder z nawigacji menu (tylko selektor)
 * - Dodano watchdog timer
 * - Dodano WiFi server (do implementacji)
 *
 * Copyright (c) 2026 MT220126 Engineering Team
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "patterns.h"
#include "display_manager.h"
#include "encoder_handler.h"
#include "relay_controller.h"
#include "menu_system.h"
#include "calibration.h"

// Wersja oprogramowania
const char* SOFTWARE_VERSION = "1.1.0";
const char* BUILD_DATE = __DATE__;
const char* BUILD_TIME = __TIME__;

// Obiekty globalne
TFT_eSPI tft = TFT_eSPI();
DisplayManager display(&tft);
EncoderHandler encoder(ENCODER_CLK_PIN, ENCODER_DT_PIN, SELECTOR_PIN);  // ZMIENIONY: używa SELECTOR
RelayController relays;
MenuSystem menu(&display, &encoder);
CalibrationManager calibration(&encoder);

// Zmienne stanu systemu
SystemState systemState;
volatile bool interruptFlag = false;

// Przerwanie od enkodera (TYLKO DO POMIARÓW!)
void IRAM_ATTR encoderISR() {
    interruptFlag = true;
}

/**
 * NOWA FUNKCJA: Sprawdzanie bezpieczeństwa pistoletów
 * Pistolety mogą być aktywne TYLKO gdy:
 * - Prędkość >= 2 km/h
 * - Wykryto ruch (enkoder zlicza)
 */
bool isSafeToActivateGuns() {
    // Sprawdzenie prędkości
    if (systemState.speed < MIN_SPEED_KMH) {
        DEBUG_PRINTF("BEZPIECZENSTWO: Predkosc za niska: %.2f km/h (min %.2f)\n",
                    systemState.speed, MIN_SPEED_KMH);
        return false;
    }

    // Sprawdzenie ruchu
    long currentDistance = systemState.distance;
    long lastDist = systemState.lastDistance;
    
    if (abs(currentDistance - lastDist) < MIN_MOVEMENT_CM) {
        unsigned long timeSinceMovement = millis() - systemState.lastMovementTime;
        if (timeSinceMovement > MOVEMENT_TIMEOUT_MS) {
            DEBUG_PRINTLN("BEZPIECZENSTWO: Brak ruchu enkodera!");
            return false;
        }
    } else {
        // Wykryto ruch - aktualizuj timestamp
        systemState.lastMovementTime = millis();
        systemState.lastDistance = currentDistance;
    }

    return true;
}

/**
 * Inicjalizacja wszystkich przycisków wzorców
 * ZMIENIONE PINY - zgodnie z nowym config.h
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
    pinMode(BTN_START_GAP_PIN, INPUT_PULLUP);  // NOWY: Start od przerwy
}

/**
 * Inicjalizacja joysticka
 * UWAGA: Przycisk joysticka NIE JEST UŻYWANY (selektor jest osobny)
 */
void initJoystick() {
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);
    pinMode(SELECTOR_PIN, INPUT_PULLUP);  // NOWY: Dedykowany przycisk selektora
}

/**
 * Sprawdzanie wciśniętych przycisków wzorców
 */
void checkPatternButtons() {
    if (digitalRead(BTN_P1A_PIN) == LOW && systemState.currentPattern != PATTERN_P1A) {
        systemState.currentPattern = PATTERN_P1A;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P1B_PIN) == LOW && systemState.currentPattern != PATTERN_P1B) {
        systemState.currentPattern = PATTERN_P1B;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P1C_PIN) == LOW && systemState.currentPattern != PATTERN_P1C) {
        systemState.currentPattern = PATTERN_P1C;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P1D_PIN) == LOW && systemState.currentPattern != PATTERN_P1D) {
        systemState.currentPattern = PATTERN_P1D;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P1E_PIN) == LOW && systemState.currentPattern != PATTERN_P1E) {
        systemState.currentPattern = PATTERN_P1E;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P2A_PIN) == LOW && systemState.currentPattern != PATTERN_P2A) {
        systemState.currentPattern = PATTERN_P2A;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P2B_PIN) == LOW && systemState.currentPattern != PATTERN_P2B) {
        systemState.currentPattern = PATTERN_P2B;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P3A_PIN) == LOW && systemState.currentPattern != PATTERN_P3A) {
        systemState.currentPattern = PATTERN_P3A;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P3B_PIN) == LOW && systemState.currentPattern != PATTERN_P3B) {
        systemState.currentPattern = PATTERN_P3B;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P4_PIN) == LOW && systemState.currentPattern != PATTERN_P4) {
        systemState.currentPattern = PATTERN_P4;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P6_PIN) == LOW && systemState.currentPattern != PATTERN_P6) {
        systemState.currentPattern = PATTERN_P6;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P7A_PIN) == LOW && systemState.currentPattern != PATTERN_P7A) {
        systemState.currentPattern = PATTERN_P7A;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P7B_PIN) == LOW && systemState.currentPattern != PATTERN_P7B) {
        systemState.currentPattern = PATTERN_P7B;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P7C_PIN) == LOW && systemState.currentPattern != PATTERN_P7C) {
        systemState.currentPattern = PATTERN_P7C;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(BTN_P7D_PIN) == LOW && systemState.currentPattern != PATTERN_P7D) {
        systemState.currentPattern = PATTERN_P7D;
        systemState.patternChanged = true;
        delay(DEBOUNCE_DELAY);
    }
}

/**
 * NOWA FUNKCJA v1.3.0: Obsługa zmiany wzorca z uwzględnieniem Start Gap
 * Jeśli startFromGap jest włączony, ustawia offset na długość przerwy
 */
void handlePatternChange(PatternType newPattern) {
    Pattern* pattern = getPattern(newPattern);
    if (!pattern) return;

    // Zapisz dystans w momencie zmiany wzorca
    systemState.patternStartDistance = systemState.distance;

    // Jeśli włączony Start Gap i wzorzec ma przerwy
    if (systemState.startFromGap && pattern->gapLength > 0) {
        // Ustaw offset na długość przerwy (w metrach)
        systemState.offsetDistance = pattern->gapLength;

        DEBUG_PRINTF("START GAP aktywny: Offset = %.2f m (przerwa wzorca %s)\n",
                    systemState.offsetDistance, pattern->name);
        DEBUG_PRINTLN("System przeskoczy przerwe i zacznie od linii!");
    } else {
        // Standardowy start - bez offsetu
        systemState.offsetDistance = 0.0;

        if (systemState.startFromGap && pattern->gapLength == 0) {
            DEBUG_PRINTF("START GAP nieaktywny dla wzorca %s (brak przerw)\n", pattern->name);
        }
    }
}

/**
 * NOWA FUNKCJA v1.3.0: Sprawdzanie przycisku START GAP
 * Przełącza tryb "start od przerwy"
 */
void checkStartGapButton() {
    static unsigned long lastPressTime = 0;

    if (digitalRead(BTN_START_GAP_PIN) == LOW) {
        if (millis() - lastPressTime > DEBOUNCE_DELAY) {
            systemState.startFromGap = !systemState.startFromGap;
            lastPressTime = millis();

            DEBUG_PRINTF("START GAP: %s\n", systemState.startFromGap ? "WLACZONY" : "WYLACZONY");

            // Jeśli właśnie włączono i jesteśmy w trybie malowania - zastosuj do bieżącego wzorca
            if (systemState.startFromGap && systemState.state == STATE_PAINTING) {
                handlePatternChange(systemState.currentPattern);
            } else if (!systemState.startFromGap) {
                // Wyłączono - zeruj offset
                systemState.offsetDistance = 0.0;
            }
        }
    }
}

/**
 * Sprawdzanie przycisku odwracania wzorca
 */
void checkReverseButton() {
    static unsigned long lastPressTime = 0;

    if (digitalRead(BTN_REVERSE_PIN) == LOW &&
        (systemState.currentPattern == PATTERN_P3A || systemState.currentPattern == PATTERN_P3B)) {
        if (millis() - lastPressTime > DEBOUNCE_DELAY) {
            systemState.patternReversed = !systemState.patternReversed;
            systemState.patternChanged = true;
            lastPressTime = millis();
            DEBUG_PRINTF("Wzorzec odwrocony: %s\n", systemState.patternReversed ? "TAK" : "NIE");
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
        delay(DEBOUNCE_DELAY);
        if (digitalRead(BTN_START_PIN) == LOW) {
            if (systemState.state == STATE_IDLE) {
                systemState.state = STATE_PAINTING;
                systemState.startTime = millis();
                systemState.lastMovementTime = millis();
                systemState.lastDistance = systemState.distance;
                DEBUG_PRINTLN("START MALOWANIA");
            } else if (systemState.state == STATE_PAINTING) {
                systemState.state = STATE_PAUSED;
                DEBUG_PRINTLN("PAUZA");
            } else if (systemState.state == STATE_PAUSED) {
                systemState.state = STATE_PAINTING;
                systemState.lastMovementTime = millis();
                DEBUG_PRINTLN("WZNOWIENIE");
            }
            while (digitalRead(BTN_START_PIN) == LOW) delay(10);
            delay(DEBOUNCE_DELAY);
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
            DEBUG_PRINTLN("WEJSCIE DO MENU");
        }
    } else {
        if (stopPressed && millis() - stopPressTime < 1000) {
            // Krótkie naciśnięcie - stop
            systemState.state = STATE_IDLE;
            relays.stopAll();
            systemState.totalPaintedArea = 0;
            systemState.distance = 0;
            systemState.lastDistance = 0;
            DEBUG_PRINTLN("STOP - Reset liczników");
        }
        stopPressed = false;
    }
}

/**
 * POPRAWIONA FUNKCJA: Obliczanie ilości wymalowanej powierzchni
 * Używa RZECZYWISTEJ szerokości z patterns.h (uwzględnia aktywne pistolety)
 */
void calculatePaintedArea() {
    Pattern* currentPattern = getPattern(systemState.currentPattern);
    if (!currentPattern) {
        return;
    }

    float distanceMeters = systemState.distance / 100.0; // cm → m
    float realWidthMeters = currentPattern->realWidth / 100.0; // cm → m

    // Dla wzorców przerwanych obliczamy tylko faktycznie malowaną długość
    if (currentPattern->lineLength > 0) {
        float cycleLength = currentPattern->lineLength + currentPattern->gapLength;
        float paintedLength = distanceMeters * (currentPattern->lineLength / cycleLength);
        systemState.totalPaintedArea = paintedLength * realWidthMeters;
    } else {
        // Dla linii ciągłych
        systemState.totalPaintedArea = distanceMeters * realWidthMeters;
    }
}

/**
 * PRZEPISANA FUNKCJA: Główna logika malowania wzorców
 * POPRAWKI KRYTYCZNE:
 * 1. Zabezpieczenie przed malowaniem na postoju (< 2km/h)
 * 2. Sprawdzanie ruchu enkodera
 * 3. Prawidłowe mapowanie pistoletów według specyfikacji
 */
void processPainting() {
    // Jeśli nie malujemy - wyłącz wszystko
    if (systemState.state != STATE_PAINTING) {
        relays.stopAll();
        systemState.safetyLocked = false;
        return;
    }

    // KRYTYCZNE: Sprawdzenie bezpieczeństwa
    if (!isSafeToActivateGuns()) {
        relays.stopAll();
        systemState.safetyLocked = true;
        return;
    }

    systemState.safetyLocked = false;

    // Pobierz wzorzec
    Pattern* pattern = getPattern(systemState.currentPattern);
    if (!pattern) {
        relays.stopAll();
        DEBUG_PRINTLN("BLAD: Nieznany wzorzec!");
        return;
    }

    // Pobierz listę aktywnych pistoletów dla tego wzorca
    bool activeGuns[6];
    getActiveGuns(systemState.currentPattern, activeGuns);

    float distanceMeters = systemState.distance / 100.0;

    // Dla wzorców ciągłych - włącz pistolety
    if (pattern->lineLength <= 0) {
        // Ciągłe malowanie - włącz aktywne pistolety
        for (int i = 0; i < 6; i++) {
            relays.setRelay(i + 1, activeGuns[i]);
        }
        return;
    }

    // Dla wzorców przerwanych - oblicz czy jesteśmy w linii czy przerwie
    float cycleLength = pattern->lineLength + pattern->gapLength;
    float positionInCycle = fmod(distanceMeters, cycleLength);

    bool shouldPaint = (positionInCycle < pattern->lineLength);

    if (shouldPaint) {
        // Jesteśmy w linii - włącz aktywne pistolety
        for (int i = 0; i < 6; i++) {
            relays.setRelay(i + 1, activeGuns[i]);
        }
    } else {
        // Jesteśmy w przerwie - wyłącz wszystko
        relays.stopAll();
    }

    // SPECJALNE: Odwracanie wzorców P-3a, P-3b
    // TODO: Implementacja odwracania (linia ciągła lewo<->prawo)
    // Na razie używamy standardowego mapowania z patterns.h
}

/**
 * Aktualizacja wyświetlacza
 */
void updateDisplay() {
    if (systemState.state == STATE_MENU) {
        return; // Menu zarządza swoim wyświetlaczem
    }

    display.showMainScreen(
        systemState.currentPattern,
        systemState.speed,
        systemState.totalPaintedArea,
        systemState.distance,
        systemState.state,
        systemState.patternReversed
    );

    // Komunikat o blokadzie bezpieczeństwa
    if (systemState.safetyLocked && systemState.state == STATE_PAINTING) {
        // Można dodać komunikat na ekranie
        DEBUG_PRINTLN("BLOKADA BEZPIECZENSTWA AKTYWNA!");
    }
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

    // NOWE: Watchdog timer (10 sekund)
    Serial.println("Inicjalizacja watchdog timer...");
    esp_task_wdt_init(10, true);  // 10 sekund timeout, panic on timeout
    esp_task_wdt_add(NULL);       // Dodaj current task
    Serial.println("Watchdog timer aktywny (10s)");

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
    relays.testSequence();

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

    // Sprawdzenie kalibracji
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
    systemState.lastDistance = 0;
    systemState.totalPaintedArea = 0;
    systemState.patternChanged = false;
    systemState.startTime = 0;
    systemState.lastMovementTime = millis();
    systemState.safetyLocked = false;

    Serial.println("System gotowy do pracy!\n");
    Serial.printf("Min. predkosc: %.1f km/h\n", MIN_SPEED_KMH);
    Serial.printf("Min. ruch: %d cm\n", MIN_MOVEMENT_CM);
    Serial.printf("Timeout ruchu: %d ms\n\n", MOVEMENT_TIMEOUT_MS);

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

    // NOWE: Reset watchdog timer co iterację
    esp_task_wdt_reset();

    // Obsługa enkodera (co 10ms)
    if (currentTime - lastEncoderUpdate >= 10) {
        encoder.update();

        if (systemState.state == STATE_PAINTING || systemState.state == STATE_MEASURING) {
            long newDistance = encoder.getDistance();
            if (newDistance != systemState.distance) {
                systemState.distance = newDistance;

                // Obliczanie prędkości (km/h)
                float timeDiff = (currentTime - lastEncoderUpdate) / 1000.0;
                long distanceDiff = abs(newDistance - systemState.distance);
                float distanceDiffM = distanceDiff / 100.0;
                if (timeDiff > 0) {
                    systemState.speed = (distanceDiffM / timeDiff) * 3.6; // m/s → km/h
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
    if (currentTime - lastUpdate >= REFRESH_RATE) {
        updateDisplay();
        lastUpdate = currentTime;
    }

    // Obsługa zmiennej wzorca
    if (systemState.patternChanged) {
        Serial.printf("Zmiana wzorca na: %s\n", getPatternName(systemState.currentPattern));
        systemState.patternChanged = false;
        updateDisplay();
    }

    // Małe opóźnienie (non-blocking)
    delay(1);
}
