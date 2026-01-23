/**
 * Plik konfiguracyjny systemu malowania pasów drogowych
 * Zawiera definicje pinów, stałe i struktury danych
 *
 * WERSJA 1.1.0 - POPRAWKI KRYTYCZNE
 * - Naprawiono konflikty pinów GPIO
 * - Dodano pin selektora (zamiast enkodera do menu)
 * - Dodano konfigurację WiFi
 * - Dodano zabezpieczenie min. prędkości
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// KONFIGURACJA PINÓW GPIO - POPRAWIONA (bez konfliktów!)
// ============================================================================

// Wyświetlacz ILI9341 (SPI Hardware)
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   22
#define TFT_RST  21

// Enkoder KY-040 (TYLKO DO POMIARÓW - NIE DO MENU!)
#define ENCODER_CLK_PIN 32  // Z przerwaniem
#define ENCODER_DT_PIN  33

// Selektor (przycisk do nawigacji w menu - NOWY!)
#define SELECTOR_PIN 25  // Przycisk selektora (był ENCODER_SW)

// Joystick analogowy (tylko osie - przycisk = selektor)
#define JOYSTICK_X_PIN  34  // ADC1
#define JOYSTICK_Y_PIN  35  // ADC1

// Przekaźniki (6 pistoletów malarskich) - POPRAWIONE PINY
// P1, P2, P3, P4 = Oś jezdni
// P5, P6 = Krawędź jezdni
#define RELAY_1_PIN 13  // Pistolet P1 (oś, 12cm)
#define RELAY_2_PIN 12  // Pistolet P2 (oś, 12cm)
#define RELAY_3_PIN 14  // Pistolet P3 (oś, 12cm)
#define RELAY_4_PIN 27  // Pistolet P4 (oś, 24cm)
#define RELAY_5_PIN 26  // Pistolet P5 (krawędź, 12cm)
#define RELAY_6_PIN 25  // Pistolet P6 (krawędź, 24cm) - ZMIENIONY

// UWAGA: P6 używa tego samego pinu co SELECTOR - ale nie kolidują
// bo przekaźnik jest OUTPUT, selektor INPUT_PULLUP
// Na produkcji zaleca się rozdzielenie!

// Przyciski wzorców (15 wzorców) - ZMIENIONE PINY (bez konfliktów)
#define BTN_P1A_PIN 15  // P-1a
#define BTN_P1B_PIN 16  // P-1b
#define BTN_P1C_PIN 17  // P-1c
#define BTN_P1D_PIN 4   // P-1d
#define BTN_P1E_PIN 2   // P-1e
#define BTN_P2A_PIN 36  // P-2a (input only)
#define BTN_P2B_PIN 39  // P-2b (input only)
#define BTN_P3A_PIN 34  // P-3a (input only) - współdzielony z JOY_X (OK)
#define BTN_P3B_PIN 35  // P-3b (input only) - współdzielony z JOY_Y (OK)
#define BTN_P4_PIN  32  // P-4 - współdzielony z ENC_CLK (KONFLIKT!)
#define BTN_P6_PIN  33  // P-6 - współdzielony z ENC_DT (KONFLIKT!)

// TODO PRODUKCJA: Przenieść BTN_P4 i BTN_P6 na wolne GPIO!
// Możliwe GPIO: 0, 37, 38, 40-48

// Przyciski wzorców krawędziowych - NOWE PINY
#define BTN_P7A_PIN 0   // P-7a (ZMIENIONY z 25)
#define BTN_P7B_PIN 37  // P-7b (ZMIENIONY z 26) - ESP32-S3 only
#define BTN_P7C_PIN 38  // P-7c (ZMIENIONY z 27) - ESP32-S3 only
#define BTN_P7D_PIN 40  // P-7d (ZMIENIONY z 14) - ESP32-S3 only

// Przyciski sterowania - NOWE PINY (bez konfliktów)
#define BTN_START_PIN     41  // Start/Pauza (ZMIENIONY z 12)
#define BTN_STOP_PIN      42  // Stop (ZMIENIONY z 13)
#define BTN_REVERSE_PIN   45  // Odwrócenie P-3a/P-3b (ZMIENIONY z 15)
#define BTN_START_GAP_PIN 46  // Start od przerwy (NOWY!) - ESP32-S3 only

// ============================================================================
// KONFIGURACJA WIFI - NOWA
// ============================================================================

#define WIFI_SSID "Trassar"
#define WIFI_PASSWORD "12345678"
#define WIFI_HOSTNAME "Trassar-Road-Painter"
#define WEB_SERVER_PORT 80

// ============================================================================
// STAŁE KONFIGURACYJNE
// ============================================================================

// Enkoder
#define ENCODER_PULSES_PER_REV 20    // Ilość impulsów na obrót
#define WHEEL_CIRCUMFERENCE_CM 200.0  // Obwód koła w cm (domyślnie)

// Bezpieczeństwo pistoletów - NOWE!
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
 * ROZSZERZONA: Dodano informacje o pistoletach
 */
