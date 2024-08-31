/* 
 *   Copyright (C) 2024 dingmos. All rights reserved.
 */
#include <Arduino.h>
#include "wireless_tuning.h"


#define WL_TUNE_MAX_PACKETSIZE  255


// const uint8_t *probe_rsp = (const uint8_t *)"STUDIO_YES";
// const char *probe_msg = "STUDIO_PROBE";
WirelessTuning::WirelessTuning( uint16_t localPort) 
  : _localPort(localPort), _packetSize(0), _packetIndex(0), _remotePort(UINT16_MAX)
{
}

WirelessTuning::~WirelessTuning() 
{
    // Destructor for cleanup
}

bool WirelessTuning::begin(const char* ssid, const char* password, 
                                            WiFiMode mode)
{
    
    bool success = false;
    if (mode == AP_MODE) {
        success = WiFi.softAP(ssid, password);
    } else if (mode == STA_MODE) {
        // Connect to WiFi network
        WiFi.begin(ssid, password);
        unsigned long start_time = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start_time < 10000) {
            delay(1000);
            Serial.println("Connecting to WiFi...");
        }
        
        success = (WiFi.status() == WL_CONNECTED);
    } else {
        // Invalid mode
        return false; // 
    }
    
    if (!success) {
        Serial.printf("setup WiFi failed\n");
        return false; // Failed to start in the selected mode
    }

    Serial.printf("Connected to WiFi.\n");
    Serial.printf("IP address: %s", WiFi.localIP().toString());
    
    _server = WiFiServer(_localPort);
    _server.begin(_localPort);

    return true;

}

int WirelessTuning::handleClientDisconnect() {
    if (_client && !_client.connected()) {

        Serial.printf("client disconnected, %s:%d\n", _client.remoteIP().toString(), 
                            _client.remotePort());
        _client.stop(); // Close the client connection
        _client = WiFiClient(); // Reset the client
        _packetSize = 0;
        _packetIndex = 0;


        return true;
    }
    return false;
}

void WirelessTuning::acceptClient() {
    if (!_client || !_client.connected()) {
        _client = _server.available(); // Accept a new _client
        if (!_client.connected()) {
            // invalid client
            return;
        }
        Serial.printf("accept new conn, %s:%d\n", _client.remoteIP().toString(), 
                            _client.remotePort());
    }
}

void WirelessTuning::receivePacket() {

    handleClientDisconnect(); // Check if the client is still connected
    acceptClient();  // Ensure we have a client connected before checking for data

    if (_client && _client.connected()) {
        if (_client.available()) {
            _packetSize = _client.read(_packetBuffer, sizeof(_packetBuffer));
            _packetIndex = 0;
        }
    }
}

int WirelessTuning::available() {
    if (handleClientDisconnect()) {
        return 0;
    }
    acceptClient();  // Ensure we have a client connected before writing data

    if (_packetIndex >= _packetSize) {
        receivePacket();  // Check for a new packet if the current one is fully read
    }
    return _packetSize - _packetIndex;
}

int WirelessTuning::read() {
    if (_packetIndex < _packetSize) {
        return _packetBuffer[_packetIndex++];
    } else {
        return -1; // No more data to read
    }
}

int WirelessTuning::peek() {
    if (_packetIndex < _packetSize) {
        return _packetBuffer[_packetIndex];
    } else {
        return -1; // No more data to peek at
    }
}

void WirelessTuning::flush() {
    if (_client) {
        _client.flush();
    }
}

size_t WirelessTuning::write(uint8_t data) {
    if (handleClientDisconnect()) {
        return 0;
    }
    acceptClient();  // Ensure we have a client connected before writing data

    if (_client && _client.connected()) {
        return _client.write(data);
    } else {
        return 0; // No _client to send data to
    }
}

size_t WirelessTuning::write(const uint8_t* buffer, size_t size) {

    if (handleClientDisconnect()) {
        return 0;
    }
    acceptClient();  // Ensure we have a client connected before writing data

    if (_client && _client.connected()) {
        return _client.write(buffer, size);
    } else {
        return 0; // No _client to send data to
    }
}
