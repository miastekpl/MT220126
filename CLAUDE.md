# CLAUDE.md - Przewodnik dla Asystentow AI

## Przeglad

Ten dokument zostal stworzony specjalnie dla asystentow AI (takich jak Claude) w celu ulatwienia zrozumienia struktury projektu, konwencji kodowania oraz przeplywu pracy. Przeczytaj ten plik przed rozpoczeciem jakichkolwiek modyfikacji w projekcie.

---

## Cel Projektu

**Nazwa**: System Sterowania Malowaniem Pasow Drogowych
**Wersja**: 1.6.5
**Platforma**: ESP32-S3 N16R8
**Framework**: Arduino (PlatformIO)
**Jezyk**: C++ z Arduino framework
**Jezyk dokumentacji**: Polski

### Czym jest ten projekt?

To profesjonalny system embedded do sterowania malowaniem pasow drogowych. System kontroluje 6 pistoletow malarskich i implementuje 15 wzorcow malowania zgodnych z polskimi normami drogowymi.

### Glowne Funkcjonalnosci

1. **Sterowanie wzorcami malowania** - 15 predefiniowanych wzorcow (P-1a do P-7d)
2. **Kalibracja enkodera** - precyzyjny pomiar odleglosci (+/-1cm)
3. **Interfejs uzytkownika** - TFT 320x240 z intuicyjnym UI (TFT Sprites od v1.6.0)
4. **Menu systemowe** - nawigacja joystickiem
5. **Zmiana "w locie"** - mozliwosc zmiany wzorca podczas malowania
6. **Odwracanie wzorcow** - P-3a i P-3b moga byc odwrocone (dziala od v1.4.0!)
7. **Start Gap** - rozpoczynanie od linii zamiast przerwy (v1.3.0)
8. **WiFi AP + REST API** - zdalne sterowanie (v1.4.0)
9. **Tryb Serwisowy** - czyszczenie pistoletow hold-to-fire (v1.4.1)
10. **Event Logger** - logowanie zdarzen systemowych (v1.5.0)
11. **Dual Encoder** - redundancja z automatycznym przelaczaniem (v1.6.0)
12. **SD Card Logging** - zapis logow na karte SD (v1.6.0)

---

## Struktura Projektu

```
MT220126/
|-- src/                              # Kod zrodlowy
|   |-- main.cpp                      # Glowna petla programu + setup()
|   |-- config_v140_NEW.h             # GLOWNY CONFIG (NIE config.h!)
|   |-- patterns.h                    # Definicje wzorcow malowania
|   |-- display_manager.h/.cpp        # Klasa DisplayManager (TFT Sprites)
|   |-- encoder_handler.h/.cpp        # Klasa EncoderHandler
|   |-- dual_encoder_manager.h/.cpp   # Klasa DualEncoderManager (v1.6.0)
|   |-- relay_controller.h/.cpp       # Klasa RelayController
|   |-- menu_system.h/.cpp            # Klasa MenuSystem
|   |-- calibration.h/.cpp            # Klasa CalibrationManager
|   |-- service_mode.h/.cpp           # Klasa ServiceMode (v1.4.1)
|   |-- event_logger.h/.cpp           # Klasa EventLogger (v1.5.0)
|   |-- sd_card_manager.h/.cpp        # Klasa SDCardManager (v1.6.0)
|   |-- wifi_server.h/.cpp            # Klasa WiFiServerManager (v1.4.0)
|   |-- state_machine.h               # State Machine pattern (przygotowane)
|   |-- config_DEPRECATED_DO_NOT_USE.h.bak  # STARY config - NIE UZYWAC!
|   |-- config_v130_DEPRECATED.h.bak        # STARY config - NIE UZYWAC!
|-- docs/                             # Dokumentacja
|   |-- DOKUMENTACJA_TECHNICZNA.md
|   |-- INSTRUKCJA_OBSLUGI.md
|   |-- INSTRUKCJA_OBSLUGI_v160_DODATEK.md
|   |-- SCHEMATY.md
|   |-- FUNKCJE.md
|   |-- REKOMENDACJE.md
|-- releases/                         # Pakiety produkcyjne
|-- platformio.ini                    # Konfiguracja PlatformIO
|-- README.md                         # Wprowadzenie
|-- CHANGELOG.md                      # Historia zmian
|-- CLAUDE.md                         # Ten plik
```

