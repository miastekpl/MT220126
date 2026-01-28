# CLAUDE.md - Przewodnik dla Asystentow AI

## Przegląd

Ten dokument został stworzony specjalnie dla asystentow AI (takich jak Claude) w celu ułatwienia zrozumienia struktury projektu, konwencji kodowania oraz przepływu pracy. Przeczytaj ten plik przed rozpoczęciem jakichkolwiek modyfikacji w projekcie.

---

## Cel Projektu

**Nazwa**: System Sterowania Malowaniem Pasow Drogowych
**Wersja**: 1.6.8
**Platforma**: ESP32-S3 N16R8
**Framework**: Arduino (PlatformIO)
**Język**: C++ z Arduino framework
**Język dokumentacji**: Polski

### Czym jest ten projekt?

To profesjonalny system embedded do sterowania malowaniem pasow drogowych. System kontroluje 6 pistoletow malarskich i implementuje 15 wzorcow malowania zgodnych z polskimi normami drogowymi.

### Główne Funkcjonalności

1. **Sterowanie wzorcami malowania** - 15 predefiniowanych wzorcow (P-1a do P-7d)
2. **Kalibracja enkodera** - precyzyjny pomiar odległości (±1cm)
3. **Dual Encoder** - redundancja z automatycznym przełączaniem (v1.6.0)
4. **Interfejs użytkownika** - TFT 320x240 z intuicyjnym UI (TFT Sprites opcjonalne)
5. **Menu systemowe** - nawigacja joystickiem
6. **Zmiana "w locie"** - możliwość zmiany wzorca podczas malowania
7. **Odwracanie wzorcow** - P-3a i P-3b mogą być odwrocone
8. **WiFi AP + REST API** - zdalne sterowanie (192.168.4.1) (v1.4.0)
9. **Tryb serwisowy** - czyszczenie pistoletow (hold-to-fire) (v1.4.1)
10. **Event Logger** - logowanie zdarzeń systemowych (v1.5.0)
11. **SD Card Logging** - zapis logow na kartę SD (v1.6.0)

---

## Struktura Projektu

```
MT220126/
├── include/                          # Nagłówki projektu (v1.6.8)
│   └── User_Setup.h                  # Konfiguracja TFT_eSPI dla ESP32-S3
├── src/                              # Kod źrodłowy
│   ├── main.cpp                      # Główna pętla programu + setup()
│   ├── config_v140_NEW.h             # AKTYWNY config - piny GPIO, stałe, struktury
│   ├── patterns.h                    # Definicje wzorcow malowania
│   ├── display_manager.h/.cpp        # Klasa DisplayManager (TFT + Sprites)
│   ├── encoder_handler.h/.cpp        # Klasa EncoderHandler
│   ├── dual_encoder_manager.h/.cpp   # Klasa DualEncoderManager (redundancja) [v1.6.0]
│   ├── relay_controller.h/.cpp       # Klasa RelayController
│   ├── menu_system.h/.cpp            # Klasa MenuSystem
│   ├── calibration.h/.cpp            # Klasa CalibrationManager
│   ├── service_mode.h/.cpp           # Klasa ServiceMode [v1.4.1]
│   ├── event_logger.h/.cpp           # Klasa EventLogger [v1.5.0]
│   ├── sd_card_manager.h/.cpp        # Klasa SDCardManager [v1.6.0]
│   ├── wifi_server.h/.cpp            # Klasa WiFiServerManager [v1.4.0]
│   ├── state_machine.h               # Przygotowanie State Pattern [v1.4.0]
│   ├── config_DEPRECATED_DO_NOT_USE.h.bak  # STARY config - NIE UŻYWAĆ!
│   └── config_v130_DEPRECATED.h.bak        # BARDZO STARY - NIE UŻYWAĆ!
├── docs/                             # Dokumentacja
│   ├── DOKUMENTACJA_TECHNICZNA.md
│   ├── INSTRUKCJA_OBSLUGI.md
│   ├── INSTRUKCJA_OBSLUGI_v160_DODATEK.md
│   ├── SCHEMATY.md
│   ├── FUNKCJE.md
│   └── REKOMENDACJE.md
├── production_packages/              # Pakiety produkcyjne (ZIP)
├── platformio.ini                    # Konfiguracja PlatformIO
├── README.md                         # Wprowadzenie
├── CHANGELOG.md                      # Historia zmian
├── ANALIZA_KODU_SENIOR_DEVELOPER.md  # Analiza kodu
└── CLAUDE.md                         # Ten plik
```

