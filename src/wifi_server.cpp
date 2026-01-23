/**
 * Implementacja serwera WiFi
 */

#include "wifi_server.h"

// HTML panel sterowania (uproszczony)
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Trassar - Panel Sterowania</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: Arial, sans-serif; background: #1a1a1a; color: #fff; padding: 20px; }
        .container { max-width: 600px; margin: 0 auto; }
        h1 { color: #00ff00; margin-bottom: 20px; }
        .status-box { background: #2a2a2a; padding: 20px; border-radius: 10px; margin-bottom: 20px; }
        .value { font-size: 2em; color: #00ff00; font-weight: bold; }
        .label { color: #888; margin-bottom: 5px; }
        button { background: #00ff00; color: #000; border: none; padding: 15px 30px; font-size: 1.2em; border-radius: 5px; margin: 10px; cursor: pointer; }
        button:active { background: #00cc00; }
        button.stop { background: #ff0000; color: #fff; }
        .pattern-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; margin: 20px 0; }
        .pattern-btn { padding: 10px; background: #333; border: 2px solid #555; }
        .pattern-btn.active { border-color: #00ff00; background: #003300; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚗 TRASSAR Road Painter</h1>
        
        <div class="status-box">
            <div class="label">Wzorzec</div>
            <div class="value" id="pattern">P-1a</div>
        </div>
        
        <div class="status-box">
            <div class="label">Prędkość</div>
            <div class="value" id="speed">0.0 km/h</div>
        </div>
        
        <div class="status-box">
            <div class="label">Powierzchnia</div>
            <div class="value" id="area">0.00 m²</div>
        </div>
        
        <div class="status-box">
            <div class="label">Dystans</div>
            <div class="value" id="distance">0.00 m</div>
        </div>
        
        <div style="text-align: center; margin: 30px 0;">
            <button onclick="control('start')">▶️ START</button>
            <button onclick="control('pause')">⏸️ PAUZA</button>
            <button class="stop" onclick="control('stop')">⏹️ STOP</button>
        </div>
        
        <h2 style="margin: 30px 0 10px 0;">Wybór Wzorca</h2>
        <div class="pattern-grid">
            <button class="pattern-btn" onclick="setPattern('P1A')">P-1a</button>
            <button class="pattern-btn" onclick="setPattern('P1B')">P-1b</button>
            <button class="pattern-btn" onclick="setPattern('P1C')">P-1c</button>
            <button class="pattern-btn" onclick="setPattern('P1D')">P-1d</button>
            <button class="pattern-btn" onclick="setPattern('P1E')">P-1e</button>
            <button class="pattern-btn" onclick="setPattern('P2A')">P-2a</button>
            <button class="pattern-btn" onclick="setPattern('P2B')">P-2b</button>
            <button class="pattern-btn" onclick="setPattern('P3A')">P-3a</button>
            <button class="pattern-btn" onclick="setPattern('P3B')">P-3b</button>
            <button class="pattern-btn" onclick="setPattern('P4')">P-4</button>
            <button class="pattern-btn" onclick="setPattern('P6')">P-6</button>
            <button class="pattern-btn" onclick="setPattern('P7A')">P-7a</button>
            <button class="pattern-btn" onclick="setPattern('P7B')">P-7b</button>
            <button class="pattern-btn" onclick="setPattern('P7C')">P-7c</button>
            <button class="pattern-btn" onclick="setPattern('P7D')">P-7d</button>
        </div>
    </div>
    
    <script>
        function updateStatus() {
            fetch('/status')
                .then(r => r.json())
                .then(data => {
                    document.getElementById('pattern').textContent = data.pattern;
                    document.getElementById('speed').textContent = data.speed.toFixed(1) + ' km/h';
                    document.getElementById('area').textContent = data.area.toFixed(2) + ' m²';
                    document.getElementById('distance').textContent = (data.distance/100).toFixed(2) + ' m';
                });
        }
        
        function control(action) {
            fetch('/control?action=' + action)
                .then(r => r.json())
                .then(data => alert(data.message));
        }
        
        function setPattern(pattern) {
            fetch('/pattern?set=' + pattern)
                .then(r => r.json())
                .then(data => {
                    alert(data.message);
                    updateStatus();
                });
        }
        
        setInterval(updateStatus, 500);
        updateStatus();
    </script>
</body>
</html>
)rawliteral";

WiFiServerManager::WiFiServerManager(SystemState* state, RelayController* rel, EncoderHandler* enc) {
    systemState = state;
    relays = rel;
    encoder = enc;
    wifiConnected = false;
    lastStatusUpdate = 0;
}

void WiFiServerManager::init() {
    Serial.println("\n=== Inicjalizacja WiFi ===");
    Serial.printf("SSID: %s\n", WIFI_SSID);
    
    // Access Point mode
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    
    IPAddress IP = WiFi.softAPIP();
    Serial.printf("IP Address: %s\n", IP.toString().c_str());
    
    server = new WebServer(WEB_SERVER_PORT);
    
    // Definicja endpointów
    server->on("/", [this]() { this->handleRoot(); });
    server->on("/status", [this]() { this->handleStatus(); });
    server->on("/control", [this]() { this->handleControl(); });
    server->on("/pattern", [this]() { this->handlePattern(); });
    server->onNotFound([this]() { this->handleNotFound(); });
    
    server->begin();
    wifiConnected = true;
    
    Serial.println("WiFi Server uruchomiony!");
    Serial.printf("Adres: http://%s\n", IP.toString().c_str());
}

void WiFiServerManager::update() {
    if (wifiConnected) {
        server->handleClient();
    }
}

bool WiFiServerManager::isConnected() {
    return wifiConnected;
}

String WiFiServerManager::getIP() {
    return WiFi.softAPIP().toString();
}

void WiFiServerManager::handleRoot() {
    server->send(200, "text/html", HTML_PAGE);
}

void WiFiServerManager::handleStatus() {
    String json = getStatusJSON();
    server->send(200, "application/json", json);
}

String WiFiServerManager::getStatusJSON() {
    StaticJsonDocument<512> doc;
    
    doc["pattern"] = getPatternName(systemState->currentPattern);
    doc["speed"] = systemState->speed;
    doc["area"] = systemState->totalPaintedArea;
    doc["distance"] = systemState->distance;
    doc["state"] = (int)systemState->state;
    doc["reversed"] = systemState->patternReversed;
    doc["safetyLocked"] = systemState->safetyLocked;
    
    String output;
    serializeJson(doc, output);
    return output;
}

void WiFiServerManager::handleControl() {
    if (!server->hasArg("action")) {
        server->send(400, "application/json", "{\"error\":\"Missing action\"}");
        return;
    }
    
    String action = server->arg("action");
    String response;
    
    if (action == "start") {
        if (systemState->state == STATE_IDLE || systemState->state == STATE_PAUSED) {
            systemState->state = STATE_PAINTING;
            response = "{\"status\":\"ok\",\"message\":\"Malowanie rozpoczęte\"}";
        } else {
            response = "{\"status\":\"error\",\"message\":\"Nie można uruchomić\"}";
        }
    } else if (action == "pause") {
        if (systemState->state == STATE_PAINTING) {
            systemState->state = STATE_PAUSED;
            response = "{\"status\":\"ok\",\"message\":\"Malowanie wstrzymane\"}";
        } else {
            response = "{\"status\":\"error\",\"message\":\"System nie maluje\"}";
        }
    } else if (action == "stop") {
        systemState->state = STATE_IDLE;
        relays->stopAll();
        response = "{\"status\":\"ok\",\"message\":\"Malowanie zatrzymane\"}";
    } else {
        response = "{\"status\":\"error\",\"message\":\"Nieznana akcja\"}";
    }
    
    server->send(200, "application/json", response);
}

void WiFiServerManager::handlePattern() {
    if (!server->hasArg("set")) {
        server->send(400, "application/json", "{\"error\":\"Missing pattern\"}");
        return;
    }
    
    String pattern = server->arg("set");
    
    // Mapowanie nazw wzorców
    if (pattern == "P1A") systemState->currentPattern = PATTERN_P1A;
    else if (pattern == "P1B") systemState->currentPattern = PATTERN_P1B;
    else if (pattern == "P1C") systemState->currentPattern = PATTERN_P1C;
    else if (pattern == "P1D") systemState->currentPattern = PATTERN_P1D;
    else if (pattern == "P1E") systemState->currentPattern = PATTERN_P1E;
    else if (pattern == "P2A") systemState->currentPattern = PATTERN_P2A;
    else if (pattern == "P2B") systemState->currentPattern = PATTERN_P2B;
    else if (pattern == "P3A") systemState->currentPattern = PATTERN_P3A;
    else if (pattern == "P3B") systemState->currentPattern = PATTERN_P3B;
    else if (pattern == "P4") systemState->currentPattern = PATTERN_P4;
    else if (pattern == "P6") systemState->currentPattern = PATTERN_P6;
    else if (pattern == "P7A") systemState->currentPattern = PATTERN_P7A;
    else if (pattern == "P7B") systemState->currentPattern = PATTERN_P7B;
    else if (pattern == "P7C") systemState->currentPattern = PATTERN_P7C;
    else if (pattern == "P7D") systemState->currentPattern = PATTERN_P7D;
    else {
        server->send(400, "application/json", "{\"error\":\"Nieznany wzorzec\"}");
        return;
    }
    
    systemState->patternChanged = true;
    
    String response = "{\"status\":\"ok\",\"message\":\"Wzorzec zmieniony na " + pattern + "\"}";
    server->send(200, "application/json", response);
}

void WiFiServerManager::handleNotFound() {
    server->send(404, "text/plain", "404 Not Found");
}
