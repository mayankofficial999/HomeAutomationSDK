#ifndef MagicNode_h
#define MagicNode_h
#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

class MagicNode {
  private:
    String nodeid,message;
    uint8_t cipher_key[16],cipher_iv[16];
    String encrypt(String plain_data);
    String decrypt(String encoded_data_str);
  public:
    String actionMessage;
    MagicNode(uint8_t* key);
    void registerNode(String serverAddr);
};
#endif