---

## KRYTYCZNE: TFT_eSPI User_Setup.h (v1.6.8 FIX)

### Problem Zidentyfikowany

Biblioteka TFT_eSPI zawierała domyślny `User_Setup.h` z pinami dla **ESP8266/NodeMCU** zamiast ESP32-S3:

```cpp
// BŁĘDNE PINY W DOMYŚLNYM User_Setup.h:
#define TFT_MISO  PIN_D6  // Makra ESP8266 - nie istnieją na ESP32!
#define TFT_MOSI  PIN_D7
#define TFT_SCLK  PIN_D5
#define TFT_CS    PIN_D8
#define TFT_DC    PIN_D3  // -> 227 (garbage value) na ESP32-S3!
#define TFT_RST   PIN_D4
```

**Skutek**: GPIO 227 (garbage) -> `pinMode(227)` FAIL -> Guru Meditation Error!

### Rozwiązanie (v1.6.8)

1. **Własny User_Setup.h**: Utworzono `include/User_Setup.h` z prawidłowymi pinami
2. **platformio.ini**: Dodano `-I include` PRZED innymi flagami
3. **lib_archive = false**: Wymusza rekompilację bibliotek

```cpp
// include/User_Setup.h - PRAWIDŁOWE PINY ESP32-S3:
#define TFT_MISO 19   // Liczby, NIE makra!
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5
#define TFT_DC   22
#define TFT_RST  21
```

### WYMAGANE przy każdej kompilacji

```bash
# ZAWSZE clean rebuild!
rm -rf .pio
pio run
```

---

## KRYTYCZNE: Include Guard Cache Problem (v1.6.5 FIX)

### Problem Zidentyfikowany

C preprocessor cachował guard `CONFIG_H` z pierwszego pliku i **CAŁKOWICIE IGNOROWAŁ** drugi plik z tym samym guardem!

```cpp
// KONFLIKT (v1.6.0-v1.6.4):
config.h:           #ifndef CONFIG_H  <- Preprocessor cachuje!
config_v140_NEW.h:  #ifndef CONFIG_H  <- IGNOROWANE! Cała zawartość pomijana!
```

**Skutek**: GPIO definicje z config_v140_NEW.h NIGDY nie były używane -> GPIO 227 -> crash!

### Rozwiązanie (v1.6.5)

1. **Unikalny Include Guard**: `CONFIG_H` -> `CONFIG_V140_NEW_H`
2. **Deprecation starych plikow**: `.h` -> `.h.bak` (uniemożliwia include)
3. **Debug output GPIO** w main.cpp przy starcie

### WYMAGANE przy każdej kompilacji

```bash
# ZAWSZE clean rebuild!
rm -rf .pio
pio run -t clean
pio run
```

---

## Kluczowe Pliki

### `src/main.cpp`
- **Funkcja**: Główny plik programu
- **Zawiera**: `setup()`, `loop()`, logikę malowania
- **Wersja**: SOFTWARE_VERSION = "1.6.5"
- **Uwagi**: Nie modyfikuj struktury stanow bez pełnego zrozumienia

### `src/config_v140_NEW.h` (AKTYWNY CONFIG!)
- **Funkcja**: Centralna konfiguracja
- **Zawiera**: Mapowanie pinow GPIO, enumeracje, struktury, FreeRTOS mutex
- **Include Guard**: `CONFIG_V140_NEW_H` (UNIKALNY!)
- **WAŻNE**: Zmiana pinow tutaj wymaga weryfikacji sprzętowej!

### `src/patterns.h`
- **Funkcja**: Definicje wzorcow malowania
- **Zawiera**: Tablicę `PATTERNS[]` z 15 wzorcami
- **UWAGA**: Wzorce zgodne z normami - zmieniaj ostrożnie!

### `src/display_manager.*`
- **Klasa**: `DisplayManager`
- **Odpowiedzialność**: Wszystkie operacje na wyświetlaczu TFT
- **Optymalizacja**: TFT Sprites (double buffering) jeśli PSRAM dostępny
- **Używa**: Biblioteki TFT_eSPI

### `src/encoder_handler.*`
- **Klasa**: `EncoderHandler`
- **Odpowiedzialność**: Pomiar odległości i prędkości
- **Przerwania**: Używa przerwania na GPIO 32 (CLK)
- **Thread-safety**: FreeRTOS mutex (v1.4.2+)
- **Precyzja**: ±1cm po kalibracji

