/* e-paper display lib */
#include <GxEPD.h>
//Use the GxGDEW029T5 class if you have Badgy Rev 2C. Make sure you are on GxEPD 3.05 or above
//#include <GxGDEW029T5/GxGDEW029T5.h>
#include <GxGDEH029A1/GxGDEH029A1.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
/* include any other fonts you want to use https://github.com/adafruit/Adafruit-GFX-Library */
#include <Fonts/FreeMonoBold9pt7b.h>
/* WiFi  libs*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

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
String quoteAPIURL = "http://quotesondesign.com/wp-json/posts?filter[orderby]=rand&filter[posts_per_page]=1";
bool OTA = false; //OTA mode disabled by default

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
  wifiManager.autoConnect("Badgy AP");

  /* If the center button is pressed on boot, enable OTA upload */
  if(digitalRead(5) == 0){
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    showIP();
  /* Otherwsie, display a quote and go to sleep for one hour */
  }else{
    showQuote();
    ESP.deepSleep(3600e6, WAKE_RF_DEFAULT);
  }
}

void loop()
{
  httpServer.handleClient();
   
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
              break;
            case 1:
              //do something when the user presses left
              break;
            case 2:
              //do something when the user presses center        
              break;
            case 3:
              //do something when the user presses right
              break;
            case 4:
              //do something when the user presses up
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

void showQuote()
{
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,10);
  /* Parse JSON from HTTP GET request */
  HTTPClient http;
  http.begin(quoteAPIURL);
  int httpCode = http.GET();
  if(httpCode > 0){
    String payload = http.getString();
    DynamicJsonBuffer jsonBuffer; //buffer for quote JSON
    JsonArray& root = jsonBuffer.parseArray(payload);
    String quote = root[0]["content"];
    http.end();
    display.println(quote);
    display.update();
  }
}

void showIP(){
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,10);

  String ip = WiFi.localIP().toString();
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
