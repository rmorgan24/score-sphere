#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
float currentTime = 3600.0;
float increment = 0.068;
boolean timerRun = false;
boolean homeInput = true;
boolean awayInput = false;
int gameChoice = 1;
int screenNum = 0;
int foulTeam = 0;
int playerNumberFoul = 0;
int homeFoulCounter = 0;
int awayFoulCounter = 0;
int cardTypeSelection = 0;
int playerNumberCard = 0;
int homeScore = 0;
int awayScore = 0;
int column[3] = {25, 33, 32};
int row[7] = {12, 13, 14, 15, 16, 17, 18};
int output[2];
int previousState[3][7];
int id = 1;
int shouldUpdate = 1;
String cardColor = "";
String team = "";
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Network and password currently being used
//Change this based on environment
const char* ssid = "BigRed";
const char* password = "riley7avery10"; 

void setup() {
  //Starts program
  Serial.begin(115200);

  //Initializes wifi connection and shows this in the serial terminal
  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  //Sets up buttons
  for (int i = 0; i < 7; i++) {
    pinMode(row[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 3; i++) {
    pinMode(column[i], OUTPUT);
    digitalWrite(column[i], HIGH);
  }
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 7; j++) {
      previousState[i][j] = HIGH;
    }
  }
  delay(100);

  //Starts up screen
  display.begin();
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  delay(1000);

  //Chooses what game is being played
  boolean gameLoop = true;
  while (gameLoop) {
    display.setCursor(0, 0);
    display.println("Choose Game:");
    int buttonNum = 0;
    for (int i = 0; i < 3; i++) {
      digitalWrite(column[i], LOW);
      for (int k = 0; k < 7; k++) {
        int currentState = digitalRead(row[k]);
        if (currentState != previousState[i][k]) {
          if (currentState == LOW) {
            output[0] = i + 1;
            output[1] = k + 1;
            buttonNum = arrayToInt(output);
          }
          previousState[i][k] = currentState;
        }
      }
      digitalWrite(column[i], HIGH);
      delay(10);
    }
    if(buttonNum == 10 && gameChoice < 4) {
      gameChoice++;
    }
    if(buttonNum == 8 && gameChoice > 1){
      gameChoice--;
    }
    if(gameChoice == 1){
      display.setCursor(0, 13);
      display.println("Soccer <--");
      display.println("Field Hockey");
      display.println("Lacrosse");
      display.println("Basketball");
    }
    if(gameChoice == 2){
      display.setCursor(0, 13);
      display.println("Soccer");
      display.println("Field Hockey <--");
      display.println("Lacrosse");
      display.println("Basketball");
    }
    if(gameChoice == 3){
      display.setCursor(0, 13);
      display.println("Soccer");
      display.println("Field Hockey");
      display.println("Lacrosse <--");
      display.println("Basketball");
    }
    if(gameChoice == 4){
      display.setCursor(0, 13);
      display.println("Soccer");
      display.println("Field Hockey");
      display.println("Lacrosse");
      display.println("Basketball <--");
    }
    display.display();
    delay(10);
    display.clearDisplay();
    //If you click ok, then game begins
    if (buttonNum == 9) {
      gameLoop = false;
    }  
  }

  //Takes in the starting time
  boolean inputLoop = true;
  int startingMin = 0;
  while (inputLoop) {
    display.setCursor(0, 0);
    display.println("Starting Minute:");
    int buttonNum = 0;
    for (int i = 0; i < 3; i++) {
      digitalWrite(column[i], LOW);
      for (int k = 0; k < 7; k++) {
        int currentState = digitalRead(row[k]);
        if (currentState != previousState[i][k]) {
          if (currentState == LOW) {
            output[0] = i + 1;
            output[1] = k + 1;
            buttonNum = arrayToInt(output);
          }
          previousState[i][k] = currentState;
        }
      }
      digitalWrite(column[i], HIGH);
      delay(10);
    }
    startingMin = updateNum(buttonNum, startingMin);
    if (buttonNum == 9) {
      inputLoop = false;
      currentTime = startingMin * 60;

      //Creates the game
      if (gameChoice == 1){
        id = createGame("soccer", int(currentTime));
      }
      if (gameChoice == 2){
       id = createGame("field-hockey", int(currentTime));
      }
      if (gameChoice == 3){
        id = createGame("lacrosse", int(currentTime));
      }
      if (gameChoice == 4){
        id = createGame("basketball", int(currentTime));
      }
    }
    display.setCursor(0, 10);
    display.println(startingMin);
    display.display();
    delay(10);
    display.clearDisplay();
  }
  
}


//resets
void(* resetFunc) (void) = 0;