### `src/dual_encoder_manager.*` [v1.6.0]
- **Klasa**: `DualEncoderManager`
- **Odpowiedzialność**: Zarządzanie dwoma enkoderami (redundancja)
- **PRIMARY**: GPIO 32/33/13, **BACKUP**: GPIO 6/7/12
- **Auto-failover**: Automatyczne przełączanie przy awarii

### `src/relay_controller.*`
- **Klasa**: `RelayController`
- **Odpowiedzialność**: Sterowanie 6 przekaźnikami
- **Bezpieczeństwo**: Natychmiastowe wyłączanie przy błędach

### `src/menu_system.*`
- **Klasa**: `MenuSystem`
- **Odpowiedzialność**: UI menu, nawigacja joystickiem
- **Pozycje**: Kalibracja, Pomiar, Serwis, Reset, Info, Wyjście

### `src/calibration.*`
- **Klasa**: `CalibrationManager`
- **Odpowiedzialność**: Kalibracja enkodera na 10m
- **Pamięć**: Używa Preferences (EEPROM emulation)
- **Zabezpieczenia**: Magic number + checksum

### `src/service_mode.*` [v1.4.1]
- **Klasa**: `ServiceMode`
- **Odpowiedzialność**: Tryb serwisowy (czyszczenie pistoletow)
- **Mechanizm**: Hold-to-fire (pistolety ON tylko gdy przycisk wciśnięty)
- **Wizualizacja**: 6 kwadratow (zielony=fire, żołty=standby, szary=off)

### `src/event_logger.*` [v1.5.0]
- **Klasa**: `EventLogger`
- **Odpowiedzialność**: Ring buffer (100 zdarzeń) z timestampami
- **Typy zdarzeń**: SYSTEM_START, PATTERN_CHANGED, STATE_CHANGED, itp.

### `src/sd_card_manager.*` [v1.6.0]
- **Klasa**: `SDCardManager`
- **Odpowiedzialność**: Zapis logow na kartę SD
- **Format**: CSV z rotacją plikow
- **Auto-save**: Co 10 min lub 50 zdarzeń

### `src/wifi_server.*` [v1.4.0]
- **Klasa**: `WiFiServerManager`
- **Odpowiedzialność**: WiFi AP + Web Dashboard + REST API
- **SSID**: "Trassar", **Hasło**: "12345678", **IP**: 192.168.4.1
- **API Endpoints**: `/status`, `/control`, `/pattern`, `/api/startfromgap`

---

## Architektura Systemu

### Stany Systemu (enum SystemStateType)

```cpp
STATE_IDLE        // Gotowy, czeka na polecenie
STATE_PAINTING    // Aktywne malowanie
STATE_PAUSED      // Wstrzymane (pauza)
STATE_MENU        // Wyświetlone menu
STATE_CALIBRATING // Kalibracja w toku
STATE_MEASURING   // Pomiar dystansu
STATE_ERROR       // Stan błędu [v1.4.0]
STATE_SERVICE     // Tryb serwisowy [v1.4.1]
```

### Mapowanie GPIO (v1.6.5)

#### Wyświetlacz ILI9341 (SPI)
| Pin | GPIO | Funkcja |
|-----|------|---------|
| MISO | 19 | SPI Master In |
| MOSI | 23 | SPI Master Out |
| SCLK | 18 | SPI Clock |
| CS | 5 | Chip Select TFT |
| DC | 22 | Data/Command |
| RST | 21 | Reset |

#### Enkodery
| Enkoder | CLK | DT | SW |
|---------|-----|----|----|
| PRIMARY | 32 | 33 | 13 |
| BACKUP | 6 | 7 | 12 |

#### Przekaźniki (po fix v1.5.0 - bez strapping pins!)
| Przekaźnik | GPIO | Pistolet |
|------------|------|----------|
| RELAY_1 | 10 | P1 (oś, 12cm) |
| RELAY_2 | 11 | P2 (oś, 12cm) |
| RELAY_3 | 8 | P3 (oś, 12cm) |
| RELAY_4 | 9 | P4 (oś, 24cm) |
| RELAY_5 | 16 | P5 (krawędź, 12cm) |
| RELAY_6 | 17 | P6 (krawędź, 24cm) |

