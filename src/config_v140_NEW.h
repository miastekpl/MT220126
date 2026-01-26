/**
 * Plik konfiguracyjny systemu malowania pasów drogowych
 * WERSJA 1.4.0 - KOMPLEKSOWA REFAKTORYZACJA
 *
 * KRYTYCZNE ZMIANY:
 * - Naprawione WSZYSTKIE konflikty GPIO (oryginalna dokumentacja miała błędy!)
 * - Dodano FreeRTOS mutex dla thread-safety
 * - Zoptymalizowano stałe (integer math)
 * - Dodano STATE_ERROR dla obsługi błędów
 * - Dodano konfigurację WiFi
 * - Przygotowano do State Machine pattern
 *
 * Copyright (c) 2026 MT220126 Engineering Team
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// ============================================================================
// KONFIGURACJA PINÓW GPIO - POPRAWIONA (BEZ KONFLIKTÓW!)
// ============================================================================
// Schemat został całkowicie przeprojektowany aby wyeliminować wszystkie konflikty
// GPIO z oryginalnej dokumentacji (miała błędy - niektóre piny 3x wykorzystane!)

// Wyświetlacz ILI9341 (SPI Hardware) - NIE ZMIENIAĆ
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   22
#define TFT_RST  21

// Enkoder KY-040 (TYLKO DO POMIARÓW - NIE DO MENU!)
#define ENCODER_CLK_PIN 32  // Z przerwaniem (INT)
#define ENCODER_DT_PIN  33  // Data

// Selektor (przycisk do nawigacji w menu - osobny od enkodera!)
#define SELECTOR_PIN 20     // ✅ NOWY - wolny pin bez konfliktów

// Joystick analogowy (tylko osie - przycisk to selektor)
#define JOYSTICK_X_PIN  34  // ADC1 (input-only)
#define JOYSTICK_Y_PIN  35  // ADC1 (input-only)

// Przekaźniki (6 pistoletów malarskich) - NAPRAWIONE v1.5.0
// KRYTYCZNA ZMIANA: GPIO 12-15 są strapping pins!
// GPIO 12 musi być LOW przy boot (flash voltage) - jeśli HIGH → ESP może NIE WYSTARTOWAĆ!
// NOWE MAPOWANIE (bezpieczne piny):
#define RELAY_1_PIN 10  // Pistolet P1 (oś, 12cm) - ✅ BEZPIECZNY
#define RELAY_2_PIN 11  // Pistolet P2 (oś, 12cm) - ✅ BEZPIECZNY
#define RELAY_3_PIN 8   // Pistolet P3 (oś, 12cm) - ✅ BEZPIECZNY
#define RELAY_4_PIN 9   // Pistolet P4 (oś, 24cm) - ✅ BEZPIECZNY
#define RELAY_5_PIN 16  // Pistolet P5 (krawędź, 12cm) - ✅ Bez zmian
#define RELAY_6_PIN 17  // Pistolet P6 (krawędź, 24cm) - ✅ Bez zmian

// Przyciski sterowania - POPRAWIONE (bez konfliktów)
#define BTN_START_PIN     0   // Start/Pauza (OK dla boot gdy INPUT_PULLUP)
#define BTN_STOP_PIN      2   // Stop
#define BTN_REVERSE_PIN   4   // Odwrócenie P-3a/P-3b
#define BTN_START_GAP_PIN 46  // Start od przerwy (v1.3.0)

// Przyciski wzorców (15 wzorców) - POPRAWIONE (ESP32-S3 specific GPIOs)
#define BTN_P1A_PIN 26  // P-1a
#define BTN_P1B_PIN 27  // P-1b
#define BTN_P1C_PIN 36  // P-1c (input-only)
#define BTN_P1D_PIN 37  // P-1d
#define BTN_P1E_PIN 38  // P-1e
#define BTN_P2A_PIN 39  // P-2a (input-only)
#define BTN_P2B_PIN 40  // P-2b
#define BTN_P3A_PIN 41  // P-3a
#define BTN_P3B_PIN 42  // P-3b
#define BTN_P4_PIN  43  // P-4
#define BTN_P6_PIN  44  // P-6
#define BTN_P7A_PIN 45  // P-7a
#define BTN_P7B_PIN 47  // P-7b
#define BTN_P7C_PIN 48  // P-7c
#define BTN_P7D_PIN 3   // P-7d (UART RX) - ZMIENIONE z GPIO 1 na GPIO 3 (v1.5.0)

// UWAGA v1.5.0: BTN_P7D przeniesiony z GPIO 1 (UART TX) na GPIO 3 (UART RX)
// GPIO 3 jest bezpieczniejszy - mniej konfliktów z bootowaniem
// W produkcji Serial może być wyłączony (DEBUG_ENABLED = 0)

// ============================================================================
// KONFIGURACJA WIFI - v1.2.0
// ============================================================================

#define WIFI_SSID "Trassar"
#define WIFI_PASSWORD "12345678"
#define WIFI_HOSTNAME "Trassar-Road-Painter"
#define WEB_SERVER_PORT 80
#define WIFI_AP_MODE true        // Access Point mode
#define WIFI_AP_CHANNEL 6        // Kanał WiFi
#define WIFI_MAX_CONNECTIONS 4   // Max klientów

// ============================================================================
// STAŁE KONFIGURACYJNE
// ============================================================================

// Enkoder
#define ENCODER_PULSES_PER_REV 20    // Ilość impulsów na obrót
#define WHEEL_CIRCUMFERENCE_CM 200.0  // Obwód koła w cm (domyślnie, zmienia się przy kalibracji)

// Bezpieczeństwo pistoletów
#define MIN_SPEED_KMH 2.0           // Minimalna prędkość do aktywacji pistoletów
#define MIN_MOVEMENT_CM 1           // Minimalny ruch do wykrycia (1cm)
#define MOVEMENT_TIMEOUT_MS 1000    // Timeout bez ruchu (1 sekunda)

// Wyświetlacz
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define REFRESH_RATE  100  // ms

// Kalibracja
#define CALIBRATION_DISTANCE_CM 1000  // 10 metrów w cm
#define EEPROM_CALIBRATION_ADDR 0
#define EEPROM_MAGIC_NUMBER 0xCAFE
#define MIN_PULSES_PER_CM 0.01    // Sanity check min
#define MAX_PULSES_PER_CM 1000.0  // Sanity check max

// Debounce - NOWE (non-blocking)
#define DEBOUNCE_DELAY 50         // ms - dla przycisków

// Kolory (RGB565)
#define COLOR_BACKGROUND 0x0000  // Czarny
#define COLOR_TEXT       0xFFFF  // Biały
#define COLOR_HEADER     0x07FF  // Cyan
#define COLOR_WARNING    0xFFE0  // Żółty
#define COLOR_ERROR      0xF800  // Czerwony
#define COLOR_SUCCESS    0x07E0  // Zielony
#define COLOR_BUTTON     0x0451  // Ciemnoniebieski
#define COLOR_ACTIVE     0x07E0  // Zielony

// Joystick - Progi analogowe
#define JOYSTICK_THRESHOLD_UP    1000
#define JOYSTICK_THRESHOLD_DOWN  3000
#define JOYSTICK_CENTER_MIN      1800
#define JOYSTICK_CENTER_MAX      2200

// ============================================================================
// WYLICZENIA
// ============================================================================

// Wzorce malowania
enum PatternType {
    PATTERN_P1A = 0,
    PATTERN_P1B,
    PATTERN_P1C,
    PATTERN_P1D,
    PATTERN_P1E,
    PATTERN_P2A,
    PATTERN_P2B,
    PATTERN_P3A,
    PATTERN_P3B,
    PATTERN_P4,
    PATTERN_P6,
    PATTERN_P7A,
    PATTERN_P7B,
    PATTERN_P7C,
    PATTERN_P7D,
    PATTERN_COUNT
};

// Stany systemu - ROZSZERZONE (dodano ERROR i SERVICE)
enum SystemStateType {
    STATE_IDLE = 0,
    STATE_PAINTING,
    STATE_PAUSED,
    STATE_MENU,
    STATE_CALIBRATING,
    STATE_MEASURING,
    STATE_ERROR,        // NOWE v1.4.0 - stan błędu
    STATE_SERVICE       // NOWE v1.4.1 - tryb serwisowy (czyszczenie pistoletów)
};

// Wyniki menu
enum MenuResult {
    MENU_NONE = 0,
    MENU_EXIT,
    MENU_CALIBRATION_START,
    MENU_MEASURE_START,
    MENU_SERVICE_START  // NOWE v1.4.1 - start trybu serwisowego
};

// Wyniki kalibracji
enum CalibrationResult {
    CALIBRATION_NONE = 0,
    CALIBRATION_COMPLETE,
    CALIBRATION_CANCELLED,
    CALIBRATION_IN_PROGRESS
};

// Kody błędów - NOWE v1.4.0
enum ErrorCode {
    ERROR_NONE = 0,
    ERROR_PATTERN_INVALID,
    ERROR_CALIBRATION_FAILED,
    ERROR_ENCODER_FAULT,
    ERROR_RELAY_FAULT,
    ERROR_DISPLAY_FAULT,
    ERROR_WIFI_FAULT,
    ERROR_MEMORY_FAULT,
    ERROR_WATCHDOG_TIMEOUT
};

// ============================================================================
// STRUKTURY DANYCH
// ============================================================================

/**
 * Struktura opisująca wzorzec malowania
 */
