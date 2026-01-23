# Instrukcja Obsługi
## System Sterowania Malowaniem Pasów Drogowych v1.4.0

---

## 🎉 CO NOWEGO w v1.4.0

### Najważniejsze Ulepszenia:

✅ **Naprawione Konflikty GPIO**
- Wszystkie przyciski działają poprawnie
- Rozwiązano problemy z ADC podczas pracy WiFi
- Stabilne działanie wszystkich funkcji

✅ **Przycisk REVERSE Działa Poprawnie**
- Odwracanie wzorców P-3a i P-3b działa bez problemu
- Nowy przycisk dedykowany na GPIO 33

✅ **Start Gap (Od Przerwy) Stabilny**
- Funkcja Start Gap działa niezawodnie
- Brak przypadkowych włączeń
- Dedykowany GPIO 25

✅ **Zdalne Sterowanie przez WiFi**
- Nowa funkcja: **panel WWW do zdalnego sterowania**
- Połącz się z WiFi "Trassar" (hasło: 12345678)
- Dashboard w przeglądarce: http://192.168.4.1
- Steruj systemem ze smartfona lub tabletu
- Zobacz status w czasie rzeczywistym

### Dlaczego warto zaktualizować?
- **Większa niezawodność** - wszystkie przyciski działają stabilnie
- **Więcej możliwości** - steruj systemem zdalnie
- **Mniej problemów** - rozwiązane konflikty sprzętowe

---

## 📖 Spis Treści

