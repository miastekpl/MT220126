/**
 * Implementacja SD Card Manager
 * v1.6.0
 */

#include "sd_card_manager.h"

SDCardManager::SDCardManager(EventLogger* eventLogger) {
    logger = eventLogger;
    status = SD_NOT_INITIALIZED;

    lastSaveTime = 0;
    lastSavedEventCount = 0;
    currentFileIndex = 1;

    initialized = false;
    autoSaveEnabled = true;

    memset(currentFilename, 0, sizeof(currentFilename));
}

bool SDCardManager::init() {
    DEBUG_PRINTLN("SDCard: Inicjalizacja...");

    // Inicjalizacja SPI dla SD Card
    SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);

    // Próba montowania karty SD
    if (!SD.begin(SD_CS_PIN)) {
        DEBUG_PRINTLN("SDCard: BŁĄD - Brak karty lub błąd montowania!");
        status = SD_NOT_PRESENT;
        return false;
    }

    // Sprawdzenie typu karty
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        DEBUG_PRINTLN("SDCard: BŁĄD - Brak karty!");
        status = SD_NOT_PRESENT;
        return false;
    }

    // Informacje o karcie
    DEBUG_PRINT("SDCard: Typ karty: ");
    if (cardType == CARD_MMC) {
        DEBUG_PRINTLN("MMC");
    } else if (cardType == CARD_SD) {
        DEBUG_PRINTLN("SDSC");
    } else if (cardType == CARD_SDHC) {
        DEBUG_PRINTLN("SDHC");
    } else {
        DEBUG_PRINTLN("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    DEBUG_PRINTF("SDCard: Rozmiar: %llu MB\n", cardSize);

    uint64_t usedSize = SD.usedBytes() / (1024 * 1024);
    uint64_t freeSize = (SD.totalBytes() - SD.usedBytes()) / (1024 * 1024);
    DEBUG_PRINTF("SDCard: Użyte: %llu MB, Wolne: %llu MB\n", usedSize, freeSize);

    // Znajdź ostatni plik logów
    currentFileIndex = 1;
    while (currentFileIndex <= SD_MAX_FILES) {
        generateFilename();
        if (!SD.exists(currentFilename)) {
            break;
        }
        currentFileIndex++;
    }

    // Jeśli wszystkie pliki istnieją, użyj ostatniego
    if (currentFileIndex > SD_MAX_FILES) {
        currentFileIndex = SD_MAX_FILES;
        generateFilename();
    }

    DEBUG_PRINTF("SDCard: Aktywny plik: %s\n", currentFilename);

    // Jeśli plik nie istnieje, utwórz z nagłówkiem CSV
    if (!SD.exists(currentFilename)) {
        if (!writeCSVHeader()) {
            DEBUG_PRINTLN("SDCard: BŁĄD - Nie można zapisać nagłówka!");
            status = SD_WRITE_ERROR;
            return false;
        }
    }

    status = SD_OK;
    initialized = true;
    lastSaveTime = millis();

    DEBUG_PRINTLN("SDCard: Zainicjalizowana pomyślnie!");

    return true;
}

void SDCardManager::generateFilename() {
    snprintf(currentFilename, sizeof(currentFilename), "/logs_%03d.csv", currentFileIndex);
}

bool SDCardManager::writeCSVHeader() {
    File file = SD.open(currentFilename, FILE_WRITE);
    if (!file) {
        return false;
    }

    // Nagłówek CSV
    file.println("Timestamp_ms,Time_formatted,Event_Type,Event_Name,Data1,Data2,Message");
    file.close();

    DEBUG_PRINTF("SDCard: Utworzono plik z nagłówkiem: %s\n", currentFilename);
    return true;
}

void SDCardManager::update() {
    if (!initialized || !autoSaveEnabled || status != SD_OK) {
        return;
    }

    if (!logger) {
        return;
    }

    unsigned long currentTime = millis();
    uint16_t currentEventCount = logger->getEventCount();

    // Sprawdź czy czas na automatyczny zapis
    bool timeToSave = (currentTime - lastSaveTime >= SD_AUTO_SAVE_INTERVAL);
    bool eventCountReached = (currentEventCount - lastSavedEventCount >= SD_AUTO_SAVE_EVENT_COUNT);

    if (timeToSave || eventCountReached) {
        DEBUG_PRINTF("SDCard: Auto-zapis (czas=%d, zdarzenia=%d)\n", timeToSave, eventCountReached);
        saveLogsNow();
    }

    // Sprawdź rotację pliku
    if (isFileTooLarge()) {
        rotateFile();
    }
}

bool SDCardManager::saveLogsNow() {
    if (!initialized || status != SD_OK || !logger) {
        DEBUG_PRINTLN("SDCard: Nie można zapisać - SD niedostępna");
        return false;
    }

    DEBUG_PRINTF("SDCard: Zapisywanie logów do %s...\n", currentFilename);

    // Pobierz najnowsze zdarzenia od ostatniego zapisu
    uint16_t currentEventCount = logger->getEventCount();
    uint16_t newEvents = currentEventCount - lastSavedEventCount;

    if (newEvents == 0) {
        DEBUG_PRINTLN("SDCard: Brak nowych zdarzeń do zapisu");
        return true;
    }

    // Pobierz zdarzenia
    LogEvent events[100];  // Max 100 zdarzeń
    uint16_t eventsToSave = (newEvents > 100) ? 100 : newEvents;
    uint16_t fetched = logger->getRecent(events, eventsToSave);

    // Otwórz plik w trybie append
    File file = SD.open(currentFilename, FILE_APPEND);
    if (!file) {
        DEBUG_PRINTLN("SDCard: BŁĄD - Nie można otworzyć pliku!");
        status = SD_WRITE_ERROR;
        return false;
    }

    // Zapisz każde zdarzenie jako linia CSV
    uint16_t savedCount = 0;
    for (uint16_t i = 0; i < fetched; i++) {
        if (writeEventToCSV(events[i])) {
            savedCount++;
        }
    }

    file.close();

    lastSaveTime = millis();
    lastSavedEventCount = currentEventCount;

    DEBUG_PRINTF("SDCard: Zapisano %d zdarzeń\n", savedCount);

    return (savedCount == fetched);
}

bool SDCardManager::writeEventToCSV(const LogEvent& event) {
    File file = SD.open(currentFilename, FILE_APPEND);
    if (!file) {
        return false;
    }

    // Format timestampa (XXh XXm XXs)
    unsigned long seconds = event.timestamp / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    seconds %= 60;
    minutes %= 60;

    char timeStr[32];
    if (hours > 0) {
        snprintf(timeStr, sizeof(timeStr), "%luh %lum %lus", hours, minutes, seconds);
    } else if (minutes > 0) {
        snprintf(timeStr, sizeof(timeStr), "%lum %lus", minutes, seconds);
    } else {
        snprintf(timeStr, sizeof(timeStr), "%lus", seconds);
    }

    // Nazwa typu zdarzenia
    const char* eventTypeName = getEventTypeName(event.type);

    // Zapis linii CSV:
    // timestamp_ms, time_formatted, event_type_num, event_type_name, data1, data2, message
    file.printf("%lu,%s,%d,%s,%u,%u,\"%s\"\n",
               event.timestamp,
               timeStr,
               event.type,
               eventTypeName,
               event.data1,
               event.data2,
               event.message);

    file.close();
    return true;
}

const char* SDCardManager::getEventTypeName(EventType type) {
    switch (type) {
        case EVENT_SYSTEM_START:        return "SYSTEM_START";
        case EVENT_PATTERN_CHANGED:     return "PATTERN_CHANGED";
        case EVENT_STATE_CHANGED:       return "STATE_CHANGED";
        case EVENT_SAFETY_TRIGGERED:    return "SAFETY_TRIGGERED";
        case EVENT_CALIBRATION_START:   return "CALIBRATION_START";
        case EVENT_CALIBRATION_COMPLETE:return "CALIBRATION_COMPLETE";
        case EVENT_ERROR_OCCURRED:      return "ERROR_OCCURRED";
        case EVENT_BUTTON_PRESSED:      return "BUTTON_PRESSED";
        case EVENT_WIFI_CONNECTED:      return "WIFI_CONNECTED";
        case EVENT_WIFI_DISCONNECTED:   return "WIFI_DISCONNECTED";
        default:                        return "UNKNOWN";
    }
}

bool SDCardManager::isFileTooLarge() {
    File file = SD.open(currentFilename, FILE_READ);
    if (!file) {
        return false;
    }

    size_t fileSize = file.size();
    file.close();

    return (fileSize >= SD_MAX_FILE_SIZE);
}

void SDCardManager::rotateFile() {
    DEBUG_PRINTF("SDCard: Rotacja pliku (plik %s za duży)\n", currentFilename);

    currentFileIndex++;

    // Jeśli przekroczono limit plików, wyczyść stare
    if (currentFileIndex > SD_MAX_FILES) {
        cleanupOldFiles();
        currentFileIndex = 1;
    }

    generateFilename();

    // Utwórz nowy plik z nagłówkiem
    writeCSVHeader();

    DEBUG_PRINTF("SDCard: Nowy plik: %s\n", currentFilename);
}

void SDCardManager::cleanupOldFiles() {
    DEBUG_PRINTLN("SDCard: Czyszczenie starych plików...");

    // Usuń pierwszy plik (najstarszy)
    char filename[32];
    snprintf(filename, sizeof(filename), "/logs_001.csv");

    if (SD.exists(filename)) {
        SD.remove(filename);
        DEBUG_PRINTF("SDCard: Usunięto %s\n", filename);
    }

    // Przesuń wszystkie pliki o 1 w dół
    for (int i = 2; i <= SD_MAX_FILES; i++) {
        char oldName[32], newName[32];
        snprintf(oldName, sizeof(oldName), "/logs_%03d.csv", i);
        snprintf(newName, sizeof(newName), "/logs_%03d.csv", i - 1);

        if (SD.exists(oldName)) {
            SD.rename(oldName, newName);
        }
    }
}

const char* SDCardManager::getStatusName() {
    switch (status) {
        case SD_NOT_INITIALIZED: return "Nie zainicjalizowana";
        case SD_OK:              return "OK";
        case SD_NOT_PRESENT:     return "Brak karty";
        case SD_WRITE_ERROR:     return "Błąd zapisu";
        case SD_FULL:            return "Pełna";
        default:                 return "Nieznany";
    }
}

uint32_t SDCardManager::getFreeSpaceMB() {
    if (status != SD_OK) return 0;
    return (SD.totalBytes() - SD.usedBytes()) / (1024 * 1024);
}

uint32_t SDCardManager::getTotalSpaceMB() {
    if (status != SD_OK) return 0;
    return SD.totalBytes() / (1024 * 1024);
}

bool SDCardManager::exportAllLogs(const char* outputFilename) {
    if (status != SD_OK) return false;

    DEBUG_PRINTF("SDCard: Export wszystkich logów do %s...\n", outputFilename);

    File output = SD.open(outputFilename, FILE_WRITE);
    if (!output) {
        DEBUG_PRINTLN("SDCard: BŁĄD - Nie można utworzyć pliku wyjściowego!");
        return false;
    }

    // Nagłówek
    output.println("Timestamp_ms,Time_formatted,Event_Type,Event_Name,Data1,Data2,Message");

    // Kopiuj dane ze wszystkich plików
    for (int i = 1; i <= SD_MAX_FILES; i++) {
        char filename[32];
        snprintf(filename, sizeof(filename), "/logs_%03d.csv", i);

        if (!SD.exists(filename)) continue;

        File input = SD.open(filename, FILE_READ);
        if (!input) continue;

        // Pomiń nagłówek pierwszej linii
        input.readStringUntil('\n');

        // Kopiuj resztę
        while (input.available()) {
            String line = input.readStringUntil('\n');
            output.println(line);
        }

        input.close();
    }

    output.close();

    DEBUG_PRINTLN("SDCard: Export zakończony!");
    return true;
}

bool SDCardManager::deleteAllLogs() {
    if (status != SD_OK) return false;

    DEBUG_PRINTLN("SDCard: Usuwanie wszystkich logów...");

    for (int i = 1; i <= SD_MAX_FILES; i++) {
        char filename[32];
        snprintf(filename, sizeof(filename), "/logs_%03d.csv", i);

        if (SD.exists(filename)) {
            SD.remove(filename);
            DEBUG_PRINTF("SDCard: Usunięto %s\n", filename);
        }
    }

    // Reset do pierwszego pliku
    currentFileIndex = 1;
    generateFilename();
    writeCSVHeader();

    DEBUG_PRINTLN("SDCard: Wszystkie logi usunięte!");
    return true;
}

void SDCardManager::listLogFiles() {
    if (status != SD_OK) {
        DEBUG_PRINTLN("SDCard: Brak dostępu do karty");
        return;
    }

    DEBUG_PRINTLN("SDCard: Lista plików logów:");

    for (int i = 1; i <= SD_MAX_FILES; i++) {
        char filename[32];
        snprintf(filename, sizeof(filename), "/logs_%03d.csv", i);

        if (SD.exists(filename)) {
            File file = SD.open(filename, FILE_READ);
            if (file) {
                size_t size = file.size();
                file.close();
                DEBUG_PRINTF("  %s - %u bytes\n", filename, size);
            }
        }
    }
}