#### Przyciski sterowania
| Przycisk | GPIO | Uwagi |
|----------|------|-------|
| START | 0 | OK dla boot gdy INPUT_PULLUP |
| STOP | 2 | - |
| REVERSE | 14 | ZMIENIONE v1.6.4 (było 4) |
| START_GAP | 46 | v1.3.0+ |

#### SD Card (współdzielony SPI)
| Pin | GPIO |
|-----|------|
| CS | 4 |
| MOSI | 23 (wspołny z TFT) |
| MISO | 19 (wspołny z TFT) |
| SCK | 18 (wspołny z TFT) |

---

## Konwencje Kodowania

### Język

- **Kod C++**: Komentarze PO POLSKU
- **Nazwy zmiennych**: camelCase (angielskie)
- **Nazwy funkcji**: camelCase (angielskie)
- **Nazwy klas**: PascalCase (angielskie)
- **Makra**: UPPER_CASE
- **Komunikaty użytkownika**: PO POLSKU

### Przykład

```cpp
// DOBRZE
// Funkcja oblicza wymalowaną powierzchnię
float calculatePaintedArea() {
    float szerokoscMetry = wzorzec.szerokosc / 100.0;
    // ...
}

// ŹLE (komentarze po angielsku)
// Function calculates painted area
float calculatePaintedArea() {
    // ...
}
```

### Struktura Kodu

#### Plik Nagłowkowy (.h)

```cpp
/**
 * Opis klasy/modułu
 */

#ifndef NAZWA_KLASY_H      // UNIKALNY guard!
#define NAZWA_KLASY_H

#include <Arduino.h>
#include "config_v140_NEW.h"  // ZAWSZE config_v140_NEW.h, NIGDY config.h!

class NazwaKlasy {
private:
    // Prywatne składowe

public:
    // Konstruktor
    NazwaKlasy();

    // Metody publiczne
    void metodaPubliczna();
};

#endif // NAZWA_KLASY_H
```

### Thread-Safety (v1.4.0+)

```cpp
// FreeRTOS mutex dla synchronizacji
extern SemaphoreHandle_t stateMutex;
extern SemaphoreHandle_t encoderMutex;

// Użycie:
LOCK_STATE();
systemState.distance = newValue;
UNLOCK_STATE();

// Lub bezpośrednio:
if (xSemaphoreTake(encoderMutex, portMAX_DELAY) == pdTRUE) {
    position++;
    xSemaphoreGive(encoderMutex);
}
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

// Użycie
DEBUG_PRINTLN("System uruchomiony");
DEBUG_PRINTF("Dystans: %ld cm\n", distance);
```

---

## WAŻNE: Co TRZEBA Wiedzieć

### 1. Include Guards - ZAWSZE UNIKALNE!

```cpp
// DOBRZE - unikalny guard
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

// ŹLE - konflikt z innym plikiem!
#ifndef CONFIG_H
#define CONFIG_H
```

### 2. Plik Config - TYLKO config_v140_NEW.h

```cpp
// DOBRZE
#include "config_v140_NEW.h"

// ŹLE - stary plik, spowoduje crash!
#include "config.h"
```

### 3. Enkoder - Używa Przerwań + Mutex

```cpp
// main.cpp
attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR, CHANGE);

void IRAM_ATTR encoderISR() {
    interruptFlag = true;
}

// encoder_handler.cpp - thread-safe
if (encoderMutex != NULL && xSemaphoreTake(encoderMutex, portMAX_DELAY) == pdTRUE) {
    position++;
    xSemaphoreGive(encoderMutex);
}
```

### 4. Strapping Pins - UNIKAĆ GPIO 12-15!

GPIO 12-15 to strapping pins na ESP32-S3:
- GPIO 12 musi być LOW przy boot (flash voltage)
- Jeśli przekaźnik ON przy boot -> ESP może NIE WYSTARTOWAĆ!

**ROZWIĄZANIE v1.5.0**: Przekaźniki przeniesione na GPIO 8-11.

### 5. PSRAM - Opcjonalne TFT Sprites

```ini
# platformio.ini - jeśli ESP32-S3 MA PSRAM (N16R8):
; board_build.psram_type = opi
; board_build.arduino.memory_type = qio_opi

# Jeśli NIE MA PSRAM - zostaw zakomentowane (auto-fallback)
```

### 6. Przekaźniki - Bezpieczeństwo

