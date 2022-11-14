#include "MagicNode.h"
#include "Base64.h"

WiFiClient mainNode;
AsyncWebServer magicNode(80);

MagicNode::MagicNode(uint8_t* key) {
    memcpy(key, cipher_key, 16);
    memcpy(key, cipher_iv, 16);
    // Send a POST request to <IP>/post with a form field message set to <message>
    magicNode.on("/control", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("message", true)) {
            message = request->getParam("message", true)->value();
        } else {
            message = "No message sent";
        }
        actionMessage = decrypt(message);
        Serial.printf("Message: %s\n",actionMessage);
        request->send(200, "text/plain", "Hello, POST: " + message);
    });
    
    magicNode.begin();
}

String MagicNode::encrypt(String plain_data){
  int i;
  // PKCS#7 Padding (Encryption), Block Size : 16
  int len = plain_data.length();
  int n_blocks = len / 16 + 1;
  uint8_t n_padding = n_blocks * 16 - len;
  uint8_t data[n_blocks*16];
  memcpy(data, plain_data.c_str(), len);
  for(i = len; i < n_blocks * 16; i++){
    data[i] = n_padding;
  }
  
  uint8_t key[16], iv[16];
  memcpy(key, cipher_key, 16);
  memcpy(iv, cipher_iv, 16);

  // encryption context
  br_aes_big_cbcenc_keys encCtx;

  // reset the encryption context and encrypt the data
  br_aes_big_cbcenc_init(&encCtx, key, 16);
  br_aes_big_cbcenc_run( &encCtx, iv, data, n_blocks*16 );

  // Base64 encode
  len = n_blocks*16;
  char encoded_data[ base64_enc_len(len) ];
  base64_encode(encoded_data, (char *)data, len);
  
  return String(encoded_data);
}

String MagicNode::decrypt(String encoded_data_str){  
  int input_len = encoded_data_str.length();
  char *encoded_data = const_cast<char*>(encoded_data_str.c_str());
  int len = base64_dec_len(encoded_data, input_len);
  uint8_t data[ len ];
  base64_decode((char *)data, encoded_data, input_len);
  
  uint8_t key[16], iv[16];
  memcpy(key, cipher_key, 16);
  memcpy(iv, cipher_iv, 16);

  int n_blocks = len / 16;

  br_aes_big_cbcdec_keys decCtx;

  br_aes_big_cbcdec_init(&decCtx, key, 16);
  br_aes_big_cbcdec_run( &decCtx, iv, data, n_blocks*16 );

  // PKCS#7 Padding (Decryption)
  uint8_t n_padding = data[n_blocks*16-1];
  len = n_blocks*16 - n_padding;
  char plain_data[len + 1];
  memcpy(plain_data, data, len);
  plain_data[len] = '\0';

  return String(plain_data);
}

void MagicNode::registerNode(String serverAddr) {
    HTTPClient magicHttp;
    magicHttp.begin(mainNode,"http://"+serverAddr+"/reg");
    magicHttp.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = magicHttp.POST("ip_address="+WiFi.localIP().toString());
    // httpCode will be negative on error
    if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK) {
            const String& payload = magicHttp.getString();
            nodeid = payload;
        }
    } 
    else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", magicHttp.errorToString(httpCode).c_str());
    }
    mainNode.stop();
}
