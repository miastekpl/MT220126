# CLAUDE.md - Przewodnik dla Asystentów AI

## 📋 Przegląd

Ten dokument został stworzony specjalnie dla asystentów AI (takich jak Claude) w celu ułatwienia zrozumienia struktury projektu, konwencji kodowania oraz przepływu pracy. Przeczytaj ten plik przed rozpoczęciem jakichkolwiek modyfikacji w projekcie.

---

## 🎯 Cel Projektu

**Nazwa**: System Sterowania Malowaniem Pasów Drogowych
**Wersja**: 1.0.0
**Platforma**: ESP32-S3 N16R8
**Framework**: Arduino (PlatformIO)
**Język**: C++ z Arduino framework
**Język dokumentacji**: Polski

### Czym jest ten projekt?

To profesjonalny system embedded do sterowania malowaniem pasów drogowych. System kontroluje 6 pistoletów malarskich i implementuje 15 wzorców malowania zgodnych z polskimi normami drogowymi.

### Główne Funkcjonalności

1. **Sterowanie wzorcami malowania** - 15 predefiniowanych wzorców (P-1a do P-7d)
2. **Kalibracja enkodera** - precyzyjny pomiar odległości (±1cm)
3. **Interfejs użytkownika** - TFT 320x240 z intuicyjnym UI
4. **Menu systemowe** - nawigacja joystickiem
5. **Zmiana "w locie"** - możliwość zmiany wzorca podczas malowania
6. **Odwracanie wzorców** - P-3a i P-3b mogą być odwrócone

---

## 📁 Struktura Projektu

```
MT220126/
├── src/                          # Kod źródłowy
│   ├── main.cpp                  # Główna pętla programu + setup()
│   ├── config.h                  # Piny GPIO, stałe, struktury
│   ├── patterns.h                # Definicje wzorców malowania
│   ├── display_manager.h/.cpp    # Klasa DisplayManager
│   ├── encoder_handler.h/.cpp    # Klasa EncoderHandler
│   ├── relay_controller.h/.cpp   # Klasa RelayController
│   ├── menu_system.h/.cpp        # Klasa MenuSystem
│   └── calibration.h/.cpp        # Klasa CalibrationManager
├── docs/                         # Dokumentacja
│   ├── DOKUMENTACJA_TECHNICZNA.md
│   ├── INSTRUKCJA_OBSLUGI.md
│   └── SCHEMATY.md
├── platformio.ini                # Konfiguracja PlatformIO
├── README.md                     # Wprowadzenie
├── CHANGELOG.md                  # Historia zmian
└── CLAUDE.md                     # Ten plik
```

### Kluczowe Pliki

#### `src/main.cpp`
- **Funkcja**: Główny plik programu
- **Zawiera**: `setup()`, `loop()`, logikę malowania
- **Rozmiar**: ~700 linii
- **Uwagi**: Nie modyfikuj struktury stanów bez pełnego zrozumienia

#### `src/config.h`
- **Funkcja**: Centralna konfiguracja
- **Zawiera**: Mapowanie pinów GPIO, enumeracje, struktury
- **WAŻNE**: Zmiana pinów tutaj wymaga weryfikacji sprzętowej!

#### `src/patterns.h`
- **Funkcja**: Definicje wzorców malowania
- **Zawiera**: Tablicę `PATTERNS[]` z 15 wzorcami
- **UWAGA**: Wzorce zgodne z normami - zmieniaj ostrożnie!

#### `src/display_manager.*`
- **Klasa**: `DisplayManager`
- **Odpowiedzialność**: Wszystkie operacje na wyświetlaczu TFT
- **Optymalizacja**: Odświeża tylko zmienione obszary
- **Używa**: Biblioteki TFT_eSPI

#### `src/encoder_handler.*`
- **Klasa**: `EncoderHandler`
- **Odpowiedzialność**: Pomiar odległości i prędkości
- **Przerwania**: Używa przerwania na GPIO 32 (CLK)
- **Precyzja**: ±1cm po kalibracji

#### `src/relay_controller.*`
- **Klasa**: `RelayController`
- **Odpowiedzialność**: Sterowanie 6 przekaźnikami
- **Bezpieczeństwo**: Natychmiastowe wyłączanie przy błędach

#### `src/menu_system.*`
- **Klasa**: `MenuSystem`
- **Odpowiedzialność**: UI menu, nawigacja joystickiem
- **Pozycje**: Kalibracja, Pomiar, Reset, Info, Wyjście