### Kluczowe Pliki

#### `src/main.cpp`
- **Funkcja**: Glowny plik programu
- **Zawiera**: `setup()`, `loop()`, logike malowania
- **Rozmiar**: ~900+ linii
- **Uwagi**: Nie modyfikuj struktury stanow bez pelnego zrozumienia

#### `src/config_v140_NEW.h` (GLOWNY PLIK KONFIGURACYJNY!)
- **Funkcja**: Centralna konfiguracja
- **Zawiera**: Mapowanie pinow GPIO, enumeracje, struktury
- **WAZNE**: To jest JEDYNY prawidlowy plik konfiguracyjny!
- **NIE UZYWAJ**: config.h, config_v130_OLD.h (sa zdeprecjonowane jako .bak)
- **Include guard**: `CONFIG_V140_NEW_H` (NIE `CONFIG_H`!)

#### `src/patterns.h`
- **Funkcja**: Definicje wzorcow malowania
- **Zawiera**: Tablice `PATTERNS[]` z 15 wzorcami
- **UWAGA**: Wzorce zgodne z normami - zmieniaj ostroznie!

#### `src/display_manager.*`
- **Klasa**: `DisplayManager`
- **Odpowiedzialnosc**: Wszystkie operacje na wyswietlaczu TFT
- **Optymalizacja**: TFT Sprites (double buffering) od v1.6.0
- **Uzywa**: Biblioteki TFT_eSPI

#### `src/encoder_handler.*`
- **Klasa**: `EncoderHandler`
- **Odpowiedzialnosc**: Pomiar odleglosci i predkosci
- **Przerwania**: Uzywa przerwan na GPIO
- **Precyzja**: +/-1cm po kalibracji
- **Thread-safety**: Mutex (od v1.4.2)

#### `src/dual_encoder_manager.*` (v1.6.0)
- **Klasa**: `DualEncoderManager`
- **Odpowiedzialnosc**: Zarzadzanie dwoma enkoderami (PRIMARY + BACKUP)
- **Funkcja**: Automatyczna detekcja awarii i przelaczanie
- **PRIMARY**: GPIO 32/33, **BACKUP**: GPIO 6/7

#### `src/relay_controller.*`
- **Klasa**: `RelayController`
- **Odpowiedzialnosc**: Sterowanie 6 przekaznikami
- **Bezpieczenstwo**: Natychmiastowe wylaczanie przy bledach

#### `src/menu_system.*`
- **Klasa**: `MenuSystem`
- **Odpowiedzialnosc**: UI menu, nawigacja joystickiem
- **Pozycje**: Kalibracja, Pomiar, Reset, Info, Serwis, Wyjscie

#### `src/calibration.*`
- **Klasa**: `CalibrationManager`
- **Odpowiedzialnosc**: Kalibracja enkodera na 10m
- **Pamiec**: Uzywa Preferences (EEPROM emulation)
- **Zabezpieczenia**: Magic number + checksum

#### `src/service_mode.*` (v1.4.1)
- **Klasa**: `ServiceMode`
- **Odpowiedzialnosc**: Tryb czyszczenia pistoletow
- **Funkcja**: Hold-to-fire - pistolety dzialaja tylko gdy przycisk jest wcisniety
- **Bezpieczenstwo**: Jedyny tryb gdzie pistolety dzialaja na postoju

#### `src/event_logger.*` (v1.5.0)
- **Klasa**: `EventLogger`
- **Odpowiedzialnosc**: Logowanie zdarzen systemowych
- **Bufor**: Ring buffer (100 zdarzen)
- **Typy**: SYSTEM_START, PATTERN_CHANGED, STATE_CHANGED, itd.

