#include "MagicHome.h"

class MagicHome {
    private:
      String msg;
      String PARAM_MESSAGE[3]={"room_id","node_id","message"};
      void forwardRequest() {
          WiFiClient client;
          HTTPClient http;
      
          Serial.print("[HTTP] begin...\n");
          if (http.begin(client, "http://"+this->node_id+"?message="+this->msg)) {      
      
            Serial.print("[HTTP] GET...\n");
            // start connection and send HTTP header
            int httpCode = http.GET();
      
            // httpCode will be negative on error
            if (httpCode > 0) {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      
              // file found at server
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                String payload = http.getString();
                Serial.println(payload);
              }
            } else {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }
      
            http.end();
          } else {
              Serial.printf("[HTTP} Unable to connect\n");
          }
      }
    public:
      String node_id,room_id;
      MagicHome() {
        msg="_NULL_";
        node_id="";
        room_id="";
      }
      void init() {
        server_Home.on("/updateNode", HTTP_GET, [this] (AsyncWebServerRequest *request) {
          String message;
          if (request->hasParam(PARAM_MESSAGE[0])) {
              this->room_id = request->getParam(PARAM_MESSAGE[0])->value();
          } else {
              message = "No node id sent";
          }
          if (request->hasParam(PARAM_MESSAGE[1])) {
              this->node_id = request->getParam(PARAM_MESSAGE[1])->value();
          } else {
              message = "No node id sent";
          }
          if (request->hasParam(PARAM_MESSAGE[2])) {
              this->msg = request->getParam(PARAM_MESSAGE[2])->value();
          } else {
              message = "No message sent";
          }
          forwardRequest();
          request->send(200, "text/plain", "True");
          });
      }
};