#### `src/calibration.*`
- **Klasa**: `CalibrationManager`
- **Odpowiedzialność**: Kalibracja enkodera na 10m
- **Pamięć**: Używa Preferences (EEPROM emulation)
- **Zabezpieczenia**: Magic number + checksum

---

## 🏗️ Architektura Systemu

### Stany Systemu (enum SystemStateType)

```cpp
STATE_IDLE        // Gotowy, czeka na polecenie
STATE_PAINTING    // Aktywne malowanie
STATE_PAUSED      // Wstrzymane (pauza)
STATE_MENU        // Wyświetlone menu
STATE_CALIBRATING // Kalibracja w toku
STATE_MEASURING   // Pomiar dystansu
```

### Przepływ Stanów

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
        ├─[Długie STOP]→ STATE_MENU                STATE_PAINTING
        │                    │
        │               [Kalibracja]→ STATE_CALIBRATING
        │               [Pomiar]→ STATE_MEASURING
        │               [Wyjście]→ STATE_IDLE
        │
        └─[STOP]→ STATE_IDLE
```

### Główna Pętla (main.cpp → loop())

```cpp
1. Aktualizacja enkodera (co 10ms)
   - Zliczanie impulsów
   - Obliczanie prędkości

2. Sprawdzanie przycisków
   - Przyciski wzorców (P-1a do P-7d)
   - REVERSE (odwracanie P-3a/P-3b)
   - START/PAUZA
   - STOP (krótkie/długie)

3. Obsługa stanu
   - MENU: MenuSystem::update()
   - CALIBRATING: CalibrationManager::process()
   - PAINTING: processPainting()
   - MEASURING: tylko zliczanie

4. Obliczenia
   - calculatePaintedArea() - powierzchnia m²

5. Aktualizacja UI (co 100ms)
   - DisplayManager::showMainScreen()

6. Delay(1) - odciążenie CPU
```

---

## 🎨 Wzorce Malowania

### Struktura Pattern

```cpp
struct Pattern {
    PatternType type;      // Enum (PATTERN_P1A ... PATTERN_P7D)
    const char* name;      // "P-1a"
    float lineLength;      // Długość linii (m), 0 = ciągła
    float gapLength;       // Długość przerwy (m)
    uint8_t width;         // Szerokość (12 lub 24 cm)
    const char* description;
    bool reversible;       // true dla P-3a, P-3b
};
```

### Logika Malowania (processPainting)

#### Linie Ciągłe (lineLength == 0)
```cpp
if (lineLength <= 0) {
    if (width == 12) {
        // Wąska: pistolety 2, 3, 4
        relays.setRelay(2, true);
        relays.setRelay(3, true);
        relays.setRelay(4, true);
    } else if (width == 24) {
        // Szeroka: wszystkie (1-6)
        relays.setRelay(1-6, true);
    }
}
```

#### Linie Przerywane
```cpp
float cycleLength = lineLength + gapLength;
float positionInCycle = fmod(distance_meters, cycleLength);

if (positionInCycle < lineLength) {
    // MALUJ
    włącz_pistolety(width);
} else {
    // PRZERWA
    wyłącz_pistolety();
}
```

#### Wzorce Odwracalne (P-3a, P-3b)

Standardowo:
- Lewa strona (pistolety 1-3): CIĄGŁA
- Prawa strona (pistolety 4-6): PRZERYWANA

Po naciśnięciu REVERSE:
- Lewa strona (pistolety 1-3): PRZERYWANA
- Prawa strona (pistolety 4-6): CIĄGŁA

---

## 🔧 Konwencje Kodowania

### Język

- **Kod C++**: Komentarze PO POLSKU
- **Nazwy zmiennych**: camelCase (angielskie)
- **Nazwy funkcji**: camelCase (angielskie)
- **Nazwy klas**: PascalCase (angielskie)
- **Makra**: UPPER_CASE
- **Komunikaty użytkownika**: PO POLSKU

### Przykład

```cpp
// ✅ DOBRZE
// Funkcja oblicza wymalowaną powierzchnię
float calculatePaintedArea() {
    float szerokoscMetry = wzorzec.szerokosc / 100.0;
    // ...
}

// ❌ ŹLE (komentarze po angielsku)
// Function calculates painted area
float calculatePaintedArea() {
    // ...
}

