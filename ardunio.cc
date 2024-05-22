#include <WiFi.h>
#include "ArduinoJson.h"


# NOTE: YOU NEED TO INSTALL ArduinoJson 7
# follow the instructions here (https://arduinojson.org/v7/how-to/install-arduinojson/)
# I assume you are using Arduino IDE

// Replace with your network credentials
const char* ssid = "BigRed";
const char* password = "PASSWORD"; // UPDATE ME
int count = 0;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void saveMessage(String message) {
    // Prepare JSON document
    DynamicJsonDocument doc(2048);
    doc["text"] = message;

    // Serialize JSON document
    String json;
    serializeJson(doc, json);

    WiFiClient client;  // or WiFiClientSecure for HTTPS
    HTTPClient http;

    // Send request
    http.begin(client, "http://score-sphere.duckdns.org/api/message");
    http.POST(json);

    // Read response
    Serial.print(http.getString());

    // Disconnect
    http.end();
}

void createGame() {
      // Prepare JSON document
    DynamicJsonDocument doc(2048);
    doc["away_team_name"] = "Away";
    doc["home_team_name"] = "Home";
    doc["period"] = 1;
    doc["status"] = "in-progress";
    doc["time_remaining"] = 900;

    // Serialize JSON document
    String json;
    serializeJson(doc, json);

    WiFiClient client;  // or WiFiClientSecure for HTTPS
    HTTPClient http;

    // Send request
    http.begin(client, "http://score-sphere.duckdns.org/api/game");
    http.POST(json);

    // Read response
    Serial.print(http.getString());

    // Disconnect
    http.end();
}

void updateGame() {
      // Prepare JSON document
    DynamicJsonDocument doc(2048);

    int gameId = 1;
    doc["away_team_score"] = 10;
    doc["home_team_score"] = 12;
    doc["period"] = 1;
    doc["time_remaining"] = 845;

    // Serialize JSON document
    String json;
    serializeJson(doc, json);

    WiFiClient client;  // or WiFiClientSecure for HTTPS
    HTTPClient http;

    // Send request
    http.begin(client, "http://score-sphere.duckdns.org/api/game/" + gameId);
    http.PATCH(json);

    // Read response
    Serial.print(http.getString());

    // Disconnect
    http.end();
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}

void loop() {
    String message = "Message " + count;
    saveMessage(message);
    count++;
    delay(5*1000)
}