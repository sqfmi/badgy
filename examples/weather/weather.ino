/* e-paper display lib */
#include <GxEPD.h>
#include <GxGDEH029A1/GxGDEH029A1.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
/* include any other fonts you want to use https://github.com/adafruit/Adafruit-GFX-Library */
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include "icons.h"
/* WiFi  libs*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
/* Util libs */
#include <Time.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

const char* host = "api.openweathermap.org";
const char* API_KEY = "YOUR_API_KEY"; //Your API key https://home.openweathermap.org/
const char* UNITS = "metric"; //Metric or Imperial
const char* CITY_ID = "5128581"; //City ID https://openweathermap.org/find
const int time_zone = -5; // e.g. UTC−05:00 = -5

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

void setup()
{  
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

  if(digitalRead(5)  == 0){
    /* Once connected to WiFi, startup the OTA update server if the center button is held on boot */
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    showIP();
    while(1){
      httpServer.handleClient();
    }
  }
  getWeatherData(); //current weather
  getForecastData(); //forecast for the next 3 days
  ESP.deepSleep(3600e6, WAKE_RF_DEFAULT); //set sleep time 3600e6 = hourly
}

void loop()
{  
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
                showText("You pressed the left button!");
                break;
              case 2:
                //do something when the user presses center
                showText("You pressed the center button!");
                break;
              case 3:
                //do something when the user presses right
                showText("You pressed the right button!");
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

void getWeatherData()
{
  String type= "weather";
  String url = "/data/2.5/"+type+"?id="+CITY_ID+"&units="+UNITS+"&appid="+API_KEY;

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    showText("connection failed");
    return;
  }
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      showText(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read response
  String city;
  float current_temp;
  int humidity;
  float temp_min;
  float temp_max;
  float wind;
  String icon_code;
  int condition;
  int time;
  
  while(client.available()){
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
    city = root["name"].as<String>();
    current_temp = root["main"]["temp"];
    humidity = root["main"]["humidity"];
    temp_min = root["main"]["temp_min"];
    temp_max = root["main"]["temp_max"];
    wind = root["wind"]["speed"];
    icon_code = root["weather"][0]["icon"].as<String>();
    condition = root["weather"][0]["id"];
    time = root["dt"]; //time data was collected
  }
  
  time = time + (time_zone*60*60); //calculate time based on time zone
  setTime(time);
  /* Get icon for weather condition */
  const unsigned char *icon;
  icon = getIcon(condition, icon_code, false);
  /* Display weather conditions */
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(icon, -5, 5, 80, 80, GxEPD_WHITE);
  const GFXfont* small = &FreeMonoBold9pt7b;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(small);
  //display.setCursor(10,11);
  //display.println(city);
  //Current Date (actually date of last update)
  display.setCursor(120,11);
  display.print(dayShortStr(weekday()));
  display.print(" ");
  display.print(monthShortStr(month()));
  display.print(" ");
  display.print(day());
  display.print(" ");
  display.print(year());
  //Current Wind
  display.drawBitmap(strong_wind_small, 0, 62, 48, 48, GxEPD_WHITE);
  display.setCursor(50,92);
  display.print(String((int)(wind*3.6))+"km/h");
  //Current Humidity
  display.drawBitmap(humidity_small, 0, 97, 32, 32, GxEPD_WHITE);
  display.setCursor(50,119);
  display.print(String(humidity)+"%");    
  //Current Temp
  display.setCursor(72,55);
  const GFXfont* big = &FreeMonoBold18pt7b;
  display.setFont(big);    
  display.println(String((int)current_temp) + "C");
  display.update();
}

void getForecastData()
{
  String type= "forecast";
  String url = "/data/2.5/"+type+"?id="+CITY_ID+"&units="+UNITS+"&appid="+API_KEY;

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    showText("connection failed");
    return;
  }
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      showText(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read response
  while(client.available()){
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

    /* Forecast returns weather data every 3 hours for the next 5 days, we only want the next 3 days for every 24 hours*/
    for(int i=1; i<=3; i++){
      int condition= root["list"][char(i*8)]["weather"][0]["id"];
      int temp = root["list"][char(i*8)]["main"]["temp"];
      String icon_code= root["list"][char(i*8)]["weather"][0]["icon"];
    
    int offset = 100; //x offset for forecast block
        
    const unsigned char *icon;
    icon = getIcon(condition, icon_code, true);
    display.drawBitmap(icon, (offset+(i*48)), 58, 48, 48, GxEPD_WHITE); //(icon, pos_x, pos_y, size_x, size_y, bg)
    int time = root["list"][char(i*8)]["dt"];
    time = time + (time_zone*60*60);
    setTime(time);
    //Day of Week
    const GFXfont* small = &FreeMonoBold9pt7b;
    display.setTextColor(GxEPD_BLACK);
    display.setFont(small);
    display.setCursor((offset+7+(i*48)),58);
    display.print(dayShortStr(weekday()));
    //Forecasted temperature
    display.setCursor((offset+10+(i*48)),115);
    display.print(String(temp));    
    }
    display.update();
  }
}

const unsigned char * getIcon(int condition, String icon_code, bool small){
    if(condition <= 232){
      return small ? thunderstorm_small : thunderstorm;
    }else if(condition >= 300 && condition <= 321){
      return small ? showers_small : showers;
    }else if(condition >= 500 && condition <= 531){
      return small ? rain_small : rain;
    }else if(condition >= 600 && condition <= 602){
      return small ? snow_small : snow;
    }else if(condition >= 611 && condition <= 612){
      return small ? sleet_small : sleet;
    }else if(condition >= 615 && condition <= 622){
      return small ? rain_mix_small : rain_mix;
    }else if(condition == 701 || condition == 721 || condition == 741){
      return small ? fog_small : fog;
    }else if(condition == 711){
      return small ? smoke_small : smoke;
    }else if(condition == 731){
      return small ? sandstorm_small : sandstorm;
    }else if(condition == 751 || condition == 761){
      return small ? dust_small : dust;
    }else if(condition == 762){
      return small ? volcano_small : volcano;
    }else if(condition == 771){
      return small ? strong_wind_small : strong_wind;
    }else if(condition == 781){
      return small ? tornado : tornado_small;
    }else if(condition == 800){
      if(icon_code == "01d"){
        return small ? day_sunny_small : day_sunny;
      }else{
        return small ? night_clear_small : night_clear;
      }
    }else if(condition == 801 || condition == 802){
      if(icon_code == "02d" || icon_code == "03d"){
        return small ? day_cloudy_small : day_cloudy;
      }else{
        return small ? night_cloudy_small : night_cloudy;
      }
    }else if(condition == 803 || condition == 804){
      if(icon_code == "04d"){
        return small ? cloudy_small : cloudy;
      }else{
        return small ? night_cloudy_small : night_cloudy;
      }
    }
}