void loop() {
  //Runs a soccer, field hockey, or lacrosse game
  if(gameChoice < 4){
    int buttonNum = 0;
    //Sees if/what button is pressed
    for (int i = 0; i < 3; i++) {
      digitalWrite(column[i], LOW);
      for (int k = 0; k < 7; k++) {
        int currentState = digitalRead(row[k]);
        if (currentState != previousState[i][k]) {
          if (currentState == LOW) {
            output[0] = i + 1;
            output[1] = k + 1;
            buttonNum = arrayToInt(output);
          }
          previousState[i][k] = currentState;
        }
      }
      digitalWrite(column[i], HIGH);
      delay(10);
    }
    
    String milPrint = "";
    String secPrint = "";
    String minPrint = "";
    String hrPrint = "";

    //updates current seconds, milliseconds, and minutes
    int currentTimeSec = int(currentTime) % 60;
    if (currentTimeSec < 10) {
      secPrint = ("0" + String(currentTimeSec));
    }
    else {
      secPrint = String(currentTimeSec);
    }
    int currentTimeMilliSec = int((currentTime - currentTimeSec) * 100) % 100;
    milPrint = String(currentTimeMilliSec);
    int currentTimeMin =  int(currentTime / 60);
    if (currentTimeMin < 10) {
      minPrint = ("0" + String(currentTimeMin));
    }
    else {
      minPrint = String(currentTimeMin);
    }
    //resets once game is over
    if (currentTimeMilliSec<=0 && currentTime<=0){
      updateGame(id, awayScore, homeScore, 1, int(currentTime), "ended");
      delay(20*1000);
      resetFunc();
    }

    //Does things based on button pressed
    if (timerRun) {
      currentTime -= increment;
    }
    if (buttonNum == 1) {
      homeScore++;
    }
    if (buttonNum == 3) {
      awayScore++;
    }
    if (buttonNum == 15) {
      if (screenNum != 1) {
        screenNum = 1;
      }
      else {
        screenNum = 0;
      }
    }
    if (buttonNum == 17) {
      timerRun = !timerRun;
    }

    //Prints everything on the screen based on what screen it is on
    if (screenNum == 0) {
      if (buttonNum == 6 && screenNum != 2) {
        screenNum = 2;
        buttonNum = 0;
      }
      if (buttonNum == 13 && screenNum != 3) {
        screenNum = 3;
        buttonNum = 0;
        homeInput = true;
        awayInput = false;
      } 
      display.setCursor(0, 0);
      display.println("Time Left");
      display.setCursor(0, 10);
      display.println(minPrint + ":" + secPrint + "." + milPrint);
      Serial.println(String(currentTime));
      display.setCursor(0, 23);
      display.println("HOME");
      display.setCursor(50, 23);
      display.println("AWAY");
      display.setCursor(0, 33);
      display.println(String(homeScore));
      display.setCursor(50, 33);
      display.println(String(awayScore));
      display.setCursor(0, 46);
      display.println("1 - Card Menu");
      display.setCursor(0, 56);
      display.println("2 - Score Adjust");
      display.display();
      delay(10);
      display.clearDisplay();
    }
    if (screenNum == 1) {
      display.setCursor(0, 0);
      display.println("Screen Off");
      display.display();
      delay(10);
      display.clearDisplay();
    }
    if (screenNum == 2){
      display.setCursor(0, 0);
      display.println("Card Type:");
      if(buttonNum == 10 && cardTypeSelection == 0){
        cardTypeSelection = 1;
      }
      if(buttonNum == 8 && cardTypeSelection == 1){
        cardTypeSelection = 0;
      }
      if(buttonNum == 9){
        createCard(id, "away", playerNumberCard, cardColor, 1, int(currentTime));
        screenNum = 0;
        playerNumberCard = 0;
      }
      if(cardTypeSelection == 0){
        display.setCursor(0, 10);
        display.println("Yellow Card <--");
        display.println("Red Card");
        cardColor = "yellow";
      }
      if(cardTypeSelection == 1){
        display.setCursor(0, 10);
        display.println("Yellow Card");
        display.println("Red Card <--");
        cardColor = "red";
      }
      display.setCursor(0, 35);
      display.println("Enter Player Number:");
      playerNumberCard = updateNum(buttonNum, playerNumberCard);
      display.setCursor(0, 45);
      display.println(playerNumberCard);
      display.display();
      delay(10);
      display.clearDisplay();
    }
    if (screenNum == 3){
      if(homeInput){
        display.setCursor(0, 0);
        display.println("Enter Home Score:");
        homeScore = updateNum(buttonNum, homeScore);
        display.println(homeScore);
        if(buttonNum == 9){
          homeInput = false;
          awayInput = true;
          buttonNum = 0;
        }
      }
      if(awayInput){
        display.setCursor(0, 0);
        display.println("Enter Away Score:");
        awayScore = updateNum(buttonNum, awayScore);
        display.println(awayScore);
        if(buttonNum == 9){
          awayInput = false;
          screenNum = 0;
        }
      }
      display.display();
      delay(10);
      display.clearDisplay();
    }
  }

  //Runs a basketball game
  if(gameChoice == 4){
    int buttonNum = 0;
    //Sees if/what button is pressed
    for (int i = 0; i < 3; i++) {
      digitalWrite(column[i], LOW);
      for (int k = 0; k < 7; k++) {
        int currentState = digitalRead(row[k]);
        if (currentState != previousState[i][k]) {
          if (currentState == LOW) {
            output[0] = i + 1;
            output[1] = k + 1;
            buttonNum = arrayToInt(output);
          }
          previousState[i][k] = currentState;
        }
      }
      digitalWrite(column[i], HIGH);
      delay(10);
    }
    
    String milPrint = "";
    String secPrint = "";
    String minPrint = "";
    String hrPrint = "";

    //updates current seconds, milliseconds, and minutes
    int currentTimeSec = int(currentTime) % 60;
    if (currentTimeSec < 10) {
      secPrint = ("0" + String(currentTimeSec));
    }
    else {
      secPrint = String(currentTimeSec);
    }
    int currentTimeMilliSec = int((currentTime - currentTimeSec) * 100) % 100;
    milPrint = String(currentTimeMilliSec);
    int currentTimeMin =  int(currentTime / 60);
    if (currentTimeMin < 10) {
      minPrint = ("0" + String(currentTimeMin));
    }
    else {
      minPrint = String(currentTimeMin);
    }
    //resets once game is over
    if (currentTimeMilliSec<=0 && currentTime<=0){
      updateGame(id, awayScore, homeScore, 1, int(currentTime), "ended");
      delay(20*1000);
      resetFunc();
    }

    //Does things based on button pressed
    if (timerRun) {
      currentTime -= increment;
    }
    if (buttonNum == 1) {
      homeScore++;
    }
    if (buttonNum == 3) {
      awayScore++;
    }
    if (buttonNum == 15) {
      if (screenNum != 1) {
        screenNum = 1;
      }
      else {
        screenNum = 0;
      }
    }
    if (buttonNum == 17) {
      timerRun = !timerRun;
    }

    //Prints everything on the screen based on what screen it is on
    if (screenNum == 0) {
      if (buttonNum == 6 && screenNum != 2) {
        screenNum = 2;
        buttonNum = 0;
      }
      if (buttonNum == 13 && screenNum != 3) {
        screenNum = 3;
        buttonNum = 0;
        homeInput = true;
        awayInput = false;
      }
      display.setCursor(0, 0);
      display.println("Time Left");
      display.setCursor(0, 10);
      display.println(minPrint + ":" + secPrint + "." + milPrint);
      Serial.println(String(currentTime));
      display.setCursor(90, 0);
      display.println("Fouls:");
      display.setCursor(90, 10);
      display.print("H - ");
      display.print(homeFoulCounter);
      display.setCursor(90, 20);
      display.print("A - ");
      display.print(awayFoulCounter);
      display.setCursor(0, 23);
      display.println("HOME");
      display.setCursor(50, 23);
      display.println("AWAY");
      display.setCursor(0, 33);
      display.print(String(homeScore));
      display.setCursor(18, 33);
      if(awayFoulCounter > 9){
        display.println(" B+");
      }
      else if(awayFoulCounter > 6){
        display.println(" B");
      }
      display.setCursor(50, 33);
      display.print(String(awayScore));
      display.setCursor(68, 33);
      if(homeFoulCounter > 9){
        display.println(" B+");
      }
      else if(homeFoulCounter > 6){
        display.println(" B");
      }
      display.setCursor(0, 46);
      display.println("1 - Foul Menu");
      display.setCursor(0, 56);
      display.println("2 - Score Adjust");
      display.display();
      delay(10);
      display.clearDisplay();
    }
    if (screenNum == 1) {
      display.setCursor(0, 0);
      display.println("Screen Off");
      display.display();
      delay(10);
      display.clearDisplay();
    }
    if (screenNum == 2){
      display.setCursor(0, 0);
      display.println("Foul Team:");
      if(buttonNum == 10 && foulTeam == 0){
        foulTeam = 1;
      }
      if(buttonNum == 8 && foulTeam == 1){
        foulTeam = 0;
      }
      if(buttonNum == 9){
        createCard(id, team, playerNumberFoul, "red", 1, int(currentTime));
        screenNum = 0;
        playerNumberFoul = 0;
        if(foulTeam == 0){
          homeFoulCounter++;
        }
        if(foulTeam == 1){
          awayFoulCounter++;
        }
      }
      if(foulTeam == 0){
        display.setCursor(0, 10);
        display.println("Home <--");
        display.println("Away");
        team = "home";
      }
      if(foulTeam == 1){
        display.setCursor(0, 10);
        display.println("Home");
        display.println("Away <--");
        team = "away";
      }
      display.setCursor(0, 35);
      display.println("Enter Player Number:");
      playerNumberFoul = updateNum(buttonNum, playerNumberFoul);
      display.setCursor(0, 45);
      display.println(playerNumberFoul);
      display.display();
      delay(10);
      display.clearDisplay();
    }
    if (screenNum == 3){
      if(homeInput){
        display.setCursor(0, 0);
        display.println("Enter Home Score:");
        homeScore = updateNum(buttonNum, homeScore);
        display.println(homeScore);
        if(buttonNum == 9){
          homeInput = false;
          awayInput = true;
          buttonNum = 0;
        }
      }
      if(awayInput){
        display.setCursor(0, 0);
        display.println("Enter Away Score:");
        awayScore = updateNum(buttonNum, awayScore);
        display.println(awayScore);
        if(buttonNum == 9){
          awayInput = false;
          screenNum = 0;
        }
      }
      display.display();
      delay(10);
      display.clearDisplay();
    }
  }

  //Updates the game every 100 times through this loop
  if (shouldUpdate < 100){
    shouldUpdate++;
  }
  else {
    updateGame(id, awayScore, homeScore, 1, int(currentTime), "in-progress");
    shouldUpdate = 1;
  }
}


