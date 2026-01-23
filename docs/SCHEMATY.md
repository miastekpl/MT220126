# Schematy Połączeń
## System Sterowania Malowaniem Pasów Drogowych v1.0.0

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

### 2.1 Mapowanie Pinów

```
                      ESP32-S3 (Widok z góry)
                    ┌─────────────────────┐
               3.3V │ 1              40 │ GND
         (TFT_RST)  │ 2 GPIO21      39 │ GPIO36 (BTN_P2A)
         (TFT_DC)   │ 3 GPIO22      38 │ GPIO39 (BTN_P2B)
         (TFT_MOSI) │ 4 GPIO23      37 │ GPIO34 (JOY_X/BTN_P3A)
                    │ 5 GPIO19      36 │ GPIO35 (JOY_Y/BTN_P3B)
         (TFT_SCLK) │ 6 GPIO18      35 │ GPIO32 (ENC_CLK/BTN_P4)
         (BTN_P7D)  │ 7 GPIO14      34 │ GPIO33 (ENC_DT/BTN_P6)
         (RELAY_3)  │ 8 GPIO14      33 │ GPIO25 (ENC_SW/RELAY_6)
         (RELAY_2)  │ 9 GPIO12      32 │ GPIO26 (JOY_SW/RELAY_5)
         (RELAY_1)  │10 GPIO13      31 │ GPIO27 (RELAY_4/BTN_P7C)
         (BTN_P1A)  │11 GPIO15      30 │ GPIO16 (BTN_P1B)
         (BTN_P1C)  │12 GPIO17      29 │ GPIO4  (BTN_P1D)
         (TFT_CS)   │13 GPIO5       28 │ GPIO2  (BTN_P1E)
                GND │14             27 │ 3.3V
                5V  │15             26 │ EN
                    └─────────────────────┘
```

### 2.2 Tabela Pinów

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
| 25 | ENC_SW | Input | Encoder Button |
| **Joystick** ||||
| 34 | JOY_X | ADC | Oś X (analogowa) |
| 35 | JOY_Y | ADC | Oś Y (analogowa) |
| 26 | JOY_SW | Input | Przycisk joysticka |
| **Przekaźniki** ||||
| 13 | RELAY_1 | Output | Pistolet 1 (lewy) |
| 12 | RELAY_2 | Output | Pistolet 2 |
| 14 | RELAY_3 | Output | Pistolet 3 |
| 27 | RELAY_4 | Output | Pistolet 4 |
| 26 | RELAY_5 | Output | Pistolet 5 |
| 25 | RELAY_6 | Output | Pistolet 6 (prawy) |
| **Przyciski** ||||
| 15 | BTN_P1A | Input | Wzorzec P-1a |
| 16 | BTN_P1B | Input | Wzorzec P-1b |
| 17 | BTN_P1C | Input | Wzorzec P-1c |
| 4 | BTN_P1D | Input | Wzorzec P-1d |
| 2 | BTN_P1E | Input | Wzorzec P-1e |
| 36 | BTN_P2A | Input | Wzorzec P-2a |
| 39 | BTN_P2B | Input | Wzorzec P-2b |
| 34 | BTN_P3A | Input | Wzorzec P-3a |
| 35 | BTN_P3B | Input | Wzorzec P-3b |
| 32 | BTN_P4 | Input | Wzorzec P-4 |
| 33 | BTN_P6 | Input | Wzorzec P-6 |
| 25 | BTN_P7A | Input | Wzorzec P-7a |
| 26 | BTN_P7B | Input | Wzorzec P-7b |
| 27 | BTN_P7C | Input | Wzorzec P-7c |
| 14 | BTN_P7D | Input | Wzorzec P-7d |
| 12 | BTN_START | Input | Start/Pauza |
| 13 | BTN_STOP | Input | Stop |
| 15 | BTN_REVERSE | Input | Odwróć wzorzec |

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

## 4. Enkoder KY-040

### 4.1 Schemat Połączenia