#### `src/sd_card_manager.*` (v1.6.0)
- **Klasa**: `SDCardManager`
- **Odpowiedzialnosc**: Zapis logow na karte SD
- **Format**: CSV, rotacja plikow
- **Auto-zapis**: Co 10 min lub 50 zdarzen

#### `src/wifi_server.*` (v1.4.0)
- **Klasa**: `WiFiServerManager`
- **Odpowiedzialnosc**: WiFi Access Point + REST API
- **SSID**: "Trassar", haslo: "12345678"
- **IP**: 192.168.4.1
- **Endpointy**: GET /, /status, /control, /pattern, /api/startfromgap

---

## KRYTYCZNE: Konfiguracja GPIO (v1.6.5)

### WAZNE - Uzywaj TYLKO `config_v140_NEW.h`!

Od wersji 1.6.5 stare pliki konfiguracyjne zostaly zdeprecjonowane:
- `config.h` -> `config_DEPRECATED_DO_NOT_USE.h.bak`
- `config_v130_OLD.h` -> `config_v130_DEPRECATED.h.bak`

**Przyczyna**: Konflikty include guards powodowaly crash (GPIO 227 error)!

### Mapowanie GPIO (v1.6.5)

```cpp
// Wyswietlacz ILI9341 (SPI) - NIE ZMIENIAC!
TFT_MISO = 19, TFT_MOSI = 23, TFT_SCLK = 18
TFT_CS = 5, TFT_DC = 22, TFT_RST = 21

// Enkoder PRIMARY
ENCODER_CLK_PIN = 32, ENCODER_DT_PIN = 33, ENCODER_SW_PIN = 13

// Enkoder BACKUP (v1.6.0)
ENCODER_BACKUP_CLK_PIN = 6, ENCODER_BACKUP_DT_PIN = 7
ENCODER_BACKUP_SW_PIN = 12  // ZMIENIONE v1.6.4 (bylo 19!)

// SD Card (v1.6.0)
SD_CS_PIN = 4  // SPI wspoldzielony z TFT

// Joystick
JOYSTICK_X_PIN = 34, JOYSTICK_Y_PIN = 35
SELECTOR_PIN = 20 (JOYSTICK_SW_PIN = 20)

// Przekazniki - NAPRAWIONE v1.5.0 (strapping pins!)
RELAY_1_PIN = 10, RELAY_2_PIN = 11  // ZMIENIONE z 12, 13
RELAY_3_PIN = 8,  RELAY_4_PIN = 9   // ZMIENIONE z 14, 15
RELAY_5_PIN = 16, RELAY_6_PIN = 17  // Bez zmian

// Przyciski sterowania
BTN_START_PIN = 0, BTN_STOP_PIN = 2
BTN_REVERSE_PIN = 14      // ZMIENIONE v1.6.4 (bylo 4 - konflikt z SD_CS!)
BTN_START_GAP_PIN = 46

// Przyciski wzorcow (P-1a do P-7d)
GPIO: 26, 27, 36-48, 3
```

### Konflikty GPIO - Historia

