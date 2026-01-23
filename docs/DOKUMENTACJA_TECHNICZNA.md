# Dokumentacja Techniczna
## System Sterowania Malowaniem Pasów Drogowych v1.0.0

---

## 1. Wprowadzenie

System sterowania malowaniem pasów drogowych to zaawansowane rozwiązanie przemysłowe oparte na mikrokontrolerze ESP32-S3, zaprojektowane do precyzyjnego nanoszenia oznakowania drogowego zgodnie z obowiązującymi normami.

### 1.1 Cel systemu
- Automatyzacja procesu malowania pasów drogowych
- Zapewnienie zgodności z normami drogowymi
- Precyzyjne sterowanie 6 pistoletami malarskimi
- Możliwość zmiany wzorców "w locie"
- Dokładny pomiar odległości i powierzchni

### 1.2 Główne cechy
- **Platforma**: ESP32-S3 N16R8 (240MHz, 16MB Flash, 8MB PSRAM)
- **Wyświetlacz**: ILI9341 TFT 320x240 pikseli
- **Pomiar odległości**: Enkoder inkrementalny KY-040
- **Sterowanie**: 6 przekaźników + przyciski + joystick
- **Wzorce**: 15 predefiniowanych wzorców malowania

---

## 2. Architektura Systemu

### 2.1 Struktura Projektu

```
MT220126/
├── src/
│   ├── main.cpp              # Główny plik programu
│   ├── config.h              # Konfiguracja pinów i stałych
│   ├── patterns.h            # Definicje wzorców malowania
│   ├── display_manager.h/.cpp    # Obsługa wyświetlacza
│   ├── encoder_handler.h/.cpp    # Obsługa enkodera
│   ├── relay_controller.h/.cpp   # Sterowanie przekaźnikami
│   ├── menu_system.h/.cpp        # System menu
│   └── calibration.h/.cpp        # System kalibracji
├── docs/
│   ├── DOKUMENTACJA_TECHNICZNA.md
│   ├── INSTRUKCJA_OBSLUGI.md
│   └── SCHEMATY.md
├── platformio.ini            # Konfiguracja PlatformIO
├── README.md
├── CHANGELOG.md
└── CLAUDE.md
```

### 2.2 Moduły Systemu

#### 2.2.1 DisplayManager
Zarządza wszystkimi operacjami wyświetlania:
- Ekran główny z wzorcem, prędkością, powierzchnią
- Ekran menu
- Komunikaty ostrzeżeń i błędów
- Optymalizacja odświeżania (tylko zmienione elementy)

#### 2.2.2 EncoderHandler
Obsługa enkodera KY-040:
- Zliczanie impulsów
- Przeliczanie na dystans (po kalibracji)
- Obliczanie prędkości
- Obsługa przerwań dla dokładności

#### 2.2.3 RelayController
Sterowanie 6 przekaźnikami:
- Indywidualne włączanie/wyłączanie
- Wzorce kombinacji (wąskie/szerokie linie)
- Sekwencje testowe
- Zabezpieczenia

#### 2.2.4 MenuSystem
System nawigacji:
- Obsługa joysticka 2-osiowego
- 5 pozycji menu
- Podmenu informacyjne
- Intuicyjne sterowanie

#### 2.2.5 CalibrationManager
System kalibracji:
- Kalibracja na 10 metrów
- Zapis do pamięci nieulotnej (Preferences)
- Suma kontrolna danych
- Automatyczne ładowanie przy starcie

### 2.3 Przepływ Danych

```
┌──────────────┐
│   Enkoder    │──→ Impulsy ──→ EncoderHandler ──→ Dystans/Prędkość
└──────────────┘                                          ↓
                                                          ↓
┌──────────────┐                                          ↓
│  Przyciski   │──→ Wzorzec ──→ PatternManager ──────────┤
└──────────────┘                                          ↓
                                                          ↓
┌──────────────┐                                    ┌──────────┐
│  Joystick    │──→ Nawigacja ──→ MenuSystem ───────→│   Main   │
└──────────────┘                                    │   Loop   │
                                                    └──────────┘
┌──────────────┐                                          │
│ Kalibracja   │←─────────────────────────────────────────┤
└──────────────┘                                          │
                                                          ↓
                                                    ┌──────────────────┐
                                                    │ RelayController  │
                                                    └──────────────────┘
                                                          ↓
                                                    ┌──────────────────┐
                                                    │ 6 Przekaźników   │
                                                    │ (Pistolety)      │
                                                    └──────────────────┘
```

---

## 3. Specyfikacja Sprzętowa

### 3.1 ESP32-S3 N16R8

