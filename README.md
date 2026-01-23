# System Sterowania Malowaniem Pasów Drogowych v1.0.0

## 📋 Opis Projektu

Profesjonalny system sterowania malowaniem pasów drogowych wykorzystujący ESP32-S3 z obsługą 6 pistoletów malarskich sterowanych przekaźnikami. System umożliwia automatyczne malowanie zgodnie z normami drogowymi.

## 🎯 Funkcjonalności

- **13 wzorców malowania** zgodnych z normami (P-1a do P-7d)
- **Kalibracja enkodera** z precyzją do 1cm
- **Pomiar dystansu** w czasie rzeczywistym
- **Zmiana wzorców "w locie"** podczas malowania
- **Odwracanie wzorców** P-3a i P-3b (linia ciągła lewo/prawo)
- **Intuicyjny interfejs** na wyświetlaczu ILI9341
- **Licznik wymalowanych m²**
- **Wyświetlanie prędkości** maszyny

## 🔧 Komponenty

- **ESP32-S3 N16R8** - główny mikrokontroler
- **Wyświetlacz ILI9341** (320x240 TFT)
- **Enkoder KY-040** - pomiar odległości
- **Joystick 2-osiowy** z przyciskiem - nawigacja w menu
- **Przyciski fizyczne** - sterowanie wzorcami (13 szt.) + Stop + Start/Pauza
- **6 modułów przekaźników** - sterowanie pistoletami

## 📊 Wzorce Malowania

| Wzorzec | Linia (m) | Przerwa (m) | Szerokość (cm) | Opis                          |
|---------|-----------|-------------|----------------|-------------------------------|
| P-1a    | 4.0       | 8.0         | 12             | Przerywana długa              |
| P-1b    | 2.0       | 4.0         | 12             | Przerywana krótka             |
| P-1c    | 2.0       | 2.0         | 12             | Wydzielająca                  |
| P-1d    | 1.0       | 1.0         | 12             | Prowadząca wąska              |
| P-1e    | 1.0       | 1.0         | 24             | Prowadząca szeroka            |
| P-2a    | Ciągła    | -           | 12             | Ciągła wąska                  |
| P-2b    | Ciągła    | -           | 24             | Ciągła szeroka                |
| P-3a    | 4.0       | 2.0         | 12             | Przekraczalna długa *         |
| P-3b    | 1.0       | 1.0         | 12             | Przekraczalna krótka *        |
| P-4     | Ciągła    | -           | 24             | Podwójna ciągła *             |
| P-6     | 4.0       | 2.0         | 12             | Ostrzegawcza                  |
| P-7a    | 1.0       | 1.0         | 24             | Krawędziowa przeryw. szer.    |
| P-7b    | Ciągła    | -           | 24             | Krawędziowa ciągła szer.      |
| P-7c    | 1.0       | 1.0         | 12             | Krawędziowa przeryw. wąska    |
| P-7d    | Ciągła    | -           | 12             | Krawędziowa ciągła wąska      |

\* Wzorce z możliwością odwracania

## 🚀 Szybki Start

### Instalacja

1. Zainstaluj [PlatformIO](https://platformio.org/)
2. Sklonuj repozytorium
3. Otwórz projekt w PlatformIO
4. Podłącz ESP32-S3 przez USB
5. Skompiluj i wgraj: `pio run -t upload`

### Pierwsze Uruchomienie

1. **Kalibracja enkodera** (obowiązkowa przy pierwszym uruchomieniu):
   - Przytrzymaj przycisk STOP przez 1 sekundę
   - Wybierz "Kalibracja" selektorem
   - Naciśnij START
   - Przejedź dokładnie 10 metrów
   - Naciśnij START ponownie

2. **Wybór wzorca**:
   - Naciśnij przycisk odpowiadający wzorcowi (P-1a do P-7d)

3. **Rozpoczęcie malowania**:
   - Naciśnij START/PAUZA

## 📖 Dokumentacja

- [Instrukcja Obsługi](docs/INSTRUKCJA_OBSLUGI.md)
- [Dokumentacja Techniczna](docs/DOKUMENTACJA_TECHNICZNA.md)
- [Schematy Połączeń](docs/SCHEMATY.md)
- [CHANGELOG](CHANGELOG.md)
- [Przewodnik dla AI (CLAUDE.md)](CLAUDE.md)

## 🔌 Schemat Pinów

Szczegółowe schematy znajdują się w katalogu `docs/`

### Podstawowe połączenia:
- **Wyświetlacz ILI9341**: SPI (GPIO 18, 23, 5, 19, 22, 21)
- **Enkoder KY-040**: GPIO 32 (CLK), 33 (DT), 25 (SW)
- **Joystick**: GPIO 34 (X), 35 (Y), 26 (SW)
- **Przekaźniki 1-6**: GPIO 13, 12, 14, 27, 26, 25
- **Przyciski**: GPIO 15-28 (przyciski wzorców, start, stop)

## 📝 Wersja

**Aktualna wersja: 1.0.0**

Zobacz [CHANGELOG.md](CHANGELOG.md) aby poznać historię zmian.

## 👥 Autor

Zespół Inżynieryjny MT220126
- Specjalizacja: Systemy sterowania przemysłowego
- Doświadczenie: 200+ lat zbiorczego doświadczenia

## 📄 Licencja

Proprietary - Wszelkie prawa zastrzeżone

## 🆘 Wsparcie

W razie pytań lub problemów:
- Sprawdź [Instrukcję Obsługi](docs/INSTRUKCJA_OBSLUGI.md)
- Przejrzyj [FAQ](docs/FAQ.md)
- Sprawdź sekcję [Rozwiązywanie Problemów](docs/TROUBLESHOOTING.md)

---

**Uwaga**: System wymaga kalibracji przed pierwszym użyciem!