```
    KY-040 Enkoder                       ESP32-S3
   ┌────────────┐                      ┌──────────┐
   │            │                      │          │
   │ CLK     ●──┼──────────────────────┤ GPIO 32  │ (Przerwanie)
   │ DT      ●──┼──────────────────────┤ GPIO 33  │
   │ SW      ●──┼──────────────────────┤ GPIO 25  │
   │ +       ●──┼──────────────────────┤ 3.3V     │
   │ GND     ●──┼──────────────────────┤ GND      │
   │            │                      │          │
   └────────────┘                      └──────────┘

   Montaż mechaniczny:
   ┌─────────────────┐
   │   Enkoder       │
   │   KY-040        │◄──── Pasek zębaty lub koło
   └─────────────────┘
```

### 4.2 Uwagi Montażowe

- **Montaż**: Zamocuj enkoder do koła napędowego lub użyj paska zębatego
- **Przełożenie**: Można użyć przełożenia mechanicznego dla lepszej rozdzielczości
- **CLK Pin**: Podłączony do pinu z przerwaniem dla dokładności
- **Pull-up**: Wbudowane w ESP32 (INPUT_PULLUP)

---

## 5. Joystick Analogowy

### 5.1 Schemat Połączenia

```
    Joystick 2-osiowy                    ESP32-S3
   ┌────────────┐                      ┌──────────┐
   │            │                      │          │
   │ VRX     ●──┼──────────────────────┤ GPIO 34  │ (ADC1_CH6)
   │ VRY     ●──┼──────────────────────┤ GPIO 35  │ (ADC1_CH7)
   │ SW      ●──┼──────────────────────┤ GPIO 26  │
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

---

## 6. Moduły Przekaźników

### 6.1 Schemat Połączenia (Moduł 6-kanałowy)

```
    Moduł Przekaźników 6CH              ESP32-S3
   ┌────────────────────┐             ┌──────────┐
   │                    │             │          │
   │ VCC         ●──────┼─────────────┤ 5V       │ (Zewnętrzne!)
   │ GND         ●──────┼─────────────┤ GND      │
   │ IN1 (Rel 1) ●──────┼─────────────┤ GPIO 13  │
   │ IN2 (Rel 2) ●──────┼─────────────┤ GPIO 12  │
   │ IN3 (Rel 3) ●──────┼─────────────┤ GPIO 14  │
   │ IN4 (Rel 4) ●──────┼─────────────┤ GPIO 27  │
   │ IN5 (Rel 5) ●──────┼─────────────┤ GPIO 26  │
   │ IN6 (Rel 6) ●──────┼─────────────┤ GPIO 25  │
   │                    │             │          │
   └────────────────────┘             └──────────┘

   Wyjścia przekaźników (każdy kanał):
   ┌──────────────┐
   │ NC  COM  NO  │◄──── Podłączenie elektrozaworów pistoletów
   └──────────────┘
        │   │   │
        │   │   └──── Normalnie otwarty (używany)
        │   └──────── Wspólny
        └──────────── Normalnie zamknięty
```

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

## 7. Przyciski

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

### 7.2 Panel Przycisków (Sugerowany Layout)

```
┌──────────────────────────────────────────────────┐
│              PANEL STEROWANIA                    │
├──────────────────────────────────────────────────┤
│  WZORCE MALOWANIA:                               │
│                                                  │
│  [P-1a] [P-1b] [P-1c] [P-1d] [P-1e]             │
│                                                  │
│  [P-2a] [P-2b] [P-3a] [P-3b] [P-4 ]             │
│                                                  │
│  [P-6 ] [P-7a] [P-7b] [P-7c] [P-7d]             │
│                                                  │
├──────────────────────────────────────────────────┤
│  STEROWANIE:                                     │
│                                                  │
│  [START/PAUZA]    [STOP]    [REVERSE]            │
│                                                  │
└──────────────────────────────────────────────────┘
```

### 7.3 Uwagi

- Wszystkie przyciski: **INPUT_PULLUP** (wbudowany pull-up ESP32)
- Debouncing: 50ms opóźnienie w software
- Oznaczenie: Każdy przycisk wyraźnie opisany

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

**Wersja schematów**: 1.0.0
**Data**: 2026-01-23
**Inżynier**: MT220126 Engineering Team

---

**UWAGA**: Schematy mają charakter poglądowy. Przed wdrożeniem produkcyjnym zaleca się weryfikację przez certyfikowanego elektryka/elektronika.