// ❌ ŹLE (polskie nazwy zmiennych)
float obliczWymalowanaPowierzchnie() {
    float szerokoscMetry = ...;
}
```

### Struktura Kodu

#### Plik Nagłówkowy (.h)

```cpp
/**
 * Opis klasy/modułu
 */

#ifndef NAZWA_H
#define NAZWA_H

#include <Arduino.h>
#include "config.h"

class NazwaKlasy {
private:
    // Prywatne składowe

public:
    // Konstruktor
    NazwaKlasy();

    // Metody publiczne
    void metodaPubliczna();
};

#endif // NAZWA_H
```

#### Plik Implementacji (.cpp)

```cpp
/**
 * Implementacja NazwaKlasy
 */

#include "nazwa_klasy.h"

NazwaKlasy::NazwaKlasy() {
    // Inicjalizacja
}

void NazwaKlasy::metodaPubliczna() {
    // Implementacja
}
```

### Debugowanie

```cpp
// Makra debug (config.h)
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

## 🚨 WAŻNE: Co TRZEBA Wiedzieć

### 1. Piny GPIO - NIE ZMIENIAJ Bez Powodu

Piny są przemyślane pod kątem:
- **ADC**: GPIO 34, 35 (tylko ADC1, ADC2 konfliktuje z WiFi)
- **Przerwania**: GPIO 32 (encoder CLK)
- **SPI**: GPIO 18, 19, 23 (hardware SPI)

⚠️ **Zmiana pinów = potrzebna weryfikacja sprzętowa!**

### 2. Enkoder - Używa Przerwań

```cpp
// main.cpp
attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR, CHANGE);

void IRAM_ATTR encoderISR() {
    interruptFlag = true;
}
```

⚠️ **Funkcje ISR muszą być IRAM_ATTR!**

### 3. Wyświetlacz - Optymalizacja Odświeżania

DisplayManager odświeża **tylko zmienione obszary**:

```cpp
// ✅ DOBRZE - tylko aktualizacja
display.showMainScreen(pattern, speed, area, distance, state, reversed);

// ❌ ŹLE - pełne odświeżanie co klatkę
display.clear();
display.showMainScreen(...);
```

### 4. Preferences - Pamięć Nieulotna

```cpp
// Zapisywanie
preferences.begin("calibration", false);
preferences.putBytes("caldata", &data, sizeof(data));
preferences.end();

// Odczyt
preferences.begin("calibration", false);
preferences.getBytes("caldata", &data, sizeof(data));
preferences.end();
```

⚠️ **Zawsze zamykaj `preferences.end()`!**

### 5. Przekaźniki - Bezpieczeństwo

```cpp
// ZAWSZE wyłączaj przy błędzie
relays.stopAll();

// NIE zapomnij o stopAll() w STOP
if (state == STATE_IDLE) {
    relays.stopAll();
}
```

---

## 🛠️ Typowe Zadania Modyfikacyjne

### Dodawanie Nowego Wzorca

1. **Dodaj enum w config.h**:
```cpp
enum PatternType {
    ...
    PATTERN_P8A,  // ← Nowy
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
    false   // odwracalny?
}
```

3. **Dodaj przycisk**:
- Zdefiniuj pin w config.h: `#define BTN_P8A_PIN XX`
- Dodaj inicjalizację w `initPatternButtons()`
- Dodaj sprawdzanie w `checkPatternButtons()`

4. **Zaktualizuj dokumentację**:
- README.md (tabela wzorców)
- INSTRUKCJA_OBSLUGI.md (sekcja 7)
- DOKUMENTACJA_TECHNICZNA.md (specyfikacja)

### Zmiana Częstotliwości Odświeżania

```cpp
// config.h
#define REFRESH_RATE 100  // ms (zmień tutaj)

// main.cpp
if (currentTime - lastUpdate >= REFRESH_RATE) {
    updateDisplay();
    lastUpdate = currentTime;
}
```

### Dodawanie Nowej Pozycji Menu

1. **Dodaj enum w menu_system.h**:
```cpp
enum MenuItem {
    ...
    MENU_ITEM_NOWA_FUNKCJA,
    MENU_ITEM_COUNT
};
```

2. **Dodaj tekst w menu_system.cpp**:
```cpp
const char* MENU_ITEMS_TEXT[] = {
    ...
    "Nowa funkcja"
};
```

