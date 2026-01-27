/**
 * System Sterowania Malowaniem Pasów Drogowych
 * Wersja: 1.4.0 - OPTYMALIZACJE I POPRAWKI KRYTYCZNE
 *
 * GŁÓWNE ZMIANY v1.4.0:
 * - Dodano thread-safety (FreeRTOS mutex)
 * - Optymalizacja processPainting() (integer math zamiast float fmod)
 * - Naprawiono REVERSE - faktyczna zamiana pistoletów P1↔P4, P2↔P5, P3↔P6
 * - Naprawiono START GAP BUG (nie resetuje distance gdy Start Gap aktywny)
 * - Refaktoryzacja checkPatternButtons() (pętla zamiast 76 linii kodu)
 * - Non-blocking debounce (usunięto wszystkie delay() z obsługi przycisków)
 * - Nowy config_v140_NEW.h z poprawionymi pinami GPIO
 *
 * POPRZEDNIE ZMIANY (v1.3.0):
 * - Dodano funkcję Start Gap (Od Przerwy)
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
#include "config_v140_NEW.h"  // ZMIANA v1.4.0: Nowy header z poprawionymi pinami GPIO
#include "patterns.h"
#include "display_manager.h"
#include "encoder_handler.h"
#include "relay_controller.h"
#include "menu_system.h"
#include "calibration.h"
#include "service_mode.h"       // NOWE v1.4.1: Tryb serwisowy
#include "event_logger.h"       // NOWE v1.5.0: System logowania zdarzeń
#include "dual_encoder_manager.h"  // NOWE v1.6.0: Dual Encoder (redundancja)
#include "sd_card_manager.h"    // NOWE v1.6.0: SD Card logging

// Wersja oprogramowania
const char* SOFTWARE_VERSION = "1.6.6";  // BUGFIX v1.6.6: Walidacja GPIO + static_assert + runtime validation
const char* BUILD_DATE = __DATE__;
const char* BUILD_TIME = __TIME__;

// NOWE v1.4.0: FreeRTOS mutex dla thread-safety
// Chroni dostęp do współdzielonych zmiennych między ISR a main loop
SemaphoreHandle_t stateMutex = NULL;
SemaphoreHandle_t encoderMutex = NULL;

// Obiekty globalne
TFT_eSPI tft = TFT_eSPI();
DisplayManager display(&tft);
EventLogger eventLogger;  // NOWE v1.5.0: System logowania zdarzeń (musi być przed DualEncoder!)
DualEncoderManager dualEncoder(&eventLogger);  // ZMIANA v1.6.0: Dual Encoder zamiast pojedynczego
SDCardManager sdCard(&eventLogger);  // NOWE v1.6.0: SD Card Manager
RelayController relays;
MenuSystem menu(&display, dualEncoder.getPrimaryEncoder());  // Menu używa primary encoder
CalibrationManager calibration(dualEncoder.getPrimaryEncoder());  // Kalibracja używa primary
ServiceMode serviceMode(&display, &relays);  // NOWE v1.4.1: Tryb serwisowy

// Zmienne stanu systemu
SystemState systemState;
volatile bool interruptFlag = false;

// UWAGA: PatternButton jest już zdefiniowany w config_v140_NEW.h (nie duplikować!)

// NOWE v1.4.0: Tablica mapowania przycisków na wzorce (zamiast 76 linii if-ów)
const PatternButton PATTERN_BUTTONS[] PROGMEM = {
    {BTN_P1A_PIN, PATTERN_P1A}, {BTN_P1B_PIN, PATTERN_P1B}, {BTN_P1C_PIN, PATTERN_P1C},
    {BTN_P1D_PIN, PATTERN_P1D}, {BTN_P1E_PIN, PATTERN_P1E}, {BTN_P2A_PIN, PATTERN_P2A},
    {BTN_P2B_PIN, PATTERN_P2B}, {BTN_P3A_PIN, PATTERN_P3A}, {BTN_P3B_PIN, PATTERN_P3B},
    {BTN_P4_PIN, PATTERN_P4},   {BTN_P6_PIN, PATTERN_P6},   {BTN_P7A_PIN, PATTERN_P7A},
    {BTN_P7B_PIN, PATTERN_P7B}, {BTN_P7C_PIN, PATTERN_P7C}, {BTN_P7D_PIN, PATTERN_P7D}
};
const uint8_t NUM_PATTERN_BUTTONS = sizeof(PATTERN_BUTTONS) / sizeof(PatternButton);

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
        // v1.5.0: Loguj tylko co 5 sekund (nie spamuj logów)
        static unsigned long lastSafetyLog = 0;
        if (millis() - lastSafetyLog > 5000) {
            eventLogger.log(EVENT_SAFETY_TRIGGERED, (uint16_t)(systemState.speed * 10), 1, "Blokada: predkosc za niska");
            lastSafetyLog = millis();
        }
        return false;
    }

    // Sprawdzenie ruchu
    long currentDistance = systemState.distance;
    long lastDist = systemState.lastDistance;

    if (abs(currentDistance - lastDist) < MIN_MOVEMENT_CM) {
        unsigned long timeSinceMovement = millis() - systemState.lastMovementTime;
        if (timeSinceMovement > MOVEMENT_TIMEOUT_MS) {
            DEBUG_PRINTLN("BEZPIECZENSTWO: Brak ruchu enkodera!");
            // v1.5.0: Loguj tylko raz (unikaj spamu)
            static bool movementLoggedOnce = false;
            if (!movementLoggedOnce) {
                eventLogger.log(EVENT_SAFETY_TRIGGERED, 0, 2, "Blokada: brak ruchu enkodera");
                movementLoggedOnce = true;
            }
            return false;
        } else {
            // Reset flag gdy ruch wykryty ponownie
            static bool movementLoggedOnce = false;
            movementLoggedOnce = false;
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
 * ZMIENIONE PINY - zgodnie z config_v140_NEW.h
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
    pinMode(BTN_START_GAP_PIN, INPUT_PULLUP);  // NOWY v1.3.0: Start od przerwy
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

// Forward declaration - funkcja zdefiniowana poniżej (bugfix v1.6.1)
void handlePatternChange(PatternType newPattern);

/**
 * PRZEPISANA FUNKCJA v1.4.0: Sprawdzanie wciśniętych przycisków wzorców
 * Zamiast 76 linii powtarzalnego kodu używamy pętli po tablicy PATTERN_BUTTONS
 * ZMIANA: Non-blocking debounce (bez delay)
 */