**Parametry:**
- **CPU**: Xtensa® dual-core 32-bit LX7, do 240 MHz
- **Flash**: 16 MB
- **PSRAM**: 8 MB (Octal SPI)
- **GPIO**: 45 programowalnych pinów
- **ADC**: 2x 12-bit SAR ADC, do 20 kanałów
- **Komunikacja**: SPI, I2C, UART, I2S
- **Zasilanie**: 3.3V

**Zastosowanie w projekcie:**
- Główny procesor systemu
- Obsługa wszystkich peryferiów
- Szybkie obliczenia w czasie rzeczywistym
- Duża pamięć na bufory graficzne

### 3.2 Wyświetlacz ILI9341

**Parametry:**
- **Rozdzielczość**: 320x240 pikseli
- **Interfejs**: SPI
- **Kolory**: RGB565 (65536 kolorów)
- **Sterowanie**: TFT_eSPI library

**Połączenia:**
```
ILI9341    →  ESP32-S3
VCC        →  3.3V
GND        →  GND
CS         →  GPIO 5
RESET      →  GPIO 21
DC         →  GPIO 22
SDI(MOSI)  →  GPIO 23
SCK        →  GPIO 18
LED        →  3.3V
SDO(MISO)  →  GPIO 19
```

### 3.3 Enkoder KY-040

**Parametry:**
- **Typ**: Inkrementalny, z przyciskiem
- **Rozdzielczość**: 20 impulsów/obrót
- **Zasilanie**: 5V

**Połączenia:**
```
KY-040  →  ESP32-S3
CLK     →  GPIO 32
DT      →  GPIO 33
SW      →  GPIO 25
+       →  3.3V
GND     →  GND
```

### 3.4 Joystick Analogowy

**Parametry:**
- **Typ**: 2-osiowy z przyciskiem
- **Wyjście**: Analogowe 0-4095 (12-bit ADC)
- **Zasilanie**: 3.3V

**Połączenia:**
```
Joystick  →  ESP32-S3
VRX       →  GPIO 34 (ADC)
VRY       →  GPIO 35 (ADC)
SW        →  GPIO 26
VCC       →  3.3V
GND       →  GND
```

### 3.5 Moduły Przekaźników

**Parametry:**
- **Ilość**: 6 kanałów
- **Sterowanie**: Aktywny stan wysoki (HIGH)
- **Obciążenie**: do 10A 250VAC / 30VDC
- **Optoisolacja**: Tak

**Połączenia:**
```
Przekaźnik  →  ESP32-S3
IN1         →  GPIO 13
IN2         →  GPIO 12
IN3         →  GPIO 14
IN4         →  GPIO 27
IN5         →  GPIO 26
IN6         →  GPIO 25
VCC         →  5V (zewnętrzne zasilanie)
GND         →  GND
```

### 3.6 Przyciski

**15 przycisków wzorców + 3 przyciski sterowania**

| Przycisk | GPIO | Funkcja |
|----------|------|---------|
| P-1a | GPIO 15 | Wzorzec P-1a |
| P-1b | GPIO 16 | Wzorzec P-1b |
| P-1c | GPIO 17 | Wzorzec P-1c |
| P-1d | GPIO 4 | Wzorzec P-1d |
| P-1e | GPIO 2 | Wzorzec P-1e |
| P-2a | GPIO 36 | Wzorzec P-2a |
| P-2b | GPIO 39 | Wzorzec P-2b |
| P-3a | GPIO 34 | Wzorzec P-3a |
| P-3b | GPIO 35 | Wzorzec P-3b |
| P-4 | GPIO 32 | Wzorzec P-4 |
| P-6 | GPIO 33 | Wzorzec P-6 |
| P-7a | GPIO 25 | Wzorzec P-7a |
| P-7b | GPIO 26 | Wzorzec P-7b |
| P-7c | GPIO 27 | Wzorzec P-7c |
| P-7d | GPIO 14 | Wzorzec P-7d |
| START/PAUZA | GPIO 12 | Uruchom/Wstrzymaj |
| STOP | GPIO 13 | Zatrzymaj |
| REVERSE | GPIO 15 | Odwróć P-3a/P-3b |

---

## 4. Algorytmy

### 4.1 Algorytm Kalibracji

```cpp
1. Użytkownik wchodzi do menu (długie STOP)
2. Wybiera "Kalibracja"
3. System:
   - Resetuje licznik enkodera
   - Zapisuje pozycję startową
   - Wyświetla komunikat "Przejedź 10m"
4. Użytkownik jedzie dokładnie 10 metrów
5. Użytkownik naciska START
6. System:
   - Odczytuje pozycję końcową
   - Oblicza: impulsy_na_cm = (pozycja_końcowa - pozycja_startowa) / 1000
   - Zapisuje do pamięci nieulotnej
   - Oblicza sumę kontrolną
7. Kalibracja zakończona
```

