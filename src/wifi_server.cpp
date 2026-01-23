/**
 * Implementacja serwera WiFi dla systemu malowania pasów
 * Wersja: 1.4.0
 *
 * REST API z obsługą:
 * - Dashboard HTML
 * - JSON status
 * - Sterowanie (start/pauza/stop)
 * - Zmiana wzorców
 * - Start Od Przerwy (v1.3.0)
 */

#include "wifi_server.h"

// HTML dashboard - responsywny, czytelny, prosty
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Trassar - Panel Sterowania Pasów</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #1a1a2e 0%, #16213e 100%);
            color: #fff;
            padding: 10px;
            min-height: 100vh;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
        }
        header {
            text-align: center;
            margin-bottom: 30px;
            padding: 20px 0;
            border-bottom: 3px solid #00ff00;
        }
        h1 {
            color: #00ff00;
            font-size: 2.5em;
            margin-bottom: 5px;
            text-shadow: 0 0 10px #00ff00;
        }
        .subtitle {
            color: #888;
            font-size: 0.9em;
        }
        .status-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-bottom: 30px;
        }
        .status-box {
            background: rgba(42, 42, 42, 0.9);
            padding: 20px;
            border-radius: 8px;
            border-left: 4px solid #00ff00;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.3);
        }
        .status-label {
            color: #aaa;
            font-size: 0.85em;
            margin-bottom: 8px;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        .status-value {
            font-size: 1.8em;
            color: #00ff00;
            font-weight: bold;
            font-family: 'Courier New', monospace;
        }
        .state-idle { border-left-color: #ffaa00; }
        .state-painting { border-left-color: #00ff00; }
        .state-paused { border-left-color: #ffff00; }
        .state-error { border-left-color: #ff0000; }

        .controls {
            text-align: center;
            margin-bottom: 30px;
        }
        button {
            background: linear-gradient(135deg, #00ff00, #00cc00);
            color: #000;
            border: none;
            padding: 12px 30px;
            font-size: 1em;
            border-radius: 5px;
            margin: 8px;
            cursor: pointer;
            font-weight: bold;
            transition: all 0.3s;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3);
        }
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 12px rgba(0, 0, 0, 0.4);
        }
        button:active {
            background: #00aa00;
            transform: translateY(0);
        }
        button.stop {
            background: linear-gradient(135deg, #ff3333, #cc0000);
            color: #fff;
        }
        button.stop:hover {
            background: linear-gradient(135deg, #ff4444, #dd0000);
        }
        button.pause {
            background: linear-gradient(135deg, #ffaa00, #ff8800);
        }

        section {
            margin-bottom: 30px;
        }
        h2 {
            color: #00ff00;
            margin-bottom: 15px;
            padding-bottom: 10px;
            border-bottom: 2px solid #00ff00;
        }

        .pattern-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(90px, 1fr));
            gap: 8px;
        }
        .pattern-btn {
            padding: 12px;
            background: rgba(51, 51, 51, 0.9);
            border: 2px solid #555;
            color: #fff;
            border-radius: 5px;
            cursor: pointer;
            font-weight: bold;
            transition: all 0.2s;
            text-transform: uppercase;
            font-size: 0.9em;
        }
        .pattern-btn:hover {
            border-color: #00ff00;
            box-shadow: 0 0 10px rgba(0, 255, 0, 0.3);
        }
        .pattern-btn.active {
            border-color: #00ff00;
            background: rgba(0, 255, 0, 0.1);
            box-shadow: 0 0 15px rgba(0, 255, 0, 0.5);
            color: #00ff00;
        }

        .options {
            display: flex;
            gap: 15px;
            margin-top: 15px;
            flex-wrap: wrap;
        }
        .option {
            display: flex;
            align-items: center;
            gap: 8px;
        }
        .option input[type="checkbox"] {
            cursor: pointer;
            width: 18px;
            height: 18px;
        }
        .option label {
            cursor: pointer;
        }

        .footer {
            text-align: center;
            margin-top: 40px;
            padding-top: 20px;
            border-top: 1px solid #444;
            color: #666;
            font-size: 0.85em;
        }

        @media (max-width: 600px) {
            h1 { font-size: 1.8em; }
            .status-grid { grid-template-columns: 1fr; }
            button { padding: 10px 20px; margin: 5px; }
            .pattern-grid { grid-template-columns: repeat(3, 1fr); }
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🚗 TRASSAR</h1>
            <p class="subtitle">System Sterowania Malowaniem Pasów Drogowych v1.4.0</p>
        </header>

        <div class="status-grid">
            <div class="status-box" id="stateBox">
                <div class="status-label">Status</div>
                <div class="status-value" id="state">Gotowy</div>
            </div>
            <div class="status-box">
                <div class="status-label">Wzorzec</div>
                <div class="status-value" id="pattern">P-1a</div>
            </div>
            <div class="status-box">
                <div class="status-label">Prędkość</div>
                <div class="status-value" id="speed">0.0 km/h</div>
            </div>
            <div class="status-box">
                <div class="status-label">Dysans</div>
                <div class="status-value" id="distance">0.00 m</div>
            </div>
            <div class="status-box">
                <div class="status-label">Powierzchnia</div>
                <div class="status-value" id="area">0.00 m²</div>
            </div>
            <div class="status-box">
                <div class="status-label">Bezpieczeństwo</div>
                <div class="status-value" id="safety" style="color: #00ff00;">Aktywne</div>
            </div>
        </div>

        <div class="controls">
            <button onclick="control('start')" style="background: linear-gradient(135deg, #00ff00, #00cc00); color: #000;">
                ▶️ START
            </button>
            <button class="pause" onclick="control('pause')">
                ⏸️ PAUZA
            </button>
            <button class="stop" onclick="control('stop')">
                ⏹️ STOP
            </button>
        </div>

        <section>
            <h2>Wybór Wzorca</h2>
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
        </section>

        <section>
            <h2>Opcje</h2>
            <div class="options">
                <div class="option">
                    <input type="checkbox" id="startFromGap" onchange="toggleStartFromGap()">
                    <label for="startFromGap">Start Od Przerwy</label>
                </div>
            </div>
        </section>

        <footer class="footer">
            <p>Trassar Road Painter System © 2026 | Offline Mode: WiFi AP</p>
        </footer>
    </div>

    <script>
        // Auto-update statusu
        async function updateStatus() {
            try {
                const response = await fetch('/status');
                const data = await response.json();

                document.getElementById('state').textContent = data.stateName;
                document.getElementById('pattern').textContent = data.pattern;
                document.getElementById('speed').textContent = data.speed.toFixed(1) + ' km/h';
                document.getElementById('area').textContent = data.area.toFixed(2) + ' m²';
                document.getElementById('distance').textContent = (data.distance / 100).toFixed(2) + ' m';
                document.getElementById('safety').textContent = data.safetyLocked ? 'Zablokowane' : 'Aktywne';
                document.getElementById('safety').style.color = data.safetyLocked ? '#ff0000' : '#00ff00';
                document.getElementById('startFromGap').checked = data.startFromGap;

                // Zmiana koloru stanu
                const stateBox = document.getElementById('stateBox');
                stateBox.className = 'status-box';
                if (data.stateName.includes('Gotowy')) stateBox.classList.add('state-idle');
                else if (data.stateName.includes('Malowanie')) stateBox.classList.add('state-painting');
                else if (data.stateName.includes('Wstrzymane')) stateBox.classList.add('state-paused');
                else if (data.stateName.includes('Błąd')) stateBox.classList.add('state-error');
            } catch (e) {
                console.error('Błąd pobierania statusu:', e);
            }
        }

        async function control(action) {
            try {
                const response = await fetch('/control?action=' + action);
                const data = await response.json();
                if (data.status === 'ok') {
                    updateStatus();
                } else {
                    alert('Błąd: ' + data.message);
                }
            } catch (e) {
                alert('Błąd komunikacji: ' + e.message);
            }
        }

        async function setPattern(pattern) {
            try {
                const response = await fetch('/pattern?set=' + pattern);
                const data = await response.json();
                if (data.status === 'ok') {
                    updateStatus();
                    // Zaznacz przycisk
                    document.querySelectorAll('.pattern-btn').forEach(btn => {
                        btn.classList.remove('active');
                    });
                    event.target.classList.add('active');
                } else {
                    alert('Błąd: ' + data.message);
                }
            } catch (e) {
                alert('Błąd komunikacji: ' + e.message);
            }
        }

        async function toggleStartFromGap() {
            const enabled = document.getElementById('startFromGap').checked ? 1 : 0;
            try {
                const response = await fetch('/api/startfromgap?enable=' + enabled);
                const data = await response.json();
                if (data.status !== 'ok') {
                    alert('Błąd: ' + data.message);
                }
            } catch (e) {
                alert('Błąd komunikacji: ' + e.message);
            }
        }

        // Aktualizuj status co 500ms
        setInterval(updateStatus, 500);
        updateStatus();
    </script>
</body>
</html>
)rawliteral";

/**
 * Konstruktor - inicjalizacja wskaźników
 */
WiFiServerManager::WiFiServerManager(SystemState* state, RelayController* rel, EncoderHandler* enc) {
    systemState = state;
    relays = rel;
    encoder = enc;
    server = nullptr;
    wifiConnected = false;
    lastStatusUpdate = 0;

    DEBUG_PRINTLN("[WiFi] Konstruktor WiFiServerManager");
}

/**
 * Inicjalizacja WiFi w trybie Access Point (AP)
 * SSID: Trassar, Hasło: 12345678
 */
void WiFiServerManager::init() {
    DEBUG_PRINTLN("\n========== Inicjalizacja WiFi ==========");
    DEBUG_PRINTF("[WiFi] SSID: %s\n", WIFI_SSID);
    DEBUG_PRINTF("[WiFi] Hasło: %s\n", WIFI_PASSWORD);
    DEBUG_PRINTF("[WiFi] Port: %d\n", WEB_SERVER_PORT);

    // Tryb Access Point (AP)
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(
        IPAddress(192, 168, 4, 1),      // IP AP
        IPAddress(192, 168, 4, 1),      // Gateway
        IPAddress(255, 255, 255, 0)     // Maska podsieci
    );

    if (!WiFi.softAP(WIFI_SSID, WIFI_PASSWORD, WIFI_AP_CHANNEL, false, WIFI_MAX_CONNECTIONS)) {
        DEBUG_PRINTLN("[WiFi] BŁĄD: Nie udało się uruchomić AP!");
        wifiConnected = false;
        return;
    }

    IPAddress IP = WiFi.softAPIP();
    DEBUG_PRINTF("[WiFi] IP Address: %s\n", IP.toString().c_str());
    DEBUG_PRINTF("[WiFi] Broadcast: %s\n", WiFi.softAPBroadcastIP().toString().c_str());

    // Inicjalizacja web servera
    server = new WebServer(WEB_SERVER_PORT);

    // Rejestracja endpointów
    server->on("/", [this]() { this->handleRoot(); });
    server->on("/status", [this]() { this->handleStatus(); });
    server->on("/control", [this]() { this->handleControl(); });
    server->on("/pattern", [this]() { this->handlePattern(); });
    server->on("/api/startfromgap", [this]() { this->handleStartFromGap(); });
    server->onNotFound([this]() { this->handleNotFound(); });

    server->begin();
    wifiConnected = true;

    DEBUG_PRINTLN("[WiFi] ========== WiFi Server Gotów ==========");
    DEBUG_PRINTF("[WiFi] Otwórz przeglądarkę: http://%s\n", IP.toString().c_str());
    DEBUG_PRINTLN("[WiFi] Endpoint dokumentacja:");
    DEBUG_PRINTLN("[WiFi]   GET / → HTML Dashboard");
    DEBUG_PRINTLN("[WiFi]   GET /status → JSON Status");
    DEBUG_PRINTLN("[WiFi]   GET /control?action=start|pause|stop");
    DEBUG_PRINTLN("[WiFi]   GET /pattern?set=P1A|P1B|...");
    DEBUG_PRINTLN("[WiFi]   GET /api/startfromgap?enable=0|1");
}

/**
 * Obsługa klientów WiFi w głównej pętli
 */
void WiFiServerManager::update() {
    if (wifiConnected && server) {
        server->handleClient();
    }
}

/**
 * Sprawdzenie statusu połączenia
 */
bool WiFiServerManager::isConnected() {
    return wifiConnected;
}

/**
 * Pobranie IP Access Point
 */
String WiFiServerManager::getIP() {
    return WiFi.softAPIP().toString();
}

// ============================================================================
// HANDLERY DLA ENDPOINTÓW
// ============================================================================

/**
 * GET / - Serwowanie HTML dashboard
 */
void WiFiServerManager::handleRoot() {
    DEBUG_PRINTLN("[WiFi] GET / - Żądanie Dashboard");
    server->send(200, "text/html; charset=utf-8", HTML_PAGE);
}

/**
 * GET /status - JSON ze statusem systemowym
 * Zwraca: wzorzec, prędkość, powierzchnia, dystans, stan, bezpieczeństwo, startFromGap
 */
void WiFiServerManager::handleStatus() {
    String json = getStatusJSON();
    DEBUG_PRINTF("[WiFi] GET /status - Odpowiedź: %s\n", json.c_str());
    server->send(200, "application/json; charset=utf-8", json);
}

/**
 * GET /control?action=start|pause|stop - Sterowanie systemem
 */
void WiFiServerManager::handleControl() {
    if (!server->hasArg("action")) {
        DEBUG_PRINTLN("[WiFi] GET /control - BŁĄD: Brak parametru 'action'");
        server->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Brakuje parametru action\"}");
        return;
    }

    String action = server->arg("action");
    action.toLowerCase();

    DEBUG_PRINTF("[WiFi] GET /control?action=%s\n", action.c_str());

    String response;

    if (action == "start") {
        if (systemState->state == STATE_IDLE || systemState->state == STATE_PAUSED) {
            systemState->state = STATE_PAINTING;
            response = "{\"status\":\"ok\",\"message\":\"Malowanie rozpoczęte\"}";
            DEBUG_PRINTLN("[WiFi] → Zmiana stanu na STATE_PAINTING");
        } else {
            response = "{\"status\":\"error\",\"message\":\"System nie jest gotowy\"}";
            DEBUG_PRINTF("[WiFi] → Błąd: Stan systemu = %d\n", systemState->state);
        }
    }
    else if (action == "pause") {
        if (systemState->state == STATE_PAINTING) {
            systemState->state = STATE_PAUSED;
            response = "{\"status\":\"ok\",\"message\":\"Malowanie wstrzymane\"}";
            DEBUG_PRINTLN("[WiFi] → Zmiana stanu na STATE_PAUSED");
        } else {
            response = "{\"status\":\"error\",\"message\":\"System nie maluje\"}";
        }
    }
    else if (action == "stop") {
        systemState->state = STATE_IDLE;
        relays->stopAll();
        response = "{\"status\":\"ok\",\"message\":\"Malowanie zatrzymane\"}";
        DEBUG_PRINTLN("[WiFi] → Zmiana stanu na STATE_IDLE + stopAll()");
    }
    else {
        response = "{\"status\":\"error\",\"message\":\"Nieznana akcja\"}";
        DEBUG_PRINTF("[WiFi] → Błąd: Nieznana akcja '%s'\n", action.c_str());
    }

    server->send(200, "application/json", response);
}

/**
 * GET /pattern?set=P1A|P1B|... - Zmiana wzorca malowania
 */
void WiFiServerManager::handlePattern() {
    if (!server->hasArg("set")) {
        DEBUG_PRINTLN("[WiFi] GET /pattern - BŁĄD: Brak parametru 'set'");
        server->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Brakuje parametru set\"}");
        return;
    }

    String patternName = server->arg("set");
    patternName.toUpperCase();

    DEBUG_PRINTF("[WiFi] GET /pattern?set=%s\n", patternName.c_str());

    PatternType newPattern = patternNameToEnum(patternName);

    if (newPattern == PATTERN_P1A || (newPattern >= PATTERN_P1A && newPattern < PATTERN_COUNT)) {
        systemState->currentPattern = newPattern;
        systemState->patternChanged = true;

        String response = "{\"status\":\"ok\",\"message\":\"Wzorzec zmieniony na " + patternName + "\"}";
        DEBUG_PRINTF("[WiFi] → Nowy wzorzec: %s\n", getPatternName(newPattern));
        server->send(200, "application/json", response);
    }
    else {
        DEBUG_PRINTF("[WiFi] → Błąd: Nieznany wzorzec '%s'\n", patternName.c_str());
        server->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Nieznany wzorzec\"}");
    }
}

/**
 * GET /api/startfromgap?enable=0|1 - Toggle "Start Od Przerwy" (v1.3.0)
 */
void WiFiServerManager::handleStartFromGap() {
    if (!server->hasArg("enable")) {
        DEBUG_PRINTLN("[WiFi] GET /api/startfromgap - BŁĄD: Brak parametru 'enable'");
        server->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Brakuje parametru enable\"}");
        return;
    }

    String enableStr = server->arg("enable");
    bool enabled = (enableStr == "1" || enableStr.equalsIgnoreCase("true"));

    DEBUG_PRINTF("[WiFi] GET /api/startfromgap?enable=%s\n", enabled ? "1" : "0");

    systemState->startFromGap = enabled;

    String response = "{\"status\":\"ok\",\"message\":\"Start od przerwy: ";
    response += (enabled ? "WŁĄCZONE" : "WYŁĄCZONE");
    response += "\"}";

    DEBUG_PRINTF("[WiFi] → Start Od Przerwy: %s\n", enabled ? "WŁĄCZONE" : "WYŁĄCZONE");
    server->send(200, "application/json", response);
}

/**
 * Obsługa 404 Not Found
 */
void WiFiServerManager::handleNotFound() {
    DEBUG_PRINTF("[WiFi] GET %s - 404 Not Found\n", server->uri().c_str());
    server->send(404, "application/json", "{\"error\":\"Endpoint nie znaleziony\"}");
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * Generowanie JSON ze statusem systemowym
 */
String WiFiServerManager::getStatusJSON() {
    // Manualne generowanie JSON (bez ArduinoJson, aby zaoszczędzić RAM)
    String json = "{";

    // Status i wzorzec
    json += "\"state\":" + String((int)systemState->state) + ",";
    json += "\"stateName\":\"" + String(getStateNamePL(systemState->state)) + "\",";
    json += "\"pattern\":\"" + String(getPatternName(systemState->currentPattern)) + "\",";
    json += "\"reversed\":" + String(systemState->patternReversed ? "true" : "false") + ",";

    // Dane pomiarowe
    json += "\"speed\":" + String(systemState->speed, 2) + ",";
    json += "\"distance\":" + String(systemState->distance) + ",";
    json += "\"area\":" + String(systemState->totalPaintedArea, 3) + ",";

    // Bezpieczeństwo i opcje
    json += "\"safetyLocked\":" + String(systemState->safetyLocked ? "true" : "false") + ",";
    json += "\"startFromGap\":" + String(systemState->startFromGap ? "true" : "false");

    json += "}";
    return json;
}

/**
 * Zwraca nazwę stanu w języku polskim
 */
const char* WiFiServerManager::getStateNamePL(SystemStateType state) {
    switch (state) {
        case STATE_IDLE:       return "Gotowy";
        case STATE_PAINTING:   return "Malowanie";
        case STATE_PAUSED:     return "Wstrzymane";
        case STATE_MENU:       return "Menu";
        case STATE_CALIBRATING: return "Kalibracja";
        case STATE_MEASURING:  return "Pomiar";
        case STATE_ERROR:      return "Błąd";
        default:               return "Nieznany";
    }
}

/**
 * Mapowanie tekstu na enum PatternType
 */
PatternType WiFiServerManager::patternNameToEnum(const String& name) {
    if (name == "P1A") return PATTERN_P1A;
    if (name == "P1B") return PATTERN_P1B;
    if (name == "P1C") return PATTERN_P1C;
    if (name == "P1D") return PATTERN_P1D;
    if (name == "P1E") return PATTERN_P1E;
    if (name == "P2A") return PATTERN_P2A;
    if (name == "P2B") return PATTERN_P2B;
    if (name == "P3A") return PATTERN_P3A;
    if (name == "P3B") return PATTERN_P3B;
    if (name == "P4") return PATTERN_P4;
    if (name == "P6") return PATTERN_P6;
    if (name == "P7A") return PATTERN_P7A;
    if (name == "P7B") return PATTERN_P7B;
    if (name == "P7C") return PATTERN_P7C;
    if (name == "P7D") return PATTERN_P7D;
    return PATTERN_P1A;  // Domyślnie P-1a jeśli nieznany
}
