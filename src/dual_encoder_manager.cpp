/**
 * Implementacja Dual Encoder Manager
 * v1.6.0
 */

#include "dual_encoder_manager.h"

DualEncoderManager::DualEncoderManager(EventLogger* eventLogger) {
    logger = eventLogger;

    // Tworzenie instancji enkoderów
    // PRIMARY: GPIO 32 (CLK), 33 (DT), 20 (SW)
    primaryEncoder = new EncoderHandler(32, 33, 20);

    // BACKUP: GPIO 6 (CLK), 7 (DT), 19 (SW)
    backupEncoder = new EncoderHandler(6, 7, 19);

    // Stan początkowy
    activeEncoder = ENCODER_PRIMARY;
    primaryStatus = ENCODER_OK;
    backupStatus = ENCODER_OK;

    lastCheckTime = 0;
    lastPrimaryDistance = 0;
    lastBackupDistance = 0;

    divergenceCount = 0;
    primaryStuckCount = 0;
    backupStuckCount = 0;

    initialized = false;
    warningShown = false;
}

DualEncoderManager::~DualEncoderManager() {
    delete primaryEncoder;
    delete backupEncoder;
}

void DualEncoderManager::init() {
    DEBUG_PRINTLN("DualEncoderManager: Inicjalizacja...");

    // Inicjalizacja obu enkoderów
    primaryEncoder->init();
    backupEncoder->init();

    // Reset pozycji
    primaryEncoder->reset();
    backupEncoder->reset();

    lastCheckTime = millis();
    initialized = true;

    DEBUG_PRINTLN("DualEncoderManager: Zainicjalizowany");
    DEBUG_PRINTLN("  PRIMARY: GPIO 32 (CLK), 33 (DT), 20 (SW)");
    DEBUG_PRINTLN("  BACKUP:  GPIO 6 (CLK), 7 (DT), 19 (SW)");

    if (logger) {
        logger->log(EVENT_SYSTEM_START, 0, 0, "Dual Encoder: Zainicjalizowany");
    }
}

void DualEncoderManager::update() {
    if (!initialized) return;

    // Aktualizacja obu enkoderów
    primaryEncoder->update();
    backupEncoder->update();

    // Sprawdzenie co ENCODER_CHECK_INTERVAL
    unsigned long currentTime = millis();
    if (currentTime - lastCheckTime >= ENCODER_CHECK_INTERVAL) {
        updateStatus();
        lastCheckTime = currentTime;
    }
}

void DualEncoderManager::updateStatus() {
    // Pobierz aktualne odczyty
    long primaryDist = primaryEncoder->getDistance();
    long backupDist = backupEncoder->getDistance();

    // 1. Sprawdź spójność między enkoderami
    if (!checkConsistency()) {
        divergenceCount++;

        if (divergenceCount >= 3) {
            // Po 3 kolejnych rozbieżnościach = problem
            if (activeEncoder == ENCODER_PRIMARY) {
                primaryStatus = ENCODER_DIVERGENCE;
                DEBUG_PRINTF("WARNING: PRIMARY ENCODER - Rozbieznosc! (PRIMARY=%ld, BACKUP=%ld)\n",
                           primaryDist, backupDist);

                if (logger) {
                    logger->log(EVENT_ERROR_OCCURRED, primaryDist, backupDist,
                               "PRIMARY: Rozbieznosc enkoderow");
                }

                // Przełącz na backup
                switchToBackup();
            } else {
                backupStatus = ENCODER_DIVERGENCE;
                DEBUG_PRINTF("WARNING: BACKUP ENCODER - Rozbieznosc! (PRIMARY=%ld, BACKUP=%ld)\n",
                           primaryDist, backupDist);

                if (logger) {
                    logger->log(EVENT_ERROR_OCCURRED, primaryDist, backupDist,
                               "BACKUP: Rozbieznosc enkoderow");
                }
            }
        }
    } else {
        // Spójność OK - reset licznika
        if (divergenceCount > 0) {
            DEBUG_PRINTLN("DualEncoder: Spójność przywrócona");
        }
        divergenceCount = 0;

        // Przywróć status OK jeśli był DIVERGENCE
        if (primaryStatus == ENCODER_DIVERGENCE) {
            primaryStatus = ENCODER_OK;
        }
        if (backupStatus == ENCODER_DIVERGENCE) {
            backupStatus = ENCODER_OK;
        }
    }

    // 2. Sprawdź czy enkodery się poruszają (nie stuck)
    bool primaryMoving = checkMovement(primaryEncoder, lastPrimaryDistance);
    bool backupMoving = checkMovement(backupEncoder, lastBackupDistance);

    if (!primaryMoving) {
        primaryStuckCount++;
        if (primaryStuckCount >= 5 && activeEncoder == ENCODER_PRIMARY) {
            primaryStatus = ENCODER_STUCK;
            DEBUG_PRINTLN("ERROR: PRIMARY ENCODER - Zawieszony!");

            if (logger) {
                logger->log(EVENT_ERROR_OCCURRED, 0, 0, "PRIMARY: Enkoder zawieszony");
            }

            switchToBackup();
        }
    } else {
        primaryStuckCount = 0;
        if (primaryStatus == ENCODER_STUCK) {
            primaryStatus = ENCODER_OK;
        }
    }

    if (!backupMoving) {
        backupStuckCount++;
        if (backupStuckCount >= 5 && activeEncoder == ENCODER_BACKUP) {
            backupStatus = ENCODER_STUCK;
            DEBUG_PRINTLN("ERROR: BACKUP ENCODER - Zawieszony!");

            if (logger) {
                logger->log(EVENT_ERROR_OCCURRED, 0, 0, "BACKUP: Enkoder zawieszony");
            }

            // Próbuj wrócić do primary
            tryRestorePrimary();
        }
    } else {
        backupStuckCount = 0;
        if (backupStatus == ENCODER_STUCK) {
            backupStatus = ENCODER_OK;
        }
    }

    // 3. Próba przywrócenia primary gdy backup jest aktywny
    if (activeEncoder == ENCODER_BACKUP && primaryStatus == ENCODER_OK) {
        tryRestorePrimary();
    }

    // Zapisz ostatnie pozycje
    lastPrimaryDistance = primaryDist;
    lastBackupDistance = backupDist;
}

