# CLAUDE.md - Przewodnik dla Asystentow AI

## Przeglad

Ten dokument zostal stworzony specjalnie dla asystentow AI (takich jak Claude) w celu ulatwienia zrozumienia struktury projektu, konwencji kodowania oraz przeplywu pracy. Przeczytaj ten plik przed rozpoczeciem jakichkolwiek modyfikacji w projekcie.

---

## Cel Projektu

**Nazwa**: System Sterowania Malowaniem Pasow Drogowych
**Wersja**: 1.6.5
**Platforma**: ESP32-S3 N16R8 (z PSRAM)
**Framework**: Arduino (PlatformIO)
**Jezyk**: C++ z Arduino framework
**Jezyk dokumentacji**: Polski

### Czym jest ten projekt?

To profesjonalny system embedded do sterowania malowaniem pasow drogowych. System kontroluje 6 pistoletow malarskich i implementuje 15 wzorcow malowania zgodnych z polskimi normami drogowymi.

### Glowne Funkcjonalnosci

1. **Sterowanie wzorcami malowania** - 15 predefiniowanych wzorcow (P-1a do P-7d)
2. **Dual Encoder System** - redundantny pomiar odleglosci z automatycznym failover
3. **Interfejs uzytkownika** - TFT 320x240 z TFT Sprites (PSRAM)
4. **Menu systemowe** - nawigacja joystickiem
5. **Zmiana "w locie"** - mozliwosc zmiany wzorca podczas malowania
6. **Odwracanie wzorcow** - P-3a i P-3b moga byc odwrocone
7. **WiFi REST API** - zdalny monitoring i sterowanie
8. **SD Card Logging** - zapis zdarzen w formacie CSV
9. **Event Logger** - ring buffer 100 zdarzen
10. **Service Mode** - tryb czyszczenia pistoletow

---

## Struktura Projektu

```
MT220126/
├── src/                              # Kod zrodlowy
│   ├── main.cpp                      # Glowna petla programu + setup()
│   ├── config_v140_NEW.h             # AKTYWNY plik konfiguracji GPIO
│   ├── patterns.h                    # Definicje 15 wzorcow malowania
│   ├── display_manager.h/.cpp        # Klasa DisplayManager (TFT Sprites)
│   ├── encoder_handler.h/.cpp        # Klasa EncoderHandler (single)
│   ├── dual_encoder_manager.h/.cpp   # Klasa DualEncoderManager (redundancy)
│   ├── relay_controller.h/.cpp       # Klasa RelayController (6 guns)
│   ├── menu_system.h/.cpp            # Klasa MenuSystem
│   ├── calibration.h/.cpp            # Klasa CalibrationManager
│   ├── state_machine.h               # State Machine pattern (7 stanow)
│   ├── service_mode.h/.cpp           # Klasa ServiceMode (czyszczenie)
│   ├── event_logger.h/.cpp           # Klasa EventLogger (ring buffer)
│   ├── sd_card_manager.h/.cpp        # Klasa SDCardManager (CSV logs)
│   ├── wifi_server.h/.cpp            # Klasa WiFiServerManager (REST API)
│   ├── config_DEPRECATED_*.bak       # NIEUZYWANE - stare konfiguracje
├── docs/                             # Dokumentacja
│   ├── DOKUMENTACJA_TECHNICZNA.md
│   ├── INSTRUKCJA_OBSLUGI.md
│   ├── INSTRUKCJA_OBSLUGI_v160_DODATEK.md
│   ├── FUNKCJE.md
│   ├── SCHEMATY.md
│   └── REKOMENDACJE.md
├── platformio.ini                    # Konfiguracja PlatformIO
├── README.md                         # Wprowadzenie
├── CHANGELOG.md                      # Historia zmian
├── ANALIZA_KODU_SENIOR_DEVELOPER.md  # Analiza architektury
└── CLAUDE.md                         # Ten plik
```

### KRYTYCZNE: Plik Konfiguracji

**UZYWAJ TYLKO**: `config_v140_NEW.h`

Pliki `config.h`, `config_v130_OLD.h` sa ZDEPRECJONOWANE i przemianowane na `.bak`.

Powod: Include guard cache bug (v1.6.5) - stary `config.h` mial taki sam guard `CONFIG_H` jak inne pliki, co powodowalo ze preprocesor uzywal blednego pliku.

