#ifndef MagicServer_h
#define MagicServer_h
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
#include <UUID.h>

class MagicServer {
  private:
    UUID uuid;
    String homeid,roomid,nodeid,message;
    uint8_t cipher_key[16],cipher_iv[16];
    String getAddress(String id);
    String encrypt(String plain_data);
    String decrypt(String encoded_data_str);
    int noOfHomes=0;
    //Max Homes = 10
    String homes[10];
    String ip_addr[10];
  public:
    String encryptedMessage;
    MagicServer(uint8_t* key);
    void process();
};
#endif
