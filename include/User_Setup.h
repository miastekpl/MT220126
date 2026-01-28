/**
 * User_Setup.h - Konfiguracja TFT_eSPI dla ESP32-S3
 *
 * Ten plik NADPISUJE domyślny User_Setup.h z biblioteki TFT_eSPI!
 * v1.6.8 - Rozwiązanie problemu GPIO 227 (ESP8266 piny zamiast ESP32-S3)
 *
 * UWAGA: Plik musi być w katalogu include/ i platformio.ini musi mieć
 * "-I include" PRZED innymi flagami!
 */

#ifndef USER_SETUP_H
#define USER_SETUP_H

// =====================================================
// SEKCJA 1: DRIVER - ILI9341
// =====================================================
#define ILI9341_DRIVER

// =====================================================
// SEKCJA 2: ROZMIAR EKRANU
// =====================================================
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// =====================================================
// SEKCJA 3: PINY GPIO DLA ESP32-S3
// KRYTYCZNE: Te definicje MUSZĄ być liczbami, NIE makrami ESP8266!
// =====================================================

// SPI piny - ESP32-S3 hardware SPI (HSPI)
#define TFT_MISO 19   // Master In Slave Out
#define TFT_MOSI 23   // Master Out Slave In
#define TFT_SCLK 18   // SPI Clock
#define TFT_CS    5   // Chip Select
#define TFT_DC   22   // Data/Command
#define TFT_RST  21   // Reset (lub -1 jeśli połączony z EN)

// Podświetlenie (opcjonalne)
// #define TFT_BL   15   // Backlight control
// #define TFT_BACKLIGHT_ON HIGH

// =====================================================
// SEKCJA 4: CZCIONKI
// =====================================================
#define LOAD_GLCD    // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2   // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4   // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6   // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7   // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8   // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF   // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

// =====================================================
// SEKCJA 5: CZĘSTOTLIWOŚCI SPI
// =====================================================
#define SPI_FREQUENCY       40000000   // 40 MHz - stabilne dla ILI9341
#define SPI_READ_FREQUENCY  20000000   // 20 MHz dla odczytu
#define SPI_TOUCH_FREQUENCY  2500000   // Touch controller (jeśli używany)

// =====================================================
// SEKCJA 6: OPCJE OPTYMALIZACJI ESP32-S3
// =====================================================

// Użyj transakcji SPI (bezpieczne dla wielu urządzeń na tej samej magistrali)
#define SUPPORT_TRANSACTIONS

// DMA dla ESP32 (szybsze transfery)
// UWAGA: Może powodować problemy z niektórymi wersjami - wyłączone dla bezpieczeństwa
// #define USE_DMA_FOR_TRANSFER

// =====================================================
// SEKCJA 7: PSRAM (TFT Sprites)
// =====================================================
// PSRAM jest wykrywane automatycznie przez ESP32-S3
// Sprites będą używać PSRAM jeśli dostępne

// =====================================================
// KONIEC KONFIGURACJI
// =====================================================

// Informacja debug przy kompilacji
#pragma message ">>> User_Setup.h: Zaladowano konfiguracje TFT dla ESP32-S3 <<<"
#pragma message ">>> TFT_MISO=19, TFT_MOSI=23, TFT_SCLK=18, TFT_CS=5, TFT_DC=22, TFT_RST=21 <<<"

#endif // USER_SETUP_H