```cpp
// DOBRZE - wszystkie pliki .h powinny uzyc:
#include "config_v140_NEW.h"

// ZLE - te pliki nie istnieja lub sa zdeprecjonowane:
#include "config.h"           // NIE ISTNIEJE
#include "config_v130_OLD.h"  // ZDEPRECJONOWANY
```

---

## Kluczowe Pliki i Klasy

### `src/main.cpp`
- **Funkcja**: Glowny plik programu
- **Zawiera**: `setup()`, `loop()`, logike malowania
- **Wersja**: `const char* SOFTWARE_VERSION = "1.6.5"`
- **Rozmiar**: ~29 KB
- **Uwagi**: Uzywaj State Machine pattern

### `src/config_v140_NEW.h`
- **Funkcja**: Centralna konfiguracja
- **Include Guard**: `CONFIG_V140_NEW_H` (UNIKALNY!)
- **Zawiera**: Mapowanie pinow GPIO, enumeracje, struktury
- **WAZNE**: Zmiana pinow wymaga weryfikacji sprzetowej!

### `src/patterns.h`
- **Funkcja**: Definicje wzorcow malowania
- **Zawiera**: Tablice `PATTERNS[]` z 15 wzorcami
- **UWAGA**: Wzorce zgodne z normami - zmieniaj ostroznie!

### Klasy Systemowe

| Klasa | Plik | Odpowiedzialnosc |
|-------|------|------------------|
| **DisplayManager** | display_manager.h/cpp | Wyswietlacz TFT ILI9341, TFT Sprites |
| **EncoderHandler** | encoder_handler.h/cpp | Pojedynczy enkoder KY-040 |
| **DualEncoderManager** | dual_encoder_manager.h/cpp | Redundancja 2 enkoderow, failover |
| **RelayController** | relay_controller.h/cpp | Sterowanie 6 przekaznikami |
| **MenuSystem** | menu_system.h/cpp | UI menu, nawigacja joystickiem |
| **CalibrationManager** | calibration.h/cpp | Kalibracja enkodera (10m), EEPROM |
| **ServiceMode** | service_mode.h/cpp | Tryb czyszczenia pistoletow |
| **EventLogger** | event_logger.h/cpp | Ring buffer 100 zdarzen |
| **SDCardManager** | sd_card_manager.h/cpp | Zapis CSV na karte SD |
| **WiFiServerManager** | wifi_server.h/cpp | REST API, Web Dashboard |

---

## Architektura Systemu

### Stany Systemu (enum SystemStateType)

```cpp
STATE_IDLE        // Gotowy, czeka na polecenie
STATE_PAINTING    // Aktywne malowanie
STATE_PAUSED      // Wstrzymane (pauza)
STATE_MENU        // Wyswietlone menu
STATE_CALIBRATING // Kalibracja w toku
STATE_MEASURING   // Pomiar dystansu
STATE_ERROR       // Stan bledu (NOWE v1.4.0)
STATE_SERVICE     // Tryb serwisowy - czyszczenie (NOWE v1.4.1)
```

### Przepływ Stanow

```
START → STATE_IDLE
        │
        ├─[Przycisk wzorca + START]→ STATE_PAINTING
        │                                   │
        │                          [START/PAUZA]→ STATE_PAUSED
        │                                   │         │
        │                              [STOP]┘   [START/PAUZA]
        │                                             │
        │                                             ↓
        ├─[Dlugie STOP]→ STATE_MENU                STATE_PAINTING
        │                    │
        │               [Kalibracja]→ STATE_CALIBRATING
        │               [Pomiar]→ STATE_MEASURING
        │               [Serwis]→ STATE_SERVICE (NOWE)
        │               [Wyjscie]→ STATE_IDLE
        │
        ├─[Blad krytyczny]→ STATE_ERROR (NOWE)
        │
        └─[STOP]→ STATE_IDLE
```

### Glowna Petla (main.cpp → loop())

