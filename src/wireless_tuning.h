#ifndef __WIRELESS_TUNING_H__
#define __WIRELESS_TUNING_H__

#include <Stream.h>
#include <WiFiUDP.h> 
#include <WiFi.h>

enum WiFiMode {
    AP_MODE,
    STA_MODE
};

class WirelessTuning: public Stream {

public:

    WirelessTuning(uint16_t localPort = 4242);
    ~WirelessTuning();

    bool begin(const char* ssid, const char* password, WiFiMode mode = STA_MODE);

    virtual int available() override;
    virtual int read() override;
    virtual int peek() override;
    virtual void flush() override;

    size_t write(uint8_t byte) override;
    size_t write(const uint8_t *buffer, size_t size) override;

    // Enable print and println methods
    using Print::write;
private:
    WiFiServer _server;
    WiFiClient _client;
    IPAddress _remoteIP;
    uint16_t _remotePort;
    uint16_t _localPort;
    int _packetSize;
    uint8_t _packetBuffer[255];
    size_t _packetIndex;

    void acceptClient();
    void receivePacket();
    int handleClientDisconnect();
};

#endif