| Wersja | Problem | Rozwiazanie |
|--------|---------|-------------|
| v1.5.0 | GPIO 12-15 (strapping pins) blokowaly boot | Przekazniki 1-4 na GPIO 8-11 |
| v1.5.0 | GPIO 1 (UART TX) konflikt | BTN_P7D na GPIO 3 |
| v1.6.4 | GPIO 4 konflikt (REVERSE vs SD_CS) | BTN_REVERSE na GPIO 14 |
| v1.6.4 | GPIO 19 konflikt (BACKUP_SW vs TFT_MISO) | BACKUP_SW na GPIO 12 |
| v1.6.5 | Include guard conflict (CONFIG_H) | Unikalny guard CONFIG_V140_NEW_H |

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
STATE_ERROR       // Stan bledu (v1.4.0)
STATE_SERVICE     // Tryb serwisowy (v1.4.1)
```

### Przeplyw Stanow

```
START -> STATE_IDLE
         |
         |--[Przycisk wzorca + START]-> STATE_PAINTING
         |                                   |
         |                          [START/PAUZA]-> STATE_PAUSED
         |                                   |         |
         |                              [STOP]|   [START/PAUZA]
         |                                   |         |
         |                                   v         v
         |--[Dlugie STOP]-> STATE_MENU    STATE_PAINTING
         |                    |
         |               [Kalibracja]-> STATE_CALIBRATING
         |               [Pomiar]-> STATE_MEASURING
         |               [Serwis]-> STATE_SERVICE (v1.4.1)
         |               [Wyjscie]-> STATE_IDLE
         |
         |--[Blad]-> STATE_ERROR (v1.4.0)
         |
         |--[STOP]-> STATE_IDLE
```

### Glowna Petla (main.cpp -> loop())

```cpp
1. Aktualizacja enkodera (co 10ms)
   - DualEncoderManager sprawdza oba enkodery
   - Automatyczne przelaczenie przy awarii
   - Zliczanie impulsow, obliczanie predkosci

2. Sprawdzanie przyciskow
   - Przyciski wzorcow (P-1a do P-7d)
   - REVERSE (odwracanie P-3a/P-3b)
   - START GAP (start od przerwy)
   - START/PAUZA, STOP (krotkie/dlugie)

3. Obsluga stanu
   - MENU: MenuSystem::update()
   - CALIBRATING: CalibrationManager::process()
   - PAINTING: processPainting()
   - SERVICE: ServiceMode::update() (hold-to-fire)
   - MEASURING: tylko zliczanie
   - ERROR: wyswietlanie komunikatu

4. Obliczenia
   - calculatePaintedArea() - powierzchnia m2
   - Integer math (optymalizacja v1.4.0)

5. Logowanie (v1.5.0+)
   - EventLogger::log() dla zdarzen
   - SDCardManager auto-zapis (v1.6.0)

6. Aktualizacja UI (co 100ms)
   - DisplayManager::showMainScreen()
   - TFT Sprites (double buffering)

7. WiFi Server (v1.4.0)
   - WiFiServerManager::handleClient()

8. Delay(1) - odciazenie CPU
```

---

## Wzorce Malowania

### Struktura Pattern (rozszerzona v1.4.0)

```cpp
struct Pattern {
    PatternType type;      // Enum (PATTERN_P1A ... PATTERN_P7D)
    const char* name;      // "P-1a"
    float lineLength;      // Dlugosc linii (m), 0 = ciagla
    float gapLength;       // Dlugosc przerwy (m)
    uint8_t width;         // Szerokosc (12 lub 24 cm)
    const char* description;
    bool reversible;       // true dla P-3a, P-3b

    // Mapowanie pistoletow (v1.4.0)
    bool gun1, gun2, gun3, gun4, gun5, gun6;
    float realWidth;       // Rzeczywista szerokosc (cm)
};
```

### Logika Malowania (processPainting)

#### Optymalizacja Integer Math (v1.4.0)
```cpp
// Przed v1.4.0 (wolne):
float positionInCycle = fmod(distance_meters, cycleLength);

// Od v1.4.0 (5-10x szybsze):
long positionInCycleCm = effectiveDistanceCm % cycleLengthCm;
```

#### Start Gap (v1.3.0)
```cpp
if (systemState.startFromGap) {
    // Przesuniecie o dlugosc przerwy
    effectiveDistanceCm = distanceCm + systemState.offsetDistanceCm;
}
```

#### Wzorce Odwracalne (DZIALA od v1.4.0!)
```cpp
if (systemState.patternReversed && (PATTERN_P3A || PATTERN_P3B)) {
    // Zamiana pistoletow: P1<->P4, P2<->P5, P3<->P6
    swap(activeGuns[0], activeGuns[3]);
    swap(activeGuns[1], activeGuns[4]);
    swap(activeGuns[2], activeGuns[5]);
}
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