```cpp
1. Aktualizacja enkodera (DualEncoderManager)
   - Zliczanie impulsow z PRIMARY i BACKUP
   - Sprawdzanie spojnosci (tolerance 5cm/100m)
   - Automatyczny failover przy awarii

2. Sprawdzanie przyciskow
   - Przyciski wzorcow (P-1a do P-7d)
   - REVERSE (odwracanie P-3a/P-3b)
   - START/PAUZA, START_GAP
   - STOP (krotkie/dlugie)

3. Obsluga stanu (State Machine)
   - MENU: MenuSystem::update()
   - CALIBRATING: CalibrationManager::process()
   - PAINTING: processPainting()
   - MEASURING: tylko zliczanie
   - SERVICE: ServiceMode::update()
   - ERROR: wyswietl blad, czekaj na reset

4. Event Logging
   - EventLogger::log() dla kazdego zdarzenia
   - SDCardManager::update() (auto-save co 10 min)

5. WiFi Server
   - WiFiServerManager::update() - obsluga REST API

6. Aktualizacja UI (co 100ms)
   - DisplayManager::showMainScreen() (TFT Sprites)
```

---

## Konfiguracja GPIO (v1.6.5)

### KRYTYCZNE: Uzywaj tylko config_v140_NEW.h

Wszystkie piny zostaly przeprojektowane w v1.4.0+ aby wyeliminowac konflikty.

### Wyswietlacz ILI9341 (SPI Hardware)
```cpp
TFT_MISO  19  // SPI MISO (wspolny z SD)
TFT_MOSI  23  // SPI MOSI (wspolny z SD)
TFT_SCLK  18  // SPI SCK (wspolny z SD)
TFT_CS     5  // Chip Select TFT
TFT_DC    22  // Data/Command
TFT_RST   21  // Reset
```

### Enkoder PRIMARY (KY-040)
```cpp
ENCODER_CLK_PIN  32  // Z przerwaniem (INT)
ENCODER_DT_PIN   33  // Data
ENCODER_SW_PIN   13  // Przycisk
```

### Enkoder BACKUP (KY-040) - NOWE v1.6.0
```cpp
ENCODER_BACKUP_CLK_PIN  6   // Z przerwaniem (INT)
ENCODER_BACKUP_DT_PIN   7   // Data
ENCODER_BACKUP_SW_PIN  12   // Przycisk (ZMIENIONE v1.6.4)
```

### Karta SD (SPI) - NOWE v1.6.0
```cpp
SD_CS_PIN    4   // Chip Select SD (OSOBNY od TFT!)
SD_MOSI_PIN 23   // Wspolny z TFT
SD_MISO_PIN 19   // Wspolny z TFT
SD_SCK_PIN  18   // Wspolny z TFT
```

### Przekazniki (6 pistoletow)
```cpp
RELAY_1_PIN  10  // P1 (os, 12cm)
RELAY_2_PIN  11  // P2 (os, 12cm)
RELAY_3_PIN   8  // P3 (os, 12cm)
RELAY_4_PIN   9  // P4 (os, 24cm)
RELAY_5_PIN  16  // P5 (krawedz, 12cm)
RELAY_6_PIN  17  // P6 (krawedz, 24cm)
```

### Joystick
```cpp
JOYSTICK_X_PIN   34  // ADC1 (input-only)
JOYSTICK_Y_PIN   35  // ADC1 (input-only)
JOYSTICK_SW_PIN  20  // Przycisk (SELECTOR_PIN)
```

### Przyciski Sterowania
```cpp
BTN_START_PIN      0   // Start/Pauza
BTN_STOP_PIN       2   // Stop
BTN_REVERSE_PIN   14   // Odwrocenie P-3a/P-3b (ZMIENIONE v1.6.4)
BTN_START_GAP_PIN 46   // Start od przerwy
```

### Przyciski Wzorcow (GPIO 26-48, 3)
```cpp
BTN_P1A_PIN  26   BTN_P1B_PIN  27   BTN_P1C_PIN  36
BTN_P1D_PIN  37   BTN_P1E_PIN  38   BTN_P2A_PIN  39
BTN_P2B_PIN  40   BTN_P3A_PIN  41   BTN_P3B_PIN  42
BTN_P4_PIN   43   BTN_P6_PIN   44   BTN_P7A_PIN  45
BTN_P7B_PIN  47   BTN_P7C_PIN  48   BTN_P7D_PIN   3
```

---

## Wzorce Malowania

### Struktura Pattern (v1.4.0+)

