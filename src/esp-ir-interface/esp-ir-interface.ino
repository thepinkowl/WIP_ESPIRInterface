/**
 * An IR detector/demodulator must be connected to the input kRecvPin.
 * Example circuit diagram: https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-receiving
 * 
 * This code is based on multiple sources and examples:
 *  > ConfigManager
 *  MIT License
 *  Copyright 2018 Nicholas Wiersma
 *  [https://github.com/nrwiersma/ConfigManager/tree/master/examples/save_config_demo]
 *  > IRremoteESP8266
 *  GNU GPL v2.1 1999 License
 *  Copyright 2009 Ken Shirriff, http://arcfn.com
 *  Copyright 2017-2019 David Conran
 *  [https://github.com/crankyoldgit/IRremoteESP8266/blob/master/examples/IRrecvDumpV2/IRrecvDumpV2.ino]
 */
#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>
#include <ConfigManager.h>
#include <WiFi.h>

// IRIS: InfraRed Interface Service
#define MYNAME "IRIS"
// An IR detector/demodulator is connected to GPIO pin 14
#define kRecvPin 14
// (seconds/100) => 30s
#define TIMEOUT 3000

const char mimeHTML[] PROGMEM = "text/html";
const char *configHTMLFile = "/settings.html";

struct Config {
  char name[255];
  bool enabled;
  char mqtt_host[255];
  char mqtt_port[10];
  char mqtt_username[255];
  char mqtt_password[255];
  char mqtt_topic[255];
} config;

struct Metadata {
  int8_t version;
} meta;

ConfigManager configManager;

const uint8_t kTimeout = 50;
const uint16_t kCaptureBufferSize = 1024;
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;

void createCustomRoute(WebServer *server) {
  server->on("/settings/edit", HTTPMethod::HTTP_GET, [server](){
    SPIFFS.begin();

    File f = SPIFFS.open(configHTMLFile, "r");
    if (!f) {
      Serial.println(F("file open failed"));
      server->send(404, FPSTR(mimeHTML), F("File not found"));
      return;
    }

    server->streamFile(f, FPSTR(mimeHTML));

    f.close();
  });
  server->on("/ir", HTTPMethod::HTTP_GET, [server](){
    Serial.println("Getting IR signal...");
    irrecv.enableIRIn();
    int i = 0;
    while (!irrecv.decode(&results) && ++i < TIMEOUT) {
      delay(10);
    }
    if (i == TIMEOUT) {
      Serial.println("TIMEOUT");
      server->send(504); // HTTP timeout
      return;
    }
    if (results.overflow) {
      Serial.println("OVERFLOW");
      server->send(507); // Insufficient storage
      return;
    }
    Serial.println("Got signal");
    server->send(200, "text/plain", resultToHumanReadableBasic(&results));
  });
  server->on("/ir", HTTPMethod::HTTP_POST, [server](){
    // TODO
  });
}

void setupConfigManager() {
  meta.version = 3;
  
  String miniMacAddressSufix = String("-" + WiFi.macAddress().substring(8));
  miniMacAddressSufix.replace(":", "");
  String APNAME = String(MYNAME + miniMacAddressSufix);
  configManager.setAPName(APNAME.c_str());
  
  configManager.setAPFilename("/index.html");
  configManager.addParameter("name", config.name, 255);

  configManager.addParameter("mqtt_host", config.mqtt_host, 255);
  configManager.addParameter("mqtt_port", config.mqtt_port, 10);
  configManager.addParameter("mqtt_username", config.mqtt_username, 255);
  configManager.addParameter("mqtt_password", config.mqtt_password, 255, set);
  configManager.addParameter("mqtt_topic", config.mqtt_topic, 255);

  configManager.addParameter("enabled", &config.enabled);
  configManager.addParameter("version", &meta.version, get);

  configManager.setAPICallback(createCustomRoute);
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
  setupConfigManager();
  configManager.begin(config);
}

void loop() {
  configManager.loop();
}
