# Schematy Połączeń Elektrycznych
## System Sterowania Malowaniem Pasów Drogowych v1.6.0

**Status**: ✅ KOMPLETNE SCHEMATY PRODUKCYJNE (NAPRAWIONE GPIO STRAPPING PINS!)
**Data**: 2026-01-26
**Autor**: MT220126 Engineering Team

---

## ⚠️ ZMIANY w wersji 1.4.0 - KOMPLEKSOWA REFAKTORYZACJA GPIO

### Dlaczego zmieniono piny?

Oryginalna dokumentacja v1.0.0 zawierała **KRYTYCZNE KONFLIKTY GPIO**:
- Niektóre piny były wykorzystane 3x jednocześnie!
- GPIO 25, 26, 27, 32-35 miały konflikty między enkoderami, joystickiem, przekaźnikami i przyciskami
- Brakowało osobnego selektora menu (używano przycisku enkodera, który był na pinie z konfliktem)

### Co się zmieniło?

#### 1. **Przekaźniki** (Pistolety malarskie)
- **v1.0.0**: GPIO 13, 12, 14, 27, 26, 25 (rozproszone, z konfliktami)
- **v1.4.0**: GPIO 12-17 (ciągła grupa, ALE strapping pins!)
- **v1.6.0**: GPIO 10, 11, 8, 9, 16, 17 ✅ **NAPRAWIONE - BEZ STRAPPING PINS!**

#### 2. **Przyciski sterowania**
- **BTN_START**: 12 → **0** (Boot pin, bezpieczny z INPUT_PULLUP)
- **BTN_STOP**: 13 → **2** (wolny pin)
- **BTN_REVERSE**: 15 → **4** (wolny pin)
- **BTN_START_GAP**: **46** (NOWY w v1.3.0)

#### 3. **Selektor menu**
- **NOWY**: GPIO **20** (osobny przycisk do nawigacji menu)
- Enkoder pozostaje tylko do pomiaru dystansu!

#### 4. **Przyciski wzorców** (15 przycisków)
- **PRZED**: Rozproszone, z konfliktami (GPIO 2, 4, 14-17, 25-27, 32-36, 39)
- **PO**: ESP32-S3 specific GPIOs (26-27, 36-48, 1)
  - Wykorzystano GPIO 36-48 (dostępne tylko na ESP32-S3!)
  - GPIO 1 (UART TX) używany jako INPUT_PULLUP (bezpieczne)

#### 5. **Enkoder i Joystick**
- **BEZ ZMIAN**: GPIO 32-35 (enkoder + joystick osie)
- Usunięto konflikt z przekaźnikami i przyciskami

---

## 📐 Spis Treści

