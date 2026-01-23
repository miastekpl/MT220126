# Instrukcja Obsługi
## System Sterowania Malowaniem Pasów Drogowych v1.0.0

---

## 📖 Spis Treści

1. [Wstęp](#1-wstęp)
2. [Bezpieczeństwo](#2-bezpieczeństwo)
3. [Pierwsze Uruchomienie](#3-pierwsze-uruchomienie)
4. [Kalibracja](#4-kalibracja)
5. [Obsługa Podstawowa](#5-obsługa-podstawowa)
6. [Menu Systemowe](#6-menu-systemowe)
7. [Wzorce Malowania](#7-wzorce-malowania)
8. [Pomiar Dystansu](#8-pomiar-dystansu)
9. [Rozwiązywanie Problemów](#9-rozwiązywanie-problemów)
10. [Konserwacja](#10-konserwacja)

---

## 1. Wstęp

System Sterowania Malowaniem Pasów Drogowych to profesjonalne urządzenie do automatycznego nanoszenia oznakowania drogowego. System steruje 6 pistoletami malarskimi i obsługuje 15 wzorców zgodnych z normami.

### 1.1 Komponenty

- **Wyświetlacz** - TFT 320x240 pokazujący wszystkie informacje
- **Przyciski wzorców** - 15 przycisków do wyboru wzorca (P-1a do P-7d)
- **START/PAUZA** - Jeden przycisk do uruchamiania i wstrzymywania
- **STOP** - Przycisk zatrzymania i dostępu do menu
- **REVERSE** - Przycisk odwracania wzorców P-3a/P-3b
- **Joystick** - Nawigacja w menu
- **Enkoder** - Czujnik odległości (automatyczny)

---

## 2. Bezpieczeństwo

### ⚠️ OSTRZEŻENIA

- **Przed uruchomieniem** upewnij się, że wszystkie pistolety są prawidłowo podłączone
- **Nie dotykaj** pistoletów podczas pracy systemu
- **Zachowaj ostrożność** przy obsłudze farby i rozpuszczalników
- **Noś środki ochrony** - rękawice, okulary, maskę
- **System działa** tylko przy włączonym silniku pojazdu (zabezpieczenie przed rozładowaniem)

### ✅ ZALECENIA

- Regularnie sprawdzaj poziom farby
- Przed rozpoczęciem pracy zrób test na krótkim odcinku
- Kalibruj system co 1000 km
- Czyść pistolety po każdym użyciu

---

## 3. Pierwsze Uruchomienie

### 3.1 Podłączenie Zasilania

1. Podłącz zasilanie **12V DC** do systemu
2. Sprawdź czy dioda zasilania świeci
3. Wyświetlacz powinien się włączyć i pokazać ekran powitalny

### 3.2 Sprawdzenie Systemu

Po włączeniu system automatycznie:
- Testuje wszystkie przekaźniki (sekwencja 1-6)
- Sprawdza kalibrację enkodera
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

1. Upewnij się, że wybrany jest właściwy wzorzec
2. Naciśnij **START/PAUZA**
3. Status zmieni się na **"MALOWANIE"**
4. Jedź zgodnie z planem malowania
5. System automatycznie:
   - Włącza/wyłącza pistolety wg wzorca
   - Zlicza dystans
   - Oblicza wymalowaną powierzchnię
   - Pokazuje prędkość

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

### 5.5 Odwracanie Wzorców P-3a/P-3b

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
2. Naciśnij przycisk **REVERSE**
3. Na ekranie pojawi się napis **[ODWR]**
4. Naciśnij **REVERSE** ponownie aby wrócić do standardu

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
- Wersja oprogramowania
- Platforma sprzętowa
- Status kalibracji
- Impulsy/cm enkodera

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

## 8. Pomiar Dystansu

### 8.1 Zastosowanie

- Sprawdzenie kalibracji
- Pomiar odległości między punktami
- Weryfikacja długości odcinków
- Kontrola dokładności

### 8.2 Procedura

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

## 9. Rozwiązywanie Problemów

### 9.1 System Nie Startuje

**Objawy:** Wyświetlacz nie świeci

**Rozwiązanie:**
1. Sprawdź zasilanie 12V
2. Sprawdź bezpieczniki
3. Sprawdź połączenie USB (serwis)

### 9.2 Wyświetlacz Nie Pokazuje Obrazu

**Objawy:** Zasilanie jest, ale ekran czarny

**Rozwiązanie:**
1. Sprawdź jasność ekranu
2. Wyłącz i włącz zasilanie
3. Skontaktuj się z serwisem

### 9.3 Pistolet Nie Maluje

**Objawy:** Wybrany wzorzec, ale pistolet nie działa

**Rozwiązanie:**
1. Sprawdź poziom farby
2. Sprawdź czy pistolet nie jest zatkany
3. Sprawdź połączenia elektryczne do przekaźnika
4. Użyj **Menu → Informacje** aby sprawdzić status

### 9.4 Nieprawidłowy Dystans

**Objawy:** Wskazanie dystansu nie zgadza się z rzeczywistością

**Rozwiązanie:**
1. **Przeprowadź kalibrację ponownie**
2. Sprawdź enkoder - czy jest dobrze zamocowany
3. Sprawdź koło - czy nie ślizga się

### 9.5 Wzorzec Maluje Nieprawidłowo

**Objawy:** Długości linii/przerw nie zgadzają się

**Rozwiązanie:**
1. Sprawdź kalibrację
2. Sprawdź prędkość - nie jedź zbyt szybko (max 20 km/h zalecane)
3. Sprawdź czy enkoder dobrze przylega do koła

### 9.6 System Zależł

**Objawy:** Wyświetlacz nie reaguje, przyciski nie działają

**Rozwiązanie:**
1. **Wyłącz zasilanie**
2. Odczekaj 10 sekund
3. **Włącz zasilanie** ponownie
4. Jeśli problem się powtarza - kontakt z serwisem

---

## 10. Konserwacja

### 10.1 Codziennie (Przed Pracą)

- [ ] Sprawdź poziom farby we wszystkich zbiornikach
- [ ] Sprawdź czy wszystkie pistolety są drożne
- [ ] Sprawdź połączenia elektryczne
- [ ] Sprawdź czy enkoder jest czysty i dobrze zamocowany
- [ ] Zrób test na krótkim odcinku (5m)

### 10.2 Co Tydzień

- [ ] Oczyść wyświetlacz miękką szmatką
- [ ] Sprawdź wszystkie połączenia elektryczne
- [ ] Oczyść obudowę systemu
- [ ] Sprawdź stan przycisków

### 10.3 Co Miesiąc

- [ ] Przeprowadź test wszystkich wzorców
- [ ] Sprawdź kalibrację (pomiar na 100m)
- [ ] Oczyść wszystkie pistolety
- [ ] Sprawdź stan przekaźników

### 10.4 Co 1000 km

- [ ] **OBOWIĄZKOWA rekalibracja**
- [ ] Przegląd wszystkich połączeń elektrycznych
- [ ] Test wszystkich funkcji systemu
- [ ] Wymiana zużytych części (jeśli potrzeba)

### 10.5 Czyszczenie

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

## 11. Dane Techniczne

### 11.1 Zasilanie
- **Napięcie**: 12V DC
- **Prąd**: max 5A
- **Pobór mocy**: max 60W

### 11.2 Warunki Pracy
- **Temperatura**: -10°C do +50°C
- **Wilgotność**: do 85% (bez kondensacji)
- **Stopień ochrony**: IP54 (obudowa)

### 11.3 Wydajność
- **Dokładność pomiaru**: ±1cm (po kalibracji)
- **Maksymalna prędkość**: 30 km/h
- **Zalecana prędkość**: 5-15 km/h

---

## 12. Gwarancja

- **Okres gwarancji**: 24 miesiące od daty zakupu
- **Gwarancja obejmuje**: Wady fabryczne i materiałowe
- **Gwarancja NIE obejmuje**: Uszkodzeń mechanicznych, zalania, nieprawidłowego użytkowania

### 12.1 Serwis

W przypadku problemów skontaktuj się z:

**MT220126 Engineering Team**
- Tel: [numer telefonu]
- Email: support@mt220126.pl
- Dostępność: Pon-Pt 8:00-16:00

---

## 13. FAQ (Najczęściej Zadawane Pytania)

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

---

**Dziękujemy za wybór naszego systemu!**

**Wersja instrukcji**: 1.0.0
**Data**: 2026-01-23
**Producent**: MT220126 Engineering Team
