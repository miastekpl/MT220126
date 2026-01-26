/**
 * Implementacja Event Loggera
 * v1.5.0
 */

#include "event_logger.h"

EventLogger::EventLogger() {
    writeIndex = 0;
    eventCount = 0;
    initialized = false;
}

void EventLogger::init() {
    // Wyczyść wszystkie zdarzenia
    for (uint16_t i = 0; i < EVENT_LOG_SIZE; i++) {
        events[i].timestamp = 0;
        events[i].type = EVENT_SYSTEM_START;
        events[i].data1 = 0;
        events[i].data2 = 0;
        events[i].message = "";
    }

    writeIndex = 0;
    eventCount = 0;
    initialized = true;

    DEBUG_PRINTLN("EventLogger: Zainicjalizowany");
}

void EventLogger::log(EventType type, uint16_t data1, uint16_t data2, const char* message) {
    if (!initialized) {
        return;
    }

    // Zapisz zdarzenie w ring buffer
    events[writeIndex].timestamp = millis();
    events[writeIndex].type = type;
    events[writeIndex].data1 = data1;
    events[writeIndex].data2 = data2;
    events[writeIndex].message = message;

    // Aktualizuj indeks (ring buffer)
    writeIndex = (writeIndex + 1) % EVENT_LOG_SIZE;

    // Aktualizuj licznik (max EVENT_LOG_SIZE)
    if (eventCount < EVENT_LOG_SIZE) {
        eventCount++;
    }

    // Debug log
    #if DEBUG_ENABLED
    char timestampStr[32];
    formatTimestamp(millis(), timestampStr, sizeof(timestampStr));
    DEBUG_PRINTF("[%s] EVENT: %s - %s (data: %d, %d)\n",
                timestampStr,
                getEventTypeName(type),
                message,
                data1,
                data2);
    #endif
}

uint16_t EventLogger::getRecent(LogEvent* buffer, uint8_t count) {
    if (!initialized || count == 0 || eventCount == 0) {
        return 0;
    }

    // Ogranicz do dostępnej ilości
    uint8_t actualCount = (count > eventCount) ? eventCount : count;

    // Kopiuj ostatnie N zdarzeń
    for (uint8_t i = 0; i < actualCount; i++) {
        // Oblicz indeks (od najnowszego do najstarszego)
        uint16_t idx = (writeIndex + EVENT_LOG_SIZE - actualCount + i) % EVENT_LOG_SIZE;
        buffer[i] = events[idx];
    }

    return actualCount;
}

void EventLogger::printToSerial() {
    if (!initialized || eventCount == 0) {
        DEBUG_PRINTLN("EventLogger: Brak zdarzen");
        return;
    }

    DEBUG_PRINTLN("\n========== EVENT LOG ==========");
    DEBUG_PRINTF("Zarejestrowanych zdarzen: %d\n\n", eventCount);

    // Wydrukuj od najstarszego do najnowszego
    for (uint16_t i = 0; i < eventCount; i++) {
        uint16_t idx = (writeIndex + EVENT_LOG_SIZE - eventCount + i) % EVENT_LOG_SIZE;

        char timestampStr[32];
        formatTimestamp(events[idx].timestamp, timestampStr, sizeof(timestampStr));

        DEBUG_PRINTF("[%s] %s: %s",
                    timestampStr,
                    getEventTypeName(events[idx].type),
                    events[idx].message);

        // Wydrukuj dane jeśli niezerowe
        if (events[idx].data1 != 0 || events[idx].data2 != 0) {
            DEBUG_PRINTF(" (data: %d, %d)", events[idx].data1, events[idx].data2);
        }

        DEBUG_PRINTLN("");
    }

    DEBUG_PRINTLN("===============================\n");
}

void EventLogger::printLastN(uint8_t count) {
    if (!initialized || eventCount == 0) {
        DEBUG_PRINTLN("EventLogger: Brak zdarzen");
        return;
    }

    uint8_t actualCount = (count > eventCount) ? eventCount : count;

    DEBUG_PRINTF("\n===== OSTATNIE %d ZDARZEN =====\n", actualCount);

    for (uint8_t i = 0; i < actualCount; i++) {
        uint16_t idx = (writeIndex + EVENT_LOG_SIZE - actualCount + i) % EVENT_LOG_SIZE;

        char timestampStr[32];
        formatTimestamp(events[idx].timestamp, timestampStr, sizeof(timestampStr));

        DEBUG_PRINTF("[%s] %s: %s",
                    timestampStr,
                    getEventTypeName(events[idx].type),
                    events[idx].message);

        if (events[idx].data1 != 0 || events[idx].data2 != 0) {
            DEBUG_PRINTF(" (data: %d, %d)", events[idx].data1, events[idx].data2);
        }

        DEBUG_PRINTLN("");
    }

    DEBUG_PRINTLN("===============================\n");
}

void EventLogger::clear() {
    writeIndex = 0;
    eventCount = 0;

    DEBUG_PRINTLN("EventLogger: Wyczyszczono wszystkie zdarzenia");
}

void EventLogger::formatTimestamp(unsigned long timestamp, char* buffer, size_t bufferSize) {
    unsigned long seconds = timestamp / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;

    seconds %= 60;
    minutes %= 60;

    if (hours > 0) {
        snprintf(buffer, bufferSize, "%luh %lum %lus", hours, minutes, seconds);
    } else if (minutes > 0) {
        snprintf(buffer, bufferSize, "%lum %lus", minutes, seconds);
    } else {
        snprintf(buffer, bufferSize, "%lus", seconds);
    }
}

const char* EventLogger::getEventTypeName(EventType type) {
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