1. [Wstęp](#1-wstęp)
2. [Bezpieczeństwo](#2-bezpieczeństwo)
3. [Pierwsze Uruchomienie](#3-pierwsze-uruchomienie)
4. [Kalibracja](#4-kalibracja)
5. [Obsługa Podstawowa](#5-obsługa-podstawowa)
6. [Menu Systemowe](#6-menu-systemowe)
7. [Wzorce Malowania](#7-wzorce-malowania)
8. [Zdalne Sterowanie przez WiFi](#8-zdalne-sterowanie-przez-wifi) ⭐ **NOWOŚĆ!**
9. [Pomiar Dystansu](#9-pomiar-dystansu)
10. [Rozwiązywanie Problemów](#10-rozwiązywanie-problemów)
11. [Konserwacja](#11-konserwacja)

---

## 1. Wstęp

System Sterowania Malowaniem Pasów Drogowych to profesjonalne urządzenie do automatycznego nanoszenia oznakowania drogowego. System steruje 6 pistoletami malarskimi i obsługuje 15 wzorców zgodnych z normami.

### 1.1 Komponenty

- **Wyświetlacz** - TFT 320x240 pokazujący wszystkie informacje
- **Przyciski wzorców** - 15 przycisków do wyboru wzorca (P-1a do P-7d)
- **START/PAUZA** - Jeden przycisk do uruchamiania i wstrzymywania
- **STOP** - Przycisk zatrzymania i dostępu do menu
- **REVERSE** - Przycisk odwracania wzorców P-3a/P-3b (GPIO 33)
- **START GAP** - Przycisk "Od Przerwy" (GPIO 25)
- **Joystick** - Nawigacja w menu
- **Enkoder** - Czujnik odległości (automatyczny)
- **WiFi** - Moduł do zdalnego sterowania ⭐ **NOWOŚĆ v1.4.0**

---

## 2. Bezpieczeństwo

### ⚠️ OSTRZEŻENIA

- **Przed uruchomieniem** upewnij się, że wszystkie pistolety są prawidłowo podłączone
- **Nie dotykaj** pistoletów podczas pracy systemu
- **Zachowaj ostrożność** przy obsłudze farby i rozpuszczalników
- **Noś środki ochrony** - rękawice, okulary, maskę
- **System działa** tylko przy włączonym silniku pojazdu (zabezpieczenie przed rozładowaniem)
- **WiFi**: Nie używaj zdalnego sterowania podczas jazdy! (tylko dla pasażera/drugiego operatora)

### ✅ ZALECENIA

- Regularnie sprawdzaj poziom farby
- Przed rozpoczęciem pracy zrób test na krótkim odcinku
- Kalibruj system co 1000 km
- Czyść pistolety po każdym użyciu
- Używaj WiFi tylko gdy pojazd stoi lub masz drugiego operatora

---

## 3. Pierwsze Uruchomienie

### 3.1 Podłączenie Zasilania

1. Podłącz zasilanie **12V DC** do systemu
2. Sprawdź czy dioda zasilania świeci
3. Wyświetlacz powinien się włączyć i pokazać ekran powitalny
4. System automatycznie uruchomi sieć WiFi "Trassar"

### 3.2 Sprawdzenie Systemu

Po włączeniu system automatycznie:
- Testuje wszystkie przekaźniki (sekwencja 1-6)
- Sprawdza kalibrację enkodera
- Uruchamia sieć WiFi w trybie Access Point
- Wyświetla ekran główny

### 3.3 Ekran Główny

```
┌────────────────────┬──────────────────────┐
│ Wzorzec: P-1a     │   Prędkość:          │
│                    │                      │
│ Przerywana długa   │     12.5 km/h        │
│                    │                      │
└────────────────────┴──────────────────────┘
│                                            │
│    Wymalowana powierzchnia:                │
│                                            │
│           45.75 m²                         │
│                                            │
└────────────────────────────────────────────┘
 Dystans: 152.35 m      Status: GOTOWY
```

**Elementy ekranu:**
- **Lewy górny róg**: Aktualny wzorzec i jego opis
- **Prawy górny róg**: Prędkość pojazdu w km/h
- **Centrum**: Wymalowana powierzchnia w m²
- **Dół**: Dystans i status systemu

---

## 4. Kalibracja

### ⚠️ WAŻNE: System WYMAGA kalibracji przed pierwszym użyciem!

### 4.1 Kiedy Kalibrować?

- **Obowiązkowo**: Przed pierwszym użyciem
- **Zalecane**: Co 1000 km
- **Konieczne**: Po wymianie koła lub enkodera

### 4.2 Procedura Kalibracji

#### Krok 1: Wejście do Menu
1. **Przytrzymaj** przycisk **STOP** przez **1 sekundę**
2. Pojawi się menu główne

#### Krok 2: Wybór Kalibracji
1. Używając **joysticka** (góra/dół) wybierz pozycję **"Kalibracja"**
2. **Naciśnij joystick** aby potwierdzić

#### Krok 3: Przygotowanie
1. Znajdź **równy** odcinek drogi
2. Odmierz **dokładnie 10 metrów** (użyj miarki lub taśmy)
3. Ustaw pojazd na początku odcinka

#### Krok 4: Kalibracja
1. Naciśnij **START**
2. Ekran pokaże: *"Przejedź dokładnie 10 metrów"*
3. **Jedź powoli** przez odmierzony odcinek (10m)
4. **Zatrzymaj się** dokładnie na końcu 10-metrowego odcinka
5. Naciśnij **START** ponownie

#### Krok 5: Zakończenie
1. System obliczy kalibrację
2. Pojawi się komunikat: *"Kalibracja zakończona!"*
3. Po 2 sekundach powrót do ekranu głównego

### 4.3 Weryfikacja Kalibracji

Po kalibracji sprawdź dokładność:
1. Wejdź do **Menu** → **"Pomiar dystansu"**
2. Przejedź znany odcinek (np. 100m)
3. Sprawdź czy wskazanie się zgadza (dopuszczalne ±1cm)

---

## 5. Obsługa Podstawowa

### 5.1 Wybór Wzorca

1. **Naciśnij przycisk** wzorca (P-1a do P-7d)
2. Ekran automatycznie pokaże wybrany wzorzec
3. Możesz zmieniać wzorce **"w locie"** podczas malowania!

### 5.2 Rozpoczęcie Malowania

#### Tryb Normalny (Od Początku)
1. Upewnij się, że wybrany jest właściwy wzorzec
2. Naciśnij **START/PAUZA**
3. Status zmieni się na **"MALOWANIE"**
4. Jedź zgodnie z planem malowania
5. System automatycznie:
   - Włącza/wyłącza pistolety wg wzorca
   - Zlicza dystans
   - Oblicza wymalowaną powierzchnię
   - Pokazuje prędkość

#### Tryb "Od Przerwy" (Start Gap) ⭐ **ULEPSZONE v1.4.0**
Funkcja do kontynuowania wzorca od przerwy (gap):

1. Wybierz wzorzec przerywany (np. P-1a)
2. **Naciśnij START GAP** (GPIO 25) zamiast START/PAUZA
3. System rozpocznie malowanie od **przerwy**, a nie od linii
4. Idealne do:
   - Dokończenia linii po zatrzymaniu
   - Korekty malowania
   - Łączenia odcinków

**Ważne:** Start Gap działa teraz **stabilnie** bez przypadkowych włączeń!

### 5.3 Wstrzymanie (Pauza)

1. Naciśnij **START/PAUZA** podczas malowania
2. Status zmieni się na **"PAUZA"**
3. Wszystkie pistolety się wyłączą
4. Liczniki zostają zamrożone
5. Naciśnij **START/PAUZA** ponownie aby wznowić

### 5.4 Zatrzymanie

1. Naciśnij **STOP** (krótko, < 1 sekunda)
2. Status zmieni się na **"GOTOWY"**
3. Wszystkie pistolety wyłączone
4. Liczniki wyzerowane

### 5.5 Odwracanie Wzorców P-3a/P-3b ⭐ **NAPRAWIONE v1.4.0**

Wzorce P-3a i P-3b (przekraczalne) można odwrócić:

**Standardowo:** Linia ciągła po lewej stronie
```
═══════════  Ciągła (lewa)
─ ─ ─ ─ ─   Przerywana (prawa)
```

**Po naciśnięciu REVERSE:** Linia ciągła po prawej
```
─ ─ ─ ─ ─   Przerywana (lewa)
═══════════  Ciągła (prawa)
```

1. Wybierz wzorzec P-3a lub P-3b
2. Naciśnij przycisk **REVERSE** (GPIO 33)
3. Na ekranie pojawi się napis **[ODWR]**
4. Naciśnij **REVERSE** ponownie aby wrócić do standardu

**NOWOŚĆ v1.4.0:** Przycisk REVERSE ma teraz dedykowany GPIO 33 i działa **stabilnie**!

---

## 6. Menu Systemowe

### 6.1 Wejście do Menu

**Przytrzymaj** przycisk **STOP** przez **1 sekundę**

### 6.2 Nawigacja

- **Joystick GÓRA/DÓŁ**: Wybór pozycji
- **Naciśnij joystick**: Potwierdzenie
- **Przytrzymaj STOP (2 sek)**: Wyjście z menu

### 6.3 Pozycje Menu

#### 📍 Kalibracja
Przeprowadza kalibrację enkodera na 10 metrów (patrz rozdział 4)

#### 📍 Pomiar dystansu
Tryb pomiaru odległości bez malowania:
1. Wybierz tę opcję
2. Naciśnij **START**
3. Jedź dowolny odcinek
4. Przeczytaj dokładny dystans na ekranie
5. Naciśnij **STOP** aby zakończyć

#### 📍 Zeruj liczniki
Resetuje licznik powierzchni i dystansu do zera

#### 📍 Informacje
Pokazuje informacje o systemie:
- Wersja oprogramowania (1.4.0)
- Platforma sprzętowa (ESP32-S3)
- Status kalibracji
- Impulsy/cm enkodera
- Status WiFi (IP: 192.168.4.1)

#### 📍 Wyjście
Powrót do ekranu głównego

---

## 7. Wzorce Malowania

### 7.1 Lista Wszystkich Wzorców

| Przycisk | Wzorzec | Linia | Przerwa | Szerokość | Zastosowanie |
|----------|---------|-------|---------|-----------|--------------|
| **P-1a** | Przerywana długa | 4m | 8m | 12cm | Linia środkowa, możliwość wyprzedzania |
| **P-1b** | Przerywana krótka | 2m | 4m | 12cm | Linia ostrzegawcza przed zakazem |
| **P-1c** | Wydzielająca | 2m | 2m | 12cm | Wydzielenie pasa ruchu |
| **P-1d** | Prowadząca wąska | 1m | 1m | 12cm | Prowadzenie w łukach |
| **P-1e** | Prowadząca szeroka | 1m | 1m | 24cm | Prowadzenie w łukach (szeroka) |
| **P-2a** | Ciągła wąska | Ciągła | - | 12cm | Zakaz przekraczania |
| **P-2b** | Ciągła szeroka | Ciągła | - | 24cm | Zakaz przekraczania (wzmocniona) |
| **P-3a** | Przekraczalna długa * | 4m | 2m | 12cm | Linia mieszana |
| **P-3b** | Przekraczalna krótka * | 1m | 1m | 12cm | Linia mieszana krótka |
| **P-4** | Podwójna ciągła | Ciągła | - | 2x12cm | Podwójny zakaz |
| **P-6** | Ostrzegawcza | 4m | 2m | 12cm | Ostrzeżenie przed przeszkodą |
| **P-7a** | Krawędziowa przeryw. szer. | 1m | 1m | 24cm | Krawędź jezdni |
| **P-7b** | Krawędziowa ciągła szer. | Ciągła | - | 24cm | Krawędź autostrady |
| **P-7c** | Krawędziowa przeryw. wąska | 1m | 1m | 12cm | Krawędź jezdni (wąska) |
| **P-7d** | Krawędziowa ciągła wąska | Ciągła | - | 12cm | Krawędź drogi |

\* Wzorce odwracalne (przycisk REVERSE)

### 7.2 Przykłady Użycia

#### Droga Dwukierunkowa - Możliwość Wyprzedzania
```
Pas 1:  ════════  (ciągła krawędziowa P-7d)
Pas 1:  ─  ─  ─  (przerywana środkowa P-1a)
Pas 2:  ════════  (ciągła krawędziowa P-7d)
```
Użyj: **P-7d** (krawędzie) + **P-1a** (środek)

#### Droga Dwukierunkowa - Zakaz Wyprzedzania
```
Pas 1:  ════════  (ciągła krawędziowa)
Pas 1:  ════════  (ciągła środkowa P-2a)
Pas 2:  ════════  (ciągła krawędziowa)
```
Użyj: **P-7d** (krawędzie) + **P-2a** (środek)

#### Linia Przekraczalna (z jednej strony)
```
Pas 1:  ════════  (ciągła - zakaz)
Pas 1:  ─  ─  ─  (przerywana - można)
Pas 2:  pojazd może przekroczyć z Pas 2 → Pas 1
Pas 2:  pojazd NIE MOŻE przekroczyć z Pas 1 → Pas 2
```
Użyj: **P-3a** (standardowo lub REVERSE w zależności od kierunku)

---

## 8. Zdalne Sterowanie przez WiFi ⭐ **NOWOŚĆ v1.4.0!**

### 8.1 Czym Jest Zdalne Sterowanie?

System ma wbudowany moduł WiFi, który pozwala na:
- **Sterowanie systemem** ze smartfona lub tabletu
- **Podgląd statusu** w czasie rzeczywistym
- **Wybór wzorców** bez dotykania przycisków fizycznych
- **Start/Stop/Pauza** z poziomu przeglądarki

### 8.2 Połączenie z Systemem

#### Krok 1: Włączenie Systemu
1. Włącz system (normalnie, jak zawsze)
2. System automatycznie utworzy sieć WiFi
3. Na wyświetlaczu pojawi się potwierdzenie (przy starcie)

#### Krok 2: Połączenie WiFi
1. Weź smartfona lub tablet
2. Otwórz ustawienia WiFi
3. Znajdź sieć: **"Trassar"**
4. Hasło: **"12345678"**
5. Połącz się

⏱️ Połączenie zajmuje 5-10 sekund

#### Krok 3: Otwarcie Dashboardu
1. Otwórz przeglądarkę (Chrome, Safari, Firefox)
2. Wpisz adres: **http://192.168.4.1**
3. Dashboard otworzy się automatycznie

**Gotowe!** Teraz możesz sterować systemem zdalnie.

### 8.3 Dashboard WWW - Jak Wygląda?

Dashboard to prosta strona WWW z dużymi przyciskami:

```
┌─────────────────────────────────────┐
│   TRASSAR - Panel Sterowania       │
├─────────────────────────────────────┤
│                                     │
│  STATUS: GOTOWY                     │
│  Wzorzec: P-1a (Przerywana długa)  │
│  Prędkość: 12.5 km/h                │
│  Dystans: 152.35 m                  │
│  Powierzchnia: 45.75 m²             │
│                                     │
├─────────────────────────────────────┤
│  [  START  ]  [  PAUZA  ]  [STOP]  │
│                                     │
│  WZORCE:                            │
│  [P-1a] [P-1b] [P-1c] [P-1d] ...   │
│                                     │
│  [ REVERSE ]  [ START GAP ]         │
│                                     │
└─────────────────────────────────────┘
```

Wszystko działa na dotyk - **jak aplikacja mobilna!**

### 8.4 Co Można Robić Zdalnie?

#### ✅ Możliwe Operacje:
- **START** - rozpoczęcie malowania
- **PAUZA** - wstrzymanie pracy
- **STOP** - zatrzymanie systemu
- **Wybór wzorca** - wszystkie 15 wzorców
- **REVERSE** - odwrócenie P-3a/P-3b
- **START GAP** - start od przerwy
- **Podgląd statusu** - aktualizacja co 1 sekundę

#### ❌ Czego NIE MOŻNA Robić:
- Kalibracja (tylko fizycznie)
- Reset liczników (tylko przez menu)
- Zmiana ustawień systemowych

### 8.5 Typowe Scenariusze Użycia

#### Scenariusz 1: Operator + Kierowca
```
Kierowca:   Prowadzi pojazd, patrzy na drogę
Operator:   Siedzi obok z tabletem, steruje malowaniem
            - Wybiera wzorce
            - Uruchamia/zatrzymuje
            - Kontroluje powierzchnię
```
**Bezpieczniej!** Kierowca skupia się na prowadzeniu.

#### Scenariusz 2: Korekta z Zewnątrz
```
1. Pojazd stoi
2. Operator wychodzi i sprawdza jakość malowania
3. Z telefonu może:
   - Przetestować inne wzorce
   - Uruchomić START GAP
   - Sprawdzić liczniki
```
**Wygodniej!** Nie trzeba wracać do kabiny.

#### Scenariusz 3: Monitoring dla Nadzoru
```
Nadzorujący:  Łączy się z WiFi
              Widzi dokładne dane:
              - Ile pomalowano (m²)
              - Jaki dystans (m)
              - Aktualna prędkość
```
**Transparentniej!** Pełna kontrola nad pracą.

### 8.6 Bezpieczeństwo WiFi

#### 🔒 Zabezpieczenia:
- Hasło WPA2 ("12345678")
- Sieć lokalna (Access Point) - **NIE MA połączenia z Internetem**
- Tylko 1 urządzenie może sterować jednocześnie
- Dashboard wymaga autoryzacji

#### ⚠️ WAŻNE - Bezpieczeństwo Operacji:
- **NIE UŻYWAJ** zdalnego sterowania podczas jazdy (tylko pasażer!)
- **NIE STERUJ** z dużej odległości (max 20 metrów od pojazdu)
- **ZAWSZE** patrz na ekran fizyczny jako główny
- **WiFi** to dodatek, nie zastępuje fizycznych przycisków

#### 💡 Wskazówki:
- Dashboard działa najlepiej na tabletach (duży ekran)
- Można połączyć kilka urządzeń do **podglądu** (ale steruje tylko jedno)
- Jeśli WiFi nie działa - system nadal działa normalnie!

### 8.7 Zasięg i Jakość Połączenia

| Odległość | Jakość |
|-----------|--------|
| 0-5m | Doskonała ✅ |
| 5-10m | Bardzo dobra ✅ |
| 10-20m | Dobra ⚠️ |
| 20-30m | Słaba ⚠️ (mogą być opóźnienia) |
| >30m | Brak połączenia ❌ |

**Rekomendacja:** Używaj WiFi w odległości max **10 metrów**.

---

## 9. Pomiar Dystansu

### 9.1 Zastosowanie

- Sprawdzenie kalibracji
- Pomiar odległości między punktami
- Weryfikacja długości odcinków
- Kontrola dokładności

### 9.2 Procedura

1. **Przytrzymaj STOP** (1 sek) aby wejść do menu
2. Joystickiem wybierz **"Pomiar dystansu"**
3. **Naciśnij joystick** aby potwierdzić
4. Pojawi się ekran pomiaru z wyzerowanym dystansem
5. **Naciśnij START**
6. **Jedź** przez mierzony odcinek
7. **Zatrzymaj się** na końcu
8. Przeczytaj dokładny dystans na ekranie (z dokładnością do 1cm!)
9. **Przytrzymaj STOP (2 sek)** aby wyjść

---

## 10. Rozwiązywanie Problemów

### 10.1 System Nie Startuje

**Objawy:** Wyświetlacz nie świeci

**Rozwiązanie:**
1. Sprawdź zasilanie 12V
2. Sprawdź bezpieczniki
3. Sprawdź połączenie USB (serwis)

### 10.2 Wyświetlacz Nie Pokazuje Obrazu

**Objawy:** Zasilanie jest, ale ekran czarny

**Rozwiązanie:**
1. Sprawdź jasność ekranu
2. Wyłącz i włącz zasilanie
3. Skontaktuj się z serwisem

### 10.3 Pistolet Nie Maluje

**Objawy:** Wybrany wzorzec, ale pistolet nie działa

**Rozwiązanie:**
1. Sprawdź poziom farby
2. Sprawdź czy pistolet nie jest zatkany
3. Sprawdź połączenia elektryczne do przekaźnika
4. Użyj **Menu → Informacje** aby sprawdzić status

### 10.4 Nieprawidłowy Dystans

**Objawy:** Wskazanie dystansu nie zgadza się z rzeczywistością

**Rozwiązanie:**
1. **Przeprowadź kalibrację ponownie**
2. Sprawdź enkoder - czy jest dobrze zamocowany
3. Sprawdź koło - czy nie ślizga się

### 10.5 Wzorzec Maluje Nieprawidłowo

**Objawy:** Długości linii/przerw nie zgadzają się

**Rozwiązanie:**
1. Sprawdź kalibrację
2. Sprawdź prędkość - nie jedź zbyt szybko (max 20 km/h zalecane)
3. Sprawdź czy enkoder dobrze przylega do koła

### 10.6 System Zależł

**Objawy:** Wyświetlacz nie reaguje, przyciski nie działają

**Rozwiązanie:**
1. **Wyłącz zasilanie**
2. Odczekaj 10 sekund
3. **Włącz zasilanie** ponownie
4. Jeśli problem się powtarza - kontakt z serwisem

### 10.7 Przycisk REVERSE Nie Działa ⭐ **NAPRAWIONE w v1.4.0**

**Objawy:** Naciśnięcie REVERSE nie odwraca wzorca

**Rozwiązanie w v1.4.0:**
- Problem został **całkowicie rozwiązany**
- Przycisk ma teraz dedykowany GPIO 33
- Jeśli nadal nie działa:
  1. Sprawdź czy wybrany jest wzorzec P-3a lub P-3b (tylko te są odwracalne!)
  2. Restart systemu
  3. Kontakt z serwisem

### 10.8 Start Gap Przypadkowo Włącza Się ⭐ **NAPRAWIONE w v1.4.0**

**Objawy:** System rozpoczyna malowanie od przerwy bez powodu

**Rozwiązanie w v1.4.0:**
- Problem został **całkowicie rozwiązany**
- Start Gap ma teraz dedykowany GPIO 25
- Funkcja działa **stabilnie**
- Jeśli nadal występuje:
  1. Restart systemu
  2. Kontakt z serwisem

### 10.9 WiFi Nie Działa ⭐ **NOWOŚĆ v1.4.0**

#### Problem: Nie widzę sieci "Trassar"

**Rozwiązanie:**
1. Sprawdź czy system jest włączony
2. Poczekaj 30 sekund po starcie (WiFi potrzebuje czasu)
3. Sprawdź czy WiFi w telefonie jest włączone
4. Restart systemu
5. Sprawdź **Menu → Informacje** czy WiFi jest aktywne

#### Problem: Nie mogę się połączyć

**Rozwiązanie:**
1. Sprawdź hasło: **"12345678"** (8 cyfr)
2. Spróbuj "zapomnieć" sieć i połączyć ponownie
3. Sprawdź czy inne urządzenie nie jest już połączone
4. Restart telefonu/tabletu

#### Problem: Dashboard nie otwiera się (http://192.168.4.1)

**Rozwiązanie:**
1. Sprawdź czy jesteś połączony z "Trassar"
2. Spróbuj: http://192.168.4.1/ (ze slashem na końcu)
3. Wyczyść cache przeglądarki
4. Spróbuj innej przeglądarki (Chrome, Safari)
5. Restart systemu

#### Problem: Dashboard działa bardzo wolno

**Rozwiązanie:**
1. Zbliż się do pojazdu (max 10m)
2. Sprawdź czy inny telefon nie zabiera połączenia
3. Wyłącz Bluetooth w telefonie (może kolidować)
4. Restart WiFi w telefonie

#### Problem: Przyciski w Dashboardzie nie reagują

**Rozwiązanie:**
1. Odśwież stronę (F5 lub przeciągnij w dół)
2. Sprawdź czy system nie jest w stanie MENU (wtedy zdalne sterowanie jest zablokowane)
3. Sprawdź czy fizyczny przycisk STOP nie jest wciśnięty
4. Restart Dashboardu

---

## 11. Konserwacja

### 11.1 Codziennie (Przed Pracą)

- [ ] Sprawdź poziom farby we wszystkich zbiornikach
- [ ] Sprawdź czy wszystkie pistolety są drożne
- [ ] Sprawdź połączenia elektryczne
- [ ] Sprawdź czy enkoder jest czysty i dobrze zamocowany
- [ ] Zrób test na krótkim odcinku (5m)
- [ ] Sprawdź czy WiFi działa (opcjonalnie)

### 11.2 Co Tydzień

- [ ] Oczyść wyświetlacz miękką szmatką
- [ ] Sprawdź wszystkie połączenia elektryczne
- [ ] Oczyść obudowę systemu
- [ ] Sprawdź stan przycisków (zwłaszcza REVERSE i START GAP)
- [ ] Test zdalnego sterowania WiFi

### 11.3 Co Miesiąc

- [ ] Przeprowadź test wszystkich wzorców
- [ ] Sprawdź kalibrację (pomiar na 100m)
- [ ] Oczyść wszystkie pistolety
- [ ] Sprawdź stan przekaźników
- [ ] Test wszystkich funkcji WiFi

### 11.4 Co 1000 km

- [ ] **OBOWIĄZKOWA rekalibracja**
- [ ] Przegląd wszystkich połączeń elektrycznych
- [ ] Test wszystkich funkcji systemu
- [ ] Wymiana zużytych części (jeśli potrzeba)

### 11.5 Czyszczenie

#### Wyświetlacz:
- Używaj **miękkiej, suchej** szmatki
- **NIE używaj** chemikaliów
- **NIE naciskaj** mocno

#### Przyciski:
- Czyść suchą szmatką
- Możesz użyć sprężonego powietrza

#### Enkoder:
- Regularnie czyść z kurzu i brudu
- Sprawdź czy obraca się swobodnie
- **NIE** smaruj

---

## 12. Dane Techniczne

### 12.1 Zasilanie
- **Napięcie**: 12V DC
- **Prąd**: max 5A
- **Pobór mocy**: max 60W

### 12.2 Warunki Pracy
- **Temperatura**: -10°C do +50°C
- **Wilgotność**: do 85% (bez kondensacji)
- **Stopień ochrony**: IP54 (obudowa)

### 12.3 Wydajność
- **Dokładność pomiaru**: ±1cm (po kalibracji)
- **Maksymalna prędkość**: 30 km/h
- **Zalecana prędkość**: 5-15 km/h

### 12.4 Łączność (v1.4.0)
- **WiFi**: 802.11 b/g/n (2.4 GHz)
- **Tryb**: Access Point
- **SSID**: Trassar
- **Hasło**: 12345678
- **IP**: 192.168.4.1
- **Zasięg**: do 30m (zalecane 10m)

---

## 13. Gwarancja

- **Okres gwarancji**: 24 miesiące od daty zakupu
- **Gwarancja obejmuje**: Wady fabryczne i materiałowe
- **Gwarancja NIE obejmuje**: Uszkodzeń mechanicznych, zalania, nieprawidłowego użytkowania

### 13.1 Serwis

W przypadku problemów skontaktuj się z:

**MT220126 Engineering Team**
- Tel: [numer telefonu]
- Email: support@mt220126.pl
- Dostępność: Pon-Pt 8:00-16:00

---

## 14. FAQ (Najczęściej Zadawane Pytania)

**P: Czy mogę używać systemu w deszczu?**
O: Obudowa ma stopień IP54, ale zalecamy unikanie pracy w intensywnym deszczu.

**P: Jak często kalibrować?**
O: Obowiązkowo przed pierwszym użyciem i co 1000 km.

**P: Czy mogę zmieniać wzorce podczas malowania?**
O: TAK! System obsługuje zmianę "w locie".

**P: Co oznacza [ODWR] na ekranie?**
O: Wzorzec P-3a/P-3b jest odwrócony (linia ciągła po prawej zamiast lewej).

**P: Maksymalna prędkość malowania?**
O: System działa do 30 km/h, ale zalecamy 5-15 km/h dla najlepszej jakości.

**P: System pokazuje "WYMAGANA KALIBRACJA"**
O: Musisz przeprowadzić kalibrację przed użyciem (Menu → Kalibracja).

**P: Jak sprawdzić wersję oprogramowania?**
O: Menu → Informacje

**P: Czy WiFi działa bez Internetu? ⭐ NOWOŚĆ**
O: TAK! System tworzy własną sieć WiFi (Access Point). Nie potrzebujesz Internetu ani routera.

**P: Czy mogę sterować zdalnie podczas jazdy? ⭐ NOWOŚĆ**
O: **NIE ZALECAMY!** Kierowca musi skupić się na prowadzeniu. Zdalne sterowanie jest dla pasażera/drugiego operatora.

**P: Ile urządzeń może się połączyć przez WiFi? ⭐ NOWOŚĆ**
O: Wiele urządzeń może **oglądać** status, ale tylko **jedno** może sterować.

**P: Co jeśli WiFi przestanie działać? ⭐ NOWOŚĆ**
O: System nadal działa normalnie! WiFi to tylko dodatek - wszystkie funkcje są dostępne przez fizyczne przyciski.

**P: Czy Start Gap działa stabilnie? ⭐ NOWOŚĆ v1.4.0**
O: TAK! W wersji 1.4.0 całkowicie naprawiliśmy funkcję Start Gap (dedykowany GPIO 25).

**P: Czy przycisk REVERSE działa? ⭐ NOWOŚĆ v1.4.0**
O: TAK! W wersji 1.4.0 REVERSE ma dedykowany GPIO 33 i działa bez problemu.

---

**Dziękujemy za wybór naszego systemu!**

**Wersja instrukcji**: 1.4.0
**Data**: 2026-01-23
**Producent**: MT220126 Engineering Team

---

## 🎖️ Historia Wersji

### v1.4.0 (2026-01-23)
- ✅ Naprawione konflikty GPIO
- ✅ Funkcja REVERSE działa stabilnie (GPIO 33)
- ✅ Start Gap działa stabilnie (GPIO 25)
- ✅ Dodano zdalne sterowanie przez WiFi
- ✅ Panel WWW 192.168.4.1

### v1.3.0
- Dodano funkcję Start Gap (Od Przerwy)

### v1.2.0
- Dodano serwer WiFi + panel WWW

### v1.1.0
- Krytyczne poprawki bezpieczeństwa

### v1.0.0
- Pierwsze wydanie systemu