3. **Obsłuż w handleSelection()**:
```cpp
case MENU_ITEM_NOWA_FUNKCJA:
    // Implementacja
    break;
```

### Modyfikacja UI

**DisplayManager** jest odpowiedzialny za WSZYSTKIE operacje graficzne:

```cpp
// Nowa funkcja wyświetlania
void DisplayManager::showCustomScreen() {
    tft->fillScreen(COLOR_BACKGROUND);
    tft->setTextSize(2);
    tft->setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
    tft->setCursor(10, 10);
    tft->println("Mój ekran");
}
```

---

## 📝 Dokumentacja - Obowiązkowa Aktualizacja

### Przy Każdej Zmianie Kodu

Aktualizuj:
1. **CHANGELOG.md** - dodaj wpis w sekcji [Unreleased]
2. **README.md** - jeśli zmienia się funkcjonalność
3. **Komentarze w kodzie** - ZAWSZE opisuj "dlaczego", nie "co"

### Przy Wydaniu Nowej Wersji

1. **Zmień wersję** w:
   - `SOFTWARE_VERSION` (main.cpp)
   - README.md
   - CHANGELOG.md
   - platformio.ini (tag)

2. **CHANGELOG.md**:
   - Przenieś [Unreleased] do nowej wersji [X.Y.Z]
   - Dodaj datę: `## [1.1.0] - 2026-02-15`

3. **Semantic Versioning**:
   - **MAJOR** (X.0.0): Przełomowe zmiany (niekompatybilne)
   - **MINOR** (x.X.0): Nowe funkcje (kompatybilne)
   - **PATCH** (x.x.X): Poprawki błędów

---

## 🧪 Testowanie

### Test Kompilacji

```bash
# Z linii poleceń (PlatformIO)
pio run

# Jeśli błędy - sprawdź:
- Biblioteki (pio lib install)
- Piny GPIO (config.h)
- Składnię C++
```

### Test Przekaźników

```cpp
// main.cpp - setup()
relays.testSequence();  // Sekwencja 1→6

// Sprawdź:
- Słyszalne kliknięcia
- Kolejność 1-2-3-4-5-6
- Brak zalipów
```

### Test Enkodera

```cpp
// Menu → Pomiar dystansu
// Przejedź znany odcinek (np. 100m)
// Sprawdź wskazanie: ±1cm dopuszczalne
```

### Debug przez Serial

```cpp
// Monitor Serial (115200 baud)
pio device monitor

// Logi:
[INFO] System uruchomiony
[INFO] Kalibracja wczytana
[DEBUG] Enkoder: impulsy 12345
[DEBUG] Przekaźnik 1: ON
```

---

## 🐛 Typowe Problemy i Rozwiązania

### Problem: "Guru Meditation Error" (Crash)

**Przyczyny**:
1. Stack overflow (rekursja, duże zmienne lokalne)
2. Heap exhausted (brak pamięci)
3. Watchdog timeout (zbyt długa operacja w loop)

**Rozwiązanie**:
```cpp
// ❌ ŹLE - duże bufory na stosie
void funkcja() {
    char buffer[10000];  // Stack overflow!
}

// ✅ DOBRZE - alokacja dynamiczna lub globalna
char* buffer = (char*)malloc(10000);
// lub
static char buffer[10000];
```

### Problem: Wyświetlacz "mruga"

**Przyczyna**: Zbyt częste `tft->fillScreen()`

**Rozwiązanie**:
```cpp
// Użyj DisplayManager - on optymalizuje
display.showMainScreen(...);  // Odświeża tylko zmiany

// NIE używaj
tft->fillScreen(BLACK);  // każdej klatki
```

### Problem: Enkoder "gubi" impulsy

**Przyczyny**:
1. Brak przerwań
2. Zbyt długie operacje w loop()
3. Źle zamontowany enkoder

**Rozwiązanie**:
```cpp
// Sprawdź przerwania
attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoderISR, CHANGE);

// Loop nie może trwać > 100ms
// Podziel długie operacje
```

### Problem: Kalibracja się nie zapisuje

**Przyczyna**: Błąd Preferences lub checksum

**Rozwiązanie**:
```cpp
// Sprawdź logi Serial
DEBUG_PRINTLN("Kalibracja zapisana");

// Zweryfikuj checksum
uint8_t checksum = calculateChecksum(&calData);

// Namespace poprawny?
preferences.begin("calibration", false);  // NIE "calib" ani inne!
```