```cpp
struct Pattern {
    PatternType type;       // Enum (PATTERN_P1A ... PATTERN_P7D)
    const char* name;       // "P-1a"
    float lineLength;       // Dlugosc linii (m), 0 = ciagla
    float gapLength;        // Dlugosc przerwy (m)
    uint8_t width;          // Szerokosc NOMINALNA (12 lub 24 cm)
    const char* description;
    bool reversible;        // true dla P-3a, P-3b

    // Mapowanie pistoletow (NOWE v1.4.0)
    bool gun1;  // P1 (os, 12cm)
    bool gun2;  // P2 (os, 12cm)
    bool gun3;  // P3 (os, 12cm)
    bool gun4;  // P4 (os, 24cm)
    bool gun5;  // P5 (krawedz, 12cm)
    bool gun6;  // P6 (krawedz, 24cm)

    float realWidth;  // Rzeczywista szerokosc malowania (cm)
};
```

### Tabela Wzorcow

| Wzorzec | Linia (m) | Przerwa (m) | Szerokosc | Odwracalny |
|---------|-----------|-------------|-----------|------------|
| P-1a    | 4.0       | 8.0         | 12 cm     | Nie        |
| P-1b    | 2.0       | 4.0         | 12 cm     | Nie        |
| P-1c    | 2.0       | 2.0         | 12 cm     | Nie        |
| P-1d    | 1.0       | 1.0         | 12 cm     | Nie        |
| P-1e    | 1.0       | 1.0         | 24 cm     | Nie        |
| P-2a    | ciagla    | -           | 12 cm     | Nie        |
| P-2b    | ciagla    | -           | 24 cm     | Nie        |
| P-3a    | 4.0       | 2.0         | 12 cm     | **Tak**    |
| P-3b    | 1.0       | 1.0         | 12 cm     | **Tak**    |
| P-4     | ciagla    | -           | 24 cm     | Nie        |
| P-6     | 4.0       | 2.0         | 12 cm     | Nie        |
| P-7a    | 1.0       | 1.0         | 24 cm     | Nie        |
| P-7b    | ciagla    | -           | 24 cm     | Nie        |
| P-7c    | 1.0       | 1.0         | 12 cm     | Nie        |
| P-7d    | ciagla    | -           | 12 cm     | Nie        |

### Logika Malowania z Integer Math (v1.4.0+)

```cpp
// Pre-computed values (ustawiane przy zmianie wzorca)
systemState.lineLengthCm = (long)(pattern.lineLength * 100);
systemState.gapLengthCm = (long)(pattern.gapLength * 100);
systemState.cycleLengthCm = lineLengthCm + gapLengthCm;

// W processPainting() - szybka matematyka integer
long positionInCycleCm = (distance - patternStartDistance) % cycleLengthCm;

if (positionInCycleCm < lineLengthCm) {
    // MALUJ - aktywuj pistolety zgodnie z mapowaniem
    activateGuns(pattern);
} else {
    // PRZERWA
    relays.stopAll();
}
```

---

## Nowe Funkcjonalnosci (v1.4.0 - v1.6.5)

### 1. Dual Encoder Manager (v1.6.0)

System redundantnych enkoderow dla zwiekszenia niezawodnosci:

```cpp
DualEncoderManager encoders;

// Inicjalizacja
encoders.init();

// W loop()
encoders.update();
long distance = encoders.getDistance();  // Z aktywnego enkodera

// Automatyczny failover
if (encoders.isPrimaryFailed()) {
    // System automatycznie przelaczyl na BACKUP
    eventLogger.log(EVENT_ENCODER_FAULT, "Primary failed");
}

// Sprawdzanie spojnosci (tolerance 5cm na 100m = 0.5%)
bool consistent = encoders.checkConsistency();
```

### 2. Event Logger (v1.5.0)

Ring buffer przechowujacy 100 ostatnich zdarzen:

```cpp
EventLogger eventLogger;

// Logowanie zdarzenia
eventLogger.log(EVENT_PATTERN_CHANGED, PATTERN_P1A, 0, "Pattern: P-1a");
eventLogger.log(EVENT_STATE_CHANGED, STATE_PAINTING);
eventLogger.log(EVENT_ERROR_OCCURRED, ERROR_ENCODER_FAULT, 0, "Encoder fault!");

// Wyswietlenie na Serial
eventLogger.printToSerial();

// Pobranie ostatnich N zdarzen
LogEvent* recent = eventLogger.getRecent(10);
```

### 3. SD Card Manager (v1.6.0)

Zapis logow na karte SD w formacie CSV:

