/**
 * System Sterowania Malowaniem Pasów Drogowych
 * v1.6.9 - DIAGNOSTYKA: Minimalny program testowy
 *
 * CEL: Ustalenie czy crash StoreProhibited (EXCVADDR=0x10) jest
 * spowodowany przez nasz kod czy przez hardware/framework.
 *
 * INSTRUKCJA:
 * 1. rm -rf .pio
 * 2. pio run && pio run -t upload
 * 3. pio device monitor
 *
 * OCZEKIWANY WYNIK:
 * Jeśli widzisz "=== ETAP X ===" - system bootuje poprawnie do tego etapu.
 * Jeśli crash - problem jest w POPRZEDNIM etapie.
 *
 * Copyright (c) 2026 MT220126 Engineering Team
 */

#include <Arduino.h>

// === ETAP 1: Podstawowe includes (bez TFT!) ===
#include <SPI.h>
#include <esp_task_wdt.h>

// === ETAP 2: Config (definicje GPIO) ===
#include "config_v140_NEW.h"

// === ETAP 3: Patterns (const array) ===
#include "patterns.h"

// === ETAP 4: TFT_eSPI ===
#include <TFT_eSPI.h>

// === ETAP 5: Klasy projektu ===
#include "display_manager.h"
#include "encoder_handler.h"
#include "relay_controller.h"
#include "menu_system.h"
#include "calibration.h"
#include "service_mode.h"
#include "event_logger.h"
#include "dual_encoder_manager.h"
#include "sd_card_manager.h"

// Wersja
const char* SOFTWARE_VERSION = "1.6.9-DIAG";
const char* BUILD_DATE = __DATE__;
const char* BUILD_TIME = __TIME__;

// FreeRTOS mutex
SemaphoreHandle_t stateMutex = NULL;
SemaphoreHandle_t encoderMutex = NULL;

// Wszystkie obiekty jako pointery (nullptr) - v1.6.7
TFT_eSPI* tft = nullptr;
DisplayManager* display = nullptr;
EventLogger* eventLogger = nullptr;
DualEncoderManager* dualEncoder = nullptr;
SDCardManager* sdCard = nullptr;
RelayController* relays = nullptr;
MenuSystem* menu = nullptr;
CalibrationManager* calibration = nullptr;
ServiceMode* serviceMode = nullptr;

// Stan systemu
SystemState systemState;
volatile bool interruptFlag = false;

// Tablica przycisków
const PatternButton PATTERN_BUTTONS[] = {
    {BTN_P1A_PIN, PATTERN_P1A}, {BTN_P1B_PIN, PATTERN_P1B}, {BTN_P1C_PIN, PATTERN_P1C},
    {BTN_P1D_PIN, PATTERN_P1D}, {BTN_P1E_PIN, PATTERN_P1E}, {BTN_P2A_PIN, PATTERN_P2A},
    {BTN_P2B_PIN, PATTERN_P2B}, {BTN_P3A_PIN, PATTERN_P3A}, {BTN_P3B_PIN, PATTERN_P3B},
    {BTN_P4_PIN, PATTERN_P4},   {BTN_P6_PIN, PATTERN_P6},   {BTN_P7A_PIN, PATTERN_P7A},
    {BTN_P7B_PIN, PATTERN_P7B}, {BTN_P7C_PIN, PATTERN_P7C}, {BTN_P7D_PIN, PATTERN_P7D}
};
const uint8_t NUM_PATTERN_BUTTONS = sizeof(PATTERN_BUTTONS) / sizeof(PatternButton);

void IRAM_ATTR encoderISR() {
    interruptFlag = true;
}