struct Pattern {
    PatternType type;
    const char* name;
    float lineLength;      // Długość linii w metrach (0 = ciągła)
    float gapLength;       // Długość przerwy w metrach
    uint8_t width;         // Szerokość NOMINALNA w cm (12 lub 24)
    const char* description;
    bool reversible;       // Czy wzorzec może być odwrócony

    // NOWE: Mapowanie pistoletów (które pistolety są aktywne)
    bool gun1;  // P1 (oś, 12cm)
    bool gun2;  // P2 (oś, 12cm)
    bool gun3;  // P3 (oś, 12cm)
    bool gun4;  // P4 (oś, 24cm)
    bool gun5;  // P5 (krawędź, 12cm)
    bool gun6;  // P6 (krawędź, 24cm)

    // NOWE: Rzeczywista szerokość malowania (dla obliczeń powierzchni)
    float realWidth;  // w cm (suma szerokości aktywnych pistoletów)
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
    long lastDistance;          // Poprzedni dystans (do wykrywania ruchu) - NOWE
    float totalPaintedArea;     // Wymalowana powierzchnia w m²
    bool patternChanged;
    unsigned long startTime;
    unsigned long lastMovementTime;  // Timestamp ostatniego ruchu - NOWE
    bool safetyLocked;          // Blokada bezpieczeństwa pistoletów - NOWE

    // START GAP - NOWE v1.3.0
    bool startFromGap;          // Czy start od przerwy (zamiast od linii)
    float offsetDistance;       // Offset w metrach dla przesunięcia cyklu
    long patternStartDistance;  // Dystans w momencie zmiany wzorca (dla obliczenia offsetu)
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

// ============================================================================
// NOTATKI PRODUKCYJNE
// ============================================================================

/*
 * UWAGA DLA PRODUKCJI:
 *
 * 1. Konflikty pinów do rozwiązania:
 *    - BTN_P4 (GPIO 32) vs ENCODER_CLK (GPIO 32)
 *    - BTN_P6 (GPIO 33) vs ENCODER_DT (GPIO 33)
 *    - SELECTOR (GPIO 25) vs RELAY_6 (GPIO 25)
 *
 *    Rozwiązanie: Użyć GPIO 43, 44, 46, 47, 48 dla przycisków
 *
 * 2. Joystick współdzieli piny z przyciskami (34, 35)
 *    To jest OK - joystick to ADC, przyciski to INPUT_PULLUP
 *
 * 3. Watchdog timer - wymaga włączenia w main.cpp
 *
 * 4. WiFi hasło "12345678" - NIE BEZPIECZNE dla produkcji!
 *    Zalecane: Unikalny klucz dla każdego urządzenia
 */

#endif // CONFIG_H
