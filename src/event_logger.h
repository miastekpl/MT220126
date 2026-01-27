/**
 * Event Logger - System logowania zdarzeń
 * Wersja: 1.6.5
 *
 * Funkcje:
 * - Ring buffer dla zdarzeń (100 najnowszych)
 * - Timestampy z millis()
 * - Wydruk przez Serial
 * - Zapis do SD przez SDCardManager (od v1.6.0)
 *
 * Copyright (c) 2026 MT220126 Engineering Team
 */

#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include <Arduino.h>
#include "config_v140_NEW.h"

/**
 * Klasa EventLogger
 *
 * Ring buffer przechowujący ostatnie EVENT_LOG_SIZE zdarzeń.
 * Gdy buffer pełny, najstarsze zdarzenia są nadpisywane.
 *
 * Użycie:
 *   EventLogger logger;
 *   logger.init();
 *   logger.log(EVENT_SYSTEM_START, 0, 0, "System uruchomiony");
 *   logger.printToSerial();
 */
class EventLogger {
private:
    LogEvent events[EVENT_LOG_SIZE];  // Ring buffer
    uint16_t writeIndex;              // Indeks zapisu (0-99)
    uint16_t eventCount;              // Ilość zdarzeń (0-100)
    bool initialized;

    /**
     * Formatowanie timestampu do postaci czytelnej
     * Konwertuje millis() na format: XXh XXm XXs
     */
    void formatTimestamp(unsigned long timestamp, char* buffer, size_t bufferSize);

    /**
     * Pobieranie nazwy typu zdarzenia (enum → string)
     */
    const char* getEventTypeName(EventType type);

public:
    /**
     * Konstruktor
     */
    EventLogger();

    /**
     * Inicjalizacja loggera
     */
    void init();

    /**
     * Logowanie zdarzenia
     *
     * @param type Typ zdarzenia (enum EventType)
     * @param data1 Opcjonalne dane 1 (np. PatternType, StateType)
     * @param data2 Opcjonalne dane 2
     * @param message Krótki komunikat (max 50 znaków)
     */
    void log(EventType type, uint16_t data1, uint16_t data2, const char* message);

    /**
     * Pobranie ostatnich N zdarzeń
     *
     * @param buffer Bufor na zdarzenia
     * @param count Ilość zdarzeń do pobrania
     * @return Faktyczna ilość zwróconych zdarzeń
     */
    uint16_t getRecent(LogEvent* buffer, uint8_t count);

    /**
     * Wydruk wszystkich zdarzeń przez Serial
     * Format: [timestamp] EVENT_TYPE: message (data1, data2)
     */
    void printToSerial();

    /**
     * Wydruk ostatnich N zdarzeń
     */
    void printLastN(uint8_t count);

    /**
     * Wyczyszczenie wszystkich zdarzeń
     */
    void clear();

    /**
     * Pobranie ilości zarejestrowanych zdarzeń
     */
    uint16_t getEventCount() { return eventCount; }

    /**
     * Sprawdzenie czy logger zainicjalizowany
     */
    bool isInitialized() { return initialized; }

    // UWAGA: Zapis do SD jest realizowany przez SDCardManager (v1.6.0+)
    // SDCardManager automatycznie pobiera zdarzenia z EventLogger i zapisuje na kartę SD
};

#endif // EVENT_LOGGER_H
