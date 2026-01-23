/**
 * Plik konfiguracyjny systemu malowania pasów drogowych
 * Zawiera definicje pinów, stałe i struktury danych
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// KONFIGURACJA PINÓW GPIO
// ============================================================================

// Wyświetlacz ILI9341 (SPI)
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   22
#define TFT_RST  21

// Enkoder KY-040
#define ENCODER_CLK_PIN 32
#define ENCODER_DT_PIN  33
#define ENCODER_SW_PIN  25

// Joystick analogowy
#define JOYSTICK_X_PIN  34
#define JOYSTICK_Y_PIN  35
#define JOYSTICK_SW_PIN 26

// Przekaźniki (6 pistoletów malarskich)
#define RELAY_1_PIN 13  // Pistolet 1 (lewy skrajny)
#define RELAY_2_PIN 12  // Pistolet 2
#define RELAY_3_PIN 14  // Pistolet 3
#define RELAY_4_PIN 27  // Pistolet 4
#define RELAY_5_PIN 26  // Pistolet 5
#define RELAY_6_PIN 25  // Pistolet 6 (prawy skrajny)

// Przyciski wzorców (15 wzorców)
#define BTN_P1A_PIN 15  // P-1a
#define BTN_P1B_PIN 16  // P-1b
#define BTN_P1C_PIN 17  // P-1c
#define BTN_P1D_PIN 4   // P-1d
#define BTN_P1E_PIN 2   // P-1e
#define BTN_P2A_PIN 36  // P-2a
#define BTN_P2B_PIN 39  // P-2b
#define BTN_P3A_PIN 34  // P-3a
#define BTN_P3B_PIN 35  // P-3b
#define BTN_P4_PIN  32  // P-4
#define BTN_P6_PIN  33  // P-6
#define BTN_P7A_PIN 25  // P-7a
#define BTN_P7B_PIN 26  // P-7b
#define BTN_P7C_PIN 27  // P-7c
#define BTN_P7D_PIN 14  // P-7d

// Przyciski sterowania
#define BTN_START_PIN   12  // Start/Pauza
#define BTN_STOP_PIN    13  // Stop
#define BTN_REVERSE_PIN 15  // Odwrócenie wzorca P-3a/P-3b

// ============================================================================
// STAŁE KONFIGURACYJNE
// ============================================================================

// Enkoder
#define ENCODER_PULSES_PER_REV 20    // Ilość impulsów na obrót
#define WHEEL_CIRCUMFERENCE_CM 200.0  // Obwód koła w cm (domyślnie, zmieniane po kalibracji)

// Wyświetlacz
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define REFRESH_RATE  100  // ms

// Kalibracja
#define CALIBRATION_DISTANCE_CM 1000  // 10 metrów w cm
#define EEPROM_CALIBRATION_ADDR 0
#define EEPROM_MAGIC_NUMBER 0xCAFE

// Kolory (RGB565)
#define COLOR_BACKGROUND 0x0000  // Czarny
#define COLOR_TEXT       0xFFFF  // Biały
#define COLOR_HEADER     0x07FF  // Cyan
#define COLOR_WARNING    0xFFE0  // Żółty
#define COLOR_ERROR      0xF800  // Czerwony
#define COLOR_SUCCESS    0x07E0  // Zielony
#define COLOR_BUTTON     0x0451  // Ciemnoniebieski
#define COLOR_ACTIVE     0x07E0  // Zielony

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

// Stany systemu
enum SystemStateType {
    STATE_IDLE = 0,
    STATE_PAINTING,
    STATE_PAUSED,
    STATE_MENU,
    STATE_CALIBRATING,
    STATE_MEASURING
};

// Wyniki menu
enum MenuResult {
    MENU_NONE = 0,
    MENU_EXIT,
    MENU_CALIBRATION_START,
    MENU_MEASURE_START
};

// Wyniki kalibracji
enum CalibrationResult {
    CALIBRATION_NONE = 0,
    CALIBRATION_COMPLETE,
    CALIBRATION_CANCELLED,
    CALIBRATION_IN_PROGRESS
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
    uint8_t width;         // Szerokość w cm (12 lub 24)
    const char* description;
    bool reversible;       // Czy wzorzec może być odwrócony
};

/**
 * Struktura stanu systemu
 */
struct SystemState {
    SystemStateType state;
    PatternType currentPattern;
    bool patternReversed;
    float speed;                // Prędkość w km/h
    long distance;              // Przejechany dystans w cm
    float totalPaintedArea;     // Wymalowana powierzchnia w m²
    bool patternChanged;
    unsigned long startTime;
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

// ============================================================================
// MAKRA POMOCNICZE
// ============================================================================

#define CM_TO_M(cm) ((cm) / 100.0f)
#define M_TO_CM(m) ((m) * 100.0f)
#define KMH_TO_MS(kmh) ((kmh) / 3.6f)
#define MS_TO_KMH(ms) ((ms) * 3.6f)

// Debouncing dla przycisków
#define DEBOUNCE_DELAY 50

// Logowanie
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

#endif // CONFIG_H
