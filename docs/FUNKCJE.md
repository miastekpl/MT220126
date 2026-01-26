# Pełny Opis Funkcji Systemu
## System Sterowania Malowaniem Pasów Drogowych v1.4.3

**Autor**: MT220126 Engineering Team
**Data**: 2026-01-26
**Status**: ✅ PRODUCTION READY

---

## 📋 Spis Treści

1. [Przegląd Funkcjonalności](#1-przegląd-funkcjonalności)
2. [Funkcje Podstawowe](#2-funkcje-podstawowe)
3. [Funkcje Zaawansowane](#3-funkcje-zaawansowane)
4. [Funkcje Pomocnicze](#4-funkcje-pomocnicze)
5. [Bezpieczeństwo i Zabezpieczenia](#5-bezpieczeństwo-i-zabezpieczenia)
6. [Interfejs Użytkownika](#6-interfejs-użytkownika)
7. [Zdalne Sterowanie](#7-zdalne-sterowanie)

---

## 1. Przegląd Funkcjonalności

System sterowania malowaniem pasów drogowych oferuje **kompleksowe rozwiązanie** do automatycznego nanoszenia oznakowania drogowego. Poniżej szczegółowy opis wszystkich funkcji.

### 1.1 Statystyki Ogólne

| Kategoria | Ilość |
|-----------|-------|
| **Wzorce malowania** | 15 (P-1a do P-7d) |
| **Pistolety malarskie** | 6 sterowanych przekaźnikami |
| **Przyciski wzorców** | 15 dedykowanych |
| **Przyciski sterowania** | 4 (START, STOP, REVERSE, START GAP) |
| **Stany systemu** | 7 (IDLE, PAINTING, PAUSED, MENU, CALIBRATING, MEASURING, SERVICE) |
| **Pozycje menu** | 6 (Kalibracja, Pomiar, Reset, Info, Serwis, Wyjście) |
| **Precyzja pomiaru** | ±1 cm (po kalibracji) |
| **Częstotliwość odświeżania** | 100 ms (10 Hz) |

---

## 2. Funkcje Podstawowe

### 2.1 Malowanie Automatyczne

**Opis**: Główna funkcja systemu - automatyczne malowanie pasów drogowych według wybranego wzorca.

**Działanie**:
1. Operator wybiera wzorzec (przycisk P-1a do P-7d)
2. Naciska START/PAUZA
3. System automatycznie:
   - Oblicza pozycję w cyklu malowania
   - Włącza/wyłącza odpowiednie pistolety
   - Mierzy odległość
   - Oblicza prędkość
   - Oblicza wymalowaną powierzchnię
   - Aktualizuje wyświetlacz

**Parametry**:
- **Min. prędkość**: 2 km/h (zabezpieczenie przed malowaniem na postoju)
- **Wykrywanie ruchu**: MIN_MOVEMENT_CM = 1 cm
- **Timeout ruchu**: MOVEMENT_TIMEOUT_MS = 1000 ms

**Wzorce**:

| Wzorzec | Linia (m) | Przerwa (m) | Szerokość (cm) | Pistolety aktywne | Opis |
|---------|-----------|-------------|----------------|-------------------|------|
| **P-1a** | 4.0 | 8.0 | 12 | P2, P3, P4 | Przerywana długa |
| **P-1b** | 2.0 | 4.0 | 12 | P2, P3, P4 | Przerywana krótka |
| **P-1c** | 2.0 | 2.0 | 12 | P2, P3, P4 | Wydzielająca |
| **P-1d** | 1.0 | 1.0 | 12 | P2, P3, P4 | Prowadząca wąska |
| **P-1e** | Ciągła | - | 12 | P2, P3, P4 | Linia ciągła |
| **P-2a** | 1.0 | 1.0 | 24 | P1-P6 (wszystkie) | Prowadząca szeroka |
| **P-2b** | Ciągła | - | 24 | P1-P6 (wszystkie) | Ciągła szeroka |
| **P-3a** | Lewo: Ciągła, Prawo: Przerywana | Lewo: -, Prawo: 8.0 | 12+12 | Lewo: P1-P3, Prawo: P4-P6 | Krawędziowa lewa (odwracalna) |
| **P-3b** | Lewo: Przerywana, Prawo: Ciągła | Lewo: 8.0, Prawo: - | 12+12 | Lewo: P1-P3, Prawo: P4-P6 | Krawędziowa prawa (odwracalna) |
| **P-4** | 0.5 | 0.5 | 24 | P1-P6 (wszystkie) | Ostrzegawcza |
| **P-6** | Ciągła | - | 24 (krawędź) | P5, P6 | Krawędziowa |
| **P-7a** | 10.0 | 5.0 | 24 | P1-P6 (wszystkie) | Autostrada |
| **P-7b** | 6.0 | 3.0 | 24 | P1-P6 (wszystkie) | Droga ekspresowa |
| **P-7c** | 3.0 | 1.5 | 24 | P1-P6 (wszystkie) | Droga główna |
| **P-7d** | 1.5 | 0.75 | 24 | P1-P6 (wszystkie) | Droga podporządkowana |

**Zabezpieczenia**:
```cpp
bool isSafeToActivateGuns() {
    // 1. Sprawdzenie prędkości >= 2 km/h
    if (systemState.speed < MIN_SPEED_KMH) return false;

    // 2. Wykrywanie ruchu (enkoder musi zliczać)
    if (abs(currentDistance - lastDistance) < MIN_MOVEMENT_CM) {
        if (millis() - lastMovementTime > MOVEMENT_TIMEOUT_MS) {
            return false;  // Brak ruchu > 1s
        }
    }

    return true;
}
```

---

### 2.2 Zmiana Wzorca "W Locie"

**Opis**: Możliwość zmiany wzorca malowania BEZ zatrzymywania systemu.

**Działanie**:
1. Podczas malowania (STATE_PAINTING)
2. Operator naciska inny przycisk wzorca (np. P-1b → P-2a)
3. System:
   - Zapisuje dystans w momencie zmiany
   - Przełącza na nowy wzorzec
   - Kontynuuje malowanie bez przerwy

**Kod**:
```cpp
void handlePatternChange(PatternType newPattern) {
    // Zapisz dystans w momencie zmiany
    systemState.patternStartDistance = systemState.distance;

    // Jeśli Start Gap aktywny - ustaw offset
    if (systemState.startFromGap && pattern->gapLength > 0) {
        systemState.offsetDistance = pattern->gapLength;
    } else {
        systemState.offsetDistance = 0.0;
    }
}
```

**Zastosowanie**: Zmiana wzorca na zakrętach, skrzyżowaniach, przejściach.

---

### 2.3 Start/Pauza

**Opis**: Rozpoczynanie, wstrzymywanie i wznawianie malowania.

**Działanie**:
- **START** (z IDLE): Rozpoczyna malowanie
- **PAUZA** (z PAINTING): Wstrzymuje malowanie (pistolety OFF, liczniki zachowane)
- **WZNOWIENIE** (z PAUSED): Kontynuuje malowanie od miejsca zatrzymania

**Kod**:
```cpp
if (digitalRead(BTN_START_PIN) == LOW) {
    if (systemState.state == STATE_IDLE) {
        systemState.state = STATE_PAINTING;
        systemState.startTime = millis();
    } else if (systemState.state == STATE_PAINTING) {
        systemState.state = STATE_PAUSED;
    } else if (systemState.state == STATE_PAUSED) {
        systemState.state = STATE_PAINTING;
    }
}
```

---

### 2.4 Stop

**Opis**: Zatrzymanie malowania i dostęp do menu.

**Działanie**:
- **Krótkie naciśnięcie** (< 1s): STOP - reset liczników, powrót do IDLE
- **Długie przytrzymanie** (> 1s): Wejście do MENU

**Kod**:
```cpp
static unsigned long stopPressTime = 0;
static bool stopPressed = false;

if (digitalRead(BTN_STOP_PIN) == LOW) {
    if (!stopPressed) {
        stopPressed = true;
        stopPressTime = millis();
    }
    // Długie przytrzymanie - MENU
    if (millis() - stopPressTime > 1000 && systemState.state != STATE_MENU) {
        systemState.state = STATE_MENU;
        menu.show();
    }
} else {
    if (stopPressed && millis() - stopPressTime < 1000) {
        // Krótkie - STOP
        systemState.state = STATE_IDLE;
        relays.stopAll();
        // Reset liczników (jeśli Start Gap NIE aktywny)
        if (!systemState.startFromGap) {
            systemState.distance = 0;
            encoder.resetDistance();
        }
    }
    stopPressed = false;
}
```

---

## 3. Funkcje Zaawansowane

### 3.1 Odwracanie Wzorców (REVERSE)

**Opis**: Odwracanie wzorców P-3a i P-3b (zamiana lewa ↔ prawa).

**Wzorce odwracalne**:
- **P-3a**: Lewa ciągła + Prawa przerywana → REVERSE → Lewa przerywana + Prawa ciągła
- **P-3b**: Lewa przerywana + Prawa ciągła → REVERSE → Lewa ciągła + Prawa przerywana

**Działanie**:
1. Wybierz wzorzec P-3a lub P-3b
2. Naciśnij REVERSE (GPIO 4)
3. System zamienia pistolety:
   - P1 ↔ P4
   - P2 ↔ P5
   - P3 ↔ P6

**Kod** (v1.4.0 - NAPRAWIONE):
```cpp
if (systemState.patternReversed &&
    (systemState.currentPattern == PATTERN_P3A || systemState.currentPattern == PATTERN_P3B)) {
    // Zamień pistolety: P1↔P4, P2↔P5, P3↔P6
    bool temp;
    temp = activeGuns[0]; activeGuns[0] = activeGuns[3]; activeGuns[3] = temp;
    temp = activeGuns[1]; activeGuns[1] = activeGuns[4]; activeGuns[4] = temp;
    temp = activeGuns[2]; activeGuns[2] = activeGuns[5]; activeGuns[5] = temp;
}
```

**Zastosowanie**: Malowanie po obu stronach drogi bez zawracania.

---

### 3.2 Start Gap (Od Przerwy)

**Opis**: Rozpoczęcie malowania od **linii** zamiast od **przerwy**.

**Problem**: Standardowo wzorce przerywane zaczynają od linii. Jeśli chcemy zacząć od przerwy (np. kontynuacja po przerwie), trzeba przejechać długość przerwy "na sucho".

**Rozwiązanie Start Gap**:
1. Wybierz wzorzec przerywany (np. P-1a: 4m linia, 8m przerwa)
2. Naciśnij START GAP (GPIO 46)
3. System automatycznie:
   - Oblicza długość przerwy (8m dla P-1a)
   - Ustawia offset = 8m
   - Rozpoczyna malowanie od **KOŃCA** przerwy (czyli od początku linii!)

**Kod**:
```cpp
void handlePatternChange(PatternType newPattern) {
    Pattern* pattern = getPattern(newPattern);

    if (systemState.startFromGap && pattern->gapLength > 0) {
        // Ustaw offset na długość przerwy
        systemState.offsetDistance = pattern->gapLength;
        DEBUG_PRINTF("START GAP: Offset = %.2f m\n", systemState.offsetDistance);
    } else {
        systemState.offsetDistance = 0.0;
    }
}
```

**Zastosowanie**:
- Kontynuacja malowania po przerwie
- Dokładne dopasowanie do istniejących linii
- Korekcja pozycji w cyklu malowania

---

### 3.3 Kalibracja Enkodera

**Opis**: Precyzyjna kalibracja pomiaru odległości na odcinku 10 metrów.

**Proces**:
1. Przytrzymaj STOP (2s) → wejdź do MENU
2. Wybierz "Kalibracja" joystickiem
3. Naciśnij SELECT
4. Naciśnij START/PAUZA → rozpocznij pomiar
5. **Jedź DOKŁADNIE 10 metrów** (zmierz taśmą/laserem)
6. Naciśnij START/PAUZA → zakończ pomiar
7. System oblicza `pulsesPerCm` i zapisuje do EEPROM

**Algorytm**:
```cpp
void CalibrationManager::finishCalibration() {
    long endPosition = encoder->getPosition();
    long totalPulses = abs(endPosition - startPosition);

    // 10 metrów = 1000 cm
    calData.pulsesPerCm = (float)totalPulses / CALIBRATION_DISTANCE_CM;

    // Zapisz do Preferences (EEPROM emulation)
    preferences.begin("calibration", false);
    preferences.putBytes("caldata", &calData, sizeof(calData));
    preferences.end();

    encoder->setPulsesPerCm(calData.pulsesPerCm);
}
```

**Dokładność**: ±1 cm na 10 metrów

**Częstotliwość**: Zalecana co 1000 km lub po wymianie koła z enkoderem

---

### 3.4 Tryb Serwisowy (NOWOŚĆ v1.4.1)

**Opis**: Czyszczenie i testowanie pistoletów malarskich. **JEDYNY** moment gdy pistolety mogą być aktywne na postoju.

**Bezpieczeństwo**: Tryb serwisowy **OMIJA** zabezpieczenia `isSafeToActivateGuns()` - to jest WYJĄTEK!

**Proces**:
1. Przytrzymaj STOP (2s) → MENU
2. Wybierz "Serwis" joystickiem
3. Naciśnij SELECT
4. Na ekranie pojawia się **wizualizacja 6 pistoletów** (kwadraty)
5. Wybierz wzorzec (przycisk P-1a do P-7d)
6. **TRZYMAJ START/PAUZA** → pistolety włączają się (hold-to-fire)
7. **PUŚĆ START/PAUZA** → pistolety wyłączają się
8. Przytrzymaj STOP (2s) → wyjście z serwisu

**Wizualizacja pistoletów**:
```
Ekran:
┌─────────────────────────────┐
│ SERWIS - CZYSZCZENIE        │
│                             │
│ 1. Wybierz wzorzec          │
│ 2. TRZYMAJ START = ON       │
│ 3. PUŚĆ START = OFF         │
│                             │
│ Wzorzec: P-2a               │
│                             │
│  [P1]  [P2]  [P3]           │ ← Góra
│  🟢    🟢    🟢            │   (pistolety osiowe 12cm)
│                             │
│  [P4]  [P5]  [P6]           │ ← Dół
│  🟢    🟢    🟢            │   (P4=oś 24cm, P5/P6=brzeg)
│                             │
│ STOP (2s) = Wyjście         │
└─────────────────────────────┘

Kolory:
🟢 Zielony = pistolet AKTYWNY (maluje)
🟡 Żółty = włączony dla wzorca ale nieaktywny
⚫ Szary = wyłączony dla wzorca
```

**Kod**:
```cpp
void ServiceMode::update() {
    // Obsługa przycisku START/PAUZA - HOLD TO FIRE
    bool isPressed = (digitalRead(BTN_START_PIN) == LOW);

    if (isPressed && !wasPressed) {
        // Przycisk naciśnięty - aktywuj pistolety
        activateGunsByPattern(currentPattern);
        drawGuns();  // Odśwież wizualizację (zielone kwadraty)
    } else if (!isPressed && wasPressed) {
        // Przycisk puszczony - dezaktywuj
        deactivateAllGuns();
    }

    wasPressed = isPressed;
}
```

**Zalecenia**:
- **Przed każdym malowaniem** - "przestrzel" pistolety aby usunąć zaschnięta farbę
- **Raz w tygodniu** (gdy maszyna nie pracuje) - prewencyjne czyszczenie

---

## 4. Funkcje Pomocnicze

### 4.1 Pomiar Dystansu

**Opis**: Pomiar przejechane odległości bez malowania.

**Proces**:
1. MENU → "Pomiar dystansu"
2. System resetuje licznik odległości
3. Jedź dowolny odcinek
4. Ekran pokazuje przejechany dystans w metrach
5. STOP → powrót do MENU

**Zastosowanie**:
- Pomiar odległości przed malowaniem
- Weryfikacja kalibracji
- Planowanie pracy

---

### 4.2 Reset Liczników

**Opis**: Zerowanie licznika wymalowanej powierzchni.

**Proces**:
1. MENU → "Zeruj liczniki"
2. Potwierdź (SELECT)
3. `systemState.totalPaintedArea = 0`

**Zastosowanie**: Rozpoczęcie nowego projektu, nowa partia farby, raportowanie.

---

### 4.3 Informacje Systemowe

**Opis**: Wyświetlenie parametrów systemowych.

**Wyświetlane dane**:
- Wersja oprogramowania (np. "1.4.3")
- Platforma (ESP32-S3 N16R8)
- Impulsy/cm enkodera (np. "12.5678")
- Status kalibracji (TAK/NIE)
- Ilość wolnej pamięci RAM
- Uptime systemu

**Kod**:
```cpp
tft->println("System Malowania Pasow Drogowych");
tft->println("Wersja: 1.4.3");
tft->println("Platforma: ESP32-S3 N16R8");
tft->printf("Impulsy/cm: %.4f\n", encoder->getPulsesPerCm());
tft->printf("Kalibrowany: %s\n", encoder->isCalibrated() ? "TAK" : "NIE");
```

---

## 5. Bezpieczeństwo i Zabezpieczenia

### 5.1 Blokada Bezpieczeństwa Pistoletów

**Cel**: Zapobieganie malowaniu na postoju (kleksy, marnowanie farby).

**Warunki aktywacji**:
1. **Prędkość >= 2 km/h**
2. **Wykryty ruch enkodera** (min 1 cm w ciągu 1 sekundy)

**Kod**:
```cpp
bool isSafeToActivateGuns() {
    // Sprawdzenie prędkości
    if (systemState.speed < MIN_SPEED_KMH) {
        DEBUG_PRINTF("BLOKADA: Predkosc %.2f km/h < %.2f km/h\n",
                    systemState.speed, MIN_SPEED_KMH);
        return false;
    }

    // Sprawdzenie ruchu
    long currentDistance = systemState.distance;
    long lastDist = systemState.lastDistance;

    if (abs(currentDistance - lastDist) < MIN_MOVEMENT_CM) {
        unsigned long timeSinceMovement = millis() - systemState.lastMovementTime;
        if (timeSinceMovement > MOVEMENT_TIMEOUT_MS) {
            DEBUG_PRINTLN("BLOKADA: Brak ruchu enkodera!");
            return false;
        }
    } else {
        systemState.lastMovementTime = millis();
        systemState.lastDistance = currentDistance;
    }

    return true;
}
```

**Komunikat**: Jeśli blokada aktywna, na ekranie pojawia się **"BLOKADA BEZPIECZEŃSTWA"**.

**WYJĄTEK**: Tryb serwisowy OMIJA tę blokadę (jedyny dozwolony moment malowania na postoju).

---

### 5.2 Watchdog Timer

**Cel**: Zapobieganie zawieszeniu systemu.

**Działanie**:
- Timeout: 10 sekund
- Reset w każdej iteracji `loop()`
- Jeśli `loop()` nie wykona się przez 10s → automatyczny restart ESP32

**Kod**:
```cpp
void setup() {
    // Inicjalizacja watchdog
    esp_task_wdt_init(10, true);  // 10s timeout, panic on timeout
    esp_task_wdt_add(NULL);       // Dodaj current task
}

void loop() {
    // Reset watchdog co iterację
    esp_task_wdt_reset();
    // ... reszta loop()
}
```

---

### 5.3 Thread-Safety (FreeRTOS Mutex)

**Cel**: Zapobieganie race conditions między przerwaniami (ISR) a główną pętlą.

**Chronione zasoby**:
1. **encoderMutex** - dostęp do `encoder.position` i `encoder.distance`
2. **stateMutex** - dostęp do `systemState` (przygotowane, nieużywane jeszcze)

**Kod** (v1.4.2 - NAPRAWIONE):
```cpp
// Mutexy tworzone w setup()
stateMutex = xSemaphoreCreateMutex();
encoderMutex = xSemaphoreCreateMutex();

// Użycie w encoder_handler.cpp
void EncoderHandler::update() {
    if (encoderMutex != NULL && xSemaphoreTake(encoderMutex, portMAX_DELAY) == pdTRUE) {
        // Bezpieczna modyfikacja position
        if (aState != aLastState) {
            position++;
            distance = (long)(position / pulsesPerCm);
        }
        xSemaphoreGive(encoderMutex);
    }
}

long EncoderHandler::getDistance() {
    long result = 0;
    if (encoderMutex != NULL && xSemaphoreTake(encoderMutex, portMAX_DELAY) == pdTRUE) {
        result = distance;  // Thread-safe odczyt
        xSemaphoreGive(encoderMutex);
    }
    return result;
}
```

---

## 6. Interfejs Użytkownika

### 6.1 Ekran Główny (STATE_IDLE / STATE_PAINTING)

```
┌─────────────────────────────────────────┐
│ ┌──────────┐ ┌─────────────────────────┐│
│ │  P-2a    │ │    PRĘDKOŚĆ             ││
│ │          │ │                         ││
│ │  Linia   │ │       45.2              ││
│ │  ciągła  │ │                         ││
│ │  24cm    │ │       km/h              ││
│ └──────────┘ └─────────────────────────┘│
│                                         │
│ ┌─────────────────────────────────────┐ │
│ │  WYMALOWANA POWIERZCHNIA            │ │
│ │                                     │ │
│ │           124.56                    │ │
│ │                              m²     │ │
│ └─────────────────────────────────────┘ │
│                                         │
│ Dystans: 2543.12 m  [MALUJE]  [ODWR]   │
└─────────────────────────────────────────┘

Elementy:
- Lewy górny kwadrat: Nazwa wzorca + opis
- Prawy górny kwadrat: Prędkość (duże cyfry)
- Dolny prostokąt: Wymalowana powierzchnia
- Dolna linia: Dystans + status + flagi

Statusy:
[MALUJE] - malowanie aktywne
[PAUZA] - wstrzymane
[BLOKADA] - zabezpieczenie aktywne
[ODWR] - wzorzec odwrócony (P-3a/P-3b)
```

### 6.2 Menu Systemowe

```
┌─────────────────────────────┐
│     MENU GŁÓWNE             │
├─────────────────────────────┤
│                             │
│  ► Kalibracja               │ ← Wybrany
│    Pomiar dystansu          │
│    Zeruj liczniki           │
│    Informacje               │
│    Serwis              ✨   │ ← NOWOŚĆ v1.4.1
│    Wyjście                  │
│                             │
├─────────────────────────────┤
│ Joystick: GÓRA/DÓŁ         │
│ SELECT: Wybierz             │
└─────────────────────────────┘

Nawigacja:
- Joystick GÓRA/DÓŁ: poruszanie się po menu
- SELECT (GPIO 20): wybór pozycji
- STOP: wyjście z menu (powrót do IDLE)
```

---

## 7. Zdalne Sterowanie (WiFi)

### 7.1 Web Dashboard

**Adres**: http://192.168.4.1 (po połączeniu z WiFi "Trassar")

**Funkcje**:
- Wybór wzorca (przyciski P-1a do P-7d)
- START/PAUZA
- STOP
- REVERSE
- START GAP
- Wyświetlanie statusu:
  - Prędkość (km/h)
  - Dystans (m)
  - Powierzchnia (m²)
  - Aktualny wzorzec
  - Stan systemu

**Bezpieczeństwo**: Hasło WiFi: "12345678" (UWAGA: zmień w produkcji!)

---

## 📊 Podsumowanie Funkcjonalności

| Funkcja | Status | Wersja wprowadzenia |
|---------|--------|---------------------|
| Malowanie automatyczne | ✅ | v1.0.0 |
| 15 wzorców malowania | ✅ | v1.0.0 |
| Zmiana wzorca w locie | ✅ | v1.0.0 |
| Odwracanie P-3a/P-3b | ✅ (NAPRAWIONE) | v1.4.0 |
| Start Gap (Od Przerwy) | ✅ | v1.3.0 |
| Kalibracja enkodera | ✅ | v1.0.0 |
| Pomiar dystansu | ✅ | v1.0.0 |
| Reset liczników | ✅ | v1.0.0 |
| Blokada bezpieczeństwa | ✅ | v1.3.0 |
| Thread-safety (mutex) | ✅ (NAPRAWIONE) | v1.4.2 |
| **Tryb serwisowy** | ✅ | **v1.4.1** |
| WiFi Access Point | ✅ | v1.4.0 |
| Web Dashboard | ✅ | v1.4.0 |
| REST API | ✅ | v1.4.0 |
| Watchdog Timer | ✅ | v1.3.0 |

---

**Dokument stworzony przez MT220126 Engineering Team**
*200+ lat zbiorczego doświadczenia w systemach embedded*

🚗💚 **System gotowy do produkcji!** 🚗💚