bool DualEncoderManager::checkConsistency() {
    long primaryDist = primaryEncoder->getDistance();
    long backupDist = backupEncoder->getDistance();

    long difference = abs(primaryDist - backupDist);

    return (difference <= ENCODER_TOLERANCE_CM);
}

bool DualEncoderManager::checkMovement(EncoderHandler* encoder, long lastDistance) {
    // Jeśli dystans się zmienił = enkoder się porusza
    long currentDistance = encoder->getDistance();
    return (currentDistance != lastDistance);
}

void DualEncoderManager::switchToBackup() {
    if (activeEncoder == ENCODER_BACKUP) {
        // Już na backupie
        return;
    }

    DEBUG_PRINTLN("!!! PRZEŁĄCZANIE NA BACKUP ENCODER !!!");

    activeEncoder = ENCODER_BACKUP;
    warningShown = true;

    if (logger) {
        logger->log(EVENT_ERROR_OCCURRED, ENCODER_PRIMARY, ENCODER_BACKUP,
                   "PRZELACZENIE: PRIMARY -> BACKUP");
    }

    // Synchronizuj dystans backupu z primary (jeśli backup OK)
    if (backupStatus == ENCODER_OK) {
        // Możemy zaufać primary distance jako punkt odniesienia
        // (backup będzie kontynuował od tego punktu)
        DEBUG_PRINTF("Synchronizacja: Backup kontynuuje od %ld cm\n",
                    primaryEncoder->getDistance());
    }
}

void DualEncoderManager::tryRestorePrimary() {
    if (activeEncoder == ENCODER_PRIMARY) {
        // Już na primary
        return;
    }

    // Sprawdź czy primary jest zdrowy
    if (primaryStatus == ENCODER_OK && checkConsistency()) {
        DEBUG_PRINTLN(">>> PRZYWRACANIE PRIMARY ENCODER <<<");

        activeEncoder = ENCODER_PRIMARY;
        warningShown = false;

        if (logger) {
            logger->log(EVENT_SYSTEM_START, ENCODER_BACKUP, ENCODER_PRIMARY,
                       "PRZYWROCENIE: BACKUP -> PRIMARY");
        }
    }
}

void DualEncoderManager::reset() {
    primaryEncoder->reset();
    backupEncoder->reset();

    lastPrimaryDistance = 0;
    lastBackupDistance = 0;

    DEBUG_PRINTLN("DualEncoder: Reset obu enkoderów");
}

void DualEncoderManager::resetDistance() {
    primaryEncoder->resetDistance();
    backupEncoder->resetDistance();

    lastPrimaryDistance = 0;
    lastBackupDistance = 0;

    DEBUG_PRINTLN("DualEncoder: Reset dystansu obu enkoderów");
}

long DualEncoderManager::getDistance() {
    if (activeEncoder == ENCODER_PRIMARY) {
        return primaryEncoder->getDistance();
    } else {
        return backupEncoder->getDistance();
    }
}

long DualEncoderManager::getPosition() {
    if (activeEncoder == ENCODER_PRIMARY) {
        return primaryEncoder->getPosition();
    } else {
        return backupEncoder->getPosition();
    }
}

float DualEncoderManager::getSpeed() {
    if (activeEncoder == ENCODER_PRIMARY) {
        return primaryEncoder->getSpeed();
    } else {
        return backupEncoder->getSpeed();
    }
}

void DualEncoderManager::setPulsesPerCm(float pulses) {
    primaryEncoder->setPulsesPerCm(pulses);
    backupEncoder->setPulsesPerCm(pulses);

    DEBUG_PRINTF("DualEncoder: Kalibracja obu enkoderow: %.4f pulsów/cm\n", pulses);
}

float DualEncoderManager::getPulsesPerCm() {
    // Zwróć z aktywnego enkodera
    if (activeEncoder == ENCODER_PRIMARY) {
        return primaryEncoder->getPulsesPerCm();
    } else {
        return backupEncoder->getPulsesPerCm();
    }
}

bool DualEncoderManager::isCalibrated() {
    // System skalibrowany jeśli chociaż jeden enkoder skalibrowany
    return primaryEncoder->isCalibrated() || backupEncoder->isCalibrated();
}

const char* DualEncoderManager::getActiveEncoderName() {
    return (activeEncoder == ENCODER_PRIMARY) ? "PRIMARY" : "BACKUP";
}

const char* DualEncoderManager::getStatusName(EncoderStatus status) {
    switch (status) {
        case ENCODER_OK:         return "OK";
        case ENCODER_DIVERGENCE: return "ROZBIEZNOSC";
        case ENCODER_STUCK:      return "ZAWIESZONY";
        case ENCODER_FAILED:     return "AWARIA";
        default:                 return "UNKNOWN";
    }
}

bool DualEncoderManager::isHealthy() {
    // System zdrowy jeśli co najmniej jeden enkoder OK
    return (primaryStatus == ENCODER_OK || backupStatus == ENCODER_OK);
}

void DualEncoderManager::forceEncoder(ActiveEncoder encoder) {
    activeEncoder = encoder;
    DEBUG_PRINTF("DualEncoder: Wymuszono enkoder: %s\n", getActiveEncoderName());
}
