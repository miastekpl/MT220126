/**
 * SD Card Manager - Zarządzanie kartą SD
 * Wersja: 1.6.0
 *
 * Funkcje:
 * - Zapisywanie logów Event Loggera na kartę SD
 * - Format CSV dla łatwej analizy
 * - Automatyczny zapis co N zdarzeń lub co X minut
 * - Rotacja plików (max rozmiar, max ilość)
 * - Export danych przez WiFi
 *
 * Hardware:
 * - SD Card Module (SPI):
 *   - CS:   GPIO 5 (współdzielony z TFT - multiplexing)
 *   - MOSI: GPIO 23 (SPI MOSI)
 *   - MISO: GPIO 19 (SPI MISO)
 *   - SCK:  GPIO 18 (SPI SCK)
 *
 * UWAGA: CS pin jest współdzielony z TFT!
 * Używamy CS=5 dla TFT, CS nowy (np. GPIO 4) dla SD
 *
 * Copyright (c) 2026 MT220126 Engineering Team
 */

#ifndef SD_CARD_MANAGER_H
#define SD_CARD_MANAGER_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "event_logger.h"
#include "config_v140_NEW.h"

// Piny SD Card (SPI)
#define SD_CS_PIN 4      // Chip Select dla SD (NOWY, osobny od TFT)
#define SD_MOSI_PIN 23   // SPI MOSI (współdzielony)
#define SD_MISO_PIN 19   // SPI MISO (współdzielony)
#define SD_SCK_PIN 18    // SPI SCK (współdzielony)

// Konfiguracja zapisu
#define SD_AUTO_SAVE_INTERVAL 600000    // Zapis co 10 minut (600s)
#define SD_AUTO_SAVE_EVENT_COUNT 50     // Zapis co 50 zdarzeń
#define SD_MAX_FILE_SIZE 1048576        // Max 1MB na plik (1024*1024)
#define SD_MAX_FILES 10                 // Max 10 plików logów

/**
 * Status karty SD
 */
enum SDCardStatus {
    SD_NOT_INITIALIZED = 0,
    SD_OK,
    SD_NOT_PRESENT,
    SD_WRITE_ERROR,
    SD_FULL
};

/**
 * Klasa SDCardManager
 *
 * Zarządza zapisem logów na kartę SD:
 * - Automatyczny zapis co X czasu lub Y zdarzeń
 * - Format CSV dla łatwej analizy w Excel/Python
 * - Rotacja plików (gdy plik >1MB, nowy plik)
 * - Bezpieczne zarządzanie CS pin (multiplexing z TFT)
 *
 * Użycie:
 *   SDCardManager sdCard(&eventLogger);
 *   sdCard.init();
 *   sdCard.update();  // W loop() - automatyczny zapis
 *
 *   // Ręczny zapis:
 *   sdCard.saveLogsNow();
 */
class SDCardManager {
private:
    EventLogger* logger;
    SDCardStatus status;

    unsigned long lastSaveTime;
    uint16_t lastSavedEventCount;
    uint16_t currentFileIndex;

    bool initialized;
    bool autoSaveEnabled;

    char currentFilename[32];  // Nazwa aktualnego pliku (logs_001.csv)

    /**
     * Generowanie nazwy pliku z indeksem
     * Format: logs_001.csv, logs_002.csv, ...
     */
    void generateFilename();

    /**
     * Sprawdzenie czy plik przekroczył max rozmiar
     * @return true jeśli plik za duży
     */
    bool isFileTooLarge();

    /**
     * Przejście do następnego pliku (rotacja)
     */
    void rotateFile();

    /**
     * Usunięcie najstarszych plików jeśli limit przekroczony
     */
    void cleanupOldFiles();

    /**
     * Zapis nagłówka CSV do nowego pliku
     */
    bool writeCSVHeader();

    /**
     * Zapis pojedynczego zdarzenia do CSV
     */
    bool writeEventToCSV(const LogEvent& event);

public:
    /**
     * Konstruktor
     * @param eventLogger Wskaźnik na Event Logger
     */
    SDCardManager(EventLogger* eventLogger);

    /**
     * Inicjalizacja karty SD
     * @return true jeśli inicjalizacja OK
     */
    bool init();

    /**
     * Aktualizacja - wywoływać w loop()
     * Automatycznie zapisuje logi zgodnie z konfiguracją
     */
    void update();

    /**
     * Natychmiastowy zapis wszystkich logów
     * @return true jeśli zapis udany
     */
    bool saveLogsNow();

    /**
     * Włączenie/wyłączenie automatycznego zapisu
     */
    void setAutoSave(bool enabled) { autoSaveEnabled = enabled; }

    /**
     * Pobranie statusu karty SD
     */
    SDCardStatus getStatus() { return status; }

    /**
     * Pobranie nazwy statusu (do wyświetlania)
     */
    const char* getStatusName();

    /**
     * Sprawdzenie czy karta SD dostępna
     */
    bool isAvailable() { return (status == SD_OK); }

    /**
     * Pobranie nazwy aktualnego pliku
     */
    const char* getCurrentFilename() { return currentFilename; }

    /**
     * Pobranie wolnego miejsca na karcie (MB)
     */
    uint32_t getFreeSpaceMB();

    /**
     * Pobranie całkowitej pojemności karty (MB)
     */
    uint32_t getTotalSpaceMB();

    /**
     * Export wszystkich plików logów do jednego pliku
     * @param outputFilename Nazwa pliku wyjściowego
     * @return true jeśli export udany
     */
    bool exportAllLogs(const char* outputFilename);

    /**
     * Usunięcie wszystkich plików logów
     * @return true jeśli usunięcie udane
     */
    bool deleteAllLogs();

    /**
     * Lista plików logów na karcie
     */
    void listLogFiles();
};

#endif // SD_CARD_MANAGER_H