void setup() {
    // =========================================
    // ETAP A: Serial
    // =========================================
    Serial.begin(115200);
    delay(500);  // Więcej czasu na inicjalizację USB-CDC

    Serial.println("\n\n\n");
    Serial.println("########################################");
    Serial.println("# v1.6.9 DIAGNOSTYKA - Test bootowania #");
    Serial.println("########################################");
    Serial.printf("Build: %s %s\n", BUILD_DATE, BUILD_TIME);
    Serial.println("=== ETAP A: Serial OK ===\n");

    // =========================================
    // ETAP B: GPIO Validation
    // =========================================
    Serial.println("--- ETAP B: Walidacja GPIO ---");
    if (!validateGPIOPins()) {
        Serial.println("!!! GPIO VALIDATION FAILED !!!");
        while(true) { delay(1000); Serial.println("ZABLOKOWANY - GPIO ERROR"); }
    }
    Serial.println("=== ETAP B: GPIO OK ===\n");

    // =========================================
    // ETAP C: FreeRTOS Mutex
    // =========================================
    Serial.println("--- ETAP C: FreeRTOS Mutex ---");
    stateMutex = xSemaphoreCreateMutex();
    encoderMutex = xSemaphoreCreateMutex();
    if (!stateMutex || !encoderMutex) {
        Serial.println("FATAL: Mutex creation failed!");
        while(1) delay(1000);
    }
    Serial.println("=== ETAP C: Mutex OK ===\n");

    // =========================================
    // ETAP D: TFT_eSPI object
    // =========================================
    Serial.println("--- ETAP D: Tworzenie TFT_eSPI ---");
    tft = new TFT_eSPI();
    if (tft == nullptr) {
        Serial.println("FATAL: TFT_eSPI allocation failed!");
        while(1) delay(1000);
    }
    Serial.println("=== ETAP D: TFT_eSPI object OK ===\n");

    // =========================================
    // ETAP E: TFT init
    // =========================================
    Serial.println("--- ETAP E: TFT init() ---");
    tft->init();
    tft->setRotation(1);
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->drawString("DIAGNOSTYKA v1.6.9", 10, 10, 2);
    Serial.println("=== ETAP E: TFT init OK ===\n");

    // =========================================
    // ETAP F: EventLogger
    // =========================================
    Serial.println("--- ETAP F: EventLogger ---");
    eventLogger = new EventLogger();
    eventLogger->init();
    Serial.println("=== ETAP F: EventLogger OK ===\n");

    // =========================================
    // ETAP G: DisplayManager
    // =========================================
    Serial.println("--- ETAP G: DisplayManager ---");
    display = new DisplayManager(tft);
    display->init();
    Serial.println("=== ETAP G: DisplayManager OK ===\n");

    // =========================================
    // ETAP H: DualEncoderManager
    // =========================================
    Serial.println("--- ETAP H: DualEncoderManager ---");
    dualEncoder = new DualEncoderManager(eventLogger);
    dualEncoder->init();
    Serial.println("=== ETAP H: DualEncoder OK ===\n");

    // =========================================
    // ETAP I: SD Card
    // =========================================
    Serial.println("--- ETAP I: SDCardManager ---");
    sdCard = new SDCardManager(eventLogger);
    bool sdOk = sdCard->init();
    Serial.printf("=== ETAP I: SD Card %s ===\n\n", sdOk ? "OK" : "NIEDOSTEPNA (to OK)");

    // =========================================
    // ETAP J: RelayController
    // =========================================
    Serial.println("--- ETAP J: RelayController ---");
    relays = new RelayController();
    relays->init();
    Serial.println("=== ETAP J: Relays OK ===\n");

    // =========================================
    // ETAP K: MenuSystem + Calibration + ServiceMode
    // =========================================
    Serial.println("--- ETAP K: Menu + Calibration + ServiceMode ---");
    menu = new MenuSystem(display, dualEncoder->getPrimaryEncoder());
    menu->init();
    calibration = new CalibrationManager(dualEncoder->getPrimaryEncoder());
    calibration->init();
    serviceMode = new ServiceMode(display, relays);
    serviceMode->init();
    Serial.println("=== ETAP K: Menu/Cal/Service OK ===\n");

    // =========================================
    // ETAP L: Encoder Interrupts
    // =========================================
    Serial.println("--- ETAP L: Encoder Interrupts ---");
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_BACKUP_CLK_PIN), encoderISR, CHANGE);
    Serial.println("=== ETAP L: Interrupts OK ===\n");

    // =========================================
    // ETAP M: Buttons
    // =========================================
    Serial.println("--- ETAP M: Buttons ---");
    for (int i = 0; i < NUM_PATTERN_BUTTONS; i++) {
        pinMode(PATTERN_BUTTONS[i].pin, INPUT_PULLUP);
    }
    pinMode(BTN_START_PIN, INPUT_PULLUP);
    pinMode(BTN_STOP_PIN, INPUT_PULLUP);
    pinMode(BTN_REVERSE_PIN, INPUT_PULLUP);
    pinMode(BTN_START_GAP_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);
    pinMode(SELECTOR_PIN, INPUT_PULLUP);
    Serial.println("=== ETAP M: Buttons OK ===\n");

    // =========================================
    // ETAP N: System State Init
    // =========================================
    Serial.println("--- ETAP N: System State ---");
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
    systemState.startFromGap = false;
    systemState.offsetDistance = 0.0;
    systemState.patternStartDistance = 0;
    Serial.println("=== ETAP N: State OK ===\n");

    // =========================================
    // WSZYSTKO OK!
    // =========================================
    Serial.println("########################################");
    Serial.println("# WSZYSTKIE ETAPY PRZESZLY POMYSLNIE! #");
    Serial.println("# System bootuje poprawnie.            #");
    Serial.println("########################################\n");

    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_GREEN, TFT_BLACK);
    tft->drawString("DIAGNOSTYKA OK!", 10, 10, 4);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->drawString("Wszystkie etapy A-N OK", 10, 50, 2);
    tft->drawString("System gotowy do pracy", 10, 80, 2);
    tft->drawString("v1.6.9-DIAG", 10, 110, 2);

    // Watchdog
    esp_task_wdt_init(10, true);
    esp_task_wdt_add(NULL);
}

void loop() {
    esp_task_wdt_reset();

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 5000) {
        Serial.printf("[%lu] System dziala OK, heap: %d bytes free\n",
                     millis(), ESP.getFreeHeap());
        lastPrint = millis();
    }

    delay(10);
}