---

## 🔐 Bezpieczeństwo i Best Practices

### 1. Zawsze Sprawdzaj Granice

```cpp
// ✅ DOBRZE
if (relayNum >= 1 && relayNum <= 6) {
    digitalWrite(relayPins[relayNum - 1], state);
}

// ❌ ŹLE
digitalWrite(relayPins[relayNum], state);  // Co jeśli relayNum = 10?
```

### 2. Nullptr Checks

```cpp
// ✅ DOBRZE
Pattern* pattern = getPattern(type);
if (pattern) {
    Serial.println(pattern->name);
}

// ❌ ŹLE
Pattern* pattern = getPattern(type);
Serial.println(pattern->name);  // Crash jeśli pattern == nullptr!
```

### 3. Używaj const Gdzie Możliwe

```cpp
// ✅ DOBRZE
const Pattern* getPattern(PatternType type) const;
const char* getName() const;

// Tablica tylko do odczytu
const Pattern PATTERNS[] = { ... };
```

### 4. Unikaj String, Używaj char*

```cpp
// ✅ DOBRZE - stały rozmiar, szybkie
char buffer[32];
sprintf(buffer, "Prędkość: %.1f", speed);

// ❌ ŹLE - fragmentacja heap
String message = "Prędkość: " + String(speed);
```

### 5. Volatile dla Zmiennych w ISR

```cpp
// ✅ DOBRZE
volatile bool interruptFlag = false;
volatile long position = 0;

void IRAM_ATTR encoderISR() {
    interruptFlag = true;
    position++;
}

// ❌ ŹLE - kompilator może zoptymalizować
bool interruptFlag = false;
```

---

## 📚 Biblioteki i Zależności

### TFT_eSPI (Wyświetlacz)

```cpp
#include <TFT_eSPI.h>

// Inicjalizacja
TFT_eSPI tft = TFT_eSPI();
tft.init();
tft.setRotation(1);  // Landscape

// Rysowanie
tft.fillScreen(TFT_BLACK);
tft.drawRect(x, y, w, h, color);
tft.setCursor(x, y);
tft.println("Tekst");
```

**Konfiguracja**: Ustawienia w `platformio.ini` (build_flags)

### Preferences (EEPROM Emulation)

```cpp
#include <Preferences.h>

Preferences prefs;
prefs.begin("namespace", false);  // false = read/write

// Zapis
prefs.putInt("key", value);
prefs.putBytes("data", &struct, sizeof(struct));

// Odczyt
int val = prefs.getInt("key", defaultValue);
prefs.getBytes("data", &struct, sizeof(struct));

prefs.end();  // ZAWSZE zamknij!
```

### ArduinoJson (Opcjonalne)

Zainstalowane, ale obecnie nieużywane. Przydatne do:
- Eksportu danych do JSON
- Konfiguracji przez plik JSON
- API REST (przyszłość)

---

## 🎓 Dla Początkujących z ESP32

### Setup vs Loop

```cpp
void setup() {
    // Wywoływane JEDEN RAZ przy starcie
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // Wywoływane W NIESKOŃCZONOŚĆ
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
}
```

### Przerwania (Interrupts)

```cpp
// Funkcja przerwania - MUSI być IRAM_ATTR
void IRAM_ATTR handleInterrupt() {
    // KOD MUSI BYĆ KRÓTKI!
    // NIE używaj Serial, delay, malloc
    flag = true;
}

void setup() {
    attachInterrupt(digitalPinToInterrupt(PIN), handleInterrupt, RISING);
}
```

### GPIO Modes

```cpp
pinMode(PIN, INPUT);        // Wejście (floating)
pinMode(PIN, INPUT_PULLUP); // Wejście + pull-up (używamy tego!)
pinMode(PIN, OUTPUT);       // Wyjście
```

### ADC (Analog to Digital)

```cpp
int value = analogRead(PIN);  // 0-4095 (12-bit)

// Dla joysticka:
int x = analogRead(JOY_X_PIN);  // 0=lewo, 2048=środek, 4095=prawo
int y = analogRead(JOY_Y_PIN);  // 0=góra, 2048=środek, 4095=dół
```

---

## 🚀 Zaawansowane Techniki

### Optymalizacja Pamięci