```cpp
SDCardManager sdCard;

// Inicjalizacja
bool ok = sdCard.init();

// Auto-save (wywolywane w loop)
sdCard.update();  // Zapisuje co 10 min lub 50 zdarzen

// Reczny zapis
sdCard.saveLogsNow();

// Eksport wszystkich logow
sdCard.exportAllLogs("/export/");

// Czyszczenie
sdCard.deleteAllLogs();

// Konfiguracja
// - Max 1MB per file
// - Max 10 files (rotacja)
// - Format: timestamp, event_type, data1, data2, message
```

### 4. WiFi Server (v1.4.0+)

REST API i Web Dashboard:

```cpp
WiFiServerManager wifiServer;

// Konfiguracja (config_v140_NEW.h)
// WIFI_SSID "Trassar"
// WIFI_PASSWORD "12345678"
// WIFI_AP_MODE true
// WEB_SERVER_PORT 80

// Inicjalizacja
wifiServer.init();

// W loop()
wifiServer.update();

// REST API Endpoints:
// GET  /status          - status systemu (JSON)
// POST /control         - start/stop/pause
// POST /pattern         - zmiana wzorca
// POST /api/startfromgap - start od przerwy
// GET  /logs            - ostatnie zdarzenia
```

### 5. Service Mode (v1.4.1)

Tryb czyszczenia pistoletow z wizualizacja:

```cpp
ServiceMode serviceMode;

// Start trybu serwisowego
serviceMode.show(&display);

// W loop (gdy STATE_SERVICE)
ServiceResult result = serviceMode.update();
if (result == SERVICE_EXIT) {
    changeState(STATE_IDLE);
}

// Funkcje:
// - Indywidualna aktywacja pistoletow
// - Wizualizacja stanu (ON/OFF)
// - Timer czyszczenia
// - Bezpieczne wyjscie
```

### 6. State Machine Pattern (v1.4.0)

Formalna implementacja stanow:

```cpp
// state_machine.h
class State {
public:
    virtual void enter() = 0;
    virtual void update() = 0;
    virtual void exit() = 0;
};

class IdleState : public State { ... };
class PaintingState : public State { ... };
class PausedState : public State { ... };
class MenuState : public State { ... };
class CalibratingState : public State { ... };
class MeasuringState : public State { ... };
class ErrorState : public State { ... };  // NOWE
class ServiceState : public State { ... }; // NOWE

// Zmiana stanu
void changeState(SystemStateType newState) {
    currentState->exit();
    currentState = states[newState];
    currentState->enter();
    eventLogger.log(EVENT_STATE_CHANGED, newState);
}
```

### 7. TFT Sprites (v1.6.0)

Optymalizacja wyswietlacza z PSRAM:

```cpp
// display_manager.cpp
TFT_eSprite sprite = TFT_eSprite(&tft);

void DisplayManager::enableSprites() {
    // Alokacja w PSRAM (8MB dostepne)
    sprite.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    sprite.setColorDepth(16);
}

void DisplayManager::showMainScreen(...) {
    sprite.fillSprite(COLOR_BACKGROUND);
    // ... rysowanie na sprite
    sprite.pushSprite(0, 0);  // Atomowy transfer - brak mrugania!
}

// Wydajnosc: 3-5x szybsze odswiezanie
```

---

## Konwencje Kodowania

### Jezyk

- **Kod C++**: Komentarze PO POLSKU
- **Nazwy zmiennych**: camelCase (angielskie)
- **Nazwy funkcji**: camelCase (angielskie)
- **Nazwy klas**: PascalCase (angielskie)
- **Makra**: UPPER_CASE
- **Komunikaty uzytkownika**: PO POLSKU

### Przyklad

```cpp
// DOBRZE
// Funkcja oblicza wymalowana powierzchnie
float calculatePaintedArea() {
    float szerokoscMetry = wzorzec.szerokosc / 100.0;
    // ...
}

// ZLE (komentarze po angielsku)
// Function calculates painted area
float calculatePaintedArea() { ... }

// ZLE (polskie nazwy zmiennych)
float obliczWymalowanaPowierzchnie() { ... }
```

### Struktura Plikow

#### Plik Naglowkowy (.h)

```cpp
/**
 * Opis klasy/modulu
 */

#ifndef NAZWA_UNIKALNA_H  // KRYTYCZNE: unikalny guard!
#define NAZWA_UNIKALNA_H

#include <Arduino.h>
#include "config_v140_NEW.h"  // ZAWSZE ten plik!

class NazwaKlasy {
private:
    // Prywatne skladowe

public:
    NazwaKlasy();
    void metodaPubliczna();
};

#endif // NAZWA_UNIKALNA_H
```