```cpp
// ZAWSZE wyłączaj przy błędzie
relays.stopAll();

// Min prędkość do aktywacji pistoletow
if (systemState.speed < MIN_SPEED_KMH) {
    relays.stopAll();  // 2 km/h minimum!
}
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

2. **Dodaj definicję w patterns.h**:
```cpp
{
    PATTERN_P8A,
    "P-8a",
    3.0,    // linia
    1.5,    // przerwa
    12,     // szerokość
    "Nowy wzorzec",
    false,  // odwracalny?
    true, true, true, false, false, false,  // gun1-6
    12.0    // realWidth
}
```

3. **Dodaj przycisk w config_v140_NEW.h i main.cpp**

4. **Zaktualizuj dokumentację**

### Dodawanie Nowej Pozycji Menu

1. **Dodaj enum w menu_system.h**:
```cpp
enum MenuItem {
    ...
    MENU_ITEM_NOWA_FUNKCJA,
    MENU_ITEM_COUNT
};
```

2. **Dodaj tekst w menu_system.cpp**
3. **Obsłuż w handleSelection()**

---

## Testowanie

### Test Kompilacji

```bash
# ZAWSZE clean rebuild!
rm -rf .pio
pio run -t clean
pio run
```

### Weryfikacja GPIO

Sprawdź w Serial Monitor przy starcie:
```
--- DEBUG GPIO PINS ---
ENCODER BACKUP: ... SW=12    <- MUSI BYĆ 12, NIE 19!
BUTTONS: REVERSE=14 ...      <- MUSI BYĆ 14, NIE 4!
--- END GPIO DEBUG ---
```

### Debug przez Serial

```bash
# Monitor Serial (115200 baud)
pio device monitor
```

---

## Typowe Problemy i Rozwiązania

### Problem: "Guru Meditation Error" / GPIO 227

**Przyczyna**: Include guard cache - preprocessor pomija config_v140_NEW.h

**Rozwiązanie**:
1. Usuń `.pio` folder
2. `pio run -t clean`
3. Sprawdź czy wszystkie .h includują `config_v140_NEW.h` (NIE `config.h`!)

### Problem: ESP32 nie bootuje

**Przyczyna**: Przekaźniki na strapping pins (GPIO 12-15)

**Rozwiązanie**: Użyj v1.5.0+ (przekaźniki na GPIO 8-11)

### Problem: Prędkość zawsze 0 km/h

**Przyczyna**: Bug obliczania distanceDiff (naprawiony v1.4.2)

**Sprawdź**: SOFTWARE_VERSION powinno być >= "1.4.2"

### Problem: WiFi nie działa

**Sprawdź**:
- SSID: "Trassar"
- Hasło: "12345678"
- IP: 192.168.4.1

---

## Bezpieczeństwo i Best Practices

### 1. Zawsze Sprawdzaj Granice

```cpp
// DOBRZE
if (relayNum >= 1 && relayNum <= 6) {
    digitalWrite(relayPins[relayNum - 1], state);
}
```

### 2. Używaj fabs() dla float

```cpp
// DOBRZE
#include <cmath>
if (fabs(speed - lastSpeed) > 0.1) { ... }

// ŹLE - undefined behavior!
if (abs(speed - lastSpeed) > 0.1) { ... }
```

### 3. snprintf zamiast sprintf

```cpp
// DOBRZE - bezpieczne
char buffer[32];
snprintf(buffer, sizeof(buffer), "%.1f", speed);

// ŹLE - buffer overflow risk!
sprintf(buffer, "%.1f", speed);
```

### 4. Volatile dla ISR

```cpp
// DOBRZE
volatile bool interruptFlag = false;
volatile long position = 0;

void IRAM_ATTR encoderISR() {
    interruptFlag = true;
}
```

---

## Biblioteki i Zależności

### platformio.ini

```ini
lib_deps =
    bodmer/TFT_eSPI@^2.5.43
    bblanchon/ArduinoJson@^6.21.4
    adafruit/Adafruit GFX Library@^1.11.9