### Struktura Kodu - Include Guards

**KRYTYCZNE od v1.6.5**: Kazdy plik .h MUSI miec UNIKALNY include guard!

```cpp
// DOBRZE - unikalny guard
#ifndef NAZWA_PLIKU_H
#define NAZWA_PLIKU_H
// ...
#endif // NAZWA_PLIKU_H

// ZLE - konflikt z innym plikiem
#ifndef CONFIG_H  // Konflikt z config.h!
#define CONFIG_H
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
```

---

## WAZNE: Co TRZEBA Wiedziec

### 1. Plik Konfiguracyjny - TYLKO `config_v140_NEW.h`!

```cpp
// DOBRZE
#include "config_v140_NEW.h"

// ZLE - te pliki sa zdeprecjonowane!
#include "config.h"              // DEPRECATED!
#include "config_v130_OLD.h"     // DEPRECATED!
```

### 2. Thread-Safety (od v1.4.2)

```cpp
// Uzycie mutex dla krytycznych sekcji
if (encoderMutex != NULL && xSemaphoreTake(encoderMutex, portMAX_DELAY) == pdTRUE) {
    position++;  // Thread-safe!
    xSemaphoreGive(encoderMutex);
}

// Makra pomocnicze
LOCK_STATE();
systemState.distance = newValue;
UNLOCK_STATE();
```

### 3. Enkoder - Dual Encoder (v1.6.0)

```cpp
// DualEncoderManager automatycznie zarzadza dwoma enkoderami
dualEncoder.update();               // Sprawdza oba enkodery
long distance = dualEncoder.getDistance();  // Zwraca z aktywnego

// Automatyczne przelaczenie przy awarii PRIMARY
// Logi w Serial: "ENCODER: Switching to BACKUP!"
```

### 4. Wyswietlacz - TFT Sprites (v1.6.0)

```cpp
// DisplayManager automatycznie uzywa Sprites jesli PSRAM dostepny
// Double buffering: 3-5x szybsze odswiezanie (5-8ms vs 20-30ms)

// Fallback do tradycyjnego renderowania jesli brak PSRAM
// (wolniejsze, ale stabilne)
```

### 5. Preferences - Pamiec Nieulotna

```cpp
preferences.begin("calibration", false);
preferences.putBytes("caldata", &data, sizeof(data));
preferences.end();  // ZAWSZE zamykaj!
```

### 6. Przekazniki - Bezpieczenstwo

```cpp
// ZAWSZE wylaczaj przy bledzie
relays.stopAll();

// Minimalna predkosc do aktywacji: 2 km/h
if (!isSafeToActivateGuns()) {
    relays.stopAll();
}
```

### 7. Event Logger (v1.5.0)

```cpp
eventLogger.log(EVENT_PATTERN_CHANGED, oldPattern, newPattern, "Zmiana wzorca");
eventLogger.log(EVENT_SAFETY_TRIGGERED, speed*10, 1, "Blokada: predkosc za niska");
eventLogger.printToSerial();  // Wydruk wszystkich logow
```

### 8. SD Card (v1.6.0)

```cpp
sdCardManager.begin();  // Inicjalizacja
sdCardManager.logEvent(EVENT_SYSTEM_START, 0, 0);  // Zapis zdarzenia
// Auto-flush co 10 min lub 50 zdarzen
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
    3.0,    // linia
    1.5,    // przerwa
    12,     // szerokosc
    "Nowy wzorzec",
    false,  // odwracalny?
    true, true, true, false, false, false,  // gun1-6
    12.0    // realWidth
}
```

3. **Dodaj przycisk**:
- Zdefiniuj pin w config_v140_NEW.h: `#define BTN_P8A_PIN XX`
- Dodaj do tablicy PATTERN_BUTTONS w main.cpp
- Sprawdz czy pin nie koliduje z innymi!