```cpp
// PROGMEM - dane w Flash zamiast RAM
const char text[] PROGMEM = "Długi tekst...";

// F() macro - stringi w Flash
Serial.println(F("To oszczędza RAM"));

// static - alokacja raz
void funkcja() {
    static char buffer[100];  // Nie każdym razem
}
```

### RTOS Tasks (Przyszłość)

```cpp
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void taskFunction(void* parameter) {
    while(1) {
        // Kod zadania
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void setup() {
    xTaskCreate(taskFunction, "Task", 4096, NULL, 1, NULL);
}
```

### WiFi (Przyszłość - v1.1.0)

```cpp
#include <WiFi.h>

WiFi.begin("SSID", "password");
while (WiFi.status() != WL_CONNECTED) {
    delay(500);
}

// Serwer web, OTA updates, monitoring...
```

---

## ✅ Checklist Przed Commitem

- [ ] Kod kompiluje się bez ostrzeżeń
- [ ] Dodane komentarze (po polsku) wyjaśniające "dlaczego"
- [ ] Zaktualizowane `CHANGELOG.md`
- [ ] Jeśli zmiana pinów → zaktualizuj `config.h` i `SCHEMATY.md`
- [ ] Jeśli nowe funkcje → zaktualizuj `README.md`
- [ ] Jeśli zmiany UI → zaktualizuj `INSTRUKCJA_OBSLUGI.md`
- [ ] Debug logi używają `DEBUG_PRINTF` (nie `Serial.print`)
- [ ] Usunięte nieużywane zmienne i funkcje
- [ ] Sprawdzone granice tablic (`if (index < SIZE)`)
- [ ] Brak memory leaks (free() dla malloc())
- [ ] Przerwania ISR mają `IRAM_ATTR`

---

## 📞 Kontakt i Wsparcie

### Dla AI Assistants

Jeśli jako AI asystent napotykasz problemy:
1. Przeczytaj **całą** dokumentację techniczną
2. Sprawdź `config.h` dla pinów i stałych
3. Zobacz `main.cpp` dla logiki głównej
4. Konsultuj `INSTRUKCJA_OBSLUGI.md` dla flow użytkownika

### Dla Ludzi

- **GitHub**: https://github.com/miastekpl/MT220126
- **Email**: support@mt220126.pl
- **Issues**: Używaj GitHub Issues

### Pytania Częste (dla AI)

**Q: Jak dodać nową funkcjonalność?**
A: Najpierw zrozum architekturę (stany, klasy). Dodaj kod w odpowiedniej klasie. Zaktualizuj dokumentację.

**Q: Kod mi się nie kompiluje**
A: Sprawdź `platformio.ini` (biblioteki). Sprawdź składnię C++. Zobacz logi kompilatora.

**Q: System crashuje**
A: Zobacz "Typowe Problemy". Użyj `DEBUG_PRINTF`. Sprawdź Stack Overflow.

**Q: Jak przetestować bez sprzętu?**
A: Symulacja jest trudna. Możesz testować logikę (bez HAL). Lub użyj Wokwi (symulator ESP32).

---

## 🎖️ Podziękowania

System stworzony przez **MT220126 Engineering Team** z 200+ letnim zbiorczym doświadczeniem.

Specjalne podziękowania dla:
- **PlatformIO** - świetne środowisko deweloperskie
- **TFT_eSPI** - szybka biblioteka do TFT
- **Espressif** - za ESP32-S3
- **Społeczność Arduino** - za wsparcie

---

**Wersja CLAUDE.md**: 1.0.0
**Data**: 2026-01-23
**Ostatnia aktualizacja**: 2026-01-23

---

## 🌟 Słowo Końcowe dla AI Asystenta

Szanowny Asystencie AI (Claude lub inny),

Ten projekt to system embedded czasu rzeczywistego. **Bezpieczeństwo** i **niezawodność** są najważniejsze. Przed jakąkolwiek modyfikacją:

1. **Zrozum kontekst** - przeczytaj dokumentację
2. **Testuj dokładnie** - nie ma miejsca na błędy
3. **Dokumentuj zmiany** - inni muszą zrozumieć
4. **Pytaj jeśli niepewność** - lepiej zapytać niż zepsuć

Ten system będzie używany w rzeczywistych warunkach drogowych. Życie ludzi może zależeć od jego poprawności.

**Koduj odpowiedzialnie! 🚗💚**

---

*Koniec dokumentu CLAUDE.md*