```

### TFT_eSPI

Konfiguracja w `platformio.ini` (build_flags):
- `-DUSER_SETUP_LOADED=1`
- `-DILI9341_DRIVER=1`
- `-DTFT_WIDTH=240`, `-DTFT_HEIGHT=320`
- SPI pins: MISO=19, MOSI=23, SCLK=18, CS=5, DC=22, RST=21

### FreeRTOS (wbudowany w ESP32)

```cpp
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
xSemaphoreTake(mutex, portMAX_DELAY);
// critical section
xSemaphoreGive(mutex);
```

---

## Checklist Przed Commitem

- [ ] Kod kompiluje się bez ostrzeżeń (`rm -rf .pio && pio run`)
- [ ] Wszystkie .h includują `config_v140_NEW.h` (NIE `config.h`!)
- [ ] Include guards są UNIKALNE
- [ ] Dodane komentarze (po polsku) wyjaśniające "dlaczego"
- [ ] Zaktualizowane `CHANGELOG.md`
- [ ] Jeśli zmiana pinow -> zaktualizuj `config_v140_NEW.h` i `docs/SCHEMATY.md`
- [ ] Debug logi używają `DEBUG_PRINTF` (nie `Serial.print`)
- [ ] Funkcje ISR mają `IRAM_ATTR`
- [ ] Float comparisons używają `fabs()` (nie `abs()`)
- [ ] Użyto `snprintf()` zamiast `sprintf()`

---

## Historia Wersji (Skrocona)

| Wersja | Data | Główne zmiany |
|--------|------|---------------|
| 1.6.5 | 2026-01-27 | Include guard cache fix (CONFIG_V140_NEW_H) |
| 1.6.4 | 2026-01-27 | GPIO conflicts fix (REVERSE=14, BACKUP_SW=12) |
| 1.6.0 | 2026-01-26 | Dual Encoder, SD Card, TFT Sprites |
| 1.5.0 | 2026-01-26 | Strapping pins fix, Event Logger |
| 1.4.2 | 2026-01-26 | 5 critical bugfixes (speed, mutex, fabs) |
| 1.4.1 | 2026-01-26 | Service Mode |
| 1.4.0 | 2026-01-23 | WiFi, thread-safety, GPIO redesign |
| 1.3.0 | 2026-01-23 | Start Gap function |
| 1.0.0 | 2026-01-23 | Initial production release |

Pełna historia: patrz `CHANGELOG.md`

---

## Kontakt i Wsparcie

### Dla AI Assistants

Jeśli jako AI asystent napotykasz problemy:
1. Przeczytaj **całą** dokumentację techniczną
2. Sprawdź `config_v140_NEW.h` dla pinow i stałych (NIE config.h!)
3. Zobacz `main.cpp` dla logiki głównej
4. Konsultuj `INSTRUKCJA_OBSLUGI.md` dla flow użytkownika
5. Sprawdź `CHANGELOG.md` dla historii bugfixow

### Pytania Częste (dla AI)

**Q: Ktory plik config używać?**
A: TYLKO `config_v140_NEW.h`. Pliki `config.h` i `config_v130_OLD.h` są deprecated (.bak).

**Q: Jak dodać nową funkcjonalność?**
A: Najpierw zrozum architekturę (stany, klasy). Dodaj kod w odpowiedniej klasie. Zaktualizuj dokumentację.

**Q: Kod mi się nie kompiluje**
A: `rm -rf .pio && pio run -t clean && pio run`. Sprawdź include guards!

**Q: System crashuje (GPIO 227)**
A: Include guard conflict. Upewnij się że WSZYSTKIE .h includują `config_v140_NEW.h`.

---

## Słowo Końcowe dla AI Asystenta

Szanowny Asystencie AI (Claude lub inny),

Ten projekt to system embedded czasu rzeczywistego. **Bezpieczeństwo** i **niezawodność** są najważniejsze. Przed jakąkolwiek modyfikacją:

1. **Zrozum kontekst** - przeczytaj dokumentację
2. **Include guards** - ZAWSZE unikalne, NIGDY duplikaty!
3. **config_v140_NEW.h** - JEDYNY aktywny config
4. **Clean rebuild** - `rm -rf .pio` przed każdą kompilacją
5. **Testuj dokładnie** - nie ma miejsca na błędy
6. **Dokumentuj zmiany** - inni muszą zrozumieć

Ten system będzie używany w rzeczywistych warunkach drogowych. Życie ludzi może zależeć od jego poprawności.

**Koduj odpowiedzialnie!**

---

**Wersja CLAUDE.md**: 1.6.8
**Data**: 2026-01-28
**Ostatnia aktualizacja**: 2026-01-28

---

*Koniec dokumentu CLAUDE.md*