4. **Zaktualizuj dokumentacje**

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
3. **Obsluz w handleSelection()**

### Modyfikacja WiFi API (v1.4.0)

```cpp
// wifi_server.cpp - dodaj nowy endpoint
if (request.indexOf("GET /api/myendpoint") >= 0) {
    sendJsonResponse(...);
}
```

---

## Kompilacja

### Standardowa Kompilacja

```bash
pio run
```

### WAZNE: Clean Rebuild (wymagany po zmianach GPIO!)

```bash
rm -rf .pio
pio run -t clean
pio run
```

### Upload

```bash
pio run -t upload
```

### Monitor Serial

```bash
pio device monitor  # 115200 baud
```

### Weryfikacja GPIO (v1.6.5)

W Serial Monitor przy starcie sprawdz:
```
--- DEBUG GPIO PINS ---
ENCODER PRIMARY: CLK=32 DT=33 SW=13
ENCODER BACKUP:  CLK=6 DT=7 SW=12     <- MUSI BYC 12, NIE 19!
BUTTONS: REVERSE=14 ...               <- MUSI BYC 14, NIE 4!
--- END GPIO DEBUG ---
```

Jesli widzisz GPIO 227 lub > 48 -> Clean rebuild nie zostal wykonany!

---

## Typowe Problemy i Rozwiazania

### Problem: "Guru Meditation Error" / GPIO 227

**Przyczyna**: Include guard conflict - preprocessor ignorowal config_v140_NEW.h

**Rozwiazanie** (v1.6.5):
1. Sprawdz czy wszystkie pliki .h includuja `config_v140_NEW.h` (NIE `config.h`)
2. Wykonaj clean rebuild: `rm -rf .pio && pio run`
3. Sprawdz logi GPIO przy starcie

### Problem: Predkosc ZAWSZE 0 km/h

**Przyczyna** (v1.4.1 bug): Obliczanie distanceDiff po zmianie systemState.distance

**Rozwiazanie** (v1.4.2+): Naprawione - zapisz oldDistance PRZED zmiana

### Problem: Pistolety nie dzialaja

**Sprawdz**:
1. Minimalna predkosc >= 2 km/h
2. Stan systemu = STATE_PAINTING
3. Przekazniki podlaczone do prawidlowych GPIO (v1.5.0+: 8-11, 16-17)

### Problem: Wyswietlacz "mruga"

**Przyczyna**: Zbyt czeste `tft->fillScreen()`

**Rozwiazanie**: Uzywaj DisplayManager (TFT Sprites od v1.6.0)

### Problem: Enkoder "gubi" impulsy

**Rozwiazanie**:
1. Sprawdz przerwania
2. Uzyj DualEncoderManager (v1.6.0) dla redundancji
3. Sprawdz mutex (v1.4.2+)

---

## Bezpieczenstwo i Best Practices

### 1. Uzywaj snprintf zamiast sprintf (v1.5.0)

```cpp
// DOBRZE
snprintf(buffer, sizeof(buffer), "%.1f", speed);

// ZLE - buffer overflow risk
sprintf(buffer, "%.1f", speed);
```

### 2. Uzywaj fabs() dla float (v1.4.2)

```cpp
// DOBRZE
if (fabs(speed - lastSpeed) > 0.1) { ... }

// ZLE - undefined behavior
if (abs(speed - lastSpeed) > 0.1) { ... }
```

### 3. Volatile dla zmiennych w ISR

```cpp
volatile bool interruptFlag = false;
volatile long position = 0;

void IRAM_ATTR encoderISR() {
    interruptFlag = true;
}
```

### 4. Sprawdzaj granice tablic

```cpp
if (relayNum >= 1 && relayNum <= 6) {
    digitalWrite(relayPins[relayNum - 1], state);
}
```

---

## Biblioteki i Zaleznosci

### platformio.ini