struct Pattern {
    PatternType type;
    const char* name;
    float lineLength;      // Długość linii w metrach (0 = ciągła)
    float gapLength;       // Długość przerwy w metrach
    uint8_t width;         // Szerokość NOMINALNA w cm (12 lub 24)
    const char* description;
    bool reversible;       // Czy wzorzec może być odwrócony

    // Mapowanie pistoletów (które pistolety są aktywne)
    bool gun1;  // P1 (oś, 12cm)
    bool gun2;  // P2 (oś, 12cm)
    bool gun3;  // P3 (oś, 12cm)
    bool gun4;  // P4 (oś, 24cm)
    bool gun5;  // P5 (krawędź, 12cm)
    bool gun6;  // P6 (krawędź, 24cm)

    // Rzeczywista szerokość malowania (dla obliczeń powierzchni)
    float realWidth;  // w cm
};

/**
 * Struktura stanu systemu - ROZSZERZONA v1.4.0
 */
struct SystemState {
    SystemStateType state;
    PatternType currentPattern;
    bool patternReversed;
    float speed;                // Prędkość w km/h
    long distance;              // Przejechany dystans w cm
    long lastDistance;          // Poprzedni dystans (do wykrywania ruchu)
    float totalPaintedArea;     // Wymalowana powierzchnia w m²
    bool patternChanged;
    unsigned long startTime;
    unsigned long lastMovementTime;  // Timestamp ostatniego ruchu
    bool safetyLocked;          // Blokada bezpieczeństwa pistoletów

