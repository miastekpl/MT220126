/**
 * Serwer WiFi dla systemu malowania pasów
 * Wersja: 1.2.0
 * 
 * SSID: Trassar
 * Hasło: 12345678
 * Port: 80
 */

#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h"
#include "patterns.h"

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
    void handleCalibration();
    void handleNotFound();
    
    // Helper functions
    String getStatusJSON();
    String getWebInterface();

public:
    WiFiServerManager(SystemState* state, RelayController* rel, EncoderHandler* enc);
    
    void init();
    void update();
    bool isConnected();
    String getIP();
};

#endif // WIFI_SERVER_H