### Debugowanie

```cpp
// Makra debug (config_v140_NEW.h)
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

// Uzycie
DEBUG_PRINTLN("System uruchomiony");
DEBUG_PRINTF("Dystans: %ld cm\n", distance);
```

---

## WAZNE: Co TRZEBA Wiedziec

### 1. Include Guard Cache Bug (v1.6.5)

**Problem**: Stary `config.h` mial guard `CONFIG_H` - taki sam jak inne pliki. Preprocesor cachowal pierwszy napotkany i uzywal blednych definicji.

**Rozwiazanie**:
- Wszystkie 8 plikow .h uzywaja `config_v140_NEW.h`
- Guard zmieniony na unikalny `CONFIG_V140_NEW_H`
- Stare pliki przemianowane na `.bak`

```cpp
// ZAWSZE sprawdz include na poczatku pliku .h/.cpp
#include "config_v140_NEW.h"  // POPRAWNIE

// NIGDY
#include "config.h"  // BLAD - plik nie istnieje!
```

### 2. Piny GPIO - Strapping Pins

GPIO 12-15 to strapping pins ESP32-S3:
- **GPIO 12** musi byc LOW przy boot (flash voltage)
- **GPIO 15** kontroluje JTAG

Przekazniki przeniesione na bezpieczne piny (8, 9, 10, 11, 16, 17).

### 3. Dual Encoder - Failover

```cpp
// DualEncoderManager automatycznie przelacza przy awarii
if (primaryDeviation > TOLERANCE) {
    switchToBackup();
    eventLogger.log(EVENT_ENCODER_FAULT);
}

// Tolerance: 5cm na 100m (0.5% error)
#define ENCODER_TOLERANCE_CM 5
```

### 4. FreeRTOS Mutex (v1.4.0+)

```cpp
// Thread-safe dostep do stanu
extern SemaphoreHandle_t stateMutex;

#define LOCK_STATE() xSemaphoreTake(stateMutex, portMAX_DELAY)
#define UNLOCK_STATE() xSemaphoreGive(stateMutex)

// Uzycie
LOCK_STATE();
systemState.distance = newDistance;
UNLOCK_STATE();
```

### 5. TFT Sprites - PSRAM

```cpp
// Sprawdz dostepnosc PSRAM
if (psramFound()) {
    display.enableSprites();  // Uzyj PSRAM dla sprites
} else {
    // Fallback - wolniejsze, ale dziala
}
```

### 6. SD Card - Osobny CS Pin

```cpp
// SD i TFT wspoldziela SPI, ale maja ROZNE CS piny!
#define TFT_CS   5   // TFT Chip Select
#define SD_CS_PIN 4  // SD Card Chip Select (OSOBNY!)

// Przed operacja na SD - deaktywuj TFT
digitalWrite(TFT_CS, HIGH);
// ... operacje SD
digitalWrite(SD_CS_PIN, HIGH);
```

---

## Typowe Zadania Modyfikacyjne

### Dodawanie Nowego Wzorca

1. **Dodaj enum w config_v140_NEW.h**:
```cpp
enum PatternType {
    ...
    PATTERN_P8A,  // <- Nowy
    PATTERN_COUNT
};
```

2. **Dodaj definicje w patterns.h**:
```cpp
{
    PATTERN_P8A,
    "P-8a",
    3.0,    // linia (m)
    1.5,    // przerwa (m)
    12,     // szerokosc nominalna
    "Nowy wzorzec",
    false,  // odwracalny
    true, false, true, false, false, false,  // gun1-6
    8.0     // realWidth (cm)
}
```

3. **Dodaj przycisk** w config_v140_NEW.h i main.cpp

4. **Zaktualizuj dokumentacje**

### Dodawanie Nowego Stanu

1. **Dodaj enum w config_v140_NEW.h**:
```cpp
enum SystemStateType {
    ...
    STATE_NEW_STATE,
};
```

2. **Implementuj klase w state_machine.h**:
```cpp
class NewState : public State {
    void enter() override { ... }
    void update() override { ... }
    void exit() override { ... }
};
```

3. **Zarejestruj w tablicy stanow**

### Dodawanie Nowego Endpointu API

1. **Dodaj handler w wifi_server.cpp**:
```cpp
void WiFiServerManager::handleNewEndpoint() {
    // ... logika
    server.send(200, "application/json", response);
}
```