```ini
lib_deps =
    bodmer/TFT_eSPI@^2.5.43
    bblanchon/ArduinoJson@^6.21.4
    adafruit/Adafruit GFX Library@^1.11.9
```

### TFT_eSPI - Konfiguracja w build_flags

```ini
build_flags =
    -DUSER_SETUP_LOADED=1
    -DILI9341_DRIVER=1
    -DTFT_WIDTH=240
    -DTFT_HEIGHT=320
    -DTFT_MISO=19
    -DTFT_MOSI=23
    -DTFT_SCLK=18
    -DTFT_CS=5
    -DTFT_DC=22
    -DTFT_RST=21
```

### PSRAM (opcjonalnie, dla TFT Sprites)

```ini
; Odkomentuj jesli ESP32-S3 MA PSRAM (N16R8):
; board_build.psram_type = opi
; board_build.arduino.memory_type = qio_opi
```

---

## Checklist Przed Commitem

- [ ] Kod kompiluje sie bez ostrzezen
- [ ] Wykonano clean rebuild jesli zmieniono GPIO lub include
- [ ] Wszystkie pliki .h includuja `config_v140_NEW.h` (NIE `config.h`!)
- [ ] Dodane komentarze (po polsku) wyjasniajace "dlaczego"
- [ ] Zaktualizowane `CHANGELOG.md`
- [ ] Jesli zmiana pinow -> zaktualizuj `docs/SCHEMATY.md`
- [ ] Jesli nowe funkcje -> zaktualizuj `README.md`
- [ ] Debug logi uzywaja `DEBUG_PRINTF` (nie `Serial.print`)
- [ ] snprintf zamiast sprintf
- [ ] fabs() zamiast abs() dla float
- [ ] Mutex dla zmiennych wspoldzielonych z ISR
- [ ] Przerwania ISR maja `IRAM_ATTR`

---

## Kontakt i Wsparcie

### Dla AI Assistants

Jesli jako AI asystent napotykasz problemy:
1. Przeczytaj **caly** CHANGELOG.md (szczegolnie v1.4.0-v1.6.5)
2. Sprawdz `config_v140_NEW.h` dla pinow i stalych
3. Zobacz `main.cpp` dla logiki glownej
4. **ZAWSZE** includuj `config_v140_NEW.h`, NIGDY `config.h`!
5. Po zmianach GPIO -> clean rebuild!

### Pytania Czeste (dla AI)

**Q: Ktory plik konfiguracyjny uzywac?**
A: TYLKO `config_v140_NEW.h`! Stary `config.h` jest zdeprecjonowany.

**Q: Dlaczego GPIO 227 error?**
A: Include guard conflict. Sprawdz czy wszystkie .h includuja config_v140_NEW.h i wykonaj clean rebuild.

**Q: Jak dodac nowa funkcjonalnosc?**
A: Najpierw zrozum architekture. Sprawdz wersje w CHANGELOG. Dodaj kod w odpowiedniej klasie. Zaktualizuj dokumentacje.

**Q: Kod sie nie kompiluje**
A: 1) Clean rebuild, 2) Sprawdz include guards, 3) Sprawdz skladnie C++

---

**Wersja CLAUDE.md**: 1.6.5
**Data**: 2026-01-27
**Ostatnia aktualizacja**: 2026-01-27

---

## Slowo Koncowe dla AI Asystenta

Ten projekt to system embedded czasu rzeczywistego. **Bezpieczenstwo** i **niezawodnosc** sa najwazniejsze.

**KRYTYCZNE zasady**:
1. ZAWSZE uzywaj `config_v140_NEW.h`
2. Po zmianach GPIO -> clean rebuild
3. Sprawdzaj include guards (musza byc unikalne!)
4. Testuj dokladnie
5. Dokumentuj zmiany

Ten system bedzie uzywany w rzeczywistych warunkach drogowych.

**Koduj odpowiedzialnie!**

---

*Koniec dokumentu CLAUDE.md*