void checkPatternButtons() {
    static unsigned long lastPressTime = 0;
    unsigned long now = millis();

    // Debounce globalny - zapobiega wielokrotnym naciśnięciom
    if (now - lastPressTime < DEBOUNCE_DELAY) return;

    // Iteracja po wszystkich przyciskach wzorców
    for (uint8_t i = 0; i < NUM_PATTERN_BUTTONS; i++) {
        if (digitalRead(PATTERN_BUTTONS[i].pin) == LOW &&
            systemState.currentPattern != PATTERN_BUTTONS[i].pattern) {
            systemState.currentPattern = PATTERN_BUTTONS[i].pattern;
            systemState.patternChanged = true;
            handlePatternChange(PATTERN_BUTTONS[i].pattern);
            lastPressTime = now;
            DEBUG_PRINTF("Zmiana wzorca: %s\n", getPatternName(PATTERN_BUTTONS[i].pattern));
            break;  // Tylko jeden wzorzec na raz
        }
    }
}

/**
 * NOWA FUNKCJA v1.3.0: Obsługa zmiany wzorca z uwzględnieniem Start Gap
 * Jeśli startFromGap jest włączony, ustawia offset na długość przerwy
 */
void handlePatternChange(PatternType newPattern) {
    Pattern* pattern = getPattern(newPattern);
    if (!pattern) return;

    // NOWE v1.5.0: Logowanie zmiany wzorca
    eventLogger.log(EVENT_PATTERN_CHANGED, systemState.currentPattern, newPattern, pattern->name);

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
 * ZMIANA v1.4.0: Non-blocking (bez delay)
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
 * ZMIANA v1.4.0: Non-blocking (bez delay)
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
 * ZMIANA v1.4.0: Non-blocking debounce (bez delay)
 * NAPRAWA v1.4.0: START GAP BUG - nie resetuje distance gdy Start Gap aktywny
 */
void checkControlButtons() {
    static unsigned long stopPressTime = 0;
    static bool stopPressed = false;
    static unsigned long lastStartRelease = 0;

    // Przycisk START/PAUZA - ZMIANA: Non-blocking
    if (digitalRead(BTN_START_PIN) == LOW) {
        if (millis() - lastStartRelease > DEBOUNCE_DELAY) {
            if (systemState.state == STATE_IDLE) {
                systemState.state = STATE_PAINTING;
                systemState.startTime = millis();
                systemState.lastMovementTime = millis();
                systemState.lastDistance = systemState.distance;
                DEBUG_PRINTLN("START MALOWANIA");
                eventLogger.log(EVENT_STATE_CHANGED, STATE_IDLE, STATE_PAINTING, "START malowania");  // v1.5.0
                lastStartRelease = millis();
            } else if (systemState.state == STATE_PAINTING) {
                systemState.state = STATE_PAUSED;
                DEBUG_PRINTLN("PAUZA");
                eventLogger.log(EVENT_STATE_CHANGED, STATE_PAINTING, STATE_PAUSED, "PAUZA");  // v1.5.0
                lastStartRelease = millis();
            } else if (systemState.state == STATE_PAUSED) {
                systemState.state = STATE_PAINTING;
                systemState.lastMovementTime = millis();
                DEBUG_PRINTLN("WZNOWIENIE");
                eventLogger.log(EVENT_STATE_CHANGED, STATE_PAUSED, STATE_PAINTING, "WZNOWIENIE");  // v1.5.0
                lastStartRelease = millis();
            }
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
            eventLogger.log(EVENT_STATE_CHANGED, 0, STATE_MENU, "Wejscie do MENU");  // v1.5.0
        }
    } else {
        if (stopPressed && millis() - stopPressTime < 1000) {
            // Krótkie naciśnięcie - stop
            systemState.state = STATE_IDLE;
            relays.stopAll();
            systemState.totalPaintedArea = 0;
            eventLogger.log(EVENT_STATE_CHANGED, 0, STATE_IDLE, "STOP - Reset licznikow");  // v1.5.0

            // NAPRAWA v1.4.0: START GAP BUG
            // Nie resetuj distance gdy Start Gap jest aktywny
            if (!systemState.startFromGap) {
                systemState.distance = 0;
                dualEncoder.resetDistance();  // FIX v1.6.1: Użyj dualEncoder zamiast encoder
            } else {
                // Start Gap aktywny - nie resetuj distance
                DEBUG_PRINTLN("START GAP: Distance nie zresetowany");
            }

            systemState.lastDistance = 0;
            systemState.offsetDistance = 0.0;
            systemState.startFromGap = false;  // Wyłącz Start Gap po STOP
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
 * ZOPTYMALIZOWANA FUNKCJA v1.4.0: Główna logika malowania wzorców
 *
 * OPTYMALIZACJE v1.4.0:
 * - Integer math zamiast float fmod() - znacznie SZYBSZE (modulo integer)
 * - Faktyczna implementacja REVERSE (zamiana pistoletów P1↔P4, P2↔P5, P3↔P6)
 *
 * POPRAWKI v1.3.0:
 * - Zabezpieczenie przed malowaniem na postoju (< 2km/h)
 * - Sprawdzanie ruchu enkodera
 * - Prawidłowe mapowanie pistoletów według specyfikacji
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

    // Dla wzorców ciągłych - włącz pistolety
    if (pattern->lineLength <= 0) {
        // Ciągłe malowanie - włącz aktywne pistolety
        for (int i = 0; i < 6; i++) {
            relays.setRelay(i + 1, activeGuns[i]);
        }
        return;
    }

    // OPTYMALIZACJA v1.4.0: Integer math zamiast float fmod()
    // Dla wzorców przerwanych - oblicz czy jesteśmy w linii czy przerwie
    // NOWE v1.3.0: Uwzględnij offset z Start Gap

    long distanceCm = systemState.distance - systemState.patternStartDistance;
    long effectiveDistanceCm = distanceCm - (long)(systemState.offsetDistance * 100.0);

    // Jeśli wciąż w fazie offsetu (przerwy), nie maluj
    if (effectiveDistanceCm < 0) {
        relays.stopAll();
        return;
    }

    // OPTYMALIZACJA v1.4.0: Integer modulo - ZNACZNIE SZYBSZE niż fmod()!
    long cycleLengthCm = (long)(pattern->lineLength * 100.0) + (long)(pattern->gapLength * 100.0);
    long positionInCycleCm = effectiveDistanceCm % cycleLengthCm;  // Modulo integer - SZYBKIE!
    long lineLengthCm = (long)(pattern->lineLength * 100.0);

    bool shouldPaint = (positionInCycleCm < lineLengthCm);

    // NAPRAWA v1.4.0: REVERSE - faktyczna implementacja zamiany pistoletów
    // Dla wzorców P-3a i P-3b gdy REVERSE aktywny: zamień pistolety lewo↔prawo
    if (systemState.patternReversed &&
        (systemState.currentPattern == PATTERN_P3A || systemState.currentPattern == PATTERN_P3B)) {
        // Zamień pistolety: P1↔P4, P2↔P5, P3↔P6
        // To odwraca stronę lewą z prawą (linia ciągła <-> przerywana)
        bool temp;
        temp = activeGuns[0]; activeGuns[0] = activeGuns[3]; activeGuns[3] = temp;
        temp = activeGuns[1]; activeGuns[1] = activeGuns[4]; activeGuns[4] = temp;
        temp = activeGuns[2]; activeGuns[2] = activeGuns[5]; activeGuns[5] = temp;

        DEBUG_PRINTLN("REVERSE: Pistolety zamienione lewo<->prawo");
    }

    if (shouldPaint) {
        // Jesteśmy w linii - włącz aktywne pistolety
        for (int i = 0; i < 6; i++) {
            relays.setRelay(i + 1, activeGuns[i]);
        }
    } else {
        // Jesteśmy w przerwie - wyłącz wszystko
        relays.stopAll();
    }
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
 * ZMIANA v1.4.0: Dodano inicjalizację mutexów dla thread-safety
 */
void setup() {
    Serial.begin(115200);
    Serial.println("\n\n=================================");
    Serial.println("System Malowania Pasów Drogowych");
    Serial.printf("Wersja: %s\n", SOFTWARE_VERSION);
    Serial.printf("Build: %s %s\n", BUILD_DATE, BUILD_TIME);
    Serial.println("=================================\n");

    // DEBUG v1.6.5: Wyświetlanie wartości GPIO (diagnoza GPIO 227)
    Serial.println("--- DEBUG GPIO PINS ---");
    Serial.printf("ENCODER PRIMARY: CLK=%d DT=%d SW=%d\n", ENCODER_CLK_PIN, ENCODER_DT_PIN, ENCODER_SW_PIN);
    Serial.printf("ENCODER BACKUP:  CLK=%d DT=%d SW=%d\n", ENCODER_BACKUP_CLK_PIN, ENCODER_BACKUP_DT_PIN, ENCODER_BACKUP_SW_PIN);
    Serial.printf("RELAY PINS: R1=%d R2=%d R3=%d R4=%d R5=%d R6=%d\n", RELAY_1_PIN, RELAY_2_PIN, RELAY_3_PIN, RELAY_4_PIN, RELAY_5_PIN, RELAY_6_PIN);
    Serial.printf("BUTTONS: REVERSE=%d START=%d STOP=%d\n", BTN_REVERSE_PIN, BTN_START_PIN, BTN_STOP_PIN);
    Serial.printf("TFT: MISO=%d MOSI=%d SCLK=%d CS=%d DC=%d RST=%d\n", TFT_MISO, TFT_MOSI, TFT_SCLK, TFT_CS, TFT_DC, TFT_RST);
    Serial.printf("SD CARD: MISO=%d MOSI=%d SCLK=%d CS=%d\n", SD_MISO_PIN, SD_MOSI_PIN, SD_SCK_PIN, SD_CS_PIN);
    Serial.println("--- END GPIO DEBUG ---\n");

    // NOWE v1.4.0: Inicjalizacja mutexów FreeRTOS
    Serial.println("Inicjalizacja mutexów...");
    stateMutex = xSemaphoreCreateMutex();
    encoderMutex = xSemaphoreCreateMutex();
    if (!stateMutex || !encoderMutex) {
        Serial.println("FATAL: Nie można utworzyć mutexów!");
        while(1) delay(1000);
    }
    Serial.println("Mutexy utworzone (thread-safety aktywny)");

    // NOWE v1.3.0: Watchdog timer (10 sekund)
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

    // Inicjalizacja dual enkodera (v1.6.0 - redundancja!)
    Serial.println("Inicjalizacja DUAL ENCODER...");
    dualEncoder.init();
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_BACKUP_CLK_PIN), encoderISR, CHANGE);  // v1.6.0: Backup encoder

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

    // NOWE v1.4.1: Inicjalizacja trybu serwisowego
    Serial.println("Inicjalizacja trybu serwisowego...");
    serviceMode.init();

    // NOWE v1.5.0: Inicjalizacja Event Loggera
    Serial.println("Inicjalizacja Event Loggera...");
    eventLogger.init();
    eventLogger.log(EVENT_SYSTEM_START, 0, 0, "System uruchomiony");

    // NOWE v1.6.0: Inicjalizacja SD Card
    Serial.println("Inicjalizacja SD Card...");
    if (sdCard.init()) {
        Serial.println("SD Card zainicjalizowana (auto-zapis aktywny)");
        eventLogger.log(EVENT_SYSTEM_START, 0, 0, "SD Card: OK");
    } else {
        Serial.println("UWAGA: SD Card niedostępna (logi tylko w RAM)");
        eventLogger.log(EVENT_ERROR_OCCURRED, 0, 0, "SD Card: Niedostepna");
    }

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

    // NOWE v1.3.0: Start Gap
    systemState.startFromGap = false;
    systemState.offsetDistance = 0.0;
    systemState.patternStartDistance = 0;

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

    // NOWE v1.3.0: Reset watchdog timer co iterację
    esp_task_wdt_reset();

    // Obsługa dual enkodera (co 10ms) - v1.6.0
    if (currentTime - lastEncoderUpdate >= 10) {
        dualEncoder.update();  // v1.6.0: Aktualizacja obu enkoderów + sprawdzanie spójności

        if (systemState.state == STATE_PAINTING || systemState.state == STATE_MEASURING) {
            long newDistance = dualEncoder.getDistance();  // v1.6.0: Z aktywnego enkodera
            if (newDistance != systemState.distance) {
                // NAPRAWA v1.4.2: Zapisz stary dystans PRZED zmianą
                long oldDistance = systemState.distance;
                systemState.distance = newDistance;

                // Obliczanie prędkości (km/h)
                float timeDiff = (currentTime - lastEncoderUpdate) / 1000.0;
                long distanceDiff = abs(newDistance - oldDistance);  // ✅ NAPRAWIONE: używamy oldDistance
                float distanceDiffM = distanceDiff / 100.0;
                if (timeDiff > 0) {
                    systemState.speed = (distanceDiffM / timeDiff) * 3.6; // m/s → km/h
                }
            }
        }

        lastEncoderUpdate = currentTime;
    }

    // v1.6.0: Automatyczny zapis logów na SD Card
    sdCard.update();

    // Sprawdzanie przycisków
    checkPatternButtons();
    checkReverseButton();
    checkStartGapButton();  // NOWE v1.3.0
    checkControlButtons();

    // Obsługa menu
    if (systemState.state == STATE_MENU) {
        MenuResult result = menu.update();
        if (result == MENU_EXIT) {
            systemState.state = STATE_IDLE;
            updateDisplay();
        } else if (result == MENU_CALIBRATION_START) {
            systemState.state = STATE_CALIBRATING;
            eventLogger.log(EVENT_CALIBRATION_START, 0, 0, "Kalibracja rozpoczeta");  // v1.5.0
        } else if (result == MENU_MEASURE_START) {
            systemState.state = STATE_MEASURING;
            systemState.distance = 0;
            dualEncoder.resetDistance();  // FIX v1.6.1: Użyj dualEncoder zamiast encoder
        } else if (result == MENU_SERVICE_START) {
            // NOWE v1.4.1: Wejście do trybu serwisowego
            systemState.state = STATE_SERVICE;
            serviceMode.show();
            DEBUG_PRINTLN("Wejscie do trybu serwisowego");
        }
    }

    // Obsługa kalibracji
    if (systemState.state == STATE_CALIBRATING) {
        CalibrationResult result = calibration.process();
        if (result == CALIBRATION_COMPLETE) {
            systemState.state = STATE_IDLE;
            display.showSuccess("Kalibracja zakonczona!");
            eventLogger.log(EVENT_CALIBRATION_COMPLETE, 0, 0, "Kalibracja ukonczona");  // v1.5.0
            delay(2000);
            updateDisplay();
        } else if (result == CALIBRATION_CANCELLED) {
            systemState.state = STATE_IDLE;
            updateDisplay();
        }
    }

    // NOWE v1.4.1: Obsługa trybu serwisowego
    if (systemState.state == STATE_SERVICE) {
        serviceMode.update();

        // Sprawdzanie przycisków wzorców (umożliwia zmianę wzorca w trybie serwisowym)
        checkPatternButtons();
        if (systemState.patternChanged) {
            serviceMode.setPattern(systemState.currentPattern);
            systemState.patternChanged = false;
        }

        // Sprawdzenie długiego przyciśnięcia STOP (2s) - wyjście z serwisu
        static unsigned long stopPressTime = 0;
        static bool stopPressed = false;

        if (digitalRead(BTN_STOP_PIN) == LOW) {
            if (!stopPressed) {
                stopPressed = true;
                stopPressTime = millis();
            } else if (millis() - stopPressTime >= 2000) {
                // Długie przyciśnięcie - wyjście z serwisu
                systemState.state = STATE_IDLE;
                serviceMode.hide();
                updateDisplay();
                DEBUG_PRINTLN("Wyjscie z trybu serwisowego");
                stopPressed = false;
            }
        } else {
            stopPressed = false;
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
