 /* e-paper display lib */
#include <GxEPD.h>
#include <GxGDEH029A1/GxGDEH029A1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
/* include any other fonts you want to use https://github.com/adafruit/Adafruit-GFX-Library */
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
/* WiFi  libs*/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <FS.h>

#include "buttons.h"

/* Always include the update server, or else you won't be able to do OTA updates! */
/**/const int port = 8888;
/**/ESP8266WebServer httpServer(port);
/**/ESP8266HTTPUpdateServer httpUpdater;
/*                                                                                */

/* Configure pins for display */
GxIO_Class io(SPI, SS, 0, 2);
GxEPD_Class display(io); // default selection of D4, D2

/* A single byte is used to store the button states for debouncing */
byte buttonState = 0;
byte lastButtonState = 0;   //the previous reading from the input pin
unsigned long lastDebounceTime = 0;  //the last time the output pin was toggled
unsigned long debounceDelay = 50;    //the debounce time

unsigned long updateInterval = 800;
unsigned long lastUpdate = 0;


/* Spotify API endpoints */
String apiHost = "api.spotify.com";
String authHost = "accounts.spotify.com";
String currentPlayerAPI = "/v1/me/player/";
String tokenAPI = "/api/token";
const int httpsPort = 443;
String MDNSName = "badgy-spotify";

/* Spotify app credentials, go to https://developer.spotify.com to setup */
String clientId = "YOUR_CLIENT_ID";
String clientSecret = "YOUR_CLIENT_SECRET";
String token = "";
bool isPlaying = false;

void setup(){
  if(!SPIFFS.begin()){
    showText("Formatting FS...");
    SPIFFS.format();
    showText("FS format complete!");
    SPIFFS.begin();
  }

  display.init();
  
  pinMode(1,INPUT_PULLUP); //down
  pinMode(3,INPUT_PULLUP); //left
  pinMode(5,INPUT_PULLUP); //center
  pinMode(12,INPUT_PULLUP); //right
  pinMode(10,INPUT_PULLUP); //up

  /* WiFi Manager automatically connects using the saved credentials, if that fails it will go into AP mode */
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("Badgy AP");

  /* Once connected to WiFi, startup the OTA update server if the center button is held on boot */
  if(digitalRead(5)  == 0){
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    showIP();
    while(1){
      httpServer.handleClient();
    }
  }

  getPlayback();
  delay(1000);
}

void loop(){
  byte reading =  (digitalRead(1)  == 0 ? 0 : (1<<0)) | //down
                  (digitalRead(3)  == 0 ? 0 : (1<<1)) | //left
                  (digitalRead(5)  == 0 ? 0 : (1<<2)) | //center
                  (digitalRead(12) == 0 ? 0 : (1<<3)) | //right
                  (digitalRead(10) == 0 ? 0 : (1<<4));  //up
                  
  if(reading != lastButtonState){
    lastDebounceTime = millis();
  }
  if((millis() - lastDebounceTime) > debounceDelay){
    if(reading != buttonState){
      buttonState = reading;
      for(int i=0; i<5; i++){
        if(bitRead(buttonState, i) == 0){
          switch(i){
            case 0:
              //do something when the user presses down
              showText("You pressed the down button!");
              break;
            case 1:
              //do something when the user presses left
              //showText("You pressed the left button!");
              controlPlayer(1);
              display.drawBitmap(prev, 73, 84, 32, 32, GxEPD_BLACK);
              break;
            case 2:
              //do something when the user presses center
              //showText("You pressed the center button!");
              controlPlayer(2);
              break;
            case 3:
              //do something when the user presses right
              //showText("You pressed the right button!");
              controlPlayer(0);
              display.drawBitmap(next, 193, 84, 32, 32, GxEPD_BLACK);
              break;
            case 4:
              //do something when the user presses up
              showText("You pressed the up button!");
              break;                              
            default:
              break;
          }
        }
      }
    }
  }
  lastButtonState = reading;

  if (millis() - lastUpdate > updateInterval){
    getPlayback();
    lastUpdate = millis();
  }
  
}

void configModeCallback (WiFiManager *myWiFiManager){
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,50);
  display.println("Connect to Badgy AP");
  display.println("to setup your WiFi!");
  display.update();  
}

void showIP(){
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,10);

  String url = WiFi.localIP().toString() + ":"+String(port)+"/update";
  byte charArraySize = url.length() + 1;
  char urlCharArray[charArraySize];
  url.toCharArray(urlCharArray, charArraySize);

  display.println("You are now connected!");
  display.println("");  
  display.println("Go to:");
  display.println(urlCharArray);
  display.println("to upload a new sketch.");
  display.update();  
}