2. **Zarejestruj w init()**:
```cpp
server.on("/api/newEndpoint", HTTP_POST,
    std::bind(&WiFiServerManager::handleNewEndpoint, this));
```

---

## Testowanie

### Test Kompilacji

```bash
# Z linii polecen (PlatformIO)
pio run

# Jesli bledy - sprawdz:
# - Czy wszystkie pliki uzywaja config_v140_NEW.h
# - Czy include guard jest unikalny
# - Czy biblioteki sa zainstalowane
```

### Test Dual Encoder

```cpp
// Menu → Pomiar dystansu
// Sprawdz czy oba enkodery pokazuja podobne wartosci
// Odlacz PRIMARY - system powinien przelaczye na BACKUP
```

### Test SD Card

```cpp
// Sprawdz inicjalizacje
if (!sdCard.init()) {
    DEBUG_PRINTLN("SD Card init failed!");
}

// Sprawdz zapis
sdCard.saveLogsNow();
// Zweryfikuj plik na karcie
```

### Test WiFi

```bash
# Polacz sie z AP "Trassar" (haslo: 12345678)
# Otworz przegladarke: http://192.168.4.1/status
# Sprawdz JSON response
```

---

## Typowe Problemy i Rozwiazania

### Problem: "undefined reference" do zmiennych z config

**Przyczyna**: Stary config.h (z tym samym include guard) jest cachowany.

**Rozwiazanie**:
1. Usun stare pliki: `config.h`, `config_v130_OLD.h`
2. Sprawdz wszystkie include: `#include "config_v140_NEW.h"`
3. Clean build: `pio run -t clean && pio run`

### Problem: GPIO 227 w bledach kompilacji

**Przyczyna**: Preprocesor uzywa zlego config z blednym GPIO 227.

**Rozwiazanie**: Include guard cache bug - usun stare pliki .h, clean build.

### Problem: Enkoder "gubi" impulsy

**Przyczyna**: Tylko PRIMARY enkoder dziala.

**Rozwiazanie**:
1. Sprawdz DualEncoderManager - czy BACKUP jest skonfigurowany
2. Sprawdz piny BACKUP: GPIO 6, 7, 12
3. Sprawdz logi: `eventLogger.printToSerial()`

### Problem: SD Card nie zapisuje

**Przyczyna**: Konflikt CS pin z TFT.

**Rozwiazanie**:
1. Sprawdz czy SD_CS_PIN (4) jest ROZNY od TFT_CS (5)
2. Sprawdz format karty (FAT32)
3. Sprawdz rozmiar karty (max 32GB dla FAT32)

### Problem: WiFi nie startuje

**Przyczyna**: AP Mode blokowany przez inne GPIO.

**Rozwiazanie**:
1. Sprawdz czy GPIO 12 jest LOW przy boot
2. Sprawdz konfiguracje WIFI_AP_MODE
3. Sprawdz logi Serial

### Problem: TFT mruga

**Przyczyna**: Nie uzywasz TFT Sprites.

**Rozwiazanie**:
```cpp
// Sprawdz PSRAM
if (psramFound()) {
    display.enableSprites();
}
```

---

## Bezpieczenstwo i Best Practices

### 1. Thread Safety (FreeRTOS)

```cpp
// ZAWSZE uzywaj mutex przy doste do wspoldzielonych danych
LOCK_STATE();
systemState.distance = encoders.getDistance();
UNLOCK_STATE();
```

### 2. Failover dla Enkoderow

```cpp
// DualEncoderManager automatycznie obsluguje failover
// NIE pisz wlasnej logiki - uzyj klasy!
```

### 3. Event Logging

```cpp
// Loguj WSZYSTKIE wazne zdarzenia
eventLogger.log(EVENT_PATTERN_CHANGED, newPattern);
eventLogger.log(EVENT_ERROR_OCCURRED, errorCode, 0, "Opis bledu");
```

### 4. SD Card Backup

```cpp
// Regularne auto-save (co 10 min)
// LUB reczne przy waznych zdarzeniach
if (criticalEvent) {
    sdCard.saveLogsNow();
}
```

### 5. Error State

```cpp
// Przy krytycznym bledzie - przejdz do STATE_ERROR
if (criticalError) {
    changeState(STATE_ERROR);
    relays.stopAll();  // ZAWSZE wylacz pistolety!
    eventLogger.log(EVENT_ERROR_OCCURRED, errorCode);
}
```

