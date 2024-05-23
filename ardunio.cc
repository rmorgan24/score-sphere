#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"


// NOTE: YOU NEED TO INSTALL ArduinoJson 7
// follow the instructions here (https://arduinojson.org/v7/how-to/install-arduinojson/)
// I assume you are using Arduino IDE

// Replace with your network credentials
const char* ssid = "BigRed";
const char* password = "PASSWORD"; // UPDATE ME

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(WiFi.status());
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

int saveMessage(String message) {
    // Prepare JSON document
    DynamicJsonDocument doc(2048);
    doc["text"] = message;

    // Serialize JSON document
    String json;
    serializeJson(doc, json);

    WiFiClient client;  // or WiFiClientSecure for HTTPS
    HTTPClient http;

    // Send request
    http.begin(client, "http://score-sphere.duckdns.org:8888/api/message");
    http.addHeader("Content-Type", "application/json");
    http.POST(json);

    DynamicJsonDocument res(2048);
    deserializeJson(res, http.getStream());

    // Disconnect
    http.end();
    return res["id"].as<int>();
}

int createGame(String sport, int time_remaining) {
    // Prepare JSON document
    DynamicJsonDocument doc(2048);
    doc["away_team_name"] = "Away";
    doc["home_team_name"] = "Home";
    doc["sport"] = sport;
    doc["period"] = 1;
    doc["status"] = "in-progress";
    doc["time_remaining"] = time_remaining;

    // Serialize JSON document
    String json;
    serializeJson(doc, json);

    WiFiClient client;  // or WiFiClientSecure for HTTPS
    HTTPClient http;

    // Send request
    http.begin(client, "http://score-sphere.duckdns.org:8888/api/game");
    http.addHeader("Content-Type", "application/json");
    http.POST(json);

    DynamicJsonDocument res(2048);
    deserializeJson(res, http.getStream());

    // Disconnect
    http.end();
    return res["id"].as<int>();
}

int updateGame(int id, int away_score, int home_score, int period, int time_remaining) {
    // Prepare JSON document
    DynamicJsonDocument doc(2048);

    doc["away_team_score"] = away_score;
    doc["home_team_score"] = home_score;
    doc["period"] = period;
    doc["time_remaining"] = time_remaining;

    // Serialize JSON document
    String json;
    serializeJson(doc, json);

    WiFiClient client;  // or WiFiClientSecure for HTTPS
    HTTPClient http;

    // Send request
    String baseUrl = "http://score-sphere.duckdns.org:8888/api/game/";
    http.begin(client, baseUrl + id);
    http.addHeader("Content-Type", "application/json");
    http.PATCH(json);

    DynamicJsonDocument res(2048);
    deserializeJson(res, http.getStream());

    // Disconnect
    http.end();
    return res["id"].as<int>();
}

void createCard(int id, String team, int player_number, String card_color, int period, int time_remaining) {
    // Prepare JSON document
    DynamicJsonDocument doc(2048);

    doc["team"] = team;
    doc["player_number"] = player_number;
    doc["card_color"] = card_color;
    doc["period"] = period;
    doc["time_remaining"] = time_remaining;

    // Serialize JSON document
    String json;
    serializeJson(doc, json);

    WiFiClient client;  // or WiFiClientSecure for HTTPS
    HTTPClient http;

    // Send request
    String baseUrl = "http://score-sphere.duckdns.org:8888/api/game/";
    http.begin(client, baseUrl + id + "/card");
    http.addHeader("Content-Type", "application/json");
    http.PUT(json);

    DynamicJsonDocument res(2048);
    deserializeJson(res, http.getStream());

    // Disconnect
    http.end();
}

void setup() {
  Serial.begin(115200);

  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
  int messageId = saveMessage("Hello Riley!");
  Serial.print("Message Sent ");
  Serial.println(messageId);

  int gameId = createGame("lacrosse", 15*60);
  Serial.print("Game Created ");
  Serial.println(gameId);
  updateGame(gameId, 10, 5, 1, 12*60);
  Serial.println("Game Updated");
  createCard(gameId, "home", 34, "red", 1, 11*60);
  Serial.println("Card Created");
}

void loop() {
} 