    // START GAP - v1.3.0
    bool startFromGap;          // Czy start od przerwy (zamiast od linii)
    float offsetDistance;       // Offset w metrach dla przesunięcia cyklu
    long patternStartDistance;  // Dystans w momencie zmiany wzorca

    // ERROR HANDLING - NOWE v1.4.0
    ErrorCode lastError;        // Ostatni błąd
    const char* errorMessage;   // Komunikat błędu
    unsigned long errorTime;    // Timestamp błędu

    // OPTYMALIZACJA - NOWE v1.4.0
    // Pre-computed values (obliczane raz przy zmianie wzorca)
    long lineLengthCm;          // Długość linii w cm (dla integer math)
    long gapLengthCm;           // Długość przerwy w cm
    long cycleLengthCm;         // Długość cyklu w cm
    long offsetDistanceCm;      // Offset w cm
};

/**
 * Struktura danych kalibracji
 */
struct CalibrationData {
    uint16_t magicNumber;       // Numer magiczny do weryfikacji
    float pulsesPerCm;          // Impulsy enkodera na 1 cm
    uint32_t calibrationDate;   // Data kalibracji (timestamp)
    uint8_t checksum;           // Suma kontrolna
};

/**
 * Struktura przycisku wzorca - NOWE v1.4.0 (dla refaktoryzacji)
 */
struct PatternButton {
    uint8_t pin;
    PatternType pattern;
};

// ============================================================================
// MAKRA POMOCNICZE
// ============================================================================

#define CM_TO_M(cm) ((cm) / 100.0f)
#define M_TO_CM(m) ((m) * 100.0f)
#define KMH_TO_MS(kmh) ((kmh) / 3.6f)
#define MS_TO_KMH(ms) ((ms) * 3.6f)

// DEBUG macros
#define DEBUG_ENABLED 1

#if DEBUG_ENABLED
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(...)
#endif

// ============================================================================
// ZMIENNE GLOBALNE - Thread-safe
// ============================================================================

// NOWE v1.4.0: FreeRTOS mutex dla thread-safety
extern SemaphoreHandle_t stateMutex;  // Mutex dla systemState
extern SemaphoreHandle_t encoderMutex; // Mutex dla enkodera

// Helper do bezpiecznego dostępu do stanu
#define LOCK_STATE() xSemaphoreTake(stateMutex, portMAX_DELAY)
#define UNLOCK_STATE() xSemaphoreGive(stateMutex)

// ============================================================================
// KONFIGURACJA EVENT LOG - NOWE v1.4.0
// ============================================================================

#define EVENT_LOG_ENABLED 1
#define EVENT_LOG_SIZE 100  // Ilość zdarzeń w ring buffer

enum EventType {
    EVENT_SYSTEM_START = 0,
    EVENT_PATTERN_CHANGED,
    EVENT_STATE_CHANGED,
    EVENT_SAFETY_TRIGGERED,
    EVENT_CALIBRATION_START,
    EVENT_CALIBRATION_COMPLETE,
    EVENT_ERROR_OCCURRED,
    EVENT_BUTTON_PRESSED,
    EVENT_WIFI_CONNECTED,
    EVENT_WIFI_DISCONNECTED
};

struct LogEvent {
    unsigned long timestamp;  // millis()
    EventType type;
    uint16_t data1;          // Opcjonalne dane (np. PatternType, StateType)
    uint16_t data2;          // Opcjonalne dane
    const char* message;     // Krótki komunikat
};

#endif // CONFIG_H