int arrayToInt(int button[]) {
  return (button[0] - 1) * 7 + button[1];
}

int updateNum(int buttonNum, int inputMin) {
    if (buttonNum == 14) {
        inputMin *= 10;
        inputMin += 0;
    }
    if (buttonNum == 4) {
        inputMin *= 10;
        inputMin += 7;
    }
    if (buttonNum == 5) {
        inputMin *= 10;
        inputMin += 4;
    }
    if (buttonNum == 6) {
        inputMin *= 10;
        inputMin += 1;
    }
    if (buttonNum == 11) {
        inputMin *= 10;
        inputMin += 8;
    }
    if (buttonNum == 12) {
        inputMin *= 10;
        inputMin += 5;
    }
    if (buttonNum == 13) {
        inputMin *= 10;
        inputMin += 2;
    }
    if (buttonNum == 18) {
        inputMin *= 10;
        inputMin += 9;
    }
    if (buttonNum == 19) {
        inputMin *= 10;
        inputMin += 6;
    }
    if (buttonNum == 20) {
        inputMin *= 10;
        inputMin += 3;
    }
    if (buttonNum == 21) {
        inputMin /= 10;
    }
    return inputMin;
}

int numPadReturn(int buttonNum){
    int buttonReturn = 0;
    if (buttonNum == 14) {
        buttonReturn = 0;
    }
    if (buttonNum == 4) {
        buttonReturn = 7;
    }
    if (buttonNum == 5) {
        buttonReturn = 4;
    }
    if (buttonNum == 6) {
        buttonReturn = 1;
    }
    if (buttonNum == 11) {
        buttonReturn = 8;
    }
    if (buttonNum == 12) {
        buttonReturn = 5;
    }
    if (buttonNum == 13) {
        buttonReturn = 2;
    }
    if (buttonNum == 18) {
        buttonReturn = 9;
    }
    if (buttonNum == 19) {
        buttonReturn = 6;
    }
    if (buttonNum == 20) {
        buttonReturn = 3;
    }
    return buttonReturn;
}

//Starts up the wifi
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

//Can post a message to show that the code is running (will not show up on final website)
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

//Creates a new game taking in the sport and the time remaining (in seconds)
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

//Updates a game on the website uploading current time, score, etc.
int updateGame(int id, int away_score, int home_score, int period, int time_remaining, String gameStatus) {
    // Prepare JSON document
    DynamicJsonDocument doc(2048);

    doc["away_team_score"] = away_score;
    doc["home_team_score"] = home_score;
    doc["period"] = period;
    doc["time_remaining"] = time_remaining;
    doc["status"] = gameStatus;

    // Serialize JSON document
    String json;
    serializeJson(doc, json);

    WiFiClient client;  // or WiFiClientSecure for HTTPS
    HTTPClient http;

    // Send request
    String baseUrl = "http://score-sphere.duckdns.org:8888/api/game/";
    String info = "";
    info = baseUrl + id;
    http.begin(client, info);
    http.addHeader("Content-Type", "application/json");
    http.PATCH(json);

    DynamicJsonDocument res(2048);
    deserializeJson(res, http.getStream());

    // Disconnect
    http.end();
    return res["id"].as<int>();
}

//Adds a card to the website
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