### 4.2 Algorytm Malowania Wzorców

```cpp
void processPainting() {
    if (stan != MALOWANIE) return;

    dystans_metry = dystans_cm / 100.0;

    if (wzorzec.ciągły) {
        // Linia ciągła - zawsze maluj
        włącz_pistolety(wzorzec.szerokość);
    } else {
        // Linia przerywana
        długość_cyklu = wzorzec.linia + wzorzec.przerwa;
        pozycja_w_cyklu = dystans_metry % długość_cyklu;

        if (pozycja_w_cyklu < wzorzec.linia) {
            włącz_pistolety(wzorzec.szerokość);
        } else {
            wyłącz_pistolety();
        }
    }
}
```

### 4.3 Algorytm Obliczania Powierzchni

```cpp
float oblicz_powierzchnię() {
    szerokość_m = wzorzec.szerokość / 100.0;  // cm → m
    dystans_m = dystans_cm / 100.0;            // cm → m

    if (wzorzec.ciągły) {
        return dystans_m * szerokość_m;
    } else {
        długość_cyklu = wzorzec.linia + wzorzec.przerwa;
        procent_malowania = wzorzec.linia / długość_cyklu;
        długość_malowana = dystans_m * procent_malowania;

        return długość_malowana * szerokość_m;
    }
}
```

### 4.4 Algorytm Odwracania Wzorców

Dla wzorców P-3a i P-3b (przekraczalne):
- **Standardowo**: Linia ciągła po lewej (pistolety 1-3), przerywana po prawej (4-6)
- **Odwrócone**: Linia ciągła po prawej (pistolety 4-6), przerywana po lewej (1-3)

```cpp
if (wzorzec == P3A || wzorzec == P3B) {
    if (odwrócony) {
        // Linia ciągła na prawo
        pistolety_4_5_6 = ON (ciągłe);
        pistolety_1_2_3 = przerywane;
    } else {
        // Linia ciągła na lewo (standard)
        pistolety_1_2_3 = ON (ciągłe);
        pistolety_4_5_6 = przerywane;
    }
}
```

---

## 5. Stany Systemu

System operuje w 6 stanach:

### 5.1 STATE_IDLE (Gotowy)
- System czeka na polecenie
- Wyświetlacz pokazuje wybrany wzorzec
- Liczniki wyzerowane lub zatrzymane
- Przekaźniki wyłączone

### 5.2 STATE_PAINTING (Malowanie)
- Aktywne malowanie
- Enkoder zlicza dystans
- Przekaźniki sterowane wg wzorca
- Aktualizacja wyświetlacza co 100ms

### 5.3 STATE_PAUSED (Pauza)
- Malowanie wstrzymane
- Liczniki zamrożone
- Przekaźniki wyłączone
- Możliwość wznowienia

### 5.4 STATE_MENU (Menu)
- Wyświetlane menu systemowe
- Nawigacja joystickiem
- Dostęp do kalibracji, pomiarów, info

### 5.5 STATE_CALIBRATING (Kalibracja)
- Proces kalibracji enkodera
- Pomiar na 10 metrów
- Zapis do pamięci

### 5.6 STATE_MEASURING (Pomiar dystansu)
- Tryb pomiaru odległości
- Wyświetlanie dystansu w czasie rzeczywistym
- Bez włączania przekaźników

---

## 6. Pamięć i Przechowywanie Danych

### 6.1 Pamięć Nieulotna (Preferences)

System używa biblioteki Preferences ESP32 do przechowywania:

```cpp
struct CalibrationData {
    uint16_t magicNumber;       // 0xCAFE - weryfikacja
    float pulsesPerCm;          // Impulsy na centymetr
    uint32_t calibrationDate;   // Timestamp kalibracji
    uint8_t checksum;           // XOR wszystkich bajtów
};
```

**Namespace**: "calibration"
**Klucz**: "caldata"

### 6.2 Weryfikacja Danych

Przy starcie systemu:
1. Odczyt danych z Preferences
2. Sprawdzenie magicNumber (0xCAFE)
3. Obliczenie i porównanie sumy kontrolnej
4. Jeśli poprawne: załadowanie kalibracji
5. Jeśli niepoprawne: wymuszenie kalibracji

---

## 7. Protokoły Komunikacji

### 7.1 SPI (Wyświetlacz)