void showText(char *text)
{
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(10,70);
  display.println(text);
  display.update();
}

String getToken(){
  
  /* Check if refresh token exists*/
  String refreshToken = "";
  showText("Loading config");
  File f = SPIFFS.open("/refresh.token", "r");
  if(!f){
    showText("Failed to open config file");
  }else{
    while(f.available()) {
        //Lets read line by line from the file
        refreshToken = f.readStringUntil('\r');
        //Serial.printf("Refresh Token: %s\n", refreshToken.c_str());
        f.close();
    }
  }

  String authCode = "";
  if(refreshToken == ""){
  /********************************************************************************/
  /* 1. Start MDNS service to provide callback URL for authorizing Spotify access */
  /********************************************************************************/
  if(!MDNS.begin(MDNSName.c_str())){
    //Serial.println("Error setting up MDNS responder!");
    while(1){ 
      delay(1000);
    }
  }

  /**************************************************************************************************************/
  /* 2. Create portal to authorize the Badgy Spotify Remote application with Spotify, get an authorization code */
  /**************************************************************************************************************/
  ESP8266WebServer server;

  /* Server route handlers */
  server.on("/", [&server](){
    server.sendHeader("Location", String("https://accounts.spotify.com/authorize/?client_id=" 
      + clientId
      + "&response_type=code&redirect_uri=" 
      + "http://" + MDNSName + ".local/callback/"
      + "&scope=user-read-playback-state%20user-modify-playback-state"), true);
    server.send(302, "text/plain", "");
  });
  server.on("/callback/",[&authCode,&server](){
    if(server.hasArg("code")){
      authCode = server.arg("code");
      server.send(200, "text/html", "Authorization Success!");
    }else{
      server.send(500, "text/plain", "Error Getting Authorization Code!");
    }
  });

  /* Start server */
  server.begin();
  MDNS.addService("http", "tcp", 80);

  /* Handle server requests until user has authorized */
  while(authCode == ""){
    server.handleClient();
    yield();
  }

  }//refreshToken == ""
  /**************************************************************************************************************/
  /* 3. Use authorization code from previous step to obtain an API token                                        */
  /**************************************************************************************************************/  
  
  /* Initiate HTTPS connection to Spotify's authentication endpoint  */
  WiFiClientSecure client;
  if(!client.connect(authHost.c_str(), httpsPort)){
    showText("Connection Failed!");
  }
  
  /* There are 2 grant types, authorization_code for a new token, and refresh_token if you're refreshing */
  String grantType    = (refreshToken == "") ? "authorization_code" : "refresh_token";
  String codeType    = (refreshToken == "") ? "code" : "refresh_token";
  authCode = (refreshToken == "") ? authCode : refreshToken;
  /* HTTPS request body */
  String requestBody  = "grant_type="       + grantType       +
                        "&"+ codeType +"="            + authCode        +
                        "&client_id="       + clientId        +
                        "&client_secret="   + clientSecret    +
                        "&redirect_uri="    + "http%3A%2F%2F" + MDNSName + ".local%2Fcallback%2F";
  /* HTTPS request string */                      
  String httpsString   = "POST "             + tokenAPI        + " "
                        "HTTP/1.1\r\n"      + "Host: "        + authHost.c_str()  + "\r\n"  +
                        "Content-Length: "  + String(requestBody.length())        + "\r\n"  + 
                        "Content-Type: application/x-www-form-urlencoded\r\n"     + 
                        "Connection: close\r\n\r\n" + requestBody;
  /* Send request */
  //Serial.println(httpsString);
  client.print(httpsString);

  /* Check HTTP status response */
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  //Serial.println(status);
  if(strcmp(status, "HTTP/1.1 200 OK") != 0){
    showText("HTTP Status Error!");
  }

  /* Find the end of headers */
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    showText("Invalid Response...");
  }

  /* Start parsing the JSON in the response body */
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(client);
  if(!root.success()){
    showText("JSON parsing failed!");
  }

  /* We have our tokens!  */
  String token = root["access_token"];
  String newRefreshToken = root["refresh_token"];

  /* Save the refresh token */
  if(newRefreshToken != ""){
    File s = SPIFFS.open("/refresh.token", "w+");
    if (!s) {
      showText("Failed to open config file");
    }
    s.println(newRefreshToken);
    s.close();
  }
  /**/
  //Serial.println(token);
  return token;
}

