/**
 * Serwer WiFi dla systemu malowania pasów
 * Wersja: 1.4.0
 *
 * WiFi AP: Trassar
 * Hasło: 12345678
 * Port: 80
 *
 * REST API Endpoints:
 * - GET / → HTML dashboard
 * - GET /status → JSON ze statusem systemowym
 * - GET /control?action=start|pause|stop → Sterowanie systemem
 * - GET /pattern?set=P1A|P1B|... → Zmiana wzorca
 * - GET /api/startfromgap?enable=0|1 → Włączenie Start Od Przerwy
 */

#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "config_v140_NEW.h"  // BUGFIX v1.6.4: Zmiana z config.h (konflikt include guards!)
#include "patterns.h"

// BUGFIX v1.6.2: Forward declarations dla RelayController i EncoderHandler
class RelayController;
class EncoderHandler;

class WiFiServerManager {
private:
    WebServer* server;
    SystemState* systemState;
    RelayController* relays;
    EncoderHandler* encoder;

    bool wifiConnected;
    unsigned long lastStatusUpdate;

    // Handlery dla endpointów
    void handleRoot();
    void handleStatus();
    void handleControl();
    void handlePattern();
    void handleStartFromGap();
    void handleNotFound();

    // Helper functions
    String getStatusJSON();
    const char* getStateNamePL(SystemStateType state);

    // Helper do mapowania nazw wzorców
    PatternType patternNameToEnum(const String& name);

public:
    WiFiServerManager(SystemState* state, RelayController* rel, EncoderHandler* enc);

    void init();
    void update();
    bool isConnected();
    String getIP();
};

#endif // WIFI_SERVER_H