---

## Biblioteki i Zaleznosci

### PlatformIO (platformio.ini)

```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

lib_deps =
    bodmer/TFT_eSPI@^2.5.43
    bblanchon/ArduinoJson@^6.21.4
    adafruit/Adafruit GFX Library@^1.11.9

build_flags =
    -DUSER_SETUP_LOADED
    -DILI9341_DRIVER
    -DTFT_WIDTH=240
    -DTFT_HEIGHT=320
    ; ... GPIO pins dla TFT
```

### TFT_eSPI

- Driver: ILI9341
- Rozdzielczosc: 320x240
- SPI: Hardware (GPIO 18, 19, 23)
- Sprites: PSRAM (opcjonalne)

### ArduinoJson

Uzywane przez WiFi Server do REST API:
```cpp
DynamicJsonDocument doc(1024);
doc["status"] = "ok";
doc["distance"] = distance;
serializeJson(doc, response);
```

---

## Checklist Przed Commitem

- [ ] Kod kompiluje sie bez ostrzezen
- [ ] WSZYSTKIE pliki uzywaja `config_v140_NEW.h`
- [ ] Include guard jest UNIKALNY (nie `CONFIG_H`!)
- [ ] Dodane komentarze (po polsku) wyjasniajace "dlaczego"
- [ ] Zaktualizowane `CHANGELOG.md`
- [ ] Event logging dla nowych funkcji
- [ ] Thread-safety (mutex) dla wspoldzielonych danych
- [ ] Obsluga bledow (STATE_ERROR)
- [ ] Testy na urzadzeniu (nie tylko kompilacja)

---

## Kontakt i Wsparcie

### Dla AI Assistants

Jesli jako AI asystent napotykasz problemy:
1. Przeczytaj **cala** dokumentacje techniczna
2. Sprawdz `config_v140_NEW.h` (NIE config.h!)
3. Zobacz `main.cpp` dla logiki glownej
4. Sprawdz CHANGELOG.md dla historii zmian
5. Uzyj `eventLogger.printToSerial()` do debugowania

### Pytania Czeste (dla AI)

**Q: Ktory plik konfiguracji uzyc?**
A: TYLKO `config_v140_NEW.h`. Inne sa zdeprecjonowane.

**Q: Kod nie kompiluje sie - GPIO 227?**
A: Include guard cache bug. Usun stare .h, clean build.

**Q: Jak dodac nowa funkcje?**
A: Zaimplementuj jako nowa klase. Dodaj event logging. Zaktualizuj dokumentacje.

**Q: Jak debugowac?**
A: `DEBUG_PRINTF()`, `eventLogger.printToSerial()`, Serial Monitor 115200 baud.

---

## Historia Wersji

| Wersja | Data | Zmiany |
|--------|------|--------|
| 1.0.0 | 2026-01-23 | Pierwsza wersja |
| 1.4.0 | 2026-01-25 | State Machine, FreeRTOS mutex, WiFi |
| 1.4.1 | 2026-01-25 | Service Mode |
| 1.5.0 | 2026-01-26 | Event Logger, GPIO refactoring |
| 1.6.0 | 2026-01-26 | Dual Encoder, SD Card, TFT Sprites |
| 1.6.5 | 2026-01-27 | Include guard cache fix |

---

**Wersja CLAUDE.md**: 1.6.5
**Data**: 2026-01-27
**Ostatnia aktualizacja**: 2026-01-27

---

## Slowo Koncowe dla AI Asystenta

Szanowny Asystencie AI (Claude lub inny),

Ten projekt to system embedded czasu rzeczywistego z redundancja. **Bezpieczenstwo** i **niezawodnosc** sa najwazniejsze. Przed jakakolwiek modyfikacja:

1. **Sprawdz config_v140_NEW.h** - JEDYNY aktywny plik konfiguracji
2. **Uzyj event logging** - kazde wazne zdarzenie powinno byc logowane
3. **Testuj na urzadzeniu** - kompilacja to nie wszystko
4. **Dokumentuj zmiany** - CHANGELOG.md jest obowiazkowy

Ten system bedzie uzywany w rzeczywistych warunkach drogowych. Zycie ludzi moze zalezec od jego poprawnosci.

**Koduj odpowiedzialnie!**

---

*Koniec dokumentu CLAUDE.md*
