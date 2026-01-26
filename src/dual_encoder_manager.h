/**
 * Dual Encoder Manager - System redundancji enkoderów
 * Wersja: 1.6.0
 *
 * Funkcje:
 * - Obsługa dwóch niezależnych enkoderów (PRIMARY + BACKUP)
 * - Ciągłe porównywanie odczytów
 * - Automatyczna detekcja awarii
 * - Przełączanie na backup przy błędzie
 * - Logowanie rozbieżności
 *
 * Hardware:
 * - Enkoder PRIMARY: GPIO 32 (CLK), 33 (DT), 20 (SW)
 * - Enkoder BACKUP:  GPIO 6 (CLK), 7 (DT), 19 (SW)
 *
 * Copyright (c) 2026 MT220126 Engineering Team
 */

#ifndef DUAL_ENCODER_MANAGER_H
#define DUAL_ENCODER_MANAGER_H

#include <Arduino.h>
#include "encoder_handler.h"
#include "event_logger.h"
#include "config_v140_NEW.h"

// Tolerancja rozbieżności między enkoderami
#define ENCODER_TOLERANCE_CM 5      // Różnica max 5 cm (na 100m = 0.5% błędu)
#define ENCODER_CHECK_INTERVAL 1000 // Sprawdzanie co 1 sekundę

/**
 * Stan enkodera
 */
enum EncoderStatus {
    ENCODER_OK = 0,           // Działa poprawnie
    ENCODER_DIVERGENCE,       // Rozbieżność z drugim enkoderem
    ENCODER_STUCK,            // Brak zmian pozycji (zawieszony)
    ENCODER_FAILED            // Całkowita awaria
};

/**
 * Aktywny enkoder
 */
enum ActiveEncoder {
    ENCODER_PRIMARY = 0,      // Enkoder główny (GPIO 32/33)
    ENCODER_BACKUP = 1        // Enkoder zapasowy (GPIO 6/7)
};

/**
 * Klasa DualEncoderManager
 *
 * Zarządza dwoma enkoderami zapewniając:
 * - Redundancję (backup przy awarii primary)
 * - Ciągłą weryfikację spójności
 * - Automatyczne wykrywanie awarii
 * - Logowanie wszystkich anomalii
 *
 * Użycie:
 *   DualEncoderManager dualEncoder(&eventLogger);
 *   dualEncoder.init();
 *   dualEncoder.update();  // W loop()
 *   long distance = dualEncoder.getDistance();
 */
class DualEncoderManager {
private:
    EncoderHandler* primaryEncoder;   // Enkoder główny
    EncoderHandler* backupEncoder;    // Enkoder zapasowy
    EventLogger* logger;              // Logger zdarzeń

    ActiveEncoder activeEncoder;      // Który enkoder jest aktywny
    EncoderStatus primaryStatus;      // Status enkodera głównego
    EncoderStatus backupStatus;       // Status enkodera zapasowego

    unsigned long lastCheckTime;      // Ostatnie sprawdzenie
    long lastPrimaryDistance;         // Ostatnia pozycja primary
    long lastBackupDistance;          // Ostatnia pozycja backup

    uint16_t divergenceCount;         // Licznik rozbieżności
    uint16_t primaryStuckCount;       // Licznik zawieszeń primary
    uint16_t backupStuckCount;        // Licznik zawieszeń backup

    bool initialized;
    bool warningShown;                // Czy pokazano ostrzeżenie

    /**
     * Sprawdzenie spójności odczytów z dwóch enkoderów
     * @return true jeśli enkodery zgadzają się w tolerancji
     */
    bool checkConsistency();

    /**
     * Sprawdzenie czy enkoder się porusza (nie jest stuck)
     * @param encoder Wskaźnik na enkoder do sprawdzenia
     * @param lastDistance Ostatnia zapisana pozycja
     * @return true jeśli enkoder się porusza
     */
    bool checkMovement(EncoderHandler* encoder, long lastDistance);

    /**
     * Przełączenie na enkoder zapasowy
     */
    void switchToBackup();

    /**
     * Próba powrotu do enkodera głównego
     * (gdy backup aktywny, ale primary znów działa)
     */
    void tryRestorePrimary();

    /**
     * Aktualizacja statusów enkoderów
     */
    void updateStatus();

public:
    /**
     * Konstruktor
     * @param eventLogger Wskaźnik na Event Logger
     */
    DualEncoderManager(EventLogger* eventLogger);

    /**
     * Destruktor
     */
    ~DualEncoderManager();

    /**
     * Inicjalizacja dual encoder system
     */
    void init();

    /**
     * Aktualizacja - wywoływać w loop()
     */
    void update();

    /**
     * Reset pozycji obu enkoderów
     */
    void reset();

    /**
     * Reset dystansu obu enkoderów
     */
    void resetDistance();

    /**
     * Pobranie dystansu z aktywnego enkodera
     * @return Dystans w cm
     */
    long getDistance();

    /**
     * Pobranie pozycji z aktywnego enkodera
     * @return Pozycja (impulsy)
     */
    long getPosition();

    /**
     * Pobranie prędkości z aktywnego enkodera
     * @return Prędkość w m/s
     */
    float getSpeed();

    /**
     * Ustawienie kalibracji dla obu enkoderów
     * @param pulses Impulsy na centymetr
     */
    void setPulsesPerCm(float pulses);

    /**
     * Pobranie kalibracji
     */
    float getPulsesPerCm();

    /**
     * Sprawdzenie czy system skalibrowany
     */
    bool isCalibrated();

    /**
     * Pobranie statusu enkodera głównego
     */
    EncoderStatus getPrimaryStatus() { return primaryStatus; }

    /**
     * Pobranie statusu enkodera zapasowego
     */
    EncoderStatus getBackupStatus() { return backupStatus; }

    /**
     * Pobranie aktywnego enkodera
     */
    ActiveEncoder getActiveEncoder() { return activeEncoder; }

    /**
     * Pobranie nazwy aktywnego enkodera (do wyświetlania)
     */
    const char* getActiveEncoderName();

    /**
     * Pobranie nazwy statusu enkodera
     */
    const char* getStatusName(EncoderStatus status);

    /**
     * Sprawdzenie czy system jest zdrowy (co najmniej 1 enkoder OK)
     */
    bool isHealthy();

    /**
     * Wymuszenie użycia konkretnego enkodera (do testowania)
     */
    void forceEncoder(ActiveEncoder encoder);

    /**
     * Pobranie wskaźnika na enkoder główny (do kalibracji)
     */
    EncoderHandler* getPrimaryEncoder() { return primaryEncoder; }

    /**
     * Pobranie wskaźnika na enkoder zapasowy
     */
    EncoderHandler* getBackupEncoder() { return backupEncoder; }
};

#endif // DUAL_ENCODER_MANAGER_H
