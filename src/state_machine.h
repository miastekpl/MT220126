/**
 * State Machine Pattern - v1.4.0
 * Profesjonalne zarządzanie stanami systemu
 *
 * Architektura oparta na wzorcu State Pattern (GoF)
 * Każdy stan to osobna klasa z metodami enter(), update(), exit()
 *
 * Copyright (c) 2026 MT220126 Engineering Team
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include "config_v140_NEW.h"

// Forward declarations
class StateMachine;
class RelayController;
class DisplayManager;
class EncoderHandler;

/**
 * Bazowa klasa abstrakcyjna dla stanów
 */
class State {
public:
    virtual ~State() {}

    /**
     * Wywoływane gdy wchodzimy do stanu
     */
    virtual void enter(StateMachine* sm) = 0;

    /**
     * Wywoływane w każdej iteracji loop()
     */
    virtual void update(StateMachine* sm) = 0;

    /**
     * Wywoływane gdy wychodzimy ze stanu
     */
    virtual void exit(StateMachine* sm) = 0;

    /**
     * Zwraca nazwę stanu (do debugowania)
     */
    virtual const char* getName() const = 0;
};

/**
 * Stan: IDLE - Oczekiwanie
 */
class IdleState : public State {
public:
    void enter(StateMachine* sm) override;
    void update(StateMachine* sm) override;
    void exit(StateMachine* sm) override;
    const char* getName() const override { return "IDLE"; }
};

/**
 * Stan: PAINTING - Aktywne malowanie
 */
class PaintingState : public State {
private:
    unsigned long startTime;

public:
    void enter(StateMachine* sm) override;
    void update(StateMachine* sm) override;
    void exit(StateMachine* sm) override;
    const char* getName() const override { return "PAINTING"; }
};

/**
 * Stan: PAUSED - Wstrzymane
 */
class PausedState : public State {
public:
    void enter(StateMachine* sm) override;
    void update(StateMachine* sm) override;
    void exit(StateMachine* sm) override;
    const char* getName() const override { return "PAUSED"; }
};

/**
 * Stan: MENU - System menu
 */
class MenuState : public State {
public:
    void enter(StateMachine* sm) override;
    void update(StateMachine* sm) override;
    void exit(StateMachine* sm) override;
    const char* getName() const override { return "MENU"; }
};

/**
 * Stan: CALIBRATING - Kalibracja w toku
 */
class CalibratingState : public State {
public:
    void enter(StateMachine* sm) override;
    void update(StateMachine* sm) override;
    void exit(StateMachine* sm) override;
    const char* getName() const override { return "CALIBRATING"; }
};

/**
 * Stan: MEASURING - Pomiar dystansu
 */
class MeasuringState : public State {
public:
    void enter(StateMachine* sm) override;
    void update(StateMachine* sm) override;
    void exit(StateMachine* sm) override;
    const char* getName() const override { return "MEASURING"; }
};

/**
 * Stan: ERROR - Błąd systemu (NOWY v1.4.0)
 */
class ErrorState : public State {
private:
    unsigned long errorStartTime;
    bool errorAcknowledged;

public:
    void enter(StateMachine* sm) override;
    void update(StateMachine* sm) override;
    void exit(StateMachine* sm) override;
    const char* getName() const override { return "ERROR"; }
};

/**
 * Główna maszyna stanów
 */
class StateMachine {
private:
    State* currentState;
    State* previousState;

    // Instancje stanów (singleton pattern)
    IdleState idleState;
    PaintingState paintingState;
    PausedState pausedState;
    MenuState menuState;
    CalibratingState calibratingState;
    MeasuringState measuringState;
    ErrorState errorState;

    // Referencje do komponentów systemu
    RelayController* relays;
    DisplayManager* display;
    EncoderHandler* encoder;
    SystemState* systemState;

public:
    /**
     * Konstruktor
     */
    StateMachine(RelayController* r, DisplayManager* d, EncoderHandler* e, SystemState* s);

    /**
     * Inicjalizacja - ustawia stan początkowy
     */
    void init();

    /**
     * Aktualizacja - wywołaj w każdej iteracji loop()
     */
    void update();

    /**
     * Zmiana stanu
     */
    void changeState(SystemStateType newStateType);

    /**
     * Przejście do stanu ERROR
     */
    void error(ErrorCode code, const char* message);

    /**
     * Gettery
     */
    State* getCurrentState() { return currentState; }
    State* getPreviousState() { return previousState; }
    SystemStateType getCurrentStateType();

    /**
     * Dostęp do komponentów (dla stanów)
     */
    RelayController* getRelays() { return relays; }
    DisplayManager* getDisplay() { return display; }
    EncoderHandler* getEncoder() { return encoder; }
    SystemState* getSystemState() { return systemState; }
};

#endif // STATE_MACHINE_H