void controlPlayer(int control){

  String command = "";
  String method = "";
  if(control == 0){
    command = "next";
    method = "POST ";
  }
  else if(control == 1){
    command = "previous";
    method = "POST ";
  }
  else if(control == 2){
     command = isPlaying ? "pause" : "play";
     method = "PUT ";
  }else{
    return;
  }
  
  if(token == ""){
    token = getToken();
  }
  /* Construct request string and send */
  WiFiClientSecure client;
  if(!client.connect(apiHost.c_str(), httpsPort)){
    showText("Connection Failed!");
  }
    
  String request = "" + method + currentPlayerAPI + command + " HTTP/1.1\r\n" +
                   "Host: " + apiHost + "\r\n" +
                   "Authorization: Bearer " + token + "\r\n" +
                   "Content-Length: 0\r\n" + 
                   "Connection: close\r\n\r\n";
  client.print(request);

  /* Catch response errors */
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if(strcmp(status, "HTTP/1.1 200 OK") == 0){
    //showText("Spotify Player Active");
  }
  else if(strcmp(status, "HTTP/1.1 204 No Content") == 0){
    //showText("Spotify Player Inactive");
    return;
  }else{
    showText(status);
    return;
  }
  
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    showText("Invalid Response...");
    return;
  }

  /* Parse JSON response*/
  DynamicJsonBuffer jsonBuffer; //buffer for JSON response
  JsonObject& root = jsonBuffer.parseObject(client);
  if(!root.success()){
    showText("JSON parsing failed!");
    return;
  }
}

void getPlayback(){

  if(token == ""){
    token = getToken();
  }
  
  /* Construct request string and send */
  WiFiClientSecure client;
  if(!client.connect(apiHost.c_str(), httpsPort)){
    showText("Connection Failed!");
  }
    
  String request = "GET " + currentPlayerAPI + " HTTP/1.1\r\n" +
                   "Host: " + apiHost + "\r\n" +
                   "Authorization: Bearer " + token + "\r\n" +
                   "Connection: close\r\n\r\n";
  client.print(request);

  /* Catch response errors */
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if(strcmp(status, "HTTP/1.1 200 OK") == 0){
    //showText("Spotify Player Active");
  }
  else if(strcmp(status, "HTTP/1.1 204 No Content") == 0){
    showText("Spotify Player Inactive");
    return;
  }else{
    showText("Unexpected Response...");
    return;
  }
  
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    showText("Invalid Response...");
    return;
  }

  /* Parse JSON response*/
  DynamicJsonBuffer jsonBuffer; //buffer for JSON response
  JsonObject& root = jsonBuffer.parseObject(client);
  if(!root.success()){
    showText("JSON parsing failed!");
    return;
  }

  /* Print current track details on display */
  String currentTrack = root["item"]["name"];
  String currentAlbum = root["item"]["album"]["name"];
  String currentArtist = root["item"]["artists"][0]["name"];
  isPlaying = root["is_playing"] == "true";
  

  /* Print current track details on display */
  long currentProgress = root["progress_ms"];
  long currentDuration = root["item"]["duration_ms"];
  int minutes = (currentProgress/1000) / 60;
  int seconds = (currentProgress/1000) % 60;
  int location = ((currentProgress * (296 - 20)) / currentDuration) + 10;
  
 // Serial.println(currentTrack);
 // Serial.println(currentAlbum);
 // Serial.println(currentArtist);

  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold12pt7b;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,20);
  display.println(currentTrack);
  const GFXfont* g = &FreeMonoBold9pt7b;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(g);
  display.print(currentAlbum);
  display.print(" - ");
  display.println(currentArtist);

  //display.setCursor(10,60);
  // display.print(minutes);
  //display.print(":");
  //display.print(seconds);
  display.fillRect(10, 70, (296 - 20), 5, GxEPD_BLACK);
  display.fillCircle(location, 72, 5, GxEPD_BLACK);
  //display.update();
  if(isPlaying){
    display.drawBitmap(play, 133, 84, 30, 30, GxEPD_WHITE);
  }else{
    display.drawBitmap(pause, 133, 84, 30, 30, GxEPD_WHITE);
  }
  display.drawBitmap(prev, 73, 84, 32, 32, GxEPD_WHITE);
  display.drawBitmap(next, 193, 84, 32, 32, GxEPD_WHITE);
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false); //last param is using rotation
}