1. [Schemat Blokowy Systemu](#1-schemat-blokowy-systemu)
2. [Schemat Połączeń ESP32-S3](#2-schemat-połączeń-esp32-s3)
3. [Wyświetlacz ILI9341](#3-wyświetlacz-ili9341)
4. [Enkoder KY-040](#4-enkoder-ky-040)
5. [Joystick Analogowy](#5-joystick-analogowy)
6. [Moduły Przekaźników](#6-moduły-przekaźników)
7. [Przyciski](#7-przyciski)
8. [Zasilanie](#8-zasilanie)
9. [Montaż](#9-montaż)

---

## 1. Schemat Blokowy Systemu

```
                        ┌─────────────────────────────────┐
                        │                                 │
                        │         ESP32-S3 N16R8          │
                        │      (Główny Kontroler)         │
                        │                                 │
                        └─────────────────────────────────┘
                                      │
        ┌─────────────────────────────┼─────────────────────────────┐
        │                             │                             │
        ▼                             ▼                             ▼
┌──────────────┐            ┌──────────────────┐         ┌─────────────────┐
│ Wyświetlacz  │            │    Enkoder       │         │  Moduł          │
│  ILI9341     │            │    KY-040        │         │  Przekaźników   │
│  (SPI)       │            │  (Pomiar dist.)  │         │  (6 kanałów)    │
└──────────────┘            └──────────────────┘         └─────────────────┘
                                                                   │
        ┌───────────────────────────────────────────────────────────┤
        │                    │                    │                 │
        ▼                    ▼                    ▼                 ▼
┌──────────────┐   ┌──────────────┐   ┌──────────────┐   ┌────────────┐
│  Pistolet 1  │   │  Pistolet 2  │   │  Pistolet 3  │   │ Pistolety  │
│  (lewy)      │   │              │   │              │   │  4, 5, 6   │
└──────────────┘   └──────────────┘   └──────────────┘   └────────────┘

        ┌─────────────────────────────┐
        │      Inne Peryferia:        │
        │  - Joystick (nawigacja)     │
        │  - 15 przycisków wzorców    │
        │  - 3 przyciski sterowania   │
        └─────────────────────────────┘
```

---

## 2. Schemat Połączeń ESP32-S3

### 2.1 Mapowanie Pinów (✅ v1.4.0 - POPRAWIONE)

**UWAGA**: Schemat poniżej jest uproszczony dla czytelności.
ESP32-S3 N16R8 ma 49 GPIO (0-48), nie wszystkie są wyprowadzone na standardowych pinach.

```
                      ESP32-S3 (Kluczowe GPIO - v1.6.0)

   WYŚWIETLACZ SPI:          PRZEKAŹNIKI (v1.6.0 NAPRAWIONE!):
   ┌──────────────┐          ┌──────────────────────┐
   │ GPIO 5  (CS) │          │ GPIO 10 → RELAY_1 ✅ │
   │ GPIO 18 (SCK)│          │ GPIO 11 → RELAY_2 ✅ │
   │ GPIO 19 (MISO)│         │ GPIO 8  → RELAY_3 ✅ │
   │ GPIO 21 (RST)│          │ GPIO 9  → RELAY_4 ✅ │
   │ GPIO 22 (DC) │          │ GPIO 16 → RELAY_5    │
   │ GPIO 23 (MOSI)│         │ GPIO 17 → RELAY_6    │
   └──────────────┘          └──────────────────────┘

   ENKODER + JOYSTICK:       STEROWANIE:
   ┌──────────────┐          ┌──────────────────────┐
   │ GPIO 32 (ENC_CLK)│      │ GPIO 0  → START      │
   │ GPIO 33 (ENC_DT) │      │ GPIO 2  → STOP       │
   │ GPIO 34 (JOY_X)  │      │ GPIO 4  → REVERSE    │
   │ GPIO 35 (JOY_Y)  │      │ GPIO 46 → START_GAP  │
   └──────────────┘          │ GPIO 20 → SELEKTOR   │
                             └──────────────────────┘

   PRZYCISKI WZORCÓW (ESP32-S3 specific GPIO 26-48):
   ┌────────────────────────────────────────────────┐
   │ GPIO 26→P1A  27→P1B  36→P1C  37→P1D  38→P1E  │
   │ GPIO 39→P2A  40→P2B  41→P3A  42→P3B  43→P4   │
   │ GPIO 44→P6   45→P7A  47→P7B  48→P7C  3→P7D ✅ │
   └────────────────────────────────────────────────┘

   ⚠️ UWAGI SPECJALNE v1.6.0:
   - GPIO 36, 39: Input-only (zewnętrzne pull-up 10kΩ)
   - GPIO 0: Boot pin (nie trzymaj przy starcie)
   - GPIO 3: UART RX (zmienione z GPIO 1 w v1.6.0, bezpieczniejsze)
```

### 2.2 Tabela Pinów (✅ v1.4.0 - POPRAWIONE)

| GPIO | Funkcja | Typ | Opis |
|------|---------|-----|------|
| **SPI (Wyświetlacz)** ||||
| 18 | TFT_SCLK | Output | SPI Clock |
| 19 | TFT_MISO | Input | SPI MISO (opcjonalne) |
| 23 | TFT_MOSI | Output | SPI MOSI |
| 5 | TFT_CS | Output | Chip Select |
| 22 | TFT_DC | Output | Data/Command |
| 21 | TFT_RST | Output | Reset |
| **Enkoder** ||||
| 32 | ENC_CLK | Input | Encoder Clock (przerwanie) |
| 33 | ENC_DT | Input | Encoder Data |
| **Selektor Menu** ||||
| 20 | SELECTOR | Input | Przycisk nawigacji menu (NOWY!) |
| **Joystick** ||||
| 34 | JOY_X | ADC | Oś X (analogowa) |
| 35 | JOY_Y | ADC | Oś Y (analogowa) |
| **Przekaźniki** ||||
| 10 | RELAY_1 | Output | Pistolet 1 (oś, 12cm) ✅ v1.6.0 |
| 11 | RELAY_2 | Output | Pistolet 2 (oś, 12cm) ✅ v1.6.0 |
| 8  | RELAY_3 | Output | Pistolet 3 (oś, 12cm) ✅ v1.6.0 |
| 9  | RELAY_4 | Output | Pistolet 4 (oś, 24cm) ✅ v1.6.0 |
| 16 | RELAY_5 | Output | Pistolet 5 (krawędź, 12cm) |
| 17 | RELAY_6 | Output | Pistolet 6 (krawędź, 24cm) |
| **Przyciski Sterowania** ||||
| 0 | BTN_START | Input | Start/Pauza |
| 2 | BTN_STOP | Input | Stop |
| 4 | BTN_REVERSE | Input | Odwróć wzorzec (P-3a/P-3b) |
| 46 | BTN_START_GAP | Input | Start Od Przerwy (v1.3.0) |
| **Przyciski Wzorców** ||||
| 26 | BTN_P1A | Input | Wzorzec P-1a |
| 27 | BTN_P1B | Input | Wzorzec P-1b |
| 36 | BTN_P1C | Input | Wzorzec P-1c (input-only) |
| 37 | BTN_P1D | Input | Wzorzec P-1d |
| 38 | BTN_P1E | Input | Wzorzec P-1e |
| 39 | BTN_P2A | Input | Wzorzec P-2a (input-only) |
| 40 | BTN_P2B | Input | Wzorzec P-2b |
| 41 | BTN_P3A | Input | Wzorzec P-3a |
| 42 | BTN_P3B | Input | Wzorzec P-3b |
| 43 | BTN_P4 | Input | Wzorzec P-4 |
| 44 | BTN_P6 | Input | Wzorzec P-6 |
| 45 | BTN_P7A | Input | Wzorzec P-7a |
| 47 | BTN_P7B | Input | Wzorzec P-7b |
| 48 | BTN_P7C | Input | Wzorzec P-7c |
| 3  | BTN_P7D | Input | Wzorzec P-7d (UART RX) ✅ v1.6.0 |

**⚠️ UWAGI v1.6.0**:
- GPIO 36, 39: Input-only pins (bez wbudowanych pull-up, wymagają zewnętrznych rezystorów 10kΩ)
- **KRYTYCZNE v1.6.0**: GPIO 12-15 są STRAPPING PINS! Zmieniono na GPIO 8-11 (bezpieczne)
- GPIO 3 (BTN_P7D): UART RX - zmienione z GPIO 1 (v1.6.0, bezpieczniejsze)
- GPIO 0 (BTN_START): Boot pin - bezpieczne z INPUT_PULLUP, nie trzymaj wciśniętego przy starcie!
- GPIO 8-11, 16-17: Nowe mapowanie przekaźników - BEZ KONFLIKTÓW BOOT!
- GPIO 26-48: ESP32-S3 specific GPIOs (36-48 dostępne tylko na S3!)

---

## 3. Wyświetlacz ILI9341

### 3.1 Schemat Połączenia

```
    ILI9341 TFT                          ESP32-S3
   ┌────────────┐                      ┌──────────┐
   │            │                      │          │
   │ VCC     ●──┼──────────────────────┤ 3.3V     │
   │ GND     ●──┼──────────────────────┤ GND      │
   │ CS      ●──┼──────────────────────┤ GPIO 5   │
   │ RESET   ●──┼──────────────────────┤ GPIO 21  │
   │ DC      ●──┼──────────────────────┤ GPIO 22  │
   │ SDI     ●──┼──────────────────────┤ GPIO 23  │ (MOSI)
   │ SCK     ●──┼──────────────────────┤ GPIO 18  │
   │ LED     ●──┼──────────────────────┤ 3.3V     │
   │ SDO     ●──┼──────────────────────┤ GPIO 19  │ (MISO - opcj.)
   │            │                      │          │
   └────────────┘                      └──────────┘
```

### 3.2 Uwagi Montażowe

- **Zasilanie**: Wyłącznie 3.3V! (ESP32 nie toleruje 5V)
- **SPI**: Standardowy interfejs SPI hardware
- **Prędkość**: 40 MHz (zdefiniowane w TFT_eSPI)
- **Backlight**: Podłączony na stałe do 3.3V

---

## 4. Enkoder KY-040 (✅ v1.4.0)

### 4.1 Schemat Połączenia

```
    KY-040 Enkoder                       ESP32-S3
   ┌────────────┐                      ┌──────────┐
   │            │                      │          │
   │ CLK     ●──┼──────────────────────┤ GPIO 32  │ (Przerwanie)
   │ DT      ●──┼──────────────────────┤ GPIO 33  │
   │ SW      ●──┼──── NIE UŻYWANY      │          │ ⚠️ Zmiana!
   │ +       ●──┼──────────────────────┤ 3.3V     │
   │ GND     ●──┼──────────────────────┤ GND      │
   │            │                      │          │
   └────────────┘                      └──────────┘

   Montaż mechaniczny:
   ┌─────────────────┐
   │   Enkoder       │
   │   KY-040        │◄──── Pasek zębaty lub koło
   └─────────────────┘

   Selektor Menu (osobny przycisk):
   ┌────────────┐                      ┌──────────┐
   │ Przycisk   │──────────────────────┤ GPIO 20  │ NOWY!
   └────────────┘                      └──────────┘
```

### 4.2 Uwagi Montażowe

- **Montaż**: Zamocuj enkoder do koła napędowego lub użyj paska zębatego
- **Przełożenie**: Można użyć przełożenia mechanicznego dla lepszej rozdzielczości
- **CLK Pin**: Podłączony do pinu z przerwaniem dla dokładności
- **Pull-up**: Wbudowane w ESP32 (INPUT_PULLUP)

### 4.3 ⚠️ ZMIANA v1.4.0: Enkoder vs Selektor

**PRZED (v1.0.0)**:
- Enkoder KY-040 używany do: pomiarów dystansu + nawigacja menu (przycisk SW)
- GPIO 25 (ENC_SW) - konflikt z przekaźnikiem!

**PO (v1.4.0)**:
- **Enkoder KY-040**: TYLKO do pomiaru dystansu (CLK + DT)
- **Selektor (GPIO 20)**: Osobny przycisk do nawigacji menu
- Brak konfliktów GPIO!

---

## 5. Joystick Analogowy (✅ v1.4.0)

### 5.1 Schemat Połączenia

```
    Joystick 2-osiowy                    ESP32-S3
   ┌────────────┐                      ┌──────────┐
   │            │                      │          │
   │ VRX     ●──┼──────────────────────┤ GPIO 34  │ (ADC1_CH6)
   │ VRY     ●──┼──────────────────────┤ GPIO 35  │ (ADC1_CH7)
   │ SW      ●──┼──── NIE UŻYWANY      │          │ ⚠️ Zmiana!
   │ VCC     ●──┼──────────────────────┤ 3.3V     │
   │ GND     ●──┼──────────────────────┤ GND      │
   │            │                      │          │
   └────────────┘                      └──────────┘

   Wartości ADC:
   - Środek: ~2048 (12-bit ADC)
   - Góra: ~0
   - Dół: ~4095
   - Lewo: ~0
   - Prawo: ~4095
```

### 5.2 Uwagi

- **ADC**: Używaj tylko pinów ADC1 (ADC2 konfliktuje z WiFi)
- **Rozdzielczość**: 12-bit (0-4095)
- **Kalibracja**: Wartość środkowa może się różnić (~1800-2200)

### 5.3 ⚠️ ZMIANA v1.4.0: Joystick bez przycisku

**PRZED (v1.0.0)**:
- GPIO 26 (JOY_SW) - konflikt z przekaźnikiem RELAY_5!

**PO (v1.4.0)**:
- Joystick używa TYLKO osi analogowych (VRX, VRY)
- Przycisk joysticka (SW) NIE UŻYWANY
- Do nawigacji menu: joystick osie + selektor (GPIO 20)

---

## 6. Moduły Przekaźników (✅ v1.4.0 - NOWE PINY!)

### 6.1 Schemat Połączenia (Moduł 6-kanałowy)

```
    Moduł Przekaźników 6CH              ESP32-S3
   ┌────────────────────┐             ┌──────────┐
   │                    │             │          │
   │ VCC         ●──────┼─────────────┤ 5V       │ (Zewnętrzne!)
   │ GND         ●──────┼─────────────┤ GND      │
   │ IN1 (Rel 1) ●──────┼─────────────┤ GPIO 12  │ ← ZMIANA
   │ IN2 (Rel 2) ●──────┼─────────────┤ GPIO 13  │ ← ZMIANA
   │ IN3 (Rel 3) ●──────┼─────────────┤ GPIO 14  │
   │ IN4 (Rel 4) ●──────┼─────────────┤ GPIO 15  │ ← ZMIANA
   │ IN5 (Rel 5) ●──────┼─────────────┤ GPIO 16  │ ← ZMIANA
   │ IN6 (Rel 6) ●──────┼─────────────┤ GPIO 17  │ ← ZMIANA
   │                    │             │          │
   └────────────────────┘             └──────────┘

   Mapowanie Pistoletów:
   ┌─────────────────────────────────────────────────┐
   │ RELAY_1 (GPIO 12) → Pistolet P1 (oś, 12cm)     │
   │ RELAY_2 (GPIO 13) → Pistolet P2 (oś, 12cm)     │
   │ RELAY_3 (GPIO 14) → Pistolet P3 (oś, 12cm)     │
   │ RELAY_4 (GPIO 15) → Pistolet P4 (oś, 24cm)     │
   │ RELAY_5 (GPIO 16) → Pistolet P5 (krawędź, 12cm)│
   │ RELAY_6 (GPIO 17) → Pistolet P6 (krawędź, 24cm)│
   └─────────────────────────────────────────────────┘

   Wyjścia przekaźników (każdy kanał):
   ┌──────────────┐
   │ NC  COM  NO  │◄──── Podłączenie elektrozaworów pistoletów
   └──────────────┘
        │   │   │
        │   │   └──── Normalnie otwarty (używany)
        │   └──────── Wspólny
        └──────────── Normalnie zamknięty
```

### 6.1.1 ⚠️ ZMIANA v1.4.0: Piny Przekaźników

**PRZED (v1.0.0)**: GPIO 13, 12, 14, 27, 26, 25 (rozproszone, z konfliktami)
**PO (v1.4.0)**: GPIO **12-17** (ciągła grupa)

**Zalety nowego mapowania**:
- ✅ Ciągła grupa GPIO (łatwy routing na PCB)
- ✅ Brak konfliktów z przyciskami, enkoderami, joystickiem
- ✅ Logiczne uporządkowanie (12→P1, 13→P2, ... 17→P6)
- ✅ Łatwiejsze debugowanie (sekwencja numeryczna)

### 6.2 Podłączenie Pistoletów

```
Pistolet malarskiElektrozawór
   ┌──────────────────────────┐
   │                          │
   │    Elektrozawór 12V DC   │
   │                          │
   └──────────────────────────┘
          │              │
          │              │
   ┌──────┴──────┐  ┌───┴────┐
   │     +12V    │  │   NO   │◄──── Z przekaźnika
   └─────────────┘  └────────┘
          │
          │
   ┌──────┴──────┐
   │     GND     │
   └─────────────┘
```

### 6.3 Uwagi Bezpieczeństwa

⚠️ **WAŻNE**:
- Zasilanie modułu przekaźników: **5V zewnętrzne** (nie z ESP32!)
- Maksymalny prąd: 10A na kanał
- Używaj diod zabezpieczających przy indukcyjnych obciążeniach
- Optoisolacja: Chroni ESP32 przed przepięciami

---

## 7. Przyciski (✅ v1.4.0 - NOWE PINY!)

### 7.1 Schemat Pojedynczego Przycisku

```
                   ESP32-S3
                  ┌──────────┐
                  │ GPIO XX  │──────┐
                  │          │      │
                  │ GND      │──┐   │
                  └──────────┘  │   │
                                │   │
                            ┌───┴───┴───┐
                            │           │
                            │ Przycisk  │
                            │           │
                            └───────────┘

   Konfiguracja: INPUT_PULLUP
   - Nie naciśnięty: HIGH (3.3V)
   - Naciśnięty: LOW (GND)
```

### 7.2 Panel Przycisków (Sugerowany Layout v1.4.0)

```
┌──────────────────────────────────────────────────────┐
│              PANEL STEROWANIA v1.4.0                 │
├──────────────────────────────────────────────────────┤
│  WZORCE MALOWANIA:                                   │
│                                                      │
│  [P-1a] [P-1b] [P-1c] [P-1d] [P-1e]                 │
│  GPIO:   26     27     36     37     38              │
│                                                      │
│  [P-2a] [P-2b] [P-3a] [P-3b] [P-4 ]                 │
│  GPIO:   39     40     41     42     43              │
│                                                      │
│  [P-6 ] [P-7a] [P-7b] [P-7c] [P-7d]                 │
│  GPIO:   44     45     47     48      1              │
│                                                      │
├──────────────────────────────────────────────────────┤
│  STEROWANIE:                                         │
│                                                      │
│  [START/PAUZA]  [STOP]  [REVERSE]  [START GAP]      │
│  GPIO: 0        2       4          46                │
│                                      (v1.3.0)        │
│                                                      │
├──────────────────────────────────────────────────────┤
│  NAWIGACJA MENU:                                     │
│                                                      │
│  [Joystick] (GPIO 34/35) + [SELEKTOR] (GPIO 20)     │
│                                                      │
└──────────────────────────────────────────────────────┘
```

### 7.3 Mapowanie Przycisków - Kompletna Tabela

#### Przyciski Wzorców (15 wzorców)
| Przycisk | GPIO | Uwagi |
|----------|------|-------|
| P-1a | 26 | Standardowy I/O |
| P-1b | 27 | Standardowy I/O |
| P-1c | 36 | **Input-only** (wymaga zewnętrznego pull-up 10kΩ) |
| P-1d | 37 | Standardowy I/O |
| P-1e | 38 | Standardowy I/O |
| P-2a | 39 | **Input-only** (wymaga zewnętrznego pull-up 10kΩ) |
| P-2b | 40 | Standardowy I/O |
| P-3a | 41 | Standardowy I/O |
| P-3b | 42 | Standardowy I/O |
| P-4  | 43 | Standardowy I/O |
| P-6  | 44 | Standardowy I/O |
| P-7a | 45 | Standardowy I/O |
| P-7b | 47 | Standardowy I/O |
| P-7c | 48 | Standardowy I/O |
| P-7d | 1  | **UART TX** (INPUT_PULLUP gdy Serial nieużywany) |

#### Przyciski Sterowania (4 przyciski)
| Przycisk | GPIO | Uwagi |
|----------|------|-------|
| START/PAUZA | 0 | **Boot pin** (nie trzymaj wciśniętego przy starcie!) |
| STOP | 2 | Standardowy I/O |
| REVERSE | 4 | Standardowy I/O |
| START GAP | 46 | Nowy w v1.3.0 |

#### Nawigacja Menu
| Funkcja | GPIO | Uwagi |
|---------|------|-------|
| Joystick X | 34 | ADC1 (input-only) |
| Joystick Y | 35 | ADC1 (input-only) |
| Selektor | 20 | Nowy w v1.4.0 (osobny od enkodera!) |

### 7.4 Uwagi Montażowe v1.4.0

- **INPUT_PULLUP**: Większość przycisków (wbudowany pull-up ESP32)
- **GPIO 36, 39**: Input-only, wymagają **zewnętrznych rezystorów pull-up 10kΩ**
- **GPIO 0**: Boot pin - NIE TRZYMAJ wciśniętego podczas startu ESP32!
- **GPIO 1**: UART TX - bezpieczne jako przycisk gdy Serial używany tylko w setup()
- **Debouncing**: 50ms opóźnienie w software (non-blocking)
- **Oznaczenie**: Każdy przycisk wyraźnie opisany (etykiety, grawerowanie)

### 7.5 ⚠️ ZMIANY v1.4.0: Przyciski

**PRZED (v1.0.0)**:
- Przyciski rozproszone: GPIO 2, 4, 12-17, 25-27, 32-36, 39
- **KONFLIKTY** z enkoderami, przekaźnikami, joystickiem!

**PO (v1.4.0)**:
- Wykorzystano GPIO **26-27, 36-48, 1** (ESP32-S3 specific!)
- Sterowanie: GPIO **0, 2, 4, 46**
- **ZERO KONFLIKTÓW** - każdy pin unikatowy!

---

## 8. Zasilanie

### 8.1 Schemat Zasilania

```
                    Bateria 12V (akumulator pojazdu)
                            │
                   ┌────────┴─────────┐
                   │  Bezpiecznik 10A  │
                   └────────┬─────────┘
                            │
              ┌─────────────┴──────────────┐
              │                            │
              ▼                            ▼
    ┌──────────────────┐        ┌──────────────────┐
    │  Przetwornica    │        │   Moduł          │
    │  12V → 5V        │        │   Przekaźników   │
    │  (Buck 5A)       │        │   (12V zasilanie)│
    └────────┬─────────┘        └──────────────────┘
             │                            │
             │                            ▼
             ▼                   ┌──────────────────┐
   ┌──────────────────┐          │  Elektrozawory   │
   │  Przetwornica    │          │  Pistoletów      │
   │  5V → 3.3V       │          │  (12V)           │
   │  (LDO 1A)        │          └──────────────────┘
   └────────┬─────────┘
            │
            ▼
   ┌──────────────────┐
   │   ESP32-S3       │
   │   + Peryferia    │
   │   (3.3V)         │
   └──────────────────┘
```

### 8.2 Wymagania Zasilania

| Komponent | Napięcie | Prąd | Moc |
|-----------|----------|------|-----|
| ESP32-S3 | 3.3V | 500mA | 1.65W |
| Wyświetlacz ILI9341 | 3.3V | 100mA | 0.33W |
| Enkoder | 3.3V | 10mA | 0.03W |
| Joystick | 3.3V | 5mA | 0.02W |
| Moduł przekaźników | 5V | 500mA | 2.5W |
| Elektrozawory (6x) | 12V | 3A | 36W |
| **SUMA** | - | - | **~40W** |

### 8.3 Zalecane Przetwornice

1. **12V → 5V**: Buck converter, min. 5A (np. LM2596)
2. **5V → 3.3V**: LDO regulator, min. 1A (np. AMS1117-3.3)

### 8.4 Bezpieczniki

- Główny: **10A** (na wejściu 12V)
- ESP32: **1A** (na linii 3.3V)

---

## 9. Montaż

### 9.1 Układ Komponentów w Obudowie

```
┌──────────────────────────────────────────────┐
│  OBUDOWA SYSTEMU (widok z góry)              │
├──────────────────────────────────────────────┤
│                                              │
│  ┌─────────────────┐                        │
│  │  Wyświetlacz    │                        │
│  │  TFT 320x240    │                        │
│  └─────────────────┘                        │
│                                              │
│  [Przyciski - patrz 7.2]                    │
│                                              │
│  [Joystick]                                 │
│                                              │
├──────────────────────────────────────────────┤
│  WNĘTRZE (pod pokrywą):                     │
│                                              │
│  ┌───────────┐  ┌──────────────┐            │
│  │  ESP32-S3 │  │  Moduł       │            │
│  │           │  │  Przekaźników│            │
│  └───────────┘  └──────────────┘            │
│                                              │
│  ┌─────────────────────────────┐            │
│  │  Przetwornice zasilania     │            │
│  │  12V→5V→3.3V                │            │
│  └─────────────────────────────┘            │
│                                              │
└──────────────────────────────────────────────┘

Przyłącza zewnętrzne:
- Zasilanie 12V (wtyk DC)
- Wyjścia do 6 pistoletów (złącza szybkozłączne)
- Wejście od enkodera (kabel ekranowany)
- USB (programowanie/debug)
```

### 9.2 Zalecenia Montażowe

1. **Obudowa**: IP54 lub wyższa, plastik ABS
2. **Uszczelki**: Na wszystkich złączach
3. **Mocowanie**: Wibroodporne (amortyzatory gumowe)
4. **Wentylacja**: Otwory wentylacyjne z filtrami
5. **Kable**: Ekranowane dla enkodera i sygnałów cyfrowych
6. **Złącza**: Wodoodporne szybkozłącza

### 9.3 Instalacja w Pojeździe

```
   Pojazd (widok z boku)
   ┌────────────────────────────────────┐
   │                                    │
   │  [Kabina]   [Panel sterowania]    │
   │     │              │               │
   │     └──────────────┘               │
   │                                    │
   │  [Zbiornik farby]                 │
   │        │                           │
   │        └─── [Pistolety (6x)]      │
   │                    │               │
   └────────────────────┼───────────────┘
                        │
                    [Koło + Enkoder]
```

**Umiejscowienie:**
- Panel sterowania: W kabinie, łatwo dostępny dla operatora
- Enkoder: Na kole pojazdu (najlepiej tylnym, napędzanym)
- Pistolety: Równo rozłożone, szerokość ~150cm (24cm × 6 + odstępy)
- Zbiornik: Centralnie, z możliwością dolotu grawitacyjnego

---

## 10. Lista Materiałów (BOM)

### 10.1 Komponenty Elektroniczne

| Ilość | Komponent | Specyfikacja | Cena (szac.) |
|-------|-----------|--------------|--------------|
| 1 | ESP32-S3 N16R8 | Moduł deweloperski | 80 PLN |
| 1 | Wyświetlacz ILI9341 | 320x240 TFT SPI | 40 PLN |
| 1 | Enkoder KY-040 | Inkrementalny | 15 PLN |
| 1 | Joystick analogowy | 2-osiowy z przyciskiem | 10 PLN |
| 1 | Moduł przekaźników | 6-kanałowy, 12V, opto | 35 PLN |
| 18 | Przyciski | Podświetlane, IP65 | 180 PLN |
| 1 | Przetwornica 12V→5V | LM2596, 5A | 15 PLN |
| 1 | Stabilizator 5V→3.3V | AMS1117-3.3, 1A | 3 PLN |
| 1 | Obudowa | ABS, IP54, ~300x200x100mm | 120 PLN |
| - | Kable, złącza, montaż | - | 100 PLN |
| **SUMA** | | | **~598 PLN** |

### 10.2 Komponenty Mechaniczne (Pistolety - nie wliczone)

- 6x Elektrozawory 12V
- 6x Pistolety malarskie
- Zbiornik(i) farby
- System podciśnienia/ciśnienia
- Węże i połączenia

---

## 11. Testowanie i Weryfikacja

### 11.1 Testy Po Montażu

1. **Test zasilania**:
   ```
   [ ] 12V obecne
   [ ] 5V = 5.0V ±0.1V
   [ ] 3.3V = 3.3V ±0.05V
   [ ] Brak zwarć
   ```

2. **Test ESP32**:
   ```
   [ ] LED zasilania świeci
   [ ] Możliwość uploadu kodu
   [ ] Port Serial działa (115200)
   ```

3. **Test wyświetlacza**:
   ```
   [ ] Wyświetlacz się włącza
   [ ] Ekran powitalny OK
   [ ] Brak martwych pikseli
   ```

4. **Test enkodera**:
   ```
   [ ] Impulsy zliczane
   [ ] Kierunek prawidłowy
   [ ] Przycisk działa
   ```

5. **Test przycisków**:
   ```
   [ ] Wszystkie 18 przycisków OK
   [ ] Debouncing działa
   [ ] LEDy podświetlenia OK
   ```

6. **Test przekaźników**:
   ```
   [ ] Sekwencja testowa 1-6 OK
   [ ] Słyszalne kliknięcia
   [ ] Brak zalipów
   ```

7. **Test pistoletów**:
   ```
   [ ] Elektrozawory otwierają się
   [ ] Farba wypływa
   [ ] Brak przecieków
   ```

---

## 12. Migracja z v1.0.0 do v1.4.0

### 12.1 Checklist Aktualizacji Hardware

Jeśli aktualizujesz z wersji 1.0.0 do 1.4.0, wykonaj następujące kroki:

- [ ] **Przekaźniki**: Przepnij przewody z GPIO (13,12,14,27,26,25) → (12,13,14,15,16,17)
- [ ] **BTN_START**: Przepnij z GPIO 12 → GPIO 0
- [ ] **BTN_STOP**: Przepnij z GPIO 13 → GPIO 2
- [ ] **BTN_REVERSE**: Przepnij z GPIO 15 → GPIO 4
- [ ] **BTN_START_GAP**: Dodaj nowy przycisk na GPIO 46 (v1.3.0)
- [ ] **SELEKTOR**: Dodaj nowy przycisk na GPIO 20 (zamiast ENC_SW)
- [ ] **Przyciski wzorców**: Przepnij WSZYSTKIE według nowej tabeli (sekcja 7.3)
- [ ] **GPIO 36, 39**: Dodaj zewnętrzne rezystory pull-up 10kΩ (input-only pins)
- [ ] **Enkoder SW**: Odłącz (nie używany)
- [ ] **Joystick SW**: Odłącz (nie używany)

### 12.2 Weryfikacja Po Aktualizacji

```
[ ] Test zasilania (3.3V, 5V, 12V)
[ ] Test wyświetlacza (SPI OK)
[ ] Test enkodera (CLK + DT)
[ ] Test joysticka (osie X, Y)
[ ] Test selektora (GPIO 20)
[ ] Test przekaźników (sekwencja 12→13→14→15→16→17)
[ ] Test przycisków sterowania (START, STOP, REVERSE, START_GAP)
[ ] Test wszystkich 15 przycisków wzorców
[ ] Upload firmware v1.4.0
[ ] Test całego systemu
```

### 12.3 Troubleshooting

**Problem**: Przyciski nie reagują
- **Sprawdź**: Czy GPIO 36/39 mają zewnętrzne pull-up 10kΩ?
- **Sprawdź**: Czy GPIO 0 nie jest przytrzymany podczas startu?

**Problem**: Przekaźniki nie działają
- **Sprawdź**: Czy używasz nowych pinów 12-17?
- **Sprawdź**: Logikę (LOW = ON, HIGH = OFF na większości modułów)

**Problem**: Menu nie działa
- **Sprawdź**: Czy selektor podłączony do GPIO 20?
- **Sprawdź**: Czy enkoder SW odłączony?

---

**Wersja schematów**: **1.4.0** (Kompleksowa refaktoryzacja GPIO)
**Data ostatniej aktualizacji**: 2026-01-23
**Poprzednie wersje**: 1.0.0 (2026-01-23), 1.2.0 (WiFi), 1.3.0 (Start Gap)
**Inżynier**: MT220126 Engineering Team

---

**UWAGA**: Schematy mają charakter poglądowy. Przed wdrożeniem produkcyjnym zaleca się weryfikację przez certyfikowanego elektryka/elektronika.

**⚠️ KRYTYCZNE**: Dokumentacja v1.0.0 zawierała błędy w mapowaniu GPIO (konflikty)!
Zawsze używaj najnowszej wersji: **v1.4.0**
