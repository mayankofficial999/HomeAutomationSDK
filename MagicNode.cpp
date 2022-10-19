#include "MagicNode.h"

class MagicNode {
    private:
      String server_id,msg;
      String PARAM_MESSAGE="message";
      void forwardRequest() {
          WiFiClient client;
          HTTPClient http;
      
          Serial.print("[HTTP] begin...\n");
          if (http.begin(client, "http://"+this->server_id+"?response=True")) {      
      
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
      MagicNode(String server_id) {
        this->server_id=server_id;
        msg="_NULL_";
      }
      void init() {
        server_Node.on("/", HTTP_GET, [this] (AsyncWebServerRequest *request) {
          String message;
          if (request->hasParam(PARAM_MESSAGE)) {
              this->msg = request->getParam(PARAM_MESSAGE)->value();
          } else {
              message = "No message sent";
          }
          forwardRequest();
          request->send(200, "text/plain", "True");
          });
      }
};