**Parametry:**
- **Częstotliwość**: 40 MHz (odczyt), 40 MHz (zapis)
- **Tryb**: Mode 0 (CPOL=0, CPHA=0)
- **Kolejność bitów**: MSB First

### 7.2 Przerwania

**Enkoder (GPIO 32):**
- Typ: CHANGE
- Priorytet: Wysoki
- Funkcja: Zliczanie impulsów w czasie rzeczywistym

---

## 8. Optymalizacje

### 8.1 Wydajność

1. **Przerwania dla enkodera**: Zapewnienie dokładności pomiaru
2. **Częściowe odświeżanie ekranu**: Aktualizacja tylko zmienionych obszarów
3. **Bufory tekstowe**: Sprintf do formatowania, unikanie String
4. **Debouncing sprzętowy**: Pull-up rezystory + opóźnienia programowe

### 8.2 Oszczędność Energii

1. **Optymalizacja częstotliwości odświeżania**: 100ms zamiast ciągłego
2. **Wyłączanie przekaźników**: Natychmiastowe po zatrzymaniu
3. **Efektywne biblioteki**: TFT_eSPI zamiast Adafruit GFX

---

## 9. Bezpieczeństwo

### 9.1 Zabezpieczenia Sprzętowe

1. **Optoisolacja przekaźników**: Separacja zasilania
2. **Pull-up rezystory**: Określony stan przycisków
3. **Ograniczniki prądowe**: Ochrona GPIO

### 9.2 Zabezpieczenia Programowe

1. **Sprawdzanie granic**: Walidacja indeksów tablic
2. **Timeout'y**: Ograniczenie czasu operacji
3. **Suma kontrolna**: Weryfikacja danych kalibracji
4. **Debouncing**: Eliminacja drgań przycisków

---

## 10. Testowanie

### 10.1 Test Przekaźników

Przy starcie systemu wykonywana jest sekwencja testowa:
```cpp
void testSequence() {
    for (int i = 1; i <= 6; i++) {
        włącz_przekaźnik(i);
        delay(200);
        wyłącz_przekaźnik(i);
        delay(100);
    }
}
```

### 10.2 Debug

System posiada makra DEBUG do logowania:
```cpp
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
```

Włączanie/wyłączanie w config.h:
```cpp
#define DEBUG_ENABLED 1  // 1=włącz, 0=wyłącz
```

---

## 11. Rozwiązywanie Problemów

### 11.1 System nie startuje
- Sprawdź zasilanie 3.3V i 5V
- Sprawdź połączenie USB
- Sprawdź logi Serial (115200 baud)

### 11.2 Wyświetlacz nie działa
- Sprawdź połączenia SPI
- Sprawdź zasilanie 3.3V
- Zweryfikuj piny w config.h

### 11.3 Enkoder nie zlicza
- Sprawdź kalibrację
- Sprawdź połączenia CLK, DT
- Sprawdź przerwania

### 11.4 Przekaźniki nie reagują
- Sprawdź zasilanie 5V
- Sprawdź połączenia GPIO
- Użyj testSequence()

---

## 12. Rozszerzenia i Modyfikacje

### 12.1 Dodawanie Nowych Wzorców

1. Dodaj enum w `config.h`:
```cpp
enum PatternType {
    ...
    PATTERN_CUSTOM,
    PATTERN_COUNT
};
```

2. Dodaj definicję w `patterns.h`:
```cpp
{
    PATTERN_CUSTOM,
    "P-XX",
    linia_m,
    przerwa_m,
    szerokość_cm,
    "Opis",
    odwracalny
}
```

3. Dodaj przycisk w pinach i `main.cpp`

### 12.2 Zmiana Częstotliwości Odświeżania

W `config.h`:
```cpp
#define REFRESH_RATE 100  // ms (zmień na żądaną wartość)
```

---

## 13. Konserwacja

### 13.1 Regularna

- Sprawdzanie połączeń co 100 godzin pracy
- Czyszczenie wyświetlacza
- Test przekaźników

### 13.2 Rekalibracja

Zalecana co:
- 1000 km przejechanych
- Wymiana koła/enkodera
- Zauważalne niedokładności

---

## 14. Specyfikacja Wydajności

- **Czas reakcji na zmianę wzorca**: < 50ms
- **Dokładność pomiaru dystansu**: ±1cm (po kalibracji)
- **Częstotliwość odświeżania wyświetlacza**: 10 Hz
- **Maksymalna prędkość**: Ograniczona tylko enkoderem
- **Żywotność przekaźników**: 100,000 cykli

---

**Wersja dokumentacji**: 1.0.0
**Data**: 2026-01-23
**Autor**: MT220126 Engineering Team
